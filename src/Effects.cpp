#include "Effects.h"

namespace OIF::Effects
{
    // ------------------ Helpers ------------------
    static std::unordered_set<std::uint32_t> processedItems; 
    static std::mutex processedItemsMutex;

    bool IsItemProcessed(RE::TESObjectREFR* item)
    {
        if (!item)
            return false;
            
        const auto id = item->GetFormID();
        std::lock_guard lock(processedItemsMutex);
        return processedItems.contains(id);
    }

    void MarkItemAsProcessed(RE::TESObjectREFR* item)
    {
        if (!item)
            return;
            
        const auto id = item->GetFormID();
        std::lock_guard lock(processedItemsMutex);
        processedItems.insert(id);
    }

    void RemoveItemFromProcessed(RE::TESObjectREFR* item)
    {
        if (!item)
            return;
        const auto id = item->GetFormID();
        std::lock_guard lock(processedItemsMutex);
        processedItems.erase(id);
    }

    void ClearProcessedItems()
    {
        std::lock_guard lock(processedItemsMutex);
        processedItems.clear();
    }

    // ------------------ Effects ------------------
    void DisposeItem(const RuleContext& ctx)
    {
        if (!ctx.target) {
            logger::error("DisposeItem: No target to dispose");
            return;
        }

        if (OIF::Effects::IsItemProcessed(ctx.target))
            return;

        ctx.target->Disable();
        ctx.target->SetDelete(true);
    }

    void SpawnItem(const RuleContext& ctx, const std::vector<ItemSpawnData>& itemsData)
    {
        if (!ctx.target) {
            logger::error("SpawnMultipleItems: No target to spawn items");
            return;
        }

        if (OIF::Effects::IsItemProcessed(ctx.target))
            return;

        for (const auto& itemData : itemsData) {
            if (!itemData.item)
                continue;

            for (std::uint32_t i = 0; i < itemData.count; ++i) {
                ctx.target->PlaceObjectAtMe(itemData.item, false);
            }
        }
    }

    void SpawnSpell(const RuleContext& ctx, const std::vector<SpellSpawnData>& spellsData)
    {
        if (!ctx.source || spellsData.empty()) {
            logger::error("SpawnSpell: No source or spells to spawn");
            return;
        }

        if (OIF::Effects::IsItemProcessed(ctx.target))
            return;

        RE::TESObjectREFR* caster = ctx.target;
        RE::TESObjectREFR* target = ctx.source; // The spell targets the source (attacker)

        if (!target) {
            logger::warn("SpawnSpell: No valid target to cast the spell");
            return;
        }

        if (!caster) {
            logger::warn("SpawnSpell: No valid source to cast the spell");
            return;
        }

        auto* mc = caster->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
        if (!mc) {
            logger::error("SpawnSpell: caster has no MagicCaster");
            return;
        }

        for (const auto& spellData : spellsData) {
            if (!spellData.spell)
                continue;

            for (std::uint32_t i = 0; i < spellData.count; ++i) {
                mc->CastSpellImmediate(spellData.spell, false, target ? target : caster, 1.0f, false, 0.0f, ctx.source);
            }
        }
    }

    void SpawnSpellOnItem(const RuleContext& ctx, const std::vector<SpellSpawnData>& spellsData)
    {
        if (!ctx.source || spellsData.empty()) {
            logger::error("SpawnSpell: No source or spells to spawn");
            return;
        }

        if (OIF::Effects::IsItemProcessed(ctx.target))
            return;

        RE::TESObjectREFR* caster = ctx.source;
        RE::TESObjectREFR* target = ctx.target;

        if (!target) {
            logger::warn("SpawnSpell: No valid target to cast the spell");
            return;
        }

        if (!caster) {
            logger::warn("SpawnSpell: No valid source to cast the spell");
            return;
        }

        auto* mc = caster->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
        if (!mc) {
            logger::error("SpawnSpell: caster has no MagicCaster");
            return;
        }

        for (const auto& spellData : spellsData) {
            if (!spellData.spell)
                continue;

            for (std::uint32_t i = 0; i < spellData.count; ++i) {
                mc->CastSpellImmediate(spellData.spell, false, target ? target : caster, 1.0f, false, 0.0f, ctx.source);
            }
        }
    }

