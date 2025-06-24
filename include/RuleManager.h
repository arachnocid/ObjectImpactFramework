#pragma once
#include <shared_mutex>
#include <future>

namespace OIF
{
	// ---------------------- Enums ----------------------
	enum class EventType { 
		kActivate, 
		kHit, // TESHitEvent + TESMagicEffectApplyEvent + ExplosionHook
		kGrab,
		kRelease,
		kThrow,
		kTelekinesis,
		kCellAttach,
		kCellDetach,
		kWeatherChange
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
		kSpawnEffectShader, kSpawnEffectShaderOnItem, kAttachNode, 
		kToggleNode, //kToggleShaderFlag,
		kUnlockItem, kLockItem, kActivateItem, 
		kAddContainerItem, kAddActorItem, kRemoveContainerItem,
		kRemoveActorItem
	};

	// ---------------------- Filer ----------------------
	struct FormListEntry {
		std::uint32_t formID = 0; 											// id of the form of the formlist
		int index = -1;           											// -1 -use the entire list
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
		std::unordered_set<std::uint32_t> formIDs;           	  			// FormIDs to match
		std::unordered_set<std::uint32_t> formIDsNot;       	  			// FormIDs to avoid
		std::vector<FormListEntry> formLists; 								// formlists contents to match
		std::vector<FormListEntry> formListsNot; 							// formlists contents to avoid 
		std::unordered_set<RE::BGSKeyword*> keywords;        	  			// must have ANY of these keywords
		std::unordered_set<RE::BGSKeyword*> keywordsNot;      				// must NOT have ANY of these keywords
		std::uint32_t questItemStatus{ 0 };									// 0 - not a quest item, 1 - an item with an alias, 2 - a quest item, 3 - all/undefined
		float chance{ 100.f };         							   			// the chance of 0‑100 % for the event to trigger
		std::uint32_t interactions{ 1 };									// number of interactions required to satisfy filter
		std::uint32_t limit = 0; 											// number of interactions to stop the effect
		std::uint32_t isInitiallyDisabled{ 2 }; 							// 0 - not initially disabled, 1 - initially disabled, 2 - all/undefined
		
		// New hit-specific filters
		std::unordered_set<std::string> weaponsTypes;          	 			// weapon type categories
		std::unordered_set<std::string> weaponsTypesNot;      				// weapon type categories to avoid
		std::unordered_set<RE::BGSKeyword*> weaponsKeywords;	   			// specific weapons or spells keywords
		std::unordered_set<RE::BGSKeyword*> weaponsKeywordsNot;				// specific weapons or spells keywords to avoid
		std::unordered_set<RE::TESForm*> weapons;       	 				// specific weapons or spells
		std::unordered_set<RE::TESForm*> weaponsNot;       					// specific weapons or spells to avoid
		std::vector<FormListEntry> weaponsLists;           					// specific weapons or spells lists
		std::vector<FormListEntry> weaponsListsNot;       					// specific weapons or spells lists to avoid
		std::uint32_t allowProjectiles{ 1 }; 								// 0 - no projectiles, 1 - allow projectiles
		std::unordered_set<RE::TESForm*> projectiles;  	  					// specific projectiles
		std::unordered_set<RE::TESForm*> projectilesNot; 					// specific projectiles to avoid
		std::vector<FormListEntry> projectilesLists; 						// specific projectiles lists
		std::vector<FormListEntry> projectilesListsNot; 					// specific projectiles lists to avoid
		std::unordered_set<std::string> attackTypes;           				// attack types
		std::unordered_set<std::string> attackTypesNot;       				// attack types to avoid
		std::unordered_set<std::string> deliveryTypes;           			// delivery types
		std::unordered_set<std::string> deliveryTypesNot;       			// delivery types to avoid

		// Location and weather filters
		std::set<RE::FormID> locationIDs; 									// location IDs
		std::set<RE::FormID> locationIDsNot; 								// location IDs to avoid
		std::vector<RE::FormID> locationLists; 								// location lists
		std::vector<RE::FormID> locationListsNot; 							// location lists to avoid
		std::set<RE::FormID> weatherIDs; 									// weather IDs
		std::set<RE::FormID> weatherIDsNot; 								// weather IDs to avoid
		std::vector<RE::FormID> weatherLists; 								// weather lists
		std::vector<RE::FormID> weatherListsNot; 							// weather lists to avoid

		// Actor values and inventory filters
		std::unordered_set<std::uint32_t> perks;           					// perks to match
		std::unordered_set<std::uint32_t> perksNot;       					// perks to avoid 
		std::set<RE::FormID> hasItem; 										// has any of these items
		std::set<RE::FormID> hasItemNot; 									// does not have any of these items
		std::vector<LevelCondition> level; 									// level conditions, e.g. {">=", 10}
		std::vector<LevelCondition> levelNot; 								// level conditions to avoid, e.g. {"<", 5}
		std::vector<ActorValueCondition> actorValue; 						// actor value conditions, e.g. {"Health", ">=", 50.0f}
		std::vector<ActorValueCondition> actorValueNot; 					// actor value conditions to avoid, e.g. {"Magicka", "<", 20.0f}

