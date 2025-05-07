#pragma once
#include <vector>
#include <string>
#include <unordered_set>
#include <functional>
#include <random>
#include <filesystem>
#include <shared_mutex>
#include <mutex>
#include "RE/Skyrim.h"
#include "SKSE/SKSE.h"

namespace OIF   // ObjectImpactFramework
{
	// ---------------------- ENUMS ----------------------
	enum class EventType { kActivate, kHit };

	enum class EffectType { kDisposeItem, kSpawnItem, kSpawnMultipleItems, kSpawnSpell, kSpawnActor, kSpawnImpact, kSpawnExplosion, kSwapItem, kSwapWithMultipleItems };

	// ---------------------- FILTER ----------------------
	struct Filter
	{
		std::unordered_set<RE::FormType> formTypes;           	 	// filter by base form type
		std::unordered_set<std::uint32_t> formIDs;           	  	// full FormIDs to match
		std::unordered_set<RE::BGSKeyword*> keywords;        	  	// must have ANY of these keywords
		
		// New hit-specific filters
		std::unordered_set<std::string> weaponTypes;          	 	// weapon type categories
		std::unordered_set<RE::TESObjectWEAP*> weapons;       	 	// specific weapons
		std::unordered_set<RE::BGSProjectile*> projectiles;  	  	// specific projectiles
		std::unordered_set<std::string> attackTypes;          		// attack types
	};

	// ---------------------- EFFECT ----------------------
	struct Effect
	{
		EffectType type{ EffectType::kSpawnItem };
		RE::TESForm* form{ nullptr };  							   	// the thing to spawn / cast / play
		std::uint32_t count{ 1 };      							   	// the amount for SpawnItem / SpawnActor
		float chance{ 100.f };         							   	// the chance of 0‑100 %
		std::vector<std::pair<RE::TESForm*, std::uint32_t>> items; 	// for SpawnMultipleItems / SwapWithMultipleItems
	};

	// Context passed from EventSinks
	struct RuleContext
	{
		EventType event;
		RE::Actor* source{ nullptr };
		RE::TESObjectREFR* target{ nullptr };
		RE::TESForm* baseObj{ nullptr };
		
		// Hit-specific context
		RE::TESObjectWEAP* weapon{ nullptr };
		RE::BGSProjectile* projectile{ nullptr };
		std::string weaponType;
		std::string attackType;
		bool isHitEvent{ false };
	};

	struct ItemSpawnData 
	{
		RE::TESBoundObject* item;
		std::uint32_t count;
	};

	struct Rule
	{
		std::vector<EventType> events;
		Filter  filter;
		std::vector<Effect> effects;
	};

	// ---------------------- MANAGER ----------------------
	class RuleManager
	{
	public:
		static RuleManager* GetSingleton();

		void LoadRules();
		void Trigger(const RuleContext& ctx);

	private:
RuleManager(const RuleManager&) = delete;
RuleManager& operator=(const RuleManager&) = delete;

		RuleManager() = default;

		void ParseJSON(const std::filesystem::path& path);
		bool MatchFilter(const Filter& f, const RuleContext& ctx) const;
		void ApplyEffect(const Effect& eff, const RuleContext& ctx) const;

		template <class T = RE::TESForm>
		static T* GetFormFromIdentifier(const std::string& identifier);

		std::vector<Rule> _rules;
		mutable std::shared_mutex _ruleMutex;
	};
}