#include "Effects.h"
#include <future>

namespace OIF::Effects
{
    // ------------------ Helpers ------------------
    /*std::unordered_map<std::string, RE::BSShaderProperty::EShaderPropertyFlag> g_shaderFlagMap = {
        // Shader Flags 1
        {"specular", RE::BSShaderProperty::EShaderPropertyFlag::kSpecular},
        {"skinned", RE::BSShaderProperty::EShaderPropertyFlag::kSkinned},
        {"temp_refraction", RE::BSShaderProperty::EShaderPropertyFlag::kTempRefraction},
        {"vertex_alpha", RE::BSShaderProperty::EShaderPropertyFlag::kVertexAlpha},
        {"grayscale_to_palette_color", RE::BSShaderProperty::EShaderPropertyFlag::kGrayscaleToPaletteColor},
        {"grayscale_to_palette_alpha", RE::BSShaderProperty::EShaderPropertyFlag::kGrayscaleToPaletteAlpha},
        {"falloff", RE::BSShaderProperty::EShaderPropertyFlag::kFalloff},
        {"env_map", RE::BSShaderProperty::EShaderPropertyFlag::kEnvMap},
        {"receive_shadows", RE::BSShaderProperty::EShaderPropertyFlag::kReceiveShadows},
        {"cast_shadows", RE::BSShaderProperty::EShaderPropertyFlag::kCastShadows},
        {"face", RE::BSShaderProperty::EShaderPropertyFlag::kFace},
        {"parallax", RE::BSShaderProperty::EShaderPropertyFlag::kParallax},
        {"model_space_normals", RE::BSShaderProperty::EShaderPropertyFlag::kModelSpaceNormals},
        {"non_projective_shadows", RE::BSShaderProperty::EShaderPropertyFlag::kNonProjectiveShadows},
        {"multi_texture_landscape", RE::BSShaderProperty::EShaderPropertyFlag::kMultiTextureLandscape},
        {"refraction", RE::BSShaderProperty::EShaderPropertyFlag::kRefraction},
        {"refraction_falloff", RE::BSShaderProperty::EShaderPropertyFlag::kRefractionFalloff},
        {"eye_reflect", RE::BSShaderProperty::EShaderPropertyFlag::kEyeReflect},
        {"hair_tint", RE::BSShaderProperty::EShaderPropertyFlag::kHairTint},
        {"screendoor_alpha_fade", RE::BSShaderProperty::EShaderPropertyFlag::kScreendoorAlphaFade},
        {"local_map_clear", RE::BSShaderProperty::EShaderPropertyFlag::kLocalMapClear},
        {"face_gen_rgb_tint", RE::BSShaderProperty::EShaderPropertyFlag::kFaceGenRGBTint},
        {"own_emit", RE::BSShaderProperty::EShaderPropertyFlag::kOwnEmit},
        {"projected_uv", RE::BSShaderProperty::EShaderPropertyFlag::kProjectedUV},
        {"multiple_textures", RE::BSShaderProperty::EShaderPropertyFlag::kMultipleTextures},
        {"remappable_textures", RE::BSShaderProperty::EShaderPropertyFlag::kRemappableTextures},
        {"decal", RE::BSShaderProperty::EShaderPropertyFlag::kDecal},
        {"dynamic_decal", RE::BSShaderProperty::EShaderPropertyFlag::kDynamicDecal},
        {"parallax_occlusion", RE::BSShaderProperty::EShaderPropertyFlag::kParallaxOcclusion},
        {"external_emittance", RE::BSShaderProperty::EShaderPropertyFlag::kExternalEmittance},
        {"soft_effect", RE::BSShaderProperty::EShaderPropertyFlag::kSoftEffect},
        {"z_buffer_test", RE::BSShaderProperty::EShaderPropertyFlag::kZBufferTest},
        
        // Shader Flags 2
        {"z_buffer_write", RE::BSShaderProperty::EShaderPropertyFlag::kZBufferWrite},
        {"lod_landscape", RE::BSShaderProperty::EShaderPropertyFlag::kLODLandscape},
        {"lod_objects", RE::BSShaderProperty::EShaderPropertyFlag::kLODObjects},
        {"no_fade", RE::BSShaderProperty::EShaderPropertyFlag::kNoFade},
        {"two_sided", RE::BSShaderProperty::EShaderPropertyFlag::kTwoSided},
        {"vertex_colors", RE::BSShaderProperty::EShaderPropertyFlag::kVertexColors},
        {"glow_map", RE::BSShaderProperty::EShaderPropertyFlag::kGlowMap},
        {"assume_shadowmask", RE::BSShaderProperty::EShaderPropertyFlag::kAssumeShadowmask},
        {"character_lighting", RE::BSShaderProperty::EShaderPropertyFlag::kCharacterLighting},
        {"multi_index_snow", RE::BSShaderProperty::EShaderPropertyFlag::kMultiIndexSnow},
        {"vertex_lighting", RE::BSShaderProperty::EShaderPropertyFlag::kVertexLighting},
        {"uniform_scale", RE::BSShaderProperty::EShaderPropertyFlag::kUniformScale},
        {"fit_slope", RE::BSShaderProperty::EShaderPropertyFlag::kFitSlope},
        {"billboard", RE::BSShaderProperty::EShaderPropertyFlag::kBillboard},
        {"no_lod_land_blend", RE::BSShaderProperty::EShaderPropertyFlag::kNoLODLandBlend},
        {"envmap_light_fade", RE::BSShaderProperty::EShaderPropertyFlag::kEnvmapLightFade},
        {"wireframe", RE::BSShaderProperty::EShaderPropertyFlag::kWireframe},
        {"weapon_blood", RE::BSShaderProperty::EShaderPropertyFlag::kWeaponBlood},
        {"hide_on_local_map", RE::BSShaderProperty::EShaderPropertyFlag::kHideOnLocalMap},
        {"premult_alpha", RE::BSShaderProperty::EShaderPropertyFlag::kPremultAlpha},
        {"cloud_lod", RE::BSShaderProperty::EShaderPropertyFlag::kCloudLOD},
        {"anisotropic_lighting", RE::BSShaderProperty::EShaderPropertyFlag::kAnisotropicLighting},
        {"no_transparency_multisample", RE::BSShaderProperty::EShaderPropertyFlag::kNoTransparencyMultiSample},
        {"menu_screen", RE::BSShaderProperty::EShaderPropertyFlag::kMenuScreen},
        {"multi_layer_parallax", RE::BSShaderProperty::EShaderPropertyFlag::kMultiLayerParallax},
        {"soft_lighting", RE::BSShaderProperty::EShaderPropertyFlag::kSoftLighting},
        {"rim_lighting", RE::BSShaderProperty::EShaderPropertyFlag::kRimLighting},
        {"back_lighting", RE::BSShaderProperty::EShaderPropertyFlag::kBackLighting},
        {"snow", RE::BSShaderProperty::EShaderPropertyFlag::kSnow},
        {"tree_anim", RE::BSShaderProperty::EShaderPropertyFlag::kTreeAnim},
        {"effect_lighting", RE::BSShaderProperty::EShaderPropertyFlag::kEffectLighting},
        {"hd_lod_objects", RE::BSShaderProperty::EShaderPropertyFlag::kHDLODObjects}
    };*/

    static void CollectNodes(RE::NiNode* root, const std::vector<std::string>& nodeNames, std::vector<RE::NiNode*>& out)
    {
        if (!root || nodeNames.empty()) return;
    
        std::unordered_set<RE::NiNode*> visited;
    
        auto isNodeMatchingPattern = [&](RE::NiAVObject* node) -> bool
        {
            if (!node || !node->name.c_str()) return false;
    
            std::string nName{ node->name.c_str() };
            for (const auto& pattern : nodeNames) {
                if (std::search(nName.begin(), nName.end(), 
                            pattern.begin(), pattern.end(),
                            [](char a, char b) { 
                                return std::tolower(static_cast<unsigned char>(a)) == std::tolower(static_cast<unsigned char>(b)); 
                            }) != nName.end())
                    return true;
            }
            return false;
        };
    
        std::function<void(RE::NiNode*, int)> dfs = [&](RE::NiNode* node, int depth)
        {
            if (!node || depth > 100) return;
            
            if (visited.find(node) != visited.end()) return;
            visited.insert(node);
    
            if (isNodeMatchingPattern(node)) out.emplace_back(node);
    
            auto& children = node->GetChildren();
            for (auto const& child : children)
            {
                if (child && child->AsNode()) dfs(child->AsNode(), depth + 1);
            }
            
            visited.erase(node);
        };
    
        dfs(root, 0);
    }

