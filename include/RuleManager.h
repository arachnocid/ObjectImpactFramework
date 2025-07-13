#pragma once
#include <shared_mutex>
#include <future>

namespace OIF
{
	// ---------------------- Enums ----------------------
	enum class EventType { 
		kNone, // placeholder for no event (unavailable for users)
		kActivate, 
		kHit, // TESHitEvent + TESMagicEffectApplyEvent + ExplosionHook
		kGrab,
		kRelease,
		kThrow,
		kTelekinesis,
		kCellAttach,
		kCellDetach,
		kWeatherChange,
		kOnUpdate,
		kDestructionStageChange
	};

	enum class EffectType { 
		kRemoveItem, kDisableItem, kEnableItem,
		kSpawnItem, kSpawnSpell, kSpawnSpellOnItem, 
		kSpawnActor, kSpawnImpact, kSpawnImpactDataSet, 
		kSpawnExplosion, kSwapItem, kPlaySound, 
		kSpillInventory, kSwapActor, kSpawnLeveledItem,
		kSwapLeveledItem, kSpawnLeveledSpell, kSpawnLeveledSpellOnItem, 
		kSpawnLeveledActor, kSwapLeveledActor, kApplyIngestible, 
		kApplyOtherIngestible, kSpawnLight, kRemoveLight,
		kEnableLight, kDisableLight, kPlayIdle, 
		kSpawnEffectShader, kSpawnEffectShaderOnItem, kToggleNode, 
		//kToggleShaderFlag,
		kUnlockItem, kLockItem, kActivateItem, 
		kAddContainerItem, kAddActorItem, kRemoveContainerItem,
		kRemoveActorItem, kAddActorSpell, kRemoveActorSpell,
		kAddActorPerk, kRemoveActorPerk, kSpawnArtObject,
		kSpawnArtObjectOnItem, kExecuteConsoleCommand, kExecuteConsoleCommandOnItem, 
		kShowNotification, kShowMessageBox
	};

	// ---------------------- Filer ----------------------
	struct FormListEntry {
		std::uint32_t formID = 0; 											// id of the form of the formlist
		int index = -1;           											// -1 -use the entire list
	};

	struct NearbyEntry {
		RE::TESForm* form{ nullptr }; 										// form to match
		float radius{ 150.f };  											// radius to match
	};

	struct TimerEntry {
		float time{ 0.0f }; 												// time in seconds
		std::uint32_t matchFilterRecheck{ 0 }; 								// 0 - no re-check, 1 - re-check after timer expires
	};

	struct TimeCondition {
		std::string field;        											// "hour", "minute", "day", "month", "year", "dayofweek"
		std::string operator_type; 											// ">=", "=", "<", ">", "<=", "!="
		float value = 0.0f;
	};

	struct LevelCondition {
		std::string operator_type; 											// ">=", "=", "<", ">", "<=", "!="
		int value = 0;
	};

	struct ActorValueCondition {
		std::string actorValue; 											// "Health", "Magicka", "Stamina", etc.
		std::string operator_type; 											// ">=", "=", "<", ">", "<=", "!="
		float value = 0.0f;
	};

	struct Filter {
		// General filters
		std::unordered_set<RE::FormType> formTypes;           	 			// base form type to match
		std::unordered_set<RE::FormType> formTypesNot;       				// base form type to avoid
		std::unordered_set<RE::FormID> formIDs;           	  				// FormIDs to match
		std::unordered_set<RE::FormID> formIDsNot;       	  				// FormIDs to avoid
		std::vector<FormListEntry> formLists; 								// formlists contents to match
		std::vector<FormListEntry> formListsNot; 							// formlists contents to avoid 
		std::set<RE::BGSKeyword*> keywords;        	  						// must have ANY of these keywords
		std::set<RE::BGSKeyword*> keywordsNot;      						// must NOT have ANY of these keywords
		std::uint32_t questItemStatus{ 0 };									// 0 - not a quest item, 1 - an item with an alias, 2 - a quest item, 3 - all/undefined
		float chance{ 100.f };         							   			// the chance of 0‑100 % for the event to trigger
		std::uint32_t interactions{ 1 };									// number of interactions required to satisfy filter
		std::uint32_t limit = 0; 											// number of interactions to stop the effect
		std::uint32_t isInitiallyDisabled{ 2 }; 							// 0 - not initially disabled, 1 - initially disabled, 2 - all/undefined
		std::int32_t destructionStage{ -1 }; 								// destruction stage to match, -1 - any stage
		
