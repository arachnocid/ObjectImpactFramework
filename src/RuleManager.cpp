#include "RuleManager.h"
#include "Effects.h"
#include <nlohmann/json.hpp>
#include <string>
#include <fstream>
#include <algorithm>
#include <ranges>
#include <shared_mutex>
#include <cctype>

namespace fs = std::filesystem;
using json = nlohmann::json;

namespace OIF {
    // ------------------ Helpers ------------------
    static RE::FormType MapStringToFormType(std::string_view s) {
        std::string str(s);
        std::ranges::transform(str, str.begin(), [](unsigned char c) { return std::tolower(c); });
        if (str == "activator")         return RE::FormType::Activator;
        if (str == "talkingactivator")  return RE::FormType::TalkingActivator;
        if (str == "weapon")            return RE::FormType::Weapon;
        if (str == "armor")             return RE::FormType::Armor;
        if (str == "ammo")              return RE::FormType::Ammo;
        if (str == "ingredient")        return RE::FormType::Ingredient;
        if (str == "misc")              return RE::FormType::Misc;
        if (str == "book")              return RE::FormType::Book;
        if (str == "scroll")            return RE::FormType::Scroll;
        if (str == "soulgem")           return RE::FormType::SoulGem;
        if (str == "potion")            return RE::FormType::AlchemyItem;
        if (str == "furniture")         return RE::FormType::Furniture;
        if (str == "flora")             return RE::FormType::Flora;
        if (str == "container")         return RE::FormType::Container;
        if (str == "static")            return RE::FormType::Static;
        if (str == "moveablestatic")    return RE::FormType::MovableStatic;
        if (str == "tree")              return RE::FormType::Tree;
        return RE::FormType::None;
    }

    static std::string MapAttackTypeToString(std::string_view s) {
        std::string str(s);
        std::ranges::transform(str, str.begin(), [](unsigned char c) { return std::tolower(c); });
        if (str == "regular")       return "Regular";
        if (str == "power")         return "Power";
        if (str == "bash")          return "Bash";
        if (str == "projectile")    return "Projectile";
        return "Regular";
    }
    
    static std::string MapWeaponTypeToString(std::string_view s) {
        std::string str(s);
        std::ranges::transform(str, str.begin(), [](unsigned char c) { return std::tolower(c); });
        if (str == "onehandsword")  return "OneHandSword";
        if (str == "twohandsword")  return "TwoHandSword";
        if (str == "onehandaxe")    return "OneHandAxe";
        if (str == "twohandaxe")    return "TwoHandAxe";
        if (str == "onehandmace")   return "OneHandMace";
        if (str == "twohandmace")   return "TwoHandMace";
        if (str == "dagger")        return "Dagger";
        if (str == "ranged")        return "Ranged";
        if (str == "staff")         return "Staff";
        if (str == "handtohand")    return "HandToHand";
        if (str == "spell")         return "Spell";
        return "Other";
    }