    /*static void CollectTriShapes(RE::NiNode* root, const std::vector<std::string>& strings, std::vector<RE::BSGeometry*>& out)
    {
        if (!root) return;
        
        bool collectAll = strings.empty();
        
        std::function<void(RE::NiAVObject*)> traverse = [&](RE::NiAVObject* obj) {
            if (!obj) return;
            
            if (auto* geometry = skyrim_cast<RE::BSGeometry*>(obj)) {
                bool shouldAdd = collectAll;
                
                if (!collectAll && obj->name.c_str()) {
                    std::string objName = obj->name.c_str();
                    for (const auto& pattern : strings) {
                        if (objName.find(pattern) != std::string::npos) {
                            shouldAdd = true;
                            break;
                        }
                    }
                }
                
                if (shouldAdd) {
                    out.push_back(geometry);
                }
            }
            
            if (auto* node = obj->AsNode()) {
                auto& children = node->GetChildren();
                for (auto& child : children) {
                    if (child) {
                        traverse(child.get());
                    }
                }
            }
        };
        
        traverse(root);
    }*/

    // Copy ownership from one reference to another
    void CopyOwnership(RE::TESObjectREFR* from, RE::TESObjectREFR* to)
    {
        if (!from || !to || !from->GetOwner()) return;

        if (auto* owner = from->GetOwner()) {
            to->SetOwner(owner);
        }
    }

    // Execute a list of commands in the console - taken and adapted from the ConsoleUtil NG source code
    template <typename... Args>
    void ExecuteCommand(const std::string& command, RE::TESObjectREFR* targetRef = nullptr, Args... args) {
        const auto scriptFactory = RE::IFormFactory::GetConcreteFormFactoryByType<RE::Script>();
        const auto script = scriptFactory ? scriptFactory->Create() : nullptr;
        
        if (!script) {
            logger::error("Failed to create script object");
            return;
        }
    
        std::string formattedCommand;
        if constexpr (sizeof...(args) > 0) {
            formattedCommand = std::vformat(command, std::make_format_args(args...));
        } else {
            formattedCommand = command;
        }
        
        script->SetCommand(formattedCommand);
        
        using func_t = void(RE::Script*, RE::ScriptCompiler*, RE::COMPILER_NAME, RE::TESObjectREFR*);
        REL::Relocation<func_t> compileAndRun{
            RELOCATION_ID(21416, REL::Module::get().version().patch() < 1130 ? 21890 : 441582)
        };
    
        RE::ScriptCompiler compiler;
        compileAndRun(script, &compiler, RE::COMPILER_NAME::kSystemWindowCompiler, targetRef);
    
        delete script;
    }

    // CommonlibSSE-NG doesn't have a SetScale function, that's a workaround
    void SetObjectScale(RE::TESObjectREFR* ref, float scale) {
        if (!ref) return;
        ExecuteCommand("setscale " + std::to_string(scale), ref);
        ExecuteCommand("", nullptr);
    }

