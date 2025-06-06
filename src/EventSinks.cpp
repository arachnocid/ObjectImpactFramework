#include "EventSinks.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

using namespace std::chrono;

namespace OIF
{
    // ------------------ Enums ------------------
    enum class WeaponType {
        HandToHand,
        OneHandSword,
        Dagger,
        OneHandAxe,
        OneHandMace,
        TwoHandSword,
        TwoHandAxe,
        Ranged,
        Staff,
        Spell,
        Shout,
        Ability,
        LesserPower,
        Power,
        Total,
        Other
    };

    enum class AttackType {
        Regular,
        Power,
        Bash,
        Projectile,
        Charge,
        Rotating,
        Continuous
    }; 

    // ------------------ Static Variables ------------------
    static auto startTime = std::chrono::steady_clock::now();

    // ------------------ Helpers ------------------
    WeaponType GetWeaponType(RE::TESObjectWEAP* weapon) {
        WeaponType weaponType = WeaponType::Other;
        switch (weapon->GetWeaponType()) {
            case RE::WEAPON_TYPE::kHandToHandMelee:  weaponType = WeaponType::HandToHand;   break;
            case RE::WEAPON_TYPE::kOneHandSword:     weaponType = WeaponType::OneHandSword; break;
            case RE::WEAPON_TYPE::kOneHandDagger:    weaponType = WeaponType::Dagger;       break;
            case RE::WEAPON_TYPE::kOneHandAxe:       weaponType = WeaponType::OneHandAxe;   break;
            case RE::WEAPON_TYPE::kOneHandMace:      weaponType = WeaponType::OneHandMace;  break;
            case RE::WEAPON_TYPE::kTwoHandSword:     weaponType = WeaponType::TwoHandSword; break;
            case RE::WEAPON_TYPE::kTwoHandAxe:       weaponType = WeaponType::TwoHandAxe;   break;
            case RE::WEAPON_TYPE::kBow:
            case RE::WEAPON_TYPE::kCrossbow:         weaponType = WeaponType::Ranged;       break;
            case RE::WEAPON_TYPE::kStaff:            weaponType = WeaponType::Staff;        break;
            case RE::WEAPON_TYPE::kTotal:            weaponType = WeaponType::Total;        break;
            default:                                 weaponType = WeaponType::Other;        return WeaponType::Other;
        }
        return weaponType;
    }

    AttackType GetAttackType(RE::BGSAttackData* attackData, RE::BGSProjectile* projectile) {
        if (projectile) return AttackType::Projectile;
        if (attackData) {
            if (attackData->data.flags.any(RE::AttackData::AttackFlag::kPowerAttack)) return AttackType::Power;
            if (attackData->data.flags.any(RE::AttackData::AttackFlag::kBashAttack)) return AttackType::Bash;
            if (attackData->data.flags.any(RE::AttackData::AttackFlag::kChargeAttack)) return AttackType::Charge;
            if (attackData->data.flags.any(RE::AttackData::AttackFlag::kRotatingAttack)) return AttackType::Rotating;
            if (attackData->data.flags.any(RE::AttackData::AttackFlag::kContinuousAttack)) return AttackType::Continuous;
        }
        return AttackType::Regular;
    }