		// New hit-specific filters
		std::unordered_set<std::string> weaponsTypes;          	 			// weapon type categories
		std::unordered_set<std::string> weaponsTypesNot;      				// weapon type categories to avoid
		std::set<RE::BGSKeyword*> weaponsKeywords;	   						// specific weapons or spells keywords
		std::set<RE::BGSKeyword*> weaponsKeywordsNot;						// specific weapons or spells keywords to avoid
		std::set<RE::TESForm*> weapons;       	 							// specific weapons or spells
		std::set<RE::TESForm*> weaponsNot;       							// specific weapons or spells to avoid
		std::uint32_t allowProjectiles{ 1 }; 								// 0 - no projectiles, 1 - allow projectiles
		std::set<RE::TESForm*> projectiles;  	  							// specific projectiles
		std::set<RE::TESForm*> projectilesNot; 								// specific projectiles to avoid
		std::unordered_set<std::string> attackTypes;           				// attack types
		std::unordered_set<std::string> attackTypesNot;       				// attack types to avoid
		std::unordered_set<std::string> deliveryTypes;           			// delivery types
		std::unordered_set<std::string> deliveryTypesNot;       			// delivery types to avoid

		// Location and weather filters
		std::unordered_set<RE::FormID> locations; 							// location IDs
		std::unordered_set<RE::FormID> locationsNot; 						// location IDs to avoid
		std::unordered_set<RE::FormID> weathers; 							// weather IDs
		std::unordered_set<RE::FormID> weathersNot; 						// weather IDs to avoid

		// Proximity filters
		std::vector<NearbyEntry> nearby;           							// nearby items to match
		std::vector<NearbyEntry> nearbyNot;     							// nearby items to avoid

		// Time filters
		std::vector<TimeCondition> time; 									// time conditions, e.g. ["hour >= 12", "dayofweek = 1"]
		std::vector<TimeCondition> timeNot; 								// time conditions to avoid
		TimerEntry timer;													// timer for the event

		// Actor values and inventory filters
		std::unordered_set<RE::FormID> perks;           					// perks to match
		std::unordered_set<RE::FormID> perksNot;       						// perks to avoid 
		std::unordered_set<RE::FormID> spells; 								// has any of these spells
		std::unordered_set<RE::FormID> spellsNot; 							// does not have any of these spells
		std::unordered_set<RE::FormID> hasItem; 							// has any of these items
		std::unordered_set<RE::FormID> hasItemNot; 							// does not have any of these items
		std::vector<LevelCondition> level; 									// level conditions, e.g. [">= 10"]
		std::vector<LevelCondition> levelNot; 								// level conditions to avoid, e.g. {"<", 5}
		std::vector<ActorValueCondition> actorValue; 						// actor value conditions, e.g. ["Health >= 50.0"]
		std::vector<ActorValueCondition> actorValueNot; 					// actor value conditions to avoid, e.g. ["Magicka < 20.0"]
		std::set<RE::BGSKeyword*> actorKeywords;           					// actor keywords to match
		std::set<RE::BGSKeyword*> actorKeywordsNot; 	 					// actor keywords to avoid
		std::set<RE::TESRace*> actorRaces;           						// actor races to match
		std::set<RE::TESRace*> actorRacesNot; 	 							// actor races to avoid
		std::uint32_t isSneaking{ 2 }; 										// 0 - not sneaking, 1 - sneaking, 2 - all/undefined
		std::uint32_t isSwimming{ 2 }; 										// 0 - not swimming, 1 - swimming, 2 - all/undefined
		std::uint32_t isInCombat{ 2 }; 										// 0 - not in combat, 1 - in combat, 2 - all/undefined
		std::uint32_t isMounted{ 2 }; 										// 0 - not mounted, 1 - mounted, 2 - all/undefined
		std::uint32_t isDualCasting{ 2 }; 									// 0 - not dual casting, 1 - dual casting, 2 - all/undefined
		std::uint32_t isSprinting{ 2 }; 									// 0 - not sprinting, 1 - sprinting, 2 - all/undefined
		std::uint32_t isWeaponDrawn{ 2 }; 									// 0 - not weapon drawn, 1 - weapon drawn, 2 - all/undefined