    // Same as above, but for playing idle animations on actors
    void PlayIdleOnActor(RE::Actor* actor, const std::string& idleName) {
        if (!actor) return;
        ExecuteCommand("sendanimevent " + idleName, actor);
        ExecuteCommand("", nullptr);
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

    inline NiPoint3 GetObjectCenter(RE::TESObjectREFR* target) {
        if (!target) return NiPoint3{0.0f, 0.0f, 0.0f};
        
        if (auto* root = target->Get3D()) {
            return root->worldBound.center;
        }
        
        const auto& bmin = target->GetBoundMin();
        const auto& bmax = target->GetBoundMax();
        return NiPoint3{
            (bmin.x + bmax.x) * 0.5f,
            (bmin.y + bmax.y) * 0.5f,
            (bmin.z + bmax.z) * 0.5f
        };
    }

    inline NiPoint3 GetObjectBottom(RE::TESObjectREFR* target) {
        if (!target) return NiPoint3{0.0f, 0.0f, 0.0f};
        
        if (auto* root = target->Get3D()) {
            const auto& center = root->worldBound.center;
            const auto& radius = root->worldBound.radius;
            return NiPoint3{center.x, center.y, center.z - radius};
        }
        
        const auto& bmin = target->GetBoundMin();
        const auto& bmax = target->GetBoundMax();
        return NiPoint3{
            (bmin.x + bmax.x) * 0.5f,
            (bmin.y + bmax.y) * 0.5f,
            bmin.z
        };
    }
    
    inline NiPoint3 GetObjectTop(RE::TESObjectREFR* target) {
        if (!target) return NiPoint3{0.0f, 0.0f, 0.0f};
        
        if (auto* root = target->Get3D()) {
            const auto& center = root->worldBound.center;
            const auto& radius = root->worldBound.radius;
            return NiPoint3{center.x, center.y, center.z + radius};
        }
        
        const auto& bmin = target->GetBoundMin();
        const auto& bmax = target->GetBoundMax();
        return NiPoint3{
            (bmin.x + bmax.x) * 0.5f,
            (bmin.y + bmax.y) * 0.5f,
            bmax.z
        };
    }
    
    inline void GetToNearestNavmesh(RE::TESObjectREFR* dummy, float verticalOffset = 1.0f) {
        if (!dummy || dummy->IsDeleted() || !dummy->GetParentCell()) {
            logger::warn("GetToNearestNavmesh: Invalid ref, dummy, or cell");
            return;
        }
    
        const auto cell = dummy->GetParentCell();
        const auto& runtimeData = cell->GetRuntimeData();
        if (!runtimeData.navMeshes) {
            logger::warn("GetToNearestNavmesh: No navmeshes in cell");
            return;
        }
    
        const auto dummyPos = dummy->GetPosition();
    
        // Taken and adapted from the Papyrus Extender source code
        auto& navMeshes = runtimeData.navMeshes->navMeshes;
        auto shortestDistance = (std::numeric_limits<float>::max)();
        std::optional<RE::NiPoint3> nearestNavmeshPos = std::nullopt;
    
        int navmeshCount = 0;
        int vertexCount = 0;
    
        for (const auto& navMesh : navMeshes) {
            if (!navMesh) continue;
            navmeshCount++;
            
            for (auto& [location] : navMesh->vertices) {
                vertexCount++;
                const auto linearDistance = dummyPos.GetDistance(location);
                if (linearDistance < shortestDistance) {
                    shortestDistance = linearDistance;
                    nearestNavmeshPos.emplace(location);
                }
            }
        }
    
        if (nearestNavmeshPos) {
            const RE::NiPoint3 finalPos = {
                dummyPos.x, 
                dummyPos.y,      
                nearestNavmeshPos->z + verticalOffset 
            };

            dummy->SetPosition(finalPos);
        } else {
            logger::warn("GetToNearestNavmesh: Navmesh position not found");
        }
    }

    RE::NiPointer<RE::TESObjectREFR> Spawn(RE::TESObjectREFR* target, RE::TESBoundObject* item, std::uint32_t type, std::uint32_t fade) {
        if (!target || !item) {
            logger::error("Spawn: Invalid target or item pointer");
            return nullptr;
        }
    
        if (target->IsDeleted() || !target->GetParentCell()) {
            logger::error("Spawn: Target is deleted or has no parent cell");
            return nullptr;
        }
    
        // Helper lambda for fade application
        auto ApplyFade = [](RE::NiPointer<RE::TESObjectREFR> ref) {
            if (!ref || ref->IsDeleted()) {
                return;
            }
                        
            try {
                ref->formFlags |= static_cast<std::uint32_t>(RE::TESObjectREFR::RecordFlags::kNeverFades);
            } catch (...) {
                logger::error("Spawn: Exception while applying fade to spawned object");
            }
        };
    
        // For types <= 3, just place the item directly
        if (type <= 3) {
            auto spawnedItem = target->PlaceObjectAtMe(item, true);
            if (spawnedItem) {
                if (spawnedItem->IsDeleted()) {
                    logger::warn("Spawn: Spawned item was deleted immediately");
                    return nullptr;
                }
                
                if (!spawnedItem->IsDeleted() && spawnedItem->GetParentCell()) {
                    try {
                        switch (type) {
                            case 1: spawnedItem->SetPosition(GetObjectCenter(target));  break;
                            case 2: spawnedItem->SetPosition(GetObjectTop(target));     break;
                            case 3: spawnedItem->SetPosition(GetObjectBottom(target));  break;
                            default: break;
                        }
                    } catch (...) {
                        logger::error("Spawn: Exception while setting spawned item position");
                    }
                    
                    if (fade == 0) {
                        ApplyFade(spawnedItem);
                    }
                }
            }
            return spawnedItem;
        }
    
        static RE::TESBoundObject* dummyForm = nullptr;
        static std::mutex dummyFormMutex;
        static bool dummyFormInitialized = false;
        
        {
            // Try to get the dummy
            std::lock_guard<std::mutex> lock(dummyFormMutex);
            if (!dummyFormInitialized) {
                dummyFormInitialized = true;
                auto* dh = RE::TESDataHandler::GetSingleton();
                if (dh) {
                    auto* form = dh->LookupForm(0x000B79FF, "Skyrim.esm");
                    if (form) {
                        dummyForm = form->As<RE::TESBoundObject>();
                        if (!dummyForm) {
                            logger::error("Spawn: Failed to cast dummy form to TESBoundObject");
                        }
                    } else {
                        logger::error("Spawn: Failed to lookup dummy form 0x000B79FF");
                    }
                } else {
                    logger::error("Spawn: TESDataHandler singleton is null");
                }
            }
        }
    
        if (!dummyForm) {
            logger::warn("Spawn: Dummy form unavailable, falling back to direct spawn");
            auto spawned = target->PlaceObjectAtMe(item, true);
            if (spawned && !spawned->IsDeleted()) {
                if (fade == 0) {
                    ApplyFade(spawned);
                }
                return spawned;
            }
        }
    
        // Create the dummy
        auto dummy = target->PlaceObjectAtMe(dummyForm, false);
        if (!dummy) {
            logger::warn("Spawn: Failed to create dummy, falling back to direct spawn");
            auto spawned = target->PlaceObjectAtMe(item, true);
            if (spawned && !spawned->IsDeleted()) {
                if (fade == 0) {
                    ApplyFade(spawned);
                }
                return spawned;
            }
        }
    
        // Validate dummy state before proceeding
        if (dummy->IsDeleted() || !dummy->GetParentCell() || dummy->IsDisabled()) {
            logger::warn("Spawn: Dummy is in invalid state, cleaning up and falling back");
            if (dummy && !dummy->IsDeleted()) {
                try {
                    dummy->SetDelete(true);
                } catch (...) {
                    logger::error("Spawn: Exception while deleting dummy");
                }
            }
            auto spawned = target->PlaceObjectAtMe(item, true);
            if (spawned && !spawned->IsDeleted()) {
                if (fade == 0) {
                    ApplyFade(spawned);
                }
                return spawned;
            }
        }
    
        // Move the dummy to the target node
        if (auto targetObject = target->Get3D()) {
            if (auto node = targetObject->AsNode()) {
                if (dummy && !dummy->IsDeleted() && dummy->GetParentCell() && 
                    target && !target->IsDeleted() && target->GetParentCell()) {
                    try {
                        dummy->MoveToNode(target, node);
                        dummy->data.angle = target->data.angle;
                    } catch (...) {
                        logger::error("Spawn: Exception while moving dummy to node");
                    }
                }
            }
        }
    
        // Position the dummy based on type
        if (dummy && !dummy->IsDeleted() && dummy->GetParentCell()) {
            try {
                switch (type) {
                    case 5: dummy->SetPosition(GetObjectCenter(target));  break;
                    case 6: dummy->SetPosition(GetObjectTop(target));     break;
                    case 7: dummy->SetPosition(GetObjectBottom(target));  break;
                    case 8: {
                        auto dummyPos = dummy->GetPosition();
                        GetToNearestNavmesh(dummy.get(), 1.0f);
                        dummy->data.angle = RE::NiPoint3{0.0f, 0.0f, 0.0f};
                        break;
                    }
                }
            } catch (...) {
                logger::error("Spawn: Exception while setting dummy position");
            }
        }
    
        // Spawn the real item from dummy
        RE::NiPointer<RE::TESObjectREFR> spawned;
        if (dummy && !dummy->IsDeleted() && dummy->GetParentCell()) {
            try {
                spawned = dummy->PlaceObjectAtMe(item, true);
            } catch (...) {
                logger::error("Spawn: Exception while spawning item from dummy");
                spawned = nullptr;
            }
        }
    
        if (!spawned || spawned->IsDeleted()) {
            try {
                spawned = target->PlaceObjectAtMe(item, true);
            } catch (...) {
                logger::error("Spawn: Exception during fallback direct spawn");
                spawned = nullptr;
            }
        }
    
        if (fade == 0 && spawned && !spawned->IsDeleted()) {
            ApplyFade(spawned);
        }
    
        if (dummy && !dummy->IsDeleted()) {
            try {
                dummy->Disable();
                dummy->SetDelete(true);
            } catch (...) {
                logger::error("Spawn: Exception while cleaning up dummy");
            }
        }
    
        return spawned;
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

    void DisableItem(const RuleContext& ctx)
    {
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::error("DisableItem: No target to disable");
            return;
        }

        if (!ctx.target->IsDisabled()) ctx.target->Disable();
    }

    void EnableItem(const RuleContext& ctx)
    {
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::error("EnableItem: No target to enable");
            return;
        }

        if (ctx.target->IsDisabled()) ctx.target->Enable(false);
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

        for (const auto& itemData : itemsData) {
            if (!itemData.item)
                continue;

            for (std::uint32_t i = 0; i < itemData.count; ++i) {
                auto item = Spawn(ctx.target, itemData.item, itemData.spawnType, itemData.fade);
                if (item && ctx.target) {
                    CopyOwnership(ctx.target, item.get());
                    if (itemData.scale > -1.0f && ctx.target) {
                        SetObjectScale(item.get(), ctx.target->GetScale());
                    }
                }
            }
        }
    }

    void SpawnSpell(const RuleContext& ctx, const std::vector<SpellSpawnData>& spellsData)
    {
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::warn("SpawnSpell: No valid target location for dummy caster");
            return;
        }

        if (spellsData.empty()) {
            logger::error("SpawnSpell: No spells to spawn");
            return;
        }
    
        static RE::TESBoundObject* dummyForm = nullptr;
        if (!dummyForm) {
            auto* dh = RE::TESDataHandler::GetSingleton();
            auto* form = dh ? dh->LookupForm(0x000B79FF, "Skyrim.esm") : nullptr;
            dummyForm = form ? form->As<RE::TESBoundObject>() : nullptr;
        }
    
        if (!dummyForm) {
            logger::error("SpawnSpell: Cannot create dummy caster");
            return;
        }
    
        auto dummy = ctx.target->PlaceObjectAtMe(dummyForm, true);
        if (!dummy) {
            logger::error("SpawnSpell: Failed to create dummy caster");
            return;
        }
    
        if (auto targetObject = ctx.target->Get3D()) {
            if (auto node = targetObject->AsNode()) {
                dummy->MoveToNode(ctx.target, node);
            }
        }
    
        auto* mc = dummy->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
        if (!mc) {
            logger::error("SpawnSpell: Dummy has no MagicCaster");
            dummy->Disable();
            dummy->SetDelete(true);
            return;
        }

        auto* tes = RE::TES::GetSingleton();
        if (!tes) {
            logger::error("SpawnLeveledSpell: Cannot get TES singleton");
            if (dummy) {
                dummy->Disable();
                dummy->SetDelete(true);
                return;
            }
        }
    