    // ------------------ Sinks ------------------
    RE::BSEventNotifyControl ActivateSink::ProcessEvent(const RE::TESActivateEvent* evn, RE::BSTEventSource<RE::TESActivateEvent>*)
    {
        if (!evn || !evn->objectActivated || !evn->objectActivated.get()) return RE::BSEventNotifyControl::kContinue;

        auto* targetRef = evn->objectActivated.get();
        if (!targetRef || targetRef->IsDeleted() || !targetRef->GetFormID()) return RE::BSEventNotifyControl::kContinue;

        auto* baseObj = targetRef->GetBaseObject();
        if (!baseObj) return RE::BSEventNotifyControl::kContinue;
        
        RE::Actor* source = nullptr;
        if (evn->actionRef && evn->actionRef.get()) source = evn->actionRef.get()->As<RE::Actor>();

        if (!source || source->IsDeleted() || source->IsDead()) return RE::BSEventNotifyControl::kContinue;

        SKSE::GetTaskInterface()->AddTask([source, targetRef, baseObj]() {
            if (source && !source->IsDeleted() && targetRef && !targetRef->IsDeleted() && baseObj) {
                RuleContext ctx{ 
                    EventType::kActivate, 
                    source, 
                    targetRef, 
                    baseObj
                };
                RuleManager::GetSingleton()->Trigger(ctx);
            }
        });

        return RE::BSEventNotifyControl::kContinue;
    }

