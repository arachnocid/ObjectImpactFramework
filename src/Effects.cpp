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
    void RemoveItem(const RuleContext& ctx)
    {
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::error("RemoveItem: No target to remove");
            return;
        }

        if (!ctx.target->IsDisabled()) ctx.target->Disable();
        ctx.target->SetDelete(true); 
    }

    void SpawnItem(const RuleContext& ctx, const std::vector<ItemSpawnData>& itemsData)
    {
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::error("SpawnItem: No target to spawn items");
            return;
        }

        if (itemsData.empty()) {
            logger::error("SpawnItem: No items to spawn");
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
        if (!ctx.source) {
            logger::error("SpawnSpell: No source to cast the spell");
            return;
        }

        if (spellsData.empty()) {
            logger::error("SpawnSpell: No spells to spawn");
            return;
        }

        // Determine caster and target
        // The target object is the source of the effect
        auto* caster = ctx.target;
        auto* target = ctx.source;

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

        std::vector<RE::Actor*> targets;

        RE::TES::GetSingleton()->ForEachReferenceInRange(caster, 350.0, [&](RE::TESObjectREFR* a_ref) {
            auto* actor = a_ref->As<RE::Actor>();
            if (actor && !actor->IsDead() && !actor->IsDisabled()) {
                targets.push_back(actor);
            }
            return RE::BSContainer::ForEachResult::kContinue;
        });

        if (targets.empty()) {
            return;
        }

        for (const auto& spellData : spellsData) {
            if (!spellData.spell) continue;

            for (auto* tgt : targets) {
                for (std::uint32_t i = 0; i < spellData.count; ++i) {
                    mc->CastSpellImmediate(spellData.spell, false, tgt, 1.0f, false, 0.0f, nullptr);
                }
            }
        }
    }

    void SpawnSpellOnItem(const RuleContext& ctx, const std::vector<SpellSpawnData>& spellsData)
    {
        if (!ctx.source) {
            logger::error("SpawnSpellOnItem: No source to cast the spell");
            return;
        }

        if (spellsData.empty()) {
            logger::error("SpawnSpellOnItem: No spells to spawn");
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
                mc->CastSpellImmediate(spellData.spell, false, target ? target : caster, 1.0f, false, 0.0f, nullptr);
            }
        }
    }

    void SpawnActor(const RuleContext& ctx, const std::vector<ActorSpawnData>& actorsData)
    {
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::error("SpawnActor: No target to spawn actors");
            return;
        }

        if (actorsData.empty()) {
            logger::error("SpawnActor: No actors to spawn");
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
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::error("SpawnImpact: No target to spawn impacts");
            return;
        }

        if (impactsData.empty()) {
            logger::error("SpawnImpact: No impacts to spawn");
            return;
        }

        auto* im = RE::BGSImpactManager::GetSingleton();
        if (!im)
            logger::error("SpawnImpact: Failed to get impact manager");
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
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::error("SpawnExplosion: No target to spawn explosions");
            return;
        }

        if (explosionsData.empty()) {
            logger::error("SpawnExplosion: No explosions to spawn");
            return;
        }

        for (const auto& explosionData : explosionsData) {
            if (!explosionData.explosion)
                continue;

            for (std::uint32_t i = 0; i < explosionData.count; ++i) {
                ctx.target->PlaceObjectAtMe(explosionData.explosion, true);
            }
        }
    }

    void SwapItem(const RuleContext& ctx, const std::vector<ItemSpawnData>& itemsData)
    {
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::error("SwapItem: No target to swap items");
            return;
        }

        if (itemsData.empty()) {
            logger::error("SwapItem: No items to swap with");
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
            if (!ctx.target->IsDisabled()) ctx.target->Disable();
            ctx.target->SetDelete(true); 
        }
    }

    void PlaySound(const RuleContext& ctx, const std::vector<SoundSpawnData>& soundsData)
    {
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::error("PlaySound: No target to play sound");
            return;
        }

        if (soundsData.empty()) {
            logger::error("PlaySound: No sounds to play");
            return;
        }

        RE::NiPoint3 pos;
        if (auto* root = ctx.target->Get3D())
            pos = root->worldBound.center;
        else
            pos = ctx.target->GetPosition();

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
                    handle.SetObjectToFollow(ctx.target->Get3D());
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
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::error("SwapActor: No target to swap actors");
            return;
        }

        if (actorsData.empty()) {
            logger::error("SwapActor: No actors to swap with");
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
            if (!ctx.target->IsDisabled()) ctx.target->Disable();
            ctx.target->SetDelete(true); 
        }
    }

    void SpawnLeveledItem(const RuleContext& ctx, const std::vector<LvlItemSpawnData>& itemsData)
    {
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::error("SpawnLeveledItem: No target to spawn/swap leveled items");
            return;
        }

        if (itemsData.empty()) {
            logger::error("SpawnLeveledItem: No leveled items to spawn/swap with");
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

        for (const auto& d : itemsData) {
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

    void SwapLeveledItem(const RuleContext& ctx, const std::vector<LvlItemSpawnData>& itemsData)
    {
        bool spawned = false;
        SpawnLeveledItem(ctx, itemsData);

        {
            std::lock_guard lock(processedItemsMutex);
            spawned = !processedItems.empty();
        }
        
        if (spawned) {
            if (!ctx.target->IsDisabled()) ctx.target->Disable();
            ctx.target->SetDelete(true); 
        }
    }

    void SpawnLeveledSpell(const RuleContext& ctx, const std::vector<LvlSpellSpawnData>& spellsData)
    {
        if (!ctx.source) {
            logger::error("SpawnLeveledSpell: No source to cast the spell");
            return;
        }

        if (spellsData.empty()) {
            logger::error("SpawnLeveledSpell: No spells to spawn");
            return;
        }

        auto* caster = ctx.target;
        auto* target = ctx.source;

        if (!target || target->IsDeleted()) {
            logger::warn("SpawnLeveledSpell: No valid target to cast the spell");
            return;
        }

        if (!caster || caster->IsDeleted()) {
            logger::warn("SpawnLeveledSpell: No valid source to cast the spell");
            return;
        }

        auto* mc = caster->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
        if (!mc) {
            logger::error("SpawnLeveledSpell: The caster has no MagicCaster");
            return;
        }

        std::vector<RE::Actor*> targets;

        RE::TES::GetSingleton()->ForEachReferenceInRange(caster, 350.0, [&](RE::TESObjectREFR* a_ref) {
            auto* actor = a_ref->As<RE::Actor>();
            if (actor && !actor->IsDead() && !actor->IsDisabled()) {
                targets.push_back(actor);
            }
            return RE::BSContainer::ForEachResult::kContinue;
        });

        if (targets.empty()) {
            return;
        }

        for (const auto& d : spellsData) {
            if (!d.spell) continue;

            auto* spell = ResolveLeveledSpell(d.spell);
            if (!spell) {
                logger::warn("SpawnLeveledSpell: Can't resolve LVLS {:X}", d.spell->GetFormID());
                continue;
            }

            for (auto* tgt : targets) {
                for (std::uint32_t i = 0; i < d.count; ++i) {
                    mc->CastSpellImmediate(spell, false, tgt, 1.0f, false, 0.0f, nullptr);
                }
            }
        }
    }

    void SpawnLeveledSpellOnItem(const RuleContext& ctx, const std::vector<LvlSpellSpawnData>& spellsData)
    {
        if (!ctx.source) {
            logger::error("SpawnLeveledSpellOnItem: No source to cast the spell");
            return;
        }

        if (spellsData.empty()) {
            logger::error("SpawnLeveledSpellOnItem: No spells to spawn");
            return;
        }

        auto* caster = ctx.source;
        auto* target = ctx.target;

        if (!target || target->IsDeleted()) {
            logger::warn("SpawnLeveledSpellOnItem: No valid target to cast the spell");
            return;
        }

        if (!caster || caster->IsDeleted()) {
            logger::warn("SpawnLeveledSpellOnItem: No valid source to cast the spell");
            return;
        }

        auto* mc = caster->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
        if (!mc) {
            logger::error("SpawnLeveledSpellOnItem: The caster has no MagicCaster");
            return;
        }

        for (const auto& d : spellsData) {
            if (!d.spell) continue;

            for (std::uint32_t i = 0; i < d.count; ++i) {
                auto* spell = ResolveLeveledSpell(d.spell);
                if (!spell) {
                    logger::warn("SpawnLeveledSpellOnItem: Can't resolve LVLS {:X}",
                                d.spell->GetFormID());
                    continue;
                }
                mc->CastSpellImmediate(spell, false, target, 1.0f, false, 0.0f, nullptr);
            }
        }
    }

    void SpawnLeveledActor(const RuleContext& ctx, const std::vector<LvlActorSpawnData>& actorsData)
    {
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::error("SpawnLeveledActor: No target to spawn actors");
            return;
        }

        if (actorsData.empty()) {
            logger::error("SpawnLeveledActor: No actors to spawn");
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

        for (const auto& d : actorsData) {
            if (!d.npc) continue;

            for (std::uint32_t i = 0; i < d.count; ++i) {
                auto* npcBase = ResolveLeveledNPC(d.npc);
                if (!npcBase) {
                    logger::warn("SpawnLeveledActor: Can't resolve LVLC {:X}",
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

    void SwapLeveledActor(const RuleContext& ctx, const std::vector<LvlActorSpawnData>& actorsData)
    {
        bool spawned = false;
        SpawnLeveledActor(ctx, actorsData);

        {
            std::lock_guard lock(processedItemsMutex);
            spawned = !processedItems.empty();
        }

        if (spawned) {
            if (!ctx.target->IsDisabled()) ctx.target->Disable();
            ctx.target->SetDelete(true); 
        }
    }

    void ApplyIngestible(const RuleContext& ctx)
    {
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::error("ApplyIngestible: No target to apply ingestible");
            return;
        }
        if (!ctx.source || ctx.source->IsDeleted()) {
            logger::error("ApplyIngestible: No source to apply ingestible");
            return;
        }
    
        auto* targetRef = ctx.target;
        auto* casterRef = ctx.source;
        auto* casterMC  = casterRef->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
        if (!casterMC) {
            logger::warn("ApplyIngestible: The source has no MagicCaster");
            return;
        }
    
        auto* magicItem = targetRef->GetBaseObject()->As<RE::MagicItem>();
        if (!magicItem) {
            logger::warn("ApplyIngestible: The target is not a MagicItem");
            return;
        }

        const bool hostile = magicItem->IsPoison();

        RE::TES::GetSingleton()->ForEachReferenceInRange(targetRef, 150.0, [&](RE::TESObjectREFR* a_ref) {
            auto* actor = a_ref->As<RE::Actor>();
            if (actor && !actor->IsDisabled() && !actor->IsDeleted() && !actor->IsDead() && !actor->IsGhost()) {
                casterMC->CastSpellImmediate(magicItem, false, actor, 1.0f, hostile, 0.0f, nullptr);
            }
            return RE::BSContainer::ForEachResult::kContinue;
        });
    }

    void ApplyOtherIngestible(const RuleContext& ctx, const std::vector<IngestibleApplyData>& ingestiblesData)
    {
        if (ingestiblesData.empty()) {
            logger::error("ApplyOtherIngestible: No ingestibles to apply");
            return;
        }

        if (!ctx.source || ctx.source->IsDeleted()) {
            logger::error("ApplyOtherIngestible: No source to apply ingestible");
            return;
        }
        
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::error("ApplyOtherIngestible: No target to apply ingestible");
            return;
        }

        auto* targetRef = ctx.target;
        auto* casterRef = ctx.source;
        auto* casterMC  = casterRef->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
        if (!casterMC) {
            logger::warn("ApplyOtherIngestible: The source has no MagicCaster");
            return;
        }

        std::vector<RE::Actor*> targets;
    
        // Include the caster if it's a valid actor
        if (auto* casterActor = casterRef->As<RE::Actor>(); casterActor && !casterActor->IsDead() && !casterActor->IsDisabled()) {
            targets.push_back(casterActor);
        }
    
        RE::TES::GetSingleton()->ForEachReferenceInRange(targetRef, 150.0, [&](RE::TESObjectREFR* a_ref) {
            if (auto* actor = a_ref->As<RE::Actor>(); actor && !actor->IsDead() && !actor->IsDisabled()) {
                targets.push_back(actor);
            }
            return RE::BSContainer::ForEachResult::kContinue;
        });
    
        if (targets.empty()) {
            return;
        }
    
        for (const auto& pd : ingestiblesData) {
            if (!pd.ingestible) continue;
    
            const bool hostile = pd.ingestible->IsPoison();
    
            for (auto* tgt : targets) {
                for (std::uint32_t i = 0; i < pd.count; ++i) {
                    casterMC->CastSpellImmediate(pd.ingestible, false, tgt, 1.0f, hostile, 0.0f, nullptr);
                }
            }
        }
    }

    void SpawnLight(const RuleContext& ctx, const std::vector<LightSpawnData>& lightsData)
    {
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::error("SpawnLight: No target to spawn lights");
            return;
        }

        if (lightsData.empty()) {
            logger::error("SpawnLight: No lights to spawn");
            return;
        }

        NiPoint3 pos;
        if (auto* root = ctx.target->Get3D()) {
            pos = root->worldBound.center;
        } else {
            const auto& bmin = ctx.target->GetBoundMin();
            const auto& bmax = ctx.target->GetBoundMax();
            pos = NiPoint3{
                (bmin.x + bmax.x) * 0.5f,
                (bmin.y + bmax.y) * 0.5f,
                (bmin.z + bmax.z) * 0.5f
            };
        }

        for (const auto& lightData : lightsData) {
            if (!lightData.light)
                continue;

            for (std::uint32_t i = 0; i < lightData.count; ++i) {
                RE::NiPointer<RE::TESObjectREFR> ref = ctx.target->PlaceObjectAtMe(lightData.light, true);
                if (ref) {
                    ref->SetPosition(pos);
                    ref->Enable(false);
                }
            }
        }
    }

    void RemoveLight(const RuleContext& ctx, const std::vector<LightRemoveData>& lightsData)
    {
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::error("RemoveLight: No target to remove light around");
            return;
        }

        if (lightsData.empty()) {
            logger::warn("RemoveLight: No lights to remove");
            return;
        }

        auto* tes = RE::TES::GetSingleton();
        if (!tes)
            return;

        for (const auto& data : lightsData) {
            if (data.radius <= 0)
                continue;

            tes->ForEachReferenceInRange(ctx.target, static_cast<float>(data.radius), [&](RE::TESObjectREFR* ref) {
                    if (!ref || ref->IsDisabled() || ref->IsDeleted())
                        return RE::BSContainer::ForEachResult::kContinue;
    
                    auto* baseObj = ref->GetBaseObject();
                    if (baseObj && baseObj->Is(RE::FormType::Light)) {
                        if (!ref->IsDisabled()) ref->Disable();
                        ref->SetDelete(true); 
                    }
                    return RE::BSContainer::ForEachResult::kContinue;
                }
            );
        }
    }

    void EnableLight(const RuleContext& ctx, const std::vector<LightRemoveData>& lightsData)
    {
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::error("EnableLight: No target to enable light around");
            return;
        }

        if (lightsData.empty()) {
            logger::warn("EnableLight: No lights to enable");
            return;
        }

        auto* tes = RE::TES::GetSingleton();
        if (!tes)
            return;

        for (const auto& data : lightsData) {
            if (data.radius <= 0)
                continue;

            tes->ForEachReferenceInRange(ctx.target, static_cast<float>(data.radius), [&](RE::TESObjectREFR* ref) {
                    if (!ref || ref->IsDisabled() || ref->IsDeleted())
                        return RE::BSContainer::ForEachResult::kContinue;
    
                    auto* baseObj = ref->GetBaseObject();
                    if (baseObj && baseObj->Is(RE::FormType::Light)) {
                        if (ref->IsDisabled()) ref->Enable(false);
                    }
                    return RE::BSContainer::ForEachResult::kContinue;
                }
            );
        }
    }

    void DisableLight(const RuleContext& ctx, const std::vector<LightRemoveData>& lightsData)
    {
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::error("DisableLight: No target to disable light around");
            return;
        }

        if (lightsData.empty()) {
            logger::warn("DisableLight: No lights to disable");
            return;
        }

        auto* tes = RE::TES::GetSingleton();
        if (!tes)
            return;

        for (const auto& data : lightsData) {
            if (data.radius <= 0)
                continue;

            tes->ForEachReferenceInRange(ctx.target, static_cast<float>(data.radius), [&](RE::TESObjectREFR* ref) {
                    if (!ref || ref->IsDisabled() || ref->IsDeleted())
                        return RE::BSContainer::ForEachResult::kContinue;
    
                    auto* baseObj = ref->GetBaseObject();
                    if (baseObj && baseObj->Is(RE::FormType::Light)) {
                        if (!ref->IsDisabled()) ref->Disable();
                    }
                    return RE::BSContainer::ForEachResult::kContinue;
                }
            );
        }
    }
}