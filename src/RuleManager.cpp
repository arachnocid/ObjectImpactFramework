#include "RuleManager.h"
#include "Effects.h"
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace {

    // ------------------ Case‑insensitive helpers ------------------
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
}

namespace OIF {

    // ------------------ Helpers ------------------
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
        {"key", RE::FormType::KeyMaster}
    };

    static const std::unordered_map<std::string_view, std::string> attackTypeMap = {
        {"regular", "Regular"},
        {"power", "Power"},
        {"bash", "Bash"},
        {"projectile", "Projectile"},
        {"charge", "Charge"},
        {"rotating", "Rotating"},
        {"continuous", "Continuous"}
    };
    
    static const std::unordered_map<std::string_view, std::string> weaponTypeMap = {
        {"onehandsword", "OneHandSword"},
        {"twohandsword", "TwoHandSword"},
        {"onehandaxe", "OneHandAxe"},
        {"twohandaxe", "TwoHandAxe"},
        {"onehandmace", "OneHandMace"},
        {"twohandmace", "TwoHandMace"},
        {"dagger", "Dagger"},
        {"ranged", "Ranged"},
        {"staff", "Staff"},
        {"spell", "Spell"},
        {"shout", "Shout"},
        {"ability", "Ability"},
        {"lesserpower", "LesserPower"},
        {"power", "Power"},
        {"handtohand", "HandToHand"},
        {"total", "Total"},
        {"other", "Other"}
    };

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
        if (lowerName == "blockmodifier") return RE::ActorValue::kBlockModifier;
        if (lowerName == "blockpowermodifier") return RE::ActorValue::kBlockPowerModifier;
        if (lowerName == "blockskilladvance") return RE::ActorValue::kBlockSkillAdvance;
        if (lowerName == "bowspeedbonus") return RE::ActorValue::kBowSpeedBonus;
        if (lowerName == "bowstaggerbonus") return RE::ActorValue::kBowStaggerBonus;
        if (lowerName == "braincondition") return RE::ActorValue::kBrainCondition;
        if (lowerName == "bypassvendorkeywordcheck") return RE::ActorValue::kBypassVendorKeywordCheck;
        if (lowerName == "bypassvendorstolencheck") return RE::ActorValue::kBypassVendorStolenCheck;
        if (lowerName == "carryweight") return RE::ActorValue::kCarryWeight;
        if (lowerName == "combathealthregenmultiply") return RE::ActorValue::kCombatHealthRegenMultiply;
        if (lowerName == "confidence") return RE::ActorValue::kConfidence;
        if (lowerName == "conjuration") return RE::ActorValue::kConjuration;
        if (lowerName == "conjurationmodifier") return RE::ActorValue::kConjurationModifier;
        if (lowerName == "conjurationpowermodifier") return RE::ActorValue::kConjurationPowerModifier;
        if (lowerName == "conjurationskilladvance") return RE::ActorValue::kConjurationSkillAdvance;
        if (lowerName == "criticalchance") return RE::ActorValue::kCriticalChance;
        if (lowerName == "damageresist") return RE::ActorValue::kDamageResist;
        if (lowerName == "deprecated05") return RE::ActorValue::kDEPRECATED05;
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

    static RE::FormType MapStringToFormType(std::string_view s) {
        auto it = formTypeMap.find(tolower_str(s));
        return it != formTypeMap.end() ? it->second : RE::FormType::None;
    }

    static std::string MapAttackTypeToString(std::string_view s) {
        auto it = attackTypeMap.find(tolower_str(s));
        return it != attackTypeMap.end() ? it->second : "Regular";
    }

    static std::string MapWeaponTypeToString(std::string_view s) {
        auto it = weaponTypeMap.find(tolower_str(s));
        return it != weaponTypeMap.end() ? it->second : "Other";
    }

    static RE::BGSKeyword* GetKeywordByName(const std::string& keywordName) {
        if (keywordName.empty()) {
            logger::warn("Empty keyword name provided");
            return nullptr;
        }

        static std::recursive_mutex dataHandlerMutex;
        std::lock_guard<std::recursive_mutex> lock(dataHandlerMutex);
        
        auto* dh = RE::TESDataHandler::GetSingleton();
        if (!dh) {
            logger::error("TESDataHandler not available");
            return nullptr;
        }
    
        for (auto& keyword : dh->GetFormArray<RE::BGSKeyword>()) {
            if (keyword && keyword->GetFormEditorID() &&
                _stricmp(keyword->GetFormEditorID(), keywordName.c_str()) == 0) {
                return keyword;
            }
        }
        
        return nullptr;
    }

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
    
        // Remove 0x prefix if present
        if (idStr.size() > 2 && idStr.substr(0, 2) == "0x") {
            idStr = idStr.substr(2);
        }
    
        // Handle FE prefix for ESL/ESPFE plugins (convert FE123456 to 123456)
        if (idStr.size() > 2 && (idStr.substr(0, 2) == "FE" || idStr.substr(0, 2) == "fe")) {
            idStr = idStr.substr(2);
        }
    
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

    bool CheckLevelCondition(const LevelCondition& condition, int currentLevel) {
        return CompareValues(condition.operator_type, currentLevel, condition.value);
    }

    bool CheckActorValueCondition(const ActorValueCondition& condition, RE::Actor* actor) {
        RE::ActorValue av = GetActorValueFromString(condition.actorValue);
        if (av == RE::ActorValue::kNone) return false;
        
        auto* npc = actor->As<RE::TESNPC>();
        if (!npc) return false;
        
        float currentValue = npc->GetActorValue(av);
        
        return CompareValues(condition.operator_type, currentValue, condition.value);
    }

    bool HasItem(RE::Actor* actor, RE::FormID formID) {
        if (!actor) return false;
        auto inventory = actor->GetInventory();
        if (!inventory.empty()) {
            for (const auto& [item, invData] : inventory) {
                if (item && item->GetFormID() == formID && invData.first > 0) {
                    return true;
                }
            }
        }
        return false;
    }
    
    std::uint32_t GetQuestItemStatus(RE::TESObjectREFR* ref) {
        if (!ref) return 0;

        if (ref->HasQuestObject()) {
            return 2;
        }
        
        if (const auto xAliases = ref->extraList.GetByType<RE::ExtraAliasInstanceArray>(); xAliases) { // the line is taken from the PO3 Papyrus Extender
            if (xAliases) RE::BSReadLockGuard locker(xAliases->lock);
            if (!xAliases->aliases.empty()) {
                return 1;
            }
        }
        
        return 0;
    }

    bool CheckLocationFilter(const Filter& f, const RuleContext& ctx, Rule& currentRule) {
        // If there are no filters - skip the check
        if (f.locationIDs.empty() && f.locationIDsNot.empty() && f.locationLists.empty() && f.locationListsNot.empty()) {
            currentRule.filterCache.locationMatch = true;
            return true;
        }

        // Check the cache
        if (currentRule.filterCache.IsValid() && currentRule.filterCache.locationMatch.has_value()) {
            return currentRule.filterCache.locationMatch.value();
        }
    
        auto* target = ctx.target->As<RE::TESObjectREFR>();
        if (!target) {
            currentRule.filterCache.locationMatch = false;
            return false;
        }
    
        auto* currentCell = target->GetParentCell();
        if (!currentCell) {
            currentRule.filterCache.locationMatch = false;
            return false;
        }
    
        RE::FormID currentCellID = currentCell->GetFormID();
        RE::FormID currentWorldspaceID = 0;
        if (auto* worldspace = currentCell->GetRuntimeData().worldSpace) {
            currentWorldspaceID = worldspace->GetFormID();
        }
    
        // Get the current locations of the player
        std::set<RE::FormID> currentLocationIDs;
        std::set<RE::FormID> visitedParents;
        if (auto* player = RE::PlayerCharacter::GetSingleton()) {
            if (auto* currentLocation = player->GetCurrentLocation()) {
                currentLocationIDs.insert(currentLocation->GetFormID());
                auto* parentLoc = currentLocation;
                while (parentLoc && parentLoc->parentLoc) {
                    parentLoc = parentLoc->parentLoc;
                    if (visitedParents.contains(parentLoc->GetFormID())) {
                        logger::warn("Circular reference detected in location hierarchy");
                        break;
                    }
                    visitedParents.insert(parentLoc->GetFormID());
                    currentLocationIDs.insert(parentLoc->GetFormID());
                }
            }
        }
    
        // Checking including filters
        if (!f.locationIDs.empty()) {
            bool matched = f.locationIDs.contains(currentCellID) || (currentWorldspaceID && f.locationIDs.contains(currentWorldspaceID));

            if (!matched) {
                for (auto locID : currentLocationIDs) {
                    if (f.locationIDs.contains(locID)) {
                        matched = true;
                        break;
                    }
                }
            }
            
            if (!matched) {
                currentRule.filterCache.locationMatch = false;
                return false;
            }
        }
    
        // Checking excluding filters
        if (!f.locationIDsNot.empty()) {
            bool excluded = f.locationIDsNot.contains(currentCellID) || (currentWorldspaceID && f.locationIDsNot.contains(currentWorldspaceID));

            if (!excluded) {
                for (auto locID : currentLocationIDs) {
                    if (f.locationIDsNot.contains(locID)) {
                        excluded = true;
                        break;
                    }
                }
            }
            
            if (excluded) {
                currentRule.filterCache.locationMatch = false;
                return false;
            }
        }
    
        // Checking the including formlists
        if (!f.locationLists.empty()) {
            bool matched = false;
            for (auto listFormID : f.locationLists) {
                auto* list = RE::TESForm::LookupByID<RE::BGSListForm>(listFormID);
                if (!list) continue;
                
                for (auto* el : list->forms) {
                    if (!el) continue;
                    auto elID = el->GetFormID();
                    
                    if (elID == currentCellID || 
                        (currentWorldspaceID && elID == currentWorldspaceID) ||
                        currentLocationIDs.contains(elID)) {
                        matched = true;
                        break;
                    }
                }
                if (matched) break;
            }
            if (!matched) {
                currentRule.filterCache.locationMatch = false;
                return false;
            }
        }
    
        // Checking the excluding formlists
        if (!f.locationListsNot.empty()) {
            for (auto listFormID : f.locationListsNot) {
                auto* list = RE::TESForm::LookupByID<RE::BGSListForm>(listFormID);
                if (!list) continue;
                
                for (auto* el : list->forms) {
                    if (!el) continue;
                    auto elID = el->GetFormID();
                    
                    if (elID == currentCellID || 
                        (currentWorldspaceID && elID == currentWorldspaceID) ||
                        currentLocationIDs.contains(elID)) {
                        currentRule.filterCache.locationMatch = false;
                        return false;
                    }
                }
            }
        }
    
        currentRule.filterCache.locationMatch = true;
        currentRule.filterCache.lastUpdate = std::chrono::steady_clock::now();
        return true;
    }
    
    bool CheckWeatherFilter(const Filter& f, Rule& currentRule) {
        if (f.weatherIDs.empty() && f.weatherIDsNot.empty() && f.weatherLists.empty() && f.weatherListsNot.empty()) {
            currentRule.filterCache.weatherMatch = true;
            return true;
        }

        if (currentRule.filterCache.IsValid() && currentRule.filterCache.weatherMatch.has_value()) {
            return currentRule.filterCache.weatherMatch.value();
        }
    
        auto* sky = RE::Sky::GetSingleton();
        if (!sky || !sky->currentWeather) {
            currentRule.filterCache.weatherMatch = f.weatherIDs.empty() && f.weatherLists.empty();
            return currentRule.filterCache.weatherMatch.value();
        }
    
        RE::FormID currentWeatherID = sky->currentWeather->GetFormID();

        if (!f.weatherIDs.empty()) {
            if (!f.weatherIDs.contains(currentWeatherID)) {
                currentRule.filterCache.weatherMatch = false;
                return false;
            }
        }

        if (!f.weatherIDsNot.empty()) {
            if (f.weatherIDsNot.contains(currentWeatherID)) {
                currentRule.filterCache.weatherMatch = false;
                return false;
            }
        }

        if (!f.weatherLists.empty()) {
            bool matched = false;
            for (auto listFormID : f.weatherLists) {
                auto* list = RE::TESForm::LookupByID<RE::BGSListForm>(listFormID);
                if (!list) continue;
                
                for (auto* el : list->forms) {
                    if (el && el->GetFormID() == currentWeatherID) {
                        matched = true;
                        break;
                    }
                }
                if (matched) break;
            }
            if (!matched) {
                currentRule.filterCache.weatherMatch = false;
                return false;
            }
        }

        if (!f.weatherListsNot.empty()) {
            for (auto listFormID : f.weatherListsNot) {
                auto* list = RE::TESForm::LookupByID<RE::BGSListForm>(listFormID);
                if (!list) continue;
                
                for (auto* el : list->forms) {
                    if (el && el->GetFormID() == currentWeatherID) {
                        currentRule.filterCache.weatherMatch = false;
                        return false;
                    }
                }
            }
        }
    
        currentRule.filterCache.weatherMatch = true;
        currentRule.filterCache.lastUpdate = std::chrono::steady_clock::now();
        return true;
    }

    void RuleManager::ResetInteractionCounts()
    {
        std::unique_lock lock(_ruleMutex);
        _limitCounts.clear();
        _interactionsCounts.clear();
    }
    
    void RuleManager::OnSave(SKSE::SerializationInterface* intf)
    {
        std::shared_lock lock(_ruleMutex);
    
        if (!intf->OpenRecord('LCNT', 2))
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
            if (type == 'LCNT') {       // limit
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
            ser->SetUniqueID('OIFL');   // Unique ID for Object Impact Framework

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

    bool IsDllPresent(const std::string& dllName)
    {
        namespace fs = std::filesystem;
        fs::path dllPath = fs::path("Data") / "SKSE" / "Plugins" / dllName;
        return fs::exists(dllPath);
    }

    // ------------------ Singleton ------------------
    RuleManager* RuleManager::GetSingleton() {
        static RuleManager inst;
        return &inst;
    }

    // ------------------ Load ------------------
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

        logger::info("Total rules loaded: {}", _rules.size());
    }

    // ------------------ Parse single file ------------------
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

            // -------- events ----------
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
                else logger::warn("Unknown event '{}' in {}", ev, path.string());
            }

            if (r.events.empty()) {
                logger::warn("Skipping rule in {}: no valid events", path.string());
                continue;
            }

            // -------- filters ----------
            bool hasObjectIdentifier = false;

            if (jr.contains("filter") && jr["filter"].is_object()) {
                const auto& jf = jr["filter"];

                if (jf.contains("chance") && jf["chance"].is_number()) {
                    try {
                        r.filter.chance = jf["chance"].get<float>();
                    } catch (const std::exception& e) {
                        logger::warn("Invalid chance value in filter of {}: {}", path.string(), e.what());
                    }
                } 

                if (jf.contains("interactions") && jf["interactions"].is_number_unsigned()) {
                    try {
                        r.filter.interactions = jf["interactions"].get<std::uint32_t>();
                    } catch (const std::exception& e) {
                        logger::warn("Invalid interactions value in filter of {}: {}", path.string(), e.what());
                    }
                }

                if (jf.contains("limit") && jf["limit"].is_number_unsigned()) {
                    try {
                        r.filter.limit = jf["limit"].get<std::uint32_t>();
                    } catch (const std::exception& e) {
                        logger::warn("Invalid limit value in filter of {}: {}", path.string(), e.what());
                    }
                }

                if (jf.contains("formtypes") && jf["formtypes"].is_array()) {
                    for (auto const& ft : jf["formtypes"]) {
                        if (ft.is_string()) r.filter.formTypes.insert(MapStringToFormType(tolower_str(ft.get<std::string>())));
                    }
                    if (!r.filter.formTypes.empty()) hasObjectIdentifier = true;
                }

                if (jf.contains("formids") && jf["formids"].is_array()) {
                    for (auto const& bid : jf["formids"]) {
                        if (bid.is_string()) {
                            if (auto* form = GetFormFromIdentifier<RE::TESForm>(bid.get<std::string>())) {
                                r.filter.formIDs.insert(form->GetFormID());
                            } else {
                                logger::warn("Invalid formID '{}' in filter of {}", bid.get<std::string>(), path.string());
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
                                logger::warn("Invalid formID '{}' in filter of {}", bid.get<std::string>(), path.string());
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
                                    logger::warn("Invalid formlist '{}' in filter of {}", formIdStr, path.string());
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
                                    logger::warn("Invalid formlist '{}' in filter of {}", formIdStr, path.string());
                                }
                            }
                        }
                    }
                }
                
                if (jf.contains("keywords") && jf["keywords"].is_array()) {
                    for (auto const& kwEntry : jf["keywords"]) {
                        if (kwEntry.is_string()) {
                            std::string kwStr = kwEntry.get<std::string>();
                            RE::BGSKeyword* kw = nullptr;
                            
                            if (kwStr.find(':') != std::string::npos) {
                                kw = GetFormFromIdentifier<RE::BGSKeyword>(kwStr);
                            } else {
                                kw = GetKeywordByName(kwStr);
                            }
                            
                            if (kw) {
                                r.filter.keywords.insert(kw);
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
                            RE::BGSKeyword* kw = nullptr;
                            
                            if (kwStr.find(':') != std::string::npos) {
                                kw = GetFormFromIdentifier<RE::BGSKeyword>(kwStr);
                            } else {
                                kw = GetKeywordByName(kwStr);
                            }
                            
                            if (kw) {
                                r.filter.keywordsNot.insert(kw);
                            } else {
                                logger::warn("Keyword not found: '{}' in keywordsNot filter of {}", 
                                kwStr, path.string());
                            }
                        }
                    }
                }

                if (jf.contains("questitemstatus") && jf["questitemstatus"].is_number_unsigned()) {
                    try {
                        r.filter.questItemStatus = jf["questitemstatus"].get<std::uint32_t>();
                    } catch (const std::exception& e) {
                        logger::warn("Invalid QuestItemStatus value in filter of {}: {}", path.string(), e.what());
                    }
                }

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

                if (jf.contains("weaponstypes") && jf["weaponstypes"].is_array()) {
                    for (auto const& wt : jf["weaponstypes"]) {
                        if (wt.is_string()) {
                            r.filter.weaponsTypes.insert(MapWeaponTypeToString(tolower_str(wt.get<std::string>())));
                        } else {
                            logger::warn("Invalid weapon type '{}' in filter of {}", wt.get<std::string>(), path.string());
                        }
                    }
                }

                if (jf.contains("weaponstypesnot") && jf["weaponstypesnot"].is_array()) {
                    for (auto const& wt : jf["weaponstypesnot"]) {
                        if (wt.is_string()) {
                            r.filter.weaponsTypesNot.insert(MapWeaponTypeToString(tolower_str(wt.get<std::string>())));
                        } else {
                            logger::warn("Invalid weapon type '{}' in filter of {}", wt.get<std::string>(), path.string());
                        }
                    }
                }

                if (jf.contains("weaponskeywords") && jf["weaponskeywords"].is_array()) {
                    for (auto const& kwEntry : jf["weaponskeywords"]) {
                        if (kwEntry.is_string()) {
                            std::string kwStr = kwEntry.get<std::string>();
                            RE::BGSKeyword* kw = nullptr;
                
                            if (kwStr.find(':') != std::string::npos) {
                                kw = GetFormFromIdentifier<RE::BGSKeyword>(kwStr);
                            } else {
                                kw = GetKeywordByName(kwStr);
                            }
                
                            if (kw) {
                                r.filter.weaponsKeywords.insert(kw);
                            } else {
                                logger::warn("Keyword not found: '{}' in weaponsKeywords filter of {}", 
                                kwStr, path.string());
                            }
                        }
                    }
                }
                
                if (jf.contains("weaponskeywordsnot") && jf["weaponskeywordsnot"].is_array()) {
                    for (auto const& kwEntry : jf["weaponskeywordsnot"]) {
                        if (kwEntry.is_string()) {
                            std::string kwStr = kwEntry.get<std::string>();
                            RE::BGSKeyword* kw = nullptr;
                
                            if (kwStr.find(':') != std::string::npos) {
                                kw = GetFormFromIdentifier<RE::BGSKeyword>(kwStr);
                            } else {
                                kw = GetKeywordByName(kwStr);
                            }
                
                            if (kw) {
                                r.filter.weaponsKeywordsNot.insert(kw);
                            } else {
                                logger::warn("Keyword not found: '{}' in weaponsKeywordsNot filter of {}", 
                                kwStr, path.string());
                            }
                        }
                    }
                }

                if (jf.contains("weapons") && jf["weapons"].is_array()) {
                    for (auto const& wf : jf["weapons"]) {
                        if (wf.is_string()) {
                            if (auto* weapon = GetFormFromIdentifier<RE::TESObjectWEAP>(wf.get<std::string>())) {
                                r.filter.weapons.insert(weapon);
                            } else if (auto* spell = GetFormFromIdentifier<RE::SpellItem>(wf.get<std::string>())) {
                                r.filter.weapons.insert(spell);
                            } else {
                                logger::warn("Invalid weapon formID '{}' in filter of {}", wf.get<std::string>(), path.string());
                            }
                        }
                    }
                }

                if (jf.contains("weaponsnot") && jf["weaponsnot"].is_array()) {
                    for (auto const& wf : jf["weaponsnot"]) {
                        if (wf.is_string()) {
                            if (auto* weapon = GetFormFromIdentifier<RE::TESObjectWEAP>(wf.get<std::string>())) {
                                r.filter.weaponsNot.insert(weapon);
                            } else if (auto* spell = GetFormFromIdentifier<RE::SpellItem>(wf.get<std::string>())) {
                                r.filter.weaponsNot.insert(spell);
                            } else {
                                logger::warn("Invalid weapon formID '{}' in filter of {}", wf.get<std::string>(), path.string());
                            }
                        }
                    }
                }

                if (jf.contains("weaponsformlists") && jf["weaponsformlists"].is_array()) {
                    for (const auto& entry : jf["weaponsformlists"]) {
                        if (entry.is_object()) {
                            std::string formIdStr;
                            int idx = -1;
                
                            if (entry.contains("formid") && entry["formid"].is_string()) {
                                formIdStr = entry["formid"].get<std::string>();
                
                                if (entry.contains("index") && entry["index"].is_number_integer())
                                    idx = entry["index"].get<int>();
                
                                auto* list = GetFormFromIdentifier<RE::BGSListForm>(formIdStr);
                                if (list) {
                                    r.filter.weaponsLists.push_back({ list->formID, idx });
                                } else {
                                    logger::warn("Invalid weapons formlist '{}' in filter of {}", formIdStr, path.string());
                                }
                            }
                        }
                    }
                }

                if (jf.contains("weaponsformlistsnot") && jf["weaponsformlistsnot"].is_array()) {
                    for (const auto& entry : jf["weaponsformlistsnot"]) {
                        if (entry.is_object()) {
                            std::string formIdStr;
                            int idx = -1;
                
                            if (entry.contains("formid") && entry["formid"].is_string()) {
                                formIdStr = entry["formid"].get<std::string>();
                
                                if (entry.contains("index") && entry["index"].is_number_integer())
                                    idx = entry["index"].get<int>();
                
                                auto* list = GetFormFromIdentifier<RE::BGSListForm>(formIdStr);
                                if (list) {
                                    r.filter.weaponsListsNot.push_back({ list->formID, idx });
                                } else {
                                    logger::warn("Invalid weapons formlist '{}' in filter of {}", formIdStr, path.string());
                                }
                            }
                        }
                    }
                }

                if (jf.contains("projectiles") && jf["projectiles"].is_array()) {
                    for (auto const& pf : jf["projectiles"]) {
                        if (pf.is_string()) {
                            if (auto* proj = GetFormFromIdentifier<RE::BGSProjectile>(pf.get<std::string>())) {
                                r.filter.projectiles.insert(proj);
                            } else {
                                logger::warn("Invalid projectile formID '{}' in filter of {}", pf.get<std::string>(), path.string());
                            }
                        }
                    }
                }

                if (jf.contains("projectilesnot") && jf["projectilesnot"].is_array()) {
                    for (auto const& pf : jf["projectilesnot"]) {
                        if (pf.is_string()) {
                            if (auto* proj = GetFormFromIdentifier<RE::BGSProjectile>(pf.get<std::string>())) {
                                r.filter.projectilesNot.insert(proj);
                            } else {
                                logger::warn("Invalid projectile formID '{}' in filter of {}", pf.get<std::string>(), path.string());
                            }
                        }
                    }
                }

                if (jf.contains("attacks") && jf["attacks"].is_array()) {
                    for (auto const& at : jf["attacks"]) {
                        if (at.is_string()) {
                            r.filter.attackTypes.insert(MapAttackTypeToString(tolower_str(at.get<std::string>())));
                        } else {
                            logger::warn("Invalid attack type '{}' in filter of {}", at.get<std::string>(), path.string());
                        }
                    }
                }

                if (jf.contains("attacksnot") && jf["attacksnot"].is_array()) {
                    for (auto const& at : jf["attacksnot"]) {
                        if (at.is_string()) {
                            r.filter.attackTypesNot.insert(MapAttackTypeToString(tolower_str(at.get<std::string>())));
                        } else {
                            logger::warn("Invalid attack type '{}' in filter of {}", at.get<std::string>(), path.string());
                        }
                    }
                }

                if (jf.contains("locations") && jf["locations"].is_array()) {
                    for (auto const& loc : jf["locations"]) {
                        if (loc.is_string()) {
                            auto formStr = loc.get<std::string>();
                            if (auto* form = GetFormFromIdentifier<RE::TESForm>(formStr)) {
                                if (form->Is(RE::FormType::Cell) || form->Is(RE::FormType::Location) || form->Is(RE::FormType::WorldSpace)) {
                                    r.filter.locationIDs.insert(form->GetFormID());
                                } else if (auto* formList = form->As<RE::BGSListForm>()) {
                                    r.filter.locationLists.push_back(form->GetFormID());
                                } else {
                                    logger::warn("Invalid location form '{}' (not Cell, Location, Worldspace or FormList) in filter of {}", formStr, path.string());
                                }
                            } else {
                                logger::warn("Invalid location formID '{}' in filter of {}", formStr, path.string());
                            }
                        }
                    }
                }

                if (jf.contains("locationsnot") && jf["locationsnot"].is_array()) {
                    for (auto const& loc : jf["locationsnot"]) {
                        if (loc.is_string()) {
                            auto formStr = loc.get<std::string>();
                            if (auto* form = GetFormFromIdentifier<RE::TESForm>(formStr)) {
                                if (form->Is(RE::FormType::Cell) || form->Is(RE::FormType::Location) || form->Is(RE::FormType::WorldSpace)) {
                                    r.filter.locationIDsNot.insert(form->GetFormID());
                                } else if (auto* formList = form->As<RE::BGSListForm>()) {
                                    r.filter.locationListsNot.push_back(form->GetFormID());
                                } else {
                                    logger::warn("Invalid location form '{}' (not Cell, Location, Worldspace or FormList) in filter of {}", formStr, path.string());
                                }
                            } else {
                                logger::warn("Invalid location formID '{}' in filter of {}", formStr, path.string());
                            }
                        }
                    }
                }

                if (jf.contains("weathers") && jf["weathers"].is_array()) {
                    for (auto const& weather : jf["weathers"]) {
                        if (weather.is_string()) {
                            auto formStr = weather.get<std::string>();
                            if (auto* form = GetFormFromIdentifier<RE::TESForm>(formStr)) {
                                if (form->Is(RE::FormType::Weather)) {
                                    r.filter.weatherIDs.insert(form->GetFormID());
                                } else if (auto* formList = form->As<RE::BGSListForm>()) {
                                    r.filter.weatherLists.push_back(form->GetFormID());
                                } else {
                                    logger::warn("Invalid weather form '{}' (not Weather or FormList) in filter of {}", formStr, path.string());
                                }
                            } else {
                                logger::warn("Invalid weather formID '{}' in filter of {}", formStr, path.string());
                            }
                        }
                    }
                }

                if (jf.contains("weathersnot") && jf["weathersnot"].is_array()) {
                    for (auto const& weather : jf["weathersnot"]) {
                        if (weather.is_string()) {
                            auto formStr = weather.get<std::string>();
                            if (auto* form = GetFormFromIdentifier<RE::TESForm>(formStr)) {
                                if (form->Is(RE::FormType::Weather)) {
                                    r.filter.weatherIDsNot.insert(form->GetFormID());
                                } else if (auto* formList = form->As<RE::BGSListForm>()) {
                                    r.filter.weatherListsNot.push_back(form->GetFormID());
                                } else {
                                    logger::warn("Invalid weather form '{}' (not Weather or FormList) in filter of {}", formStr, path.string());
                                }
                            } else {
                                logger::warn("Invalid weather formID '{}' in filter of {}", formStr, path.string());
                            }
                        }
                    }
                }

                if (jf.contains("perks") && jf["perks"].is_array()) {
                    for (const auto& perk : jf["perks"]) {
                        if (perk.is_string()) {
                            auto perkStr = perk.get<std::string>();
                            if (auto* form = GetFormFromIdentifier<RE::BGSPerk>(perkStr)) {
                                r.filter.perks.insert(form->GetFormID());
                            } else {
                                logger::warn("Invalid player perk '{}' in filter of {}", perkStr, path.string());
                            }
                        }
                    }
                }

                if (jf.contains("perksnot") && jf["perksnot"].is_array()) {
                    for (const auto& perk : jf["perksnot"]) {
                        if (perk.is_string()) {
                            auto perkStr = perk.get<std::string>();
                            if (auto* form = GetFormFromIdentifier<RE::BGSPerk>(perkStr)) {
                                r.filter.perksNot.insert(form->GetFormID());
                            } else {
                                logger::warn("Invalid player perk '{}' in filter of {}", perkStr, path.string());
                            }
                        }
                    }
                }

                if (jf.contains("hasitem") && jf["hasitem"].is_array()) {
                    for (auto const& item : jf["hasitem"]) {
                        if (item.is_string()) {
                            if (auto* form = GetFormFromIdentifier<RE::TESForm>(item.get<std::string>())) {
                                r.filter.hasItem.insert(form->GetFormID());
                            } else {
                                logger::warn("Invalid hasitem identifier '{}' in filter of {}", item.get<std::string>(), path.string());
                            }
                        }
                    }
                }
                
                if (jf.contains("hasitemnot") && jf["hasitemnot"].is_array()) {
                    for (auto const& item : jf["hasitemnot"]) {
                        if (item.is_string()) {
                            if (auto* form = GetFormFromIdentifier<RE::TESForm>(item.get<std::string>())) {
                                r.filter.hasItemNot.insert(form->GetFormID());
                            } else {
                                logger::warn("Invalid hasitemnot identifier '{}' in filter of {}", item.get<std::string>(), path.string());
                            }
                        }
                    }
                }
                
                if (jf.contains("levels") && jf["levels"].is_array()) {
                    for (auto const& lvl : jf["levels"]) {
                        if (lvl.is_string()) {
                            std::string levelStr = lvl.get<std::string>();

                            if (levelStr.empty()) {
                                logger::warn("Empty levels string in filter of {}", path.string());
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
                                        logger::warn("Invalid levels value '{}' in filter of {}: {}", levelStr, path.string(), e.what());
                                    }
                                } else if (levelStr[0] == '>' || levelStr[0] == '<' || levelStr[0] == '=') {
                                    condition.operator_type = levelStr.substr(0, 1);
                                    try {
                                        condition.value = std::stoi(levelStr.substr(1));
                                        r.filter.level.push_back(condition);
                                    } catch (const std::exception& e) {
                                        logger::warn("Invalid levels value '{}' in filter of {}: {}", levelStr, path.string(), e.what());
                                    }
                                } else {
                                    logger::warn("Invalid levels operator in '{}' in filter of {}", levelStr, path.string());
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
                                logger::warn("Empty levels string in filter of {}", path.string());
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
                                        logger::warn("Invalid levelsnot value '{}' in filter of {}: {}", levelStr, path.string(), e.what());
                                    }
                                } else if (levelStr[0] == '>' || levelStr[0] == '<' || levelStr[0] == '=') {
                                    condition.operator_type = levelStr.substr(0, 1);
                                    try {
                                        condition.value = std::stoi(levelStr.substr(1));
                                        r.filter.levelNot.push_back(condition);
                                    } catch (const std::exception& e) {
                                        logger::warn("Invalid levelsnot value '{}' in filter of {}: {}", levelStr, path.string(), e.what());
                                    }
                                } else {
                                    logger::warn("Invalid levelsnot operator in '{}' in filter of {}", levelStr, path.string());
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
                                    logger::warn("Invalid actorvalues value '{}' in filter of {}: {}", avStr, path.string(), e.what());
                                }
                            } else {
                                logger::warn("Invalid actorvalues format '{}' in filter of {}, expected format: 'ActorValue >= Value'", avStr, path.string());
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
                                    logger::warn("Invalid actorvaluesnot value '{}' in filter of {}: {}", avStr, path.string(), e.what());
                                }
                            } else {
                                logger::warn("Invalid actorvaluesnot format '{}' in filter of {}, expected format: 'ActorValue >= Value'", avStr, path.string());
                            }
                        }
                    }
                }     
            }           

            if (!hasObjectIdentifier) {
                logger::warn("Skipping rule in {}: no valid object identifiers (formTypes, formIDs, formLists, or keywords)", path.string());
                continue;
            }

            // -------- effects ----------
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
                {"spawnactor", EffectType::kSpawnActor},
                {"spawnimpact", EffectType::kSpawnImpact},
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
                {"spawneffectshaderonitem", EffectType::kSpawnEffectShaderOnItem}
            };

            for (const auto& effj : effectArray) {
                if (!effj.is_object()) continue;
                Effect eff;

                std::string typeStrRaw = effj.value("type", "spawnitem");
                std::string typeStr = tolower_str(typeStrRaw);
                auto it = effectTypeMap.find(typeStr);
                if (it == effectTypeMap.end()) {
                    logger::warn("Unknown effect type '{}' in {}", typeStr, path.string());
                    continue;
                }
                eff.type = it->second;

                eff.chance = effj.value("chance", r.filter.chance);

                const std::vector<EffectType> effectsWithoutItems = {
                    EffectType::kRemoveItem,
                    EffectType::kDisableItem,
                    EffectType::kEnableItem,
                    EffectType::kSpillInventory
                };

                bool needsItems = std::find(effectsWithoutItems.begin(), effectsWithoutItems.end(), eff.type) == effectsWithoutItems.end();
                if (needsItems) {
                    if (effj.contains("items") && effj["items"].is_array()) {
                        for (const auto& itemJson : effj["items"]) {
                            EffectExtendedData extData;
                            extData.form = nullptr;
                            extData.nonDeletable = itemJson.value("nondeletable", 0U);
                            extData.spawnType = itemJson.value("spawntype", 4U);
                            extData.fade = itemJson.value("fade", 0U);
                            extData.count = itemJson.value("count", 1U);
                            extData.radius = itemJson.value("radius", 150.0f);
                            extData.scale = itemJson.value("scale", -1.0f);
                            extData.amount = itemJson.value("amount", 1U);
                            extData.chance = itemJson.value("chance", 100.0f);
                            extData.duration = itemJson.value("duration", 1.0f);
                            extData.string = itemJson.value("string", std::string{});

                            bool haveIdentifier = false;

                            if (itemJson.contains("formid") && itemJson["formid"].is_string()) {
                                if (auto* form = GetFormFromIdentifier<RE::TESForm>(itemJson["formid"].get<std::string>())) {
                                    extData.form = form;
                                    extData.isFormList = false;
                                    extData.index = -1;
                                    haveIdentifier = true;
                                }
                            }

                            else if (itemJson.contains("formlist") && itemJson["formlist"].is_array()) {
                                for (const auto& formListEntry : itemJson["formlist"]) {
                                    if (formListEntry.is_object() && formListEntry.contains("formid") && formListEntry["formid"].is_string()) {
                                        std::string formListId = formListEntry["formid"].get<std::string>();
                                        int idx = formListEntry.value("index", -1);

                                        auto* list = GetFormFromIdentifier<RE::BGSListForm>(formListId);
                                        if (list) {
                                            extData.form = list;
                                            extData.isFormList = true;
                                            extData.index = idx;
                                            eff.items.emplace_back(extData.form, extData);
                                        } else {
                                            logger::warn("Invalid formlist '{}' in items of {}", formListId, path.string());
                                        }
                                    }
                                }
                            }

                            const std::vector<EffectType> effectsWithoutForm = {
                                EffectType::kApplyIngestible,
                                EffectType::kRemoveLight,
                                EffectType::kEnableLight,
                                EffectType::kDisableLight,
                                EffectType::kPlayIdle                            
                            };

                            bool needsForm = std::find(effectsWithoutForm.begin(), effectsWithoutForm.end(), eff.type) == effectsWithoutForm.end();

                            if (haveIdentifier || !needsForm) {
                                auto formToAdd = needsForm ? extData.form : nullptr;
                                eff.items.emplace_back(formToAdd, extData);
                            }

                            if (eff.items.empty() && needsForm) continue;
                        }
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

    // ------------------ Match ------------------
    bool RuleManager::MatchFilter(const Filter& f, const RuleContext& ctx, Rule& currentRule) const {
        if (!ctx.baseObj) return false;

        if (!f.formTypes.empty() && !f.formTypes.contains(ctx.baseObj->GetFormType())) return false;
        if (!f.formIDs.empty() && !f.formIDs.contains(ctx.baseObj->GetFormID())) return false;
        if (!f.formIDsNot.empty() && f.formIDsNot.contains(ctx.baseObj->GetFormID())) return false;
        if (!f.formLists.empty()) {
            bool matched = false;
            for (const auto& entry : f.formLists) {
                auto* list = RE::TESForm::LookupByID<RE::BGSListForm>(entry.formID);
                if (!list) continue;

                if (entry.index == -2) {
                    // Find the object index in a formlist
                    int foundIdx = -1;
                    for (int i = 0; i < static_cast<int>(list->forms.size()); ++i) {
                        if (list->forms[i] && list->forms[i]->GetFormID() == ctx.baseObj->GetFormID()) {
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
                        if (el && el->GetFormID() == ctx.baseObj->GetFormID()) {
                            matched = true;
                            break;
                        }
                    }
                } else {
                    // Check all the elements of the list
                    for (auto* el : list->forms) {
                        if (el && el->GetFormID() == ctx.baseObj->GetFormID()) {
                            matched = true;
                            break;
                        }
                    }
                    if (matched) break;
                }
            }
            if (!matched) return false;
        }

        if (!f.formListsNot.empty()) {
            for (const auto& entry : f.formListsNot) {
                auto* list = RE::TESForm::LookupByID<RE::BGSListForm>(entry.formID);
                if (!list) continue;
                // Check all the elements of the list
                for (auto* el : list->forms) {
                    if (el && el->GetFormID() == ctx.baseObj->GetFormID()) return false;
                }
            }
        }

        if (!f.keywords.empty()) {
            auto* kwf = ctx.baseObj->As<RE::BGSKeywordForm>();
            if (!kwf) return false;
            
            bool hasAny = false;
            for (auto* kw : f.keywords) {
                if (kw && kwf->HasKeyword(kw)) {
                    hasAny = true;
                    break;
                }
            }
            if (!hasAny) return false;
        }

        if (!f.keywordsNot.empty()) {
            auto* kwf = ctx.baseObj->As<RE::BGSKeywordForm>();
            if (!kwf) return false;

            for (auto* kw : f.keywordsNot) {
                if (kw && kwf->HasKeyword(kw)) return false;
            }
        }

        if (f.questItemStatus != 3) {
            auto currentStatus = GetQuestItemStatus(ctx.target);
            
            if (f.questItemStatus != currentStatus) {
                return false;
            }
        }

        if (!f.requiredPlugins.empty()) {
            auto* dataHandler = RE::TESDataHandler::GetSingleton();
            bool allPresent = std::all_of(f.requiredPlugins.begin(), f.requiredPlugins.end(), [&](const std::string& name) { return dataHandler->LookupModByName(name.c_str()); });
            if (!allPresent) return false;
        }

        if (!f.requiredPluginsNot.empty()) {
            auto* dataHandler = RE::TESDataHandler::GetSingleton();
            bool anyPresent = std::any_of(f.requiredPluginsNot.begin(), f.requiredPluginsNot.end(), [&](const std::string& name) { return dataHandler->LookupModByName(name.c_str()); });
            if (anyPresent) return false;
        }

        if (!f.requiredDLLs.empty()) {
            bool allPresent = std::all_of(f.requiredDLLs.begin(), f.requiredDLLs.end(), [](const std::string& n) { return IsDllPresent(n); });
            if (!allPresent) return false;
        }        

        if (!f.requiredDLLsNot.empty()) {
            bool anyPresent = std::any_of(f.requiredDLLsNot.begin(), f.requiredDLLsNot.end(), [](const std::string& n) { return IsDllPresent(n); });
            if (anyPresent) return false;
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

        if (ctx.isHitEvent) {
            if (!f.weaponsTypes.empty() && f.weaponsTypes.find(ctx.weaponType) == f.weaponsTypes.end()) return false;
            if (!f.weaponsTypesNot.empty() && f.weaponsTypesNot.find(ctx.weaponType) != f.weaponsTypesNot.end()) return false;
            if (!f.weapons.empty()) {
                if (!ctx.attackSource) return false;
                if (f.weapons.find(ctx.attackSource) == f.weapons.end()) return false;
            }
            if (!f.weaponsNot.empty()) {
                if (!ctx.attackSource) return false;
                if (f.weaponsNot.find(ctx.attackSource) != f.weaponsNot.end()) return false;
            }
            if (!f.weaponsLists.empty()) {
                if (!ctx.attackSource) return false;
                bool matched = false;
                for (const auto& entry : f.weaponsLists) {
                    auto* list = RE::TESForm::LookupByID<RE::BGSListForm>(entry.formID);
                    if (!list) continue;

                    if (entry.index == -2) {
                        int foundIdx = -1;
                        for (int i = 0; i < static_cast<int>(list->forms.size()); ++i) {
                            if (list->forms[i] && list->forms[i]->GetFormID() == ctx.attackSource->GetFormID()) {
                                foundIdx = i;
                                break;
                            }
                        }
                        if (foundIdx != -1) {
                            currentRule.dynamicIndex = foundIdx;
                            matched = true;
                        }
                    } else if (entry.index >= 0) {
                        if (entry.index < static_cast<int>(list->forms.size())) {
                            auto* el = list->forms[entry.index];
                            if (el && el->GetFormID() == ctx.attackSource->GetFormID()) {
                                matched = true;
                                break;
                            }
                        }
                    } else {
                        for (auto* el : list->forms) {
                            if (el && el->GetFormID() == ctx.attackSource->GetFormID()) {
                                matched = true;
                                break;
                            }
                        }
                        if (matched) break;
                    }
                }
                if (!matched) return false;
            }
            if (!f.weaponsListsNot.empty()) {
                if (!ctx.attackSource) return false;
                for (const auto& entry : f.weaponsListsNot) {
                    auto* list = RE::TESForm::LookupByID<RE::BGSListForm>(entry.formID);
                    if (!list) continue;
                    for (auto* el : list->forms) {
                        if (el && el->GetFormID() == ctx.attackSource->GetFormID()) return false;
                    }
                }
            }
            if (!f.projectiles.empty() && (!ctx.projectile || f.projectiles.find(ctx.projectile) == f.projectiles.end())) return false;
            if (!f.projectilesNot.empty() && ctx.projectile && f.projectilesNot.find(ctx.projectile) != f.projectilesNot.end()) return false;
            if (!f.attackTypes.empty() && f.attackTypes.find(ctx.attackType) == f.attackTypes.end()) return false;
            if (!f.attackTypesNot.empty() && f.attackTypesNot.find(ctx.attackType) != f.attackTypesNot.end()) return false;
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
                if (!kwf) return false;

                for (auto* kw : f.weaponsKeywordsNot) {
                    if (kw && kwf->HasKeyword(kw)) return false;
                }
            }
        }

        return true;
    }

    // ------------------ Apply ------------------
    void RuleManager::ApplyEffect(const Effect& eff, const RuleContext& ctx, Rule& currentRule) const {     
        if (!ctx.target || !ctx.target->GetBaseObject()) return;
        if (!ctx.source || !ctx.source->GetBaseObject()) return;
        if (!CheckLocationFilter(currentRule.filter, ctx, currentRule)) return;
        if (!CheckWeatherFilter(currentRule.filter, currentRule)) return;

        Effect effCopy = eff;
        RuleContext ctxCopy = ctx;
        Rule ruleCopy = currentRule;

        SKSE::GetTaskInterface()->AddTask([effCopy, ctxCopy, ruleCopy]() {
            auto* target = ctxCopy.target;
            auto* source = ctxCopy.source;

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

            static thread_local std::mt19937 rng(std::random_device{}());
            float globalRoll = std::uniform_real_distribution<float>(0.f, 100.f)(rng);
            if (globalRoll > effCopy.chance) return;
            
                try {
                    switch (effCopy.type) {
                        case EffectType::kRemoveItem:
                            Effects::RemoveItem(ctxCopy);
                            break;

                        case EffectType::kDisableItem:
                            Effects::DisableItem(ctxCopy);
                            break;

                        case EffectType::kEnableItem:
                            Effects::EnableItem(ctxCopy);
                            break;

                        case EffectType::kSpillInventory:
                            Effects::SpillInventory(ctxCopy);
                            break;

                        case EffectType::kSpawnItem:
                        {
                            std::vector<ItemSpawnData> itemsData;
                            for (const auto& [form, extData] : effCopy.items) {
                                if (!form) continue;
                                float roll = std::uniform_real_distribution<float>(0.f, 100.f)(rng);
                                if (roll > extData.chance) continue;

                                if (extData.isFormList) {
                                    auto* list = form->As<RE::BGSListForm>();
                                    if (!list) continue;

                                    int idx = extData.index;
                                    bool spawnAll = false;

                                    // Use random index if -3 is specified
                                    if (idx == -3) {
                                        idx = std::uniform_int_distribution<int>(0, static_cast<int>(list->forms.size()) - 1)(rng);
                                    }

                                    // Use dynamic index if -2 is specified
                                    else if (idx == -2) {
                                        idx = ruleCopy.dynamicIndex;
                                    }

                                    // If index is -1, spawn all elements in the list
                                    else if (idx == -1) {
                                        spawnAll = true;
                                    }

                                    if (spawnAll) {
                                        for (auto* el : list->forms) {
                                            if (!el) continue;
                                            if (auto* item = el->As<RE::TESBoundObject>()) itemsData.emplace_back(item, extData.count, extData.spawnType, extData.fade, extData.scale);
                                        }
                                    }

                                    // Use index to select a specific element
                                    else if (idx >= 0 && idx < static_cast<int>(list->forms.size())) {
                                        auto* el = list->forms[idx];
                                        if (el) {
                                            if (auto* item = el->As<RE::TESBoundObject>()) itemsData.emplace_back(item, extData.count, extData.spawnType, extData.fade, extData.scale);
                                        }
                                    }
                                }
                                else if (auto* item = form->As<RE::TESBoundObject>()) {
                                    itemsData.emplace_back(item, extData.count, extData.spawnType, extData.fade, extData.scale);
                                }
                            }
                            if (!itemsData.empty()) {
                                Effects::SpawnItem(ctxCopy, itemsData);
                            }
                        }
                        break;
                        
                        case EffectType::kSpawnSpell:
                        {
                            std::vector<SpellSpawnData> spellsData;
                            for (const auto& [form, extData] : effCopy.items) {
                                if (!form) continue;
                                float roll = std::uniform_real_distribution<float>(0.f, 100.f)(rng);
                                if (roll > extData.chance) continue;

                                if (extData.isFormList) {
                                    auto* list = form->As<RE::BGSListForm>();
                                    if (!list) continue;

                                    int idx = extData.index;
                                    bool spawnAll = false;

                                    if (idx == -3) {
                                        idx = std::uniform_int_distribution<int>(0, static_cast<int>(list->forms.size()) - 1)(rng);
                                    }
                                    else if (idx == -2) {
                                        idx = ruleCopy.dynamicIndex;
                                    }
                                    else if (idx == -1) {
                                        spawnAll = true;
                                    }
                                    if (spawnAll) {
                                        for (auto* el : list->forms) {
                                            if (!el) continue;
                                            if (auto* spell = el->As<RE::SpellItem>()) spellsData.emplace_back(spell, extData.count, extData.radius);
                                        }
                                    }
                                    else if (idx >= 0 && idx < static_cast<int>(list->forms.size())) {
                                        auto* el = list->forms[idx];
                                        if (el) {
                                            if (auto* spell = el->As<RE::SpellItem>()) spellsData.emplace_back(spell, extData.count, extData.radius);
                                        }
                                    }
                                }
                                else if (auto* spell = form->As<RE::SpellItem>()) {
                                    spellsData.emplace_back(spell, extData.count, extData.radius);
                                }
                            }
                            if (!spellsData.empty()) {
                                Effects::SpawnSpell(ctxCopy, spellsData);
                            }
                        }
                        break;
                        
                        case EffectType::kSpawnSpellOnItem:
                        {
                            std::vector<SpellSpawnData> spellsData;
                            for (const auto& [form, extData] : effCopy.items) {
                                if (!form) continue;
                                float roll = std::uniform_real_distribution<float>(0.f, 100.f)(rng);
                                if (roll > extData.chance) continue;

                                if (extData.isFormList) {
                                    auto* list = form->As<RE::BGSListForm>();
                                    if (!list) continue;

                                    int idx = extData.index;
                                    bool spawnAll = false;

                                    if (idx == -3) {
                                        idx = std::uniform_int_distribution<int>(0, static_cast<int>(list->forms.size()) - 1)(rng);
                                    }
                                    else if (idx == -2) {
                                        idx = ruleCopy.dynamicIndex;
                                    }
                                    else if (idx == -1) {
                                        spawnAll = true;
                                    }
                                    if (spawnAll) {
                                        for (auto* el : list->forms) {
                                            if (!el) continue;
                                            if (auto* spell = el->As<RE::SpellItem>()) spellsData.emplace_back(spell, extData.count);
                                        }
                                    }
                                    else if (idx >= 0 && idx < static_cast<int>(list->forms.size())) {
                                        auto* el = list->forms[idx];
                                        if (el) {
                                            if (auto* spell = el->As<RE::SpellItem>()) spellsData.emplace_back(spell, extData.count);
                                        }
                                    }
                                }
                                else if (auto* spell = form->As<RE::SpellItem>()) {
                                    spellsData.emplace_back(spell, extData.count);
                                }
                            }
                            if (!spellsData.empty()) {
                                Effects::SpawnSpellOnItem(ctxCopy, spellsData);
                            }
                        }
                        break;
                        
                        case EffectType::kSpawnActor:
                        {
                            std::vector<ActorSpawnData> actorsData;
                            for (const auto& [form, extData] : effCopy.items) {
                                if (!form) continue;
                                float roll = std::uniform_real_distribution<float>(0.f, 100.f)(rng);
                                if (roll > extData.chance) continue;

                                if (extData.isFormList) {
                                    auto* list = form->As<RE::BGSListForm>();
                                    if (!list) continue;

                                    int idx = extData.index;
                                    bool spawnAll = false;

                                    if (idx == -3) {
                                        idx = std::uniform_int_distribution<int>(0, static_cast<int>(list->forms.size()) - 1)(rng);
                                    }
                                    else if (idx == -2) {
                                        idx = ruleCopy.dynamicIndex;
                                    }
                                    else if (idx == -1) {
                                        spawnAll = true;
                                    }
                                    if (spawnAll) {
                                        for (auto* el : list->forms) {
                                            if (!el) continue;
                                            if (auto* actor = el->As<RE::TESNPC>()) actorsData.emplace_back(actor, extData.count, extData.spawnType, extData.fade, extData.scale);
                                        }
                                    }
                                    else if (idx >= 0 && idx < static_cast<int>(list->forms.size())) {
                                        auto* el = list->forms[idx];
                                        if (el) {
                                            if (auto* actor = el->As<RE::TESNPC>()) actorsData.emplace_back(actor, extData.count, extData.spawnType, extData.fade, extData.scale);
                                        }
                                    }
                                }
                                else if (auto* actor = form->As<RE::TESNPC>()) {
                                    actorsData.emplace_back(actor, extData.count, extData.spawnType, extData.fade, extData.scale);
                                }
                            }
                            if (!actorsData.empty()) {
                                Effects::SpawnActor(ctxCopy, actorsData);
                            }
                        }
                        break;
                        
                        case EffectType::kSpawnImpact:
                        {
                            std::vector<ImpactSpawnData> impactsData;
                            for (const auto& [form, extData] : effCopy.items) {
                                if (!form) continue;
                                float roll = std::uniform_real_distribution<float>(0.f, 100.f)(rng);
                                if (roll > extData.chance) continue;

                                if (extData.isFormList) {
                                    auto* list = form->As<RE::BGSListForm>();
                                    if (!list) continue;

                                    int idx = extData.index;
                                    bool spawnAll = false;

                                    if (idx == -3) {
                                        idx = std::uniform_int_distribution<int>(0, static_cast<int>(list->forms.size()) - 1)(rng);
                                    }
                                    else if (idx == -2) {
                                        idx = ruleCopy.dynamicIndex;
                                    }
                                    else if (idx == -1) {
                                        spawnAll = true;
                                    }
                                    if (spawnAll) {
                                        for (auto* el : list->forms) {
                                            if (!el) continue;
                                            if (auto* impact = el->As<RE::BGSImpactDataSet>()) impactsData.emplace_back(impact, extData.count);
                                        }
                                    }
                                    else if (idx >= 0 && idx < static_cast<int>(list->forms.size())) {
                                        auto* el = list->forms[idx];
                                        if (el) {
                                            if (auto* impact = el->As<RE::BGSImpactDataSet>()) impactsData.emplace_back(impact, extData.count);
                                        }
                                    }
                                }
                                else if (auto* impact = form->As<RE::BGSImpactDataSet>()) {
                                    impactsData.emplace_back(impact, extData.count);
                                }
                            }
                            if (!impactsData.empty()) {
                                Effects::SpawnImpact(ctxCopy, impactsData);
                            }
                        }
                        break;
                        
                        case EffectType::kSpawnExplosion:
                        {
                            std::vector<ExplosionSpawnData> explosionsData;
                            for (const auto& [form, extData] : effCopy.items) {
                                if (!form) continue;
                                float roll = std::uniform_real_distribution<float>(0.f, 100.f)(rng);
                                if (roll > extData.chance) continue;

                                if (extData.isFormList) {
                                    auto* list = form->As<RE::BGSListForm>();
                                    if (!list) continue;

                                    int idx = extData.index;
                                    bool spawnAll = false;

                                    if (idx == -3) {
                                        idx = std::uniform_int_distribution<int>(0, static_cast<int>(list->forms.size()) - 1)(rng);
                                    }
                                    else if (idx == -2) {
                                        idx = ruleCopy.dynamicIndex;
                                    }
                                    else if (idx == -1) {
                                        spawnAll = true;
                                    }
                                    if (spawnAll) {
                                        for (auto* el : list->forms) {
                                            if (!el) continue;
                                            if (auto* explosion = el->As<RE::BGSExplosion>()) explosionsData.emplace_back(explosion, extData.count, extData.spawnType, extData.fade);
                                        }
                                    }
                                    else if (idx >= 0 && idx < static_cast<int>(list->forms.size())) {
                                        auto* el = list->forms[idx];
                                        if (el) {
                                            if (auto* explosion = el->As<RE::BGSExplosion>()) explosionsData.emplace_back(explosion, extData.count, extData.spawnType, extData.fade);
                                        }
                                    }
                                }
                                else if (auto* explosion = form->As<RE::BGSExplosion>()) {
                                    explosionsData.emplace_back(explosion, extData.count, extData.spawnType, extData.fade);
                                }
                            }
                            if (!explosionsData.empty()) {
                                Effects::SpawnExplosion(ctxCopy, explosionsData);
                            }
                        }
                        break;
                        
                        case EffectType::kSwapItem:
                        {
                            std::vector<ItemSpawnData> itemsData;
                            for (const auto& [form, extData] : effCopy.items) {
                                if (!form) continue;
                                float roll = std::uniform_real_distribution<float>(0.f, 100.f)(rng);
                                if (roll > extData.chance) continue;

                                if (extData.isFormList) {
                                    auto* list = form->As<RE::BGSListForm>();
                                    if (!list) continue;

                                    int idx = extData.index;
                                    bool spawnAll = false;

                                    if (idx == -3) {
                                        idx = std::uniform_int_distribution<int>(0, static_cast<int>(list->forms.size()) - 1)(rng);
                                    }
                                    else if (idx == -2) {
                                        idx = ruleCopy.dynamicIndex;
                                    }
                                    else if (idx == -1) {
                                        spawnAll = true;
                                    }
                                    if (spawnAll) {
                                        for (auto* el : list->forms) {
                                            if (!el) continue;
                                            if (auto* item = el->As<RE::TESBoundObject>()) itemsData.emplace_back(item, extData.count, extData.spawnType, extData.fade, extData.scale, extData.nonDeletable);
                                        }
                                    }
                                    else if (idx >= 0 && idx < static_cast<int>(list->forms.size())) {
                                        auto* el = list->forms[idx];
                                        if (el) {
                                            if (auto* item = el->As<RE::TESBoundObject>()) itemsData.emplace_back(item, extData.count, extData.spawnType, extData.fade, extData.scale, extData.nonDeletable);
                                        }
                                    }
                                }
                                else if (auto* item = form->As<RE::TESBoundObject>()) {
                                    itemsData.emplace_back(item, extData.count, extData.spawnType, extData.fade, extData.scale, extData.nonDeletable);
                                }
                            }
                            if (!itemsData.empty()) {
                                Effects::SwapItem(ctxCopy, itemsData);
                            }
                        }
                        break;
                        
                        case EffectType::kPlaySound:
                        {
                            std::vector<SoundSpawnData> soundsData;
                            for (const auto& [form, extData] : effCopy.items) {
                                if (!form) continue;
                                float roll = std::uniform_real_distribution<float>(0.f, 100.f)(rng);
                                if (roll > extData.chance) continue;

                                if (extData.isFormList) {
                                    auto* list = form->As<RE::BGSListForm>();
                                    if (!list) continue;

                                    int idx = extData.index;
                                    bool spawnAll = false;

                                    if (idx == -3) {
                                        idx = std::uniform_int_distribution<int>(0, static_cast<int>(list->forms.size()) - 1)(rng);
                                    }
                                    else if (idx == -2) {
                                        idx = ruleCopy.dynamicIndex;
                                    }
                                    else if (idx == -1) {
                                        spawnAll = true;
                                    }
                                    if (spawnAll) {
                                        for (auto* el : list->forms) {
                                            if (!el) continue;
                                            if (auto* sound = el->As<RE::BGSSoundDescriptorForm>()) soundsData.emplace_back(sound, extData.count);
                                        }
                                    }
                                    else if (idx >= 0 && idx < static_cast<int>(list->forms.size())) {
                                        auto* el = list->forms[idx];
                                        if (el) {
                                            if (auto* sound = el->As<RE::BGSSoundDescriptorForm>()) soundsData.emplace_back(sound, extData.count);
                                        }
                                    }
                                }
                                else if (auto* sound = form->As<RE::BGSSoundDescriptorForm>()) {
                                    soundsData.emplace_back(sound, extData.count);
                                }
                            }
                            if (!soundsData.empty()) {
                                Effects::PlaySound(ctxCopy, soundsData);
                            }
                        }
                        break;
                        
                        case EffectType::kSwapActor:
                        {
                            std::vector<ActorSpawnData> actorsData;
                            for (const auto& [form, extData] : effCopy.items) {
                                if (!form) continue;
                                float roll = std::uniform_real_distribution<float>(0.f, 100.f)(rng);
                                if (roll > extData.chance) continue;

                                if (extData.isFormList) {
                                    auto* list = form->As<RE::BGSListForm>();
                                    if (!list) continue;

                                    int idx = extData.index;
                                    bool spawnAll = false;

                                    if (idx == -3) {
                                        idx = std::uniform_int_distribution<int>(0, static_cast<int>(list->forms.size()) - 1)(rng);
                                    }
                                    else if (idx == -2) {
                                        idx = ruleCopy.dynamicIndex;
                                    }
                                    else if (idx == -1) {
                                        spawnAll = true;
                                    }
                                    if (spawnAll) {
                                        for (auto* el : list->forms) {
                                            if (!el) continue;
                                            if (auto* actor = el->As<RE::TESNPC>()) actorsData.emplace_back(actor, extData.count, extData.spawnType, extData.fade, extData.scale, extData.nonDeletable);
                                        }
                                    }
                                    else if (idx >= 0 && idx < static_cast<int>(list->forms.size())) {
                                        auto* el = list->forms[idx];
                                        if (el) {
                                            if (auto* actor = el->As<RE::TESNPC>()) actorsData.emplace_back(actor, extData.count, extData.spawnType, extData.fade, extData.scale, extData.nonDeletable);
                                        }
                                    }
                                }
                                else if (auto* actor = form->As<RE::TESNPC>()) {
                                    actorsData.emplace_back(actor, extData.count, extData.spawnType, extData.fade, extData.scale, extData.nonDeletable);
                                }
                            }
                            if (!actorsData.empty()) {
                                Effects::SwapActor(ctxCopy, actorsData);
                            }
                        }
                        break;
                        
                        case EffectType::kSpawnLeveledItem:
                        {
                            std::vector<LvlItemSpawnData> lvlItemsData;
                            for (const auto& [form, extData] : effCopy.items) {
                                if (!form) continue;
                                float roll = std::uniform_real_distribution<float>(0.f, 100.f)(rng);
                                if (roll > extData.chance) continue;

                                if (extData.isFormList) {
                                    auto* list = form->As<RE::BGSListForm>();
                                    if (!list) continue;

                                    if (extData.index >= 0) {
                                        if (extData.index < static_cast<int>(list->forms.size())) {
                                            auto* el = list->forms[extData.index];
                                            if (el) {
                                                if (auto* lvli = el->As<RE::TESLevItem>())
                                                    lvlItemsData.emplace_back(lvli, extData.count, extData.spawnType, extData.fade, extData.scale);
                                            }
                                        }
                                    } else {
                                        for (auto* el : list->forms) {
                                            if (!el) continue;
                                            if (auto* lvli = el->As<RE::TESLevItem>())
                                                lvlItemsData.emplace_back(lvli, extData.count, extData.spawnType, extData.fade, extData.scale);
                                        }
                                    }
                                }
                                else if (auto* lvli = form->As<RE::TESLevItem>())
                                    lvlItemsData.emplace_back(lvli, extData.count, extData.spawnType, extData.fade, extData.scale);
                            }
                            if (!lvlItemsData.empty()) {
                                Effects::SpawnLeveledItem(ctxCopy, lvlItemsData);
                            }
                        }
                        break;

                        case EffectType::kSwapLeveledItem:
                        {
                            std::vector<LvlItemSpawnData> lvlItemsData;
                            for (const auto& [form, extData] : effCopy.items) {
                                if (!form) continue;
                                float roll = std::uniform_real_distribution<float>(0.f, 100.f)(rng);
                                if (roll > extData.chance) continue;

                                if (extData.isFormList) {
                                    auto* list = form->As<RE::BGSListForm>();
                                    if (!list) continue;

                                    if (extData.index >= 0) {
                                        if (extData.index < static_cast<int>(list->forms.size())) {
                                            auto* el = list->forms[extData.index];
                                            if (el) {
                                                if (auto* lvli = el->As<RE::TESLevItem>())
                                                    lvlItemsData.emplace_back(lvli, extData.count, extData.spawnType, extData.fade, extData.scale, extData.nonDeletable);
                                            }
                                        }
                                    } else {
                                        for (auto* el : list->forms) {
                                            if (!el) continue;
                                            if (auto* lvli = el->As<RE::TESLevItem>())
                                                lvlItemsData.emplace_back(lvli, extData.count, extData.spawnType, extData.fade, extData.scale, extData.nonDeletable);
                                        }
                                    }
                                }
                                else if (auto* lvli = form->As<RE::TESLevItem>())
                                    lvlItemsData.emplace_back(lvli, extData.count, extData.spawnType, extData.fade, extData.scale, extData.nonDeletable);
                            }
                            if (!lvlItemsData.empty()) {
                                Effects::SwapLeveledItem(ctxCopy, lvlItemsData);
                            }
                        }
                        break;

                        case EffectType::kSpawnLeveledSpell:
                        {
                            std::vector<LvlSpellSpawnData> lvlSpellsData;
                            for (const auto& [form, extData] : effCopy.items) {
                                if (!form) continue;
                                float roll = std::uniform_real_distribution<float>(0.f, 100.f)(rng);
                                if (roll > extData.chance) continue;

                                if (extData.isFormList) {
                                    auto* list = form->As<RE::BGSListForm>();
                                    if (!list) continue;

                                    if (extData.index >= 0) {
                                        if (extData.index < static_cast<int>(list->forms.size())) {
                                            auto* el = list->forms[extData.index];
                                            if (el) {
                                                if (auto* lvls = el->As<RE::TESLevSpell>())
                                                    lvlSpellsData.emplace_back(lvls, extData.count, extData.radius);
                                            }
                                        }
                                    } else {
                                        for (auto* el : list->forms) {
                                            if (!el) continue;
                                            if (auto* lvls = el->As<RE::TESLevSpell>())
                                                lvlSpellsData.emplace_back(lvls, extData.count, extData.radius);
                                        }
                                    }
                                }
                                else if (auto* lvls = form->As<RE::TESLevSpell>())
                                    lvlSpellsData.emplace_back(lvls, extData.count, extData.radius);
                            }
                            if (!lvlSpellsData.empty()) {
                                Effects::SpawnLeveledSpell(ctxCopy, lvlSpellsData);
                            }
                        }
                        break;

                        case EffectType::kSpawnLeveledSpellOnItem:
                        {
                            std::vector<LvlSpellSpawnData> lvlSpellsData;
                            for (const auto& [form, extData] : effCopy.items) {
                                if (!form) continue;
                                float roll = std::uniform_real_distribution<float>(0.f, 100.f)(rng);
                                if (roll > extData.chance) continue;

                                if (extData.isFormList) {
                                    auto* list = form->As<RE::BGSListForm>();
                                    if (!list) continue;

                                    if (extData.index >= 0) {
                                        if (extData.index < static_cast<int>(list->forms.size())) {
                                            auto* el = list->forms[extData.index];
                                            if (el) {
                                                if (auto* lvls = el->As<RE::TESLevSpell>())
                                                    lvlSpellsData.emplace_back(lvls, extData.count);
                                            }
                                        }
                                    } else {
                                        for (auto* el : list->forms) {
                                            if (!el) continue;
                                            if (auto* lvls = el->As<RE::TESLevSpell>())
                                                lvlSpellsData.emplace_back(lvls, extData.count);
                                        }
                                    }
                                }
                                else if (auto* lvls = form->As<RE::TESLevSpell>())
                                    lvlSpellsData.emplace_back(lvls, extData.count);
                            }
                            if (!lvlSpellsData.empty()) {
                                Effects::SpawnLeveledSpellOnItem(ctxCopy, lvlSpellsData);
                            }
                        }
                        break;

                        case EffectType::kSpawnLeveledActor:
                        {
                            std::vector<LvlActorSpawnData> lvlActorsData;
                            for (const auto& [form, extData] : effCopy.items) {
                                if (!form) continue;
                                float roll = std::uniform_real_distribution<float>(0.f, 100.f)(rng);
                                if (roll > extData.chance) continue;

                                if (extData.isFormList) {
                                    auto* list = form->As<RE::BGSListForm>();
                                    if (!list) continue;

                                    if (extData.index >= 0) {
                                        if (extData.index < static_cast<int>(list->forms.size())) {
                                            auto* el = list->forms[extData.index];
                                            if (el) {
                                                if (auto* lvlc = el->As<RE::TESLevCharacter>())
                                                    lvlActorsData.emplace_back(lvlc, extData.count, extData.spawnType, extData.fade, extData.scale);
                                            }
                                        }
                                    } else {
                                        for (auto* el : list->forms) {
                                            if (!el) continue;
                                            if (auto* lvlc = el->As<RE::TESLevCharacter>())
                                                lvlActorsData.emplace_back(lvlc, extData.count, extData.spawnType, extData.fade, extData.scale);
                                        }
                                    }
                                }
                                else if (auto* lvlc = form->As<RE::TESLevCharacter>()) {
                                    lvlActorsData.emplace_back(lvlc, extData.count, extData.spawnType, extData.fade, extData.scale);
                                }
                            }
                            if (!lvlActorsData.empty()) {
                                Effects::SpawnLeveledActor(ctxCopy, lvlActorsData);
                            }
                        }
                        break;

                        case EffectType::kSwapLeveledActor:
                        {
                            std::vector<LvlActorSpawnData> lvlActorsData;
                            for (const auto& [form, extData] : effCopy.items) {
                                if (!form) continue;
                                float roll = std::uniform_real_distribution<float>(0.f, 100.f)(rng);
                                if (roll > extData.chance) continue;

                                if (extData.isFormList) {
                                    auto* list = form->As<RE::BGSListForm>();
                                    if (!list) continue;

                                    if (extData.index >= 0) {
                                        if (extData.index < static_cast<int>(list->forms.size())) {
                                            auto* el = list->forms[extData.index];
                                            if (el) {
                                                if (auto* lvlc = el->As<RE::TESLevCharacter>())
                                                    lvlActorsData.emplace_back(lvlc, extData.count, extData.spawnType, extData.fade, extData.scale, extData.nonDeletable);
                                            }
                                        }
                                    } else {
                                        for (auto* el : list->forms) {
                                            if (!el) continue;
                                            if (auto* lvlc = el->As<RE::TESLevCharacter>())
                                                lvlActorsData.emplace_back(lvlc, extData.count, extData.spawnType, extData.fade, extData.scale, extData.nonDeletable);
                                        }
                                    }
                                }
                                else if (auto* lvlc = form->As<RE::TESLevCharacter>())
                                    lvlActorsData.emplace_back(lvlc, extData.count, extData.spawnType, extData.fade, extData.scale, extData.nonDeletable);
                            }
                            if (!lvlActorsData.empty()) {
                                Effects::SwapLeveledActor(ctxCopy, lvlActorsData);
                            }
                        }
                        break;

                        case EffectType::kApplyIngestible:
                        {
                            std::vector<IngestibleApplyData> ingestibleData;
                            for (const auto& [form, extData] : effCopy.items) {                                
                                float roll = std::uniform_real_distribution<float>(0.f, 100.f)(rng);
                                if (roll > extData.chance) continue;

                                IngestibleApplyData data;
                                data.ingestible = nullptr;      // not used, a placeholder
                                data.count = 0;                 // not used, a placeholder
                                data.radius = extData.radius;
                                data.chance = extData.chance;
                                ingestibleData.emplace_back(std::move(data));
                            }
                            if (!ingestibleData.empty()) {
                                Effects::ApplyIngestible(ctxCopy, ingestibleData);
                            }
                        }
                        break;

                        case EffectType::kApplyOtherIngestible:
                        {
                            std::vector<IngestibleApplyData> ingestibleData;
                            for (const auto& [form, extData] : effCopy.items) {
                                if (!form) continue;
                                float roll = std::uniform_real_distribution<float>(0.f, 100.f)(rng);
                                if (roll > extData.chance) continue;

                                if (extData.isFormList) {
                                    auto* list = form->As<RE::BGSListForm>();
                                    if (!list) continue;

                                    int idx = extData.index;
                                    bool spawnAll = false;

                                    if (idx == -3) {
                                        idx = std::uniform_int_distribution<int>(0, static_cast<int>(list->forms.size()) - 1)(rng);
                                    }
                                    else if (idx == -2) {
                                        idx = ruleCopy.dynamicIndex;
                                    }
                                    else if (idx == -1) {
                                        spawnAll = true;
                                    }
                                    if (spawnAll) {
                                        for (auto* el : list->forms) {
                                            if (!el) continue;
                                            if (auto* ingestible = el->As<RE::MagicItem>()) ingestibleData.emplace_back(ingestible, extData.count, extData.radius);
                                        }
                                    }
                                    else if (idx >= 0 && idx < static_cast<int>(list->forms.size())) {
                                        auto* el = list->forms[idx];
                                        if (el) {
                                            if (auto* ingestible = el->As<RE::MagicItem>()) ingestibleData.emplace_back(ingestible, extData.count, extData.radius);
                                        }
                                    }
                                }
                                else if (auto* ingestible = form->As<RE::MagicItem>()) {
                                    ingestibleData.emplace_back(ingestible, extData.count, extData.radius);
                                }
                            }
                            if (!ingestibleData.empty()) {
                                Effects::ApplyOtherIngestible(ctxCopy, ingestibleData);
                            }
                        }
                        break;

                        case EffectType::kSpawnLight:
                        {
                            std::vector<LightSpawnData> lightsData;
                            for (const auto& [form, extData] : effCopy.items) {
                                if (!form) continue;
                                float roll = std::uniform_real_distribution<float>(0.f, 100.f)(rng);
                                if (roll > extData.chance) continue;

                                if (extData.isFormList) {
                                    auto* list = form->As<RE::BGSListForm>();
                                    if (!list) continue;

                                    int idx = extData.index;
                                    bool spawnAll = false;

                                    if (idx == -3) {
                                        idx = std::uniform_int_distribution<int>(0, static_cast<int>(list->forms.size()) - 1)(rng);
                                    }
                                    else if (idx == -2) {
                                        idx = ruleCopy.dynamicIndex;
                                    }
                                    else if (idx == -1) {
                                        spawnAll = true;
                                    }
                                    if (spawnAll) {
                                        for (auto* el : list->forms) {
                                            if (!el) continue;
                                            if (auto* light = el->As<RE::TESObjectLIGH>()) lightsData.emplace_back(light, extData.count, extData.spawnType, extData.fade, extData.scale);
                                        }
                                    }
                                    else if (idx >= 0 && idx < static_cast<int>(list->forms.size())) {
                                        auto* el = list->forms[idx];
                                        if (el) {
                                            if (auto* light = el->As<RE::TESObjectLIGH>()) lightsData.emplace_back(light, extData.count, extData.spawnType, extData.fade, extData.scale);
                                        }
                                    }
                                }
                                else if (auto* light = form->As<RE::TESObjectLIGH>()) {
                                    lightsData.emplace_back(light, extData.count, extData.spawnType, extData.fade, extData.scale);
                                }
                            }
                            if (!lightsData.empty()) {
                                Effects::SpawnLight(ctxCopy, lightsData);
                            }
                        }
                        break;

                        case EffectType::kRemoveLight:
                        {
                            std::vector<LightRemoveData> lightsData;
                            for (const auto& [form, extData] : effCopy.items) {
                                float roll = std::uniform_real_distribution<float>(0.f, 100.f)(rng);
                                if (roll > extData.chance) continue;
                        
                                if (extData.isFormList) {
                                    auto* list = form ? form->As<RE::BGSListForm>() : nullptr;
                                    if (!list) continue;
                        
                                    int idx = extData.index;
                                    bool spawnAll = false;

                                    if (idx == -3) {
                                        idx = std::uniform_int_distribution<int>(0, static_cast<int>(list->forms.size()) - 1)(rng);
                                    }
                                    else if (idx == -2) {
                                        idx = ruleCopy.dynamicIndex;
                                    }
                                    else if (idx == -1) {
                                        spawnAll = true;
                                    }
                                    if (spawnAll) {
                                        for (auto* el : list->forms) {
                                            if (!el) continue;
                                            if (auto* light = el->As<RE::TESObjectLIGH>()) lightsData.emplace_back(light, extData.radius);
                                        }
                                    }
                                    else if (idx >= 0 && idx < static_cast<int>(list->forms.size())) {
                                        auto* el = list->forms[idx];
                                        if (el) {
                                            if (auto* light = el->As<RE::TESObjectLIGH>()) lightsData.emplace_back(light, extData.radius);
                                        }
                                    }
                                }
                                else if (form && form->As<RE::TESObjectLIGH>()) {
                                    auto* light = form->As<RE::TESObjectLIGH>();
                                    lightsData.emplace_back(light, extData.radius);
                                }
                            }
                            if (!lightsData.empty()) {
                                Effects::RemoveLight(ctxCopy, lightsData);
                            }
                        }
                        break;

                        case EffectType::kEnableLight:
                        {
                            std::vector<LightRemoveData> lightsData;
                            for (const auto& [form, extData] : effCopy.items) {
                                float roll = std::uniform_real_distribution<float>(0.f, 100.f)(rng);
                                if (roll > extData.chance) continue;

                                if (extData.isFormList) {
                                    auto* list = form ? form->As<RE::BGSListForm>() : nullptr;
                                    if (!list) continue;

                                    int idx = extData.index;
                                    bool spawnAll = false;

                                    if (idx == -3) {
                                        idx = std::uniform_int_distribution<int>(0, static_cast<int>(list->forms.size()) - 1)(rng);
                                    }
                                    else if (idx == -2) {
                                        idx = ruleCopy.dynamicIndex;
                                    }
                                    else if (idx == -1) {
                                        spawnAll = true;
                                    }
                                    if (spawnAll) {
                                        for (auto* el : list->forms) {
                                            if (!el) continue;
                                            if (auto* light = el->As<RE::TESObjectLIGH>()) lightsData.emplace_back(light, extData.radius);
                                        }
                                    }
                                    else if (idx >= 0 && idx < static_cast<int>(list->forms.size())) {
                                        auto* el = list->forms[idx];
                                        if (el) {
                                            if (auto* light = el->As<RE::TESObjectLIGH>()) lightsData.emplace_back(light, extData.radius);
                                        }
                                    }
                                }
                                else if (form && form->As<RE::TESObjectLIGH>()) {
                                    auto* light = form->As<RE::TESObjectLIGH>();
                                    lightsData.emplace_back(light, extData.radius);
                                }
                            }
                            if (!lightsData.empty()) {
                                Effects::EnableLight(ctxCopy, lightsData);
                            }
                        }
                        break;

                        case EffectType::kDisableLight:
                        {
                            std::vector<LightRemoveData> lightsData;
                            for (const auto& [form, extData] : effCopy.items) {
                                float roll = std::uniform_real_distribution<float>(0.f, 100.f)(rng);
                                if (roll > extData.chance) continue;

                                if (extData.isFormList) {
                                    auto* list = form ? form->As<RE::BGSListForm>() : nullptr;
                                    if (!list) continue;

                                    int idx = extData.index;
                                    bool spawnAll = false;

                                    if (idx == -3) {
                                        idx = std::uniform_int_distribution<int>(0, static_cast<int>(list->forms.size()) - 1)(rng);
                                    }
                                    else if (idx == -2) {
                                        idx = ruleCopy.dynamicIndex;
                                    }
                                    else if (idx == -1) {
                                        spawnAll = true;
                                    }
                                    if (spawnAll) {
                                        for (auto* el : list->forms) {
                                            if (!el) continue;
                                            if (auto* light = el->As<RE::TESObjectLIGH>()) lightsData.emplace_back(light, extData.radius, light->GetFormID(), extData.chance);
                                        }
                                    }
                                    else if (idx >= 0 && idx < static_cast<int>(list->forms.size())) {
                                        auto* el = list->forms[idx];
                                        if (el) {
                                            if (auto* light = el->As<RE::TESObjectLIGH>()) lightsData.emplace_back(light, extData.radius);
                                        }
                                    }
                                }
                                else if (form && form->As<RE::TESObjectLIGH>()) {
                                    auto* light = form->As<RE::TESObjectLIGH>();
                                    lightsData.emplace_back(light, extData.radius, light->GetFormID(), extData.chance);
                                }
                            }
                            if (!lightsData.empty()) {
                                Effects::DisableLight(ctxCopy, lightsData);
                            }
                        }
                        break;

                        case EffectType::kPlayIdle:
                        {
                            std::vector<PlayIdleData> idleData;
                            for (const auto& [form, extData] : effCopy.items) {
                                float roll = std::uniform_real_distribution<float>(0.f, 100.f)(rng);
                                if (roll > extData.chance) continue;
                        
                                if (!extData.string.empty()) {
                                    PlayIdleData data;
                                    data.string = extData.string;
                                    data.duration = extData.duration > 0.0f ? extData.duration : 1.0f;
                                    data.chance = extData.chance;
                                    idleData.emplace_back(std::move(data));
                                }
                            }
                            if (!idleData.empty()) {
                                Effects::PlayIdle(ctxCopy, idleData);
                            }
                        }
                        break;

                        case EffectType::kSpawnEffectShader:
                        {
                            std::vector<EffectShaderSpawnData> effectShadersData;
                            for (const auto& [form, extData] : effCopy.items) {
                                if (!form) continue;
                                float roll = std::uniform_real_distribution<float>(0.f, 100.f)(rng);
                                if (roll > extData.chance) continue;

                                if (extData.isFormList) {
                                    auto* list = form->As<RE::BGSListForm>();
                                    if (!list) continue;

                                    int idx = extData.index;
                                    bool spawnAll = false;

                                    if (idx == -3) {
                                        idx = std::uniform_int_distribution<int>(0, static_cast<int>(list->forms.size()) - 1)(rng);
                                    }
                                    else if (idx == -2) {
                                        idx = ruleCopy.dynamicIndex;
                                    }
                                    else if (idx == -1) {
                                        spawnAll = true;
                                    }
                                    if (spawnAll) {
                                        for (auto* el : list->forms) {
                                            if (!el) continue;
                                            if (auto* shader = el->As<RE::TESEffectShader>()) effectShadersData.emplace_back(shader, extData.count, extData.radius, extData.duration);
                                        }
                                    }
                                    else if (idx >= 0 && idx < static_cast<int>(list->forms.size())) {
                                        auto* el = list->forms[idx];
                                        if (el) {
                                            if (auto* shader = el->As<RE::TESEffectShader>()) effectShadersData.emplace_back(shader, extData.count, extData.radius, extData.duration);
                                        }
                                    }
                                }
                                else if (auto* shader = form->As<RE::TESEffectShader>()) {
                                    effectShadersData.emplace_back(shader, extData.count, extData.radius, extData.duration);
                                }
                            }
                            if (!effectShadersData.empty()) {
                                Effects::SpawnEffectShader(ctxCopy, effectShadersData);
                            }
                        }
                        break;

                        case EffectType::kSpawnEffectShaderOnItem:
                        {
                            std::vector<EffectShaderSpawnData> effectShadersData;
                            for (const auto& [form, extData] : effCopy.items) {
                                if (!form) continue;
                                float roll = std::uniform_real_distribution<float>(0.f, 100.f)(rng);
                                if (roll > extData.chance) continue;

                                if (extData.isFormList) {
                                    auto* list = form->As<RE::BGSListForm>();
                                    if (!list) continue;

                                    int idx = extData.index;
                                    bool spawnAll = false;

                                    if (idx == -3) {
                                        idx = std::uniform_int_distribution<int>(0, static_cast<int>(list->forms.size()) - 1)(rng);
                                    }
                                    else if (idx == -2) {
                                        idx = ruleCopy.dynamicIndex;
                                    }
                                    else if (idx == -1) {
                                        spawnAll = true;
                                    }
                                    if (spawnAll) {
                                        for (auto* el : list->forms) {
                                            if (!el) continue;
                                            if (auto* shader = el->As<RE::TESEffectShader>()) effectShadersData.emplace_back(shader, extData.count, extData.duration);
                                        }
                                    }
                                    else if (idx >= 0 && idx < static_cast<int>(list->forms.size())) {
                                        auto* el = list->forms[idx];
                                        if (el) {
                                            if (auto* shader = el->As<RE::TESEffectShader>()) effectShadersData.emplace_back(shader, extData.count, extData.duration);
                                        }
                                    }
                                }
                                else if (auto* shader = form->As<RE::TESEffectShader>()) {
                                    effectShadersData.emplace_back(shader, extData.count, extData.duration);
                                }
                            }
                            if (!effectShadersData.empty()) {
                                Effects::SpawnEffectShaderOnItem(ctxCopy, effectShadersData);
                            }
                        }
                        break;
                        
                    default:
                        logger::warn("Unknown effect type {}", static_cast<int>(effCopy.type));
                    }
                } catch (const std::exception& e) {
                    logger::error("Exception in effect task: {}", e.what());
                } catch (...) {
                    logger::error("Unknown exception in effect task");
                }
        });
    }

    // ------------------ Trigger ------------------
    void RuleManager::Trigger(const RuleContext& ctx)
    {
        std::unique_lock lock(_ruleMutex);

        if (!ctx.target || ctx.target->IsDeleted() || !ctx.source || ctx.source->IsDeleted()) return;

        // Collect all effects for batching
        static std::vector<std::tuple<Effect, RuleContext, Rule>> pendingEffects;
        static std::mutex pendingMutex;

        // Walk through every rule and apply those whose filters match
        for (std::size_t ruleIdx = 0; ruleIdx < _rules.size(); ++ruleIdx) {
            Rule& r = _rules[ruleIdx];

            // Event type must be listed in the rule
            if (std::find(r.events.begin(), r.events.end(), ctx.event) == r.events.end()) continue;

            r.dynamicIndex = 0;

            if (!MatchFilter(r.filter, ctx, r)) continue;

            // 1. LIMIT CHECK BLOCK (important data which will be partially saved)
            bool limitCheckPassed = true;
            if (r.filter.limit > 0) {
                Key limitKey{
                    static_cast<std::uint32_t>(ctx.source->GetFormID()),
                    static_cast<std::uint32_t>(ctx.target->GetFormID()),
                    static_cast<std::uint16_t>(ruleIdx)
                };

                std::uint32_t& limitCnt = _limitCounts[limitKey];
                if (limitCnt >= r.filter.limit) {
                    limitCheckPassed = false;
                } else {
                    ++limitCnt;
                }
            }

            if (!limitCheckPassed) continue;

            // 2. INTERACTIONS CHECK BLOCK (temporary data - can be reset)
            bool interactionCheckPassed = true;
            if (r.filter.interactions > 1) {
                Key interactionKey{
                    static_cast<std::uint32_t>(ctx.source->GetFormID()),
                    static_cast<std::uint32_t>(ctx.target->GetFormID()),
                    static_cast<std::uint16_t>(ruleIdx)
                };

                std::uint32_t& interactionsCnt = _interactionsCounts[interactionKey];
                if (++interactionsCnt < r.filter.interactions) {
                    interactionCheckPassed = false;
                } else {
                    interactionsCnt = 0;
                }
            }

            if (!interactionCheckPassed) continue;

            // Apply every effect bound to this rule
            for (const auto& eff : r.effects) {
                ApplyEffect(eff, ctx, r);
            }
        }

        // Prevent unbounded growth of interaction counter map
        if (_interactionsCounts.size() > 1000) {
            logger::debug("Clearing interactions cache: {} entries", _interactionsCounts.size());
            _interactionsCounts.clear();
        }

        if (_limitCounts.size() > 1000) {
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
        if (_interactionsCounts.size() > 5000) {
            logger::warn("Emergency interactions cleanup!");
            _interactionsCounts.clear();
        }
    
        if (_limitCounts.size() > 5000) {
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
}