    RE::BSEventNotifyControl HitSink::ProcessEvent(const RE::TESHitEvent* evn, RE::BSTEventSource<RE::TESHitEvent>*)
    {
        if (!evn || !evn->target || !evn->target.get()) return RE::BSEventNotifyControl::kContinue;

        auto* targetRef = evn->target.get();
        if (!targetRef || targetRef->IsDeleted() || !targetRef->GetFormID()) return RE::BSEventNotifyControl::kContinue;

        auto* baseObj = targetRef->GetBaseObject();
        if (!baseObj) return RE::BSEventNotifyControl::kContinue;
        
        RE::Actor* source = nullptr;
        if (evn->cause && evn->cause.get()) source = evn->cause.get()->As<RE::Actor>();

        if (!source || source->IsDeleted() || source->IsDead())  return RE::BSEventNotifyControl::kContinue;

        RE::TESForm* hitSourceForm = evn->source ? RE::TESForm::LookupByID(evn->source) : nullptr;

        SKSE::GetTaskInterface()->AddTask([evn, source, targetRef, baseObj, hitSourceForm]() {
            if (evn && source && !source->IsDeleted() && targetRef && !targetRef->IsDeleted() && baseObj) {
                
                RE::TESForm* attackSource = nullptr;
                RE::BGSProjectile* projectile = nullptr;
                WeaponType weaponType = WeaponType::Other;
                AttackType attackType = AttackType::Regular;

                if (!hitSourceForm) weaponType = WeaponType::Other;

                RE::HighProcessData* highData = nullptr;
                if (auto* actorState = source->GetActorRuntimeData().currentProcess) {
                    if (actorState->high) {
                        highData = actorState->high;
                        if (highData->attackData) {
                            attackType = GetAttackType(highData->attackData.get(), projectile);
                        }
                    }
                }

                if (source) {
                    RE::TESForm* rightObj = source->GetEquippedObject(false);
                    RE::TESForm* leftObj  = source->GetEquippedObject(true);

                    bool leftHandAttack  = hitSourceForm && leftObj  && (hitSourceForm == leftObj);
                    bool rightHandAttack = hitSourceForm && rightObj && (hitSourceForm == rightObj);

                    if (leftHandAttack) {
                        attackSource = leftObj;
                    } else if (rightHandAttack) {
                        attackSource = rightObj;
                    }

                    if (attackSource) {
                        if (auto* weapon = attackSource->As<RE::TESObjectWEAP>()) {
                            weaponType = GetWeaponType(weapon);
                        } else if (auto* spell = hitSourceForm->As<RE::SpellItem>()) {
                            switch (spell->GetSpellType()) {
                                case RE::MagicSystem::SpellType::kSpell:        weaponType = WeaponType::Spell;        break;
                                case RE::MagicSystem::SpellType::kVoicePower:   weaponType = WeaponType::Shout;        break;
                                case RE::MagicSystem::SpellType::kAbility:      weaponType = WeaponType::Ability;      break;
                                case RE::MagicSystem::SpellType::kLesserPower:  weaponType = WeaponType::LesserPower;  break;
                                case RE::MagicSystem::SpellType::kPower:        weaponType = WeaponType::Power;        break;
                                default:                                        weaponType = WeaponType::Other;        break;
                            }
                        }
                    } else {
                        for (auto cs : {
                            RE::MagicSystem::CastingSource::kLeftHand,
                            RE::MagicSystem::CastingSource::kRightHand,
                            RE::MagicSystem::CastingSource::kOther,
                            RE::MagicSystem::CastingSource::kInstant })
                        {
                            if (auto* mc = source->GetMagicCaster(cs)) {
                                if (auto* spell = mc->currentSpell) {
                                    switch (spell->GetSpellType()) {
                                        case RE::MagicSystem::SpellType::kSpell:        weaponType = WeaponType::Spell;        break;
                                        case RE::MagicSystem::SpellType::kVoicePower:   weaponType = WeaponType::Shout;        break;
                                        case RE::MagicSystem::SpellType::kAbility:      weaponType = WeaponType::Ability;      break;
                                        case RE::MagicSystem::SpellType::kLesserPower:  weaponType = WeaponType::LesserPower;  break;
                                        case RE::MagicSystem::SpellType::kPower:        weaponType = WeaponType::Power;        break;
                                        default:                                        weaponType = WeaponType::Other;        break;
                                    }
                                }
                            }
                        }
                    }
                }

                std::string weaponTypeStr;
                switch (weaponType) {
                    case WeaponType::HandToHand:    weaponTypeStr = "HandToHand";       break;
                    case WeaponType::OneHandSword:  weaponTypeStr = "OneHandSword";     break;
                    case WeaponType::Dagger:        weaponTypeStr = "Dagger";           break;
                    case WeaponType::OneHandAxe:    weaponTypeStr = "OneHandAxe";       break;
                    case WeaponType::OneHandMace:   weaponTypeStr = "OneHandMace";      break;
                    case WeaponType::TwoHandSword:  weaponTypeStr = "TwoHandSword";     break;
                    case WeaponType::TwoHandAxe:    weaponTypeStr = "TwoHandAxe";       break;
                    case WeaponType::Ranged:        weaponTypeStr = "Ranged";           break;
                    case WeaponType::Staff:         weaponTypeStr = "Staff";            break;
                    case WeaponType::Spell:         weaponTypeStr = "Spell";            break;
                    case WeaponType::Shout:         weaponTypeStr = "Shout";            break;
                    case WeaponType::Ability:       weaponTypeStr = "Ability";          break;
                    case WeaponType::LesserPower:   weaponTypeStr = "LesserPower";      break;
                    case WeaponType::Power:         weaponTypeStr = "Power";            break;
                    case WeaponType::Total:         weaponTypeStr = "Total";            break;
                    default:                        weaponTypeStr = "Other";            break;
                }

                std::string attackTypeStr;
                switch (attackType) {
                    case AttackType::Regular:       attackTypeStr = "Regular";          break;
                    case AttackType::Power:         attackTypeStr = "Power";            break;
                    case AttackType::Bash:          attackTypeStr = "Bash";             break;
                    case AttackType::Projectile:    attackTypeStr = "Projectile";       break;
                    case AttackType::Charge:        attackTypeStr = "Charge";           break;
                    case AttackType::Rotating:      attackTypeStr = "Rotating";         break;
                    case AttackType::Continuous:    attackTypeStr = "Continuous";       break;
                    default:                        attackTypeStr = "Regular";          break;
                }

                RuleContext ctx{ 
                    EventType::kHit,
                    source,
                    targetRef, 
                    baseObj,
                    attackSource,
                    projectile,
                    weaponTypeStr,
                    attackTypeStr,
                    true  // isHitEvent flag
                };
                
                RuleManager::GetSingleton()->Trigger(ctx);
            }
        });

        return RE::BSEventNotifyControl::kContinue;
    }

