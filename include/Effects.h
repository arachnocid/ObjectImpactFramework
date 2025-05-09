#pragma once
#include "RuleManager.h"

namespace OIF::Effects
{
    // ------------------ Helpers ------------------
    bool IsItemProcessed(RE::TESObjectREFR* item);
    void MarkItemAsProcessed(RE::TESObjectREFR* item);
    void RemoveItemFromProcessed(RE::TESObjectREFR* item);
    void ClearProcessedItems();

    // ------------------ Effects ------------------
    void DisposeItem(const RuleContext& ctx);
    void SpawnItem(const RuleContext& ctx, const std::vector<ItemSpawnData>& itemsData);
    void SpawnSpell(const RuleContext& ctx, const std::vector<SpellSpawnData>& spellsData);
    void SpawnSpellOnItem(const RuleContext& ctx, const std::vector<SpellSpawnData>& spellsData);
    void SpawnActor(const RuleContext& ctx, const std::vector<ActorSpawnData>& actorsData);
    void SpawnImpact(const RuleContext& ctx, const std::vector<ImpactSpawnData>& impactsData);
    void SpawnExplosion(const RuleContext& ctx, const std::vector<ExplosionSpawnData>& explosionsData);
    void SwapItem(const RuleContext& ctx, const std::vector<ItemSpawnData>& itemsData);
    void PlaySound(const RuleContext& ctx, const std::vector<SoundSpawnData>& soundsData);
}
