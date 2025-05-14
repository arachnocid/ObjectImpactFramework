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

        if (idStr.size() > 2 && idStr.substr(0, 2) == "0x") {
            idStr = idStr.substr(2);
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
                    r.filter.chance = jf["chance"].get<float>();
                } 

                if (jf.contains("interactions") && jf["interactions"].is_number_unsigned()) {
                    r.filter.interactions = jf["interactions"].get<std::uint32_t>();
                }

                if (jf.contains("limit") && jf["limit"].is_number_unsigned()) {
                    r.filter.limit = jf["limit"].get<std::uint32_t>();
                }

                if (jf.contains("formtypes") && jf["formtypes"].is_array()) {
                    for (auto const& ft : jf["formtypes"]) {
                        if (ft.is_string())
                            r.filter.formTypes.insert(MapStringToFormType(tolower_str(ft.get<std::string>())));
                    }
                    if (!r.filter.formTypes.empty()) {
                        hasObjectIdentifier = true;
                    }
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

                if (jf.contains("weaponstypes") && jf["weaponstypes"].is_array()) {
                    for (auto const& wt : jf["weaponstypes"]) {
                        if (wt.is_string()) {
                            r.filter.weaponTypes.insert(MapWeaponTypeToString(tolower_str(wt.get<std::string>())));
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

                if (jf.contains("weapons") && jf["weapons"].is_array()) {
                    for (auto const& wf : jf["weapons"]) {
                        if (wf.is_string()) {
                            if (auto* weapon = GetFormFromIdentifier<RE::TESObjectWEAP>(wf.get<std::string>())) {
                                r.filter.weapons.insert(weapon);
                            } else {
                                logger::warn("Invalid weapon formID '{}' in filter of {}", wf.get<std::string>(), path.string());
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

                if (jf.contains("attacks") && jf["attacks"].is_array()) {
                    for (auto const& at : jf["attacks"]) {
                        if (at.is_string()) {
                            r.filter.attackTypes.insert(MapAttackTypeToString(tolower_str(at.get<std::string>())));
                        } else {
                            logger::warn("Invalid attack type '{}' in filter of {}", at.get<std::string>(), path.string());
                        }
                    }
                }
            }

            if (!hasObjectIdentifier) {
                logger::warn("Skipping rule in {}: no valid object identifiers (formTypes, formIDs, or keywords)", path.string());
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
                {"removeitem", EffectType::kDisposeItem},
                {"spawnitem", EffectType::kSpawnItem},
                {"spawnspell", EffectType::kSpawnSpell},
                {"spawnspellonitem", EffectType::kSpawnSpellOnItem},
                {"spawnactor", EffectType::kSpawnActor},
                {"spawnimpact", EffectType::kSpawnImpact},
                {"spawnexplosion", EffectType::kSpawnExplosion},
                {"swapitem", EffectType::kSwapItem},
                {"playsound", EffectType::kPlaySound},
                {"spillinventory", EffectType::kSpillInventory},
                {"swapactor", EffectType::kSwapActor}
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

                if (eff.type != EffectType::kDisposeItem && eff.type != EffectType::kSpillInventory) {
                    if (effj.contains("items") && effj["items"].is_array()) {
                        for (const auto& itemJson : effj["items"]) {
                            if (!itemJson.is_object() || !itemJson.contains("formid") || !itemJson["formid"].is_string()) continue;
                            if (auto* form = GetFormFromIdentifier<RE::TESForm>(itemJson["formid"].get<std::string>())) {
                                EffectExtendedData extData;
                                extData.form = form;
                                extData.count = itemJson.value("count", 1U);
                                extData.chance = itemJson.value("chance", 100.0f);
                                eff.items.emplace_back(form, extData);
                            }
                        }
                    }
                    if (eff.items.empty()) continue;
                }

                r.effects.push_back(eff);
            }

            if (r.effects.empty()) {
                logger::warn("Skipping rule in {}: no valid effects parsed", path.string());
                continue;
            }

            _rules.push_back(std::move(r));
        }
    }

    // ------------------ Match ------------------
    bool RuleManager::MatchFilter(const Filter& f, const RuleContext& ctx) const {

        if (!ctx.baseObj) return false;

        if (!f.formTypes.empty() && !f.formTypes.contains(ctx.baseObj->GetFormType())) return false;
        if (!f.formIDs.empty() && !f.formIDs.contains(ctx.baseObj->GetFormID())) return false;

        if (!f.keywords.empty()) {
            bool skipKeywordCheck = (ctx.baseObj->GetFormType() == RE::FormType::Container ||
                                     ctx.baseObj->GetFormType() == RE::FormType::Static ||
                                     ctx.baseObj->GetFormType() == RE::FormType::MovableStatic ||
                                     ctx.baseObj->GetFormType() == RE::FormType::Tree ||
                                     ctx.baseObj->GetFormType() == RE::FormType::Door);

            if (!skipKeywordCheck) {
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
        }

        if (ctx.isHitEvent) {
            if (!f.weaponTypes.empty() && f.weaponTypes.find(ctx.weaponType) == f.weaponTypes.end()) return false;
            if (!f.weapons.empty() && (!ctx.weapon || f.weapons.find(ctx.weapon) == f.weapons.end())) return false;
            if (!f.projectiles.empty() && (!ctx.projectile || f.projectiles.find(ctx.projectile) == f.projectiles.end())) return false;
            if (!f.attackTypes.empty() && f.attackTypes.find(ctx.attackType) == f.attackTypes.end()) return false;
            if (!f.weaponsKeywords.empty()) {
                if (!ctx.weapon) return false; 

                auto* kwf = ctx.weapon->As<RE::BGSKeywordForm>();
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
        }

        return true;
    }

    // ------------------ Apply ------------------
    void RuleManager::ApplyEffect(const Effect& eff, const RuleContext& ctx) const {
        if (!ctx.target || !ctx.target->GetBaseObject()) return;
        if (!ctx.source || !ctx.source->GetBaseObject()) return;

        RE::FormID targetFormID = ctx.target->GetFormID();
        RE::FormID sourceFormID = ctx.source->GetFormID();
        Effect effCopy = eff;
        EventType eventType = ctx.event;

        SKSE::GetTaskInterface()->AddTask([effCopy, targetFormID, sourceFormID, eventType]() {
            auto* target = RE::TESForm::LookupByID<RE::TESObjectREFR>(targetFormID);
            auto* source = sourceFormID ? RE::TESForm::LookupByID<RE::Actor>(sourceFormID) : nullptr;
            
            if (!target || !target->GetBaseObject() || target->IsDisabled() || target->IsDeleted()) {
                logger::warn("Target {} is invalid or deleted", targetFormID);
                return;
            }

            if (!source || !source->GetBaseObject() || source->IsDisabled() || source->IsDeleted() || source->IsDead()) {
                logger::warn("Source {} is invalid, dead or deleted", sourceFormID);
                return;
            }

            RuleContext newCtx{eventType, source, target, target->GetBaseObject()};

            static thread_local std::mt19937 rng(std::random_device{}());
            float globalRoll = std::uniform_real_distribution<float>(0.f, 100.f)(rng);
            if (globalRoll > effCopy.chance) return;

            try {
                switch (effCopy.type) {
                    case EffectType::kDisposeItem:
                        Effects::DisposeItem(newCtx);
                        break;

                    case EffectType::kSpillInventory:
                        Effects::SpillInventory(newCtx);
                        break;

                    case EffectType::kSpawnItem:
                    {
                        std::vector<ItemSpawnData> itemsData;
                        for (const auto& [form, extData] : effCopy.items) {
                            if (!form) continue;
                            float roll = std::uniform_real_distribution<float>(0.f, 100.f)(rng);
                            if (roll > extData.chance) continue;
                            if (auto* item = form->As<RE::TESBoundObject>()) {
                                itemsData.emplace_back(item, extData.count);
                            }
                        }
                        if (!itemsData.empty()) {
                            Effects::SpawnItem(newCtx, itemsData);
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
                            if (auto* spell = form->As<RE::SpellItem>()) {
                                spellsData.emplace_back(spell, extData.count);
                            }
                        }
                        if (!spellsData.empty()) {
                            Effects::SpawnSpell(newCtx, spellsData);
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
                            if (auto* spell = form->As<RE::SpellItem>()) {
                                spellsData.emplace_back(spell, extData.count);
                            }
                        }
                        if (!spellsData.empty()) {
                            Effects::SpawnSpellOnItem(newCtx, spellsData);
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
                            if (auto* actor = form->As<RE::TESNPC>()) {
                                actorsData.emplace_back(actor, extData.count);
                            }
                        }
                        if (!actorsData.empty()) {
                            Effects::SpawnActor(newCtx, actorsData);
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
                            if (auto* impact = form->As<RE::BGSImpactDataSet>()) {
                                impactsData.emplace_back(impact, extData.count);
                            }
                        }
                        if (!impactsData.empty()) {
                            Effects::SpawnImpact(newCtx, impactsData);
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
                            if (auto* explosion = form->As<RE::BGSExplosion>()) {
                                explosionsData.emplace_back(explosion, extData.count);
                            }
                        }
                        if (!explosionsData.empty()) {
                            Effects::SpawnExplosion(newCtx, explosionsData);
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
                            if (auto* item = form->As<RE::TESBoundObject>()) {
                                itemsData.emplace_back(item, extData.count);
                            }
                        }
                        if (!itemsData.empty()) {
                            Effects::SwapItem(newCtx, itemsData);
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
                            if (auto* sound = form->As<RE::BGSSoundDescriptorForm>()) {
                                soundsData.emplace_back(sound, extData.count);
                            }
                        }
                        if (!soundsData.empty()) {
                            Effects::PlaySound(newCtx, soundsData);
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
                            if (auto* actor = form->As<RE::TESNPC>()) {
                                actorsData.emplace_back(actor, extData.count);
                            }
                        }
                        if (!actorsData.empty()) {
                            Effects::SwapActor(newCtx, actorsData);
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

        struct ProcessedCleaner {
            ~ProcessedCleaner() {
                OIF::Effects::ClearProcessedItems();
            }
        } _cleaner;

        if (!ctx.target || 
            ctx.target->IsDisabled() || 
            ctx.target->IsDeleted() || 
            !ctx.source || 
            ctx.source->IsDisabled() || 
            ctx.source->IsDeleted() || 
            ctx.source->IsDead())
            return;

        auto formType = ctx.target->GetBaseObject()->GetFormType();
        bool supported = false;
        for (const auto& [typeStr, type] : formTypeMap) {
            if (type == formType) {
                supported = true;
                break;
            }
        }
        if (!supported) {
            return;
        }

        if (OIF::Effects::IsItemProcessed(ctx.target)) return;
        OIF::Effects::MarkItemAsProcessed(ctx.target);

        // Unique key for interaction counters
        const std::uint64_t eventKey =
            (static_cast<std::uint64_t>(ctx.source->GetFormID()) << 40) |
            (static_cast<std::uint64_t>(ctx.target->GetFormID()) << 8) |
            static_cast<std::uint8_t>(ctx.event);

        // Walk through every rule and apply those whose filters match
        for (std::size_t ruleIdx = 0; ruleIdx < _rules.size(); ++ruleIdx) {
            const Rule& r = _rules[ruleIdx];

            // Event type must be listed in the rule
            if (std::find(r.events.begin(), r.events.end(), ctx.event) == r.events.end())
                continue;

            // Custom filter check
            if (!MatchFilter(r.filter, ctx))
                continue;

            // Unique key for interaction counters
            std::uint64_t baseKey = (eventKey << 16) | ruleIdx;
            std::uint64_t limitKey = baseKey | 0x1;
            std::uint64_t interactionsKey = baseKey | 0x2;

            // Limit of interactions (if limit is 0, it means no limit)
            if (r.filter.limit > 0) {
                std::uint32_t& limitCnt = _filterInteractionCounts[limitKey];
                if (limitCnt >= r.filter.limit)
                    continue;
                ++limitCnt;
            }

            // Interaction quota ("hit twice before effect")
            if (r.filter.interactions > 1) {
                std::uint32_t& interactionsCnt = _filterInteractionCounts[interactionsKey];
                if (++interactionsCnt < r.filter.interactions)
                    continue;
                interactionsCnt = 0;
            }

            // Apply every effect bound to this rule
            for (const auto& eff : r.effects) {
                ApplyEffect(eff, ctx);
            }
        }

        // Prevent unbounded growth of interaction counter map
        constexpr std::size_t MAX_INTERACTION_ENTRIES = 5000;
        if (_filterInteractionCounts.size() > MAX_INTERACTION_ENTRIES) {
            _filterInteractionCounts.clear();
        }
    }
}