    RE::BSEventNotifyControl GrabReleaseSink::ProcessEvent(const RE::TESGrabReleaseEvent* evn, RE::BSTEventSource<RE::TESGrabReleaseEvent>*)
    {
        if (!evn || !evn->ref || !evn->ref.get()) return RE::BSEventNotifyControl::kContinue;

        auto* targetRef = evn->ref.get();
        if (!targetRef || targetRef->IsDeleted() || !targetRef->GetFormID()) return RE::BSEventNotifyControl::kContinue;

        auto* baseObj = targetRef->GetBaseObject();
        if (!baseObj) return RE::BSEventNotifyControl::kContinue;

        RE::Actor* source = nullptr;
        if (auto* handle = RE::PlayerCharacter::GetSingleton()) source = handle->As<RE::Actor>();

        if (!source || source->IsDeleted() || source->IsDead()) return RE::BSEventNotifyControl::kContinue;

        bool isGrabbed = evn->grabbed;

        if (isGrabbed) {
            SKSE::GetTaskInterface()->AddTask([source, targetRef, baseObj]() {
                if (source && !source->IsDeleted() && targetRef && !targetRef->IsDeleted() && baseObj) {   
                    RuleContext ctx{
                        EventType::kGrab,
                        source,
                        targetRef,
                        baseObj
                    };
                    RuleManager::GetSingleton()->Trigger(ctx);
                }
            });
        } else {
            SKSE::GetTaskInterface()->AddTask([source, targetRef, baseObj]() {
                if (source && !source->IsDeleted() && targetRef && !targetRef->IsDeleted() && baseObj) {
                    bool isTelekinesis = false;
                    bool isThrown = false;

                    RE::TESForm* leftSpell = source->GetEquippedObject(true);
                    RE::TESForm* rightSpell = source->GetEquippedObject(false);
                    
                    RE::MagicItem* leftMagicItem = leftSpell ? leftSpell->As<RE::MagicItem>() : nullptr;
                    RE::MagicItem* rightMagicItem = rightSpell ? rightSpell->As<RE::MagicItem>() : nullptr;
                    
                    if ((leftSpell || rightSpell) && (leftMagicItem || rightMagicItem)) {
                        bool hasGrabEffect = false;
                        for (auto* spell : { leftMagicItem, rightMagicItem }) {
                            if (spell) {
                                for (auto* effect : spell->effects) {
                                    if (effect && effect->baseEffect) {
                                        if (effect->baseEffect->data.archetype == RE::EffectArchetypes::ArchetypeID::kTelekinesis ||
                                            effect->baseEffect->data.archetype == RE::EffectArchetypes::ArchetypeID::kGrabActor) {
                                            hasGrabEffect = true;
                                            break;
                                        }
                                    }
                                }
                            }
                            if (hasGrabEffect) break;
                        }
                        isTelekinesis = hasGrabEffect;
                    }

                    auto* inputHandler = InputHandler::GetSingleton();
                    bool wasRKeyReleased = inputHandler->WasKeyJustReleased();
                    
                    if (auto threedimObj = targetRef->Get3D()) {
                        if (auto collisionObj = threedimObj->GetCollisionObject()) {
                            if (auto bhkBody = collisionObj->GetRigidBody()) {
                                if (auto hkpBody = bhkBody->GetRigidBody()) {
                                    if (isTelekinesis || (!isTelekinesis && wasRKeyReleased)) {
                                        int propertyId = isTelekinesis ? 314159 : 628318; // HK_PROPERTY_TELEKINESIS : HK_PROPERTY_GRABTHROWNOBJECT
                                        
                                        if (hkpBody->HasProperty(propertyId)) {
                                            float now = duration_cast<duration<float>>(steady_clock::now() - startTime).count();
                                            hkpBody->SetProperty(propertyId, now);
                                        }
                                        
                                        hkpBody->AddContactListener(LandingSink::GetSingleton());
                                    }
                                    
                                    if (wasRKeyReleased) {
                                        isThrown = true;
                                        InputHandler::GetSingleton()->ResetKeyState();
                                    }
                                }
                            }
                        }
                    }
                    if (!isThrown) {
                        RuleContext ctx{
                            EventType::kRelease,
                            source,
                            targetRef,
                            baseObj
                        };
                        RuleManager::GetSingleton()->Trigger(ctx);
                    }
                }
            });
        }

        return RE::BSEventNotifyControl::kContinue;
    }