		// Other filters
		std::unordered_set<std::string> requiredPlugins; 					// required plugins
		std::unordered_set<std::string> requiredPluginsNot; 				// plugins to avoid
		std::unordered_set<std::string> requiredDLLs; 						// required DLLs
		std::unordered_set<std::string> requiredDLLsNot; 					// DLLs to avoid
	};

	struct FilterCache {
		std::optional<bool> locationMatch;
		std::optional<bool> weatherMatch;
		std::chrono::steady_clock::time_point lastUpdate;
		
		bool IsValid(std::chrono::milliseconds maxAge = std::chrono::milliseconds(1000)) const {
			auto now = std::chrono::steady_clock::now();
			return (now - lastUpdate) < maxAge;
		}
		
		void Invalidate() {
			locationMatch.reset();
			weatherMatch.reset();
		}
	};

	// ---------------------- Effect ----------------------
	struct EffectExtendedData {
		RE::TESForm* formID{ nullptr };  							   		// the thing to spawn/cast/play
		std::vector<FormListEntry> formLists; 								// formlists contents to spawn/cast/play
		std::uint32_t count{ 1 };      							   			// the amount of items to spawn/cast/play
		float chance{ 100.f };         							   			// the chance of 0‑100 %
		TimerEntry timer; 													// the timer for the effect, e.g. 1.0f for 1 second wait before applying
		float duration{ 1.f }; 												// the duration of the playidle effect
		float radius{ 100 };												// the radius of the DetachNearbyLight effect
		float scale{ -1.f };												// the scale of the spawned item
		std::string string;													// the text string to use in different effects
		std::vector<std::string> strings;									// the test strings to use in different effects
		std::uint32_t nonDeletable{ 0 }; 									// 1 if the form should not be deleted after disabling
		std::uint32_t spawnType{ 4 };										// the type of spawn
		std::uint32_t fade{ 1 };											// 0 if the effect should not fade, 1 if it should fade out
		std::uint32_t mode{ 0 };											// the type of search for the node, 0 - by name, 1 - by type
		//std::vector<std::string> flagNames;								// the name of the shader flags to toggle
		std::uint32_t rank{ 0 };											// the rank of the perk to apply
		bool isFormList = false;											// true if the form is a BGSListForm
		int index = -1;														// index of the form in the list
	};

	struct Effect {
		EffectType type{ EffectType::kSpawnItem }; 							// the type of effect
		std::vector<std::pair<RE::TESForm*, EffectExtendedData>> items;		// the vector of items to utilize
		float chance{ 100.f }; 												// the chance of 0‑100% (private, uses filter's value)
	};

	struct RuleContext {
		// General context
		EventType event;
		RE::Actor* source{ nullptr };
		RE::TESObjectREFR* target{ nullptr };
		RE::TESForm* baseObj{ nullptr };
		
		// Hit-specific context
		RE::TESForm* attackSource{ nullptr };
		RE::TESForm* projectileSource{ nullptr };
		std::string weaponType;
		std::string attackType;
		std::string deliveryType;
		bool isHitEvent{ false };

		// Additional context
		RE::TESWeather* weather{ nullptr };
		std::int32_t destructionStage{ -1 };
	};

	struct InventoryData {
		RE::TESBoundObject* item;
		std::uint32_t count;
	};

	struct ItemSpawnData {
		RE::TESBoundObject* item;
		std::string string; 
		std::uint32_t count;
		std::uint32_t spawnType{ 4 };
		std::uint32_t fade{ 1 };
		float scale;
		std::uint32_t nonDeletable{ 0 };
	};

	struct SpellSpawnData {
		RE::SpellItem* spell;
		std::uint32_t count;
		float radius;
	};

	struct PerkData {
		RE::BGSPerk* perk;
		std::uint32_t rank;
	};

	struct ActorSpawnData {
		RE::TESNPC* npc;
		std::string string;
		std::uint32_t count;
		std::uint32_t spawnType{ 4 };
		std::uint32_t fade{ 1 };
		float scale;
		std::uint32_t nonDeletable{ 0 };
	};