        for (const auto& spellData : spellsData) {
            if (!spellData.spell) continue;
            if (spellData.radius <= 0) return;
    
            std::vector<RE::Actor*> targets;

            tes->ForEachReferenceInRange(dummy.get(), spellData.radius, [&](RE::TESObjectREFR* a_ref) {
                auto* actor = a_ref->As<RE::Actor>();
                if (actor && !actor->IsDead() && !actor->IsDisabled()) {
                    targets.push_back(actor);
                }
                return RE::BSContainer::ForEachResult::kContinue;
            });
    
            if (targets.empty()) {
                continue;
            }
    
            for (auto* tgt : targets) {
                for (std::uint32_t i = 0; i < spellData.count; ++i) {
                    if (!mc || !tgt || tgt->IsDeleted() || !spellData.spell) {
                        logger::warn("SpawnSpell: Invalid MagicCaster, target, or spell");
                        continue;
                    }
                    mc->CastSpellImmediate(spellData.spell, false, tgt, 1.0f, false, 0.0f, nullptr);
                }
            }
        }

        if (!dummy) {
            logger::error("SpawnSpell: Dummy caster is null after casting");
            return;
        }
    
        dummy->Disable();
        dummy->SetDelete(true);
    }

    void SpawnSpellOnItem(const RuleContext& ctx, const std::vector<SpellSpawnData>& spellsData)
    {
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::warn("SpawnSpellOnItem: No valid target to cast the spell on");
            return;
        }
    
        if (spellsData.empty()) {
            logger::error("SpawnSpellOnItem: No spells to spawn");
            return;
        }
    
        static RE::TESBoundObject* dummyForm = nullptr;
        if (!dummyForm) {
            auto* dh = RE::TESDataHandler::GetSingleton();
            auto* form = dh ? dh->LookupForm(0x000B79FF, "Skyrim.esm") : nullptr;
            dummyForm = form ? form->As<RE::TESBoundObject>() : nullptr;
        }
    
        if (!dummyForm) {
            logger::error("SpawnSpellOnItem: Cannot create dummy for spell casting");
            return;
        }
    
        auto dummy = ctx.target->PlaceObjectAtMe(dummyForm, true);
        if (!dummy) {
            logger::error("SpawnSpellOnItem: Failed to create dummy");
            return;
        }
    
        if (auto targetObject = ctx.target->Get3D()) {
            if (auto node = targetObject->AsNode()) {
                dummy->MoveToNode(ctx.target, node);
            }
        }
    
        auto* mc = dummy->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
        if (!mc) {
            logger::error("SpawnSpellOnItem: Dummy has no MagicCaster");
            if (dummy) {
                dummy->Disable();
                dummy->SetDelete(true);
            }
            return;
        }
    
        for (const auto& spellData : spellsData) {
            if (!spellData.spell) continue;
    
            for (std::uint32_t i = 0; i < spellData.count; ++i) {
                if (!mc || !ctx.target || ctx.target->IsDeleted() || !spellData.spell) {
                    logger::warn("SpawnSpellOnItem: Invalid MagicCaster, target, or spell");
                    continue;
                }
                mc->CastSpellImmediate(spellData.spell, false, ctx.target, 1.0f, false, 0.0f, nullptr);
            }
        }

        if (!dummy) {
            logger::error("SpawnSpellOnItem: Dummy caster is null after casting");
            return;
        }
    
        dummy->Disable();
        dummy->SetDelete(true);
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

        for (const auto& actorData : actorsData) {
            if (!actorData.npc)
                continue;

            for (std::uint32_t i = 0; i < actorData.count; ++i) {
                auto actor = Spawn(ctx.target, actorData.npc, actorData.spawnType, actorData.fade);
                if (actor && ctx.target) {
                    if (actorData.scale > -1.0f) {
                        SetObjectScale(actor.get(), ctx.target->GetScale());
                    }
                }
            }
        }
    }

    // A placeholder. Haven't find a decent way to spawn pure impacts yet
    void SpawnImpact(const RuleContext& ctx, const std::vector<ImpactSpawnData>& impactsData) {
        (void)ctx;
        (void)impactsData;
        return;
    }
    
    void SpawnImpactDataSet(const RuleContext& ctx, const std::vector<ImpactDataSetSpawnData>& impactsData)
    {
        if (!ctx.target || ctx.target->IsDeleted() || impactsData.empty()) {
            logger::error("SpawnImpactDataSet: No target or impacts data to spawn");
            return;
        }   

        auto* im = RE::BGSImpactManager::GetSingleton();
        if (!im) {
            logger::error("SpawnImpactDataSet: BGSImpactManager is null");
            return;
        }

        auto* sourceActor = ctx.source ? ctx.source->As<RE::Actor>() : nullptr;
        if (!sourceActor) {
            auto* player = RE::PlayerCharacter::GetSingleton();
            if (!player) {
                logger::error("SpawnImpactDataSet: No source actor or player found");
                return;
            }
            sourceActor = player;
        }

        static RE::TESBoundObject* dummyForm = nullptr;
        if (!dummyForm) {
            auto* dh = RE::TESDataHandler::GetSingleton();
            auto* form = dh ? dh->LookupForm(0x000B79FF, "Skyrim.esm") : nullptr;
            dummyForm = form ? form->As<RE::TESBoundObject>() : nullptr;
        }

        if (!dummyForm) {
            logger::error("SpawnImpactDataSet: Cannot create dummy");
            return;
        }

        auto dummy = ctx.target->PlaceObjectAtMe(dummyForm, false);
        if (!dummy) {
            logger::error("SpawnImpactDataSet: Failed to create dummy");
            return;
        }

        if (auto targetObject = ctx.target->Get3D()) {
            if (auto node = targetObject->AsNode()) {
                dummy->MoveToNode(ctx.target, node);
            }
        }

        RE::NiPoint3 hitPos;
        bool hitPosFound = false;
        
        if (sourceActor->IsPlayerRef()) {
            auto* crosshairData = RE::CrosshairPickData::GetSingleton();
            if (crosshairData && crosshairData->target->get().get() == ctx.target) {
                auto crosshairHitPos = *crosshairData->collisionPoint;
                if (crosshairHitPos != RE::NiPoint3{0.0f, 0.0f, 0.0f}) {
                    hitPos = crosshairHitPos;
                    hitPosFound = true;
                }
            }
        }
        
        if (!hitPosFound) {
            hitPos = ctx.target->GetPosition();
            
            if (auto* targetRef3D = ctx.target->Get3D()) {
                auto bounds = targetRef3D->worldBound;
                if (bounds.radius > 0.0f) {
                    hitPos = bounds.center;
                    hitPos.z = bounds.center.z - bounds.radius + 5.0f;
                }
            } else {
                logger::warn("SpawnImpactDataSet: Target has no 3D model, skipping");
                dummy->Disable();
                dummy->SetDelete(true);
                return;
            }
        }

        auto dummyPos = dummy->GetPosition();
        dummyPos.z = hitPos.z;
        dummy->SetPosition(dummyPos);

        auto finalHitPos = dummy->GetPosition();

        for (const auto& data : impactsData) {
            if (!data.impact)
                continue;
        
            for (uint32_t i = 0; i < data.count; ++i) {
                im->PlayImpactEffect(ctx.target, data.impact, "", finalHitPos, 0.0f, false, false);
            }
        }

        dummy->Disable();
        dummy->SetDelete(true);
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
                auto explosion = Spawn(ctx.target, explosionData.explosion, explosionData.spawnType, explosionData.fade);              
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

        bool anyItemSpawned = false;

        for (const auto& itemData : itemsData) {
            if (!itemData.item)
                continue;

            for (std::uint32_t i = 0; i < itemData.count; ++i) {
                auto item = Spawn(ctx.target, itemData.item, itemData.spawnType, itemData.fade);
                if (item && ctx.target) {
                    anyItemSpawned = true;
                    CopyOwnership(ctx.target, item.get());
                    if (itemData.scale > -1.0f) {
                        SetObjectScale(item.get(), ctx.target->GetScale());
                    }
                }
            }
        }

        if (anyItemSpawned && ctx.target) {
            if (!ctx.target->IsDisabled()) ctx.target->Disable();
            for (const auto& itemData : itemsData) {
                if (!itemData.nonDeletable && ctx.target) {
                    ctx.target->SetDelete(true);
                }
            }
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
                logger::warn("SpillInventory: Invalid inventory object for target {}", containerRef ? containerRef->GetFormID() : 0);
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

        bool anyActorSpawned = false;

        for (const auto& actorData : actorsData) {
            if (!actorData.npc)
                continue;

            for (std::uint32_t i = 0; i < actorData.count; ++i) {
                auto actor = Spawn(ctx.target, actorData.npc, actorData.spawnType, actorData.fade);
                if (actor && ctx.target) {
                    anyActorSpawned = true;
                    if (actorData.scale > -1.0f && ctx.target) {
                        SetObjectScale(actor.get(), ctx.target->GetScale());
                    }
                }
            }
        }

        if (anyActorSpawned && ctx.target) {
            if (!ctx.target->IsDisabled()) ctx.target->Disable();
            for (const auto& itemData : actorsData) {
                if (!itemData.nonDeletable && ctx.target) {
                    ctx.target->SetDelete(true);
                }
            }
        }
    }

    void SpawnLeveledItem(const RuleContext& ctx, const std::vector<LvlItemSpawnData>& itemsData)
    {
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::error("SpawnLeveledItem: No target to spawn leveled items");
            return;
        }

        if (itemsData.empty()) {
            logger::error("SpawnLeveledItem: No leveled items to spawn with");
            return;
        }

        for (const auto& itemData : itemsData) {
            if (!itemData.item) continue;

            for (std::uint32_t i = 0; i < itemData.count; ++i) {
                auto* obj = ResolveLeveledItem(itemData.item);
                if (!obj) {
                    logger::warn("SpawnLeveledItem: can't resolve LVLI {:X}", itemData.item ? itemData.item->GetFormID() : 0);
                    continue;
                }

                auto item = Spawn(ctx.target, obj, itemData.spawnType, itemData.fade);
                if (item && ctx.target) {
                    CopyOwnership(ctx.target, item.get());
                    if (itemData.scale > -1.0f && ctx.target) {
                        SetObjectScale(item.get(), ctx.target->GetScale());
                    }
                } else {
                    logger::warn("SpawnLeveledItem: place failed for {:X}", obj ? obj->GetFormID() : 0);
                }
            }
        }
    }

    void SwapLeveledItem(const RuleContext& ctx, const std::vector<LvlItemSpawnData>& itemsData)
    {
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::error("SwapLeveledItem: No target to swap leveled items");
            return;
        }

        if (itemsData.empty()) {
            logger::error("SwapLeveledItem: No leveled items to swap with");
            return;
        }

        bool spawned = false;

        for (const auto& itemData : itemsData) {
            if (!itemData.item) continue;

            for (std::uint32_t i = 0; i < itemData.count; ++i) {
                auto* obj = ResolveLeveledItem(itemData.item);
                if (!obj) {
                    logger::warn("SwapLeveledItem: can't resolve LVLI {:X}", itemData.item ? itemData.item->GetFormID() : 0);
                    continue;
                }

                auto item = Spawn(ctx.target, obj, itemData.spawnType, itemData.fade);
                if (item && ctx.target) {
                    spawned = true;
                    CopyOwnership(ctx.target, item.get());
                    if (itemData.scale > -1.0f && ctx.target) {
                        SetObjectScale(item.get(), ctx.target->GetScale());
                    }
                } else {
                    logger::warn("SwapLeveledItem: place failed for {:X}", obj ? obj->GetFormID() : 0);
                }
            }
        }
        
        if (spawned && ctx.target) {
            if (!ctx.target->IsDisabled()) ctx.target->Disable();
            for (const auto& itemData : itemsData) {
                if (!itemData.nonDeletable && ctx.target) {
                    ctx.target->SetDelete(true);
                }
            }        
        }
    }

    void SpawnLeveledSpell(const RuleContext& ctx, const std::vector<LvlSpellSpawnData>& spellsData)
    {
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::warn("SpawnLeveledSpell: No valid target location for dummy caster");
            return;
        }
    
        if (spellsData.empty()) {
            logger::error("SpawnLeveledSpell: No spells to spawn");
            return;
        }
    
        static RE::TESBoundObject* dummyForm = nullptr;
        if (!dummyForm) {
            auto* dh = RE::TESDataHandler::GetSingleton();
            auto* form = dh ? dh->LookupForm(0x000B79FF, "Skyrim.esm") : nullptr;
            dummyForm = form ? form->As<RE::TESBoundObject>() : nullptr;
        }
    
        if (!dummyForm) {
            logger::error("SpawnLeveledSpell: Cannot create dummy caster");
            return;
        }
    
        auto dummy = ctx.target->PlaceObjectAtMe(dummyForm, true);
        if (!dummy) {
            logger::error("SpawnLeveledSpell: Failed to create dummy caster");
            return;
        }
    
        if (auto targetObject = ctx.target->Get3D()) {
            if (auto node = targetObject->AsNode()) {
                dummy->MoveToNode(ctx.target, node);
            }
        }
    
        auto* mc = dummy->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
        if (!mc) {
            logger::error("SpawnLeveledSpell: Dummy has no MagicCaster");
            dummy->Disable();
            dummy->SetDelete(true);
            return;
        }
    
        auto* tes = RE::TES::GetSingleton();
        if (!tes) {
            logger::error("SpawnLeveledSpell: Cannot get TES singleton");
            if (dummy) {
                dummy->Disable();
                dummy->SetDelete(true);
                return;
            }
        }
    
        for (const auto& spellData : spellsData) {
            if (!spellData.spell) continue;
            if (spellData.radius <= 0) return;
    
            std::vector<RE::Actor*> targets;
    
            tes->ForEachReferenceInRange(dummy.get(), spellData.radius, [&](RE::TESObjectREFR* a_ref) {
                auto* actor = a_ref->As<RE::Actor>();
                if (actor && !actor->IsDead() && !actor->IsDisabled()) {
                    targets.push_back(actor);
                }
                return RE::BSContainer::ForEachResult::kContinue;
            });
    
            if (targets.empty()) {
                continue;
            }
    
            auto* spell = ResolveLeveledSpell(spellData.spell);
            if (!spell) {
                logger::warn("SpawnLeveledSpell: Can't resolve LVLS {:X}", spellData.spell ? spellData.spell->GetFormID() : 0);
                continue;
            }
    
            for (auto* tgt : targets) {
                for (std::uint32_t i = 0; i < spellData.count; ++i) {
                    if (!mc || !tgt || tgt->IsDeleted() || !spell) {
                        logger::warn("SpawnLeveledSpell: Invalid MagicCaster, target, or spell");
                        continue;
                    }
                    mc->CastSpellImmediate(spell, false, tgt, 1.0f, false, 0.0f, nullptr);
                }
            }
        }
        
        if (!dummy) {
            logger::error("SpawnLeveledSpell: Dummy caster is null after casting");
            return;
        }
    
        dummy->Disable();
        dummy->SetDelete(true);
    }
    
    void SpawnLeveledSpellOnItem(const RuleContext& ctx, const std::vector<LvlSpellSpawnData>& spellsData)
    {
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::warn("SpawnLeveledSpellOnItem: No valid target to cast the spell on");
            return;
        }
    
        if (spellsData.empty()) {
            logger::error("SpawnLeveledSpellOnItem: No spells to spawn");
            return;
        }
    
        static RE::TESBoundObject* dummyForm = nullptr;
        if (!dummyForm) {
            auto* dh = RE::TESDataHandler::GetSingleton();
            auto* form = dh ? dh->LookupForm(0x000B79FF, "Skyrim.esm") : nullptr;
            dummyForm = form ? form->As<RE::TESBoundObject>() : nullptr;
        }
    
        if (!dummyForm) {
            logger::error("SpawnLeveledSpellOnItem: Cannot create dummy for spell casting");
            return;
        }
    
        auto dummy = ctx.target->PlaceObjectAtMe(dummyForm, true);
        if (!dummy) {
            logger::error("SpawnLeveledSpellOnItem: Failed to create dummy");
            return;
        }
    
        if (auto targetObject = ctx.target->Get3D()) {
            if (auto node = targetObject->AsNode()) {
                dummy->MoveToNode(ctx.target, node);
            }
        }
    
        auto* mc = dummy->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
        if (!mc) {
            logger::error("SpawnLeveledSpellOnItem: Dummy has no MagicCaster");
            if (dummy) {
                dummy->Disable();
                dummy->SetDelete(true);
            }
            return;
        }
    
        for (const auto& spellData : spellsData) {
            if (!spellData.spell) continue;
    
            for (std::uint32_t i = 0; i < spellData.count; ++i) {
                auto* spell = ResolveLeveledSpell(spellData.spell);
                if (!spell) {
                    logger::warn("SpawnLeveledSpellOnItem: Can't resolve LVLS {:X}", spellData.spell ? spellData.spell->GetFormID() : 0);
                    continue;
                }
                if (!mc || !ctx.target || ctx.target->IsDeleted() || !spell) {
                    logger::warn("SpawnLeveledSpellOnItem: Invalid MagicCaster, target, or spell");
                    continue;
                }
                mc->CastSpellImmediate(spell, false, ctx.target, 1.0f, false, 0.0f, nullptr);
            }
        }

        if (!dummy) {
            logger::error("SpawnLeveledSpellOnItem: Dummy caster is null after casting");
            return;
        }
    
        dummy->Disable();
        dummy->SetDelete(true);
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

        for (const auto& actorData : actorsData) {
            if (!actorData.npc) continue;

            for (std::uint32_t i = 0; i < actorData.count; ++i) {
                auto* npcBase = ResolveLeveledNPC(actorData.npc);
                if (!npcBase) {
                    logger::warn("SpawnLeveledActor: Can't resolve LVLC {:X}", actorData.npc ? actorData.npc->GetFormID() : 0);
                    continue;
                }

                auto actor = Spawn(ctx.target, npcBase, actorData.spawnType, actorData.fade);
                if (actor && ctx.target) {
                    if (actorData.scale > -1.0f && ctx.target) {
                        SetObjectScale(actor.get(), ctx.target->GetScale());
                    }
                }
            }
        }
    }

    void SwapLeveledActor(const RuleContext& ctx, const std::vector<LvlActorSpawnData>& actorsData)
    {
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::error("SwapLeveledActor: No target to swap actors with");
            return;
        }

        if (actorsData.empty()) {
            logger::error("SwapLeveledActor: No actors to swap with");
            return;
        }

        bool spawned = false;

        for (const auto& actorData : actorsData) {
            if (!actorData.npc) continue;

            for (std::uint32_t i = 0; i < actorData.count; ++i) {
                auto* npcBase = ResolveLeveledNPC(actorData.npc);
                if (!npcBase) {
                    logger::warn("SpawnLeveledActor: Can't resolve LVLC {:X}", actorData.npc ? actorData.npc->GetFormID() : 0);
                    continue;
                }

                auto actor = Spawn(ctx.target, npcBase, actorData.spawnType, actorData.fade);
                if (actor && ctx.target) {
                    spawned = true;
                    if (actorData.scale > -1.0f && ctx.target) {
                        SetObjectScale(actor.get(), ctx.target->GetScale());
                    }
                } 
            }
        }

        if (spawned && ctx.target) {
            if (!ctx.target->IsDisabled()) ctx.target->Disable();
            for (const auto& itemData : actorsData) {
                if (!itemData.nonDeletable && ctx.target) {
                    ctx.target->SetDelete(true);
                }
            }        
        }
    }

    void ApplyIngestible(const RuleContext& ctx, const std::vector<IngestibleApplyData>& ingestiblesData)
    {
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::warn("ApplyIngestible: No valid target location for dummy caster");
            return;
        }

        if (ingestiblesData.empty()) {
            logger::error("ApplyOtherIngestible: No ingestibles to apply");
            return;
        }

        auto* baseObject = ctx.target->GetBaseObject();
        if (!baseObject) {
            logger::error("ApplyIngestible: Target has no base object");
            return;
        }

        auto* magicItem = baseObject->As<RE::MagicItem>();
        if (!magicItem) {
            logger::warn("ApplyIngestible: The target is not a MagicItem");
            return;
        }

        static RE::TESBoundObject* dummyForm = nullptr;
        if (!dummyForm) {
            auto* dh = RE::TESDataHandler::GetSingleton();
            auto* form = dh ? dh->LookupForm(0x000B79FF, "Skyrim.esm") : nullptr;
            dummyForm = form ? form->As<RE::TESBoundObject>() : nullptr;
        }

        if (!dummyForm) {
            logger::error("ApplyIngestible: Cannot create dummy caster");
            return;
        }

        auto dummy = ctx.target->PlaceObjectAtMe(dummyForm, true);
        if (!dummy) {
            logger::error("ApplyIngestible: Failed to create dummy caster");
            return;
        }

        if (auto targetObject = ctx.target->Get3D()) {
            if (auto node = targetObject->AsNode()) {
                dummy->MoveToNode(ctx.target, node);
            }
        }

        auto* mc = dummy->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
        if (!mc) {
            logger::error("ApplyIngestible: Dummy has no MagicCaster");
            dummy->Disable();
            dummy->SetDelete(true);
            return;
        }

        auto* tes = RE::TES::GetSingleton();
        if (!tes) {
            logger::error("ApplyIngestible: TES singleton is null");
            if (dummy) {
                dummy->Disable();
                dummy->SetDelete(true);
                return;
            }
        }

        const auto& data = ingestiblesData[0];
        if (data.radius <= 0) return;

        const bool hostile = magicItem->IsPoison();
        std::vector<RE::Actor*> targets;

        tes->ForEachReferenceInRange(dummy.get(), data.radius, [&](RE::TESObjectREFR* a_ref) {
            auto* actor = a_ref->As<RE::Actor>();
            if (actor && !actor->IsDisabled() && !actor->IsDeleted() && !actor->IsDead() && !actor->IsGhost()) {
                targets.push_back(actor);
            }
            return RE::BSContainer::ForEachResult::kContinue;
        });

        if (targets.empty()) {
        } else {
            for (auto* actor : targets) {
                if (!mc || !actor || actor->IsDeleted() || !magicItem) {
                    logger::warn("ApplyIngestible: Invalid MagicCaster, target, or magic item");
                    continue;
                }
                mc->CastSpellImmediate(magicItem, false, actor, 1.0f, hostile, 0.0f, nullptr);
            }
        }

        if (!dummy) {
            logger::error("ApplyIngestible: Dummy caster is null after casting");
            return;
        }

        dummy->Disable();
        dummy->SetDelete(true);
    }

    void ApplyOtherIngestible(const RuleContext& ctx, const std::vector<IngestibleApplyData>& ingestiblesData)
    {
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::warn("ApplyOtherIngestible: No valid target location for dummy caster");
            return;
        }

        if (ingestiblesData.empty()) {
            logger::error("ApplyOtherIngestible: No ingestibles to apply");
            return;
        }

        static RE::TESBoundObject* dummyForm = nullptr;
        if (!dummyForm) {
            auto* dh = RE::TESDataHandler::GetSingleton();
            auto* form = dh ? dh->LookupForm(0x000B79FF, "Skyrim.esm") : nullptr;
            dummyForm = form ? form->As<RE::TESBoundObject>() : nullptr;
        }

        if (!dummyForm) {
            logger::error("ApplyOtherIngestible: Cannot create dummy caster");
            return;
        }

        auto dummy = ctx.target->PlaceObjectAtMe(dummyForm, true);
        if (!dummy) {
            logger::error("ApplyOtherIngestible: Failed to create dummy caster");
            return;
        }

        if (auto targetObject = ctx.target->Get3D()) {
            if (auto node = targetObject->AsNode()) {
                dummy->MoveToNode(ctx.target, node);
            }
        }

        auto* mc = dummy->GetMagicCaster(RE::MagicSystem::CastingSource::kInstant);
        if (!mc) {
            logger::error("ApplyOtherIngestible: Dummy has no MagicCaster");
            dummy->Disable();
            dummy->SetDelete(true);
            return;
        }

        auto* tes = RE::TES::GetSingleton();
        if (!tes) {
            logger::error("ApplyIngestible: TES singleton is null");
            if (dummy) {
                dummy->Disable();
                dummy->SetDelete(true);
                return;
            }
        }

        for (const auto& ingestibleData : ingestiblesData) {
            if (!ingestibleData.ingestible) continue;
            if (ingestibleData.radius <= 0) continue;

            std::vector<RE::Actor*> targets;

            tes->ForEachReferenceInRange(dummy.get(), ingestibleData.radius, [&](RE::TESObjectREFR* a_ref) {
                auto* actor = a_ref->As<RE::Actor>();
                if (actor && !actor->IsDisabled() && !actor->IsDeleted() && !actor->IsDead() && !actor->IsGhost()) {
                    targets.push_back(actor);
                }
                return RE::BSContainer::ForEachResult::kContinue;
            });

            if (targets.empty()) {
                continue;
            }

            const bool hostile = ingestibleData.ingestible->IsPoison();

            for (auto* tgt : targets) {
                for (std::uint32_t i = 0; i < ingestibleData.count; ++i) {
                    if (!mc || !tgt || tgt->IsDeleted() || !ingestibleData.ingestible) {
                        logger::warn("ApplyOtherIngestible: Invalid MagicCaster, target, or ingestible");
                        continue;
                    }
                    mc->CastSpellImmediate(ingestibleData.ingestible, false, tgt, 1.0f, hostile, 0.0f, nullptr);
                }
            }
        }

        if (!dummy) {
            logger::error("ApplyOtherIngestible: Dummy caster is null after casting");
            return;
        }

        dummy->Disable();
        dummy->SetDelete(true);
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

        for (const auto& lightData : lightsData) {
            if (!lightData.light)
                continue;

            for (std::uint32_t i = 0; i < lightData.count; ++i) {
                auto light = Spawn(ctx.target, lightData.light, lightData.spawnType, lightData.fade);
                if (light && ctx.target) {
                    light->Enable(false);
                    if (lightData.scale > -1.0f && ctx.target) {
                        SetObjectScale(light.get(), ctx.target->GetScale());
                    }
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
        if (!tes) {
            logger::error("RemoveLight: TES singleton is null");
            return;
        }

        for (const auto& lightData : lightsData) {
            if (lightData.radius <= 0) continue;

            tes->ForEachReferenceInRange(ctx.target, lightData.radius, [&](RE::TESObjectREFR* ref) {
                if (!ref || ref->IsDisabled() || ref->IsDeleted())
                    return RE::BSContainer::ForEachResult::kContinue;

                auto* baseObj = ref->GetBaseObject();
                if (ref && baseObj && baseObj->Is(RE::FormType::Light)) {
                    if (!ref->IsDisabled()) ref->Disable();
                    ref->SetDelete(true); 
                }
                return RE::BSContainer::ForEachResult::kContinue;
            });
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
        if (!tes) {
            logger::error("EnableLight: TES singleton is null");
            return;
        }

        for (const auto& lightData : lightsData) {
            if (lightData.radius <= 0) continue;

            tes->ForEachReferenceInRange(ctx.target, lightData.radius, [&](RE::TESObjectREFR* ref) {
                if (!ref || ref->IsDisabled() || ref->IsDeleted())
                    return RE::BSContainer::ForEachResult::kContinue;

                auto* baseObj = ref->GetBaseObject();
                if (ref && baseObj && baseObj->Is(RE::FormType::Light)) {
                    if (ref->IsDisabled()) ref->Enable(false);
                }
                return RE::BSContainer::ForEachResult::kContinue;
            });
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
        if (!tes) {
            logger::error("DisableLight: TES singleton is null");
            return;
        }

        for (const auto& lightData : lightsData) {
            if (lightData.radius <= 0) continue;

            tes->ForEachReferenceInRange(ctx.target, lightData.radius, [&](RE::TESObjectREFR* ref) {
                if (!ref || ref->IsDisabled() || ref->IsDeleted())
                    return RE::BSContainer::ForEachResult::kContinue;

                auto* baseObj = ref->GetBaseObject();
                if (ref && baseObj && baseObj->Is(RE::FormType::Light)) {
                    if (!ref->IsDisabled()) ref->Disable();
                }
                return RE::BSContainer::ForEachResult::kContinue;
            });
        }
    }

    void PlayIdle(const RuleContext& ctx, const std::vector<PlayIdleData>& playIdleData) 
    {
        if (!ctx.source || ctx.source->IsDeleted() || ctx.source->IsDead()) {
            logger::error("PlayIdle: No valid actor to play idle animation");
            return;
        }
        
        const auto& data = playIdleData[0];
        
        if (data.string.empty()) {
            logger::error("PlayIdle: No idle name (string) provided");
            return;
        }
        
        if (data.duration <= 0.0f) {
            logger::error("PlayIdle: Invalid duration for idle animation: {}", data.duration);
            return;
        }

        auto* player = RE::PlayerCharacter::GetSingleton();
        if (player && ctx.source == player) {
            RE::PlayerCamera* camera = RE::PlayerCamera::GetSingleton();
            if (!camera || camera->IsInFirstPerson()) {
                return;
            }
        }
        
        if (ctx.source->IsInCombat() || ctx.source->IsInKillMove() || ctx.source->IsInRagdollState() || ctx.source->IsOnMount() ||
            ctx.source->AsActorState()->IsFlying() || ctx.source->AsActorState()->IsWeaponDrawn() || ctx.source->AsActorState()->IsBleedingOut()) {
            return;
        }
        
        PlayIdleOnActor(ctx.source, data.string);

        static std::vector<std::future<void>> runningTasks;
        static std::mutex tasksMutex;

        auto future = std::async(std::launch::async, [actor = ctx.source, duration = data.duration]() {
            std::this_thread::sleep_for(std::chrono::duration<float>(duration));
            
            SKSE::GetTaskInterface()->AddTask([actor]() {
                if (actor && !actor->IsDeleted() && !actor->IsDead()) {
                    PlayIdleOnActor(actor, "IdleStop");
                }
            });
        });

        {
            std::lock_guard<std::mutex> lock(tasksMutex);
            runningTasks.push_back(std::move(future));
            
            runningTasks.erase(
                std::remove_if(runningTasks.begin(), runningTasks.end(),
                    [](const std::future<void>& f) {
                        return f.wait_for(std::chrono::seconds(0)) == std::future_status::ready;
                    }),
                runningTasks.end()
            );
        }
    }

    void SpawnEffectShader(const RuleContext& ctx, const std::vector<EffectShaderSpawnData>& effectShadersData) 
    {
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::error("SpawnEffectShader: No target to spawn effect shaders");
            return;
        }

        if (effectShadersData.empty()) {
            logger::error("SpawnEffectShader: No effect shaders to spawn");
            return;
        }

        auto* tes = RE::TES::GetSingleton();
        if (!tes) {
            logger::error("SpawnEffectShader: TES singleton is null");
            return;
        }
        
        for (const auto& effectShaderData : effectShadersData) {
            if (!effectShaderData.effectShader) continue;
    
            std::vector<RE::Actor*> targets;
        
            tes->ForEachReferenceInRange(ctx.target, effectShaderData.radius, [&](RE::TESObjectREFR* a_ref) {
                auto* actor = a_ref->As<RE::Actor>();
                if (actor && !actor->IsDead() && !actor->IsDisabled()) {
                    targets.push_back(actor);
                }
                return RE::BSContainer::ForEachResult::kContinue;
            });
        
            if (targets.empty()) {
                logger::warn("SpawnEffectShader: No valid actors found in range");
                return;
            }
    
            for (auto* actor : targets) {
                for (std::uint32_t i = 0; i < effectShaderData.count; ++i) {
                    auto shaderEffect = actor->ApplyEffectShader(effectShaderData.effectShader, effectShaderData.duration, nullptr, false, false, nullptr, false);
    
                    if (!shaderEffect) {
                        logger::error("SpawnEffectShader: Failed to apply effect shader {} on actor {}", 
                            effectShaderData.effectShader ? effectShaderData.effectShader->GetFormID() : 0,
                            actor ? actor->GetFormID() : 0);
                    }
                }
            }
        }
    }

    void SpawnEffectShaderOnItem(const RuleContext& ctx, const std::vector<EffectShaderSpawnData>& effectShadersData) 
    {
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::error("SpawnEffectShader: No target to spawn effect shaders");
            return;
        }

        if (effectShadersData.empty()) {
            logger::error("SpawnEffectShader: No effect shaders to spawn");
            return;
        }

        for (const auto& effectShaderData : effectShadersData) {
            if (!effectShaderData.effectShader) continue;

            for (std::uint32_t i = 0; i < effectShaderData.count; ++i) {
                auto shaderEffect = ctx.target->ApplyEffectShader(effectShaderData.effectShader, effectShaderData.duration, nullptr, false, false, nullptr, false);

                if (!shaderEffect) {
                    logger::error("SpawnEffectShader: Failed to apply effect shader {} on target {}", 
                        effectShaderData.effectShader ? effectShaderData.effectShader->GetFormID() : 0,
                        ctx.target ? ctx.target->GetFormID() : 0);
                }
            }
        }
    }

    void ToggleNode(const RuleContext& ctx, const std::vector<NodeData>& nodeData)
    {
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::error("ToggleNode: No target to toggle node on");
            return;
        }

        if (!ctx.target->Get3D()) {
            ctx.target->Load3D(true);
        }

        auto* rootObj = ctx.target->Get3D();
        if (!rootObj) {
            logger::error("ToggleNode: Target has no 3D");
            return;
        }

        auto* rootNode = rootObj->AsNode();
        if (!rootNode) {
            logger::error("ToggleNode: Root 3D is not NiNode");
            return;
        }

        for (const auto& data : nodeData) {
            std::vector<RE::NiNode*> matches;
            CollectNodes(rootNode, data.strings, matches);
            if (matches.empty()) {
                logger::warn("ToggleNode: No BSnode found for {} names", data.strings.size());
                continue;
            }
            
            for (RE::NiNode* node : matches) {
                if (!node) continue;

                try {
                    if (data.mode == 0) {
                        node->local.scale = 0.00001f;
                    } else {
                        node->local.scale = 1.0f;
                    }
                }
                catch (...) {
                    logger::error("ToggleNode: Exception while trying to toggle '{}'", node->name.c_str());
                }
            }
        }
    }
                
    /*void ToggleShaderFlag(const RuleContext& ctx, const std::vector<ShaderFlagData>& shaderFlagsData)
    {
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::error("ToggleShaderFlag: No target to modify shader flags on");
            return;
        }
    
        if (!ctx.target->Get3D()) {
            ctx.target->Load3D(true);
        }
    
        auto* original3D = ctx.target->Get3D();
        if (!original3D) {
            logger::error("ToggleShaderFlag: Target has no 3D");
            return;
        }
    
        auto* cloned3D = original3D->Clone();
        if (!cloned3D) {
            logger::error("ToggleShaderFlag: Clone3D failed");
            return;
        }
    
        auto* rootNode = cloned3D->AsNode();
        if (!rootNode) {
            logger::error("ToggleShaderFlag: Root 3D is not NiNode");
            return;
        }
    
        for (const auto& data : shaderFlagsData) {
            std::vector<RE::BSGeometry*> matches;
            CollectTriShapes(rootNode, data.strings, matches);
    
            if (matches.empty()) {
                logger::warn("ToggleShaderFlag: No TriShape found");
                continue;
            }
    
            for (RE::BSGeometry* geometry : matches) {
                if (!geometry) continue;
    
                using State = RE::BSGeometry::States;
                
                auto effect = geometry->GetGeometryRuntimeData().properties[State::kEffect].get();
                if (!effect) {
                    logger::warn("ToggleShaderFlag: TriShape '{}' has no effect property", geometry->name.c_str());
                    continue;
                }
                
                auto lightingShader = netimmerse_cast<RE::BSLightingShaderProperty*>(effect);
                if (!lightingShader) {
                    logger::warn("ToggleShaderFlag: TriShape '{}' does not have BSLightingShaderProperty", geometry->name.c_str());
                    continue;
                }

                for (const auto& flagName : data.flagNames) {
                    auto it = g_shaderFlagMap.find(flagName);
                    if (it == g_shaderFlagMap.end()) {
                        continue;
                    }

                    RE::BSShaderProperty::EShaderPropertyFlag flag = it->second;

                    if (flag == RE::BSShaderProperty::EShaderPropertyFlag::kOwnEmit ||
                        flag == RE::BSShaderProperty::EShaderPropertyFlag::kGlowMap ||
                        flag == RE::BSShaderProperty::EShaderPropertyFlag::kEffectLighting ||
                        flag == RE::BSShaderProperty::EShaderPropertyFlag::kExternalEmittance) {

                        lightingShader->flags.reset(RE::BSShaderProperty::EShaderPropertyFlag::kOwnEmit);
                        lightingShader->flags.reset(RE::BSShaderProperty::EShaderPropertyFlag::kGlowMap);
                        lightingShader->flags.reset(RE::BSShaderProperty::EShaderPropertyFlag::kEffectLighting);
                        lightingShader->flags.reset(RE::BSShaderProperty::EShaderPropertyFlag::kExternalEmittance);
                        
                        if (lightingShader->emissiveColor) {
                            delete lightingShader->emissiveColor;
                            lightingShader->emissiveColor = nullptr;
                        }
                        lightingShader->emissiveMult = 0.0f;
                    }

                    lightingShader->lastRenderPassState = (std::numeric_limits<std::int32_t>::max)();
                    
                    lightingShader->DoClearRenderPasses();

                    logger::info("Before: flags = 0x{:X}, lastRenderPassState = {}", 
                        lightingShader->flags.underlying(), lightingShader->lastRenderPassState);
                    
                    if (data.mode == 0) {
                        lightingShader->flags.reset(flag);
                        logger::info("ToggleShaderFlag: Disabled flag '{}' on TriShape '{}'", flagName, geometry->name.c_str());
                    } else {
                        lightingShader->flags.set(flag);
                        logger::info("ToggleShaderFlag: Enabled flag '{}' on TriShape '{}'", flagName, geometry->name.c_str());
                        if (flag == RE::BSShaderProperty::EShaderPropertyFlag::kOwnEmit ||
                            flag == RE::BSShaderProperty::EShaderPropertyFlag::kGlowMap ||
                            flag == RE::BSShaderProperty::EShaderPropertyFlag::kEffectLighting ||
                            flag == RE::BSShaderProperty::EShaderPropertyFlag::kExternalEmittance) {
                            if (!lightingShader->emissiveColor) {
                                lightingShader->emissiveColor = new RE::NiColor(1.0f, 1.0f, 1.0f);
                            }
                            lightingShader->emissiveMult = 1.0f;
                        }
                    }

                    logger::info("After: flags = 0x{:X}, lastRenderPassState = {}", 
                        lightingShader->flags.underlying(), lightingShader->lastRenderPassState);
    
                    lightingShader->SetupGeometry(geometry);
                    lightingShader->FinishSetupGeometry(geometry);
                }
            }
        }
    
        ctx.target->Set3D(nullptr, false);
        ctx.target->Set3D(cloned3D, true);
        ctx.target->Load3D(true);
        ctx.target->Disable();
        ctx.target->Enable(false);
        ctx.target->formFlags |= static_cast<std::uint32_t>(RE::TESObjectREFR::RecordFlags::kNeverFades);
    }*/

    void UnlockItem(const RuleContext& ctx)
    {
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::error("UnlockItem: No target to open");
            return;
        }

        auto* openCloseForm = ctx.target->GetBaseObject()->As<RE::BGSOpenCloseForm>();
        if (!openCloseForm) {
            return;
        }

        auto state = RE::BGSOpenCloseForm::GetOpenState(ctx.target);
        if (state == RE::BGSOpenCloseForm::OPEN_STATE::kOpen || state == RE::BGSOpenCloseForm::OPEN_STATE::kOpening) {
            return;
        }

        RE::BGSOpenCloseForm::SetOpenState(ctx.target, true, false);
    }

    void LockItem(const RuleContext& ctx)
    {
        if (!ctx.target || ctx.target->IsDeleted()) {
            logger::error("LockItem: No target to close");
            return;
        }

        auto* openCloseForm = ctx.target->GetBaseObject()->As<RE::BGSOpenCloseForm>();
        if (!openCloseForm) {
            return;
        }

        auto state = RE::BGSOpenCloseForm::GetOpenState(ctx.target);
        if (state == RE::BGSOpenCloseForm::OPEN_STATE::kClosed || state == RE::BGSOpenCloseForm::OPEN_STATE::kClosing) {
            return;
        }

        RE::BGSOpenCloseForm::SetOpenState(ctx.target, false, false);
    }
}