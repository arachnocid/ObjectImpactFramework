#pragma once
#include "RuleManager.h"

namespace OIF::Effects
{
    // ------------------ Helpers ------------------
    bool IsItemProcessed(RE::TESObjectREFR* item);
    void MarkItemAsProcessed(RE::TESObjectREFR* item);
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
    void SpillInventory(const RuleContext& ctx);
    void SwapActor(const RuleContext& ctx, const std::vector<ActorSpawnData>& actorsData);
    void SpawnLeveledItem(const RuleContext& ctx, const std::vector<LvlItemSpawnData>& itemsData);
    void SwapLeveledItem(const RuleContext& ctx, const std::vector<LvlItemSpawnData>& itemsData);
    void SpawnLeveledSpell(const RuleContext& ctx, const std::vector<LvlSpellSpawnData>& spellsData);
    void SpawnLeveledSpellOnItem(const RuleContext& ctx, const std::vector<LvlSpellSpawnData>& spellsData);
    void SpawnLeveledActor(const RuleContext& ctx, const std::vector<LvlActorSpawnData>& actorsData);
    void SwapLeveledActor(const RuleContext& ctx, const std::vector<LvlActorSpawnData>& actorsData);
    void ApplyIngestible(const RuleContext& ctx);
    void ApplyOtherIngestible(const RuleContext& ctx, const std::vector<IngestibleApplyData>& ingestiblesData);
}