	struct LvlItemSpawnData {
		RE::TESLevItem* item;
		std::string string;
		std::uint32_t count;
		std::uint32_t spawnType{ 4 };
		std::uint32_t fade{ 1 };
		float scale;
		std::uint32_t nonDeletable{ 0 };
	};

	struct LvlSpellSpawnData {
		RE::TESLevSpell* spell;
		std::uint32_t count;
		float radius;
	};

	struct LvlActorSpawnData {
		RE::TESLevCharacter* npc;
		std::string string;
		std::uint32_t count;
		std::uint32_t spawnType{ 4 };
		std::uint32_t fade{ 1 };
		float scale;
		std::uint32_t nonDeletable{ 0 };
	};

	struct ImpactDataSetSpawnData {
		RE::BGSImpactDataSet* impact;
		std::uint32_t count;
	};

	struct ExplosionSpawnData {
		RE::BGSExplosion* explosion;
		std::string string;
		std::uint32_t count;
		std::uint32_t spawnType{ 4 };
		std::uint32_t fade{ 1 };
	};

	struct SoundSpawnData {
		RE::BGSSoundDescriptorForm* sound;
		std::uint32_t count;
	};

	struct IngestibleApplyData {
		RE::MagicItem* ingestible;
		std::uint32_t count;
		float radius;
	};

	struct LightSpawnData {
		RE::TESObjectLIGH* light;
		std::string string;
		std::uint32_t count;
		std::uint32_t spawnType{ 4 };
		std::uint32_t fade{ 1 };
		float scale;
	};

	struct LightRemoveData {
		RE::TESObjectLIGH* light;
		float radius;
	};

	struct PlayIdleData {
        RE::Actor* actor;
        std::string string;
        float duration{ 1.0f };
    };

	struct EffectShaderSpawnData {
		RE::TESEffectShader* effectShader;
		std::uint32_t count;
		float radius;
		float duration{ -1.0f };
	};	

	struct NodeData {
		std::uint32_t mode;
		std::vector<std::string> strings; 
	};

	/*struct ShaderFlagData {
		std::uint32_t mode;
		std::vector<std::string> flagNames;
		std::vector<std::string> strings;
		float chance{ 100.f };
	};*/

	struct ArtObjectData {
		RE::BGSArtObject* artObject;
		std::uint32_t count;
		float radius;
		float duration{ -1.0f };
	};

	struct StringData {
		std::string string;
		float radius;
	};

	struct Rule {
		std::vector<EventType> events;
		Filter filter;
		std::vector<Effect> effects;
		int dynamicIndex{ 0 };
		mutable FilterCache filterCache;
	};

	struct Key {
		std::uint32_t sourceID;
		std::uint32_t targetID; 
		std::uint16_t ruleIdx;
		
		bool operator<(const Key& other) const {
			return std::tie(sourceID, targetID, ruleIdx) < std::tie(other.sourceID, other.targetID, other.ruleIdx);
		}
	};

	struct UpdateFilter {
		std::unordered_set<RE::FormType> formTypes;
		std::unordered_set<RE::FormID> formIDs;
		std::unordered_set<RE::FormID> formLists;
		std::unordered_set<RE::BGSKeyword*> keywords;
		
		bool IsEmpty() const {
			return formTypes.empty() && formIDs.empty() && formLists.empty() && keywords.empty();
		}
		
		bool Matches(RE::TESObjectREFR* ref) const {
			if (!ref || !ref->GetBaseObject()) return false;
			
			auto* baseObj = ref->GetBaseObject();

			if (IsEmpty()) return true;
			bool hasMatch = false;

			if (!formTypes.empty()) {
				if (formTypes.contains(baseObj->GetFormType())) {
					hasMatch = true;
				}
			}

			if (!hasMatch && !formIDs.empty()) {
				if (formIDs.contains(baseObj->GetFormID())) {
					hasMatch = true;
				}
			}

			if (!hasMatch && !formLists.empty()) {
				for (const auto& formID : formLists) {
					auto* list = RE::TESForm::LookupByID<RE::BGSListForm>(formID);
					if (list) {
						for (auto* listItem : list->forms) {
							if (listItem && listItem->GetFormID() == baseObj->GetFormID()) {
								hasMatch = true;
								break;
							}
						}
						if (hasMatch) break;
					}
				}
			}
			
			if (!hasMatch && !keywords.empty()) {
				if (auto* kwForm = baseObj->As<RE::BGSKeywordForm>()) {
					for (auto* keyword : keywords) {
						if (keyword && kwForm->HasKeyword(keyword)) {
							hasMatch = true;
							break;
						}
					}
				}
			}
			
			return hasMatch;
		}
	};
	
