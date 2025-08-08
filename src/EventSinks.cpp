#include "EventSinks.h"
#include "PCH.h"
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

using namespace std::chrono;

namespace OIF
{

//░█████╗░░█████╗░███╗░░██╗░██████╗████████╗░█████╗░███╗░░██╗████████╗░██████╗
//██╔══██╗██╔══██╗████╗░██║██╔════╝╚══██╔══╝██╔══██╗████╗░██║╚══██╔══╝██╔════╝
//██║░░╚═╝██║░░██║██╔██╗██║╚█████╗░░░░██║░░░███████║██╔██╗██║░░░██║░░░╚█████╗░
//██║░░██╗██║░░██║██║╚████║░╚═══██╗░░░██║░░░██╔══██║██║╚████║░░░██║░░░░╚═══██╗
//╚█████╔╝╚█████╔╝██║░╚███║██████╔╝░░░██║░░░██║░░██║██║░╚███║░░░██║░░░██████╔╝
//░╚════╝░░╚════╝░╚═╝░░╚══╝╚═════╝░░░░╚═╝░░░╚═╝░░╚═╝╚═╝░░╚══╝░░░╚═╝░░░╚═════╝░                                                                                                              
	
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

	const std::vector<RE::COL_LAYER> targetLayers = {
		RE::COL_LAYER::kAcousticSpace,		   // 21
		RE::COL_LAYER::kActorZone,			   // 22
		RE::COL_LAYER::kAnimStatic,			   // 2
		RE::COL_LAYER::kAvoidBox,			   // 34
		RE::COL_LAYER::kBiped,				   // 8
		RE::COL_LAYER::kBipedNoCC,			   // 33
		RE::COL_LAYER::kCamera,				   // 39
		RE::COL_LAYER::kCameraSphere,		   // 36
		RE::COL_LAYER::kCharController,		   // 30
		RE::COL_LAYER::kCloudTrap,			   // 16
		RE::COL_LAYER::kClutter,			   // 4
		RE::COL_LAYER::kClutterLarge,		   // 29
		RE::COL_LAYER::kCollisionBox,		   // 35
		RE::COL_LAYER::kConeProjectile,		   // 38
		RE::COL_LAYER::kDeadBip,			   // 32
		RE::COL_LAYER::kDebrisLarge,		   // 20
		RE::COL_LAYER::kDebrisSmall,		   // 19
		RE::COL_LAYER::kDoorDetection,		   // 37
		RE::COL_LAYER::kDroppingPick,		   // 46
		RE::COL_LAYER::kGasTrap,			   // 24
		RE::COL_LAYER::kGround,				   // 17
		RE::COL_LAYER::kInvisibleWall,		   // 27
		RE::COL_LAYER::kItemPicker,			   // 40
		RE::COL_LAYER::kLOS,				   // 41
		RE::COL_LAYER::kNonCollidable,		   // 15
		RE::COL_LAYER::kPathingPick,		   // 42
		RE::COL_LAYER::kPortal,				   // 18
		RE::COL_LAYER::kProjectile,			   // 6
		RE::COL_LAYER::kProjectileZone,		   // 23
		RE::COL_LAYER::kProps,				   // 10
		RE::COL_LAYER::kShellCasting,		   // 25
		RE::COL_LAYER::kSpell,				   // 7
		RE::COL_LAYER::kSpellExplosion,		   // 45
		RE::COL_LAYER::kStairHelper,		   // 31
		RE::COL_LAYER::kStatic,				   // 1
		RE::COL_LAYER::kTerrain,			   // 13
		RE::COL_LAYER::kTransparent,		   // 3
		RE::COL_LAYER::kTransparentSmallAnim,  // 28
		RE::COL_LAYER::kTransparentWall,	   // 26
		RE::COL_LAYER::kTrap,				   // 14
		RE::COL_LAYER::kTrees,				   // 9
		RE::COL_LAYER::kTrigger,			   // 12
		RE::COL_LAYER::kUnidentified,		   // 0
		RE::COL_LAYER::kUnused0,			   // 43
		RE::COL_LAYER::kUnused1,			   // 44
		RE::COL_LAYER::kWater,				   // 11
		RE::COL_LAYER::kWeapon				   // 5
	};


//███████╗███╗░░██╗██╗░░░██╗███╗░░░███╗░██████╗
//██╔════╝████╗░██║██║░░░██║████╗░████║██╔════╝
//█████╗░░██╔██╗██║██║░░░██║██╔████╔██║╚█████╗░
//██╔══╝░░██║╚████║██║░░░██║██║╚██╔╝██║░╚═══██╗
//███████╗██║░╚███║╚██████╔╝██║░╚═╝░██║██████╔╝
//╚══════╝╚═╝░░╚══╝░╚═════╝░╚═╝░░░░░╚═╝╚═════╝░                                 

	enum class WeaponType
	{
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


//░██████╗████████╗░█████╗░████████╗██╗░█████╗░░██████╗
//██╔════╝╚══██╔══╝██╔══██╗╚══██╔══╝██║██╔══██╗██╔════╝
//╚█████╗░░░░██║░░░███████║░░░██║░░░██║██║░░╚═╝╚█████╗░
//░╚═══██╗░░░██║░░░██╔══██║░░░██║░░░██║██║░░██╗░╚═══██╗
//██████╔╝░░░██║░░░██║░░██║░░░██║░░░██║╚█████╔╝██████╔╝
//╚═════╝░░░░╚═╝░░░╚═╝░░╚═╝░░░╚═╝░░░╚═╝░╚════╝░╚═════╝░                                 