	template <class T>
    T* RuleManager::GetFormFromIdentifier(const std::string& identifier) {

        static std::mutex dataHandlerMutex;
        std::lock_guard<std::mutex> lock(dataHandlerMutex);

        auto pos = identifier.find(':');
        if (pos == std::string::npos) {
            logger::error("Invalid identifier format: '{}'", identifier);
            return nullptr;
        }
        std::string modName = identifier.substr(0, pos);
        std::string idStr = identifier.substr(pos + 1);
        logger::info("Parsed modName: '{}', idStr: '{}'", modName, idStr);

        if (idStr.size() > 2 && idStr.substr(0, 2) == "0x") {
            idStr = idStr.substr(2);
        }

        std::uint32_t rawID = 0;
        try {
            rawID = std::stoul(idStr, nullptr, 16);
            logger::info("Converted rawID: 0x{:06X}", rawID);
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
        logger::info("Mod '{}' found, index: 0x{:02X}", modName, modInfo->compileIndex);

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

        logger::info("Form found: 0x{:08X}", form->GetFormID());
        return typedForm;
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
        for (auto const& entry : fs::recursive_directory_iterator{ dir }) {
            if (entry.is_regular_file() && entry.path().extension() == ".json") {
                ParseJSON(entry.path());
            }
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

        for (auto const& jr : j) {
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
                if (ev == "Hit")           r.events.push_back(EventType::kHit);
                else if (ev == "Activate") r.events.push_back(EventType::kActivate);
            }
            if (r.events.empty()) {
                logger::warn("Skipping rule in {}: no valid events", path.string());
                continue;
            }

            // -------- filter ----------
            if (jr.contains("filter") && jr["filter"].is_object()) {
                const auto& jf = jr["filter"];

                // formTypes
                if (jf.contains("formTypes") && jf["formTypes"].is_array()) {
                    for (auto const& ft : jf["formTypes"]) {
                        if (ft.is_string())
                            r.filter.formTypes.insert(MapStringToFormType(ft.get<std::string>()));
                    }
                }

                // formIDs
				if (jf.contains("formIDs") && jf["formIDs"].is_array()) {
					for (auto const& bid : jf["formIDs"]) {
						if (!bid.is_string())
							continue;
						std::string str = bid.get<std::string>();
						if (auto* form = GetFormFromIdentifier<RE::TESForm>(str)) {
							r.filter.formIDs.insert(form->GetFormID());
						} else {
							logger::warn("Invalid formID '{}' in filter of {}", bid.get<std::string>(), path.string());
						}
					}
				}

                // keywords
                if (jf.contains("keywords") && jf["keywords"].is_array()) {
                    for (auto const& kwStr : jf["keywords"]) {
                        if (!kwStr.is_string())
                            continue;
                        if (auto* kw = GetFormFromIdentifier<RE::BGSKeyword>(kwStr.get<std::string>())) {
                            r.filter.keywords.insert(kw);
                        } else {
                            logger::warn("Invalid keyword '{}' in filter of {}", kwStr.get<std::string>(), path.string());
                        }
                    }
                }

                 // WeaponsType filter
                if (jf.contains("weaponsTypes") && jf["weaponsTypes"].is_array()) {
                    for (auto const& wt : jf["weaponsTypes"]) {
                        if (wt.is_string()) {
                            r.filter.weaponTypes.insert(MapWeaponTypeToString(wt.get<std::string>()));
                        }
                    }
                }
                
                // Weapons filter (specific weapons)
                if (jf.contains("weapons") && jf["weapons"].is_array()) {
                    for (auto const& wf : jf["weapons"]) {
                        if (!wf.is_string())
                            continue;
                        if (auto* weapon = GetFormFromIdentifier<RE::TESObjectWEAP>(wf.get<std::string>())) {
                            r.filter.weapons.insert(weapon);
                        } else {
                            logger::warn("Invalid weapon formID '{}' in filter of {}", wf.get<std::string>(), path.string());
                        }
                    }
                }
                
                // Projectiles filter
                if (jf.contains("projectiles") && jf["projectiles"].is_array()) {
                    for (auto const& pf : jf["projectiles"]) {
                        if (!pf.is_string())
                            continue;
                        if (auto* proj = GetFormFromIdentifier<RE::BGSProjectile>(pf.get<std::string>())) {
                            r.filter.projectiles.insert(proj);
                        } else {
                            logger::warn("Invalid projectile formID '{}' in filter of {}", pf.get<std::string>(), path.string());
                        }
                    }
                }
                
                // Attacks filter
                if (jf.contains("attacks") && jf["attacks"].is_array()) {
                    for (auto const& at : jf["attacks"]) {
                        if (at.is_string()) {
                            r.filter.attackTypes.insert(MapAttackTypeToString(at.get<std::string>()));
                        }
                    }
                }
            }

            // -------- effect ----------
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

            for (const auto& effj : effectArray) {
                if (!effj.is_object()) {
                    logger::warn("Skipping invalid effect block in {}", path.string());
                    continue;
                }

                Effect eff;
                std::string typeStr = effj.value("type", "SpawnItem");

                // effect type
                if (typeStr == "RemoveItem")                eff.type = EffectType::kDisposeItem;
                else if (typeStr == "SpawnItem")            eff.type = EffectType::kSpawnItem;
                else if (typeStr == "SpawnMultipleItems")   eff.type = EffectType::kSpawnMultipleItems;
                else if (typeStr == "SpawnSpell")           eff.type = EffectType::kSpawnSpell;
                else if (typeStr == "SpawnActor")           eff.type = EffectType::kSpawnActor;
                else if (typeStr == "SpawnImpact")          eff.type = EffectType::kSpawnImpact;
                else if (typeStr == "SpawnExplosion")       eff.type = EffectType::kSpawnExplosion;
                else if (typeStr == "SwapItem")             eff.type = EffectType::kSwapItem;
                else if (typeStr == "SwapWithMultipleItems") eff.type = EffectType::kSwapWithMultipleItems;
                else if (typeStr == "PlaySound")            eff.type = EffectType::kPlaySound;
                else {
                    logger::warn("Skipping effect with unknown type '{}' in {}", typeStr, path.string());
                    continue;
                }

                // chance
                if (effj.contains("chance") && effj["chance"].is_number()) {
                    eff.chance = effj["chance"].get<float>();
                } else if (jr.is_object() && jr.contains("chance") && jr["chance"].is_number()) {
                    eff.chance = jr["chance"].get<float>();
                } else {
                    eff.chance = 100.0f;
                }

                // form
                bool needsForm = (eff.type != EffectType::kDisposeItem && eff.type != EffectType::kSpawnMultipleItems && eff.type != EffectType::kSwapWithMultipleItems);
                if (needsForm) {
                    if (effj.contains("formID") && effj["formID"].is_string()) {
                        if (auto* form = GetFormFromIdentifier<RE::TESForm>(effj["formID"].get<std::string>())) {
                            eff.form = form;
                        } else {
                            logger::warn("Skipping effect: invalid formID '{}' in {}", effj["formID"].get<std::string>(), path.string());
                            continue;
                        }
                    } else {
                        logger::warn("Skipping effect: missing 'formID' for type '{}' in {}", typeStr, path.string());
                        continue;
                    }
                }

                // count
                if (eff.type == EffectType::kSpawnItem || eff.type == EffectType::kDisposeItem) {
                    if (effj.contains("count") && effj["count"].is_number_unsigned()) {
                        eff.count = effj["count"].get<std::uint32_t>();
                    } else {
                        eff.count = 1;
                    }
                }

                // items for SpawnMultipleItems / SwapWithMultipleItems
                if (eff.type == EffectType::kSpawnMultipleItems || eff.type == EffectType::kSwapWithMultipleItems) {
                    if (effj.contains("items") && effj["items"].is_array()) {
                        for (const auto& itemJson : effj["items"]) {
                            if (!itemJson.is_object()) {
                                logger::warn("Invalid item entry in SpawnMultipleItems or SwapWithMultipleItems, expected object");
                                continue;
                            }
                            if (!itemJson.contains("formID") || !itemJson["formID"].is_string()) {
                                logger::warn("Missing or invalid formID in item entry");
                                continue;
                            }
                            std::string formIdStr = itemJson["formID"].get<std::string>();
                            RE::TESForm* form = GetFormFromIdentifier<RE::TESForm>(formIdStr);
                            if (!form) {
                                logger::warn("Invalid formID '{}' in SpawnMultipleItems or SwapWithMultipleItems effect", formIdStr);
                                continue;
                            }
                            std::uint32_t count = 1;
                            if (itemJson.contains("count") && itemJson["count"].is_number_unsigned()) {
                                count = itemJson["count"].get<std::uint32_t>();
                            }
                            eff.items.push_back({form, count});
                        }
                    } else {
                        logger::warn("Missing 'items' array for SpawnMultipleItems or SwapWithMultipleItems effect");
                        continue;
                    }
                }

                r.effects.push_back(eff);
            }

            if (r.effects.empty()) {
                logger::warn("Skipping rule in {}: no valid effects parsed", path.string());
                continue;
            }

            _rules.push_back(std::move(r));
            logger::info("Parsed rule with {} effects from {}", r.effects.size(), path.filename().string());
        }
    }

    // ------------------ Match ------------------
    bool RuleManager::MatchFilter(const Filter& f, const RuleContext& ctx) const {

        // formType
        if (!f.formTypes.empty()) {
            if (!ctx.baseObj || !f.formTypes.contains(ctx.baseObj->GetFormType()))
                return false;
        }

        // baseIDs
        if (!f.formIDs.empty()) {
            if (!ctx.baseObj || !f.formIDs.contains(ctx.baseObj->GetFormID()))
                return false;
        }

        // keywords
        if (!f.keywords.empty()) {
            bool has = false;
            
            bool skipKeywordCheck = false;
            if (ctx.baseObj) {
            auto formType = ctx.baseObj->GetFormType();
            if (formType == RE::FormType::Container ||
                formType == RE::FormType::Static ||
                formType == RE::FormType::MovableStatic ||
                formType == RE::FormType::Tree) {
                skipKeywordCheck = true;
                }
            }
            
            if (!skipKeywordCheck && ctx.baseObj) {
            if (auto* kwf = ctx.baseObj->As<RE::BGSKeywordForm>()) {
                for (auto* kw : f.keywords) {
                if (kwf->HasKeyword(kw)) { has = true; break; }
                }
            }
            } else if (skipKeywordCheck) {
            // Skip keyword checking for specified form types
            has = true;
            }
            
            if (!has)
            return false;
        }

        // Hit-specific filters
        if (ctx.isHitEvent) {

            // WeaponsType filter
            if (!f.weaponTypes.empty()) {
                if (f.weaponTypes.find(ctx.weaponType) == f.weaponTypes.end())
                    return false;
            }
            
            // Weapons filter
            if (!f.weapons.empty()) {
                if (!ctx.weapon || f.weapons.find(ctx.weapon) == f.weapons.end())
                    return false;
            }
            
            // Projectiles filter
            if (!f.projectiles.empty()) {
                if (!ctx.projectile || f.projectiles.find(ctx.projectile) == f.projectiles.end())
                    return false;
            }
            
            // Attack types filter
            if (!f.attackTypes.empty()) {
                if (f.attackTypes.find(ctx.attackType) == f.attackTypes.end())
                    return false;
            }
        }
        return true;
    }

    // ------------------ Apply ------------------
    void RuleManager::ApplyEffect(const Effect& eff, const RuleContext& ctx) const {

        if (!ctx.target || !ctx.target->GetBaseObject()) {
            logger::warn("Skipping effect: target or target's base object is null");
            return;
        }

        RE::FormID targetFormID = ctx.target ? ctx.target->GetFormID() : 0;
        RE::FormID sourceFormID = ctx.source ? ctx.source->GetFormID() : 0;
        EventType eventType = ctx.event;

        static thread_local std::mt19937 rng(std::random_device{}());
        std::uniform_real_distribution<float> dist(0.f, 100.f);
        if (dist(rng) > eff.chance)
            return;

        // Defensive copy of Effect and RuleContext for lambda (avoid capturing references to stack)
        Effect effCopy = eff;
        RuleContext ctxCopy = ctx;
        // Don't capture ctx by value, as it may contain stack pointers
        ctxCopy.baseObj = nullptr; // will be restored in lambda

        SKSE::GetTaskInterface()->AddTask([effCopy, targetFormID, sourceFormID, eventType]() {
            auto* target = targetFormID ? RE::TESForm::LookupByID<RE::TESObjectREFR>(targetFormID) : nullptr;
            auto* source = sourceFormID ? RE::TESForm::LookupByID<RE::Actor>(sourceFormID) : nullptr;

            if (!target || !target->GetBaseObject()) {
                logger::error("Invalid context in effect task (target or baseObj is null)");
                return;
            }

            RuleContext newCtx{
                eventType,
                source,
                target,
                target->GetBaseObject()
            };

            try {
                switch (effCopy.type) {
                case EffectType::kDisposeItem:
                    Effects::DisposeItem(newCtx);
                    break;

                case EffectType::kSpawnItem:
                    if (!effCopy.form) {
                        logger::error("No form provided for SpawnItem effect");
                        return;
                    }
                    if (auto* item = effCopy.form->As<RE::TESBoundObject>()) {
                        Effects::SpawnItem(newCtx, item, effCopy.count);
                    } else {
                        logger::error("FormID {:08X} is not TESBoundObject", effCopy.form->GetFormID());
                    }
                    break;

                case EffectType::kSpawnMultipleItems:
                    {
                        std::vector<ItemSpawnData> itemsData;
                        for (const auto& [form, count] : effCopy.items) {
                            if (!form) {
                                logger::error("Null form in SpawnMultipleItems effect");
                                continue;
                            }
                            if (auto* item = form->As<RE::TESBoundObject>()) {
                                itemsData.push_back({item, count});
                            } else {
                                logger::error("FormID {:08X} is not TESBoundObject", form->GetFormID());
                            }
                        }
                        if (!itemsData.empty()) {
                            Effects::SpawnMultipleItems(newCtx, itemsData);
                        } else {
                            logger::warn("No valid items to spawn in SpawnMultipleItems effect");
                        }
                    }
                    break;

                case EffectType::kSpawnSpell:
                    if (!effCopy.form) {
                        logger::error("No form provided for SpawnSpell effect");
                        return;
                    }
                    if (auto* spell = effCopy.form->As<RE::SpellItem>()) {
                        Effects::SpawnSpell(newCtx, spell);
                    } else {
                        logger::error("FormID {:08X} is not SpellItem", effCopy.form->GetFormID());
                    }
                    break;

                case EffectType::kSpawnActor:
                    if (!effCopy.form) {
                        logger::error("No form provided for SpawnActor effect");
                        return;
                    }
                    if (auto* npc = effCopy.form->As<RE::TESNPC>()) {
                        Effects::SpawnActor(newCtx, npc, effCopy.count);
                    } else {
                        logger::error("FormID {:08X} is not TESNPC", effCopy.form->GetFormID());
                    }
                    break;

                case EffectType::kSpawnImpact:
                    if (!effCopy.form) {
                        logger::error("No form provided for SpawnImpact effect");
                        return;
                    }
                    if (auto* impact = effCopy.form->As<RE::BGSImpactDataSet>()) {
                        Effects::SpawnImpact(newCtx, impact);
                    } else {
                        logger::error("FormID {:08X} is not BGSImpactDataSet", effCopy.form->GetFormID());
                    }
                    break;

                case EffectType::kSpawnExplosion:
                    if (!effCopy.form) {
                        logger::error("No form provided for SpawnExplosion effect");
                        return;
                    }
                    if (auto* explosion = effCopy.form->As<RE::BGSExplosion>()) {
                        Effects::SpawnExplosion(newCtx, explosion);
                    } else {
                        logger::error("FormID {:08X} is not BGSExplosion", effCopy.form->GetFormID());
                    }
                    break;

                case EffectType::kSwapItem:
                    if (!effCopy.form) {
                        logger::error("No form provided for SwapItem effect");
                        return;
                    }
                    if (auto* item = effCopy.form->As<RE::TESBoundObject>()) {
                        Effects::SwapItem(newCtx, item);
                    } else {
                        logger::error("FormID {:08X} is not TESBoundObject", effCopy.form->GetFormID());
                    }
                    break;

                case EffectType::kSwapWithMultipleItems:
                    {
                        std::vector<ItemSpawnData> itemsData;
                        for (const auto& [form, count] : effCopy.items) {
                            if (!form) {
                                logger::error("Null form in SwapWithMultipleItems effect");
                                continue;
                            }
                            if (auto* item = form->As<RE::TESBoundObject>()) {
                                itemsData.push_back({item, count});
                            } else {
                                logger::error("FormID {:08X} is not TESBoundObject", form->GetFormID());
                            }
                        }
                        if (!itemsData.empty()) {
                            Effects::SwapWithMultipleItems(newCtx, itemsData);
                        } else {
                            logger::warn("No valid items to swap in SwapWithMultipleItems effect");
                        }
                    }
                    break;

                case EffectType::kPlaySound:
                    if (!effCopy.form) {
                        logger::error("No form provided for PlaySound effect");
                        return;
                    }
                    if (auto* sound = effCopy.form->As<RE::BGSSoundDescriptorForm>()) {
                        Effects::PlaySound(newCtx, sound);
                    } else {
                        logger::error("FormID {:08X} is not BGSSoundDescriptorForm", effCopy.form->GetFormID());
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
	void RuleManager::Trigger(const RuleContext& ctx) {
        std::unique_lock lock(_ruleMutex);
    
        if (!ctx.baseObj) {
            logger::warn("Trigger called with null base object");
            return;
        }
    
        if (!ctx.target) {
            logger::warn("Trigger called with null target reference");
            return;
        }

        const auto formType = ctx.baseObj->GetFormType();

        std::vector<Rule> rulesCopy;
        {
            rulesCopy = _rules;  // Copy under mutex protection just in case
        }
        
        for (const auto& r : rulesCopy) {

            if (std::find(r.events.begin(), r.events.end(), ctx.event) == r.events.end())
                continue;
    
            bool eventTypeAllowed = false;
            
            if (ctx.event == EventType::kActivate) {
                eventTypeAllowed = (formType == RE::FormType::Activator || formType == RE::FormType::TalkingActivator);
            } 
            else if (ctx.event == EventType::kHit) {
                static const std::unordered_set<RE::FormType> hitAllowedTypes = {
                    RE::FormType::Weapon, RE::FormType::Armor,
                    RE::FormType::Ammo, RE::FormType::Ingredient,
                    RE::FormType::Misc, RE::FormType::Book,
                    RE::FormType::Scroll, RE::FormType::SoulGem,
                    RE::FormType::AlchemyItem, RE::FormType::Activator,
                    RE::FormType::Furniture, RE::FormType::Flora,
                    RE::FormType::Container, RE::FormType::Static,
                    RE::FormType::MovableStatic, RE::FormType::Tree,
                };
                eventTypeAllowed = hitAllowedTypes.contains(formType);
            }
            else {
                eventTypeAllowed = true;
            }
    
            if (!eventTypeAllowed)
                continue;

            if (!MatchFilter(r.filter, ctx))
                continue;

            for (const auto& effect : r.effects) {
                ApplyEffect(effect, ctx);
            }
        }
    }
}