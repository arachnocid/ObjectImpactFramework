#include "EventSinks.h"

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
    RE::BSEventNotifyControl ActivateSink::ProcessEvent(const RE::TESActivateEvent* evn,
        RE::BSTEventSource<RE::TESActivateEvent>*)
    {
        if (!evn || !evn->objectActivated || !evn->objectActivated.get()) 
            return RE::BSEventNotifyControl::kContinue;

        auto* target = evn->objectActivated.get();
        auto* baseObj = target->GetBaseObject();
        if (!baseObj) return RE::BSEventNotifyControl::kContinue;

        RE::Actor* source = nullptr;
        if (evn->actionRef && evn->actionRef.get()) {
            source = evn->actionRef.get()->As<RE::Actor>();
            if (!source) return RE::BSEventNotifyControl::kContinue;
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
        if (!evn || !evn->target || !evn->target.get()) 
            return RE::BSEventNotifyControl::kContinue;

        auto* targetRef = evn->target.get();
        auto* baseObj = targetRef->GetBaseObject();
        if (!baseObj) return RE::BSEventNotifyControl::kContinue;

        RE::Actor* source = nullptr;
        if (evn->cause && evn->cause.get()) {
            source = evn->cause.get()->As<RE::Actor>();
            if (!source) return RE::BSEventNotifyControl::kContinue;
        }

        RE::TESObjectWEAP* weapon = nullptr;
        RE::BGSProjectile* projectile = nullptr;
        WeaponType weaponType = WeaponType::Other;
        AttackType attackType = AttackType::Regular;
        
        if (source) {
            
            // Determine equipped objects in both hands
            RE::TESForm* rightObj = source->GetEquippedObject(false);  // Right hand
            RE::TESForm* leftObj  = source->GetEquippedObject(true);   // Left hand

            // Resolve the form that generated the hit
            RE::TESForm* hitSourceForm = evn->source ? RE::TESForm::LookupByID(evn->source) : nullptr;

            bool leftHandAttack  = hitSourceForm && leftObj  && (hitSourceForm == leftObj);
            bool rightHandAttack = hitSourceForm && rightObj && (hitSourceForm == rightObj);

            // Select weapon based on attacking hand
            if (leftHandAttack) {
                weapon = leftObj ? leftObj->As<RE::TESObjectWEAP>() : nullptr;
            } else if (rightHandAttack) {
                weapon = rightObj ? rightObj->As<RE::TESObjectWEAP>() : nullptr;
            }

            // Retrieve attack and projectile data with null safety
            if (auto* actorState = source->GetActorRuntimeData().currentProcess) {
                if (auto* highData = actorState->high) {
                    if (highData->attackData) {
                        attackType = GetAttackType(highData->attackData.get(), nullptr);
                    }
                    if (highData->muzzleFlash && highData->muzzleFlash->baseProjectile) {
                        projectile = highData->muzzleFlash->baseProjectile;
                        attackType = AttackType::Projectile;
                    }
                }
            }

            // Spell detection per hand
            bool leftSpell = false;
            bool rightSpell = false;
            if (auto* magicCasterLeft = source->GetMagicCaster(RE::MagicSystem::CastingSource::kLeftHand)) {
                leftSpell = magicCasterLeft->currentSpell != nullptr;
            }
            if (auto* magicCasterRight = source->GetMagicCaster(RE::MagicSystem::CastingSource::kRightHand)) {
                rightSpell = magicCasterRight->currentSpell != nullptr;
            }

            // Determine weapon type
            if (weapon) {
                weaponType = GetWeaponType(weapon);
            } else if ((leftHandAttack && leftSpell) || (rightHandAttack && rightSpell) || (!weapon && (leftSpell || rightSpell))) {
                weaponType = WeaponType::Spell;
                weapon = nullptr;  // Spells are not weapons
            }
        }

        // Convert enums to strings for RuleContext compatibility
        std::string weaponTypeStr;
        switch (weaponType) {
            case WeaponType::HandToHand:   weaponTypeStr = "HandToHand";   break;
            case WeaponType::OneHandSword: weaponTypeStr = "OneHandSword"; break;
            case WeaponType::Dagger:       weaponTypeStr = "Dagger";       break;
            case WeaponType::OneHandAxe:   weaponTypeStr = "OneHandAxe";   break;
            case WeaponType::OneHandMace:  weaponTypeStr = "OneHandMace";  break;
            case WeaponType::TwoHandSword: weaponTypeStr = "TwoHandSword"; break;
            case WeaponType::TwoHandAxe:   weaponTypeStr = "TwoHandAxe";   break;
            case WeaponType::Ranged:       weaponTypeStr = "Ranged";       break;
            case WeaponType::Staff:        weaponTypeStr = "Staff";        break;
            case WeaponType::Spell:        weaponTypeStr = "Spell";        break;
            default:                       weaponTypeStr = "Other";        break;
        }

        std::string attackTypeStr;
        switch (attackType) {
            case AttackType::Regular:       attackTypeStr = "Regular";    break;
            case AttackType::Power:         attackTypeStr = "Power";      break;
            case AttackType::Bash:          attackTypeStr = "Bash";       break;
            case AttackType::Projectile:    attackTypeStr = "Projectile"; break;
            case AttackType::Charge:        attackTypeStr = "Charge";     break;
            case AttackType::Rotating:      attackTypeStr = "Rotating";   break;
            case AttackType::Continuous:    attackTypeStr = "Continuous"; break;
            default:                        attackTypeStr = "Regular";    break;
        }

        RuleContext ctx{ 
            EventType::kHit,
            source,
            targetRef, 
            baseObj,
            weapon,
            projectile,
            weaponTypeStr,
            attackTypeStr,
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