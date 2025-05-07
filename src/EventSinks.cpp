#include "EventSinks.h"

namespace OIF
{
    RE::BSEventNotifyControl ActivateSink::ProcessEvent(const RE::TESActivateEvent* evn,
        RE::BSTEventSource<RE::TESActivateEvent>*)
    {
        if (!evn) 
            return RE::BSEventNotifyControl::kContinue;
    
        if (!evn->objectActivated)
            return RE::BSEventNotifyControl::kContinue;
            
        auto* target = evn->objectActivated.get();
        if (!target)
            return RE::BSEventNotifyControl::kContinue;
            
        auto* baseObj = target->GetBaseObject();
        if (!baseObj) {
            //logger::warn("Base object is null");
            return RE::BSEventNotifyControl::kContinue;
        }
    
        RE::Actor* source = nullptr;
        RE::TESObjectREFR* sourceRef = nullptr;
        
        if (evn->actionRef && evn->actionRef.get()) {
            sourceRef = evn->actionRef.get();
            source = sourceRef->As<RE::Actor>();
            
            if (!source) {
                return RE::BSEventNotifyControl::kContinue;
            }
        }
    
        RuleContext ctx{ 
            EventType::kActivate, 
            source, 
            target, 
            baseObj
        };
    
        RuleManager::GetSingleton()->Trigger(ctx);
        return RE::BSEventNotifyControl::kContinue;
    }

    RE::BSEventNotifyControl HitSink::ProcessEvent(const RE::TESHitEvent* evn,
        RE::BSTEventSource<RE::TESHitEvent>*)
    {
        if (!evn) 
            return RE::BSEventNotifyControl::kContinue;
        
        if (!evn->target)
            return RE::BSEventNotifyControl::kContinue;
        
        auto* targetRef = evn->target.get();
        if (!targetRef)
            return RE::BSEventNotifyControl::kContinue;
            
        auto* baseObj = targetRef->GetBaseObject();
        if (!baseObj) {
            //logger::warn("Base object is null");
            return RE::BSEventNotifyControl::kContinue;
        }
        
        RE::Actor* source = nullptr;
        RE::TESObjectREFR* sourceRef = nullptr;
        
        if (evn->cause && evn->cause.get()) {
            sourceRef = evn->cause.get();
            source = sourceRef->As<RE::Actor>();
            if (!source) {
                return RE::BSEventNotifyControl::kContinue;
            }
        }
        
        // Initialize hit-specific context data
        RE::TESObjectWEAP* weapon = nullptr;
        RE::BGSProjectile* projectile = nullptr;
        std::string weaponType = "Other";
        std::string attackType = "Regular";
        
        if (source) {
            // Get equipped weapon
            auto* equippedObject = source->GetEquippedObject(false);  // False for right hand
            weapon = equippedObject ? equippedObject->As<RE::TESObjectWEAP>() : nullptr;
            
            // Get attack info from actor
            if (auto* actorState = source->GetActorRuntimeData().currentProcess) {
                if (auto* highData = actorState->high) {
                    
                    if (highData->attackData && highData->attackData->data.flags.any(RE::AttackData::AttackFlag::kPowerAttack)) {
                        attackType = "Power";
                    }
                    
                    if (highData->attackData && highData->attackData->data.flags.any(RE::AttackData::AttackFlag::kBashAttack)) {
                        attackType = "Bash";
                    }
                    
                    auto* projectileSource = highData->muzzleFlash;
                    if (projectileSource && projectileSource->baseProjectile) {
                        projectile = projectileSource->baseProjectile;
                        attackType = "Projectile";
                    }
                }
            }
            
            // Determine weapon type
            if (weapon) {
                switch (weapon->GetWeaponType()) {
                    case RE::WEAPON_TYPE::kHandToHandMelee:
                        weaponType = "HandToHand";
                        break;
                    case RE::WEAPON_TYPE::kOneHandSword:
                        weaponType = "OneHandSword";
                        break;
                    case RE::WEAPON_TYPE::kOneHandDagger:
                        weaponType = "Dagger";
                        break;
                    case RE::WEAPON_TYPE::kOneHandAxe:
                        weaponType = "OneHandAxe";
                        break;
                    case RE::WEAPON_TYPE::kOneHandMace:
                        weaponType = "OneHandMace";
                        break;
                    case RE::WEAPON_TYPE::kTwoHandSword:
                        weaponType = "TwoHandSword";
                        break;
                    case RE::WEAPON_TYPE::kTwoHandAxe:
                        weaponType = "TwoHandAxe";
                        break;
                    case RE::WEAPON_TYPE::kBow:
                    case RE::WEAPON_TYPE::kCrossbow:
                        weaponType = "Ranged";
                        break;
                    case RE::WEAPON_TYPE::kStaff:
                        weaponType = "Staff";
                        break;
                    default:
                        weaponType = "Other";
                }
            }
            else {
                // Check if it's a spell
                if (auto* process = source->GetActorRuntimeData().currentProcess) {
                    bool hasSpell = false;
                    if (auto* magicCasterLeft = source->GetMagicCaster(RE::MagicSystem::CastingSource::kLeftHand)) {
                        if (magicCasterLeft->currentSpell) {
                            hasSpell = true;
                        }
                    }
                    if (auto* magicCasterRight = source->GetMagicCaster(RE::MagicSystem::CastingSource::kRightHand)) {
                        if (magicCasterRight->currentSpell) {
                            hasSpell = true;
                        }
                    }
                    if (hasSpell) {
                        weaponType = "Spell";
                    }
                }
            }
        }
        
        // Create the context with all the hit-specific info
        RuleContext ctx{ 
            EventType::kHit,
            source,
            targetRef, 
            baseObj,
            weapon,
            projectile,
            weaponType,
            attackType,
            true  // isHitEvent flag
        };
        
        RuleManager::GetSingleton()->Trigger(ctx);
        return RE::BSEventNotifyControl::kContinue;
    }

    void RegisterSinks()
    {
        auto holder = RE::ScriptEventSourceHolder::GetSingleton();
        holder->GetEventSource<RE::TESActivateEvent>()->AddEventSink(ActivateSink::GetSingleton());
        holder->GetEventSource<RE::TESHitEvent>()->AddEventSink(HitSink::GetSingleton());
    }
}