	static auto startTime = std::chrono::steady_clock::now();


//██╗░░██╗███████╗██╗░░░░░██████╗░███████╗██████╗░░██████╗
//██║░░██║██╔════╝██║░░░░░██╔══██╗██╔════╝██╔══██╗██╔════╝
//███████║█████╗░░██║░░░░░██████╔╝█████╗░░██████╔╝╚█████╗░
//██╔══██║██╔══╝░░██║░░░░░██╔═══╝░██╔══╝░░██╔══██╗░╚═══██╗
//██║░░██║███████╗███████╗██║░░░░░███████╗██║░░██║██████╔╝
//╚═╝░░╚═╝╚══════╝╚══════╝╚═╝░░░░░╚══════╝╚═╝░░╚═╝╚═════╝░

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

    void ScanCell(RE::Actor* source, std::vector<RE::TESObjectREFR*>* foundObjects = nullptr, bool triggerEvents = false, 
				  EventType eventType = EventType::kNone, RE::TESWeather* weather = nullptr, const UpdateFilter* updateFilter = nullptr)
    {
		if (!EventSinkBase::IsActorSafe(source)) return;

        auto* cell = source->GetParentCell();
        if (!cell) return;

        std::size_t processedCount = 0;
        std::size_t skippedCount = 0;

        cell->ForEachReference([&](RE::TESObjectREFR* ref) -> RE::BSContainer::ForEachResult {
            if (!EventSinkBase::IsItemSafe(ref)) return RE::BSContainer::ForEachResult::kContinue;

            if (triggerEvents) {
                if (eventType == EventType::kOnUpdate && updateFilter && !updateFilter->IsEmpty()) {
                    if (!updateFilter->Matches(ref)) {
                        skippedCount++;
                        return RE::BSContainer::ForEachResult::kContinue;
                    }
                }
               
                processedCount++;

				if (!EventSinkBase::IsItemSafe(ref)) return RE::BSContainer::ForEachResult::kContinue;

                RuleContext ctx{
                    eventType,
                    source, 
                    ref,
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
    }

	void HandleProjectileImpact(RE::Projectile* a_proj, const RE::NiPoint3& a_hitPos) 
	{
		if (!a_proj || a_proj->IsDeleted()) return;

		RE::Actor* actor = RE::PlayerCharacter::GetSingleton();
		if (auto actorCause = a_proj->GetProjectileRuntimeData().actorCause) {
			if (auto& actorHandle = actorCause->actor) {
				if (auto actorPtr = actorHandle.get()) {
					if (actorPtr) actor = actorPtr.get();
				}
			}
		}
		if (!EventSinkBase::IsActorSafe(actor)) return;

		RE::TESForm* attackSource = a_proj;
		RE::TESForm* projectileSource = a_proj;
		WeaponType weaponType = WeaponType::Ranged;
		AttackType attackType = AttackType::Regular;
		DeliveryType deliveryType = DeliveryType::None;
		RE::TESObjectCELL* cell = a_proj->GetParentCell();
		
		if (!cell) {
			cell = actor->GetParentCell();
			if (!cell) {
				if (auto* player = RE::PlayerCharacter::GetSingleton()) cell = player->GetParentCell();
			}
			if (!cell) return;
		}

		RE::FormID actorFormID = actor ? actor->GetFormID() : 0;
		RE::FormID attackSourceFormID = attackSource ? attackSource->GetFormID() : 0;
		RE::FormID projectileSourceFormID = projectileSource ? projectileSource->GetFormID() : 0;
		RE::FormID cellFormID = cell ? cell->GetFormID() : 0;
		
		if (auto* projSpell = a_proj->GetProjectileRuntimeData().spell) {
			if (auto* spell = projSpell->As<RE::SpellItem>()) {
				attackSource = spell;
				weaponType = GetSpellType(spell);

				switch (spell->data.castingType) {
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

				switch (spell->data.delivery) {
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
			}
		} else if (auto* projWeapon = a_proj->GetProjectileRuntimeData().weaponSource) {
			attackSource = projWeapon;
			weaponType = GetWeaponType(projWeapon);

			if (auto* actorState = actor->GetActorRuntimeData().currentProcess) {
				if (auto& highData = actorState->high) {
					if (auto& attackData = highData->attackData) {
						attackType = GetAttackType(attackData.get());
					}
				}
			}
		} else if (auto* projExplosion = a_proj->GetProjectileRuntimeData().explosion) {
			attackSource = projExplosion;
			weaponType = WeaponType::Explosion;
		}

		SKSE::GetTaskInterface()->AddTask([a_hitPos, actorFormID, attackSourceFormID, projectileSourceFormID, cellFormID, weaponType, attackType, deliveryType]() {
			RE::Actor* actor = nullptr;
			if (actorFormID != 0) actor = RE::TESForm::LookupByID<RE::Actor>(actorFormID);
			if (!EventSinkBase::IsActorSafe(actor)) return;

			RE::TESForm* attackSource = nullptr;
			if (attackSourceFormID != 0) attackSource = RE::TESForm::LookupByID(attackSourceFormID);

			RE::TESForm* projectileSource = nullptr;
			if (projectileSourceFormID != 0) projectileSource = RE::TESForm::LookupByID(projectileSourceFormID);

			RE::TESObjectCELL* cell = nullptr;
			if (cellFormID != 0) {
				cell = RE::TESForm::LookupByID<RE::TESObjectCELL>(cellFormID);
			} else {
				if (actor) cell = actor->GetParentCell();
				if (!cell) {
					if (auto* player = RE::PlayerCharacter::GetSingleton()) cell = player->GetParentCell();
				}
				if (!cell) return;
			}
			
			cell->ForEachReferenceInRange(a_hitPos, 65.0, [&](RE::TESObjectREFR* ref) -> RE::BSContainer::ForEachResult 
			{
				if (!EventSinkBase::IsItemSafe(ref)) return RE::BSContainer::ForEachResult::kContinue;
				auto* baseObj = ref->GetBaseObject();
				if (!baseObj) return RE::BSContainer::ForEachResult::kContinue;
				
				switch (baseObj->GetFormType()) {
				case RE::FormType::Flora:
				case RE::FormType::Tree:
					break;
				default:
					return RE::BSContainer::ForEachResult::kContinue;
				}

				if (!EventSinkBase::IsItemSafe(ref)) return RE::BSContainer::ForEachResult::kContinue;

				RuleContext ctx{
					EventType::kHit,
					actor,
					ref,
					attackSource,
					projectileSource,
					WeaponTypeToString(weaponType),
					AttackTypeToString(attackType),
					DeliveryTypeToString(deliveryType),
					true
				};
				RuleManager::GetSingleton()->Trigger(ctx);
				
				return RE::BSContainer::ForEachResult::kContinue;
			});
		});
	}


//░██████╗██╗███╗░░██╗██╗░░██╗░██████╗
//██╔════╝██║████╗░██║██║░██╔╝██╔════╝
//╚█████╗░██║██╔██╗██║█████═╝░╚█████╗░
//░╚═══██╗██║██║╚████║██╔═██╗░░╚═══██╗
//██████╔╝██║██║░╚███║██║░╚██╗██████╔╝
//╚═════╝░╚═╝╚═╝░░╚══╝╚═╝░░╚═╝╚═════╝░                           
    
	RE::BSEventNotifyControl ActivateSink::ProcessEvent(const RE::TESActivateEvent* evn, RE::BSTEventSource<RE::TESActivateEvent>*)
    {
        if (!evn || !evn->objectActivated) return RE::BSEventNotifyControl::kContinue;

        auto targetRef = evn->objectActivated;
		if (!EventSinkBase::IsItemSafe(targetRef.get())) return RE::BSEventNotifyControl::kContinue;

        auto* baseObj = targetRef->GetBaseObject();
        if (!baseObj) return RE::BSEventNotifyControl::kContinue;
        
        RE::Actor* source = nullptr;
		if (evn->actionRef && evn->actionRef.get()) source = evn->actionRef.get()->As<RE::Actor>();
        if (!EventSinkBase::IsActorSafe(source)) return RE::BSEventNotifyControl::kContinue;

        RE::FormID sourceID = source->GetFormID();
        RE::FormID targetID = targetRef->GetFormID();

        SKSE::GetTaskInterface()->AddTask([sourceID, targetID]() {
            auto* source = RE::TESForm::LookupByID<RE::Actor>(sourceID);
            auto* targetRef = RE::TESForm::LookupByID<RE::TESObjectREFR>(targetID);
            
            if (!EventSinkBase::IsActorSafe(source) || !EventSinkBase::IsItemSafe(targetRef)) return;
            
            RuleContext ctx{ 
                EventType::kActivate, 
                source, 
                targetRef, 
                targetRef->GetBaseObject()
            };
            RuleManager::GetSingleton()->Trigger(ctx);
        });

        return RE::BSEventNotifyControl::kContinue;
    }

    RE::BSEventNotifyControl HitSink::ProcessEvent(const RE::TESHitEvent* evn, RE::BSTEventSource<RE::TESHitEvent>*)
    {
        if (!evn || !evn->target) return RE::BSEventNotifyControl::kContinue;

        auto targetRef = evn->target;
        if (!EventSinkBase::IsItemSafe(targetRef.get())) return RE::BSEventNotifyControl::kContinue;

        auto* baseObj = targetRef->GetBaseObject();
        if (!baseObj) return RE::BSEventNotifyControl::kContinue;
        
        RE::Actor* source = nullptr;
        if (evn->cause && evn->cause.get()) source = evn->cause.get()->As<RE::Actor>();
        if (!EventSinkBase::IsActorSafe(source)) return RE::BSEventNotifyControl::kContinue;

        RE::FormID hitSourceID = evn->source;
        RE::FormID projectileID = evn->projectile;
        RE::FormID sourceID = source->GetFormID();
        RE::FormID targetID = targetRef->GetFormID();

        SKSE::GetTaskInterface()->AddTask([hitSourceID, projectileID, sourceID, targetID]() {
            auto* source = RE::TESForm::LookupByID<RE::Actor>(sourceID);
            auto* targetRef = RE::TESForm::LookupByID<RE::TESObjectREFR>(targetID);
            auto* hitSourceForm = hitSourceID ? RE::TESForm::LookupByID(hitSourceID) : nullptr;
            auto* projectileForm = projectileID ? RE::TESForm::LookupByID<RE::BGSProjectile>(projectileID) : nullptr;

            if (!EventSinkBase::IsActorSafe(source) || !EventSinkBase::IsItemSafe(targetRef)) return;

            RE::TESForm* attackSource = nullptr;
            RE::TESForm* projectileSource = nullptr;
            WeaponType weaponType = WeaponType::Other;
            AttackType attackType = AttackType::Regular;
            DeliveryType deliveryType = DeliveryType::None;

            if (hitSourceForm && hitSourceForm->As<RE::BGSExplosion>()) return;

            if (auto* actorState = source->GetActorRuntimeData().currentProcess) {
                auto* highData = actorState->high;
                if (!highData) return;

                if (highData->attackData) attackType = GetAttackType(highData->attackData.get());

                if (highData->muzzleFlash && highData->muzzleFlash->baseProjectile) {
                    auto* projectile = highData->muzzleFlash->baseProjectile;
                    if (projectile->As<RE::BGSExplosion>()) return;
					weaponType = WeaponType::Ranged;
                    attackSource = projectile;
                    projectileSource = projectile;
                }
            }

            if (projectileForm) {
                if (projectileForm->As<RE::BGSExplosion>()) return;
				weaponType = WeaponType::Ranged;
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

			if (!EventSinkBase::IsActorSafe(source) || !EventSinkBase::IsItemSafe(targetRef)) return;

            RuleContext ctx{ 
                EventType::kHit,
                source,
                targetRef, 
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
        if (!evn || !evn->target) return RE::BSEventNotifyControl::kContinue;
    
        auto targetRef = evn->target;
        if (!EventSinkBase::IsItemSafe(targetRef.get())) return RE::BSEventNotifyControl::kContinue;
    
        auto* baseObj = targetRef->GetBaseObject();
        if (!baseObj) return RE::BSEventNotifyControl::kContinue;
    
        RE::FormID targetID = 0;
        RE::FormID magicEffectID = 0;
        RE::FormID casterID = 0;
    
        if (targetRef) targetID = targetRef->GetFormID();

        if (evn->magicEffect) magicEffectID = evn->magicEffect;
    
        if (evn->caster) {
            auto* casterRef = evn->caster.get();
			if (auto* casterActor = casterRef->As<RE::Actor>()) {
				if (EventSinkBase::IsActorSafe(casterActor)) {
					casterID = casterRef->GetFormID();
				}
			}
        }
    
        if (targetID == 0) return RE::BSEventNotifyControl::kContinue;
        if (casterID == 0) return RE::BSEventNotifyControl::kContinue;
        if (magicEffectID == 0) return RE::BSEventNotifyControl::kContinue;
    
        SKSE::GetTaskInterface()->AddTask([targetID, magicEffectID, casterID]() {
            auto* targetRef = RE::TESForm::LookupByID<RE::TESObjectREFR>(targetID);
            auto* magicEffect = RE::TESForm::LookupByID<RE::EffectSetting>(magicEffectID);
            auto* caster = RE::TESForm::LookupByID<RE::Actor>(casterID);
            
            if (!EventSinkBase::IsItemSafe(targetRef)) return;
            if (!magicEffect) return;
			if (!EventSinkBase::IsActorSafe(caster)) return;
    
            RE::TESForm* attackSource = magicEffect;
            RE::TESForm* projectileSource = nullptr;
            WeaponType weaponType = WeaponType::Other;
            AttackType attackType = AttackType::Regular;
            DeliveryType deliveryType = DeliveryType::None;
            bool isShout = false;
            bool isSpell = false;

            if (magicEffect->data.projectileBase) {
                if (!magicEffect->data.projectileBase->As<RE::BGSExplosion>()) {
					weaponType = WeaponType::Ranged;
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

			if (!EventSinkBase::IsActorSafe(caster) || !EventSinkBase::IsItemSafe(targetRef)) return;

            RuleContext ctx{
                EventType::kHit,
                caster,
                targetRef,
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

    RE::BSEventNotifyControl GrabReleaseSink::ProcessEvent(const RE::TESGrabReleaseEvent* evn, RE::BSTEventSource<RE::TESGrabReleaseEvent>*)
    {
        if (!evn || !evn->ref) return RE::BSEventNotifyControl::kContinue;

        auto targetRef = evn->ref;
		if (!EventSinkBase::IsItemSafe(targetRef.get())) return RE::BSEventNotifyControl::kContinue;

        RE::Actor* source = nullptr;
        if (auto* handle = RE::PlayerCharacter::GetSingleton()) source = handle->As<RE::Actor>();

        if (!EventSinkBase::IsActorSafe(source)) return RE::BSEventNotifyControl::kContinue;

        bool isGrabbed = evn->grabbed;

        if (isGrabbed) {
            SKSE::GetTaskInterface()->AddTask([source, targetRef]() {
				if (EventSinkBase::IsActorSafe(source) && EventSinkBase::IsItemSafe(targetRef.get())) {   
                    RuleContext ctx{
                        EventType::kGrab,
                        source,
						targetRef.get(),
						targetRef->GetBaseObject()
                    };
                    RuleManager::GetSingleton()->Trigger(ctx);
                }
            });
        } else {
            SKSE::GetTaskInterface()->AddTask([source, targetRef]() {
				if (EventSinkBase::IsActorSafe(source) && EventSinkBase::IsItemSafe(targetRef.get())) {
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
							targetRef.get(),
							targetRef->GetBaseObject()
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
		bool isDropped = false;

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

		else if (bodyA && bodyA->HasProperty(HK_PROPERTY_DROPPEDOBJECT)) {
			specialBody = bodyA;
			isDropped = true;
		} else if (bodyB && bodyB->HasProperty(HK_PROPERTY_DROPPEDOBJECT)) {
			specialBody = bodyB;
			isDropped = true;
		}
        
        if (specialBody) {
            auto refr = specialBody->GetUserData();
            if (!refr) return;
            
            std::uint32_t objectID = refr->GetFormID();
            if (!objectID) return;

            if (processedObjects.find(objectID) != processedObjects.end()) return;

            processedObjects.insert(objectID);
            bodiesToCleanup.push_back(specialBody);

			SKSE::GetTaskInterface()->AddTask([refr, objectID, isTelekinesis, isThrown, isDropped, this]() {
				if (EventSinkBase::IsItemSafe(refr) && objectID) {
					EventType eventType = EventType::kNone;
					if (isTelekinesis) {
						eventType = EventType::kTelekinesis;
					} else if (isThrown) {
						eventType = EventType::kThrow;
					} else if (isDropped) {
						eventType = EventType::kDrop;
					}

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
						if (body->HasProperty(HK_PROPERTY_DROPPEDOBJECT)) {
							body->RemoveProperty(HK_PROPERTY_DROPPEDOBJECT);
						}
                        body->RemoveContactListener(this);
                    }
                }
                bodiesToCleanup.clear();
            });
        }
    }

	RE::BSEventNotifyControl CellAttachDetachSink::ProcessEvent(const RE::TESCellAttachDetachEvent* evn, RE::BSTEventSource<RE::TESCellAttachDetachEvent>*)
	{
		if (!evn || !evn->reference) return RE::BSEventNotifyControl::kContinue;

		auto targetRef = evn->reference;
		bool attached = evn->attached;

		if (!EventSinkBase::IsItemSafe(targetRef.get())) return RE::BSEventNotifyControl::kContinue;

		SKSE::GetTaskInterface()->AddTask([targetRef, attached]() {
			if (EventSinkBase::IsItemSafe(targetRef.get())) {
				if (attached) {
					RuleContext ctx{
						EventType::kCellAttach,
						RE::PlayerCharacter::GetSingleton()->As<RE::Actor>(),
						targetRef.get(),
						targetRef->GetBaseObject()
					};
					RuleManager::GetSingleton()->Trigger(ctx);
				} else {
					RuleContext ctx{
						EventType::kCellDetach,
						RE::PlayerCharacter::GetSingleton()->As<RE::Actor>(),
						targetRef.get(),
						targetRef->GetBaseObject()
					};
					RuleManager::GetSingleton()->Trigger(ctx);
				}
			}
		});

		return RE::BSEventNotifyControl::kContinue;
	}

	RE::BSEventNotifyControl DestructionStageChangedSink::ProcessEvent(const RE::TESDestructionStageChangedEvent* evn, RE::BSTEventSource<RE::TESDestructionStageChangedEvent>*)
	{
		if (!evn || !evn->target) return RE::BSEventNotifyControl::kContinue;

		auto targetRef = evn->target;
		if (!EventSinkBase::IsItemSafe(targetRef.get())) return RE::BSEventNotifyControl::kContinue;

		std::int32_t stage = evn->newStage;

		SKSE::GetTaskInterface()->AddTask([targetRef, stage]() {
			if (EventSinkBase::IsItemSafe(targetRef.get())) {
				RuleContext ctx{
					EventType::kDestructionStageChange,
					RE::PlayerCharacter::GetSingleton()->As<RE::Actor>(),
					targetRef.get(),
					nullptr,
					nullptr,
					"",
					"",
					"",
					true,
					nullptr,
					stage
				};
				RuleManager::GetSingleton()->Trigger(ctx);
			}
		});

		return RE::BSEventNotifyControl::kContinue;
	}

	// Still in development, target object's ref cannot be obtained directly or through actor's ExtraDroppedItemList
	/*RE::BSEventNotifyControl DropSink::ProcessEvent(const RE::TESContainerChangedEvent* evn, RE::BSTEventSource<RE::TESContainerChangedEvent>*)
	{
		logger::error("DropSink::ProcessEvent called");
		if (!evn || !evn->baseObj) return RE::BSEventNotifyControl::kContinue;

		RE::TESForm* baseObjForm = RE::TESForm::LookupByID(evn->baseObj);
		if (!baseObjForm) return RE::BSEventNotifyControl::kContinue;
		if (!EventSinkBase::IsRelevantObject(baseObjForm)) return RE::BSEventNotifyControl::kContinue;
		logger::error("DropSink::ProcessEvent called with baseObj: {}", baseObjForm->GetFormID());

		// From container to world
		if (evn->oldContainer == 0 || evn->newContainer != 0) return RE::BSEventNotifyControl::kContinue;
		logger::error("DropSink::ProcessEvent called with oldContainer: {}, newContainer: {}", evn->oldContainer, evn->newContainer);

		auto actor = RE::TESForm::LookupByID<RE::Actor>(evn->oldContainer);
		if (!actor) return RE::BSEventNotifyControl::kContinue;	 // Not dropped by actor
		logger::error("Old container is an actor");

		const auto eventCopy = evn;
		static std::vector<std::future<void>> runningTasks;
		static std::mutex tasksMutex;
		auto future = std::async(std::launch::async, [eventCopy, actorHandle = actor->CreateRefHandle()]() {
			const int checkIntervalMs = 100;
			RE::TESObjectREFR* targetRef = nullptr;

			// Wait for dropped item list to be populated and find the ref (evn->reference is always null)
			while (true) {
				if (auto actorPtr = actorHandle.get()) {
					logger::error("actorhandle exists");
					if (auto dropped = actorPtr->extraList.GetByType<RE::ExtraDroppedItemList>()) {
						logger::error("extralist exists");
						for (auto& handle : dropped->droppedItemList) {
							if (auto refPtr = handle.get()) {
								logger::error("refptr exists");
								if (auto* ref = refPtr.get()) {
									logger::error("ref exists");
									if (ref->GetBaseObject()->GetFormID() == eventCopy->baseObj) {
										targetRef = ref;
										break;
									}
								}
							}
						}
					}
				}
				if (targetRef) break;
				std::this_thread::sleep_for(std::chrono::milliseconds(checkIntervalMs));
			}

			if (!targetRef || targetRef->IsDeleted() || targetRef->IsDisabled()) return;
			logger::error("DropSink::ProcessEvent called with relevant object reference: {}", targetRef->GetFormID());

			auto* baseObj = targetRef->GetBaseObject();
			if (!baseObj) return;

			// Wait for 3D object and physics to be initialized to add the contact listener (so the effect plays upon landing)
			while (true) {
				if (!targetRef || targetRef->IsDeleted()) return;
				std::atomic<bool> taskCompleted{ false };
				std::atomic<bool> success{ false };
				SKSE::GetTaskInterface()->AddTask([targetRef, baseObj, &taskCompleted, &success]() {
					if (targetRef && !targetRef->IsDeleted()) {
						if (auto threedimObj = targetRef->Get3D()) {
							if (auto collisionObj = threedimObj->GetCollisionObject()) {
								if (auto bhkBody = collisionObj->GetRigidBody()) {
									if (auto hkpBody = bhkBody->GetRigidBody()) {
										int propertyId = 271828;
										float now = std::chrono::duration_cast<std::chrono::duration<float>>(std::chrono::steady_clock::now() - startTime).count();
										hkpBody->SetProperty(propertyId, now);
										hkpBody->AddContactListener(LandingSink::GetSingleton());
										success = true;
									}
								}
							}
						}
					}
					taskCompleted = true;
				});
				while (!taskCompleted) {
					std::this_thread::sleep_for(std::chrono::milliseconds(10));
				}
				if (success) break;
				std::this_thread::sleep_for(std::chrono::milliseconds(checkIntervalMs));
			}
		});
		{
			std::lock_guard<std::mutex> lock(tasksMutex);
			runningTasks.push_back(std::move(future));
			runningTasks.erase(
				std::remove_if(runningTasks.begin(), runningTasks.end(),
					[](const std::future<void>& f) {
						return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
					}),
				runningTasks.end());
		}
		return RE::BSEventNotifyControl::kContinue;
	}*/


//██╗░░██╗░█████╗░░█████╗░██╗░░██╗░██████╗
//██║░░██║██╔══██╗██╔══██╗██║░██╔╝██╔════╝
//███████║██║░░██║██║░░██║█████═╝░╚█████╗░
//██╔══██║██║░░██║██║░░██║██╔═██╗░░╚═══██╗
//██║░░██║╚█████╔╝╚█████╔╝██║░╚██╗██████╔╝
//╚═╝░░╚═╝░╚════╝░░╚════╝░╚═╝░░╚═╝╚═════╝░                              
    
	void ExplosionHook::thunk(RE::Explosion* a_this)
	{
		func(a_this);
		if (!EventSinkBase::IsItemSafe(a_this)) return;

		auto* baseObj = a_this->GetBaseObject();
		auto explosionPos = a_this->GetPosition();
		auto* cell = a_this->GetParentCell();
		auto& runtimeData = a_this->GetExplosionRuntimeData();

		if (runtimeData.damage <= 0.0f) return;
		float explosionRadius = runtimeData.radius;
		if (explosionRadius <= 0.0f) return;

		if (!baseObj || !cell) return;

		RE::Actor* actor = RE::PlayerCharacter::GetSingleton();
		if (auto actorCause = runtimeData.actorCause) {
			if (auto& actorHandle = actorCause->actor) {
				if (auto actorPtr = actorHandle.get()) {
					if (actorPtr)
						actor = actorPtr.get();
				}
			}
		}
		if (!EventSinkBase::IsActorSafe(actor)) return;

		RE::TESForm* attackSource = a_this;

		RE::FormID actorFormID = actor ? actor->GetFormID() : 0;
		RE::FormID attackSourceFormID = attackSource ? attackSource->GetFormID() : 0;
		RE::FormID cellFormID = cell ? cell->GetFormID() : 0;

		SKSE::GetTaskInterface()->AddTask([actorFormID, attackSourceFormID, cellFormID, explosionPos, explosionRadius]() 
		{
			RE::Actor* actor = nullptr;
			if (actorFormID != 0) actor = RE::TESForm::LookupByID<RE::Actor>(actorFormID);
			if (!EventSinkBase::IsActorSafe(actor)) return;

			RE::TESForm* attackSource = nullptr;
			if (attackSourceFormID != 0) attackSource = RE::TESForm::LookupByID(attackSourceFormID);

			RE::TESObjectCELL* cell = nullptr;
			if (cellFormID != 0) {
				cell = RE::TESForm::LookupByID<RE::TESObjectCELL>(cellFormID);
			} else {
				if (actor) cell = actor->GetParentCell();
				if (!cell) {
					if (auto* player = RE::PlayerCharacter::GetSingleton()) cell = player->GetParentCell();
				}
				if (!cell) return;
			}

			cell->ForEachReferenceInRange(explosionPos, explosionRadius, [&](RE::TESObjectREFR* ref) -> RE::BSContainer::ForEachResult 
			{
				if (!EventSinkBase::IsItemSafe(ref)) return RE::BSContainer::ForEachResult::kContinue;

				RuleContext ctx{
					EventType::kHit,
					actor,
					ref,
					attackSource,
					nullptr,
					WeaponTypeToString(WeaponType::Explosion),
					AttackTypeToString(AttackType::Regular),
					DeliveryTypeToString(DeliveryType::None),
					true
				};

				RuleManager::GetSingleton()->Trigger(ctx);

				return RE::BSContainer::ForEachResult::kContinue;
			});
		});
	}

    void ReadyWeaponHook::thunk(RE::ReadyWeaponHandler* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data)
    {
		func(a_this, a_event, a_data);

		if (a_event) {
			if (a_event->IsUp()) {
				InputHandler::GetSingleton()->SetKeyJustReleased();
			}
		}
    }

    void WeatherChangeHook::thunk(RE::TESRegion* a_region, RE::TESWeather* a_currentWeather)
    {
		func(a_region, a_currentWeather);
        
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
        if (!EventSinkBase::IsActorSafe(a_this)) return;

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

	void AttackBlockHook::thunk(RE::AttackBlockHandler* a_this, RE::ButtonEvent* a_event, RE::PlayerControlsData* a_data)
	{
		func(a_this, a_event, a_data);
		
		if (a_event) {
			if (!a_event->IsPressed() || a_event->IsHeld()) return;
		}

		auto* player = RE::PlayerCharacter::GetSingleton();
		if (!EventSinkBase::IsActorSafe(player)) return;

		bool isWeaponDrawn = player->AsActorState()->IsWeaponDrawn();
		if (!isWeaponDrawn) return;

		auto* playerNode = player->Get3D();
		if (!playerNode) return;

		RE::TESForm* attackSource = nullptr;
		WeaponType weaponType = WeaponType::Other;
		AttackType attackType = AttackType::Regular;
		DeliveryType deliveryType = DeliveryType::None;
		bool isLeftAttack = false;
		
		// meleeRange = fCombatDistance * AttackerScale * WeaponReach + fObjectHit
		// WeaponReach + fObjectHit calculations are not included anymore, the default range suits the task already
		float reach = 141.0f * player->GetScale();

		auto& actorState = player->GetActorRuntimeData().currentProcess;
		if (!actorState) return;

		auto* highData = actorState->high;
		if (!highData) return;

		if (auto& attackData = highData->attackData) {
			if (!attackData) return;

			attackType = GetAttackType(attackData.get());

			if (highData->muzzleFlash && highData->muzzleFlash->baseProjectile) {
				// If the attack is a projectile, let the ProjectileImpactHook handle it
				return;
			} else if (auto* spell = attackData->data.attackSpell) {
				attackSource = spell;
				weaponType = GetSpellType(spell);

				switch (spell->data.castingType) {
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

				switch (spell->data.delivery) {
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
			} else {
				isLeftAttack = attackData->IsLeftAttack();
				if (isLeftAttack) {
					attackSource = actorState->GetEquippedLeftHand();
				} else {
					attackSource = actorState->GetEquippedRightHand();
				}
				if (!attackSource) return;

				if (auto* weapon = attackSource->As<RE::TESObjectWEAP>()) {
					weaponType = GetWeaponType(weapon);
				}
			}
		}

		auto* cam = RE::PlayerCamera::GetSingleton();
		if (!cam || !cam->currentState || !cam->currentState->camera) return;

		RE::NiPoint3 start = player->GetPosition();
		RE::NiPoint3 dir = playerNode->world.rotate * RE::NiPoint3{ 0.f, 1.f, 0.f };
		dir.z = 0.f;
		if (dir.SqrLength() < 1e-6f) {
			dir = { 0.f, 1.f, 0.f };
		} else {
			dir.Unitize();
		}
		RE::NiPoint3 end = start + dir * reach;
		
		float foundGoodPosition = false;

		auto crosshair = RE::CrosshairPickData::GetSingleton();
		if (crosshair) {
			if (crosshair->target && crosshair->target->get()) {
				if (auto* ref = crosshair->target->get().get()) {
					if (EventSinkBase::IsItemSafe(ref)) {
						if (auto* baseObj = ref->GetBaseObject()) {
							switch (baseObj->GetFormType()) {
							case RE::FormType::Flora:
							case RE::FormType::Tree:
								end = ref->GetPosition();
								foundGoodPosition = true;
								logger::warn("Found good position for crosshair target: {}", ref->GetFormID());
								break;
							default:
								break;
							}
						}
					}
				}
			}
		}

		auto* cell = player->GetParentCell();
		if (!cell) return;

		std::vector<RE::TESObjectREFR*> validObjects;

		cell->ForEachReferenceInRange(end, 65.0, [&](RE::TESObjectREFR* ref) 
		{
			if (!EventSinkBase::IsItemSafe(ref)) return RE::BSContainer::ForEachResult::kContinue;
			auto* baseObj = ref->GetBaseObject();
			if (!baseObj) return RE::BSContainer::ForEachResult::kContinue;
			
			switch (baseObj->GetFormType()) {
			case RE::FormType::Flora:
			case RE::FormType::Tree:
				break;
			default:
				return RE::BSContainer::ForEachResult::kContinue;
			}

			bool a_arg2 = false;
			if (!player->HasLineOfSight(ref, a_arg2)) return RE::BSContainer::ForEachResult::kContinue;

			validObjects.emplace_back(ref);

			return RE::BSContainer::ForEachResult::kContinue;
		});

		if (validObjects.empty()) return;

		static std::vector<std::future<void>> runningTasks;
		static std::mutex tasksMutex;

		// Wait for the animation to finish before triggering the rule (approximate duration)
		auto future = std::async(std::launch::async, [validObjects, player, attackSource, weaponType, attackType, deliveryType, duration = 0.6]() {
			std::this_thread::sleep_for(std::chrono::duration<float>(duration));

			SKSE::GetTaskInterface()->AddTask([validObjects, player, attackSource, weaponType, attackType, deliveryType]() {
				for (auto& ref : validObjects) {
					if (EventSinkBase::IsItemSafe(ref)) {
						RuleContext ctx{
							EventType::kHit,
							player->As<RE::Actor>(),
							ref,
							attackSource,
							nullptr,
							WeaponTypeToString(weaponType),
							AttackTypeToString(attackType),
							DeliveryTypeToString(deliveryType),
							true
						};
						RuleManager::GetSingleton()->Trigger(ctx);
					}
				}
			});
		});

		{
			std::lock_guard<std::mutex> lock(tasksMutex);
			runningTasks.push_back(std::move(future));

			runningTasks.erase(
				std::remove_if(runningTasks.begin(), runningTasks.end(),
					[](const std::future<void>& f) {
						return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
					}),
				runningTasks.end());
		}
	}

	void MissileImpact::thunk(RE::Projectile* a_proj, RE::TESObjectREFR* a_ref, const RE::NiPoint3& a_hitPos,
							  const RE::NiPoint3& a_velocity, RE::hkpCollidable* a_collidable,
							  std::int32_t a_arg6, std::uint32_t a_arg7)
	{
		func(a_proj, a_ref, a_hitPos, a_velocity, a_collidable, a_arg6, a_arg7);
		HandleProjectileImpact(a_proj, a_hitPos);
	}

	void BeamImpact::thunk(RE::Projectile* a_proj, RE::TESObjectREFR* a_ref, const RE::NiPoint3& a_hitPos,
						   const RE::NiPoint3& a_velocity, RE::hkpCollidable* a_collidable,
						   std::int32_t a_arg6, std::uint32_t a_arg7)
	{
		func(a_proj, a_ref, a_hitPos, a_velocity, a_collidable, a_arg6, a_arg7);
		HandleProjectileImpact(a_proj, a_hitPos);
	}

	void FlameImpact::thunk(RE::Projectile* a_proj, RE::TESObjectREFR* a_ref, const RE::NiPoint3& a_hitPos,
							const RE::NiPoint3& a_velocity, RE::hkpCollidable* a_collidable,
							std::int32_t a_arg6, std::uint32_t a_arg7)
	{
		func(a_proj, a_ref, a_hitPos, a_velocity, a_collidable, a_arg6, a_arg7);
		HandleProjectileImpact(a_proj, a_hitPos);
	}

	void GrenadeImpact::thunk(RE::Projectile* a_proj, RE::TESObjectREFR* a_ref, const RE::NiPoint3& a_hitPos,
							  const RE::NiPoint3& a_velocity, RE::hkpCollidable* a_collidable,
							  std::int32_t a_arg6, std::uint32_t a_arg7)
	{
		func(a_proj, a_ref, a_hitPos, a_velocity, a_collidable, a_arg6, a_arg7);
		HandleProjectileImpact(a_proj, a_hitPos);
	}

	void ConeImpact::thunk(RE::Projectile* a_proj, RE::TESObjectREFR* a_ref, const RE::NiPoint3& a_hitPos,
						   const RE::NiPoint3& a_velocity, RE::hkpCollidable* a_collidable,
						   std::int32_t a_arg6, std::uint32_t a_arg7)
	{
		func(a_proj, a_ref, a_hitPos, a_velocity, a_collidable, a_arg6, a_arg7);
		HandleProjectileImpact(a_proj, a_hitPos);
	}

	void ArrowImpact::thunk(RE::Projectile* a_proj, RE::TESObjectREFR* a_ref, const RE::NiPoint3& a_hitPos,
							const RE::NiPoint3& a_velocity, RE::hkpCollidable* a_collidable,
							std::int32_t a_arg6, std::uint32_t a_arg7)
	{
		func(a_proj, a_ref, a_hitPos, a_velocity, a_collidable, a_arg6, a_arg7);
		HandleProjectileImpact(a_proj, a_hitPos);
	}


//██████╗░███████╗░██████╗░██╗░██████╗████████╗██████╗░░█████╗░████████╗██╗░█████╗░███╗░░██╗
//██╔══██╗██╔════╝██╔════╝░██║██╔════╝╚══██╔══╝██╔══██╗██╔══██╗╚══██╔══╝██║██╔══██╗████╗░██║
//██████╔╝█████╗░░██║░░██╗░██║╚█████╗░░░░██║░░░██████╔╝███████║░░░██║░░░██║██║░░██║██╔██╗██║
//██╔══██╗██╔══╝░░██║░░╚██╗██║░╚═══██╗░░░██║░░░██╔══██╗██╔══██║░░░██║░░░██║██║░░██║██║╚████║
//██║░░██║███████╗╚██████╔╝██║██████╔╝░░░██║░░░██║░░██║██║░░██║░░░██║░░░██║╚█████╔╝██║░╚███║
//╚═╝░░╚═╝╚══════╝░╚═════╝░╚═╝╚═════╝░░░░╚═╝░░░╚═╝░░╚═╝╚═╝░░╚═╝░░░╚═╝░░░╚═╝░╚════╝░╚═╝░░╚══╝                                  

    void RegisterSinks()
    {
        auto holder = RE::ScriptEventSourceHolder::GetSingleton();
        holder->GetEventSource<RE::TESActivateEvent>()->AddEventSink(ActivateSink::GetSingleton());
        holder->GetEventSource<RE::TESHitEvent>()->AddEventSink(HitSink::GetSingleton());
        holder->GetEventSource<RE::TESGrabReleaseEvent>()->AddEventSink(GrabReleaseSink::GetSingleton());
        holder->GetEventSource<RE::TESCellAttachDetachEvent>()->AddEventSink(CellAttachDetachSink::GetSingleton());
        holder->GetEventSource<RE::TESMagicEffectApplyEvent>()->AddEventSink(MagicEffectApplySink::GetSingleton());
        holder->GetEventSource<RE::TESDestructionStageChangedEvent>()->AddEventSink(DestructionStageChangedSink::GetSingleton());
		//holder->GetEventSource<RE::TESContainerChangedEvent>()->AddEventSink(DropSink::GetSingleton());
    }

    void InstallHooks() 
    {
        REL::Relocation<std::uintptr_t> weatherChangeHook{ REL::VariantID(25684, 26231, 25684), REL::VariantOffset(0x44F, 0x46C, 0x44F) }; // Taken from the Rain Extinguishes Fires source code
        ::stl::write_thunk_call<WeatherChangeHook>(weatherChangeHook.address());
        ::stl::write_vfunc<RE::ReadyWeaponHandler, ReadyWeaponHook>();
        ::stl::write_vfunc<RE::Explosion, ExplosionHook>();
        ::stl::write_vfunc<RE::PlayerCharacter, UpdateHook>();
		::stl::write_vfunc<RE::AttackBlockHandler, AttackBlockHook>();
		MissileImpact::func = REL::Relocation<std::uintptr_t>(RE::MissileProjectile::VTABLE[0]).write_vfunc(0xBD, MissileImpact::thunk);
		BeamImpact::func = REL::Relocation<std::uintptr_t>(RE::BeamProjectile::VTABLE[0]).write_vfunc(0xBD, BeamImpact::thunk);
		FlameImpact::func = REL::Relocation<std::uintptr_t>(RE::FlameProjectile::VTABLE[0]).write_vfunc(0xBD, FlameImpact::thunk);
		GrenadeImpact::func = REL::Relocation<std::uintptr_t>(RE::GrenadeProjectile::VTABLE[0]).write_vfunc(0xBD, GrenadeImpact::thunk);
		ConeImpact::func = REL::Relocation<std::uintptr_t>(RE::ConeProjectile::VTABLE[0]).write_vfunc(0xBD, ConeImpact::thunk);
		ArrowImpact::func = REL::Relocation<std::uintptr_t>(RE::ArrowProjectile::VTABLE[0]).write_vfunc(0xBD, ArrowImpact::thunk);
    }
}
