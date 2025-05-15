#include "Effects.h"

namespace OIF::Effects
{
    // ------------------ Helpers ------------------
    static std::unordered_set<std::uint32_t> processedItems; 
    static std::mutex processedItemsMutex;

    // Check if the item has already been processed
    // This is used to prevent duplicate processing of the same item
    bool IsItemProcessed(RE::TESObjectREFR* item)
    {
        if (!item)
            return false;
            
        const auto id = item->GetFormID();
        std::lock_guard lock(processedItemsMutex);
        return processedItems.contains(id);
    }

    // Mark the item as processed
    // This is used to prevent reprocessing the same item
    void MarkItemAsProcessed(RE::TESObjectREFR* item)
    {
        if (!item)
            return;
            
        const auto id = item->GetFormID();
        std::lock_guard lock(processedItemsMutex);
        processedItems.insert(id);
    }

    // Clear the processed items set
    // This is used to reset the processed items list
    void ClearProcessedItems()
    {
        std::lock_guard lock(processedItemsMutex);
        processedItems.clear();
    }

    // Copy ownership from one reference to another
    void CopyOwnership(RE::TESObjectREFR* from, RE::TESObjectREFR* to)
    {
        if (!from || !to || !from->GetOwner()) return;

        if (auto* owner = from->GetOwner()) {
            to->SetOwner(owner);
        }
    }

    // Resolve leveled items, spells, and NPCs
    static RE::TESBoundObject* ResolveLeveledItem(RE::TESLevItem* lvli)
    {
        if (!lvli || lvli->entries.empty())
            return nullptr;

        // Get player level
        auto* player = RE::PlayerCharacter::GetSingleton();
        if (!player)
            return nullptr;
        int playerLevel = player->GetLevel();

        // Filter entries by level
        std::vector<RE::LEVELED_OBJECT*> validEntries;
        for (auto& entry : lvli->entries) {
            if (entry.level <= playerLevel) {
                validEntries.push_back(&entry);
            }
        }

        if (validEntries.empty())
            return nullptr;

        // Random selection from valid entries
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<std::size_t> dist(0, validEntries.size() - 1);

        constexpr int maxTries = 8;
        for (int i = 0; i < maxTries; ++i) {
            auto* entry = validEntries[dist(gen)];
            auto* picked = entry->form;
            if (!picked)
                continue;
            if (picked->GetFormType() == RE::FormType::LeveledItem) {
                auto* resolved = ResolveLeveledItem(picked->As<RE::TESLevItem>());
                if (resolved)
                    return resolved;
            } else {
                return picked->As<RE::TESBoundObject>();
            }
        }
        return nullptr;
    }

    static RE::SpellItem* ResolveLeveledSpell(RE::TESLevSpell* lvls)
    {
        if (!lvls || lvls->entries.empty())
            return nullptr;

        auto* player = RE::PlayerCharacter::GetSingleton();
        if (!player)
            return nullptr;
        int playerLevel = player->GetLevel();

        std::vector<RE::LEVELED_OBJECT*> validEntries;
        for (auto& entry : lvls->entries) {
            if (entry.level <= playerLevel) {
                validEntries.push_back(&entry);
            }
        }

        if (validEntries.empty())
            return nullptr;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<std::size_t> dist(0, validEntries.size() - 1);

        constexpr int maxTries = 8;
        for (int i = 0; i < maxTries; ++i) {
            auto* entry = validEntries[dist(gen)];
            auto* picked = entry->form;
            if (!picked)
                continue;
            if (picked->GetFormType() == RE::FormType::LeveledSpell) {
                auto* resolved = ResolveLeveledSpell(picked->As<RE::TESLevSpell>());
                if (resolved)
                    return resolved;
            } else {
                return picked->As<RE::SpellItem>();
            }
        }
        return nullptr;
    }

