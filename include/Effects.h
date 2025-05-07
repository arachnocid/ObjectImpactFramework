#pragma once
#include "RuleManager.h"

namespace OIF::Effects
{
    void DisposeItem(const RuleContext& ctx);
    void SpawnItem(const RuleContext& ctx, RE::TESBoundObject* item, std::uint32_t count);
    void SpawnMultipleItems(const RuleContext& ctx, const std::vector<ItemSpawnData>& itemsData);
    void SpawnSpell(const RuleContext& ctx, RE::SpellItem* spell);
    void SpawnActor(const RuleContext& ctx, RE::TESNPC* npc, std::uint32_t count);
    void SpawnImpact(const RuleContext& ctx, RE::BGSImpactDataSet* impact);
    void SpawnExplosion(const RuleContext& ctx, RE::BGSExplosion* explosion);
    void SwapItem(const RuleContext& ctx, RE::TESBoundObject* item);
    void SwapWithMultipleItems(const RuleContext& ctx, const std::vector<ItemSpawnData>& itemsData);
    void PlaySound(const RuleContext& ctx, RE::BGSSoundDescriptorForm* sound);
}
