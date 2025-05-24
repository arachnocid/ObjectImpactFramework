#pragma once
#include <shared_mutex>

namespace OIF
{
	// ---------------------- Enums ----------------------
	enum class EventType { 
		kActivate, 
		kHit,
		kGrab,
		kRelease,
		kThrow
	};

	enum class EffectType { 
		kRemoveItem, 
		kSpawnItem, 
		kSpawnSpell, 
		kSpawnSpellOnItem, 
		kSpawnActor, 
		kSpawnImpact, 
		kSpawnExplosion, 
		kSwapItem, 
		kPlaySound, 
		kSpillInventory, 
		kSwapActor,
		kSpawnLeveledItem,
		kSwapLeveledItem,
		kSpawnLeveledSpell,
		kSpawnLeveledSpellOnItem,
		kSpawnLeveledActor,
		kSwapLeveledActor,
		kApplyIngestible,
		kApplyOtherIngestible,
		kSpawnLight,
		kRemoveLight,
		kEnableLight,
		kDisableLight
	};

	// ---------------------- Filer ----------------------
	struct FormListEntry {
		std::uint32_t formID = 0; 											// id of the form of the formlist
		int index = -1;           											// -1 -use the entire list
	};

	struct Filter
	{
		// General filters
		std::unordered_set<RE::FormType> formTypes;           	 			// filter by base form type
		std::unordered_set<std::uint32_t> formIDs;           	  			// full FormIDs to match
		std::vector<FormListEntry> formLists; 								// formlists contents to match
		std::unordered_set<RE::BGSKeyword*> keywords;        	  			// must have ANY of these keywords
		std::unordered_set<RE::BGSKeyword*> keywordsNot;      				// must NOT have ANY of these keywords
		float chance{ 100.f };         							   			// the chance of 0‑100 %
		std::uint32_t interactions{1};										// number of interactions required to satisfy filter
		std::uint32_t limit = 0; 											// number of interactions to stop the effect (BETA)
		
		// New hit-specific filters
		std::unordered_set<std::string> weaponTypes;          	 			// weapon type categories
		std::unordered_set<RE::BGSKeyword*> weaponsKeywords;	   			// specific weapons' or spells' keywords
		std::unordered_set<RE::BGSKeyword*> weaponsKeywordsNot;				// specific weapons' or spells' keywords to avoid
		std::unordered_set<RE::TESForm*> weapons;       	 				// specific weapons or spells
		std::unordered_set<RE::BGSProjectile*> projectiles;  	  			// specific projectiles
		std::unordered_set<std::string> attackTypes;           				// attack types

		// Other filters
		std::set<std::string> requiredPlugins; 								// required plugins
		std::set<std::string> requiredPluginsNot; 							// plugins to avoid
		std::set<std::string> requiredDLLs; 								// required DLLs
		std::set<std::string> requiredDLLsNot; 								// DLLs to avoid									// cell IDs
	};

	// ---------------------- Effect ----------------------
	struct EffectExtendedData
	{
		RE::TESForm* form{ nullptr };  							   			// the thing to spawn/cast/play
		std::vector<FormListEntry> formLists; 								// formlists contents to spawn/cast/play
		std::uint32_t count{ 1 };      							   			// the amount of items to spawn/cast/play
		float chance{ 100.f };         							   			// the chance of 0‑100 %
		std::uint32_t radius{ 100 };												// the radius of the DetachNearbyLight effect

		bool isFormList = false;											// true if the form is a BGSListForm
		int index = -1;														// index of the form in the list
	};

	struct Effect
	{
		EffectType type{ EffectType::kSpawnItem }; 							// the type of effect
		std::vector<std::pair<RE::TESForm*, EffectExtendedData>> items;		// the vector of items to utilize
		float chance{ 100.f }; 												// the chance of 0‑100% (private, uses filter's value)
	};

	struct RuleContext
	{
		// General context
		EventType event;
		RE::Actor* source{ nullptr };
		RE::TESObjectREFR* target{ nullptr };
		RE::TESForm* baseObj{ nullptr };
		
		// Hit-specific context
		RE::TESForm* attackSource{ nullptr };
		RE::BGSProjectile* projectile{ nullptr };
		std::string weaponType;
		std::string attackType;
		bool isHitEvent{ false };
	};

	struct ItemSpawnData 
	{
		RE::TESBoundObject* item;
		std::uint32_t count;
		std::uint32_t formID;
		float chance{ 100.f };
	};

	struct SpellSpawnData 
	{
		RE::SpellItem* spell;
		std::uint32_t count;
		std::uint32_t formID;
		float chance{ 100.f };
	};

	struct ActorSpawnData 
	{
		RE::TESNPC* npc;
		std::uint32_t count;
		std::uint32_t formID;
		float chance{ 100.f };
	};

	struct LvlItemSpawnData 
	{
		RE::TESLevItem* item;
		std::uint32_t count;
		std::uint32_t formID;
		float chance{ 100.f };
	};

	struct LvlSpellSpawnData 
	{
		RE::TESLevSpell* spell;
		std::uint32_t count;
		std::uint32_t formID;
		float chance{ 100.f };
	};

	struct LvlActorSpawnData 
	{
		RE::TESLevCharacter* npc;
		std::uint32_t count;
		std::uint32_t formID;
		float chance{ 100.f };
	};

	struct ImpactSpawnData 
	{
		RE::BGSImpactDataSet* impact;
		std::uint32_t count;
		std::uint32_t formID;
		float chance{ 100.f };
	};

	struct ExplosionSpawnData 
	{
		RE::BGSExplosion* explosion;
		std::uint32_t count;
		std::uint32_t formID;
		float chance{ 100.f };
	};

	struct SoundSpawnData 
	{
		RE::BGSSoundDescriptorForm* sound;
		std::uint32_t count;
		std::uint32_t formID;
		float chance{ 100.f };
	};

	struct IngestibleApplyData {
		RE::MagicItem* ingestible;
		std::uint32_t count;
		std::uint32_t formID;
		float chance{100.f};
	};

	struct LightSpawnData {
		RE::TESObjectLIGH* light;
		std::uint32_t count;
		std::uint32_t formID;
		float chance{100.f};
	};

	struct LightRemoveData {
		RE::TESObjectLIGH* light;
		std::uint32_t radius;
		std::uint32_t formID;
		float chance{100.f};
	};

	struct Rule
	{
		std::vector<EventType> events;
		Filter  filter;
		std::vector<Effect> effects;
		int dynamicIndex{ 0 };
	};

	// ---------------------- Manager ----------------------
	class RuleManager
	{
	public:
		static RuleManager* GetSingleton();

		void LoadRules();
		void Trigger(const RuleContext& ctx);
		
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

		template <class T = RE::TESForm>
		static T* GetFormFromIdentifier(const std::string& identifier);

		std::vector<Rule> _rules;
		mutable std::shared_mutex _ruleMutex;
		std::unordered_map<uint64_t, uint32_t> _filterInteractionCounts;
	};
}