    static RE::TESNPC* ResolveLeveledNPC(RE::TESLevCharacter* lvlc)
    {
        if (!lvlc || lvlc->entries.empty())
            return nullptr;

        auto* player = RE::PlayerCharacter::GetSingleton();
        if (!player)
            return nullptr;
        int playerLevel = player->GetLevel();

        std::vector<RE::LEVELED_OBJECT*> validEntries;
        for (auto& entry : lvlc->entries) {
            if (entry.level <= playerLevel) {
                validEntries.push_back(&entry);
            }
        }

        if (validEntries.empty())
            return nullptr;

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<std::size_t> dist(0, validEntries.size() - 1);

        constexpr int maxTries = 8;
        for (int i = 0; i < maxTries; ++i) {
            auto* entry = validEntries[dist(gen)];
            auto* picked = entry->form;
            if (!picked)
                continue;
            if (picked->GetFormType() == RE::FormType::LeveledNPC) {
                auto* resolved = ResolveLeveledNPC(picked->As<RE::TESLevCharacter>());
                if (resolved)
                    return resolved;
            } else {
                return picked->As<RE::TESNPC>();
            }
        }
        return nullptr;
    }

    // ------------------ Effects ------------------
    void DisposeItem(const RuleContext& ctx)
    {
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::error("DisposeItem: No target to dispose");
            return;
        }

