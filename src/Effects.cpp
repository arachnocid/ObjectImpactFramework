#include "Effects.h"

namespace OIF::Effects
{
    void DisposeItem(const RuleContext& ctx)
    {
        if (!ctx.target)
            return;

        ctx.target->Disable();
        ctx.target->SetDelete(true);
    }

    void SpawnItem(const RuleContext& ctx, RE::TESBoundObject* item, std::uint32_t count)
    {
        if (!ctx.target || !item)
            return;

        for (std::uint32_t i = 0; i < count; ++i)
            ctx.target->PlaceObjectAtMe(item, false);
    }

    void SpawnMultipleItems(const RuleContext& ctx, const std::vector<ItemSpawnData>& itemsData)
    {
        if (!ctx.target)
            return;

        for (const auto& itemData : itemsData) {
            if (!itemData.item)
                continue;
                
            for (std::uint32_t i = 0; i < itemData.count; ++i) {
                ctx.target->PlaceObjectAtMe(itemData.item, false);
            }
        }
    }

    void SpawnSpell(const RuleContext& ctx, RE::SpellItem* spell)
    {
        if (!ctx.source || !spell)
            return;

        RE::TESObjectREFR* caster = ctx.source;
        RE::TESObjectREFR* target = ctx.target;

        if (!target) {
            // If after all attempts there is no actor to cast the spell - exit
            logger::warn("SpawnSpell: No valid target to cast the spell");
            return;
        }

        if (!caster) {
            // If after all attempts there is no actor to cast the spell - exit
            logger::warn("SpawnSpell: No valid source to cast the spell");
            return;
        }

        caster = ctx.target;
        target = ctx.source; // The spell targets the source (attacker)

        caster->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant)
                  ->CastSpellImmediate(spell, false,
                                        target ? target : caster,
                                       1.0f, false, 0.0f, ctx.source);
    }

    void SpawnActor(const RuleContext& ctx, RE::TESNPC* npc, std::uint32_t count)
    {
        if (!ctx.target || !npc)
            return;

        for (std::uint32_t i = 0; i < count; ++i)
            ctx.target->PlaceObjectAtMe(npc, false);
    }

    void SpawnImpact(const RuleContext& ctx, RE::BGSImpactDataSet* impact)
    {
        if (!ctx.target || !impact)
            return;

        auto* im = RE::BGSImpactManager::GetSingleton();
        if (!im)
            return;

        RE::NiPoint3 hitPos = ctx.target->GetPosition();
        RE::NiPoint3 pickDir;
        
        if (ctx.source) {
            // If there is a source actor, use its position
            RE::NiPoint3 sourcePos = ctx.source->GetPosition();
            pickDir = hitPos - sourcePos;
        } 
        else if (ctx.source) {
            // If there is a sourceRef but no source, use the position of sourceRef
            RE::NiPoint3 sourcePos = ctx.source->GetPosition();
            pickDir = hitPos - sourcePos;
        } 
        else {
            // If there is no source at all, use the upward direction
            pickDir = RE::NiPoint3(0.0f, 0.0f, 1.0f);
        }

        float pickLen = pickDir.Length();
        if (pickLen > 0.0f) {
            pickDir /= pickLen;
        } else {
            pickLen = 1.0f;                             // minimum length to avoid division by zero
            pickDir = RE::NiPoint3(0.0f, 0.0f, 1.0f);
        }

        RE::BSFixedString nodeName("");
        im->PlayImpactEffect(ctx.target, impact, nodeName, pickDir, pickLen, false, false);
    }

    void SpawnExplosion(const RuleContext& ctx, RE::BGSExplosion* explosion)
    {
        if (!ctx.target || !explosion)
            return;

        ctx.target->PlaceObjectAtMe(explosion, false);
    }

    void SwapItem(const RuleContext& ctx, RE::TESBoundObject* item)
    {
        if (!ctx.target || !item)
            return;

        RE::TESObjectREFR* newRef = ctx.target->PlaceObjectAtMe(item, true).get();
    
        if (newRef) {
            // Only if the new object was successfully created, delete the old one
            ctx.target->Disable();
            ctx.target->SetDelete(true);
        } else {
            logger::error("Failed to place new object in SwapItem");
        }
    }

    void SwapWithMultipleItems(const RuleContext& ctx, const std::vector<ItemSpawnData>& itemsData)
    {
        if (!ctx.target)
            return;

        bool anyItemSpawned = false;

        for (const auto& itemData : itemsData) {
            if (!itemData.item)
                continue;
                
            for (std::uint32_t i = 0; i < itemData.count; ++i) {
                if (auto newRef = ctx.target->PlaceObjectAtMe(itemData.item, false)) {
                    anyItemSpawned = true;
                }
            }
        }

        if (anyItemSpawned) {
            ctx.target->Disable();
            ctx.target->SetDelete(true);
        }
    }
}