    void SpawnActor(const RuleContext& ctx, const std::vector<ActorSpawnData>& actorsData)
    {
        if (!ctx.target) {
            logger::error("SpawnMultipleActors: No target to spawn actors");
            return;
        }

        if (OIF::Effects::IsItemProcessed(ctx.target))
            return;

        for (const auto& actorData : actorsData) {
            if (!actorData.npc)
                continue;

            for (std::uint32_t i = 0; i < actorData.count; ++i) {
                ctx.target->PlaceObjectAtMe(actorData.npc, false);
            }
        }
    }
    
    void SpawnImpact(const RuleContext& ctx, const std::vector<ImpactSpawnData>& impactsData)
    {
        if (!ctx.target || impactsData.empty()) {
            logger::error("SpawnImpact: No target or impacts to spawn");
            return;
        }

        if (OIF::Effects::IsItemProcessed(ctx.target))
            return;

        auto* im = RE::BGSImpactManager::GetSingleton();
        if (!im)
            return;

        RE::NiPoint3 hitPos = ctx.target->GetPosition();
        RE::NiPoint3 pickDir;

        if (ctx.source) {
            RE::NiPoint3 sourcePos = ctx.source->GetPosition();
            pickDir = hitPos - sourcePos;
        } else {
            // Fallback: upward direction
            pickDir = RE::NiPoint3(0.0f, 0.0f, 1.0f);
        }

        float pickLen = pickDir.Length();
        if (pickLen > 0.0f) {
            pickDir /= pickLen;
        } else {
            // minimum length to avoid division by zero
            pickLen = 1.0f;                
            pickDir = RE::NiPoint3(0.0f, 0.0f, 1.0f);
        }

        RE::BSFixedString nodeName("");     // no specific node

        for (const auto& impactData : impactsData) {
            if (!impactData.impact)
                continue;

            for (std::uint32_t i = 0; i < impactData.count; ++i) {
                im->PlayImpactEffect(ctx.target, impactData.impact, nodeName, pickDir, pickLen, false, false);
            }
        }
    }

    void SpawnExplosion(const RuleContext& ctx, const std::vector<ExplosionSpawnData>& explosionsData)
    {
        if (!ctx.target || explosionsData.empty()) {
            logger::error("SpawnExplosion: No target or explosions to spawn");
            return;
        }

        if (OIF::Effects::IsItemProcessed(ctx.target))
            return;

        for (const auto& explosionData : explosionsData) {
            if (!explosionData.explosion)
                continue;

            for (std::uint32_t i = 0; i < explosionData.count; ++i) {
                ctx.target->PlaceObjectAtMe(explosionData.explosion, false);
            }
        }
    }

    void SwapItem(const RuleContext& ctx, const std::vector<ItemSpawnData>& itemsData)
    {
        if (!ctx.target) {
            logger::error("SwapWithMultipleItems: No target to swap items");
            return;
        }

        if (OIF::Effects::IsItemProcessed(ctx.target))
            return;

        bool anyItemSpawned = false;

        for (const auto& itemData : itemsData) {
            if (!itemData.item)
                continue;

            for (std::uint32_t i = 0; i < itemData.count; ++i) {
                if (ctx.target->PlaceObjectAtMe(itemData.item, false)) {
                    anyItemSpawned = true;
                }
            }
        }

        if (anyItemSpawned) {
            ctx.target->Disable();
            ctx.target->SetDelete(true);
        }
    }

    void PlaySound(const RuleContext& ctx, const std::vector<SoundSpawnData>& soundsData)
    {
        if (!ctx.target || soundsData.empty()) {
            logger::error("PlaySound: No target or sounds to play");
            return;
        }

        if (OIF::Effects::IsItemProcessed(ctx.target))
            return;

        RE::NiPoint3 pos = ctx.target->GetPosition();
        RE::BSSoundHandle handle;
        auto* audioManager = RE::BSAudioManager::GetSingleton();

        if (!audioManager) {
            logger::error("PlaySound: Failed to get audio manager");
            return;
        }

        for (const auto& soundData : soundsData) {
            if (!soundData.sound)
                continue;

            for (std::uint32_t i = 0; i < soundData.count; ++i) {
                if (audioManager->BuildSoundDataFromDescriptor(handle, soundData.sound, 1)) {
                    handle.SetPosition(pos);
                    handle.Play();
                } else {
                    logger::error("PlaySound: Failed to play sound");
                }
            }
        }
    }
}