        if (!ctx.target->IsDisabled()) {
            ctx.target->Disable();
        } else {
            ctx.target->SetDelete(true);
        }  
    }

    void SpawnItem(const RuleContext& ctx, const std::vector<ItemSpawnData>& itemsData)
    {
        if (!ctx.target || itemsData.empty() || ctx.target->IsDeleted()) {
            logger::error("SpawnItem: No target to spawn items");
            return;
        }

        // Determine drop position
        // If the container is a 3D object, use its world position
        // Otherwise, use the center of its bounding box
        NiPoint3 dropPos;
        if (auto* root = ctx.target->Get3D()) { 
            dropPos = root->worldBound.center;     
        } else {                                    
            const auto& bmin = ctx.target->GetBoundMin();
            const auto& bmax = ctx.target->GetBoundMax();
            dropPos = NiPoint3{
                (bmin.x + bmax.x) * 0.5f,
                (bmin.y + bmax.y) * 0.5f,
                (bmin.z + bmax.z) * 0.5f
            };
        }

        for (const auto& itemData : itemsData) {
            if (!itemData.item)
                continue;

            for (std::uint32_t i = 0; i < itemData.count; ++i) {
                RE::NiPointer<RE::TESObjectREFR> item = ctx.target->PlaceObjectAtMe(itemData.item, true);
                if (item) {
                    item->SetPosition(dropPos);
                    CopyOwnership(ctx.target, item.get());
                } else {
                    logger::warn("SpawnItem: Failed to spawn item {}", itemData.item->GetFormID());
                }
            }
        }
    }

    void SpawnSpell(const RuleContext& ctx, const std::vector<SpellSpawnData>& spellsData)
    {
        if (!ctx.source || spellsData.empty()) {
            logger::error("SpawnSpell: No source or spells to spawn");
            return;
        }

        RE::TESObjectREFR* caster = ctx.target;
        RE::TESObjectREFR* target = ctx.source; // The spell targets the source (attacker)

        if (!target || target->IsDeleted()) {
            logger::warn("SpawnSpell: No valid target to cast the spell");
            return;
        }

        if (!caster || caster->IsDeleted()) {
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
                mc->CastSpellImmediate(static_cast<RE::MagicItem*>(spellData.spell), false, target ? target : caster, 1.0f, false, 0.0f, ctx.source);
            }
        }
    }

    void SpawnSpellOnItem(const RuleContext& ctx, const std::vector<SpellSpawnData>& spellsData)
    {
        if (!ctx.source || spellsData.empty()) {
            logger::error("SpawnSpellOnItem: No source or spells to spawn on");
            return;
        }

        RE::TESObjectREFR* caster = ctx.source;
        RE::TESObjectREFR* target = ctx.target;

        if (!target || target->IsDeleted()) {
            logger::warn("SpawnSpell: No valid target to cast the spell");
            return;
        }

        if (!caster || caster->IsDeleted()) {
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
                mc->CastSpellImmediate(static_cast<RE::MagicItem*>(spellData.spell), false, target ? target : caster, 1.0f, false, 0.0f, ctx.source);
            }
        }
    }

    void SpawnActor(const RuleContext& ctx, const std::vector<ActorSpawnData>& actorsData)
    {
        if (!ctx.target || actorsData.empty() || ctx.target->IsDeleted()) {
            logger::error("SpawnActor: No target to spawn actors");
            return;
        }

        NiPoint3 dropPos;
        if (auto* root = ctx.target->Get3D()) { 
            dropPos = root->worldBound.center;     
        } else {                                    
            const auto& bmin = ctx.target->GetBoundMin();
            const auto& bmax = ctx.target->GetBoundMax();
            dropPos = NiPoint3{
                (bmin.x + bmax.x) * 0.5f,
                (bmin.y + bmax.y) * 0.5f,
                (bmin.z + bmax.z) * 0.5f
            };
        }

        for (const auto& actorData : actorsData) {
            if (!actorData.npc)
                continue;

            for (std::uint32_t i = 0; i < actorData.count; ++i) {
                RE::NiPointer<RE::TESObjectREFR> npc = ctx.target->PlaceObjectAtMe(actorData.npc, true);
                if (npc) {
                    npc->SetPosition(dropPos);
                }
            }
        }
    }
    
    void SpawnImpact(const RuleContext& ctx, const std::vector<ImpactSpawnData>& impactsData)
    {
        if (!ctx.target || impactsData.empty() || ctx.target->IsDeleted()) {
            logger::error("SpawnImpact: No target or impacts to spawn");
            return;
        }

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
        if (!ctx.target || explosionsData.empty() || ctx.target->IsDeleted()) {
            logger::error("SpawnExplosion: No target or explosions to spawn");
            return;
        }

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
        if (!ctx.target || itemsData.empty() || ctx.target->IsDeleted()) {
            logger::error("SwapItem: No target to swap items");
            return;
        }

        NiPoint3 dropPos;
        if (auto* root = ctx.target->Get3D()) { 
            dropPos = root->worldBound.center;     
        } else {                                    
            const auto& bmin = ctx.target->GetBoundMin();
            const auto& bmax = ctx.target->GetBoundMax();
            dropPos = NiPoint3{
                (bmin.x + bmax.x) * 0.5f,
                (bmin.y + bmax.y) * 0.5f,
                (bmin.z + bmax.z) * 0.5f
            };
        }

        bool anyItemSpawned = false;

        for (const auto& itemData : itemsData) {
            if (!itemData.item)
                continue;

            for (std::uint32_t i = 0; i < itemData.count; ++i) {
                RE::NiPointer<RE::TESObjectREFR> item = (ctx.target->PlaceObjectAtMe(itemData.item, true));
                    if (item) {
                        anyItemSpawned = true;
                        item->SetPosition(dropPos);
                        CopyOwnership(ctx.target, item.get());
                    } else {
                        logger::warn("SwapItem: Failed to swap with item {}", itemData.item->GetFormID());
                    }
                }
            }

        if (anyItemSpawned) {
            if (!ctx.target->IsDisabled()) {
                ctx.target->Disable();
            } else {
                ctx.target->SetDelete(true);
            }  
        }
    }

    void PlaySound(const RuleContext& ctx, const std::vector<SoundSpawnData>& soundsData)
    {
        if (!ctx.target || soundsData.empty() || ctx.target->IsDeleted()) {
            logger::error("PlaySound: No target or sounds to play");
            return;
        }

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

    void SpillInventory(const RuleContext& ctx)
    {
        auto* containerRef = ctx.target;
        if (!containerRef || containerRef->IsDeleted()) {
            logger::error("SpillInventory: No target to spill inventory");
            return;
        }

        auto* baseObj = containerRef->GetBaseObject();
        if (!baseObj || (!baseObj->Is(RE::FormType::Container))) {
            return;
        }

        NiPoint3 dropPos;
        if (auto* root = containerRef->Get3D()) { 
            dropPos = root->worldBound.center;     
        } else {                                    
            const auto& bmin = containerRef->GetBoundMin();
            const auto& bmax = containerRef->GetBoundMax();
            dropPos = NiPoint3{
                (bmin.x + bmax.x) * 0.5f,
                (bmin.y + bmax.y) * 0.5f,
                (bmin.z + bmax.z) * 0.5f
            };
        }
    
        NiPoint3 dropAngle = containerRef->GetAngle();  

        for (auto& [obj, data] : containerRef->GetInventory()) {
            if (!obj) {
                logger::warn("SpillInventory: Invalid inventory object for target {}", containerRef->GetFormID());
                continue;
            } else if (obj && (data.first > 0)) {
                containerRef->RemoveItem(obj, data.first, ITEM_REMOVE_REASON::kDropping, nullptr, containerRef, &dropPos, &dropAngle);
            }
        }
    }

    void SwapActor(const RuleContext& ctx, const std::vector<ActorSpawnData>& actorsData)
    {
        if (!ctx.target || actorsData.empty() || ctx.target->IsDeleted()) {
            logger::error("SwapActor: No target to swap actors");
            return;
        }

        NiPoint3 dropPos;
        if (auto* root = ctx.target->Get3D()) { 
            dropPos = root->worldBound.center;     
        } else {                                    
            const auto& bmin = ctx.target->GetBoundMin();
            const auto& bmax = ctx.target->GetBoundMax();
            dropPos = NiPoint3{
                (bmin.x + bmax.x) * 0.5f,
                (bmin.y + bmax.y) * 0.5f,
                (bmin.z + bmax.z) * 0.5f
            };
        }

        bool anyActorSpawned = false;

        for (const auto& actorData : actorsData) {
            if (!actorData.npc)
                continue;

            for (std::uint32_t i = 0; i < actorData.count; ++i) {
                RE::NiPointer<RE::TESObjectREFR> npc = ctx.target->PlaceObjectAtMe(actorData.npc, true);
                if (npc) {
                    npc->SetPosition(dropPos);
                    anyActorSpawned = true;
                }
            }
        }

        if (anyActorSpawned) {
            if (!ctx.target->IsDisabled()) {
                ctx.target->Disable();
            } else {
                ctx.target->SetDelete(true);
            }  
        }
    }

    void SpawnLeveledItem(const RuleContext& ctx, const std::vector<LvlItemSpawnData>& data)
    {
        if (!ctx.target || data.empty() || ctx.target->IsDeleted()) {
            logger::error("SpawnLeveledItem: invalid target");
            return;
        }

        NiPoint3 dropPos;
        if (auto* root = ctx.target->Get3D()) { 
            dropPos = root->worldBound.center;     
        } else {                                    
            const auto& bmin = ctx.target->GetBoundMin();
            const auto& bmax = ctx.target->GetBoundMax();
            dropPos = NiPoint3{
                (bmin.x + bmax.x) * 0.5f,
                (bmin.y + bmax.y) * 0.5f,
                (bmin.z + bmax.z) * 0.5f
            };
        }

        for (const auto& d : data) {
            if (!d.item) continue;

            for (std::uint32_t i = 0; i < d.count; ++i) {
                auto* obj = ResolveLeveledItem(d.item);
                if (!obj) {
                    logger::warn("SpawnLeveledItem: can't resolve LVLI {:X}",
                                d.item->GetFormID());
                    continue;
                }

                auto ref = ctx.target->PlaceObjectAtMe(obj, true);
                if (ref) {
                    CopyOwnership(ctx.target, ref.get());
                    ref->SetPosition(dropPos);
                } else {
                    logger::warn("SpawnLeveledItem: place failed for {:X}",
                                obj->GetFormID());
                }
            }
        }
    }

    void SwapLeveledItem(const RuleContext& ctx, const std::vector<LvlItemSpawnData>& data)
    {
        bool spawned = false;
        SpawnLeveledItem(ctx, data);

        {
            std::lock_guard lock(processedItemsMutex);
            spawned = !processedItems.empty();
        }
        
        if (spawned) {
            if (!ctx.target->IsDisabled())
                ctx.target->Disable();
            else
                ctx.target->SetDelete(true);
        }
    }

    void SpawnLeveledSpell(const RuleContext& ctx, const std::vector<LvlSpellSpawnData>& data)
    {
        if (!ctx.source || data.empty()) {
            logger::error("SpawnLeveledSpell: invalid source");
            return;
        }

        auto* caster  = ctx.target;
        auto* target  = ctx.source;

        if (!caster || caster->IsDeleted() || !target || target->IsDeleted()) {
            logger::warn("SpawnLeveledSpell: bad refs");
            return;
        }

        auto* mc = caster->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
        if (!mc) {
            logger::error("SpawnLeveledSpell: caster lacks MagicCaster");
            return;
        }

        for (const auto& d : data) {
            if (!d.spell) continue;

            for (std::uint32_t i = 0; i < d.count; ++i) {
                auto* spell = ResolveLeveledSpell(d.spell);
                if (!spell) {
                    logger::warn("SpawnLeveledSpell: can't resolve LVLS {:X}",
                                d.spell->GetFormID());
                    continue;
                }
                mc->CastSpellImmediate(spell, false, target, 1.0f, false, 0.0f, ctx.source);
            }
        }
    }

    void SpawnLeveledSpellOnItem(const RuleContext& ctx,
                                const std::vector<LvlSpellSpawnData>& data)
    {
        if (!ctx.source || data.empty()) {
            logger::error("SpawnLeveledSpellOnItem: invalid source");
            return;
        }

        auto* caster = ctx.source;
        auto* target = ctx.target;

        if (!caster || caster->IsDeleted() || !target || target->IsDeleted()) {
            logger::warn("SpawnLeveledSpellOnItem: bad refs");
            return;
        }

        auto* mc = caster->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
        if (!mc) {
            logger::error("SpawnLeveledSpellOnItem: caster lacks MagicCaster");
            return;
        }

        for (const auto& d : data) {
            if (!d.spell) continue;

            for (std::uint32_t i = 0; i < d.count; ++i) {
                auto* spell = ResolveLeveledSpell(d.spell);
                if (!spell) {
                    logger::warn("SpawnLeveledSpellOnItem: can't resolve LVLS {:X}",
                                d.spell->GetFormID());
                    continue;
                }
                mc->CastSpellImmediate(spell, false, target, 1.0f, false, 0.0f, ctx.source);
            }
        }
    }

    void SpawnLeveledActor(const RuleContext& ctx, const std::vector<LvlActorSpawnData>& data)
    {
        if (!ctx.target || data.empty() || ctx.target->IsDeleted()) {
            logger::error("SpawnLeveledActor: invalid target");
            return;
        }

        NiPoint3 dropPos;
        if (auto* root = ctx.target->Get3D()) { 
            dropPos = root->worldBound.center;     
        } else {                                    
            const auto& bmin = ctx.target->GetBoundMin();
            const auto& bmax = ctx.target->GetBoundMax();
            dropPos = NiPoint3{
                (bmin.x + bmax.x) * 0.5f,
                (bmin.y + bmax.y) * 0.5f,
                (bmin.z + bmax.z) * 0.5f
            };
        }

        for (const auto& d : data) {
            if (!d.npc) continue;

            for (std::uint32_t i = 0; i < d.count; ++i) {
                auto* npcBase = ResolveLeveledNPC(d.npc);
                if (!npcBase) {
                    logger::warn("SpawnLeveledActor: can't resolve LVLC {:X}",
                                d.npc->GetFormID());
                    continue;
                }

                auto ref = ctx.target->PlaceObjectAtMe(npcBase, true);
                if (ref) {
                    ref->SetPosition(dropPos);
                }
            }
        }
    }

    void SwapLeveledActor(const RuleContext& ctx, const std::vector<LvlActorSpawnData>& data)
    {
        bool spawned = false;
        SpawnLeveledActor(ctx, data);

        {
            std::lock_guard lock(processedItemsMutex);
            spawned = !processedItems.empty();
        }

        if (spawned) {
            if (!ctx.target->IsDisabled())
                ctx.target->Disable();
            else
                ctx.target->SetDelete(true);
        }
    }
}   