	// ---------------------- Manager ----------------------
	class RuleManager {

	private:
		RuleManager(const RuleManager&) = delete;
		RuleManager& operator=(const RuleManager&) = delete;

		RuleManager() = default;

		void ParseJSON(const std::filesystem::path& path);
		bool MatchFilter(const Filter& f, const RuleContext& ctx, Rule& currentRule) const;
		void ApplyEffect(const Effect& eff, const RuleContext& ctx, Rule& currentRule) const;

		template <typename FormT, typename DataT, typename CreateDataFunc, typename ApplyEffectFunc>
		void ProcessEffect(
			const Effect& eff, const RuleContext& ctx, const Rule& currentRule,
			bool needsForm,
			CreateDataFunc createData,
			ApplyEffectFunc applyEffect
		) const {
			static thread_local std::mt19937 rng(std::random_device{}());
			std::vector<DataT> dataList;

			for (const auto& [form, extData] : eff.items) {
				float roll = std::uniform_real_distribution<float>(0.f, 100.f)(rng);
				if (roll > extData.chance) continue;

				auto processForm = [&, this](RE::TESForm* el) {
					if constexpr (std::is_same_v<FormT, void>) {
						dataList.emplace_back(createData(nullptr, extData));
					} else {
						FormT* casted = needsForm ? el->As<FormT>() : nullptr;
						if (needsForm && !casted) return;
						dataList.emplace_back(createData(casted, extData));
					}
				};

				if (extData.isFormList && form) {
					auto* list = form->As<RE::BGSListForm>();
					if (!list || list->forms.empty()) continue;

					int idx = extData.index;
					bool spawnAll = (idx == -1);

					if (idx == -3)
						idx = std::uniform_int_distribution<int>(0, static_cast<int>(list->forms.size()) - 1)(rng);
					else if (idx == -2)
						idx = currentRule.dynamicIndex;

					if (spawnAll) {
						for (auto* el : list->forms)
							if (el) processForm(el);
					}
					else if (idx >= 0 && idx < list->forms.size()) {
						if (auto* el = list->forms[idx])
							processForm(el);
					}
				}
				else if (!extData.isFormList) {
					processForm(form);
				}
			}

			if (!dataList.empty()) {
				if (eff.items.size() > 0 && eff.items[0].second.timer.time > 0.0f) {
					float timerValue = eff.items[0].second.timer.time;
					std::uint32_t matchFilterRecheck = eff.items[0].second.timer.matchFilterRecheck;
					
					static std::vector<std::future<void>> effectTimerTasks;
					static std::mutex effectTimerMutex;
		
					auto effectTimerFuture = std::async(std::launch::async, [this, dataList, ctx, applyEffect, timerValue, matchFilterRecheck, &currentRule]() {
						std::this_thread::sleep_for(std::chrono::duration<float>(timerValue));
						
						SKSE::GetTaskInterface()->AddTask([this, dataList, ctx, applyEffect, matchFilterRecheck, &currentRule]() mutable {
							auto* target = ctx.target;
							auto* source = ctx.source;
		
							if (!target || target->IsDeleted()) return;
		
							if (source && source->IsDeleted()) return;

							if (matchFilterRecheck == 1) {
								Rule& mutableRule = const_cast<Rule&>(currentRule);
								if (!MatchFilter(currentRule.filter, ctx, mutableRule)) return;
							}
		
							applyEffect(ctx, dataList);
						});
					});
		
					{
						std::lock_guard<std::mutex> timerLock(effectTimerMutex);
						effectTimerTasks.push_back(std::move(effectTimerFuture));
						
						effectTimerTasks.erase(
							std::remove_if(effectTimerTasks.begin(), effectTimerTasks.end(),
								[](const std::future<void>& f) {
									return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
								}),
							effectTimerTasks.end()
						);
					}
				} else {
					applyEffect(ctx, dataList);
				}
			}
		}

