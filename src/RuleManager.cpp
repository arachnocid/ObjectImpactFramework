#include "RuleManager.h"
#include "Effects.h"
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace OIF {

//██╗░░██╗███████╗██╗░░░░░██████╗░███████╗██████╗░░██████╗
//██║░░██║██╔════╝██║░░░░░██╔══██╗██╔════╝██╔══██╗██╔════╝
//███████║█████╗░░██║░░░░░██████╔╝█████╗░░██████╔╝╚█████╗░
//██╔══██║██╔══╝░░██║░░░░░██╔═══╝░██╔══╝░░██╔══██╗░╚═══██╗
//██║░░██║███████╗███████╗██║░░░░░███████╗██║░░██║██████╔╝
//╚═╝░░╚═╝╚══════╝╚══════╝╚═╝░░░░░╚══════╝╚═╝░░╚═╝╚═════╝░

// ╔════════════════════════════════════╗
// ║      CASE-INSENSITIVE HELPERS      ║
// ╚════════════════════════════════════╝

	inline std::string tolower_str(std::string_view a_str)
	{
		std::string result(a_str);
		std::ranges::transform(result, result.begin(), [](unsigned char ch) {
			return static_cast<unsigned char>(std::tolower(ch));
		});
		return result;
	}

	json lower_keys(const json& j)
	{
		if (j.is_object()) {
			json res = json::object();
			for (auto it = j.begin(); it != j.end(); ++it)
				res[tolower_str(it.key())] = lower_keys(it.value());
			return res;
		} else if (j.is_array()) {
			json arr = json::array();
			for (const auto& el : j)
				arr.push_back(lower_keys(el));
			return arr;
		}
		return j;
	}

// ╔════════════════════════════════════╗
// ║       STRING MAPPING HELPERS       ║
// ╚════════════════════════════════════╝

    static const std::unordered_map<std::string_view, RE::FormType> formTypeMap = {
        {"activator", RE::FormType::Activator},
        {"talkingactivator", RE::FormType::TalkingActivator},
        {"weapon", RE::FormType::Weapon},
        {"armor", RE::FormType::Armor},
        {"ammo", RE::FormType::Ammo},
        {"ingredient", RE::FormType::Ingredient},
        {"misc", RE::FormType::Misc},
        {"book", RE::FormType::Book},
        {"note", RE::FormType::Note},
        {"scroll", RE::FormType::Scroll},
        {"soulgem", RE::FormType::SoulGem},
        {"potion", RE::FormType::AlchemyItem},
        {"furniture", RE::FormType::Furniture},
        {"door", RE::FormType::Door},
        {"flora", RE::FormType::Flora},
        {"container", RE::FormType::Container},
        {"static", RE::FormType::Static},
        {"moveablestatic", RE::FormType::MovableStatic},
        {"tree", RE::FormType::Tree},
        {"key", RE::FormType::KeyMaster},
        {"light", RE::FormType::Light}
    };

    static RE::FormType MapStringToFormType(std::string_view s) {
        auto it = formTypeMap.find(s);
        return it != formTypeMap.end() ? it->second : RE::FormType::None;
    }

    static std::string MapAttackTypeToString(std::string_view s) {
        static const std::unordered_set<std::string_view> validAttackTypes = {
            "power", "bash", "charge", "rotating", "continuous", "constant", 
            "fireandforget", "ignoreweapon", "overridedata"
        };
        return validAttackTypes.contains(s) ? std::string(s) : "regular";
    }

    static std::string MapWeaponTypeToString(std::string_view s) {
        static const std::unordered_set<std::string_view> validWeaponTypes = {
            "onehandsword", "twohandsword", "onehandaxe", "twohandaxe",
            "onehandmace", "twohandmace", "dagger", "ranged", "staff",
            "spell", "scroll", "shout", "ability", "lesserpower", "power",
            "explosion", "handtohand", "total"
        };
        return validWeaponTypes.contains(s) ? std::string(s) : "other";
    }

    static std::string MapDeliveryTypeToString(std::string_view s) {
        static const std::unordered_set<std::string_view> validDeliveryTypes = {
            "self", "aimed", "targetactor", "targetlocation", "touch", "total"
        };
        return validDeliveryTypes.contains(s) ? std::string(s) : "none";
    }

    RE::ActorValue GetActorValueFromString(const std::string& avName) {
        std::string lowerName = tolower_str(avName);
        if (lowerName == "absorbchance") return RE::ActorValue::kAbsorbChance;
        if (lowerName == "aggression") return RE::ActorValue::kAggression;
        if (lowerName == "alchemy") return RE::ActorValue::kAlchemy;
        if (lowerName == "alchemymodifier") return RE::ActorValue::kAlchemyModifier;
        if (lowerName == "alchemypowermodifier") return RE::ActorValue::kAlchemyPowerModifier;
        if (lowerName == "alchemyskilladvance") return RE::ActorValue::kAlchemySkillAdvance;
        if (lowerName == "alteration") return RE::ActorValue::kAlteration;
        if (lowerName == "alterationmodifier") return RE::ActorValue::kAlterationModifier;
        if (lowerName == "alterationpowermodifier") return RE::ActorValue::kAlterationPowerModifier;
        if (lowerName == "alterationskilladvance") return RE::ActorValue::kAlterationSkillAdvance;
        if (lowerName == "archery") return RE::ActorValue::kArchery;
        if (lowerName == "armorperks") return RE::ActorValue::kArmorPerks;
        if (lowerName == "assistance") return RE::ActorValue::kAssistance;
        if (lowerName == "attackdamagemult") return RE::ActorValue::kAttackDamageMult;
        if (lowerName == "blindness") return RE::ActorValue::kBlindness;
        if (lowerName == "block") return RE::ActorValue::kBlock;
        if (lowerName == "combathealthregenmultiply") return RE::ActorValue::kCombatHealthRegenMultiply;
        if (lowerName == "confidence") return RE::ActorValue::kConfidence;
        if (lowerName == "conjuration") return RE::ActorValue::kConjuration;
        if (lowerName == "conjurationmodifier") return RE::ActorValue::kConjurationModifier;
        if (lowerName == "conjurationpowermodifier") return RE::ActorValue::kConjurationPowerModifier;
        if (lowerName == "conjurationskilladvance") return RE::ActorValue::kConjurationSkillAdvance;
        if (lowerName == "criticalchance") return RE::ActorValue::kCriticalChance;
        if (lowerName == "damageresist") return RE::ActorValue::kDamageResist;
        if (lowerName == "destruction") return RE::ActorValue::kDestruction;
        if (lowerName == "destructionmodifier") return RE::ActorValue::kDestructionModifier;
        if (lowerName == "destructionpowermodifier") return RE::ActorValue::kDestructionPowerModifier;
        if (lowerName == "destructionskilladvance") return RE::ActorValue::kDestructionSkillAdvance;
        if (lowerName == "detectliferange") return RE::ActorValue::kDetectLifeRange;
        if (lowerName == "dragonrend") return RE::ActorValue::kDragonRend;
        if (lowerName == "dragonsouls") return RE::ActorValue::kDragonSouls;
        if (lowerName == "enchanting") return RE::ActorValue::kEnchanting;
        if (lowerName == "enchantingmodifier") return RE::ActorValue::kEnchantingModifier;
        if (lowerName == "enchantingpowermodifier") return RE::ActorValue::kEnchantingPowerModifier;
        if (lowerName == "enchantingskilladvance") return RE::ActorValue::kEnchantingSkillAdvance;
        if (lowerName == "endurancecondition") return RE::ActorValue::kEnduranceCondition;
        if (lowerName == "energy") return RE::ActorValue::kEnergy;
        if (lowerName == "fame") return RE::ActorValue::kFame;
        if (lowerName == "favoractive") return RE::ActorValue::kFavorActive;
        if (lowerName == "favorpointsbonus") return RE::ActorValue::kFavorPointsBonus;
        if (lowerName == "favorsperday") return RE::ActorValue::kFavorsPerDay;
        if (lowerName == "favorsperdaytimer") return RE::ActorValue::kFavorsPerDayTimer;
        if (lowerName == "grabactoroffset") return RE::ActorValue::kGrabActorOffset;
        if (lowerName == "grabbed") return RE::ActorValue::kGrabbed;
        if (lowerName == "healrate") return RE::ActorValue::kHealRate;
        if (lowerName == "healratemult") return RE::ActorValue::kHealRateMult;
        if (lowerName == "health") return RE::ActorValue::kHealth;
        if (lowerName == "heavyarmor") return RE::ActorValue::kHeavyArmor;
        if (lowerName == "heavyarmormodifier") return RE::ActorValue::kHeavyArmorModifier;
        if (lowerName == "heavyarmorpowermodifier") return RE::ActorValue::kHeavyArmorPowerModifier;
        if (lowerName == "heavyarmorskilladvance") return RE::ActorValue::kHeavyArmorSkillAdvance;
        if (lowerName == "ignorecripplledlimbs") return RE::ActorValue::kIgnoreCrippledLimbs;
        if (lowerName == "illusion") return RE::ActorValue::kIllusion;
        if (lowerName == "illusionmodifier") return RE::ActorValue::kIllusionModifier;
        if (lowerName == "illusionpowermodifier") return RE::ActorValue::kIllusionPowerModifier;
        if (lowerName == "illusionskilladvance") return RE::ActorValue::kIllusionSkillAdvance;
        if (lowerName == "infamy") return RE::ActorValue::kInfamy;
        if (lowerName == "inventoryweight") return RE::ActorValue::kInventoryWeight;
        if (lowerName == "invisibility") return RE::ActorValue::kInvisibility;
        if (lowerName == "jumpingbonus") return RE::ActorValue::kJumpingBonus;
        if (lowerName == "lastbribedintimidated") return RE::ActorValue::kLastBribedIntimidated;
        if (lowerName == "lastflattered") return RE::ActorValue::kLastFlattered;
        if (lowerName == "leftattackcondition") return RE::ActorValue::kLeftAttackCondition;
        if (lowerName == "leftitemcharge") return RE::ActorValue::kLeftItemCharge;
        if (lowerName == "leftmobilitycondition") return RE::ActorValue::kLeftMobilityCondition;
        if (lowerName == "leftweaponspeedmultiply") return RE::ActorValue::kLeftWeaponSpeedMultiply;
        if (lowerName == "lightarmor") return RE::ActorValue::kLightArmor;
        if (lowerName == "lightarmormodifier") return RE::ActorValue::kLightArmorModifier;
        if (lowerName == "lightarmorpowermodifier") return RE::ActorValue::kLightArmorPowerModifier;
        if (lowerName == "lightarmorskilladvance") return RE::ActorValue::kLightArmorSkillAdvance;
        if (lowerName == "lockpicking") return RE::ActorValue::kLockpicking;
        if (lowerName == "lockpickingmodifier") return RE::ActorValue::kLockpickingModifier;
        if (lowerName == "lockpickingpowermodifier") return RE::ActorValue::kLockpickingPowerModifier;
        if (lowerName == "lockpickingskilladvance") return RE::ActorValue::kLockpickingSkillAdvance;
        if (lowerName == "magicka") return RE::ActorValue::kMagicka;
        if (lowerName == "magickarate") return RE::ActorValue::kMagickaRate;
        if (lowerName == "magickaratemult") return RE::ActorValue::kMagickaRateMult;
        if (lowerName == "marksmanmodifier") return RE::ActorValue::kMarksmanModifier;
        if (lowerName == "marksmanpowermodifier") return RE::ActorValue::kMarksmanPowerModifier;
        if (lowerName == "marksmanskilladvance") return RE::ActorValue::kMarksmanSkillAdvance;
        if (lowerName == "mass") return RE::ActorValue::kMass;
        if (lowerName == "meleedamage") return RE::ActorValue::kMeleeDamage;
        if (lowerName == "mood") return RE::ActorValue::kMood;
        if (lowerName == "morality") return RE::ActorValue::kMorality;
        if (lowerName == "movementnoisemult") return RE::ActorValue::kMovementNoiseMult;
        if (lowerName == "nighteye") return RE::ActorValue::kNightEye;
        if (lowerName == "onehanded") return RE::ActorValue::kOneHanded;
        if (lowerName == "onehandedmodifier") return RE::ActorValue::kOneHandedModifier;
        if (lowerName == "onehandedpowermodifier") return RE::ActorValue::kOneHandedPowerModifier;
        if (lowerName == "onehandedskilladvance") return RE::ActorValue::kOneHandedSkillAdvance;
        if (lowerName == "paralysis") return RE::ActorValue::kParalysis;
        if (lowerName == "perceptioncondition") return RE::ActorValue::kPerceptionCondition;
        if (lowerName == "pickpocket") return RE::ActorValue::kPickpocket;
        if (lowerName == "pickpocketmodifier") return RE::ActorValue::kPickpocketModifier;
        if (lowerName == "pickpocketpowermodifier") return RE::ActorValue::kPickpocketPowerModifier;
        if (lowerName == "pickpocketskilladvance") return RE::ActorValue::kPickpocketSkillAdvance;
        if (lowerName == "poisonresist") return RE::ActorValue::kPoisonResist;
        if (lowerName == "reflectdamage") return RE::ActorValue::kReflectDamage;
        if (lowerName == "resistdisease") return RE::ActorValue::kResistDisease;
        if (lowerName == "resistfire") return RE::ActorValue::kResistFire;
        if (lowerName == "resistfrost") return RE::ActorValue::kResistFrost;
        if (lowerName == "resistmagic") return RE::ActorValue::kResistMagic;
        if (lowerName == "resistshock") return RE::ActorValue::kResistShock;
        if (lowerName == "restoration") return RE::ActorValue::kRestoration;
        if (lowerName == "restorationmodifier") return RE::ActorValue::kRestorationModifier;
        if (lowerName == "restorationpowermodifier") return RE::ActorValue::kRestorationPowerModifier;
        if (lowerName == "restorationskilladvance") return RE::ActorValue::kRestorationSkillAdvance;
        if (lowerName == "rightattackcondition") return RE::ActorValue::kRightAttackCondition;
        if (lowerName == "rightitemcharge") return RE::ActorValue::kRightItemCharge;
        if (lowerName == "rightmobilitycondition") return RE::ActorValue::kRightMobilityCondition;
        if (lowerName == "shieldperks") return RE::ActorValue::kShieldPerks;
        if (lowerName == "shoutrecoverymult") return RE::ActorValue::kShoutRecoveryMult;
        if (lowerName == "smithing") return RE::ActorValue::kSmithing;
        if (lowerName == "smithingmodifier") return RE::ActorValue::kSmithingModifier;
        if (lowerName == "smithingpowermodifier") return RE::ActorValue::kSmithingPowerModifier;
        if (lowerName == "smithingskilladvance") return RE::ActorValue::kSmithingSkillAdvance;
        if (lowerName == "sneak") return RE::ActorValue::kSneak;
        if (lowerName == "sneakingmodifier") return RE::ActorValue::kSneakingModifier;
        if (lowerName == "sneakingpowermodifier") return RE::ActorValue::kSneakingPowerModifier;
        if (lowerName == "sneakingskilladvance") return RE::ActorValue::kSneakingSkillAdvance;
        if (lowerName == "speech") return RE::ActorValue::kSpeech;
        if (lowerName == "speechcraftmodifier") return RE::ActorValue::kSpeechcraftModifier;
        if (lowerName == "speechcraftpowermodifier") return RE::ActorValue::kSpeechcraftPowerModifier;
        if (lowerName == "speechcraftskilladvance") return RE::ActorValue::kSpeechcraftSkillAdvance;
        if (lowerName == "speedmult") return RE::ActorValue::kSpeedMult;
        if (lowerName == "stamina") return RE::ActorValue::kStamina;
        if (lowerName == "staminarate") return RE::ActorValue::kStaminaRate;
        if (lowerName == "staminaratemult") return RE::ActorValue::kStaminaRateMult;
        if (lowerName == "telekinesis") return RE::ActorValue::kTelekinesis;
        if (lowerName == "twohanded") return RE::ActorValue::kTwoHanded;
        if (lowerName == "twohandedmodifier") return RE::ActorValue::kTwoHandedModifier;
        if (lowerName == "twohandedpowermodifier") return RE::ActorValue::kTwoHandedPowerModifier;
        if (lowerName == "twohandedskilladvance") return RE::ActorValue::kTwoHandedSkillAdvance;
        if (lowerName == "unarmeddamage") return RE::ActorValue::kUnarmedDamage;
        if (lowerName == "vampireperks") return RE::ActorValue::kVampirePerks;
        if (lowerName == "voicepoints") return RE::ActorValue::kVoicePoints;
        if (lowerName == "voicerate") return RE::ActorValue::kVoiceRate;
        if (lowerName == "waitingforplayer") return RE::ActorValue::kWaitingForPlayer;
        if (lowerName == "warddeflection") return RE::ActorValue::kWardDeflection;
        if (lowerName == "wardpower") return RE::ActorValue::kWardPower;
        if (lowerName == "waterbreathing") return RE::ActorValue::kWaterBreathing;
        if (lowerName == "waterwalking") return RE::ActorValue::kWaterWalking;
        if (lowerName == "weaponspeedmult") return RE::ActorValue::kWeaponSpeedMult;
        if (lowerName == "werewolfperks") return RE::ActorValue::kWerewolfPerks;
        return RE::ActorValue::kNone;
    }

// ╔════════════════════════════════════╗
// ║         COMPARISON HELPERS         ║
// ╚════════════════════════════════════╝

    template<typename T>
    bool CompareValues(const std::string& op, T current, T target) {
        if (op == ">=") return current >= target;
        if (op == "<=") return current <= target;
        if (op == ">") return current > target;
        if (op == "<") return current < target;
        if (op == "=") return current == target;
        if (op == "!=") return current != target;
        return false;
    }

    // Specialization for float, taking into account the error
    template<>
    bool CompareValues<float>(const std::string& op, float current, float target) {
        if (op == ">=") return current >= target;
        if (op == "<=") return current <= target;
        if (op == ">") return current > target;
        if (op == "<") return current < target;
        if (op == "=") return std::abs(current - target) < 0.001f;
        if (op == "!=") return std::abs(current - target) >= 0.001f;
        return false;
    }

// ╔════════════════════════════════════╗
// ║       FORM RETRIEVING HELPERS      ║
// ╚════════════════════════════════════╝

    template <class T>
    T* RuleManager::GetFormFromIdentifier(const std::string& identifier) {
    
        static std::recursive_mutex dataHandlerMutex;
        std::lock_guard<std::recursive_mutex> lock(dataHandlerMutex);
    
        auto pos = identifier.find(':');
        if (pos == std::string::npos) {
            logger::error("Invalid identifier format: '{}'", identifier);
            return nullptr;
        }
    
        std::string modName = identifier.substr(0, pos);
        std::string idStr = identifier.substr(pos + 1);
    
        auto* dh = RE::TESDataHandler::GetSingleton();
        if (!dh) {
            logger::error("TESDataHandler not available");
            return nullptr;
        }
    
        auto* modInfo = dh->LookupModByName(modName);
        if (!modInfo) {
            logger::error("Mod '{}' not found in load order", modName);
            return nullptr;
        }

		// Remove 0x prefix if present
		if (idStr.size() > 2 && (idStr[0] == '0') && (idStr[1] == 'x' || idStr[1] == 'X')) idStr = idStr.substr(2);

		// Handle FE prefix for ESL/ESPFE plugins
		if (idStr.size() > 2 && (idStr[0] == 'F' || idStr[0] == 'f') && (idStr[1] == 'E' || idStr[1] == 'e')) idStr = idStr.substr(2);

		// Remove any other prefix if present
		const std::size_t keep = modInfo->IsLight() ? 3 : 6;
		if (idStr.length() > keep) idStr = idStr.substr(idStr.length() - keep);

		// Handle leading zeros (00123456 -> 123456)
		while (idStr.size() > 1 && idStr[0] == '0') {
			idStr = idStr.substr(1);
		}

		std::uint32_t rawID = 0;
		try {
			rawID = std::stoul(idStr, nullptr, 16);
		} catch (const std::exception& e) {
			logger::error("Invalid FormID '{}' in identifier '{}': {}", idStr, identifier, e.what());
			return nullptr;
		}
    
        auto* form = dh->LookupForm(rawID, modName);
        if (!form) {
            logger::error("Form 0x{:06X} not found in mod '{}'", rawID, modName);
            return nullptr;
        }
    
        auto* typedForm = form->As<T>();
        if (!typedForm) {
            logger::error("Form 0x{:08X} exists but is not of type {} in mod '{}'", form->GetFormID(), typeid(T).name(), modName);
            return nullptr;
        }
    
        return typedForm;
    }

    template <class T>
    T* RuleManager::GetFormFromEditorID(const std::string& editorID) {
        if (editorID.empty()) {
            logger::warn("Empty EditorID provided");
            return nullptr;
        }
    
        auto* form = RE::TESForm::LookupByEditorID<T>(editorID);
        if (form) return form;

        static std::recursive_mutex dataHandlerMutex;
        std::lock_guard<std::recursive_mutex> lock(dataHandlerMutex);
        
        auto* dh = RE::TESDataHandler::GetSingleton();
        if (!dh) {
            logger::error("TESDataHandler not available");
            return nullptr;
        }

        for (auto& formPtr : dh->GetFormArray<T>()) {
            if (formPtr && formPtr->GetFormEditorID() &&
                _stricmp(formPtr->GetFormEditorID(), editorID.c_str()) == 0) {
                return formPtr;
            }
        }
        
        logger::debug("Form with EditorID '{}' not found", editorID);
        return nullptr;
    }

// ╔════════════════════════════════════╗
// ║           FILTER HELPERS           ║
// ╚════════════════════════════════════╝
	
	bool IsDllPresent(const std::string& dllName)
	{
		namespace fs = std::filesystem;
		fs::path dllPath = fs::path("Data") / "SKSE" / "Plugins" / dllName;
		return fs::exists(dllPath);
	}

	bool HasItem(RE::Actor* actor, RE::FormID formID) {
        if (!actor) return false;

        auto inventory = actor->GetInventory();
        if (!inventory.empty()) {
            for (const auto& [item, invData] : inventory) {
                if (item && item->GetFormID() == formID && invData.first > 0) return true;
            }
        }
        return false;
    }
    
    std::uint32_t GetQuestItemStatus(RE::TESObjectREFR* ref) {
        if (!ref) return 0;

        if (ref->HasQuestObject()) return 2;
        
        if (const auto xAliases = ref->extraList.GetByType<RE::ExtraAliasInstanceArray>(); xAliases) { // the line is taken from the PO3 Papyrus Extender
            if (xAliases) RE::BSReadLockGuard locker(xAliases->lock);
            if (!xAliases->aliases.empty()) return 1;
        }
        
        return 0;
    }

	RE::LOCK_LEVEL GetLockLevel(const int lvl)
	{
		if (lvl == -1) return RE::LOCK_LEVEL::kUnlocked;
		if (lvl == 0) return RE::LOCK_LEVEL::kVeryEasy;
		if (lvl == 1) return RE::LOCK_LEVEL::kEasy;
		if (lvl == 2) return RE::LOCK_LEVEL::kAverage;
		if (lvl == 3) return RE::LOCK_LEVEL::kHard;
		if (lvl == 4) return RE::LOCK_LEVEL::kVeryHard;
		if (lvl == 5) return RE::LOCK_LEVEL::kRequiresKey;
		return RE::LOCK_LEVEL::kUnlocked;
	}

    bool CheckActorValueCondition(const ActorValueCondition& condition, RE::Actor* actor) {
        RE::ActorValue av = GetActorValueFromString(condition.actorValue);
        if (av == RE::ActorValue::kNone) return false;
        
        auto* npc = actor->As<RE::TESNPC>();
        if (!npc) return false;
        
        float currentValue = npc->GetActorValue(av);
        
        return CompareValues(condition.operator_type, currentValue, condition.value);
    }

    bool CheckLevelCondition(const LevelCondition& condition, int currentLevel) {
        return CompareValues(condition.operator_type, currentLevel, condition.value);
    }

    bool CheckTimeCondition(const TimeCondition& condition) {
		RE::Calendar* calendar = nullptr;

		try {
			calendar = RE::Calendar::GetSingleton();
		} catch (const std::exception& e) {
			logger::error("Exception getting Calendar singleton: {}", e.what());
			return false;
		} catch (...) {
			logger::error("Unknown exception getting Calendar singleton");
			return false;
		}

        if (!calendar) return false;
        
        float currentValue = 0.0f;
        std::string fieldLower = tolower_str(condition.field);
        
        if (fieldLower == "hour") {
            currentValue = calendar->GetHour();
        } else if (fieldLower == "minute") {
            currentValue = static_cast<float>(calendar->GetMinutes());
        } else if (fieldLower == "day") {
            currentValue = calendar->GetDay();
        } else if (fieldLower == "month") {
            currentValue = static_cast<float>(calendar->GetMonth());
        } else if (fieldLower == "year") {
            currentValue = static_cast<float>(calendar->GetYear());
        } else if (fieldLower == "dayofweek") {
            currentValue = static_cast<float>(calendar->GetDayOfWeek());
        } else if (fieldLower == "gametime") {
            currentValue = calendar->GetCurrentGameTime();
        } else {
            return false;
        }
        
        return CompareValues(condition.operator_type, currentValue, condition.value);
    }

    bool CheckTimeFilters(const Filter& f) {
        if (f.time.empty() && f.timeNot.empty()) {
            return true;
        }

        // Check including time filters
        if (!f.time.empty()) {
            bool anyTimeMatch = false;
            for (const auto& condition : f.time) {
                if (CheckTimeCondition(condition)) {
                    anyTimeMatch = true;
                    break;
                }
            }
            if (!anyTimeMatch) return false;
        }

        // Check excluding time filters
        if (!f.timeNot.empty()) {
            for (const auto& condition : f.timeNot) {
                if (CheckTimeCondition(condition)) {
                    return false;
                }
            }
        }

        return true;
    }

	bool CheckLocationFilter(const Filter& f, const RuleContext& ctx)
	{
		// If there are no filters - skip the check
		if (f.locations.empty() && f.locationsNot.empty()) return true;

		auto* target = ctx.target->As<RE::TESObjectREFR>();
		if (!target) return false;  // If target is not a valid reference, make sure Trigger is not executed

		auto* currentCell = target->GetParentCell();
		RE::FormID currentCellID = currentCell ? currentCell->GetFormID() : 0;

		auto* currentLocation = target->GetCurrentLocation();
		RE::FormID currentLocationID = currentLocation ? currentLocation->GetFormID() : 0;

		auto* currentWorldspace = target->GetWorldspace();
		if (!currentWorldspace && currentCell) {
			currentWorldspace = currentCell->GetRuntimeData().worldSpace;
		}
		RE::FormID currentWorldspaceID = currentWorldspace ? currentWorldspace->GetFormID() : 0;

		// Collect all parent locations
		std::set<RE::FormID> currentlocations;
		std::set<RE::FormID> visitedParents;
		if (currentLocation) {
			currentlocations.insert(currentLocationID);
			auto* parentLoc = currentLocation;
			while (parentLoc && parentLoc->parentLoc) {
				parentLoc = parentLoc->parentLoc;
				if (visitedParents.contains(parentLoc->GetFormID())) {
					logger::warn("Circular reference detected in location hierarchy");
					break;
				}
				visitedParents.insert(parentLoc->GetFormID());
				currentlocations.insert(parentLoc->GetFormID());
			}
		}

		// Check including filters
		if (!f.locations.empty()) {
			bool matched = (currentCellID && f.locations.contains(currentCellID)) ||
						   (currentWorldspaceID && f.locations.contains(currentWorldspaceID)) ||
						   (currentLocationID && f.locations.contains(currentLocationID));

			if (!matched) {
				for (auto locID : currentlocations) {
					if (f.locations.contains(locID)) {
						matched = true;
						break;
					}
				}
			}

			if (!matched) return false;
		}

		// Check excluding filters
		if (!f.locationsNot.empty()) {
			bool excluded = (currentCellID && f.locationsNot.contains(currentCellID)) ||
							(currentWorldspaceID && f.locationsNot.contains(currentWorldspaceID)) ||
							(currentLocationID && f.locationsNot.contains(currentLocationID));

			if (!excluded) {
				for (auto locID : currentlocations) {
					if (f.locationsNot.contains(locID)) {
						excluded = true;
						break;
					}
				}
			}

			if (excluded) return false;
		}

		return true;
	}
    
    bool CheckWeatherFilter(const Filter& f) {
        if (f.weathers.empty() && f.weathersNot.empty()) return true;

		auto* player = RE::PlayerCharacter::GetSingleton();
		if (player) {
			auto* cell = player->GetParentCell();
			if (cell && cell->IsInteriorCell()) return true;  // If player is indoors, skip the weather check
		}
    
        auto* sky = RE::Sky::GetSingleton();
        if (!sky || !sky->currentWeather) return false;  // If weather data is unavailable, make sure Trigger is not executed
    
        RE::FormID currentWeatherID = sky->currentWeather->GetFormID();

        if (!f.weathers.empty()) {
            if (!f.weathers.contains(currentWeatherID)) return false;
        }

        if (!f.weathersNot.empty()) {
            if (f.weathersNot.contains(currentWeatherID)) return false;
        }

        return true;
    }

// ╔════════════════════════════════════╗
// ║       SERIALIZATION HELPERS        ║
// ╚════════════════════════════════════╝

    void RuleManager::ResetInteractionCounts()
    {
        std::unique_lock lock(_ruleMutex);
        _limitCounts.clear();
        _interactionsCounts.clear();
    }
    
    void RuleManager::OnSave(SKSE::SerializationInterface* intf)
    {
        std::shared_lock lock(_ruleMutex);
    
        if (!intf->OpenRecord('LCNT', 2)) // 'LCNT' for Limit Counts
            return;
    
        std::uint32_t limitSize = static_cast<std::uint32_t>(_limitCounts.size());
        intf->WriteRecordData(limitSize);
    
        for (auto& [key, val] : _limitCounts) {
            intf->WriteRecordData(&key, sizeof(key));
            intf->WriteRecordData(&val, sizeof(val));
        }
    }
    
    void RuleManager::OnLoad(SKSE::SerializationInterface* intf)
    {
        ResetInteractionCounts();
    
        std::uint32_t type, version, length;
        while (intf->GetNextRecordInfo(type, version, length)) {
            if (type == 'LCNT') {
                std::uint32_t size;
                intf->ReadRecordData(size);
                for (std::uint32_t i = 0; i < size; ++i) {
                    Key key; std::uint32_t val;
                    intf->ReadRecordData(&key, sizeof(key));
                    intf->ReadRecordData(&val, sizeof(val));
                    _limitCounts[key] = val;
                }
            }
        }
    }

    void RuleManager::InitSerialization()
    {
        if (auto* ser = SKSE::GetSerializationInterface()) {
            ser->SetUniqueID('OIFL');   // Unique ID for Object Impact Framework limit counts

            ser->SetSaveCallback([](auto* intf) {
                GetSingleton()->OnSave(intf);
            });
            ser->SetLoadCallback([](auto* intf) {
                GetSingleton()->OnLoad(intf);
            });
            ser->SetRevertCallback([](auto*) {
                GetSingleton()->ResetInteractionCounts();
            });
        }
    }


//░██████╗░███████╗███╗░░██╗███████╗██████╗░░█████╗░██╗░░░░░
//██╔════╝░██╔════╝████╗░██║██╔════╝██╔══██╗██╔══██╗██║░░░░░
//██║░░██╗░█████╗░░██╔██╗██║█████╗░░██████╔╝███████║██║░░░░░
//██║░░╚██╗██╔══╝░░██║╚████║██╔══╝░░██╔══██╗██╔══██║██║░░░░░
//╚██████╔╝███████╗██║░╚███║███████╗██║░░██║██║░░██║███████╗
//░╚═════╝░╚══════╝╚═╝░░╚══╝╚══════╝╚═╝░░╚═╝╚═╝░░╚═╝╚══════╝

// ╔════════════════════════════════════╗
// ║             SINGLETON              ║
// ╚════════════════════════════════════╝

    RuleManager* RuleManager::GetSingleton() {
        static RuleManager inst;
        return &inst;
    }

// ╔════════════════════════════════════╗
// ║           LOADING RULES            ║
// ╚════════════════════════════════════╝

    void RuleManager::LoadRules() {

        std::unique_lock lock(_ruleMutex);
        _rules.clear();
        
        const fs::path dir{ "Data/SKSE/Plugins/ObjectImpactFramework" };
        if (!fs::exists(dir)) {
            logger::error("Rules directory does not exist: {}", dir.string());
            return;
        }

        try {
            for (auto const& entry : fs::recursive_directory_iterator{ dir }) {
                if (entry.is_regular_file() && entry.path().extension() == ".json") {
                    ParseJSON(entry.path());
                }
            }
        } catch (const fs::filesystem_error& e) {
            logger::error("Filesystem error while loading rules: {}", e.what());
        }

        InvalidateUpdateCache();

        logger::info("Total rules loaded: {}", _rules.size());
    }


//██████╗░░█████╗░██████╗░░██████╗███████╗  ███████╗██╗██╗░░░░░███████╗
//██╔══██╗██╔══██╗██╔══██╗██╔════╝██╔════╝  ██╔════╝██║██║░░░░░██╔════╝
//██████╔╝███████║██████╔╝╚█████╗░█████╗░░  █████╗░░██║██║░░░░░█████╗░░
//██╔═══╝░██╔══██║██╔══██╗░╚═══██╗██╔══╝░░  ██╔══╝░░██║██║░░░░░██╔══╝░░
//██║░░░░░██║░░██║██║░░██║██████╔╝███████╗  ██║░░░░░██║███████╗███████╗
//╚═╝░░░░░╚═╝░░╚═╝╚═╝░░╚═╝╚═════╝░╚══════╝  ╚═╝░░░░░╚═╝╚══════╝╚══════╝

	void RuleManager::ParseJSON(const fs::path& path) {
        std::ifstream ifs(path);
        if (!ifs.is_open()) {
            logger::error("Failed to open JSON file: {}", path.string());
            return;
        }

        json j;
        try {
            ifs >> j;
        } catch (const std::exception& e) {
            logger::error("Error parsing {}: {}", path.string(), e.what());
            return;
        }

        if (!j.is_array()) {
            logger::error("Invalid JSON format in {}: expected array of rules", path.string());
            return;
        }

        json jLow = lower_keys(j);

        for (auto const& jr : jLow) {
            Rule r;

            // ╔════════════════════════════════════╗
			// ║               EVENTS               ║
			// ╚════════════════════════════════════╝

            if (!jr.contains("event")) {
                logger::warn("Skipping rule in {}: missing 'event' field", path.string());
                continue;
            }

            std::vector<std::string> evStrings;
            if (jr["event"].is_string()) {
                evStrings.push_back(jr["event"].get<std::string>());
            } else if (jr["event"].is_array()) {
                for (const auto& evVal : jr["event"]) {
                    if (evVal.is_string())
                        evStrings.push_back(evVal.get<std::string>());
                }
            } else {
                logger::warn("Skipping rule in {}: invalid 'event' field", path.string());
                continue;
            }

            for (const auto& ev : evStrings) {
                std::string evLower = tolower_str(ev);
                if (evLower == "hit") r.events.push_back(EventType::kHit);
                else if (evLower == "activate") r.events.push_back(EventType::kActivate);
                else if (evLower == "grab") r.events.push_back(EventType::kGrab);
                else if (evLower == "release") r.events.push_back(EventType::kRelease);
                else if (evLower == "throw") r.events.push_back(EventType::kThrow);
                else if (evLower == "telekinesis") r.events.push_back(EventType::kTelekinesis);
                else if (evLower == "cellattach") r.events.push_back(EventType::kCellAttach);
                else if (evLower == "celldetach") r.events.push_back(EventType::kCellDetach);
                else if (evLower == "weatherchange") r.events.push_back(EventType::kWeatherChange);
                else if (evLower == "onupdate") r.events.push_back(EventType::kOnUpdate);
                else if (evLower == "destructionstagechange") r.events.push_back(EventType::kDestructionStageChange);
				//else if (evLower == "drop") r.events.push_back(EventType::kDrop);
                else logger::warn("Unknown event '{}' in {}", ev, path.string());
            }

            if (r.events.empty()) {
                logger::warn("Skipping rule in {}: no valid events", path.string());
                continue;
            }

            // ╔════════════════════════════════════╗
			// ║              FILTERS               ║
			// ╚════════════════════════════════════╝

            bool hasObjectIdentifier = false;

            if (jr.contains("filter") && jr["filter"].is_object()) {
                const auto& jf = jr["filter"];

                if (jf.contains("isplugininstalled") && jf["isplugininstalled"].is_array()) {
                    for (const auto& p : jf["isplugininstalled"]) {
                        if (p.is_string())
                            r.filter.requiredPlugins.insert(p.get<std::string>());
                    }
                }

                if (jf.contains("ispluginnotinstalled") && jf["ispluginnotinstalled"].is_array()) {
                    for (const auto& p : jf["ispluginnotinstalled"]) {
                        if (p.is_string())
                            r.filter.requiredPluginsNot.insert(p.get<std::string>());
                    }
                }

                if (!r.filter.requiredPlugins.empty()) {
                    auto* dataHandler = RE::TESDataHandler::GetSingleton();
                    bool allPresent = std::all_of(r.filter.requiredPlugins.begin(), r.filter.requiredPlugins.end(), 
                        [&](const std::string& name) { return dataHandler->LookupModByName(name.c_str()); });
                    if (!allPresent) {
                        logger::debug("Skipping rule in {}: required plugin(s) not installed", path.string());
                        continue;
                    }
                }

                if (!r.filter.requiredPluginsNot.empty()) {
                    auto* dataHandler = RE::TESDataHandler::GetSingleton();
                    bool anyPresent = std::any_of(r.filter.requiredPluginsNot.begin(), r.filter.requiredPluginsNot.end(), 
                        [&](const std::string& name) { return dataHandler->LookupModByName(name.c_str()); });
                    if (anyPresent) {
                        logger::debug("Skipping rule in {}: excluded plugin(s) are installed", path.string());
                        continue;
                    }
                }

                if (jf.contains("isdllinstalled") && jf["isdllinstalled"].is_array()) {
                    for (const auto& d : jf["isdllinstalled"]) {
                        if (d.is_string())
                            r.filter.requiredDLLs.insert(d.get<std::string>());
                    }
                }

                if (jf.contains("isdllnotinstalled") && jf["isdllnotinstalled"].is_array()) {
                    for (const auto& d : jf["isdllnotinstalled"]) {
                        if (d.is_string())
                            r.filter.requiredDLLsNot.insert(d.get<std::string>());
                    }
                }

                if (!r.filter.requiredDLLs.empty()) {
                    bool allPresent = std::all_of(r.filter.requiredDLLs.begin(), r.filter.requiredDLLs.end(), [](const std::string& n) { return IsDllPresent(n); });
                    if (!allPresent) {
                        logger::debug("Skipping rule in {}: required DLL(s) not installed", path.string());
                        continue;
                    }
                }        
        
                if (!r.filter.requiredDLLsNot.empty()) {
                    bool anyPresent = std::any_of(r.filter.requiredDLLsNot.begin(), r.filter.requiredDLLsNot.end(), [](const std::string& n) { return IsDllPresent(n); });
                    if (anyPresent) {
                        logger::debug("Skipping rule in {}: excluded DLL(s) are installed", path.string());
                        continue;
                    }
                }

                if (jf.contains("chance") && (jf["chance"].is_number() || jf["chance"].is_object())) {
					if (jf["chance"].is_number()) {
						// Handle legacy format (simple number)
						try {
							r.filter.chance.value = jf["chance"].get<float>();
							r.filter.chance.useRandom = false;
						} catch (const std::exception& e) {
							logger::warn("Invalid chance value in chance filter of {}: {}", path.string(), e.what());
						}
					} else if (jf["chance"].is_object()) {
						// Handle new format (object with random)
						const auto& chanceObj = jf["chance"];

						if (chanceObj.contains("min") && chanceObj["min"].is_number() &&
							chanceObj.contains("max") && chanceObj["max"].is_number()) {
							try {
								r.filter.chance.min = chanceObj["min"].get<float>();
								r.filter.chance.max = chanceObj["max"].get<float>();
								r.filter.chance.useRandom = true;
								r.filter.chance.value = 100.0f;
							} catch (const std::exception& e) {
								logger::warn("Invalid random chance values in chance filter of {}: {}", path.string(), e.what());
							}
						} else {
							logger::warn("Invalid chance object format in chance filter of {}: missing min/max values", path.string());
						}
					}
				}

				if (jf.contains("interactions") && (jf["interactions"].is_number_unsigned() || jf["interactions"].is_object())) {
					if (jf["interactions"].is_number_unsigned()) {
						try {
							r.filter.interactions.value = jf["interactions"].get<std::uint32_t>();
							r.filter.interactions.useRandom = false;
						} catch (const std::exception& e) {
							logger::warn("Invalid interactions value in interactions filter of {}: {}", path.string(), e.what());
						}
					} else if (jf["interactions"].is_object()) {
						const auto& interactionsObj = jf["interactions"];

						if (interactionsObj.contains("min") && interactionsObj["min"].is_number_unsigned() &&
							interactionsObj.contains("max") && interactionsObj["max"].is_number_unsigned()) {
							try {
								r.filter.interactions.min = interactionsObj["min"].get<std::uint32_t>();
								r.filter.interactions.max = interactionsObj["max"].get<std::uint32_t>();
								r.filter.interactions.useRandom = true;
								r.filter.interactions.value = 1;
							} catch (const std::exception& e) {
								logger::warn("Invalid random interactions values in interactions filter of {}: {}", path.string(), e.what());
							}
						} else {
							logger::warn("Invalid interactions object format in interactions filter of {}: missing min/max values", path.string());
						}
					}
				}

                if (jf.contains("limit") && (jf["limit"].is_number_unsigned() || jf["limit"].is_object())) {
					if (jf["limit"].is_number_unsigned()) {
						try {
							r.filter.limit.value = jf["limit"].get<std::uint32_t>();
							r.filter.limit.useRandom = false;
						} catch (const std::exception& e) {
							logger::warn("Invalid limit value in limit filter of {}: {}", path.string(), e.what());
						}
					} else if (jf["limit"].is_object()) {
						const auto& limitObj = jf["limit"];

						if (limitObj.contains("min") && limitObj["min"].is_number_unsigned() &&
							limitObj.contains("max") && limitObj["max"].is_number_unsigned()) {
							try {
								r.filter.limit.min = limitObj["min"].get<std::uint32_t>();
								r.filter.limit.max = limitObj["max"].get<std::uint32_t>();
								r.filter.limit.useRandom = true;
								r.filter.limit.value = 0;
							} catch (const std::exception& e) {
								logger::warn("Invalid random limit values in limit filter of {}: {}", path.string(), e.what());
							}
						} else {
							logger::warn("Invalid limit object format in limit filter of {}: missing min/max values", path.string());
						}
					}
				}

                if (jf.contains("formtypes") && jf["formtypes"].is_array()) {
                    for (auto const& ft : jf["formtypes"]) {
                        if (ft.is_string()) {
                            std::string ftStr = ft.get<std::string>();
                            RE::FormType formType = MapStringToFormType(tolower_str(ftStr));
                            if (formType != RE::FormType::None) {
                                r.filter.formTypes.insert(formType);
                            } else {
                                logger::warn("Unknown form type '{}' in formtypes filter of {}", ftStr, path.string());
                            }
                        }
                    }
                    if (!r.filter.formTypes.empty()) hasObjectIdentifier = true;
                }
                
                if (jf.contains("formtypesnot") && jf["formtypesnot"].is_array()) {
                    for (auto const& ft : jf["formtypesnot"]) {
                        if (ft.is_string()) {
                            std::string ftStr = ft.get<std::string>();
                            RE::FormType formType = MapStringToFormType(tolower_str(ftStr));
                            if (formType != RE::FormType::None) {
                                r.filter.formTypesNot.insert(formType);
                            } else {
                                logger::warn("Unknown form type '{}' in formtypesnot filter of {}", ftStr, path.string());
                            }
                        }
                    }
                }

                if (jf.contains("formids") && jf["formids"].is_array()) {
                    for (auto const& bid : jf["formids"]) {
                        if (bid.is_string()) {
                            if (auto* form = GetFormFromIdentifier<RE::TESForm>(bid.get<std::string>())) {
                                r.filter.formIDs.insert(form->GetFormID());
                            } else {
                                logger::warn("Invalid formID '{}' in formids filter of {}", bid.get<std::string>(), path.string());
                            }
                        }
                    }
        
                    if (!r.filter.formIDs.empty()) {
                        hasObjectIdentifier = true;
                    }
                }

                if (jf.contains("formidsnot") && jf["formidsnot"].is_array()) {
                    for (auto const& bid : jf["formidsnot"]) {
                        if (bid.is_string()) {
                            if (auto* form = GetFormFromIdentifier<RE::TESForm>(bid.get<std::string>())) {
                                r.filter.formIDsNot.insert(form->GetFormID());
                            } else {
                                logger::warn("Invalid formID '{}' in formidsnot filter of {}", bid.get<std::string>(), path.string());
                            }
                        }
                    }
                }

                if (jf.contains("editorids") && jf["editorids"].is_array()) {
                    for (auto const& eid : jf["editorids"]) {
                        if (eid.is_string()) {
                            if (auto* form = GetFormFromEditorID<RE::TESForm>(eid.get<std::string>())) {
                                r.filter.formIDs.insert(form->GetFormID());
                            } else {
                                logger::warn("Invalid editorID '{}' in editorids filter of {}", eid.get<std::string>(), path.string());
                            }
                        }
                    }
                    if (!r.filter.formIDs.empty()) {
                        hasObjectIdentifier = true;
                    }
                }

                if (jf.contains("editoridsnot") && jf["editoridsnot"].is_array()) {
                    for (auto const& eid : jf["editoridsnot"]) {
                        if (eid.is_string()) {
                            if (auto* form = GetFormFromEditorID<RE::TESForm>(eid.get<std::string>())) {
                                r.filter.formIDsNot.insert(form->GetFormID());
                            } else {
                                logger::warn("Invalid editorID '{}' in editoridsnot filter of {}", eid.get<std::string>(), path.string());
                            }
                        }
                    }
                }

                if (jf.contains("formlists") && jf["formlists"].is_array()) {
                    for (const auto& entry : jf["formlists"]) {
                        if (entry.is_object()) {
                            std::string formIdStr;
                            int idx = -1;

                            if (entry.contains("formid") && entry["formid"].is_string()) {
                                formIdStr = entry["formid"].get<std::string>();
                
                                if (entry.contains("index") && entry["index"].is_number_integer())
                                    idx = entry["index"].get<int>();
                
                                auto* list = GetFormFromIdentifier<RE::BGSListForm>(formIdStr);
                                if (list) {
                                    r.filter.formLists.push_back({ list->formID, idx });
                                } else {
                                    logger::warn("Invalid formlist '{}' in formlists filter of {}", formIdStr, path.string());
                                }
                            } else if (entry.contains("editorid") && entry["editorid"].is_string()) {
                                formIdStr = entry["editorid"].get<std::string>();
                
                                if (entry.contains("index") && entry["index"].is_number_integer())
                                    idx = entry["index"].get<int>();
                
                                auto* list = GetFormFromEditorID<RE::BGSListForm>(formIdStr);
                                if (list) {
                                    r.filter.formLists.push_back({ list->formID, idx });
                                } else {
                                    logger::warn("Invalid formlist '{}' in formlists filter of {}", formIdStr, path.string());
                                }
                            }
                        }
                    }
                    if (!r.filter.formLists.empty()) {
                        hasObjectIdentifier = true;
                    }
                }         
                
                if (jf.contains("formlistsnot") && jf["formlistsnot"].is_array()) {
                    for (const auto& entry : jf["formlistsnot"]) {
                        if (entry.is_object()) {
                            std::string formIdStr;
                            int idx = -1;

                            if (entry.contains("formid") && entry["formid"].is_string()) {
                                formIdStr = entry["formid"].get<std::string>();
                
                                if (entry.contains("index") && entry["index"].is_number_integer())
                                    idx = entry["index"].get<int>();
                
                                auto* list = GetFormFromIdentifier<RE::BGSListForm>(formIdStr);
                                if (list) {
                                    r.filter.formListsNot.push_back({ list->formID, idx });
                                } else {
                                    logger::warn("Invalid formlist '{}' in formlistsnot filter of {}", formIdStr, path.string());
                                }
                            } else if (entry.contains("editorid") && entry["editorid"].is_string()) {
                                formIdStr = entry["editorid"].get<std::string>();
                
                                if (entry.contains("index") && entry["index"].is_number_integer())
                                    idx = entry["index"].get<int>();
                
                                auto* list = GetFormFromEditorID<RE::BGSListForm>(formIdStr);
                                if (list) {
                                    r.filter.formListsNot.push_back({ list->formID, idx });
                                } else {
                                    logger::warn("Invalid formlist '{}' in formlistsnot filter of {}", formIdStr, path.string());
                                }
                            }
                        }
                    }
                }
                
                if (jf.contains("keywords") && jf["keywords"].is_array()) {
                    for (auto const& kwEntry : jf["keywords"]) {
                        if (kwEntry.is_string()) {
                            std::string kwStr = kwEntry.get<std::string>();
                            RE::TESForm* form = nullptr;
                            
                            if (kwStr.find(':') != std::string::npos) {
                                form = GetFormFromIdentifier<RE::TESForm>(kwStr);
                            } else {
                                form = GetFormFromEditorID<RE::BGSKeyword>(kwStr);
                                if (!form) {
                                    form = GetFormFromEditorID<RE::BGSListForm>(kwStr);
                                }
                                if (!form) {
                                    form = GetFormFromEditorID<RE::BGSKeyword>(kwStr);
                                if (!form) {
                                    form = GetFormFromEditorID<RE::BGSListForm>(kwStr);
                                }
                                if (!form) {
                                    form = GetFormFromEditorID<RE::TESForm>(kwStr);
                                }     
                                }
                            }
                            
                            if (form) {
                                if (auto* kw = form->As<RE::BGSKeyword>()) {
                                    r.filter.keywords.insert(kw);
                                } else if (auto* formList = form->As<RE::BGSListForm>()) {
                                    for (auto* el : formList->forms) {
                                        if (el) {
                                            if (auto* listKw = el->As<RE::BGSKeyword>()) {
                                                r.filter.keywords.insert(listKw);
                                            }
                                        }
                                    }
                                } else {
                                    logger::warn("Invalid keyword form '{}' (not Keyword or FormList) in keywords filter of {}", kwStr, path.string());
                                }
                            } else {
                                logger::warn("Keyword not found: '{}' in keywords filter of {}", 
                                kwStr, path.string());
                            }
                        }
                    }
                    if (!r.filter.keywords.empty()) {
                        hasObjectIdentifier = true;
                    }
                }

                if (jf.contains("keywordsnot") && jf["keywordsnot"].is_array()) {
                    for (auto const& kwEntry : jf["keywordsnot"]) {
                        if (kwEntry.is_string()) {
                            std::string kwStr = kwEntry.get<std::string>();
                            RE::TESForm* form = nullptr;
                            
                            if (kwStr.find(':') != std::string::npos) {
                                form = GetFormFromIdentifier<RE::TESForm>(kwStr);
                            } else {
                                form = GetFormFromEditorID<RE::BGSKeyword>(kwStr);
                                if (!form) {
                                    form = GetFormFromEditorID<RE::BGSListForm>(kwStr);
                                }
                                if (!form) {
                                    form = GetFormFromEditorID<RE::BGSKeyword>(kwStr);
                                if (!form) {
                                    form = GetFormFromEditorID<RE::BGSListForm>(kwStr);
                                }
                                if (!form) {
                                    form = GetFormFromEditorID<RE::TESForm>(kwStr);
                                }     
                                }                            
                            }
                            
                            if (form) {
                                if (auto* kw = form->As<RE::BGSKeyword>()) {
                                    r.filter.keywordsNot.insert(kw);
                                } else if (auto* formList = form->As<RE::BGSListForm>()) {
                                    for (auto* el : formList->forms) {
                                        if (el) {
                                            if (auto* listKw = el->As<RE::BGSKeyword>()) {
                                                r.filter.keywordsNot.insert(listKw);
                                            }
                                        }
                                    }
                                } else {
                                    logger::warn("Invalid keywordsnot form '{}' (not Keyword or FormList) in keywordsnot filter of {}", kwStr, path.string());
                                }
                            } else {
                                logger::warn("Keyword not found: '{}' in keywordsnot filter of {}", kwStr, path.string());
                            }
                        }
                    }
                }

                if (jf.contains("questitemstatus") && jf["questitemstatus"].is_number_unsigned()) {
                    try {
                        r.filter.questItemStatus = jf["questitemstatus"].get<std::uint32_t>();
                    } catch (const std::exception& e) {
                        logger::warn("Invalid integer value in questitemstatus filter of {}: {}", path.string(), e.what());
                    }
                }

				if (jf.contains("locklevel") && jf["locklevel"].is_number_integer()) {
					try {
						r.filter.lockLevel = jf["locklevel"].get<std::int32_t>();
					} catch (const std::exception& e) {
						logger::warn("Invalid integer value in locklevel filter of {}: {}", path.string(), e.what());
					}
				}

				if (jf.contains("locklevelnot") && jf["locklevelnot"].is_number_integer()) {
					try {
						r.filter.lockLevelNot = jf["locklevelnot"].get<std::int32_t>();
					} catch (const std::exception& e) {
						logger::warn("Invalid integer value in locklevelnot filter of {}: {}", path.string(), e.what());
					}
				}

                if (jf.contains("weaponstypes") && jf["weaponstypes"].is_array()) {
                    for (auto const& wt : jf["weaponstypes"]) {
                        if (wt.is_string()) {
                            r.filter.weaponsTypes.insert(MapWeaponTypeToString(tolower_str(wt.get<std::string>())));
                        } else {
                            logger::warn("Invalid weapon type '{}' in weaponstypes filter of {}", wt.get<std::string>(), path.string());
                        }
                    }
                }

                if (jf.contains("weaponstypesnot") && jf["weaponstypesnot"].is_array()) {
                    for (auto const& wt : jf["weaponstypesnot"]) {
                        if (wt.is_string()) {
                            r.filter.weaponsTypesNot.insert(MapWeaponTypeToString(tolower_str(wt.get<std::string>())));
                        } else {
                            logger::warn("Invalid weapon type '{}' in weaponstypesnot filter of {}", wt.get<std::string>(), path.string());
                        }
                    }
                }

                if (jf.contains("weaponskeywords") && jf["weaponskeywords"].is_array()) {
                    for (auto const& kwEntry : jf["weaponskeywords"]) {
                        if (kwEntry.is_string()) {
                            std::string kwStr = kwEntry.get<std::string>();
                            RE::TESForm* form = nullptr;
                            
                            if (kwStr.find(':') != std::string::npos) {
                                form = GetFormFromIdentifier<RE::TESForm>(kwStr);
                            } else {
                                form = GetFormFromEditorID<RE::BGSKeyword>(kwStr);
                                if (!form) {
                                    form = GetFormFromEditorID<RE::BGSListForm>(kwStr);
                                }
                                if (!form) {
                                    form = GetFormFromEditorID<RE::TESForm>(kwStr);
                                }     
                            }
                            
                            if (form) {
                                if (auto* kw = form->As<RE::BGSKeyword>()) {
                                    r.filter.weaponsKeywords.insert(kw);
                                } else if (auto* formList = form->As<RE::BGSListForm>()) {
                                    for (auto* el : formList->forms) {
                                        if (el) {
                                            if (auto* listKw = el->As<RE::BGSKeyword>()) {
                                                r.filter.weaponsKeywords.insert(listKw);
                                            }
                                        }
                                    }
                                } else {
                                    logger::warn("Invalid weaponskeywords form '{}' (not Keyword or FormList) in weaponskeywords filter of {}", kwStr, path.string());
                                }
                            } else {
                                logger::warn("Keyword not found: '{}' in weaponskeywords filter of {}", 
                                kwStr, path.string());
                            }
                        }
                    }
                }
                
                if (jf.contains("weaponskeywordsnot") && jf["weaponskeywordsnot"].is_array()) {
                    for (auto const& kwEntry : jf["weaponskeywordsnot"]) {
                        if (kwEntry.is_string()) {
                            std::string kwStr = kwEntry.get<std::string>();
                            RE::TESForm* form = nullptr;
                            
                            if (kwStr.find(':') != std::string::npos) {
                                form = GetFormFromIdentifier<RE::TESForm>(kwStr);
                            } else {
                                form = GetFormFromEditorID<RE::BGSKeyword>(kwStr);
                                if (!form) {
                                    form = GetFormFromEditorID<RE::BGSListForm>(kwStr);
                                }
                                if (!form) {
                                    form = GetFormFromEditorID<RE::TESForm>(kwStr);
                                }     
                            }
                            
                            if (form) {
                                if (auto* kw = form->As<RE::BGSKeyword>()) {
                                    r.filter.weaponsKeywordsNot.insert(kw);
                                } else if (auto* formList = form->As<RE::BGSListForm>()) {
                                    for (auto* el : formList->forms) {
                                        if (el) {
                                            if (auto* listKw = el->As<RE::BGSKeyword>()) {
                                                r.filter.weaponsKeywordsNot.insert(listKw);
                                            }
                                        }
                                    }
                                } else {
                                    logger::warn("Invalid weaponskeywordsnot form '{}' (not Keyword or FormList) in weaponskeywordsnot filter of {}", kwStr, path.string());
                                }
                            } else {
                                logger::warn("Keyword not found: '{}' in weaponskeywordsnot filter of {}", kwStr, path.string());
                            }
                        }
                    }
                }

                if (jf.contains("weapons") && jf["weapons"].is_array()) {
                    for (auto const& weapon : jf["weapons"]) {
                        if (weapon.is_string()) {
                            auto weaponStr = weapon.get<std::string>();
                            RE::TESForm* form = nullptr;
                            
                            if (weaponStr.find(':') != std::string::npos) {
                                form = GetFormFromIdentifier<RE::TESForm>(weaponStr);
                            } else {
                                form = GetFormFromEditorID<RE::TESForm>(weaponStr);
                            }
                            
                            if (form) {
                                if (auto* formList = form->As<RE::BGSListForm>()) {
                                    for (auto* el : formList->forms) {
                                        if (el) {
                                            r.filter.weapons.insert(el);
                                        }
                                    }
                                } else {
                                    r.filter.weapons.insert(form);
                                }
                            } else {
                                logger::warn("Invalid weapon identifier '{}' in weapons filter of {}", weaponStr, path.string());
                            }
                        }
                    }
                }

                if ((jf.contains("weaponsnot") && jf["weaponsnot"].is_array())) {
                    for (auto const& weapon : jf["weaponsnot"]) {
                        if (weapon.is_string()) {
                            auto weaponStr = weapon.get<std::string>();
                            RE::TESForm* form = nullptr;
                            
                            if (weaponStr.find(':') != std::string::npos) {
                                form = GetFormFromIdentifier<RE::TESForm>(weaponStr);
                            } else {
                                form = GetFormFromEditorID<RE::TESForm>(weaponStr);
                            }
                            
                            if (form) {
                                if (auto* formList = form->As<RE::BGSListForm>()) {
                                    for (auto* el : formList->forms) {
                                        if (el) {
                                            r.filter.weaponsNot.insert(el);
                                        }
                                    }
                                } else {
                                    r.filter.weaponsNot.insert(form);
                                }
                            } else {
                                logger::warn("Invalid weapon identifier '{}' in weaponsnot filter of {}", weaponStr, path.string());
                            }
                        }
                    }
                }

                if (jf.contains("allowprojectiles") && jf["allowprojectiles"].is_number_unsigned()) {
                    try {
                        r.filter.allowProjectiles = jf["allowprojectiles"].get<std::uint32_t>();
                    } catch (const std::exception& e) {
                        logger::warn("Invalid integer value in allowprojectiles filter of {}: {}", path.string(), e.what());
                    }
                }

                if (jf.contains("projectiles") && jf["projectiles"].is_array()) {
                    for (auto const& proj : jf["projectiles"]) {
                        if (proj.is_string()) {
                            auto projStr = proj.get<std::string>();
                            RE::TESForm* form = nullptr;
                            
                            if (projStr.find(':') != std::string::npos) {
                                form = GetFormFromIdentifier<RE::TESForm>(projStr);
                            } else {
                                form = GetFormFromEditorID<RE::TESForm>(projStr);
                            }
                            
                            if (form) {
                                if (form->Is(RE::FormType::Projectile)) {
                                    r.filter.projectiles.insert(form);
                                } else if (auto* formList = form->As<RE::BGSListForm>()) {
                                    for (auto* el : formList->forms) {
                                        if (el && el->Is(RE::FormType::Projectile)) {
                                            r.filter.projectiles.insert(el);
                                        }
                                    }
                                } else {
                                    logger::warn("Invalid projectile form '{}' (not Projectile or FormList) in projectiles filter of {}", projStr, path.string());
                                }
                            } else {
                                logger::warn("Invalid projectile identifier '{}' in projectiles filter of {}", projStr, path.string());
                            }
                        }
                    }
                }

                if (jf.contains("projectilesnot") && jf["projectilesnot"].is_array()) {
                    for (auto const& proj : jf["projectilesnot"]) {
                        if (proj.is_string()) {
                            auto projStr = proj.get<std::string>();
                            RE::TESForm* form = nullptr;
                            
                            if (projStr.find(':') != std::string::npos) {
                                form = GetFormFromIdentifier<RE::TESForm>(projStr);
                            } else {
                                form = GetFormFromEditorID<RE::TESForm>(projStr);
                            }
                            
                            if (form) {
                                if (form->Is(RE::FormType::Projectile)) {
                                    r.filter.projectilesNot.insert(form);
                                } else if (auto* formList = form->As<RE::BGSListForm>()) {
                                    for (auto* el : formList->forms) {
                                        if (el && el->Is(RE::FormType::Projectile)) {
                                            r.filter.projectilesNot.insert(el);
                                        }
                                    }
                                } else {
                                    logger::warn("Invalid projectile form '{}' (not Projectile or FormList) in projectilesnot filter of {}", projStr, path.string());
                                }
                            } else {
                                logger::warn("Invalid projectile identifier '{}' in projectilesnot filter of {}", projStr, path.string());
                            }
                        }
                    }
                }

                if (jf.contains("attacks") && jf["attacks"].is_array() || jf.contains("attackstypes") && jf["attackstypes"].is_array()) {
                    if (jf.contains("attacks")) {
                        for (auto const& at : jf["attacks"]) {
                            if (at.is_string()) {
                                r.filter.attackTypes.insert(MapAttackTypeToString(tolower_str(at.get<std::string>())));
                            } else {
                                logger::warn("Invalid attack type '{}' in attacks filter of {}", at.get<std::string>(), path.string());
                            }
                        }
                    }
                    if (jf.contains("attackstypes")) {
                        for (auto const& at : jf["attackstypes"]) {
                            if (at.is_string()) {
                                r.filter.attackTypes.insert(MapAttackTypeToString(tolower_str(at.get<std::string>())));
                            } else {
                                logger::warn("Invalid attack type '{}' in attackstypes filter of {}", at.get<std::string>(), path.string());
                            }
                        }
                    }
                }

                if (jf.contains("attacksnot") && jf["attacksnot"].is_array() || jf.contains("attackstypesnot") && jf["attackstypesnot"].is_array()) {
                    if (jf.contains("attacksnot")) {
                        for (auto const& at : jf["attacksnot"]) {
                            if (at.is_string()) {
                                r.filter.attackTypesNot.insert(MapAttackTypeToString(tolower_str(at.get<std::string>())));
                            } else {
                                logger::warn("Invalid attack type '{}' in attacksnot filter of {}", at.get<std::string>(), path.string());
                            }
                        }
                    }
                    if (jf.contains("attackstypesnot")) {
                        for (auto const& at : jf["attackstypesnot"]) {
                            if (at.is_string()) {
                                r.filter.attackTypesNot.insert(MapAttackTypeToString(tolower_str(at.get<std::string>())));
                            } else {
                                logger::warn("Invalid attack type '{}' in attackstypesnot filter of {}", at.get<std::string>(), path.string());
                            }
                        }
                    }
                }

                if (jf.contains("deliverytypes") && jf["deliverytypes"].is_array()) {
                    for (auto const& dt : jf["deliverytypes"]) {
                        if (dt.is_string()) {
                            r.filter.deliveryTypes.insert(MapDeliveryTypeToString(tolower_str(dt.get<std::string>())));
                        } else {
                            logger::warn("Invalid delivery type '{}' in deliverytypes filter of {}", dt.get<std::string>(), path.string());
                        }
                    }
                }

                if (jf.contains("deliverytypesnot") && jf["deliverytypesnot"].is_array()) {
                    for (auto const& dt : jf["deliverytypesnot"]) {
                        if (dt.is_string()) {
                            r.filter.deliveryTypesNot.insert(MapDeliveryTypeToString(tolower_str(dt.get<std::string>())));
                        } else {
                            logger::warn("Invalid delivery type '{}' in deliverytypesnot filter of {}", dt.get<std::string>(), path.string());
                        }
                    }
                }

                if (jf.contains("locations") && jf["locations"].is_array()) {
                    for (auto const& loc : jf["locations"]) {
                        if (loc.is_string()) {
                            auto formStr = loc.get<std::string>();
                            RE::TESForm* form = nullptr;
                            
                            if (formStr.find(':') != std::string::npos) {
                                form = GetFormFromIdentifier<RE::TESForm>(formStr);
                            } else {
                                form = GetFormFromEditorID<RE::TESForm>(formStr);
                            }
                            
                            if (form) {
                                if (form->Is(RE::FormType::Cell) || form->Is(RE::FormType::Location) || form->Is(RE::FormType::WorldSpace)) {
                                    r.filter.locations.insert(form->GetFormID());
                                } else if (auto* formList = form->As<RE::BGSListForm>()) {
                                    for (auto* el : formList->forms) {
                                        if (el && (el->Is(RE::FormType::Cell) || el->Is(RE::FormType::Location) || el->Is(RE::FormType::WorldSpace))) {
                                            r.filter.locations.insert(el->GetFormID());
                                        }
                                    }
                                } else {
                                    logger::warn("Invalid location form '{}' (not Cell, Location, Worldspace or FormList) in locations filter of {}", formStr, path.string());
                                }
                            } else {
                                logger::warn("Invalid location identifier '{}' in locations filter of {}", formStr, path.string());
                            }
                        }
                    }
                }
                
                if (jf.contains("locationsnot") && jf["locationsnot"].is_array()) {
                    for (auto const& loc : jf["locationsnot"]) {
                        if (loc.is_string()) {
                            auto formStr = loc.get<std::string>();
                            RE::TESForm* form = nullptr;
                            
                            if (formStr.find(':') != std::string::npos) {
                                form = GetFormFromIdentifier<RE::TESForm>(formStr);
                            } else {
                                form = GetFormFromEditorID<RE::TESForm>(formStr);
                            }
                            
                            if (form) {
                                if (form->Is(RE::FormType::Cell) || form->Is(RE::FormType::Location) || form->Is(RE::FormType::WorldSpace)) {
                                    r.filter.locationsNot.insert(form->GetFormID());
                                } else if (auto* formList = form->As<RE::BGSListForm>()) {
                                    for (auto* el : formList->forms) {
                                        if (el && (el->Is(RE::FormType::Cell) || el->Is(RE::FormType::Location) || el->Is(RE::FormType::WorldSpace))) {
                                            r.filter.locationsNot.insert(el->GetFormID());
                                        }
                                    }
                                } else {
                                    logger::warn("Invalid location form '{}' (not Cell, Location, Worldspace or FormList) in locationsnot filter of {}", formStr, path.string());
                                }
                            } else {
                                logger::warn("Invalid location identifier '{}' in locationsnot filter of {}", formStr, path.string());
                            }
                        }
                    }
                }
                
                if (jf.contains("weathers") && jf["weathers"].is_array()) {
                    for (auto const& weather : jf["weathers"]) {
                        if (weather.is_string()) {
                            auto formStr = weather.get<std::string>();
                            RE::TESForm* form = nullptr;
                            
                            if (formStr.find(':') != std::string::npos) {
                                form = GetFormFromIdentifier<RE::TESForm>(formStr);
                            } else {
                                form = GetFormFromEditorID<RE::TESForm>(formStr);
                            }
                            
                            if (form) {
                                if (form->Is(RE::FormType::Weather)) {
                                    r.filter.weathers.insert(form->GetFormID());
                                } else if (auto* formList = form->As<RE::BGSListForm>()) {
                                    for (auto* el : formList->forms) {
                                        if (el && el->Is(RE::FormType::Weather)) {
                                            r.filter.weathers.insert(el->GetFormID());
                                        }
                                    }
                                } else {
                                    logger::warn("Invalid weather form '{}' (not Weather or FormList) in weathers filter of {}", formStr, path.string());
                                }
                            } else {
                                logger::warn("Invalid weather identifier '{}' in weathers filter of {}", formStr, path.string());
                            }
                        }
                    }
                }
                
                if (jf.contains("weathersnot") && jf["weathersnot"].is_array()) {
                    for (auto const& weather : jf["weathersnot"]) {
                        if (weather.is_string()) {
                            auto formStr = weather.get<std::string>();
                            RE::TESForm* form = nullptr;
                            
                            if (formStr.find(':') != std::string::npos) {
                                form = GetFormFromIdentifier<RE::TESForm>(formStr);
                            } else {
                                form = GetFormFromEditorID<RE::TESForm>(formStr);
                            }
                            
                            if (form) {
                                if (form->Is(RE::FormType::Weather)) {
                                    r.filter.weathersNot.insert(form->GetFormID());
                                } else if (auto* formList = form->As<RE::BGSListForm>()) {
                                    for (auto* el : formList->forms) {
                                        if (el && el->Is(RE::FormType::Weather)) {
                                            r.filter.weathersNot.insert(el->GetFormID());
                                        }
                                    }
                                } else {
                                    logger::warn("Invalid weathersnot form '{}' (not Weather or FormList) in weathersnot filter of {}", formStr, path.string());
                                }
                            } else {
                                logger::warn("Invalid weathersnot identifier '{}' in weathersnot filter of {}", formStr, path.string());
                            }
                        }
                    }
                }
                
                if (jf.contains("perks") && jf["perks"].is_array()) {
                    for (const auto& perk : jf["perks"]) {
                        if (perk.is_string()) {
                            auto perkStr = perk.get<std::string>();
                            RE::TESForm* form = nullptr;
                            
                            if (perkStr.find(':') != std::string::npos) {
                                form = GetFormFromIdentifier<RE::TESForm>(perkStr);
                            } else {
                                form = GetFormFromEditorID<RE::TESForm>(perkStr);
                            }
                            
                            if (form) {
                                if (auto* perkForm = form->As<RE::BGSPerk>()) {
                                    r.filter.perks.insert(perkForm->GetFormID());
                                } else if (auto* formList = form->As<RE::BGSListForm>()) {
                                    for (auto* el : formList->forms) {
                                        if (el) {
                                            if (auto* listPerk = el->As<RE::BGSPerk>()) {
                                                r.filter.perks.insert(listPerk->GetFormID());
                                            }
                                        }
                                    }
                                } else {
                                    logger::warn("Invalid perks form '{}' (not Perk or FormList) in perks filter of {}", perkStr, path.string());
                                }
                            } else {
                                logger::warn("Invalid perks identifier '{}' in perks filter of {}", perkStr, path.string());
                            }
                        }
                    }
                }
                
                if (jf.contains("perksnot") && jf["perksnot"].is_array()) {
                    for (const auto& perk : jf["perksnot"]) {
                        if (perk.is_string()) {
                            auto perkStr = perk.get<std::string>();
                            RE::TESForm* form = nullptr;
                            
                            if (perkStr.find(':') != std::string::npos) {
                                form = GetFormFromIdentifier<RE::TESForm>(perkStr);
                            } else {
                                form = GetFormFromEditorID<RE::TESForm>(perkStr);
                            }
                            
                            if (form) {
                                if (auto* perkForm = form->As<RE::BGSPerk>()) {
                                    r.filter.perksNot.insert(perkForm->GetFormID());
                                } else if (auto* formList = form->As<RE::BGSListForm>()) {
                                    for (auto* el : formList->forms) {
                                        if (el) {
                                            if (auto* listPerk = el->As<RE::BGSPerk>()) {
                                                r.filter.perksNot.insert(listPerk->GetFormID());
                                            }
                                        }
                                    }
                                } else {
                                    logger::warn("Invalid perksnot form '{}' (not Perk or FormList) in perksnot filter of {}", perkStr, path.string());
                                }
                            } else {
                                logger::warn("Invalid perksnot identifier '{}' in perksnot filter of {}", perkStr, path.string());
                            }
                        }
                    }
                }

                if (jf.contains("spells") && jf["spells"].is_array()) {
                    for (const auto& spell : jf["spells"]) {
                        if (spell.is_string()) {
                            auto spellStr = spell.get<std::string>();
                            RE::TESForm* form = nullptr;
                            
                            if (spellStr.find(':') != std::string::npos) {
                                form = GetFormFromIdentifier<RE::TESForm>(spellStr);
                            } else {
                                form = GetFormFromEditorID<RE::TESForm>(spellStr);
                            }
                            
                            if (form) {
                                if (auto* spellForm = form->As<RE::SpellItem>()) {
                                    r.filter.spells.insert(spellForm->GetFormID());
                                } else if (auto* formList = form->As<RE::BGSListForm>()) {
                                    for (auto* el : formList->forms) {
                                        if (el) {
                                            if (auto* listSpell = el->As<RE::SpellItem>()) {
                                                r.filter.spells.insert(listSpell->GetFormID());
                                            }
                                        }
                                    }
                                } else {
                                    logger::warn("Invalid spells form '{}' (not Spell or FormList) in spells filter of {}", spellStr, path.string());
                                }
                            } else {
                                logger::warn("Invalid spells identifier '{}' in spells filter of {}", spellStr, path.string());
                            }
                        }
                    }
                }

                if (jf.contains("spellsnot") && jf["spellsnot"].is_array()) {
                    for (const auto& spell : jf["spellsnot"]) {
                        if (spell.is_string()) {
                            auto spellStr = spell.get<std::string>();
                            RE::TESForm* form = nullptr;
                            
                            if (spellStr.find(':') != std::string::npos) {
                                form = GetFormFromIdentifier<RE::TESForm>(spellStr);
                            } else {
                                form = GetFormFromEditorID<RE::TESForm>(spellStr);
                            }
                            
                            if (form) {
                                if (auto* spellForm = form->As<RE::SpellItem>()) {
                                    r.filter.spellsNot.insert(spellForm->GetFormID());
                                } else if (auto* formList = form->As<RE::BGSListForm>()) {
                                    for (auto* el : formList->forms) {
                                        if (el) {
                                            if (auto* listSpell = el->As<RE::SpellItem>()) {
                                                r.filter.spellsNot.insert(listSpell->GetFormID());
                                            }
                                        }
                                    }
                                } else {
                                    logger::warn("Invalid spellsnot form '{}' (not Spell or FormList) in spellsnot filter of {}", spellStr, path.string());
                                }
                            } else {
                                logger::warn("Invalid spellsnot identifier '{}' in spellsnot filter of {}", spellStr, path.string());
                            }
                        }
                    }
                }

                if (jf.contains("hasitem") && jf["hasitem"].is_array()) {
                    for (auto const& item : jf["hasitem"]) {
                        if (item.is_string()) {
                            std::string itemStr = item.get<std::string>();
                            RE::TESForm* form = nullptr;

                            if (itemStr.find(':') != std::string::npos) {
                                form = GetFormFromIdentifier<RE::TESForm>(itemStr);
                            } else {
                                form = GetFormFromEditorID<RE::TESForm>(itemStr);
                            }

                            if (form) {
                                if (auto* formList = form->As<RE::BGSListForm>()) {
                                    for (auto* el : formList->forms) {
                                        if (el) {
                                            r.filter.hasItem.insert(el->GetFormID());
                                        }
                                    }
                                } else {
                                    r.filter.hasItem.insert(form->GetFormID());
                                }
                            } else {
                                logger::warn("Invalid hasitem identifier '{}' in hasitem filter of {}", itemStr, path.string());
                            }
                        }
                    }
                }
                
                if (jf.contains("hasitemnot") && jf["hasitemnot"].is_array()) {
                    for (auto const& item : jf["hasitemnot"]) {
                        if (item.is_string()) {
                            std::string itemStr = item.get<std::string>();
                            RE::TESForm* form = nullptr;

                            if (itemStr.find(':') != std::string::npos) {
                                form = GetFormFromIdentifier<RE::TESForm>(itemStr);
                            } else {
                                form = GetFormFromEditorID<RE::TESForm>(itemStr);
                            }

                            if (form) {
                                if (auto* formList = form->As<RE::BGSListForm>()) {
                                    for (auto* el : formList->forms) {
                                        if (el) {
                                            r.filter.hasItemNot.insert(el->GetFormID());
                                        }
                                    }
                                } else {
                                    r.filter.hasItemNot.insert(form->GetFormID());
                                }
                            } else {
                                logger::warn("Invalid hasitemnot identifier '{}' in hasitemnot filter of {}", itemStr, path.string());
                            }
                        }
                    }
                }
                
                if (jf.contains("levels") && jf["levels"].is_array()) {
                    for (auto const& lvl : jf["levels"]) {
                        if (lvl.is_string()) {
                            std::string levelStr = lvl.get<std::string>();

                            if (levelStr.empty()) {
                                logger::warn("Empty level string in levels filter of {}", path.string());
                                continue;
                            }

                            levelStr.erase(std::remove(levelStr.begin(), levelStr.end(), ' '), levelStr.end());
                            LevelCondition condition;
                            
                            if (levelStr.length() >= 2) {
                                if (levelStr.substr(0, 2) == ">=" || levelStr.substr(0, 2) == "<=" || levelStr.substr(0, 2) == "!=") {
                                    condition.operator_type = levelStr.substr(0, 2);
                                    try {
                                        condition.value = std::stoi(levelStr.substr(2));
                                        r.filter.level.push_back(condition);
                                    } catch (const std::exception& e) {
                                        logger::warn("Invalid level value '{}' in levels filter of {}: {}", levelStr, path.string(), e.what());
                                    }
                                } else if (levelStr[0] == '>' || levelStr[0] == '<' || levelStr[0] == '=') {
                                    condition.operator_type = levelStr.substr(0, 1);
                                    try {
                                        condition.value = std::stoi(levelStr.substr(1));
                                        r.filter.level.push_back(condition);
                                    } catch (const std::exception& e) {
                                        logger::warn("Invalid level value '{}' in levels filter of {}: {}", levelStr, path.string(), e.what());
                                    }
                                } else {
                                    logger::warn("Invalid level operator in '{}' in levels filter of {}", levelStr, path.string());
                                }
                            }
                        }
                    }
                }
                
                if (jf.contains("levelsnot") && jf["levelsnot"].is_array()) {
                    for (auto const& lvl : jf["levelsnot"]) {
                        if (lvl.is_string()) {
                            std::string levelStr = lvl.get<std::string>();

                            if (levelStr.empty()) {
                                logger::warn("Empty level string in levelsnot filter of {}", path.string());
                                continue;
                            }

                            levelStr.erase(std::remove(levelStr.begin(), levelStr.end(), ' '), levelStr.end());
                            LevelCondition condition;
                            
                            if (levelStr.length() >= 2) {
                                if (levelStr.substr(0, 2) == ">=" || levelStr.substr(0, 2) == "<=" || levelStr.substr(0, 2) == "!=") {
                                    condition.operator_type = levelStr.substr(0, 2);
                                    try {
                                        condition.value = std::stoi(levelStr.substr(2));
                                        r.filter.levelNot.push_back(condition);
                                    } catch (const std::exception& e) {
                                        logger::warn("Invalid level value '{}' in levelsnot filter of {}: {}", levelStr, path.string(), e.what());
                                    }
                                } else if (levelStr[0] == '>' || levelStr[0] == '<' || levelStr[0] == '=') {
                                    condition.operator_type = levelStr.substr(0, 1);
                                    try {
                                        condition.value = std::stoi(levelStr.substr(1));
                                        r.filter.levelNot.push_back(condition);
                                    } catch (const std::exception& e) {
                                        logger::warn("Invalid level value '{}' in levelsnot filter of {}: {}", levelStr, path.string(), e.what());
                                    }
                                } else {
                                    logger::warn("Invalid level operator in '{}' in levelsnot filter of {}", levelStr, path.string());
                                }
                            }
                        }
                    }
                }                
                
                if (jf.contains("actorvalues") && jf["actorvalues"].is_array()) {
                    for (auto const& av : jf["actorvalues"]) {
                        if (av.is_string()) {
                            std::string avStr = av.get<std::string>();
                            ActorValueCondition condition;
                            
                            std::regex avRegex(R"(([A-Za-z]+)\s*(>=|<=|!=|>|<|=)\s*(\d+(?:\.\d+)?))");
                            std::smatch matches;
                            
                            if (std::regex_match(avStr, matches, avRegex)) {
                                condition.actorValue = tolower_str(matches[1].str());
                                condition.operator_type = matches[2].str();
                                try {
                                    condition.value = std::stof(matches[3].str());
                                    r.filter.actorValue.push_back(condition);
                                } catch (const std::exception& e) {
                                    logger::warn("Invalid actor value '{}' in actorvalues filter of {}: {}", avStr, path.string(), e.what());
                                }
                            } else {
                                logger::warn("Invalid actor value format '{}' in actorvalues filter of {}, expected format: 'ActorValue >= Value'", avStr, path.string());
                            }
                        }
                    }
                }
                
                if (jf.contains("actorvaluesnot") && jf["actorvaluesnot"].is_array()) {
                    for (auto const& av : jf["actorvaluesnot"]) {
                        if (av.is_string()) {
                            std::string avStr = av.get<std::string>();
                            ActorValueCondition condition;
                            
                            std::regex avRegex(R"(([A-Za-z]+)\s*(>=|<=|!=|>|<|=)\s*(\d+(?:\.\d+)?))");
                            std::smatch matches;
                            
                            if (std::regex_match(avStr, matches, avRegex)) {
                                condition.actorValue = tolower_str(matches[1].str());
                                condition.operator_type = matches[2].str();
                                try {
                                    condition.value = std::stof(matches[3].str());
                                    r.filter.actorValueNot.push_back(condition);
                                } catch (const std::exception& e) {
                                    logger::warn("Invalid actor value '{}' in actorvaluesnot filter of {}: {}", avStr, path.string(), e.what());
                                }
                            } else {
                                logger::warn("Invalid actor value format '{}' in actorvaluesnot filter of {}, expected format: 'ActorValue >= Value'", avStr, path.string());
                            }
                        }
                    }
                }     

                if (jf.contains("actorkeywords") && jf["actorkeywords"].is_array()) {
                    for (const auto& kwEntry : jf["actorkeywords"]) {
                        if (kwEntry.is_string()) {
                            std::string kwStr = kwEntry.get<std::string>();
                            RE::TESForm* form = nullptr;
                            
                            if (kwStr.find(':') != std::string::npos) {
                                form = GetFormFromIdentifier<RE::TESForm>(kwStr);
                            } else {
                                form = GetFormFromEditorID<RE::BGSKeyword>(kwStr);
                                if (!form) {
                                    form = GetFormFromEditorID<RE::BGSListForm>(kwStr);
                                }
                                if (!form) {
                                    form = GetFormFromEditorID<RE::TESForm>(kwStr);
                                }     
                            }
                            
                            if (form) {
                                if (auto* kw = form->As<RE::BGSKeyword>()) {
                                    r.filter.actorKeywords.insert(kw);
                                } else if (auto* formList = form->As<RE::BGSListForm>()) {
                                    for (auto* el : formList->forms) {
                                        if (el) {
                                            if (auto* listKw = el->As<RE::BGSKeyword>()) {
                                                r.filter.actorKeywords.insert(listKw);
                                            }
                                        }
                                    }
                                } else {
                                    logger::warn("Invalid actorkeywords form '{}' (not Keyword or FormList) in actorkeywords filter of {}", kwStr, path.string());
                                }
                            } else {
                                logger::warn("Keyword not found: '{}' in actorkeywords filter of {}", kwStr, path.string());
                            }
                        }
                    }
                }

                if (jf.contains("actorkeywordsnot") && jf["actorkeywordsnot"].is_array()) {
                    for (const auto& kwEntry : jf["actorkeywordsnot"]) {
                        if (kwEntry.is_string()) {
                            std::string kwStr = kwEntry.get<std::string>();
                            RE::TESForm* form = nullptr;
                            
                            if (kwStr.find(':') != std::string::npos) {
                                form = GetFormFromIdentifier<RE::TESForm>(kwStr);
                            } else {
                                form = GetFormFromEditorID<RE::BGSKeyword>(kwStr);
                                if (!form) {
                                    form = GetFormFromEditorID<RE::BGSListForm>(kwStr);
                                }
                                if (!form) {
                                    form = GetFormFromEditorID<RE::TESForm>(kwStr);
                                }     
                            }
                            
                            if (form) {
                                if (auto* kw = form->As<RE::BGSKeyword>()) {
                                    r.filter.actorKeywordsNot.insert(kw);
                                } else if (auto* formList = form->As<RE::BGSListForm>()) {
                                    for (auto* el : formList->forms) {
                                        if (el) {
                                            if (auto* listKw = el->As<RE::BGSKeyword>()) {
                                                r.filter.actorKeywordsNot.insert(listKw);
                                            }
                                        }
                                    }
                                } else {
                                    logger::warn("Invalid actorkeywordsnot form '{}' (not Keyword or FormList) in actorkeywordsnot filter of {}", kwStr, path.string());
                                }
                            } else {
                                logger::warn("Keyword not found: '{}' in actorkeywordsnot filter of {}", kwStr, path.string());
                            }
                        }
                    }
                }

                if (jf.contains("actorraces") && jf["actorraces"].is_array()) {
                    for (const auto& race : jf["actorraces"]) {
                        if (race.is_string()) {
                            std::string raceStr = race.get<std::string>();
                            RE::TESForm* form = nullptr;
                            
                            if (raceStr.find(':') != std::string::npos) {
                                form = GetFormFromIdentifier<RE::TESForm>(raceStr);
                            } else {
                                form = GetFormFromEditorID<RE::TESForm>(raceStr);
                            }
                            
                            if (form) {
                                if (auto* raceForm = form->As<RE::TESRace>()) {
                                    r.filter.actorRaces.insert(raceForm);
                                } else if (auto* formList = form->As<RE::BGSListForm>()) {
                                    for (auto* el : formList->forms) {
                                        if (el) {
                                            if (auto* listRace = el->As<RE::TESRace>()) {
                                                r.filter.actorRaces.insert(listRace);
                                            }
                                        }
                                    }
                                } else {
                                    logger::warn("Invalid actorraces form '{}' (not Race or FormList) in actorraces filter of {}", raceStr, path.string());
                                }
                            } else {
                                logger::warn("Invalid actorrace identifier '{}' in actorraces filter of {}", raceStr, path.string());
                            }
                        }
                    }
                }

                if (jf.contains("actorracesnot") && jf["actorracesnot"].is_array()) {
                    for (const auto& race : jf["actorracesnot"]) {
                        if (race.is_string()) {
                            std::string raceStr = race.get<std::string>();
                            RE::TESForm* form = nullptr;
                            
                            if (raceStr.find(':') != std::string::npos) {
                                form = GetFormFromIdentifier<RE::TESForm>(raceStr);
                            } else {
                                form = GetFormFromEditorID<RE::TESForm>(raceStr);
                            }
                            
                            if (form) {
                                if (auto* raceForm = form->As<RE::TESRace>()) {
                                    r.filter.actorRacesNot.insert(raceForm);
                                } else if (auto* formList = form->As<RE::BGSListForm>()) {
                                    for (auto* el : formList->forms) {
                                        if (el) {
                                            if (auto* listRace = el->As<RE::TESRace>()) {
                                                r.filter.actorRacesNot.insert(listRace);
                                            }
                                        }
                                    }
                                } else {
                                    logger::warn("Invalid actorracesnot form '{}' (not Race or FormList) in actorracesnot filter of {}", raceStr, path.string());
                                }
                            } else {
                                logger::warn("Invalid actorrace identifier '{}' in actorracesnot filter of {}", raceStr, path.string());
                            }
                        }
                    }
                }

                if (jf.contains("issneaking") && jf["issneaking"].is_number_unsigned()) {
                    try {
                        r.filter.isSneaking = jf["issneaking"].get<std::uint32_t>();
                    } catch (const std::exception& e) {
                        logger::warn("Invalid integer value in issneaking filter of {}: {}", path.string(), e.what());
                    }
                }

                if (jf.contains("isswimming") && jf["isswimming"].is_number_unsigned()) {
                    try {
                        r.filter.isSwimming = jf["isswimming"].get<std::uint32_t>();
                    } catch (const std::exception& e) {
                        logger::warn("Invalid integer value in isswimming filter of {}: {}", path.string(), e.what());
                    }
                }

                if (jf.contains("isincombat") && jf["isincombat"].is_number_unsigned()) {
                    try {
                        r.filter.isInCombat = jf["isincombat"].get<std::uint32_t>();
                    } catch (const std::exception& e) {
                        logger::warn("Invalid integer value in isincombat filter of {}: {}", path.string(), e.what());
                    }
                }

                if (jf.contains("ismounted") && jf["ismounted"].is_number_unsigned()) {
                    try {
                        r.filter.isMounted = jf["ismounted"].get<std::uint32_t>();
                    } catch (const std::exception& e) {
                        logger::warn("Invalid integer value in ismounted filter of {}: {}", path.string(), e.what());
                    }
                }

                if (jf.contains("isdualcasting") && jf["isdualcasting"].is_number_unsigned()) {
                    try {
                        r.filter.isDualCasting = jf["isdualcasting"].get<std::uint32_t>();
                    } catch (const std::exception& e) {
                        logger::warn("Invalid integer value in isdualcasting filter of {}: {}", path.string(), e.what());
                    }
                }

                if (jf.contains("issprinting") && jf["issprinting"].is_number_unsigned()) {
                    try {
                        r.filter.isSprinting = jf["issprinting"].get<std::uint32_t>();
                    } catch (const std::exception& e) {
                        logger::warn("Invalid integer value in issprinting filter of {}: {}", path.string(), e.what());
                    }
                }

                if (jf.contains("isweapondrawn") && jf["isweapondrawn"].is_number_unsigned()) {
                    try {
                        r.filter.isWeaponDrawn = jf["isweapondrawn"].get<std::uint32_t>();
                    } catch (const std::exception& e) {
                        logger::warn("Invalid integer value in isweapondrawn filter of {}: {}", path.string(), e.what());
                    }
                }

                if (jf.contains("isinitiallydisabled") && jf["isinitiallydisabled"].is_number_unsigned()) {
                    try {
                        r.filter.isInitiallyDisabled = jf["isinitiallydisabled"].get<std::uint32_t>();
                    } catch (const std::exception& e) {
                        logger::warn("Invalid integer value in isinitiallydisabled filter of {}: {}", path.string(), e.what());
                    }
                }

                if (jf.contains("nearbyobjects") && jf["nearbyobjects"].is_array()) {
					for (const auto& entry : jf["nearbyobjects"]) {
						if (entry.is_object()) {
							std::string formIdStr;
							RadiusCondition radiusCondition;

							if (entry.contains("radius") && (entry["radius"].is_number() || entry["radius"].is_object())) {
								if (entry["radius"].is_number()) {
									try {
										radiusCondition.value = entry["radius"].get<float>();
										radiusCondition.useRandom = false;
									} catch (const std::exception& e) {
										logger::warn("Invalid radius value in nearbyobjects filter of {}: {}", path.string(), e.what());
										radiusCondition.value = 100.0;
									}
								} else if (entry["radius"].is_object()) {
									const auto& radiusObj = entry["radius"];

									if (radiusObj.contains("min") && radiusObj["min"].is_number() &&
										radiusObj.contains("max") && radiusObj["max"].is_number()) {
										try {
											radiusCondition.min = radiusObj["min"].get<float>();
											radiusCondition.max = radiusObj["max"].get<float>();
											radiusCondition.useRandom = true;
											radiusCondition.value = 100.0;
										} catch (const std::exception& e) {
											logger::warn("Invalid random radius values in nearbyobjects filter of {}: {}", path.string(), e.what());
											radiusCondition.value = 100.0;
											radiusCondition.useRandom = false;
										}
									} else {
										logger::warn("Invalid radius object format in nearbyobjects filter of {}: missing min/max values", path.string());
										radiusCondition.value = 100.0;
										radiusCondition.useRandom = false;
									}
								}
							} else {
								radiusCondition.value = 100.0;
								radiusCondition.useRandom = false;
							}

							if (entry.contains("formid") && entry["formid"].is_string()) {
								formIdStr = entry["formid"].get<std::string>();

								auto* form = GetFormFromIdentifier<RE::TESForm>(formIdStr);
								if (form) {
									if (auto* formList = form->As<RE::BGSListForm>()) {
										for (auto* el : formList->forms) {
											if (el) {
												r.filter.nearby.push_back({ el, radiusCondition });
											}
										}
									} else {
										r.filter.nearby.push_back({ form, radiusCondition });
									}
								} else {
									logger::warn("Invalid form '{}' in nearbyobjects filter of {}", formIdStr, path.string());
								}
							} else if (entry.contains("editorid") && entry["editorid"].is_string()) {
								formIdStr = entry["editorid"].get<std::string>();

								auto* form = GetFormFromEditorID<RE::TESForm>(formIdStr);
								if (form) {
									if (auto* formList = form->As<RE::BGSListForm>()) {
										for (auto* el : formList->forms) {
											if (el) {
												r.filter.nearby.push_back({ el, radiusCondition });
											}
										}
									} else {
										r.filter.nearby.push_back({ form, radiusCondition });
									}
								} else {
									logger::warn("Invalid form '{}' in nearbyobjects filter of {}", formIdStr, path.string());
								}
							}
						}
					}
				}

				if (jf.contains("nearbyobjectsnot") && jf["nearbyobjectsnot"].is_array()) {
					for (const auto& entry : jf["nearbyobjectsnot"]) {
						if (entry.is_object()) {
							std::string formIdStr;
							RadiusCondition radiusCondition;

							if (entry.contains("radius") && (entry["radius"].is_number() || entry["radius"].is_object())) {
								if (entry["radius"].is_number()) {
									try {
										radiusCondition.value = entry["radius"].get<float>();
										radiusCondition.useRandom = false;
									} catch (const std::exception& e) {
										logger::warn("Invalid radius value in nearbyobjectsnot filter of {}: {}", path.string(), e.what());
										radiusCondition.value = 100.0;
									}
								} else if (entry["radius"].is_object()) {
									const auto& radiusObj = entry["radius"];

									if (radiusObj.contains("min") && radiusObj["min"].is_number() &&
										radiusObj.contains("max") && radiusObj["max"].is_number()) {
										try {
											radiusCondition.min = radiusObj["min"].get<float>();
											radiusCondition.max = radiusObj["max"].get<float>();
											radiusCondition.useRandom = true;
											radiusCondition.value = 100.0;
										} catch (const std::exception& e) {
											logger::warn("Invalid random radius values in nearbyobjectsnot filter of {}: {}", path.string(), e.what());
											radiusCondition.value = 100.0;
											radiusCondition.useRandom = false;
										}
									} else {
										logger::warn("Invalid radius object format in nearbyobjectsnot filter of {}: missing min/max values", path.string());
										radiusCondition.value = 100.0;
										radiusCondition.useRandom = false;
									}
								}
							} else {
								radiusCondition.value = 100.0;
								radiusCondition.useRandom = false;
							}

							if (entry.contains("formid") && entry["formid"].is_string()) {
								formIdStr = entry["formid"].get<std::string>();

								auto* form = GetFormFromIdentifier<RE::TESForm>(formIdStr);
								if (form) {
									if (auto* formList = form->As<RE::BGSListForm>()) {
										for (auto* el : formList->forms) {
											if (el) {
												r.filter.nearbyNot.push_back({ el, radiusCondition });
											}
										}
									} else {
										r.filter.nearbyNot.push_back({ form, radiusCondition });
									}
								} else {
									logger::warn("Invalid form '{}' in nearbyobjectsnot filter of {}", formIdStr, path.string());
								}
							} else if (entry.contains("editorid") && entry["editorid"].is_string()) {
								formIdStr = entry["editorid"].get<std::string>();

								auto* form = GetFormFromEditorID<RE::TESForm>(formIdStr);
								if (form) {
									if (auto* formList = form->As<RE::BGSListForm>()) {
										for (auto* el : formList->forms) {
											if (el) {
												r.filter.nearbyNot.push_back({ el, radiusCondition });
											}
										}
									} else {
										r.filter.nearbyNot.push_back({ form, radiusCondition });
									}
								} else {
									logger::warn("Invalid form '{}' in nearbyobjectsnot filter of {}", formIdStr, path.string());
								}
							}
						}
					}
				}

				if (jf.contains("timer") && (jf["timer"].is_number() || jf["timer"].is_object())) {
					if (jf["timer"].is_number()) {
						try {
							r.filter.timer.time.value = jf["timer"].get<float>();
							r.filter.timer.time.useRandom = false;
							r.filter.timer.matchFilterRecheck = 0;
						} catch (const std::exception& e) {
							logger::warn("Invalid timer value in timer filter of {}: {}", path.string(), e.what());
						}
					} else if (jf["timer"].is_object()) {
						const auto& timerObj = jf["timer"];

						if (timerObj.contains("time")) {
							if (timerObj["time"].is_number()) {
								try {
									r.filter.timer.time.value = timerObj["time"].get<float>();
									r.filter.timer.time.useRandom = false;
								} catch (const std::exception& e) {
									logger::warn("Invalid time value in timer filter of {}: {}", path.string(), e.what());
								}
							} else if (timerObj["time"].is_object()) {
								const auto& timeObj = timerObj["time"];
								if (timeObj.contains("min") && timeObj["min"].is_number() &&
									timeObj.contains("max") && timeObj["max"].is_number()) {
									try {
										r.filter.timer.time.min = timeObj["min"].get<float>();
										r.filter.timer.time.max = timeObj["max"].get<float>();
										r.filter.timer.time.useRandom = true;
										r.filter.timer.time.value = 0.0f;
									} catch (const std::exception& e) {
										logger::warn("Invalid random timer values in timer filter of {}: {}", path.string(), e.what());
									}
								} else {
									logger::warn("Invalid timer time object format in timer filter of {}: missing min/max values", path.string());
								}
							}
						}

						if (timerObj.contains("matchfilterrecheck") && timerObj["matchfilterrecheck"].is_number_unsigned()) {
							try {
								r.filter.timer.matchFilterRecheck = timerObj["matchfilterrecheck"].get<std::uint32_t>();
							} catch (const std::exception& e) {
								logger::warn("Invalid matchFilterRecheck value in timer filter of {}: {}", path.string(), e.what());
							}
						}
					}
				}

                if (jf.contains("time") && jf["time"].is_array()) {
                    for (auto const& tm : jf["time"]) {
                        if (tm.is_string()) {
                            std::string timeStr = tm.get<std::string>();

                            if (timeStr.empty()) {
                                logger::warn("Empty time string in time filter of {}", path.string());
                                continue;
                            }
                            
                            std::regex timeRegex(R"(([A-Za-z]+)\s*(>=|<=|!=|>|<|=)\s*(\d+(?:\.\d+)?))");
                            std::smatch matches;
                            
                            if (std::regex_match(timeStr, matches, timeRegex)) {
                                TimeCondition condition;
                                condition.field = tolower_str(matches[1].str());
                                condition.operator_type = matches[2].str();
                                try {
                                    condition.value = std::stof(matches[3].str());
                                    r.filter.time.push_back(condition);
                                } catch (const std::exception& e) {
                                    logger::warn("Invalid time value '{}' in time filter of {}: {}", timeStr, path.string(), e.what());
                                }
                            } else {
                                logger::warn("Invalid time format '{}' in time filter of {}, expected format: 'field operator value'", timeStr, path.string());
                            }
                        }
                    }
                }

                if (jf.contains("timenot") && jf["timenot"].is_array()) {
                    for (auto const& tm : jf["timenot"]) {
                        if (tm.is_string()) {
                            std::string timeStr = tm.get<std::string>();

                            if (timeStr.empty()) {
                                logger::warn("Empty time string in timenot filter of {}", path.string());
                                continue;
                            }
                            
                            std::regex timeRegex(R"(([A-Za-z]+)\s*(>=|<=|!=|>|<|=)\s*(\d+(?:\.\d+)?))");
                            std::smatch matches;
                            
                            if (std::regex_match(timeStr, matches, timeRegex)) {
                                TimeCondition condition;
                                condition.field = tolower_str(matches[1].str());
                                condition.operator_type = matches[2].str();
                                try {
                                    condition.value = std::stof(matches[3].str());
                                    r.filter.timeNot.push_back(condition);
                                } catch (const std::exception& e) {
                                    logger::warn("Invalid time value '{}' in timenot filter of {}: {}", timeStr, path.string(), e.what());
                                }
                            } else {
                                logger::warn("Invalid time format '{}' in timenot filter of {}, expected format: 'field operator value'", timeStr, path.string());
                            }
                        }
                    }
                }

                if (jf.contains("destructionstage") && jf["destructionstage"].is_number_integer()) {
                    try {
                        r.filter.destructionStage = jf["destructionstage"].get<std::int32_t>();
                    } catch (const std::exception& e) {
                        logger::warn("Invalid destruction stage value in destructionstage filter of {}: {}", path.string(), e.what());
                    }
                }

				if (jf.contains("isstacked") && jf["isstacked"].is_number_unsigned()) {
					try {
						r.filter.isStacked = jf["isstacked"].get<std::uint32_t>();
					} catch (const std::exception& e) {
						logger::warn("Invalid isstacked value in isstacked filter of {}: {}", path.string(), e.what());
					}
				}

				if (jf.contains("isinterior") && jf["isinterior"].is_number_unsigned()) {
					try {
						r.filter.isInterior = jf["isinterior"].get<std::uint32_t>();
					} catch (const std::exception& e) {
						logger::warn("Invalid isinterior value in isinterior filter of {}: {}", path.string(), e.what());
					}
				}

				if (jf.contains("position") && jf["position"].is_number_unsigned()) {
					try {
						r.filter.position = jf["position"].get<std::uint32_t>();
					} catch (const std::exception& e) {
						logger::warn("Invalid position value in position filter of {}: {}", path.string(), e.what());
					}
				}

				if (jf.contains("isFirstPerson") && jf["isFirstPerson"].is_number_unsigned()) {
					try {
						r.filter.isFirstPerson = jf["isFirstPerson"].get<std::uint32_t>();
					} catch (const std::exception& e) {
						logger::warn("Invalid isFirstPerson value in isFirstPerson filter of {}: {}", path.string(), e.what());
					}
				}

				if (jf.contains("isThirdPerson") && jf["isThirdPerson"].is_number_unsigned()) {
					try {
						r.filter.isThirdPerson = jf["isThirdPerson"].get<std::uint32_t>();
					} catch (const std::exception& e) {
						logger::warn("Invalid isThirdPerson value in isThirdPerson filter of {}: {}", path.string(), e.what());
					}
				}
            }           

            if (!hasObjectIdentifier) {
                logger::warn("Skipping rule in {}: no valid object identifiers (formTypes, formIDs, editorIDs, formLists, or keywords)", path.string());
                continue;
            }

            // ╔════════════════════════════════════╗
			// ║              EFFECTS               ║
			// ╚════════════════════════════════════╝

            if (!jr.contains("effect")) {
                logger::warn("Skipping rule in {}: missing 'effect' field", path.string());
                continue;
            }

            std::vector<json> effectArray;
            const auto& je = jr["effect"];
            if (je.is_array()) {
                effectArray = je.get<std::vector<json>>();
            } else if (je.is_object()) {
                effectArray = { je };
            } else {
                logger::warn("Skipping rule in {}: 'effect' is not object or array", path.string());
                continue;
            }

            static const std::unordered_map<std::string, EffectType> effectTypeMap = {
                {"removeitem", EffectType::kRemoveItem},
                {"disableitem", EffectType::kDisableItem},
                {"enableitem", EffectType::kEnableItem},
                {"spawnitem", EffectType::kSpawnItem},
                {"spawnspell", EffectType::kSpawnSpell},
                {"spawnspellonitem", EffectType::kSpawnSpellOnItem},
				{"applyspell", EffectType::kApplySpell},
                {"spawnactor", EffectType::kSpawnActor},
                {"spawnimpactdataset", EffectType::kSpawnImpactDataSet},
                {"spawnexplosion", EffectType::kSpawnExplosion},
                {"swapitem", EffectType::kSwapItem},
                {"playsound", EffectType::kPlaySound},
                {"spillinventory", EffectType::kSpillInventory},
                {"swapactor", EffectType::kSwapActor},
                {"spawnleveleditem", EffectType::kSpawnLeveledItem},
                {"swapleveleditem", EffectType::kSwapLeveledItem},
                {"spawnleveledspell", EffectType::kSpawnLeveledSpell},
                {"spawnleveledspellonitem", EffectType::kSpawnLeveledSpellOnItem},
                {"spawnleveledactor", EffectType::kSpawnLeveledActor},
                {"swapleveledactor", EffectType::kSwapLeveledActor},
                {"applyingestible", EffectType::kApplyIngestible},
                {"applyotheringestible", EffectType::kApplyOtherIngestible},
                {"spawnlight", EffectType::kSpawnLight},
                {"removelight", EffectType::kRemoveLight},
                {"enablelight", EffectType::kEnableLight},
                {"disablelight", EffectType::kDisableLight},
                {"playidle", EffectType::kPlayIdle},
                {"spawneffectshader", EffectType::kSpawnEffectShader},
                {"spawneffectshaderonitem", EffectType::kSpawnEffectShaderOnItem},
                {"togglenode", EffectType::kToggleNode},
                //{"toggleshaderflag", EffectType::kToggleShaderFlag},
                {"unlockitem", EffectType::kUnlockItem},
                {"lockitem", EffectType::kLockItem},
                {"activateitem", EffectType::kActivateItem},
                {"addcontaineritem", EffectType::kAddContainerItem},
                {"addactoritem", EffectType::kAddActorItem},
                {"removecontaineritem", EffectType::kRemoveContainerItem},
                {"removeactoritem", EffectType::kRemoveActorItem},
                {"addactorspell", EffectType::kAddActorSpell},
                {"removeactorspell", EffectType::kRemoveActorSpell},
                {"addactorperk", EffectType::kAddActorPerk},
                {"removeactorperk", EffectType::kRemoveActorPerk},
                {"spawnartobject", EffectType::kSpawnArtObject},
                {"spawnartobjectonitem", EffectType::kSpawnArtObjectOnItem},
                {"executeconsolecommand", EffectType::kExecuteConsoleCommand},
                {"executeconsolecommandonitem", EffectType::kExecuteConsoleCommandOnItem},
				{"executeconsolecommandonsource", EffectType::kExecuteConsoleCommandOnSource},
                {"shownotification", EffectType::kShowNotification},
                {"showmessagebox", EffectType::kShowMessageBox}
            };

			static const std::unordered_map<EffectType, std::string> effectTypeReverseMap = []() {
				std::unordered_map<EffectType, std::string> map;
				for (const auto& [name, type] : effectTypeMap) {
					map[type] = name;
				}
				return map;
			}();

            for (const auto& effj : effectArray) {
                if (!effj.is_object()) continue;
                Effect eff;

                std::string typeStrRaw = effj.value("type", "spillinventory");
                std::string typeStr = tolower_str(typeStrRaw);
                auto it = effectTypeMap.find(typeStr);
                if (it == effectTypeMap.end()) {
                    logger::warn("Unknown effect type '{}' in {}", typeStr, path.string());
                    continue;
                }
				eff.type = it->second;

                const std::vector<EffectType> effectsWithoutItems = {
                    EffectType::kRemoveItem,
                    EffectType::kDisableItem,
                    EffectType::kEnableItem,
                    EffectType::kSpillInventory,
                    EffectType::kUnlockItem,
                    EffectType::kLockItem,
                    EffectType::kActivateItem
                };

                bool needsItems = std::find(effectsWithoutItems.begin(), effectsWithoutItems.end(), eff.type) == effectsWithoutItems.end();
                if (needsItems) {
                    if (effj.contains("items") && effj["items"].is_array()) {
						for (const auto& itemJson : effj["items"]) {
							EffectExtendedData extData;
							extData.formID = nullptr;
							extData.nonDeletable = itemJson.value("nondeletable", 0U);
							extData.spawnType = itemJson.value("spawntype", 4U);
							extData.fade = itemJson.value("fade", 1U);
							extData.duration = itemJson.value("duration", 1.0f);
							extData.string = itemJson.value("string", std::string{});
							extData.mode = itemJson.value("mode", 0U);
							extData.strings = itemJson.value("strings", std::vector<std::string>{});
							//extData.flagNames = itemJson.value("flagnames", std::vector<std::string>{});
							extData.rank = itemJson.value("rank", 0U);
							if (itemJson.contains("count") && (itemJson["count"].is_number_unsigned() || itemJson["count"].is_object())) {
								if (itemJson["count"].is_number_unsigned()) {
									try {
										extData.count.value = itemJson["count"].get<std::uint32_t>();
										extData.count.useRandom = false;
									} catch (const std::exception& e) {
										logger::warn("Invalid count value in items of {}: {}", path.string(), e.what());
									}
								} else if (itemJson["count"].is_object()) {
									const auto& countObj = itemJson["count"];
									if (countObj.contains("min") && countObj["min"].is_number_unsigned() &&
										countObj.contains("max") && countObj["max"].is_number_unsigned()) {
										try {
											extData.count.min = countObj["min"].get<std::uint32_t>();
											extData.count.max = countObj["max"].get<std::uint32_t>();
											extData.count.useRandom = true;
											extData.count.value = 1;
										} catch (const std::exception& e) {
											logger::warn("Invalid random count values in items of {}: {}", path.string(), e.what());
										}
									} else {
										logger::warn("Invalid count object format in items of {}: missing min/max values", path.string());
									}
								}
							} else {
								extData.count.value = 1;
								extData.count.useRandom = false;
							}

							if (itemJson.contains("chance") && (itemJson["chance"].is_number() || itemJson["chance"].is_object())) {
								if (itemJson["chance"].is_number()) {
									try {
										extData.chance.value = itemJson["chance"].get<float>();
										extData.chance.useRandom = false;
									} catch (const std::exception& e) {
										logger::warn("Invalid chance value in items of {}: {}", path.string(), e.what());
									}
								} else if (itemJson["chance"].is_object()) {
									const auto& chanceObj = itemJson["chance"];
									if (chanceObj.contains("min") && chanceObj["min"].is_number() &&
										chanceObj.contains("max") && chanceObj["max"].is_number()) {
										try {
											extData.chance.min = chanceObj["min"].get<float>();
											extData.chance.max = chanceObj["max"].get<float>();
											extData.chance.useRandom = true;
											extData.chance.value = 100.0f;
										} catch (const std::exception& e) {
											logger::warn("Invalid random chance values in items of {}: {}", path.string(), e.what());
										}
									} else {
										logger::warn("Invalid chance object format in items of {}: missing min/max values", path.string());
									}
								}
							} else {
								extData.chance.value = 100.0f;
								extData.chance.useRandom = false;
							}

							if (itemJson.contains("radius") && (itemJson["radius"].is_number() || itemJson["radius"].is_object())) {
								if (itemJson["radius"].is_number()) {
									try {
										extData.radius.value = itemJson["radius"].get<float>();
										extData.radius.useRandom = false;
									} catch (const std::exception& e) {
										logger::warn("Invalid radius value in items of {}: {}", path.string(), e.what());
										extData.radius.value = 100.0;
										extData.radius.useRandom = false;
									}
								} else if (itemJson["radius"].is_object()) {
									const auto& radiusObj = itemJson["radius"];

									if (radiusObj.contains("min") && radiusObj["min"].is_number() &&
										radiusObj.contains("max") && radiusObj["max"].is_number()) {
										try {
											extData.radius.min = radiusObj["min"].get<float>();
											extData.radius.max = radiusObj["max"].get<float>();
											extData.radius.useRandom = true;
											extData.radius.value = 100.0;
										} catch (const std::exception& e) {
											logger::warn("Invalid random radius values in items of {}: {}", path.string(), e.what());
											extData.radius.value = 100.0;
											extData.radius.useRandom = false;
										}
									} else {
										logger::warn("Invalid radius object format in items of {}: missing min/max values", path.string());
										extData.radius.value = 100.0;
										extData.radius.useRandom = false;
									}
								}
							} else {
								extData.radius.value = 100.0;
								extData.radius.useRandom = false;
							}

							if (itemJson.contains("scale") && (itemJson["scale"].is_number() || itemJson["scale"].is_object())) {
								if (itemJson["scale"].is_number()) {
									try {
										extData.scale.value = itemJson["scale"].get<float>();
										extData.scale.useRandom = false;
									} catch (const std::exception& e) {
										logger::warn("Invalid scale value in items of {}: {}", path.string(), e.what());
										extData.scale.value = -1.0f;
										extData.scale.useRandom = false;
									}
								} else if (itemJson["scale"].is_object()) {
									const auto& scaleObj = itemJson["scale"];

									if (scaleObj.contains("min") && scaleObj["min"].is_number() &&
										scaleObj.contains("max") && scaleObj["max"].is_number()) {
										try {
											extData.scale.min = scaleObj["min"].get<float>();
											extData.scale.max = scaleObj["max"].get<float>();
											extData.scale.useRandom = true;
											extData.scale.value = -1.0f;
										} catch (const std::exception& e) {
											logger::warn("Invalid random scale values in items of {}: {}", path.string(), e.what());
											extData.scale.value = -1.0f;
											extData.scale.useRandom = false;
										}
									} else {
										logger::warn("Invalid scale object format in items of {}: missing min/max values", path.string());
										extData.scale.value = -1.0f;
										extData.scale.useRandom = false;
									}
								}
							} else {
								extData.scale.value = -1.0f;
								extData.scale.useRandom = false;
							}

							if (itemJson.contains("timer") && (itemJson["timer"].is_number() || itemJson["timer"].is_object())) {
								if (itemJson["timer"].is_number()) {
									try {
										extData.timer.time.value = itemJson["timer"].get<float>();
										extData.timer.time.useRandom = false;
										extData.timer.matchFilterRecheck = 0;
									} catch (const std::exception& e) {
										logger::warn("Invalid timer value in items of {}: {}", path.string(), e.what());
									}
								} else if (itemJson["timer"].is_object()) {
									const auto& timerObj = itemJson["timer"];

									if (timerObj.contains("time")) {
										if (timerObj["time"].is_number()) {
											try {
												extData.timer.time.value = timerObj["time"].get<float>();
												extData.timer.time.useRandom = false;
											} catch (const std::exception& e) {
												logger::warn("Invalid time value in timer of items in {}: {}", path.string(), e.what());
											}
										} else if (timerObj["time"].is_object()) {
											const auto& timeObj = timerObj["time"];
											if (timeObj.contains("min") && timeObj["min"].is_number() &&
												timeObj.contains("max") && timeObj["max"].is_number()) {
												try {
													extData.timer.time.min = timeObj["min"].get<float>();
													extData.timer.time.max = timeObj["max"].get<float>();
													extData.timer.time.useRandom = true;
													extData.timer.time.value = 0.0f;
												} catch (const std::exception& e) {
													logger::warn("Invalid random timer values in items of {}: {}", path.string(), e.what());
												}
											} else {
												logger::warn("Invalid timer time object format in items of {}: missing min/max values", path.string());
											}
										}
									}

									if (timerObj.contains("matchfilterrecheck") && timerObj["matchfilterrecheck"].is_number_unsigned()) {
										try {
											extData.timer.matchFilterRecheck = timerObj["matchfilterrecheck"].get<std::uint32_t>();
										} catch (const std::exception& e) {
											logger::warn("Invalid matchFilterRecheck value in timer of items in {}: {}", path.string(), e.what());
										}
									}
								}
							}

							bool haveIdentifier = false;

							if (itemJson.contains("formid") && itemJson["formid"].is_string()) {
								std::string formId = itemJson["formid"].get<std::string>();
								if (auto* form = GetFormFromIdentifier<RE::TESForm>(formId)) {
									extData.formID = form;
									extData.isFormList = false;
									extData.index = -1;
									haveIdentifier = true;
								} else {
									logger::warn("Invalid formid '{}' in items of {}", formId, path.string());
								}
							}

							else if (itemJson.contains("editorid") && itemJson["editorid"].is_string()) {
								std::string editorId = itemJson["editorid"].get<std::string>();
								if (auto* form = GetFormFromEditorID<RE::TESForm>(editorId)) {
									extData.formID = form;
									extData.isFormList = false;
									extData.index = -1;
									haveIdentifier = true;
								} else {
									logger::warn("Invalid editorid '{}' in items of {}", editorId, path.string());
								}
							}

							else if (itemJson.contains("formlist") && itemJson["formlist"].is_array()) {
								for (const auto& formListEntry : itemJson["formlist"]) {
									if (formListEntry.is_object() && formListEntry.contains("formid") && formListEntry["formid"].is_string()) {
										std::string formListId = formListEntry["formid"].get<std::string>();
										int idx = formListEntry.value("index", -1);

										auto* list = GetFormFromIdentifier<RE::BGSListForm>(formListId);
										if (list) {
											extData.formID = list;
											extData.isFormList = true;
											extData.index = idx;
											eff.items.emplace_back(extData.formID, extData);
										} else {
											logger::warn("Invalid formlist '{}' in items of {}", formListId, path.string());
										}
									} else if (formListEntry.is_object() && formListEntry.contains("editorid") && formListEntry["editorid"].is_string()) {
										std::string editorId = formListEntry["editorid"].get<std::string>();
										int idx = formListEntry.value("index", -1);

										auto* list = GetFormFromEditorID<RE::BGSListForm>(editorId);
										if (list) {
											extData.formID = list;
											extData.isFormList = true;
											extData.index = idx;
											eff.items.emplace_back(extData.formID, extData);
										} else {
											logger::warn("Invalid formlist '{}' in items of {}", editorId, path.string());
										}
									} else {
										logger::warn("Invalid formlist entry in items of {}", path.string());
									}
								}
							}

							const std::vector<EffectType> effectsWithoutForm = {
								EffectType::kApplyIngestible,
								EffectType::kApplySpell,
								EffectType::kRemoveLight,
								EffectType::kEnableLight,
								EffectType::kDisableLight,
								EffectType::kPlayIdle,
								EffectType::kToggleNode,
								//EffectType::kToggleShaderFlag,
								EffectType::kUnlockItem,
								EffectType::kLockItem,
								EffectType::kActivateItem,
								EffectType::kExecuteConsoleCommand,
								EffectType::kExecuteConsoleCommandOnItem,
								EffectType::kExecuteConsoleCommandOnSource,
								EffectType::kShowNotification,
								EffectType::kShowMessageBox
							};

							bool needsFormNot = std::find(effectsWithoutForm.begin(), effectsWithoutForm.end(), eff.type) == effectsWithoutForm.end();

							if (haveIdentifier || !needsFormNot) {
								auto formToAdd = needsFormNot ? extData.formID : nullptr;
								eff.items.emplace_back(formToAdd, extData);
							}

							if (eff.items.empty() && !needsFormNot) {
								logger::warn("Effect '{}' in {} requires formID, editorID, or formlist, but none were provided or the format is incorrect", effectTypeReverseMap.at(eff.type), path.string());
								continue;
							}
						}
					} else {
						logger::warn("Effect '{}' in {} requires 'items', but none were provided or the format is incorrect", effectTypeReverseMap.at(eff.type), path.string());
						continue;
					}
                }
                r.effects.push_back(eff);
            }

            if (r.effects.empty()) {
                logger::warn("Skipping rule in {}: no valid effects parsed", path.string());
                continue;
            }

            try {
                _rules.push_back(std::move(r));
            } catch (const std::exception& e) {
                logger::error("Failed to add rule from {}: {}", path.string(), e.what());
                continue;
            }
        }
    }


//███████╗██╗██╗░░░░░████████╗███████╗██████╗░  ███╗░░░███╗░█████╗░████████╗░█████╗░██╗░░██╗
//██╔════╝██║██║░░░░░╚══██╔══╝██╔════╝██╔══██╗  ████╗░████║██╔══██╗╚══██╔══╝██╔══██╗██║░░██║
//█████╗░░██║██║░░░░░░░░██║░░░█████╗░░██████╔╝  ██╔████╔██║███████║░░░██║░░░██║░░╚═╝███████║
//██╔══╝░░██║██║░░░░░░░░██║░░░██╔══╝░░██╔══██╗  ██║╚██╔╝██║██╔══██║░░░██║░░░██║░░██╗██╔══██║
//██║░░░░░██║███████╗░░░██║░░░███████╗██║░░██║  ██║░╚═╝░██║██║░░██║░░░██║░░░╚█████╔╝██║░░██║
//╚═╝░░░░░╚═╝╚══════╝░░░╚═╝░░░╚══════╝╚═╝░░╚═╝  ╚═╝░░░░░╚═╝╚═╝░░╚═╝░░░╚═╝░░░░╚════╝░╚═╝░░╚═╝

    bool RuleManager::MatchFilter(const Filter& f, const RuleContext& ctx, Rule& currentRule) const {
        if (!CheckTimeFilters(f)) return false;
		if (!ctx.target || ctx.target->IsDeleted() || !ctx.target->GetBaseObject()) return false;
		auto* baseObj = ctx.target->GetBaseObject();

        if (!f.nearby.empty()) {
			if (!ctx.target || ctx.target->IsDeleted() || !ctx.source) return false;
			std::unordered_map<RE::FormID, float> nearbyMap;
			float maxRadius = 0.0f;
    
			std::random_device rd;
			std::mt19937 rng(rd());
    
			for (const auto& nearbyEntry : f.nearby) {
				if (!nearbyEntry.form) continue;
        
				float actualRadius = nearbyEntry.radius.value;
				if (nearbyEntry.radius.useRandom) {
					actualRadius = std::uniform_real_distribution<float>(nearbyEntry.radius.min, nearbyEntry.radius.max)(rng);
				}
        
				nearbyMap[nearbyEntry.form->GetFormID()] = actualRadius;
				maxRadius = (std::max)(maxRadius, actualRadius);
			}
    
			if (nearbyMap.empty()) return false;
			auto* tes = RE::TES::GetSingleton();
			if (!tes) return false;
    
			bool proximityConditionMet = false;
			if (!ctx.target || ctx.target->IsDeleted() || !ctx.source) return false;
			tes->ForEachReferenceInRange(ctx.target, maxRadius, [&](RE::TESObjectREFR* ref) -> RE::BSContainer::ForEachResult {
				if (!ref || ref->IsDeleted()) return RE::BSContainer::ForEachResult::kContinue;
        
				auto* baseObj = ref->GetBaseObject();
				if (!baseObj) return RE::BSContainer::ForEachResult::kContinue;
                        
				auto it = nearbyMap.find(baseObj->GetFormID());
				if (it != nearbyMap.end()) {
					if (!ctx.target || ctx.target->IsDeleted()) return RE::BSContainer::ForEachResult::kContinue;
					auto targetPos = ctx.target->GetPosition();
					float distance = targetPos.GetDistance(ref->GetPosition());
					if (distance <= it->second) {
						proximityConditionMet = true;
						return RE::BSContainer::ForEachResult::kStop;
					}
				}
        
				return RE::BSContainer::ForEachResult::kContinue;
			});
        
			if (!proximityConditionMet) return false;
		}

		if (!f.nearbyNot.empty()) {
			if (!ctx.target || ctx.target->IsDeleted() || !ctx.source) return false;
			std::unordered_map<RE::FormID, float> nearbyNotMap;
			float maxRadius = 0.0f;
    
			std::random_device rd;
			std::mt19937 rng(rd());
    
			for (const auto& nearbyEntry : f.nearbyNot) {
				if (!nearbyEntry.form) continue;
        
				float actualRadius = nearbyEntry.radius.value;
				if (nearbyEntry.radius.useRandom) {
					actualRadius = std::uniform_real_distribution<float>(nearbyEntry.radius.min, nearbyEntry.radius.max)(rng);
				}
        
				nearbyNotMap[nearbyEntry.form->GetFormID()] = actualRadius;
				maxRadius = (std::max)(maxRadius, actualRadius);
			}
    
			if (!nearbyNotMap.empty()) {
				auto* tes = RE::TES::GetSingleton();
				if (!tes) return false;
				bool foundExcludedForm = false;
				if (!ctx.target || ctx.target->IsDeleted() || !ctx.source) return false;
				tes->ForEachReferenceInRange(ctx.target, maxRadius, [&](RE::TESObjectREFR* ref) -> RE::BSContainer::ForEachResult {
					if (!ref || ref->IsDeleted()) return RE::BSContainer::ForEachResult::kContinue;
            
					auto* baseObj = ref->GetBaseObject();
					if (!baseObj) return RE::BSContainer::ForEachResult::kContinue;
                                
					auto it = nearbyNotMap.find(baseObj->GetFormID());
					if (it != nearbyNotMap.end()) {
						if (!ctx.target || ctx.target->IsDeleted()) return RE::BSContainer::ForEachResult::kContinue;
						auto targetPos = ctx.target->GetPosition();
						float distance = targetPos.GetDistance(ref->GetPosition());
						if (distance <= it->second) {
							foundExcludedForm = true;
							return RE::BSContainer::ForEachResult::kStop;
						}
					}
            
					return RE::BSContainer::ForEachResult::kContinue;
				});
            
				if (foundExcludedForm) return false;
			}
		}

        bool objectIdentifierMatch = false;
        bool hasObjectIdentifiers = false;

        if (!f.formTypes.empty()) {
            hasObjectIdentifiers = true;
            if (f.formTypes.contains(baseObj->GetFormType())) {
                objectIdentifierMatch = true;
            }
        }
        if (!f.formTypesNot.empty() && f.formTypesNot.contains(baseObj->GetFormType())) return false;
        if (!f.formIDs.empty()) {
            hasObjectIdentifiers = true;
			if (f.formIDs.contains(baseObj->GetFormID())) {
                objectIdentifierMatch = true;
            }
        }
        if (!f.formIDsNot.empty() && f.formIDsNot.contains(baseObj->GetFormID())) return false;
        if (!f.formLists.empty()) {
            hasObjectIdentifiers = true;
            bool matched = false;
            for (const auto& entry : f.formLists) {
                auto* list = RE::TESForm::LookupByID<RE::BGSListForm>(entry.formID);
                if (!list) continue;

                if (entry.index == -2) {
                    // Find the object index in a formlist
                    int foundIdx = -1;
                    for (int i = 0; i < static_cast<int>(list->forms.size()); ++i) {
                        if (list->forms[i] && list->forms[i]->GetFormID() == baseObj->GetFormID()) {
                            foundIdx = i;
                            break;
                        }
                    }
                    if (foundIdx != -1) {
                        // Save the current rule in dynamicIndex
                        currentRule.dynamicIndex = foundIdx;
                        matched = true;
                    }
                } else if (entry.index >= 0) {
                    // Check only one element in the list (according to the index)
                    if (entry.index < static_cast<int>(list->forms.size())) {
                        auto* el = list->forms[entry.index];
                        if (el && el->GetFormID() == baseObj->GetFormID()) {
                            matched = true;
                            break;
                        }
                    }
                } else {
                    // Check all the elements of the list
                    for (auto* el : list->forms) {
                        if (el && el->GetFormID() == baseObj->GetFormID()) {
                            matched = true;
                            break;
                        }
                    }
                }

                if (matched) {
                    objectIdentifierMatch = true;
                    break;
                }
            }
        }
        if (!f.formListsNot.empty()) {
            for (const auto& entry : f.formListsNot) {
                auto* list = RE::TESForm::LookupByID<RE::BGSListForm>(entry.formID);
                if (!list) continue;
                // Check all the elements of the list
                for (auto* el : list->forms) {
                    if (el && el->GetFormID() == baseObj->GetFormID()) return false;
                }
            }
        }
        if (!f.keywords.empty()) {
            hasObjectIdentifiers = true;
            auto* kwf = baseObj->As<RE::BGSKeywordForm>();
            if (kwf) {
                for (auto* kw : f.keywords) {
                    if (kw && kwf->HasKeyword(kw)) {
                        objectIdentifierMatch = true;
                        break;
                    }
                }
            }
        }
        if (!f.keywordsNot.empty()) {
            auto* kwf = baseObj->As<RE::BGSKeywordForm>();
            if (kwf) {
                for (auto* kw : f.keywordsNot) {
                    if (kw && kwf->HasKeyword(kw)) return false;
                }
            }
        }
        if (f.questItemStatus != 3) {
            auto currentStatus = GetQuestItemStatus(ctx.target);
            
            if (f.questItemStatus != currentStatus) {
                return false;
            }
        }
		if (f.lockLevel != -2) {
			bool matched = false;
			if (auto lockData = ctx.target->extraList.GetByType<RE::ExtraLock>()) {
				if (auto* lock = lockData->lock) {
					RE::LOCK_LEVEL lvl = lock->GetLockLevel(ctx.target);
					if (lvl == GetLockLevel(f.lockLevel)) {
						matched = true;
					}
				}
			} else if (f.lockLevel == -1) {
				matched = true;	 // No lock data means it's not locked
			}
			if (!matched) return false;
		}
		if (f.lockLevelNot != -2) {
			bool matched = false;
			if (auto lockData = ctx.target->extraList.GetByType<RE::ExtraLock>()) {
				if (auto* lock = lockData->lock) {
					RE::LOCK_LEVEL lvl = lock->GetLockLevel(ctx.target);
					if (lvl == GetLockLevel(f.lockLevelNot)) {
						matched = true;
					}
				}
			} else if (f.lockLevelNot == -1) {
				matched = true;
			}
			if (matched) return false;
		}
		if (f.isStacked != 2) {
			if (f.isStacked == 0 && ctx.target->extraList.GetCount() > 1) return false;
			if (f.isStacked == 1 && ctx.target->extraList.GetCount() <= 1) return false;
		}
		if (f.isInterior != 2) {
			auto* cell = ctx.target->GetParentCell();
			if (!cell) return false;
			bool interior = cell->IsInteriorCell();
			if (f.isInterior == 0 && interior) return false;
			if (f.isInterior == 1 && !interior) return false;
		}
		if (f.position != 3) {
			auto player = RE::PlayerCharacter::GetSingleton();
			if (!player) return false;

			NiPoint3 targetCenter = { 0.0f, 0.0f, 0.0f };
			NiPoint3 playerCenter = { 0.0f, 0.0f, 0.0f };

			if (auto* root = ctx.target->Get3D()) {
				targetCenter = root->worldBound.center;
			} else {
				const auto& bmin = ctx.target->GetBoundMin();
				const auto& bmax = ctx.target->GetBoundMax();
				targetCenter = {
					(bmin.x + bmax.x) * 0.5f,
					(bmin.y + bmax.y) * 0.5f,
					(bmin.z + bmax.z) * 0.5f
				};
			}

			if (auto* root = player->Get3D()) {
				playerCenter = root->worldBound.center;
			} else {
				const auto& pmin = player->GetBoundMin();
				const auto& pmax = player->GetBoundMax();
				playerCenter = {
					(pmin.x + pmax.x) * 0.5f,
					(pmin.y + pmax.y) * 0.5f,
					(pmin.z + pmax.z) * 0.5f
				};
			}

			uint32_t actualPosition;
			if (targetCenter.z < playerCenter.z - 30.0f) {
				actualPosition = 0;	 // below player's middle
			} else if (targetCenter.z > playerCenter.z + 30.0f) {
				actualPosition = 2;	 // above player's middle
			} else {
				actualPosition = 1;	 // player's middle
			}

			if (f.position != actualPosition) {
				return false;
			}
		}
		if (f.isFirstPerson != 2) {
			auto* cam = RE::PlayerCamera::GetSingleton();
			if (!cam || !cam->currentState || !cam->currentState->camera) return false;
			if (cam->GetRuntimeData().cameraStates[RE::CameraStates::kFirstPerson] ||
				cam->GetRuntimeData().cameraStates[RE::CameraStates::kVR]) {	// (Hopefully)
				if (f.isFirstPerson == 0) return false;	// Not first person
			} else {
				if (f.isFirstPerson == 1) return false;	// First person
			}
		}
		if (f.isThirdPerson != 2) {
			auto* cam = RE::PlayerCamera::GetSingleton();
			if (!cam || !cam->currentState || !cam->currentState->camera) return false;
			if (cam->GetRuntimeData().cameraStates[RE::CameraStates::kThirdPerson] || 
				cam->GetRuntimeData().cameraStates[RE::CameraStates::kVRThirdPerson]) {
				if (f.isThirdPerson == 0) return false;	// Not third person
			} else {
				if (f.isThirdPerson == 1) return false;	// Third person
			}
		}
        if (!f.perks.empty()) {
            if (!ctx.source) return false;
            auto* actor = ctx.source->As<RE::Actor>();
            if (!actor) return false;
            bool hasAny = false;
            for (const auto& perkID : f.perks) {
                auto* perk = RE::TESForm::LookupByID<RE::BGSPerk>(perkID);
                if (perk && actor->HasPerk(perk)) {
                    hasAny = true;
                    break;
                }
            }
            if (!hasAny) return false;
        }
        if (!f.perksNot.empty()) {
            if (!ctx.source) return false;
            auto* actor = ctx.source->As<RE::Actor>();
            if (!actor) return false;
            for (const auto& perkID : f.perksNot) {
                auto* perk = RE::TESForm::LookupByID<RE::BGSPerk>(perkID);
                if (perk && actor->HasPerk(perk)) return false;
            }
        }
        if (!f.spells.empty()) {
            if (!ctx.source) return false;
            auto* actor = ctx.source->As<RE::Actor>();
            if (!actor) return false;
            bool hasAny = false;
            for (const auto& spellID : f.spells) {
                auto* spell = RE::TESForm::LookupByID<RE::SpellItem>(spellID);
                if (spell && actor->HasSpell(spell)) {
                    hasAny = true;
                    break;
                }
            }
            if (!hasAny) return false;
        }
        if (!f.spellsNot.empty()) {
            if (!ctx.source) return false;
            auto* actor = ctx.source->As<RE::Actor>();
            if (!actor) return false;
            for (const auto& spellID : f.spellsNot) {
                auto* spell = RE::TESForm::LookupByID<RE::SpellItem>(spellID);
                if (spell && actor->HasSpell(spell)) return false;
            }
        }
        if (!f.hasItem.empty()) {
            if (!ctx.source) return false;
            bool hasAnyItem = false;
            for (auto formID : f.hasItem) {
                if (HasItem(ctx.source, formID)) {
                    hasAnyItem = true;
                    break;
                }
            }
            if (!hasAnyItem) return false;
        }
        if (!f.hasItemNot.empty()) {
            if (!ctx.source) return false;
            for (auto formID : f.hasItemNot) {
                if (HasItem(ctx.source, formID)) {
                    return false;
                }
            }
        }
        if (!f.level.empty()) {
            if (!ctx.source) return false;
            int currentLevel = ctx.source->GetLevel();
            
            bool anyLevelMatch = false;
            for (const auto& condition : f.level) {
                if (CheckLevelCondition(condition, currentLevel)) {
                    anyLevelMatch = true;
                    break;
                }
            }
            if (!anyLevelMatch) return false;
        }
        if (!f.levelNot.empty()) {
            if (!ctx.source) return false;
            int currentLevel = ctx.source->GetLevel();
            
            for (const auto& condition : f.levelNot) {
                if (CheckLevelCondition(condition, currentLevel)) {
                    return false;
                }
            }
        }
        if (!f.actorValue.empty()) {
            if (!ctx.source) return false;
            
            bool anyAVMatch = false;
            for (const auto& condition : f.actorValue) {
                if (CheckActorValueCondition(condition, ctx.source)) {
                    anyAVMatch = true;
                    break;
                }
            }
            if (!anyAVMatch) return false;
        }
        if (!f.actorValueNot.empty()) {
            if (!ctx.source) return false;
            
            for (const auto& condition : f.actorValueNot) {
                if (CheckActorValueCondition(condition, ctx.source)) {
                    return false;
                }
            }
        }
        if (!f.actorKeywords.empty()) {
            if (!ctx.source) return false;
            auto* kwf = ctx.source->As<RE::BGSKeywordForm>();
            if (!kwf) return false;
            bool hasAny = false;
            for (auto* kw : f.actorKeywords) {
                if (kw && kwf->HasKeyword(kw)) {
                    hasAny = true;
                    break;
                }
            }
            if (!hasAny) return false;
        }
        if (!f.actorKeywordsNot.empty()) {
            if (!ctx.source) return false;
            auto* kwf = ctx.source->As<RE::BGSKeywordForm>();
            if (kwf) {
                for (auto* kw : f.actorKeywordsNot) {
                    if (kw && kwf->HasKeyword(kw)) return false;
                }
            }
        }
        if (!f.actorRaces.empty()) {
            if (!ctx.source) return false;
            auto race = ctx.source->GetRace();
            if (race) {
                bool matched = false;
                for (auto* allowedRace : f.actorRaces) {
                    if (allowedRace && allowedRace == race) {
                        matched = true;
                        break;
                    }
                }
                if (!matched) return false;
            } else {
                return false;
            }
        }
        if (!f.actorRacesNot.empty()) {
            if (!ctx.source) return false;
            auto race = ctx.source->GetRace();
            if (race) {
                for (auto* notAllowedRace : f.actorRacesNot) {
                    if (notAllowedRace && notAllowedRace == race) {
                        return false;
                    }
                }
            }
        }
        if (f.isSneaking != 2) {
            if (!ctx.source) return false;
            bool isSneaking = ctx.source->IsSneaking();
            if (f.isSneaking == 0 && isSneaking) return false;
            if (f.isSneaking == 1 && !isSneaking) return false;
        }
        if (f.isSwimming != 2) {
            if (!ctx.source) return false;
            bool isSwimming = ctx.source->AsActorState()->IsSwimming();
            if (f.isSwimming == 0 && isSwimming) return false;
            if (f.isSwimming == 1 && !isSwimming) return false;
        }
        if (f.isInCombat != 2) {
            if (!ctx.source) return false;
            bool isInCombat = ctx.source->IsInCombat();
            if (f.isInCombat == 0 && isInCombat) return false;
            if (f.isInCombat == 1 && !isInCombat) return false;
        }
        if (f.isMounted != 2) {
            if (!ctx.source) return false;
            bool isMounted = ctx.source->IsOnMount();
            if (f.isMounted == 0 && isMounted) return false;
            if (f.isMounted == 1 && !isMounted) return false;
        }
        if (f.isSprinting != 2) {
            if (!ctx.source) return false;
            bool isSprinting = ctx.source->AsActorState()->IsSprinting();
            if (f.isSprinting == 0 && isSprinting) return false;
            if (f.isSprinting == 1 && !isSprinting) return false;
        }
        if (f.isWeaponDrawn != 2) {
            if (!ctx.source) return false;
            bool isWeaponDrawn = ctx.source->AsActorState()->IsWeaponDrawn();
            if (f.isWeaponDrawn == 0 && isWeaponDrawn) return false;
            if (f.isWeaponDrawn == 1 && !isWeaponDrawn) return false;
        }
        if (f.isInitiallyDisabled != 2) {
            if (!ctx.target) return false;
            bool isInitiallyDisabled = ctx.target->IsInitiallyDisabled();
            if (f.isInitiallyDisabled == 0 && isInitiallyDisabled) return false;
            if (f.isInitiallyDisabled == 1 && !isInitiallyDisabled) return false;
        }
        if (ctx.isHitEvent) {
            if (f.destructionStage != -1) {
                if (f.destructionStage != ctx.destructionStage) return false;
            }
            if (f.allowProjectiles != 1) {
                if (ctx.projectileSource) return false;
            }
            if (!f.weaponsTypes.empty() && f.weaponsTypes.find(ctx.weaponType) == f.weaponsTypes.end()) return false;
            if (!f.weaponsTypesNot.empty() && f.weaponsTypesNot.find(ctx.weaponType) != f.weaponsTypesNot.end()) return false;
            if (!f.weapons.empty()) {
                if (!ctx.attackSource) return false;
                bool matched = false;
                for (const auto& entry : f.weapons) {
                    auto* weapon = entry;
                    if (!weapon) continue;
                    if (weapon->GetFormID() == ctx.attackSource->GetFormID()) {
                        matched = true;
                        break;
                    }
                }
                if (!matched) return false;
            }
            if (!f.weaponsNot.empty()) {
                if (!ctx.attackSource) return false;
                for (const auto& entry : f.weaponsNot) {
                    auto* weapon = entry;
                    if (!weapon) continue;
                    if (weapon->GetFormID() == ctx.attackSource->GetFormID()) return false;
                }
            }
            if (!f.weaponsKeywords.empty()) {
                if (!ctx.attackSource) return false;
                auto* kwf = ctx.attackSource->As<RE::BGSKeywordForm>();
                if (!kwf) return false;
                bool hasAny = false;
                for (auto* kw : f.weaponsKeywords) {
                    if (kw && kwf->HasKeyword(kw)) {
                        hasAny = true;
                        break;
                    }
                }
                if (!hasAny) return false;
            }
            if (!f.weaponsKeywordsNot.empty()) {
                if (!ctx.attackSource) return false;
                auto* kwf = ctx.attackSource->As<RE::BGSKeywordForm>();
                if (kwf) {
                    for (auto* kw : f.weaponsKeywordsNot) {
                        if (kw && kwf->HasKeyword(kw)) return false;
                    }
                }
            }
            if (!f.projectiles.empty()) {
                if (!ctx.projectileSource) return false;
                bool matched = false;
                for (const auto& entry : f.projectiles) {
                    auto* projectile = entry;
                    if (!projectile) continue;
                    if (projectile->GetFormID() == ctx.projectileSource->GetFormID()) {
                        matched = true;
                        break;
                    }
                }
                if (!matched) return false;
            }
            if (!f.projectilesNot.empty()) {
                if (!ctx.projectileSource) return false;
                for (const auto& entry : f.projectilesNot) {
                    auto* projectile = entry;
                    if (!projectile) continue;
                    if (projectile->GetFormID() == ctx.projectileSource->GetFormID()) return false;
                }
            }
            if (!f.attackTypes.empty() && f.attackTypes.find(ctx.attackType) == f.attackTypes.end()) return false;
            if (!f.attackTypesNot.empty() && f.attackTypesNot.find(ctx.attackType) != f.attackTypesNot.end()) return false;
            if (!f.deliveryTypes.empty() && f.deliveryTypes.find(ctx.deliveryType) == f.deliveryTypes.end()) return false;
            if (!f.deliveryTypesNot.empty() && f.deliveryTypesNot.find(ctx.deliveryType) != f.deliveryTypesNot.end()) return false;
            if (f.isDualCasting != 2) {
                if (!ctx.source) return false;
                bool isDualCasting = ctx.source->IsDualCasting();
                if (f.isDualCasting == 0 && isDualCasting) return false;
                if (f.isDualCasting == 1 && !isDualCasting) return false;
            }
        }

        if (hasObjectIdentifiers && !objectIdentifierMatch) {
            return false;
        }

        return true;
    }


//███████╗███████╗███████╗███████╗░█████╗░████████╗░██████╗  ░█████╗░██████╗░██████╗░██╗░░░░░██╗░░░██╗
//██╔════╝██╔════╝██╔════╝██╔════╝██╔══██╗╚══██╔══╝██╔════╝  ██╔══██╗██╔══██╗██╔══██╗██║░░░░░╚██╗░██╔╝
//█████╗░░█████╗░░█████╗░░█████╗░░██║░░╚═╝░░░██║░░░╚█████╗░  ███████║██████╔╝██████╔╝██║░░░░░░╚████╔╝░
//██╔══╝░░██╔══╝░░██╔══╝░░██╔══╝░░██║░░██╗░░░██║░░░░╚═══██╗  ██╔══██║██╔═══╝░██╔═══╝░██║░░░░░░░╚██╔╝░░
//███████╗██║░░░░░██║░░░░░███████╗╚█████╔╝░░░██║░░░██████╔╝  ██║░░██║██║░░░░░██║░░░░░███████╗░░░██║░░░
//╚══════╝╚═╝░░░░░╚═╝░░░░░╚══════╝░╚════╝░░░░╚═╝░░░╚═════╝░  ╚═╝░░╚═╝╚═╝░░░░░╚═╝░░░░░╚══════╝░░░╚═╝░░░

    void RuleManager::ApplyEffect(const Effect& eff, const RuleContext& ctx, Rule& currentRule) const {     
        if (!ctx.target || !ctx.target->GetBaseObject()) return;
        if (!ctx.source || !ctx.source->GetBaseObject()) return;

        SKSE::GetTaskInterface()->AddTask([this, eff, ctx, currentRule]() {
            auto* target = ctx.target;
            auto* source = ctx.source;

            if (!target || target->IsDeleted()) {
                logger::warn("Target is invalid or deleted");
                return;
            }

            if (!source || source->IsDeleted()) {
                logger::warn("Source is invalid, dead or deleted");
                return;
            }

            auto* targetBase = target->GetBaseObject();
            auto* sourceBase = source->GetBaseObject();

            if (!targetBase) {
                logger::warn("Target base object is invalid or deleted");
                return;
            }

            if (!sourceBase) {
                logger::warn("Source base object is invalid or deleted");
                return;
            }

            auto targetFormID = targetBase->GetFormID();
            auto sourceFormID = sourceBase->GetFormID();
        
            if (!targetFormID) {
                logger::warn("Target formID is invalid");
                return;
            }

            if (!sourceFormID) {
                logger::warn("Source formID is invalid");
                return;
            }
            
            try {
                switch (eff.type) {
                    case EffectType::kRemoveItem: Effects::RemoveItem(ctx); break;
                    case EffectType::kDisableItem: Effects::DisableItem(ctx); break;
                    case EffectType::kEnableItem: Effects::EnableItem(ctx); break;
                    case EffectType::kSpillInventory: Effects::SpillInventory(ctx); break;
                    case EffectType::kUnlockItem: Effects::UnlockItem(ctx); break;
                    case EffectType::kLockItem: Effects::LockItem(ctx); break;
                    case EffectType::kActivateItem: Effects::ActivateItem(ctx); break;
                            
                    case EffectType::kSpawnItem: 
                    {
                        ProcessEffect<RE::TESBoundObject, ItemSpawnData>(
                            eff, ctx, currentRule, true,
                            [](auto* item, const EffectExtendedData& ext) {
                                return ItemSpawnData(item, ext.string, ext.count, ext.spawnType, ext.fade, ext.scale);
                            },
                            Effects::SpawnItem
                        );
                    }
                    break;

                    case EffectType::kSwapItem:
                    {
                        ProcessEffect<RE::TESBoundObject, ItemSpawnData>(
                            eff, ctx, currentRule, true,
                            [](auto* item, const EffectExtendedData& ext) {
                                return ItemSpawnData(item, ext.string, ext.count, ext.spawnType, ext.fade, ext.scale, ext.nonDeletable);
                            },
                            Effects::SwapItem
                        );
                    }
                    break;

                    case EffectType::kSpawnLeveledItem:
                    {
                        ProcessEffect<RE::TESLevItem, LvlItemSpawnData>(
                            eff, ctx, currentRule, true,
                            [](auto* lvli, const EffectExtendedData& ext) {
                                return LvlItemSpawnData(lvli, ext.string, ext.count, ext.spawnType, ext.fade, ext.scale);
                            },
                            Effects::SpawnLeveledItem
                        );
                    }
                    break;

                    case EffectType::kSwapLeveledItem:
                    {
                        ProcessEffect<RE::TESLevItem, LvlItemSpawnData>(
                            eff, ctx, currentRule, true,
                            [](auto* lvli, const EffectExtendedData& ext) {
                                return LvlItemSpawnData(lvli, ext.string, ext.count, ext.spawnType, ext.fade, ext.scale, ext.nonDeletable);
                            },
                            Effects::SwapLeveledItem
                        );
                    }
                    break;
                        
                    case EffectType::kSpawnSpell:
                    {
                        ProcessEffect<RE::SpellItem, SpellSpawnData>(
                            eff, ctx, currentRule, true,
                            [](auto* spell, const EffectExtendedData& ext) {
                                return SpellSpawnData(spell, ext.count, ext.radius);
                            },
                            Effects::SpawnSpell
                        );
                    }
                    break;
                        
                    case EffectType::kSpawnSpellOnItem:
                    {
                        ProcessEffect<RE::SpellItem, SpellSpawnData>(
                            eff, ctx, currentRule, true,
                            [](auto* spell, const EffectExtendedData& ext) {
                                return SpellSpawnData(spell, ext.count);
                            },
                            Effects::SpawnSpellOnItem
                        );
                        break;
                    }

                    case EffectType::kSpawnLeveledSpell:
                    {
                        ProcessEffect<RE::TESLevSpell, LvlSpellSpawnData>(
                            eff, ctx, currentRule, true,
                            [](auto* lvls, const EffectExtendedData& ext) {
                                return LvlSpellSpawnData(lvls, ext.count, ext.radius);
                            },
                            Effects::SpawnLeveledSpell
                        );
                    }
                    break;

                    case EffectType::kSpawnLeveledSpellOnItem:
                    {
                        ProcessEffect<RE::TESLevSpell, LvlSpellSpawnData>(
                            eff, ctx, currentRule, true,
                            [](auto* lvls, const EffectExtendedData& ext) {
                                return LvlSpellSpawnData(lvls, ext.count);
                            },
                            Effects::SpawnLeveledSpellOnItem
                        );
                    }
                    break;

                	case EffectType::kApplySpell:
					{
						ProcessEffect<void, SpellSpawnData>(
							eff, ctx, currentRule, false,
							[](std::nullptr_t, const EffectExtendedData& ext) {
								return SpellSpawnData(nullptr, ext.count, ext.radius);
							},
							Effects::ApplySpell
						);
					}
					break;
                        
                    case EffectType::kSpawnActor:
                    {
                        ProcessEffect<RE::TESNPC, ActorSpawnData>(
                            eff, ctx, currentRule, true,
                            [](auto* actor, const EffectExtendedData& ext) {
                                return ActorSpawnData(actor, ext.string, ext.count, ext.spawnType, ext.fade, ext.scale);
                            },
                            Effects::SpawnActor
                        );
                    }
                    break;

                    case EffectType::kSwapActor:
                    {
                        ProcessEffect<RE::TESNPC, ActorSpawnData>(
                            eff, ctx, currentRule, true,
                            [](auto* actor, const EffectExtendedData& ext) {
                                return ActorSpawnData(actor, ext.string, ext.count, ext.spawnType, ext.fade, ext.scale, ext.nonDeletable);
                            },
                            Effects::SwapActor
                        );
                    }
                    break;

                    case EffectType::kSpawnLeveledActor:
                    {
                        ProcessEffect<RE::TESLevCharacter, LvlActorSpawnData>(
                            eff, ctx, currentRule, true,
                            [](auto* lvlc, const EffectExtendedData& ext) {
                                return LvlActorSpawnData(lvlc, ext.string, ext.count, ext.spawnType, ext.fade, ext.scale);
                            },
                            Effects::SpawnLeveledActor
                        );
                    }
                    break;

                    case EffectType::kSwapLeveledActor:
                    {
                        ProcessEffect<RE::TESLevCharacter, LvlActorSpawnData>(
                            eff, ctx, currentRule, true,
                            [](auto* lvlc, const EffectExtendedData& ext) {
                                return LvlActorSpawnData(lvlc, ext.string, ext.count, ext.spawnType, ext.fade, ext.scale, ext.nonDeletable);
                            },
                            Effects::SwapLeveledActor
                        );
                    }
                    break;

                    case EffectType::kSpawnImpactDataSet:
                    {
                        ProcessEffect<RE::BGSImpactDataSet, ImpactDataSetSpawnData>(
                            eff, ctx, currentRule, true,
                            [](auto* impact, const EffectExtendedData& ext) {
                                return ImpactDataSetSpawnData(impact, ext.count);
                            },
                            Effects::SpawnImpactDataSet
                        );
                    }
                    break;
                        
                    case EffectType::kSpawnExplosion:
                    {
                        ProcessEffect<RE::BGSExplosion, ExplosionSpawnData>(
                            eff, ctx, currentRule, true,
                            [](auto* explosion, const EffectExtendedData& ext) {
                                return ExplosionSpawnData(explosion, ext.string, ext.count, ext.spawnType, ext.fade);
                            },
                            Effects::SpawnExplosion
                        );
                    }
                    break;
                        
                    case EffectType::kPlaySound:
                    {
                        ProcessEffect<RE::BGSSoundDescriptorForm, SoundSpawnData>(
                            eff, ctx, currentRule, true,
                            [](auto* sound, const EffectExtendedData& ext) {
                                return SoundSpawnData(sound, ext.count);
                            },
                            Effects::PlaySound
                        );
                    }
                    break;

                    case EffectType::kApplyIngestible:
                    {
                        ProcessEffect<void, IngestibleApplyData>(
                            eff, ctx, currentRule, false,
                            [](std::nullptr_t, const EffectExtendedData& ext) {
                                return IngestibleApplyData(nullptr, ext.count, ext.radius);
                            },
                            Effects::ApplyIngestible
                        );
                    }
                    break;

                    case EffectType::kApplyOtherIngestible:
                    {
                        ProcessEffect<RE::MagicItem, IngestibleApplyData>(
                            eff, ctx, currentRule, true,
                            [](auto* ingestible, const EffectExtendedData& ext) {
                                return IngestibleApplyData(ingestible, ext.count, ext.radius);
                            },
                            Effects::ApplyOtherIngestible
                        );
                    }
                    break;

                    case EffectType::kSpawnLight:
                    {
                        ProcessEffect<RE::TESObjectLIGH, LightSpawnData>(
                            eff, ctx, currentRule, true,
                            [](auto* light, const EffectExtendedData& ext) {
                                return LightSpawnData(light, ext.string, ext.count, ext.spawnType, ext.fade, ext.scale);
                            },
                            Effects::SpawnLight
                        );
                    }
                    break;

                    case EffectType::kRemoveLight:
                    {
                        ProcessEffect<void, LightRemoveData>(
                            eff, ctx, currentRule, false,
							[](std::nullptr_t, const EffectExtendedData& ext) {
                                return LightRemoveData(ext.radius);
                            },
                            Effects::RemoveLight
                        );
                    }
                    break;

                    case EffectType::kEnableLight:
                    {
                        ProcessEffect<void, LightRemoveData>(
                            eff, ctx, currentRule, false,
							[](std::nullptr_t, const EffectExtendedData& ext) {
                                return LightRemoveData(ext.radius);
                            },
                            Effects::EnableLight
                        );
                    }
                    break;

                    case EffectType::kDisableLight:
                    {
                        ProcessEffect<void, LightRemoveData>(
                            eff, ctx, currentRule, false,
							[](std::nullptr_t, const EffectExtendedData& ext) {
                                return LightRemoveData(ext.radius);
                            },
                            Effects::DisableLight
                        );
                    }
                    break;

                    case EffectType::kPlayIdle:
                    {
                        ProcessEffect<void, PlayIdleData>(
                            eff, ctx, currentRule, false,
                            [&ctx](std::nullptr_t, const EffectExtendedData& ext) {
                                return PlayIdleData(ctx.source, ext.string, ext.duration > 0.0f ? ext.duration : 1.0f);
                            },
                            Effects::PlayIdle
                        );
                    }
                    break;

                    case EffectType::kSpawnEffectShader:
                    {
                        ProcessEffect<RE::TESEffectShader, EffectShaderSpawnData>(
                            eff, ctx, currentRule, true,
                            [](auto* shader, const EffectExtendedData& ext) {
                                return EffectShaderSpawnData(shader, ext.count, ext.radius, ext.duration);
                            },
                            Effects::SpawnEffectShader
                        );
                    }
                    break;

                    case EffectType::kSpawnEffectShaderOnItem:
                    {
                        ProcessEffect<RE::TESEffectShader, EffectShaderSpawnData>(
                            eff, ctx, currentRule, true,
                            [](auto* shader, const EffectExtendedData& ext) {
                                return EffectShaderSpawnData(shader, ext.count, ext.radius, ext.duration);
                            },
                            Effects::SpawnEffectShaderOnItem
                        );
                    }
                    break;
                        
                    case EffectType::kToggleNode:
                    {
                        ProcessEffect<void, NodeData>(
                            eff, ctx, currentRule, false,
                            [](std::nullptr_t, const EffectExtendedData& ext) {
                                return NodeData(ext.mode, ext.strings);
                            },
                            Effects::ToggleNode
                        );
                    }
                    break;

                    case EffectType::kSpawnArtObject:
                    {
                        ProcessEffect<RE::BGSArtObject, ArtObjectData>(
                            eff, ctx, currentRule, true,
                            [](auto* artObject, const EffectExtendedData& ext) {
                                return ArtObjectData(artObject, ext.count, ext.radius, ext.duration);
                            },
                            Effects::SpawnArtObject
                        );
                    }
					break;

                    case EffectType::kSpawnArtObjectOnItem:
                    {
                        ProcessEffect<RE::BGSArtObject, ArtObjectData>(
                            eff, ctx, currentRule, true,
                            [](auto* artObject, const EffectExtendedData& ext) {
                                return ArtObjectData(artObject, ext.count, ext.radius, ext.duration);
                            },
                            Effects::SpawnArtObjectOnItem
                        );
                    }
					break;

                    /*case EffectType::kToggleShaderFlag:
                    {
                        std::vector<ShaderFlagData> shaderFlagData;
                        for (const auto& [form, extData] : eff.items) {
                            float roll = std::uniform_real_distribution<float>(0.f, 100.f)(rng);
                            if (roll > extData.chance) continue;

                            ShaderFlagData data;
                            data.mode = extData.mode;
                            data.flagNames = extData.flagNames;
                            data.strings = extData.strings;
                            data.chance = extData.chance;
                            shaderFlagData.emplace_back(std::move(data));
                        }
                        if (!shaderFlagData.empty()) {
                            Effects::ToggleShaderFlag(ctx, shaderFlagData);
                        }
                    }
                    {
                        
                    }
                    break;*/

                    case EffectType::kAddContainerItem:
                    case EffectType::kAddActorItem:
                    case EffectType::kRemoveContainerItem:
                    case EffectType::kRemoveActorItem:
                    {
                        ProcessEffect<RE::TESBoundObject, InventoryData>(
                            eff, ctx, currentRule, true,
                            [](auto* item, const EffectExtendedData& ext) {
                                return InventoryData(item, ext.count);
                            },
                            [eff](const RuleContext& ctx, const std::vector<InventoryData>& data) {
                                switch (eff.type) {
                                    case EffectType::kAddContainerItem: Effects::AddContainerItem(ctx, data); break;
                                    case EffectType::kAddActorItem: Effects::AddActorItem(ctx, data); break;
                                    case EffectType::kRemoveContainerItem: Effects::RemoveContainerItem(ctx, data); break;
                                    case EffectType::kRemoveActorItem: Effects::RemoveActorItem(ctx, data); break;
                                    default: break;
                                }
                            }
                        );
                    }
                    break;

                    case EffectType::kAddActorSpell:
                    {
                        ProcessEffect<RE::SpellItem, SpellSpawnData>(
                            eff, ctx, currentRule, true,
                            [](auto* spell, const EffectExtendedData& ext) {
                                return SpellSpawnData(spell, ext.count);
                            },
                            Effects::AddActorSpell
                        );
                    }
                    break;

                    case EffectType::kRemoveActorSpell:
                    {
                        ProcessEffect<RE::SpellItem, SpellSpawnData>(
                            eff, ctx, currentRule, true,
                            [](auto* spell, const EffectExtendedData& /*ext*/) {
                                return SpellSpawnData(spell);
                            },
                            Effects::RemoveActorSpell
                        );
                    }
                    break;

                    case EffectType::kAddActorPerk:
                    {
                        ProcessEffect<RE::BGSPerk, PerkData>(
                            eff, ctx, currentRule, true,
                            [](auto* perk, const EffectExtendedData& ext) {
                                return PerkData(perk, ext.rank);
                            },
                            Effects::AddActorPerk
                        );
                    }
                    break;

                    case EffectType::kRemoveActorPerk:
                    {
                        ProcessEffect<RE::BGSPerk, PerkData>(
                            eff, ctx, currentRule, true,
                            [](auto* perk, const EffectExtendedData& /*ext*/) {
                                return PerkData(perk);
                            },
                            Effects::RemoveActorPerk
                        );
                    }
                    break;

                    case EffectType::kExecuteConsoleCommand:
                    {
                        ProcessEffect<void, StringData>(
                            eff, ctx, currentRule, true,
                            [](std::nullptr_t, const EffectExtendedData& ext) {
                                return StringData(ext.string, ext.radius);
                            },
                            Effects::ExecuteConsoleCommand
                        );
                    }
                    break;

                    case EffectType::kExecuteConsoleCommandOnItem:
                    {
                        ProcessEffect<void, StringData>(
                            eff, ctx, currentRule, false,
                            [](std::nullptr_t, const EffectExtendedData& ext) {
                                return StringData(ext.string);
                            },
                            Effects::ExecuteConsoleCommandOnItem
                        );
                    }
                    break;

					case EffectType::kExecuteConsoleCommandOnSource:
					{
						ProcessEffect<void, StringData>(
							eff, ctx, currentRule, false,
							[](std::nullptr_t, const EffectExtendedData& ext) {
								return StringData(ext.string);
							},
							Effects::ExecuteConsoleCommandOnSource);
					}
					break;

                    case EffectType::kShowNotification:
                    {
                        ProcessEffect<void, StringData>(
                            eff, ctx, currentRule, false,
                            [](std::nullptr_t, const EffectExtendedData& ext) {
                                return StringData(ext.string);
                            },
                            Effects::ShowNotification
                        );
                    }
                    break;
                        
                    case EffectType::kShowMessageBox: 
                    {
                        ProcessEffect<void, StringData>(
                            eff, ctx, currentRule, false,
                            [](std::nullptr_t, const EffectExtendedData& ext) {
                                return StringData(ext.string);
                            },
                            Effects::ShowMessageBox
                        );
                    }
                    break;
                                
                default:
                    logger::warn("Unknown effect type {}", static_cast<int>(eff.type));
                }
            } catch (const std::exception& e) {
                logger::error("Exception in effect task: {}", e.what());
            } catch (...) {
                logger::error("Unknown exception in effect task");
            }
        });
    }
    

//████████╗██████╗░██╗░██████╗░░██████╗░███████╗██████╗░
//╚══██╔══╝██╔══██╗██║██╔════╝░██╔════╝░██╔════╝██╔══██╗
//░░░██║░░░██████╔╝██║██║░░██╗░██║░░██╗░█████╗░░██████╔╝
//░░░██║░░░██╔══██╗██║██║░░╚██╗██║░░╚██╗██╔══╝░░██╔══██╗
//░░░██║░░░██║░░██║██║╚██████╔╝╚██████╔╝███████╗██║░░██║
//░░░╚═╝░░░╚═╝░░╚═╝╚═╝░╚═════╝░░╚═════╝░╚══════╝╚═╝░░╚═╝

// ╔════════════════════════════════════╗
// ║        CLEAN-UP FUNCTUION          ║
// ╚════════════════════════════════════╝

    void RuleManager::CleanupCounters() {
        // Prevent unbounded growth of interaction counter map
        if (_interactionsCounts.size() > 500) {
            logger::debug("Clearing interactions cache: {} entries", _interactionsCounts.size());
            _interactionsCounts.clear();
        }

        if (_limitCounts.size() > 500) {
            std::size_t removedCount = 0;
            auto it = _limitCounts.begin();
            while (it != _limitCounts.end()) {
                if (it->second == 0) {  // Remove only unused limits
                    it = _limitCounts.erase(it);
                    ++removedCount;
                } else {
                    ++it;
                }
            }
            logger::debug("Cleaned {} unused limit entries, remaining: {}", removedCount, _limitCounts.size());
        }

        // Emergency cleanup if counts grow too large
        if (_interactionsCounts.size() > 1000) {
            logger::warn("Emergency interactions cleanup!");
            _interactionsCounts.clear();
        }

        if (_limitCounts.size() > 1000) {
            logger::warn("Emergency limits cleanup - removing only zero values!");
            auto it = _limitCounts.begin();
            while (it != _limitCounts.end()) {
                if (it->second == 0) {
                    it = _limitCounts.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }

// ╔════════════════════════════════════╗
// ║          TRIGGER FUNCTION          ║
// ╚════════════════════════════════════╝

    void RuleManager::Trigger(const RuleContext& ctx)
    {
        std::unique_lock lock(_ruleMutex);

        // Save critical data to prevent potential loss
        auto localTarget = ctx.target;
        auto localSource = ctx.source;
        if (!localTarget || localTarget->IsDeleted() || !localSource || localSource->IsDeleted()) return;

        auto targetFormID = localTarget->GetFormID();
        auto sourceFormID = localSource->GetFormID();

		// ╔════════════════════════════════════╗
		// ║           DEDUPLICATION            ║
		// ╚════════════════════════════════════╝
        // Deduplicate hit calls for the same target in a short time frame
		// The reason is that kHit event is used by multiple sinks and hooks, which can collide

        auto now = std::chrono::steady_clock::now();

        if (ctx.event == EventType::kHit) {

            static bool isFirstCall = true;
            if (isFirstCall) {
                lastCleanupTime = now;
                isFirstCall = false;
            }

            if (now - lastCleanupTime > std::chrono::milliseconds(50)) {
                auto it = recentlyProcessedItems.begin();
                while (it != recentlyProcessedItems.end()) {
                    if (now - it->second > std::chrono::milliseconds(250)) {
                        it = recentlyProcessedItems.erase(it);
                    } else {
                        ++it;
                    }
                }
                lastCleanupTime = now;
            }
                    
            if (recentlyProcessedItems.find(localTarget) != recentlyProcessedItems.end()) {
                return;
            }
            
            recentlyProcessedItems[localTarget] = now;
        }

		// Walk through every compatible rule and apply those whose filters match
		for (std::size_t ruleIdx = 0; ruleIdx < _rules.size(); ++ruleIdx) {
			Rule& r = _rules[ruleIdx];

			if (std::find(r.events.begin(), r.events.end(), ctx.event) == r.events.end()) continue;

			// ╔════════════════════════════════════╗
			// ║         RANDOM ROLLS BLOCK         ║
			// ╚════════════════════════════════════╝

			static thread_local std::mt19937 rng(std::random_device{}());

			// Roll for limit if needed
			if (r.filter.limit.useRandom && !r.filter.limit.hasRolled) {
				r.filter.limit.value = std::uniform_int_distribution<std::uint32_t>(r.filter.limit.min, r.filter.limit.max)(rng);
				r.filter.limit.hasRolled = true;
			}

			// Roll for interactions if needed
			if (r.filter.interactions.useRandom && !r.filter.interactions.hasRolled) {
				r.filter.interactions.value = std::uniform_int_distribution<std::uint32_t>(r.filter.interactions.min, r.filter.interactions.max)(rng);
				r.filter.interactions.hasRolled = true;
			}

			r.dynamicIndex = 0;
			if (!MatchFilter(r.filter, ctx, r)) continue;
			if (!CheckLocationFilter(r.filter, ctx)) return;
			if (!CheckWeatherFilter(r.filter)) return;

			// ╔════════════════════════════════════╗
			// ║         LIMIT CHECK BLOCK          ║
			// ╚════════════════════════════════════╝
			// Important data which will be partially saved

			bool limitCheckPassed = true;
			if (r.filter.limit.value > 0) {
				Key limitKey{
					sourceFormID,
					targetFormID,
					static_cast<std::uint16_t>(ruleIdx)
				};
				std::uint32_t& limitCnt = _limitCounts[limitKey];
				if (limitCnt >= r.filter.limit.value) {
					limitCheckPassed = false;
				} else {
					++limitCnt;
				}
			}
			if (!limitCheckPassed) continue;

			// ╔════════════════════════════════════╗
			// ║      INTERACTIONS CHECK BLOCK      ║
			// ╚════════════════════════════════════╝
			// Temporary data - can be reset

			bool interactionCheckPassed = true;
			if (r.filter.interactions.value > 1) {
				Key interactionKey{
					sourceFormID,
					targetFormID,
					static_cast<std::uint16_t>(ruleIdx)
				};
				std::uint32_t& interactionsCnt = _interactionsCounts[interactionKey];
				if (++interactionsCnt < r.filter.interactions.value) {
					interactionCheckPassed = false;
				} else {
					interactionsCnt = 0;
					// Reset hasRolled when interactions accumulated
					r.filter.interactions.hasRolled = false;
				}
			}
			if (!interactionCheckPassed) continue;

			// ╔════════════════════════════════════╗
			// ║         TIMER CHECK BLOCK          ║
			// ╚════════════════════════════════════╝

			bool timerBlockApplied = false;
			if (r.filter.timer.time.value > 0.0f) {
				if (ctx.event == EventType::kOnUpdate) {
					static std::map<Key, std::chrono::steady_clock::time_point> updateTimers;
					static std::chrono::steady_clock::time_point lastTimerCleanup = std::chrono::steady_clock::now();

					Key ruleKey{
						sourceFormID,
						targetFormID,
						static_cast<std::uint16_t>(ruleIdx)
					};
					auto currentTime = std::chrono::steady_clock::now();

					if (currentTime - lastTimerCleanup > std::chrono::minutes(5)) {
						auto cleanupThreshold = currentTime - std::chrono::hours(1);
						auto it = updateTimers.begin();
						while (it != updateTimers.end()) {
							if (it->second < cleanupThreshold) {
								it = updateTimers.erase(it);
							} else {
								++it;
							}
						}
						lastTimerCleanup = currentTime;

						if (updateTimers.size() > 500) {
							logger::warn("Emergency cleanup of updateTimers: {} entries", updateTimers.size());
							updateTimers.clear();
						}
					}

					auto it = updateTimers.find(ruleKey);
					if (it == updateTimers.end()) {
						updateTimers[ruleKey] = currentTime;
						continue;
					} else {
						auto elapsed = std::chrono::duration<float>(currentTime - it->second).count();
						if (elapsed < r.filter.timer.time.value) {
							continue;
						} else {
							it->second = currentTime;
						}
					}
				} else {
					static std::vector<std::future<void>> timerTasks;
					static std::mutex timerMutex;

					auto timerFuture = std::async(std::launch::async, [this, r, ctx, timerBlockApplied, timer = r.filter.timer.time.value]() {
						std::this_thread::sleep_for(std::chrono::duration<float>(timer));

						SKSE::GetTaskInterface()->AddTask([this, r, ctx, timerBlockApplied]() mutable {
							auto* target = ctx.target;
							auto* source = ctx.source;

							if (!target || target->IsDeleted()) {
								logger::warn("Target is invalid or deleted after timer");
								return;
							}

							if (!source || source->IsDeleted()) {
								logger::warn("Source is invalid or deleted after timer");
								return;
							}

							if (r.filter.timer.matchFilterRecheck == 1) {
								if (!MatchFilter(r.filter, ctx, r)) return;
							}

							float globalRoll = std::uniform_real_distribution<float>(0.f, 100.f)(rng);
							if (globalRoll < r.filter.chance.value) {
								for (auto& eff : r.effects) {
									for (auto& [form, extData] : eff.items) {
										if (extData.count.useRandom) {
											extData.count.value = std::uniform_int_distribution<std::uint32_t>(extData.count.min, extData.count.max)(rng);
										}
										if (extData.scale.useRandom) {
											extData.scale.value = std::uniform_real_distribution<float>(extData.scale.min, extData.scale.max)(rng);
										}
										if (extData.radius.useRandom) {
											extData.radius.value = std::uniform_real_distribution<float>(extData.radius.min, extData.radius.max)(rng);
										}
									}
									ApplyEffect(eff, ctx, r);
								}
								timerBlockApplied = true;
							}
						});
					});

					{
						std::lock_guard<std::mutex> timerlock(timerMutex);
						timerTasks.push_back(std::move(timerFuture));

						timerTasks.erase(
							std::remove_if(timerTasks.begin(), timerTasks.end(),
								[](const std::future<void>& f) {
									return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
								}),
							timerTasks.end());
					}

					continue;
				}
			}

			// ╔════════════════════════════════════╗
			// ║          EFFECTS APPLYING          ║
			// ╚════════════════════════════════════╝

			if (!timerBlockApplied) {
				float globalRoll = std::uniform_real_distribution<float>(0.f, 100.f)(rng);
				if (globalRoll < r.filter.chance.value) {
					for (auto& eff : r.effects) {
						for (auto& [form, extData] : eff.items) {
							if (extData.count.useRandom) {
								extData.count.value = std::uniform_int_distribution<std::uint32_t>(extData.count.min, extData.count.max)(rng);
							}
							if (extData.scale.useRandom) {
								extData.scale.value = std::uniform_real_distribution<float>(extData.scale.min, extData.scale.max)(rng);
							}
							if (extData.radius.useRandom) {
								extData.radius.value = std::uniform_real_distribution<float>(extData.radius.min, extData.radius.max)(rng);
							}
						}
						ApplyEffect(eff, ctx, r);
					}
				}
			}
		}

		// ╔════════════════════════════════════╗
		// ║              CLEAN-UP              ║
		// ╚════════════════════════════════════╝

		CleanupCounters();
	}
}