    void LandingSink::ContactPointCallback(const RE::hkpContactPointEvent& a_event) 
    {
        if (!a_event.contactPoint || !a_event.firstCallbackForFullManifold) {
            return;
        }

        auto bodyA = a_event.bodies[0];
        auto bodyB = a_event.bodies[1];
        
        RE::hkpRigidBody* specialBody = nullptr;
        bool isTelekinesis = false;
        bool isThrown = false;

        if (bodyA && bodyA->HasProperty(HK_PROPERTY_TELEKINESIS)) {
            specialBody = bodyA;
            isTelekinesis = true;
        } else if (bodyB && bodyB->HasProperty(HK_PROPERTY_TELEKINESIS)) {
            specialBody = bodyB;
            isTelekinesis = true;
        }

        else if (bodyA && bodyA->HasProperty(HK_PROPERTY_GRABTHROWNOBJECT)) {
            specialBody = bodyA;
            isThrown = true;
        } else if (bodyB && bodyB->HasProperty(HK_PROPERTY_GRABTHROWNOBJECT)) {
            specialBody = bodyB;
            isThrown = true;
        }
        
        if (specialBody) {
            auto refr = specialBody->GetUserData();
            if (!refr) return;
            
            std::uint32_t objectID = refr->GetFormID();

            if (!objectID) return;

            if (processedObjects.find(objectID) != processedObjects.end()) return;

            processedObjects.insert(objectID);
            bodiesToCleanup.push_back(specialBody);

            SKSE::GetTaskInterface()->AddTask([refr, objectID, isTelekinesis, this]() {
                if (refr && !refr->IsDeleted() && objectID) {
                    EventType eventType = isTelekinesis ? EventType::kTelekinesis : EventType::kThrow;
                    RuleContext ctx{
                        eventType,
                        RE::PlayerCharacter::GetSingleton()->As<RE::Actor>(),
                        refr,
                        refr->GetBaseObject()
                    };
                    RuleManager::GetSingleton()->Trigger(ctx);
                    
                    SKSE::GetTaskInterface()->AddTask([this, objectID]() {
                        processedObjects.erase(objectID);
                    });
                }
            });

            SKSE::GetTaskInterface()->AddTask([this]() {
                for (auto* body : bodiesToCleanup) {
                    if (body) {
                        if (body->HasProperty(HK_PROPERTY_TELEKINESIS)) {
                            body->RemoveProperty(HK_PROPERTY_TELEKINESIS);
                        }
                        if (body->HasProperty(HK_PROPERTY_GRABTHROWNOBJECT)) {
                            body->RemoveProperty(HK_PROPERTY_GRABTHROWNOBJECT);
                        }
                        body->RemoveContactListener(this);
                    }
                }
                bodiesToCleanup.clear();
            });
        }
    }

    RE::BSEventNotifyControl InputHandler::ProcessEvent(RE::InputEvent* const* a_event, RE::BSTEventSource<RE::InputEvent*>*)
    {
        if (!a_event || !*a_event) return RE::BSEventNotifyControl::kContinue;

        for (RE::InputEvent* input = *a_event; input; input = input->next) {
            if (input->eventType != RE::INPUT_EVENT_TYPE::kButton) {
                continue;
            }

            auto buttonEvent = input->AsButtonEvent();
            if (!buttonEvent) continue;

            auto* button = static_cast<RE::ButtonEvent*>(input);
            if (!button || button->device != RE::INPUT_DEVICE::kKeyboard) continue;

            std::uint32_t keyCode = button->idCode;
            bool isDown = button->IsPressed();
            bool isUp = button->IsUp();

            // Check only the R button
            if (keyCode == R_KEY_CODE) {
                if (isDown) KeyWasPressed = true;
    
                if (isUp && KeyWasPressed) {
                    KeyJustReleased = true;
                    KeyWasPressed = false;
                }
            }
        }

        return RE::BSEventNotifyControl::kContinue;
    }

