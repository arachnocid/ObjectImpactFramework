#include "EventSinks.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

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

        auto* targetRef = evn->objectActivated.get();

        if (targetRef->formFlags & RE::TESForm::RecordFlags::kDeleted || targetRef->IsDisabled())
            return RE::BSEventNotifyControl::kContinue;

        if (!targetRef->Is3DLoaded())
            return RE::BSEventNotifyControl::kContinue;

        auto* baseObj = targetRef->GetBaseObject();
        if (!baseObj) 
            return RE::BSEventNotifyControl::kContinue;

        RE::Actor* source = nullptr;
        if (evn->actionRef && evn->actionRef.get())
            source = evn->actionRef.get()->As<RE::Actor>();

        if (!source) 
            return RE::BSEventNotifyControl::kContinue;

        if (source->formFlags & RE::TESForm::RecordFlags::kDeleted || source->IsDisabled())
            return RE::BSEventNotifyControl::kContinue;

        RuleContext ctx{ 
            EventType::kActivate, 
            source, 
            targetRef, 
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

        if (targetRef->formFlags & RE::TESForm::RecordFlags::kDeleted || targetRef->IsDisabled())
            return RE::BSEventNotifyControl::kContinue;

        if (!targetRef->Is3DLoaded())
            return RE::BSEventNotifyControl::kContinue;

        auto* baseObj = targetRef->GetBaseObject();
        if (!baseObj) 
            return RE::BSEventNotifyControl::kContinue;

        RE::Actor* source = nullptr;
        if (evn->cause && evn->cause.get())
            source = evn->cause.get()->As<RE::Actor>();
        
        if (!source) 
            return RE::BSEventNotifyControl::kContinue;

        if (source->formFlags & RE::TESForm::RecordFlags::kDeleted || source->IsDisabled())
            return RE::BSEventNotifyControl::kContinue;

        RE::TESObjectWEAP* weapon = nullptr;
        RE::BGSProjectile* projectile = nullptr;
        WeaponType weaponType = WeaponType::Other;
        AttackType attackType = AttackType::Regular;

        RE::TESForm* hitSourceForm = evn->source ? RE::TESForm::LookupByID(evn->source) : nullptr;

        if (!hitSourceForm) {
            weaponType = WeaponType::Other;
        }

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
                weapon = leftObj ? leftObj->As<RE::TESObjectWEAP>() : nullptr;
            } else if (rightHandAttack) {
                weapon = rightObj ? rightObj->As<RE::TESObjectWEAP>() : nullptr;
            }

            if (weapon) {
                weaponType = GetWeaponType(weapon);
            } else if (hitSourceForm) {
                if (auto* spell = hitSourceForm->As<RE::SpellItem>()) {
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
            case WeaponType::Shout:        weaponTypeStr = "Shout";        break;
            case WeaponType::Ability:      weaponTypeStr = "Ability";      break;
            case WeaponType::LesserPower:  weaponTypeStr = "LesserPower";  break;
            case WeaponType::Power:        weaponTypeStr = "Power";        break;
            case WeaponType::Total:        weaponTypeStr = "Total";        break;
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