		std::map<Key, std::uint32_t> _limitCounts;
		std::map<Key, std::uint32_t> _interactionsCounts;

		std::unordered_map<std::uint32_t, std::chrono::steady_clock::time_point> recentlyProcessedItems;
        std::chrono::steady_clock::time_point lastCleanupTime;

		mutable std::vector<Rule*> updateRules;
		mutable bool updateRulesCached = false;

		mutable UpdateFilter cachedUpdateFilter;
    	mutable bool updateFilterCached = false;

		UpdateFilter BuildUpdateFilter() const {
			UpdateFilter filter;
			
			std::shared_lock lock(_ruleMutex);
			
			for (const auto& rule : _rules) {
				bool hasOnUpdate = std::find(rule.events.begin(), rule.events.end(), EventType::kOnUpdate) != rule.events.end();
				if (!hasOnUpdate) continue;
				
				for (auto formType : rule.filter.formTypes) {
					filter.formTypes.insert(formType);
				}
				
				for (auto formID : rule.filter.formIDs) {
					filter.formIDs.insert(formID);
				}
				
				for (const auto& entry : rule.filter.formLists) {
					auto* list = RE::TESForm::LookupByID<RE::BGSListForm>(entry.formID);
					if (!list) continue;
					
					for (auto* form : list->forms) {
						if (form) {
							filter.formIDs.insert(form->GetFormID());
						}
					}
				}

				if (!rule.filter.keywords.empty()) {
					filter.formTypes.insert(RE::FormType::Activator);
					filter.formTypes.insert(RE::FormType::TalkingActivator);
					filter.formTypes.insert(RE::FormType::Weapon);
					filter.formTypes.insert(RE::FormType::Armor);
					filter.formTypes.insert(RE::FormType::Ammo);
					filter.formTypes.insert(RE::FormType::Ingredient);
					filter.formTypes.insert(RE::FormType::Misc);
					filter.formTypes.insert(RE::FormType::Book);
					filter.formTypes.insert(RE::FormType::Note);
					filter.formTypes.insert(RE::FormType::Scroll);
					filter.formTypes.insert(RE::FormType::SoulGem);
					filter.formTypes.insert(RE::FormType::AlchemyItem);
					filter.formTypes.insert(RE::FormType::Furniture);
					//filter.formTypes.insert(RE::FormType::Door);
					filter.formTypes.insert(RE::FormType::Flora);
					//filter.formTypes.insert(RE::FormType::Container);
					//filter.formTypes.insert(RE::FormType::Static);
					//filter.formTypes.insert(RE::FormType::MovableStatic);
					//filter.formTypes.insert(RE::FormType::Tree);
					filter.formTypes.insert(RE::FormType::KeyMaster);
					//filter.formTypes.insert(RE::FormType::Light);
				}
			}

			return filter;
		}

	public:
		static RuleManager* GetSingleton();

		std::vector<Rule> _rules;
		mutable std::shared_mutex _ruleMutex;

		template <class T = RE::TESForm>
		static T* GetFormFromIdentifier(const std::string& identifier);
		template <class T = RE::TESForm>
		static T* GetFormFromEditorID(const std::string& editorID);

		void LoadRules();
		void Trigger(const RuleContext& ctx);
		void CleanupCounters();
		
		void ResetInteractionCounts();
		void OnSave(SKSE::SerializationInterface* intf);
		void OnLoad(SKSE::SerializationInterface* intf);
		void InitSerialization();

		void InvalidateUpdateCache() { 
			updateRulesCached = false; 
			updateFilterCached = false;
		}
    
		const std::vector<Rule*>& GetUpdateRules() {
			if (!updateRulesCached) {
				updateRules.clear();
				for (auto& rule : _rules) {
					if (std::find(rule.events.begin(), rule.events.end(), EventType::kOnUpdate) != rule.events.end()) {
						updateRules.push_back(&rule);
					}
				}
				updateRulesCached = true;
			}
			return updateRules;
		}

		const UpdateFilter& GetUpdateFilter() const {
			if (!updateFilterCached) {
				cachedUpdateFilter = BuildUpdateFilter();
				updateFilterCached = true;
			}
			return cachedUpdateFilter;
		}
	};
}