    void InputHandler::Register()
    {
        if (auto inputManager = RE::BSInputDeviceManager::GetSingleton()) inputManager->AddEventSink(this);
    }

    void InputHandler::Unregister()
    {
        if (auto inputManager = RE::BSInputDeviceManager::GetSingleton()) inputManager->RemoveEventSink(this);
    }

    RE::BSEventNotifyControl CellAttachDetachSink::ProcessEvent(const RE::TESCellAttachDetachEvent* evn,
        RE::BSTEventSource<RE::TESCellAttachDetachEvent>*)
    {
        if (!evn || !evn->reference) return RE::BSEventNotifyControl::kContinue;

        auto targetRef = evn->reference;
    
        if (!targetRef || targetRef->IsDeleted() || !targetRef->GetFormID()) return RE::BSEventNotifyControl::kContinue;

        SKSE::GetTaskInterface()->AddTask([evn, targetRef]() {
            if (evn && targetRef && !targetRef->IsDeleted()) {
                RE::Actor* nearestActor = nullptr;
                float minDistance = (std::numeric_limits<float>::max)();    
                
                auto* tes = RE::TES::GetSingleton();

                if (!tes) {
                    logger::error("CellAttachDetachSink: Cannot get TES singleton");
                    return;
                } 

                tes->ForEachReferenceInRange(targetRef.get(), 1024.0, [&](RE::TESObjectREFR* a_ref) {
                    if (!a_ref || a_ref->IsDeleted() || !a_ref->GetFormID()) {
                        return RE::BSContainer::ForEachResult::kContinue;
                    }
                    if (auto* actor = a_ref->As<RE::Actor>(); actor && !actor->IsDead() && !actor->IsDisabled()) {
                        float distance = targetRef.get()->GetPosition().GetDistance(actor->GetPosition());
                        if (distance < minDistance) {
                            minDistance = distance;
                            nearestActor = actor;
                        }
                    }
                    return RE::BSContainer::ForEachResult::kContinue;
                });
                RE::Actor* contextActor = nearestActor ? nearestActor : RE::PlayerCharacter::GetSingleton()->As<RE::Actor>();
                
                if (evn->attached) {
                    RuleContext ctx{
                        EventType::kCellAttach,
                        contextActor,
                        targetRef.get(),
                        targetRef->GetBaseObject()
                    };
                    RuleManager::GetSingleton()->Trigger(ctx);
                } else {
                    RuleContext ctx{
                        EventType::kCellDetach,
                        contextActor,
                        targetRef.get(),
                        targetRef->GetBaseObject()
                    };
                    RuleManager::GetSingleton()->Trigger(ctx);
                }
            }
        });

        return RE::BSEventNotifyControl::kContinue;
    }

    void RegisterSinks()
    {
        auto holder = RE::ScriptEventSourceHolder::GetSingleton();
        holder->GetEventSource<RE::TESActivateEvent>()->AddEventSink(ActivateSink::GetSingleton());
        holder->GetEventSource<RE::TESHitEvent>()->AddEventSink(HitSink::GetSingleton());
        holder->GetEventSource<RE::TESGrabReleaseEvent>()->AddEventSink(GrabReleaseSink::GetSingleton());
        holder->GetEventSource<RE::TESCellAttachDetachEvent>()->AddEventSink(CellAttachDetachSink::GetSingleton());
        RE::BSInputDeviceManager::GetSingleton()->AddEventSink(InputHandler::GetSingleton());
    }
}