		// Other filters
		std::set<std::string> requiredPlugins; 								// required plugins
		std::set<std::string> requiredPluginsNot; 							// plugins to avoid
		std::set<std::string> requiredDLLs; 								// required DLLs
		std::set<std::string> requiredDLLsNot; 								// DLLs to avoid
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
		float duration{ 1.f }; 												// the duration of the playidle effect
		float radius{ 100 };												// the radius of the DetachNearbyLight effect
		float scale{ -1.f };												// the scale of the spawned item
		std::string string;													// the text string to use in different effects
		std::vector<std::string> strings;									// the test strings to use in different effects
		std::uint32_t amount; 												// the amount to set
		std::uint32_t nonDeletable{ 0 }; 									// 1 if the form should not be deleted after disabling
		std::uint32_t spawnType{ 4 };										// the type of spawn
		std::uint32_t fade{ 1 };											// 0 if the effect should not fade, 1 if it should fade out
		std::uint32_t mode{ 0 };											// the type of search for the node, 0 - by name, 1 - by type
		std::vector<std::string> flagNames;									// the name of the shader flags to toggle
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
	};

	struct InventoryData {
		RE::TESBoundObject* item;
		std::uint32_t count;
		float chance{ 100.f };
	};

	struct ItemSpawnData {
		RE::TESBoundObject* item;
		std::uint32_t count;
		std::uint32_t spawnType{ 4 };
		std::uint32_t fade{ 1 };
		float scale;
		std::uint32_t nonDeletable{ 0 };
		float chance{ 100.f };
	};

	struct SpellSpawnData {
		RE::SpellItem* spell;
		std::uint32_t count;
		float radius;
		float chance{ 100.f };
	};

	struct ActorSpawnData {
		RE::TESNPC* npc;
		std::uint32_t count;
		std::uint32_t spawnType{ 4 };
		std::uint32_t fade{ 1 };
		float scale;
		std::uint32_t nonDeletable{ 0 };
		float chance{ 100.f };
	};

	struct LvlItemSpawnData {
		RE::TESLevItem* item;
		std::uint32_t count;
		std::uint32_t spawnType{ 4 };
		std::uint32_t fade{ 1 };
		float scale;
		std::uint32_t nonDeletable{ 0 };
		float chance{ 100.f };
	};

	struct LvlSpellSpawnData {
		RE::TESLevSpell* spell;
		std::uint32_t count;
		float radius;
		float chance{ 100.f };
	};

	struct LvlActorSpawnData {
		RE::TESLevCharacter* npc;
		std::uint32_t count;
		std::uint32_t spawnType{ 4 };
		std::uint32_t fade{ 1 };
		float scale;
		std::uint32_t nonDeletable{ 0 };
		float chance{ 100.f };
	};

	struct ImpactSpawnData {
		RE::BGSImpactData* impact;
		std::uint32_t count;
		float chance{ 100.f };
	};

	struct ImpactDataSetSpawnData {
		RE::BGSImpactDataSet* impact;
		std::uint32_t count;
		float chance{ 100.f };
	};

	struct ExplosionSpawnData {
		RE::BGSExplosion* explosion;
		std::uint32_t count;
		std::uint32_t spawnType{ 4 };
		std::uint32_t fade{ 1 };
		float chance{ 100.f };
	};

	struct SoundSpawnData {
		RE::BGSSoundDescriptorForm* sound;
		std::uint32_t count;
		float chance{ 100.f };
	};

	struct IngestibleApplyData {
		RE::MagicItem* ingestible;
		std::uint32_t count;
		float radius;
		float chance{ 100.f };
	};

	struct LightSpawnData {
		RE::TESObjectLIGH* light;
		std::uint32_t count;
		std::uint32_t spawnType{ 4 };
		std::uint32_t fade{ 1 };
		float scale;
		float chance{ 100.f };
	};

	struct LightRemoveData {
		RE::TESObjectLIGH* light;
		float radius;
		float chance{ 100.f };
	};

	struct PlayIdleData {
        RE::Actor* actor;
        std::string string;
        float duration{ 1.0f };
		float chance{ 100.f };
    };

	struct EffectShaderSpawnData {
		RE::TESEffectShader* effectShader;
		std::uint32_t count;
		float radius;
		float duration{ -1.0f };
		float chance{ 100.f };
	};	

	struct NodeData {
		std::uint32_t mode;
		std::vector<std::string> strings; 
		float chance{ 100.f };
	};

	/*struct ShaderFlagData {
		std::uint32_t mode;
		std::vector<std::string> flagNames;
		std::vector<std::string> strings;
		float chance{ 100.f };
	};*/

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

	// ---------------------- Manager ----------------------
	class RuleManager {

	public:
		static RuleManager* GetSingleton();

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

	private:
		RuleManager(const RuleManager&) = delete;
		RuleManager& operator=(const RuleManager&) = delete;

		RuleManager() = default;

		void ParseJSON(const std::filesystem::path& path);
		bool MatchFilter(const Filter& f, const RuleContext& ctx, Rule& currentRule) const;
		void ApplyEffect(const Effect& eff, const RuleContext& ctx, Rule& currentRule) const;

		std::vector<Rule> _rules;
		mutable std::shared_mutex _ruleMutex;
		std::map<Key, std::uint32_t> _limitCounts;
		std::map<Key, std::uint32_t> _interactionsCounts;

		std::unordered_map<std::uint32_t, std::chrono::steady_clock::time_point> recentlyProcessedItems;
        std::chrono::steady_clock::time_point lastCleanupTime;
	};
}