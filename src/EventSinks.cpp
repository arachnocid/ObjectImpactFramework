#include "EventSinks.h"
#include "PCH.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

using namespace std::chrono;

namespace OIF
{
    // ------------------ Constants ------------------
    const std::unordered_set<RE::FormType> EventSinkBase::relevantFormTypes = {
        RE::FormType::Activator,
        RE::FormType::TalkingActivator,
        RE::FormType::Weapon,
        RE::FormType::Armor,
        RE::FormType::Ammo,
        RE::FormType::Ingredient,
        RE::FormType::Misc,
        RE::FormType::Book,
        RE::FormType::Note,
        RE::FormType::Scroll,
        RE::FormType::SoulGem,
        RE::FormType::AlchemyItem,
        RE::FormType::Furniture,
        RE::FormType::Door,
        RE::FormType::Flora,
        RE::FormType::Container,
        RE::FormType::Static,
        RE::FormType::MovableStatic,
        RE::FormType::Tree,
        RE::FormType::KeyMaster,
        RE::FormType::Light
    };

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
        Scroll,
        Shout,
        Ability,
        LesserPower,
        Power,
        Explosion,
        Total,
        Other
    };

    enum class AttackType {
        Regular,
        Power,
        Bash,
        Charge,
        Rotating,
        Continuous,
        Constant,
        FireAndForget,
        IgnoreWeapon,
        OverrideData    
    }; 

    enum class DeliveryType {
        Self,
        Aimed,
        TargetActor,
        TargetLocation,
        Touch,
        Total,
        None
    };

    // ------------------ Static Variables ------------------
    static auto startTime = std::chrono::steady_clock::now();

    // ------------------ Helpers ------------------
    WeaponType GetWeaponType(RE::TESObjectWEAP* weapon) {
        if (!weapon) return WeaponType::Other;
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
            default:                                 weaponType = WeaponType::Other;        break;
        }
        return weaponType;
    }

    WeaponType GetSpellType(RE::SpellItem* spell) {
        if (!spell) return WeaponType::Other;
        WeaponType weaponType = WeaponType::Other;
        switch (spell->GetSpellType()) {
            case RE::MagicSystem::SpellType::kSpell:        weaponType = WeaponType::Spell;        break;
            case RE::MagicSystem::SpellType::kVoicePower:   weaponType = WeaponType::Shout;        break;
            case RE::MagicSystem::SpellType::kAbility:      weaponType = WeaponType::Ability;      break;
            case RE::MagicSystem::SpellType::kLesserPower:  weaponType = WeaponType::LesserPower;  break;
            case RE::MagicSystem::SpellType::kPower:        weaponType = WeaponType::Power;        break;
            case RE::MagicSystem::SpellType::kLeveledSpell: weaponType = WeaponType::Spell;        break;
            case RE::MagicSystem::SpellType::kScroll:       weaponType = WeaponType::Spell;        break;
            case RE::MagicSystem::SpellType::kStaffEnchantment: weaponType = WeaponType::Staff;    break;
            default:                                        weaponType = WeaponType::Other;        break;
        }
        return weaponType;
    }

    AttackType GetAttackType(RE::BGSAttackData* attackData) {
        if (attackData) {
            if (attackData->data.flags.any(RE::AttackData::AttackFlag::kPowerAttack)) return AttackType::Power;
            if (attackData->data.flags.any(RE::AttackData::AttackFlag::kBashAttack)) return AttackType::Bash;
            if (attackData->data.flags.any(RE::AttackData::AttackFlag::kChargeAttack)) return AttackType::Charge;
            if (attackData->data.flags.any(RE::AttackData::AttackFlag::kRotatingAttack)) return AttackType::Rotating;
            if (attackData->data.flags.any(RE::AttackData::AttackFlag::kContinuousAttack)) return AttackType::Continuous;
            if (attackData->data.flags.any(RE::AttackData::AttackFlag::kIgnoreWeapon)) return AttackType::IgnoreWeapon;
            if (attackData->data.flags.any(RE::AttackData::AttackFlag::kOverrideData)) return AttackType::OverrideData;
        }
        return AttackType::Regular;
    }

    std::string WeaponTypeToString(WeaponType weaponType) {
        switch (weaponType) {
            case WeaponType::HandToHand:     return "handtohand";
            case WeaponType::OneHandSword:   return "onehandsword";
            case WeaponType::Dagger:         return "dagger";
            case WeaponType::OneHandAxe:     return "onehandaxe";
            case WeaponType::OneHandMace:    return "onehandmace";
            case WeaponType::TwoHandSword:   return "twohandsword";
            case WeaponType::TwoHandAxe:     return "twohandaxe";
            case WeaponType::Ranged:         return "ranged";
            case WeaponType::Staff:          return "staff";
            case WeaponType::Spell:          return "spell";
            case WeaponType::Scroll:         return "scroll";
            case WeaponType::Shout:          return "shout";
            case WeaponType::Ability:        return "ability";
            case WeaponType::LesserPower:    return "lesserpower";
            case WeaponType::Power:          return "power";
            case WeaponType::Explosion:      return "explosion";
            case WeaponType::Total:          return "total";
            default:                         return "other";
        }
    }
    
    std::string AttackTypeToString(AttackType attackType) {
        switch (attackType) {
            case AttackType::Regular:        return "regular";
            case AttackType::Power:          return "power";
            case AttackType::Bash:           return "bash";
            case AttackType::Charge:         return "charge";
            case AttackType::Rotating:       return "rotating";
            case AttackType::Continuous:     return "continuous";
            case AttackType::Constant:       return "constant";
            case AttackType::FireAndForget:  return "fireandforget";
            case AttackType::IgnoreWeapon:   return "ignoreweapon";
            case AttackType::OverrideData:   return "overridedata";
            default:                         return "regular";
        }
    }

    std::string DeliveryTypeToString(DeliveryType delivery) {
        switch (delivery) {
            case DeliveryType::Self:           return "self";
            case DeliveryType::Aimed:          return "aimed";
            case DeliveryType::TargetActor:    return "targetactor";
            case DeliveryType::TargetLocation: return "targetlocation";
            case DeliveryType::Touch:          return "touch";
            case DeliveryType::Total:          return "total";
            case DeliveryType::None:           return "none";
            default:                           return "none";
        }
    }

    void ScanCell(RE::Actor* source, std::vector<RE::TESObjectREFR*>* foundObjects = nullptr, bool triggerEvents = false, EventType eventType = EventType::kNone, RE::TESWeather* weather = nullptr, const UpdateFilter* updateFilter = nullptr)
    {
        if (!source) return;

        auto* cell = source->GetParentCell();
        if (!cell) return;

        std::size_t processedCount = 0;
        std::size_t skippedCount = 0;

        cell->ForEachReference([&](RE::TESObjectREFR* ref) -> RE::BSContainer::ForEachResult {
            if (!ref || ref->IsDeleted() || !ref->GetFormID() || !ref->GetBaseObject()) {
                return RE::BSContainer::ForEachResult::kContinue;
            }

            if (triggerEvents) {
                if (eventType == EventType::kOnUpdate && updateFilter && !updateFilter->IsEmpty()) {
                    if (!updateFilter->Matches(ref)) {
                        skippedCount++;
                        return RE::BSContainer::ForEachResult::kContinue;
                    }
                }
                
                if (!EventSinkBase::IsRelevantObjectRef(ref)) {
                    return RE::BSContainer::ForEachResult::kContinue;
                }

                processedCount++;

                RuleContext ctx{
                    eventType,
                    source, 
                    ref,
                    ref->GetBaseObject(), 
                    nullptr,
                    nullptr,
                    "",
                    "",
                    "",
                    false,
                    weather
                };
                
                RuleManager::GetSingleton()->Trigger(ctx);
            } else {
                if (foundObjects) foundObjects->push_back(ref);
            }

            return RE::BSContainer::ForEachResult::kContinue;
        });
        
        if (eventType == EventType::kOnUpdate && updateFilter && !updateFilter->IsEmpty()) {
            logger::debug("OnUpdate ScanCell: processed {}, skipped {} objects", processedCount, skippedCount);
        }
    }

    // ------------------ Sinks ------------------
    RE::BSEventNotifyControl ActivateSink::ProcessEvent(const RE::TESActivateEvent* evn, RE::BSTEventSource<RE::TESActivateEvent>*)
    {
        if (!evn || !evn->objectActivated || !evn->objectActivated.get()) return RE::BSEventNotifyControl::kContinue;

        auto* targetRef = evn->objectActivated.get();
        if (!targetRef || targetRef->IsDeleted() || !targetRef->GetFormID()) return RE::BSEventNotifyControl::kContinue;

        if (!EventSinkBase::IsRelevantObjectRef(targetRef)) return RE::BSEventNotifyControl::kContinue;

        auto* baseObj = targetRef->GetBaseObject();
        if (!baseObj) return RE::BSEventNotifyControl::kContinue;
        
        RE::Actor* source = nullptr;
        if (evn->actionRef && evn->actionRef.get()) source = evn->actionRef.get()->As<RE::Actor>();

        if (!source || source->IsDeleted() || source->IsDead()) return RE::BSEventNotifyControl::kContinue;

        RE::FormID sourceID = source->GetFormID();
        RE::FormID targetID = targetRef->GetFormID();
        RE::FormID baseObjID = baseObj->GetFormID();

        SKSE::GetTaskInterface()->AddTask([sourceID, targetID, baseObjID]() {
            auto* source = RE::TESForm::LookupByID<RE::Actor>(sourceID);
            auto* targetRef = RE::TESForm::LookupByID<RE::TESObjectREFR>(targetID);
            auto* baseObj = RE::TESForm::LookupByID(baseObjID);
            
            if (!source || source->IsDeleted() || !targetRef || targetRef->IsDeleted() || !baseObj) return;
            
            RuleContext ctx{ 
                EventType::kActivate, 
                source, 
                targetRef, 
                baseObj
            };
            RuleManager::GetSingleton()->Trigger(ctx);
        });

        return RE::BSEventNotifyControl::kContinue;
    }

    RE::BSEventNotifyControl HitSink::ProcessEvent(const RE::TESHitEvent* evn, RE::BSTEventSource<RE::TESHitEvent>*)
    {
        if (!evn || !evn->target || !evn->target.get()) return RE::BSEventNotifyControl::kContinue;

        auto* targetRef = evn->target.get();
        if (!targetRef || targetRef->IsDeleted() || !targetRef->GetFormID()) return RE::BSEventNotifyControl::kContinue;

        if (!EventSinkBase::IsRelevantObjectRef(targetRef)) return RE::BSEventNotifyControl::kContinue;

        auto* baseObj = targetRef->GetBaseObject();
        if (!baseObj) return RE::BSEventNotifyControl::kContinue;
        
        RE::Actor* source = nullptr;
        if (evn->cause && evn->cause.get()) source = evn->cause.get()->As<RE::Actor>();

        if (!source || source->IsDeleted() || source->IsDead()) return RE::BSEventNotifyControl::kContinue;

        RE::FormID hitSourceID = evn->source;
        RE::FormID projectileID = evn->projectile;
        RE::FormID sourceID = source->GetFormID();
        RE::FormID targetID = targetRef->GetFormID();
        RE::FormID baseObjID = baseObj->GetFormID();

        SKSE::GetTaskInterface()->AddTask([hitSourceID, projectileID, sourceID, targetID, baseObjID]() {
            auto* source = RE::TESForm::LookupByID<RE::Actor>(sourceID);
            auto* targetRef = RE::TESForm::LookupByID<RE::TESObjectREFR>(targetID);
            auto* baseObj = RE::TESForm::LookupByID(baseObjID);
            auto* hitSourceForm = hitSourceID ? RE::TESForm::LookupByID(hitSourceID) : nullptr;
            auto* projectileForm = projectileID ? RE::TESForm::LookupByID<RE::BGSProjectile>(projectileID) : nullptr;

            if (!source || source->IsDeleted() || !targetRef || targetRef->IsDeleted() || !baseObj) return;

            RE::TESForm* attackSource = nullptr;
            RE::TESForm* projectileSource = nullptr;
            WeaponType weaponType = WeaponType::Other;
            AttackType attackType = AttackType::Regular;
            DeliveryType deliveryType = DeliveryType::None;

            if (hitSourceForm && hitSourceForm->As<RE::BGSExplosion>()) return;

            if (auto* actorState = source->GetActorRuntimeData().currentProcess) {
                auto* highData = actorState->high;
                if (!highData) return;

                if (highData->attackData) {
                    attackType = GetAttackType(highData->attackData.get());
                }

                if (highData->muzzleFlash && highData->muzzleFlash->baseProjectile) {
                    auto* projectile = highData->muzzleFlash->baseProjectile;
                    if (projectile->As<RE::BGSExplosion>()) return;
                    attackSource = projectile;
                    projectileSource = projectile;
                }
            }

            if (projectileForm) {
                if (projectileForm->As<RE::BGSExplosion>()) return;
                attackSource = projectileForm;
                projectileSource = projectileForm;
            }

            if (hitSourceForm) {
                if (auto* spell = hitSourceForm->As<RE::SpellItem>()) {
                    weaponType = GetSpellType(spell);
                    attackSource = spell;
                    
                    if (spell->effects.size() > 0) {
                        auto* effect = spell->effects[0];
                        if (effect && effect->baseEffect) {
                            switch (effect->baseEffect->data.delivery) {
                                case RE::MagicSystem::Delivery::kSelf:
                                    deliveryType = DeliveryType::Self;
                                    break;
                                case RE::MagicSystem::Delivery::kAimed:
                                    deliveryType = DeliveryType::Aimed;
                                    break;
                                case RE::MagicSystem::Delivery::kTargetActor:
                                    deliveryType = DeliveryType::TargetActor;
                                    break;
                                case RE::MagicSystem::Delivery::kTargetLocation:
                                    deliveryType = DeliveryType::TargetLocation;
                                    break;
                                case RE::MagicSystem::Delivery::kTouch:
                                    deliveryType = DeliveryType::Touch;
                                    break;
                                default:
                                    deliveryType = DeliveryType::None;
                                    break;
                            }
                        }
                    }

                    switch (spell->GetCastingType()) {
                        case RE::MagicSystem::CastingType::kConcentration:
                            attackType = AttackType::Continuous;
                            break;
                        case RE::MagicSystem::CastingType::kFireAndForget:
                            attackType = AttackType::FireAndForget;
                            break;
                        case RE::MagicSystem::CastingType::kConstantEffect:
                            attackType = AttackType::Constant;
                            break;
                        case RE::MagicSystem::CastingType::kScroll:
                            weaponType = WeaponType::Scroll;
                            break;
                        default:
                            break;
                    }
                }

                else if (auto* weapon = hitSourceForm->As<RE::TESObjectWEAP>()) {
                    weaponType = GetWeaponType(weapon);
                    attackSource = weapon;
                }
            }

            RuleContext ctx{ 
                EventType::kHit,
                source,
                targetRef, 
                baseObj,
                attackSource,
                projectileSource,
                WeaponTypeToString(weaponType),
                AttackTypeToString(attackType),
                DeliveryTypeToString(deliveryType),
                true
            };
            
            RuleManager::GetSingleton()->Trigger(ctx);
        });

        return RE::BSEventNotifyControl::kContinue;
    }

    RE::BSEventNotifyControl MagicEffectApplySink::ProcessEvent(const RE::TESMagicEffectApplyEvent* evn, RE::BSTEventSource<RE::TESMagicEffectApplyEvent>*)
    {
        if (!evn || !evn->target || !evn->target.get()) return RE::BSEventNotifyControl::kContinue;
    
        auto* targetRef = evn->target.get();
        if (!targetRef || targetRef->IsDeleted() || !targetRef->GetFormID()) return RE::BSEventNotifyControl::kContinue;
    
        if (!EventSinkBase::IsRelevantObjectRef(targetRef)) return RE::BSEventNotifyControl::kContinue;
    
        auto* baseObj = targetRef->GetBaseObject();
        if (!baseObj) return RE::BSEventNotifyControl::kContinue;
    
        RE::FormID targetID = 0;
        RE::FormID magicEffectID = 0;
        RE::FormID casterID = 0;
    
        if (targetRef) {
            targetID = targetRef->GetFormID();
        }
    
        if (evn->magicEffect) {
            magicEffectID = evn->magicEffect;
        }
    
        if (evn->caster) {
            auto* casterRef = evn->caster.get();
            if (casterRef && !casterRef->IsDeleted() && casterRef->As<RE::Actor>() && !casterRef->IsDead()) {
                casterID = casterRef->GetFormID();
            }
        }
    
        if (targetID == 0) return RE::BSEventNotifyControl::kContinue;
        if (casterID == 0) return RE::BSEventNotifyControl::kContinue;
        if (magicEffectID == 0) return RE::BSEventNotifyControl::kContinue;
    
        SKSE::GetTaskInterface()->AddTask([targetID, magicEffectID, casterID]() {
            auto* targetRef = RE::TESForm::LookupByID<RE::TESObjectREFR>(targetID);
            auto* magicEffect = RE::TESForm::LookupByID<RE::EffectSetting>(magicEffectID);
            auto* caster = RE::TESForm::LookupByID<RE::Actor>(casterID);
            
            if (!targetRef || targetRef->IsDeleted()) return;
            if (!magicEffect) return;
            if (!caster || caster->IsDeleted()) return;
    
            RE::TESForm* attackSource = magicEffect;
            RE::TESForm* projectileSource = nullptr;
            WeaponType weaponType = WeaponType::Other;
            AttackType attackType = AttackType::Regular;
            DeliveryType deliveryType = DeliveryType::None;
            bool isShout = false;
            bool isSpell = false;

            if (magicEffect->data.projectileBase) {
                if (!magicEffect->data.projectileBase->As<RE::BGSExplosion>()) {
                    attackSource = magicEffect->data.projectileBase;
                    projectileSource = magicEffect->data.projectileBase;
                }
            }
    
            if (magicEffect->data.associatedForm && magicEffect->data.associatedForm->formType == RE::FormType::Shout) {
                weaponType = WeaponType::Shout;
                attackSource = magicEffect->data.associatedForm;
                isShout = true;
            }
            
            if (!isShout) {
                auto* currentShout = caster->GetCurrentShout();
                if (currentShout) {
                    for (int i = 0; i < RE::TESShout::VariationIDs::kTotal; ++i) {
                        auto& variation = currentShout->variations[i];
                        if (variation.spell) {
                            for (auto* effect : variation.spell->effects) {
                                if (effect && effect->baseEffect == magicEffect) {
                                    weaponType = WeaponType::Shout;
                                    attackSource = currentShout;
                                    isShout = true;
                                    break;
                                }
                            }
                            if (isShout) break;
                        }
                    }
                }
            }
    
            if (!isShout) {
                RE::SpellItem* sourceSpell = nullptr;
    
                std::array<RE::MagicSystem::CastingSource, 4> castingSources = {
                    RE::MagicSystem::CastingSource::kInstant,
                    RE::MagicSystem::CastingSource::kLeftHand,
                    RE::MagicSystem::CastingSource::kOther,
                    RE::MagicSystem::CastingSource::kRightHand
                };
    
                for (auto castingSource : castingSources) {
                    if (auto* casterActor = caster->As<RE::Actor>()) {
                        if (auto* magicCaster = casterActor->GetMagicCaster(castingSource)) {
                            if (auto* currentSpell = magicCaster->currentSpell) {
                                for (auto* effect : currentSpell->effects) {
                                    if (effect && effect->baseEffect == magicEffect) {
                                        sourceSpell = currentSpell->As<RE::SpellItem>();
                                        break;
                                    }
                                }
                                if (sourceSpell) break;
                            }
                        }
                    }
                }
    
                if (sourceSpell) {
                    weaponType = GetSpellType(sourceSpell);
                    attackSource = sourceSpell;
                    isSpell = true;
                }
            }

            switch (magicEffect->data.castingType) {
                case RE::MagicSystem::CastingType::kConcentration:
                    attackType = AttackType::Continuous;
                    break;
                case RE::MagicSystem::CastingType::kFireAndForget:
                    attackType = AttackType::FireAndForget;
                    break;
                case RE::MagicSystem::CastingType::kConstantEffect:
                    attackType = AttackType::Constant;
                    break;
                case RE::MagicSystem::CastingType::kScroll:
                    weaponType = WeaponType::Scroll;
                    break;
                default:
                    break;
            }
    
            switch (magicEffect->data.delivery) {
                case RE::MagicSystem::Delivery::kSelf:
                    deliveryType = DeliveryType::Self;
                    break;
                case RE::MagicSystem::Delivery::kAimed:
                    deliveryType = DeliveryType::Aimed;
                    break;
                case RE::MagicSystem::Delivery::kTargetActor:
                    deliveryType = DeliveryType::TargetActor;
                    break;
                case RE::MagicSystem::Delivery::kTargetLocation:
                    deliveryType = DeliveryType::TargetLocation;
                    break;
                case RE::MagicSystem::Delivery::kTouch:
                    deliveryType = DeliveryType::Touch;
                    break;
                case RE::MagicSystem::Delivery::kTotal:
                    deliveryType = DeliveryType::Total;
                    break;
                default:
                    deliveryType = DeliveryType::None;
                    break;
            }
    
            RuleContext ctx{
                EventType::kHit,
                caster,
                targetRef,
                targetRef->GetBaseObject(),
                attackSource,
                projectileSource,
                WeaponTypeToString(weaponType),
                AttackTypeToString(attackType),
                DeliveryTypeToString(deliveryType),
                true
            };
        
            RuleManager::GetSingleton()->Trigger(ctx);
        });
    
        return RE::BSEventNotifyControl::kContinue;
    }

    void ExplosionHook::thunk(RE::Explosion* a_this)
    {        
        if (!a_this) {
            func(a_this);
            return;
        }
        
        if (a_this->IsDeleted()) {
            func(a_this);
            return;
        }
                
        auto* baseObj = a_this->GetBaseObject();
        auto explosionPos = a_this->GetPosition();
        auto* cell = a_this->GetParentCell();
        
        auto* explosionForm = skyrim_cast<RE::BGSExplosion*>(baseObj);
        auto& runtimeData = a_this->GetExplosionRuntimeData();

        if (explosionForm && explosionForm->data.damage <= 0.0f) {
            func(a_this);
            return;
        }
        
        float explosionRadius = 0.0f;
        
        if (explosionForm) {
            explosionRadius = explosionForm->data.radius;
        } else {
            explosionRadius = runtimeData.radius;
        }
        
        if (explosionRadius <= 0.0f) {
            func(a_this);
            return;
        }
                
        func(a_this);
        
        if (!baseObj || !cell) return;
        
        std::vector<RE::FormID> potentialSources;
        
        cell->ForEachReferenceInRange(explosionPos, explosionRadius, [&](RE::TESObjectREFR* ref) -> RE::BSContainer::ForEachResult {
            if (auto* actor = skyrim_cast<RE::Actor*>(ref)) {
                if (!actor->IsDeleted() && !actor->IsDead()) {
                    potentialSources.push_back(actor->GetFormID());
                }
            }
            return RE::BSContainer::ForEachResult::kContinue;
        });
        
        if (potentialSources.empty()) {
            if (auto* player = RE::PlayerCharacter::GetSingleton()) {
                potentialSources.push_back(player->GetFormID());
            }
        }
        
        if (potentialSources.empty()) return;
        
        RE::FormID baseObjID = baseObj->GetFormID();
        RE::FormID cellID = cell->GetFormID();
                
        SKSE::GetTaskInterface()->AddTask([potentialSources, baseObjID, cellID, explosionPos, explosionRadius]() {
            
            auto* baseObj = RE::TESForm::LookupByID(baseObjID);
            auto* cell = RE::TESForm::LookupByID<RE::TESObjectCELL>(cellID);
            
            if (!baseObj || !cell) return;
            
            for (size_t sourceIndex = 0; sourceIndex < potentialSources.size(); ++sourceIndex) {
                auto sourceID = potentialSources[sourceIndex];
                auto* source = RE::TESForm::LookupByID<RE::Actor>(sourceID);
                
                if (!source || source->IsDeleted()) continue;
                                
                int targetCount = 0;
                
                cell->ForEachReferenceInRange(explosionPos, explosionRadius, [&](RE::TESObjectREFR* ref) -> RE::BSContainer::ForEachResult {
                    if (!ref || ref->IsDeleted() || !ref->GetFormID()) return RE::BSContainer::ForEachResult::kContinue;
        
                    if (!EventSinkBase::IsRelevantObjectRef(ref)) return RE::BSContainer::ForEachResult::kContinue;
        
                    auto* targetBaseObj = ref->GetBaseObject();
                    if (!targetBaseObj) return RE::BSContainer::ForEachResult::kContinue;
        
                    if (ref == source) return RE::BSContainer::ForEachResult::kContinue;
        
                    targetCount++;
        
                    WeaponType weaponType = WeaponType::Explosion;
                    AttackType attackType = AttackType::Regular;
                    DeliveryType deliveryType = DeliveryType::None;
        
                    RuleContext ctx{
                        EventType::kHit,
                        source,
                        ref,
                        targetBaseObj,
                        baseObj,
                        nullptr,
                        WeaponTypeToString(weaponType),
                        AttackTypeToString(attackType),
                        DeliveryTypeToString(deliveryType),
                        true
                    };
        
                    RuleManager::GetSingleton()->Trigger(ctx);
        
                    return RE::BSContainer::ForEachResult::kContinue;
                });
            }
        });
    }

    RE::BSEventNotifyControl GrabReleaseSink::ProcessEvent(const RE::TESGrabReleaseEvent* evn, RE::BSTEventSource<RE::TESGrabReleaseEvent>*)
    {
        if (!evn || !evn->ref || !evn->ref.get()) return RE::BSEventNotifyControl::kContinue;

        auto* targetRef = evn->ref.get();
        if (!targetRef || targetRef->IsDeleted() || !targetRef->GetFormID()) return RE::BSEventNotifyControl::kContinue;

        if (!EventSinkBase::IsRelevantObjectRef(targetRef)) return RE::BSEventNotifyControl::kContinue;

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
        if (!a_event.contactPoint || !a_event.firstCallbackForFullManifold) return;

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

    void ReadyWeaponHook::thunk(RE::ReadyWeaponHandler* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data)
    {
        if (a_event) {
            if (a_event->IsUp()) {
                InputHandler::GetSingleton()->SetKeyJustReleased();
            }
        }
    
        return func(a_this, a_event, a_data);
    }

    RE::BSEventNotifyControl CellAttachDetachSink::ProcessEvent(const RE::TESCellAttachDetachEvent* evn, RE::BSTEventSource<RE::TESCellAttachDetachEvent>*)
    {
        if (!evn || !evn->reference) return RE::BSEventNotifyControl::kContinue;

        auto targetRef = evn->reference;
        bool attached = evn->attached;
    
        if (!targetRef || targetRef->IsDeleted() || !targetRef->GetFormID()) return RE::BSEventNotifyControl::kContinue;

        if (!EventSinkBase::IsRelevantObjectRef(targetRef.get())) return RE::BSEventNotifyControl::kContinue;

        auto* baseObj = targetRef->GetBaseObject();
        if (!baseObj) return RE::BSEventNotifyControl::kContinue;

        SKSE::GetTaskInterface()->AddTask([targetRef, baseObj, attached]() {
            if (targetRef && !targetRef->IsDeleted() && baseObj) {
                
                if (attached) {
                    RuleContext ctx {
                        EventType::kCellAttach,
                        RE::PlayerCharacter::GetSingleton()->As<RE::Actor>(),
                        targetRef.get(),
                        baseObj
                    };
                    RuleManager::GetSingleton()->Trigger(ctx);
                } else {
                    RuleContext ctx {
                        EventType::kCellDetach,
                        RE::PlayerCharacter::GetSingleton()->As<RE::Actor>(),
                        targetRef.get(),
                        baseObj
                    };
                    RuleManager::GetSingleton()->Trigger(ctx);
                }
            }
        });

        return RE::BSEventNotifyControl::kContinue;
    }

    void WeatherChangeHook::thunk(RE::TESWeather* a_currentWeather)
    {
        func(a_currentWeather);
        
        if (!a_currentWeather) return;
        if (a_currentWeather == currentWeather) return;
        
        currentWeather = a_currentWeather;
        
        SKSE::GetTaskInterface()->AddTask([a_currentWeather]() {
            auto* player = RE::PlayerCharacter::GetSingleton();
            if (!player) return;

            ScanCell(player, nullptr, true, EventType::kWeatherChange, a_currentWeather);
        });
    }

    void UpdateHook::thunk(RE::PlayerCharacter* a_this)
    {
        func(a_this);
        if (!a_this || a_this->IsDead()) return;

        static auto lastUpdateTime = std::chrono::steady_clock::now();
        auto currentTime = std::chrono::steady_clock::now();
        auto deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastUpdateTime);

        if (deltaTime.count() < 250) return;
        lastUpdateTime = currentTime;

        auto* ruleManager = RuleManager::GetSingleton();
        if (!ruleManager) return;

        bool hasAnyUpdateRules = false;
    
        {
            std::shared_lock lock(ruleManager->_ruleMutex);
            const auto& updateRules = ruleManager->GetUpdateRules();
            if (updateRules.empty()) return;
            hasAnyUpdateRules = !updateRules.empty();
        }
    
        if (hasAnyUpdateRules) {
            const auto& updateFilter = ruleManager->GetUpdateFilter();
            ScanCell(a_this, nullptr, true, EventType::kOnUpdate, nullptr, &updateFilter);
        }
    }

    void RegisterSinks()
    {
        auto holder = RE::ScriptEventSourceHolder::GetSingleton();
        holder->GetEventSource<RE::TESActivateEvent>()->AddEventSink(ActivateSink::GetSingleton());
        holder->GetEventSource<RE::TESHitEvent>()->AddEventSink(HitSink::GetSingleton());
        holder->GetEventSource<RE::TESGrabReleaseEvent>()->AddEventSink(GrabReleaseSink::GetSingleton());
        holder->GetEventSource<RE::TESCellAttachDetachEvent>()->AddEventSink(CellAttachDetachSink::GetSingleton());
        holder->GetEventSource<RE::TESMagicEffectApplyEvent>()->AddEventSink(MagicEffectApplySink::GetSingleton());
    }

    void InstallHooks() 
    {
        REL::Relocation<std::uintptr_t> weatherChangeHook{ REL::VariantID(25684, 26231, 25684), REL::VariantOffset(0x44F, 0x46C, 0x44F) }; // Taken from the Rain Extinguishes Fires source code
        ::stl::write_thunk_call<WeatherChangeHook>(weatherChangeHook.address());
        ::stl::write_vfunc<RE::ReadyWeaponHandler, ReadyWeaponHook>();
        ::stl::write_vfunc<RE::Explosion, ExplosionHook>();
        ::stl::write_vfunc<RE::PlayerCharacter, UpdateHook>();
    }
}