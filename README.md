# Object Impact Framework

**Object Impact Framework (OIF)** is a SKSE plugin that lets you add custom effects to items upon **activation**, **grabbing**, **releasing**, **throwing**, **hitting**, **update**, or **weather change**. You can make things happen like spawning or swapping items and actors, casting spells, creating various visual effects, playing sounds, and more - all controlled with JSON files. Compatible with everything.

This guide explains how to set up those JSON files so you can create your own mods. Examples are given at the end of the documentation.

## Users Info

- Place your JSON files in: `Data/SKSE/Plugins/ObjectImpactFramework/`

## Mod Authors Info

- **Filter Note**: An object must be defined by at least one of the three parameters - `formIDs`, `editorIDs`, `formLists`, `formTypes`, `keywords` - for the event to work. **Warning:** Equivalents with the `Not` ending do *not* count.
- **Effect Additional Fields Note**: Make sure you check whether the effect requires the presence of the `items` field. Without specifying and filling out this field (at least with one identifier or, if the effect does not support identifiers, with any available field), the effect will *not* work.
- **Priority Note**: Place effects with `"Remove"` and `"Swap"` prefixes at the very end of the event, otherwise the removed object may not have time to call other effects on itself before gets deleted.
- **Non-Collidable Objects Note**: By default, the framework can detect non-collidable objects, but this does not apply to the `Hit` event. Only `flora` and `tree` hits are supported. Since the system uses workarounds and mathematical calculations, it can cause plants location estimate not to be always accurate. **Warning:** Spells with attached projectiles are currently not supported due to the hooks issues.
- **Troubleshooting Note**: If something doesn't work, look at the mod's log file for error messages. The log can be found in `Documents/My Games/Skyrim Special Edition/SKSE/ObjectImpactFramework.log`
- You can modify existing JSON files without quitting the game, edit the file and reload the save.

---

## Table of Contents

- [Basic Rule Structure](#basic-rule-structure)
- [Filters: Choosing Which Objects to Affect and How](#filters-choosing-which-objects-to-affect-and-how)
  - [General Filters](#general-filters)
  - [Time‑Based Filters](#time-based-filters)
  - [Proximity-Based Filters](#proximity-based-filters)
  - [Source Actor Filters](#source-actor-filters)
  - [Hit-Specific Filters](#hit-specific-filters)
- [Effects: What Happens When the Rule Triggers](#effects-what-happens-when-the-rule-triggers)
  - [Effect Types](#object-management)
  - [Configuring Effects with items](#configuring-effects-with-items)
- [Examples](#examples)

---

## Basic Rule Structure

Each rule in your JSON file defines a specific behavior for the mod. Rules are written as an array of objects, where each object contains three main fields: `event`, `filter`, and `effect`. Here's what each field does:

- **`event`**: Specifies the trigger(s) for the rule. This is an array of strings, allowing multiple events to trigger the same rule. Valid values are:
  - `"Activate"`: Triggered when an object is activated (e.g., opened, used).
  - `"Hit"`: Triggered when an object is hit by a weapon, spell, or projectile.
  - `"Grab"`: Triggered when an object is picked up by the player.
  - `"Release"`: Triggered when a grabbed object is dropped.
  - `"Telekinesis"`: Triggered when an object the player was holding with telekinesis lands.
  - `"Throw"`: Triggered when a grabbed object is thrown (requires the **Grab And Throw** mod by powerofthree).
  - `"CellAttach"`: Triggered when an object is attached to a cell (works on location re-enter as well).
  - `"CellDetach"`: Triggered when an object is detached from a cell.
  - `"WeatherChange"`: Triggered on weather change.
  - `"OnUpdate"`: Triggered every 1 second.
  - `"DestructionStageChange"`: Triggered on object's destruction stage change. The effect will *not* be applied to the scene when the object is disabled or deleted.
  
- **`filter`**: Defines the targeted objects and the conditions under which a rule applies to them. At least one of `formTypes`, `formIDs`, `editorIDs`, `formLists`, or `keywords` must be provided to identify target objects.

- **`effect`**: Describes what happens when the rule is triggered. Each effect has a `type` and, for most types, an `items` array specifying what to spawn, swap, or apply.

Here's a basic example of a rule:

```json
[
    {
        "event": ["Activate", "Hit"],
        "filter": {
            "formTypes": ["activator"]
        },
        "effect": [{
            "type": "SpawnItem",
            "items": [{"formID": "Skyrim.esm:0xF"}]
        }]
    }
]
```

This rule triggers when an activator (e.g., a lever) is activated or hit, spawning a single gold coin (`Skyrim.esm:0xF`).

---

## Filters: Choosing Which Objects to Affect and How

The `filter` object determines which objects and interactions trigger a rule. It's highly customizable, allowing precise control over when effects occur.

Below are all possible filter parameters:

### General Filters

- **`formTypes`** (one of the required fields to choose from): An array of strings specifying the types of objects the rule applies to. Possible values:
  - `"activator"`: Standard activators (e.g., levers, buttons).
  - `"talkingactivator"`: Activators that can "speak" (e.g., some quest-related objects).
  - `"weapon"`: Weapons like swords or bows (lying around in the world).
  - `"armor"`: Armor pieces.
  - `"ammo"`: Ammunition (e.g., arrows, bolts).
  - `"ingredient"`: Alchemy ingredients.
  - `"misc"`: Miscellaneous items (e.g., clutter, tools).
  - `"key"`: Keys.
  - `"book"`: Books and journals.
  - `"note"`: Notes.
  - `"scroll"`: Magical scrolls.
  - `"soulgem"`: Soul gems.
  - `"potion"`: Includes potions, poisons, food, and drinks.
  - `"furniture"`: Furniture like chairs or beds.
  - `"door"`: Doors and gates.
  - `"flora"`: Harvestable plants and hanging animals.
  - `"container"`: Containers like chests or barrels.
  - `"static"`: Static objects (e.g., statues).
  - `"moveablestatic"`: Movable static objects.
  - `"tree"`: Trees.
  - `"light"`: Lights with 3D models (e.g., torches).
 
- **`formTypesNot`**: An array of strings specifying the types of objects that the rule should *not* apply to. Same format as `formTypes`.

- **`formIDs`** (one of the required fields to choose from): An array of strings identifying specific objects by their formID in the format `"modName:formID"`. Examples:
  - `"Skyrim.esm:0x123456"` (for esp/esm plugins).
  - `"MyMod.esl:0x456"` (for esl/espfe plugins).
  - `"Dawnguard.esm:00123456"` (alternate format with leading zeros (esp/esm)).
  - `"MyMod.esp:FE000800"` (alternate format with leading FE prefix (esl/espfe)).

- **`formIDsNot`**: An array of strings identifying specific objects that the rule should *not* apply to. Same format as `formIDs`.

- **`editorIDs`** (one of the required fields to choose from): An array of strings identifying specific objects by their editorID in the format `"EditorIDName"`. Example: `"VendorItemClutter"`.

- **`editorIDsNot`**: An array of strings identifying specific objects that the rule should *not* apply to. Same format as `editorIDs`.

- **`formLists`** (one of the required fields to choose from): An array of objects referencing formlists. Each entry has:
  - **`formID`**: The formlist's form ID in `"modName:formID"` format (use this or `editorID`).
  - **`editorID`**: The formlist's editor ID in `"EditorIDName"` format (use this or `formID`).
  - **`index`** (optional): An integer specifying which item in the formlist to use. Example:
 
    ```json
    "formLists": [
        {"formID": "Skyrim.esm:0x123456", "index": 0},
        {"editorID": "ExampleFormList"}
    ]
    ```
    - **Index `-1`** is default, means that all the items at once will be used. 
    - **Index `-2`** defines the parrallel relationship for mirrored formlists: matching positions between `filter` and effects' `items` formlists correspond directly, allowing ordered transformations (e.g., raw meat at position 0 in formlist 1 becomes cooked meat at position 0 in formlist 2).

- **`formListsNot`**: An array of formlist objects that the rule should *not* apply to. Same format as `formLists`.

- **`keywords`** (one of the required fields to choose from): An array of keywords or formists of keywords that the object must have. Format: `"modName:formID"` (e.g., `"Skyrim.esm:0xABCDEF"`), `"EditorIDName"` (e.g., `"VendorItemFood"`), or a formlist's formID/editorID.

- **`keywordsNot`**: An array of keywords or formists of keyword the object must *not* have. Same format as `keywords`.

- **`chance`**: A number between 0 and 100 representing the percentage chance the rule triggers (e.g., `"chance": 90.0`). Defaults to `100`.
  **Optional** detailed entry (e.g., `"chance": {"min": 0.1, "max": 85.0}`):
  - **`min`**: Minimal random value.
  - **`max`**: Maximal random value.

- **`interactions`**: An integer specifying how many interactions (e.g., hits) are required to trigger the effect (e.g., `"interactions": 5`). Defaults to `1`.
  **Optional** detailed entry (e.g., `"interactions": {"min": 1, "max": 5}`):
  - **`min`**: Minimal random value.
  - **`max`**: Maximal random value.

- **`limit`**: An integer setting the maximum number of interactions per object (e.g., `"limit": 5`). Defaults to `0` - no limit.
  **Optional** detailed entry (e.g., `"limit": {"min": 0, "max": 10}`):
  - **`min`**: Minimal random value.
  - **`max`**: Maximal random value.

- **`questItemStatus`**: An integer specifying quest item status requirements. Only works with **ACTIVE** player quests:
  - `0` (default): Not a quest item.
  - `1`: Quest alias only.
  - `2`: A full-fledged quest item.
  - `3`: All allowed.
 
- **`isInitiallyDisabled`**: An integer specifying whether the object has the `kInitiallyDisabled` flag.
  - `0`: Not initially disabled.
  - `1`: Initially disabled.
  - `2` (default): All allowed.

 - **`isStacked`**: An integer specifying whether the object is a single reference or a stacked bunch.
  - `0`: Not stacked.
  - `1`: Stacked.
  - `2` (default): All allowed.
 
- **`lockLevel`**: An integer specifying a lock level the object must have. **Note:** Do *not* use the filter with formTypes other than `door` and `container`.
  - `-2` (default): All allowed.
  - `-1`: Unlocked.
  - `0`: Novice.
  - `1`: Apprentice.
  - `2`: Adept.
  - `3`: Expert.
  - `4`: Master.
  - `5`: Requires key.
 
- **`lockLevelNot`**: An interger specifying a lock level the object must *not* have. Same format as `lockLevel`.
 
- **`isPluginInstalled`**: An array of plugin names (e.g., `"MyMod.esp"`, `"Skyrim.esm"`) that must be loaded for the rule to apply.

- **`isPluginNotInstalled`**: An array of plugin names that must *not* be loaded.

- **`isDllInstalled`**: An array of DLL filenames (e.g., `"MyPlugin.dll"`) that must be present in `Data/SKSE/Plugins/`.

- **`isDllNotInstalled`**: An array of DLL filenames that must *not* be present.

### Time-Based Filters

- **`timer`**: A defined number of seconds before triggering the effect (e.g., `"timer": 1.0`). **Optional** detailed entry has:
  - **`time`**: Number of seconds. (e.g., `"timer": {"time": 1.0}`).
    **Optional** detailed entry (e.g., `"timer": {"time": {"min": 1.0, "max": 10.0}}`:
    - **`min`**: Minimal random value.
    - **`max`**: Maximal random value.
  - **`matchFilterRecheck`**: Whether the effect needs to be canceled if conditions were violated while waiting.
    - `0`: No re-check.
    - `1`: Re-check.

  ```json
  "timer": {"time": 1.0, "matchFilterRecheck": 1}
  ```

- **`time`**: An array of in-game time conditions that must be active for the rule to apply. Format: `["Hour >= 10", "DayOfWeek = 1"]`. Available entries:
  - `Minute`
  - `Hour`
  - `Day`
  - `DayOfWeek`
  - `Month`
  - `Year`
  - `GameTime`
 
- **`timeNot`**: An array of in-game time conditions that must *not* be active. Same format as `time`.

### Proximity-Based Filters

- **`nearbyObjects`**: An array of objects that must be in a certain radius around the target object for the rule should apply. Each entry has:
  - **`formID`**: The object formID or a formlist's formID in `"modName:formID"` format (use this or `editorID`).
  - **`editorID`**: The object editorID or a formlist's editorID in `"EditorIDName"` format (use this or `formID`).
  - **`radius`**: An integer specifying in what radius to search. Example:
 
    ```json
    "nearbyObjects": [
        {"formID": "Skyrim.esm:0x123456", "radius": 150},
        {"editorID": "Player", "radius": 300}
    ]
    ```
- **`nearbyObjectsNot`**: An array of objects that must *not* be in a certain radius around the target object. Same format as `nearbyObjects`.

- **`locations`**: An array of cells, locations, or worldspaces where the rule should apply. Format: `"modName:formID"` (e.g., `"Skyrim.esm:0xABCDEF"`), `"EditorIDName"` (e.g., `"VendorItemFood"`), or a formlist's formID/editorID. **Note:** The worldspace will *not* be detected and the event will be filtered out if the cell has no linked worldspace information.

- **`locationsNot`**: An array of cells, locations, or worldspaces where the rule should *not* apply. Same format as `locations`.

- **`isInterior`**: An integer specifying whether the object is indoors.
  - `0`: Not an interior.
  - `1`: Interior.
  - `2` (default): All allowed.

- **`position`**: An integer specifying the object's position in relation to the source actor.
  - `0`: Below the middle.
  - `1`: Middle.
  - `2`: Above the middle.
  - `3` (default): All allowed.

- **`weathers`**: An array of weathers that must be active for the rule to apply. Format: `"modName:formID"` (e.g., `"Skyrim.esm:0xABCDEF"`), `"EditorIDName"` (e.g., `"VendorItemFood"`), or a formlist's formID/editorID.

- **`weathersNot`**: An array of weathers that must *not* be active. Same format as `weathers`.

### Source-Actor Filters

- **`perks`**: An array of perks that the event source actor must have. Format: `"modName:formID"` (e.g., `"Skyrim.esm:0xABCDEF"`), `"EditorIDName"` (e.g., `"VendorItemFood"`), or a formlist's formID/editorID.

- **`perksNot`**: An array of perks that the event source actor must *not* have. Same format as `perks`.

- **`spells`**: An array of spells that the event source actor must have. Format: `"modName:formID"` (e.g., `"Skyrim.esm:0xABCDEF"`), `"EditorIDName"` (e.g., `"VendorItemFood"`), or a formlist's formID/editorID.

- **`spellsNot`**: An array of spells that the event source actor must *not* have. Same format as `spells`.

- **`hasItem`**: An array of items that the event source actor must have in their inventory. Format: `"modName:formID"` (e.g., `"Skyrim.esm:0xABCDEF"`), `"EditorIDName"` (e.g., `"VendorItemFood"`), or a formlist's formID/editorID.

- **`hasItemNot`**: An array of items that the event source actor must *not* have. Same format as `hasItem`.

- **`actorKeywords`**: An array of keywords that the event source actor must have in their inventory. Format: `"modName:formID"` (e.g., `"Skyrim.esm:0xABCDEF"`), `"EditorIDName"` (e.g., `"VendorItemFood"`), or a formlist's formID/editorID.

- **`actorKeywordsNot`**: An array of keywords that the event source actor must *not* have. Same format as `actorKeywords`.

- **`actorRaces`**: An array of races that the event source actor must have in their inventory. Format: `"modName:formID"` (e.g., `"Skyrim.esm:0xABCDEF"`), `"EditorIDName"` (e.g., `"VendorItemFood"`), or a formlist's formID/editorID.

- **`actorRacesNot`**: An array of keywords that the event source actor must *not* have. Same format as `actorRaces`.

- **`actorValues`**: An array of actor value conditions that the event source actor must meet. Format: `["Health >= 10", "Aggression = 0"]`. Available entries:

  **Core Attributes:**
  - `Health`, `Magicka`, `Stamina`
  - `HealRate`, `HealRateMult`, `MagickaRate`, `MagickaRateMult`, `StaminaRate`, `StaminaRateMult`

  **AI Attributes:**
  - `Aggression`, `Confidence`, `Assistance`, `Morality`, `Mood`

  **Condition Values:**
  - `EnduranceCondition`, `PerceptionCondition`
  - `LeftAttackCondition`, `RightAttackCondition`
  - `LeftMobilityCondition`, `RightMobilityCondition`

  **Misc Values:**
  - `Fame`, `Infamy`, `Energy`, `JumpingBonus`, `MovementNoiseMult`
  - `CombatHealthRegenMultiply`, `LastBribedIntimidated`, `LastFlattered`
  - `FavorActive`, `FavorPointsBonus`, `FavorsPerDay`, `FavorsPerDayTimer`
  - `GrabActorOffset`, `Grabbed`, `WaitingForPlayer`, `IgnoreCrippledLimbs`

  **Combat Stats:**
  - `AttackDamageMult`, `CriticalChance`, `MeleeDamage`, `UnarmedDamage`
  - `WeaponSpeedMult`, `LeftWeaponSpeedMultiply`, `SpeedMult`
  - `DamageResist`, `ReflectDamage`

  **Equipment:**
  - `OneHanded`, `TwoHanded`, `Archery`, `Block`
  - `OneHandedModifier`, `OneHandedPowerModifier`, `OneHandedSkillAdvance`
  - `TwoHandedModifier`, `TwoHandedPowerModifier`, `TwoHandedSkillAdvance`
  - `MarksmanModifier`, `MarksmanPowerModifier`, `MarksmanSkillAdvance`
  - `LeftItemCharge`, `RightItemCharge`
  - `InventoryWeight`, `Mass`

  **Magic:**
  - `Destruction`, `Alteration`, `Restoration`, `Illusion`, `Conjuration`
  - `DestructionModifier`, `DestructionPowerModifier`, `DestructionSkillAdvance`
  - `AlterationModifier`, `AlterationPowerModifier`, `AlterationSkillAdvance`
  - `RestorationModifier`, `RestorationPowerModifier`, `RestorationSkillAdvance`
  - `IllusionModifier`, `IllusionPowerModifier`, `IllusionSkillAdvance`
  - `ConjurationModifier`, `ConjurationPowerModifier`, `ConjurationSkillAdvance`
  - `AbsorbChance`, `Invisibility`, `NightEye`, `Paralysis`, `Blindness`
  - `WaterBreathing`, `WaterWalking`, `Telekinesis`
  - `WardPower`, `WardDeflection`, `DetectLifeRange`
  - `ResistFire`, `ResistFrost`, `ResistShock`, `ResistMagic`
  - `ResistDisease`, `PoisonResist`

  **Shout Related:**
  - `DragonSouls`, `DragonRend`, `VoicePoints`, `VoiceRate`, `ShoutRecoveryMult`

  **Armor:**
  - `HeavyArmor`, `LightArmor`
  - `HeavyArmorModifier`, `HeavyArmorPowerModifier`, `HeavyArmorSkillAdvance`
  - `LightArmorModifier`, `LightArmorPowerModifier`, `LightArmorSkillAdvance`

  **Stealth:**
  - `Sneak`, `Lockpicking`, `Pickpocket`
  - `SneakingModifier`, `SneakingPowerModifier`, `SneakingSkillAdvance`
  - `LockpickingModifier`, `LockpickingPowerModifier`, `LockpickingSkillAdvance`
  - `PickpocketModifier`, `PickpocketPowerModifier`, `PickpocketSkillAdvance`

  **Crafting:**
  - `Smithing`, `Alchemy`, `Enchanting`
  - `SmithingModifier`, `SmithingPowerModifier`, `SmithingSkillAdvance`
  - `AlchemyModifier`, `AlchemyPowerModifier`, `AlchemySkillAdvance`
  - `EnchantingModifier`, `EnchantingPowerModifier`, `EnchantingSkillAdvance`

  **Speechcraft:**
  - `Speech`, `SpeechcraftModifier`, `SpeechcraftPowerModifier`, `SpeechcraftSkillAdvance`

  **Special Perks:**
  - `ArmorPerks`, `ShieldPerks`, `VampirePerks`, `WerewolfPerks`

- **`actorValuesNot`**: An array of actor value conditions that the event source actor must *not* meet. Same format as `actorValues`.

- **`level`**: An array of level conditions that the event source actor must meet. Format: `[">= 10", "= 20"]`.

- **`levelNot`**: An array of level conditions that the event source actor must *not* meet. Same format as `level`.

An integer specifying whether the event source actor is in the state:
- **`isSneaking`**: Sneaking.
- **`isSwimming`**: Swimming.
- **`isInCombat`**: Combat.
- **`isMounted`**: Mounted.
- **`isDualCasting`**: Dual casting.
- **`isSprinting`**: Sprinting.
- **`isWeaponDrawn`**: Drawn weapon.
- **`isFirstPerson`**: First person.
- **`isThirdPerson`**: Third person.
  - `0`: Source actor is *not* in this state.
  - `1`: Source actor is in this state.
  - `2` (default): All states allowed.

### Hit-Specific Filters

For rules with the `"Hit"` event, additional filters can refine which attacks trigger the rule:

- **`destructionStage`**: Checks with the current destruction stage. Possible values:
  - `-1` (default): All stages allowed.
  - `0`, `1`, etc.: Specific destruction stage index.

- **`weaponsTypes`**: An array of weapon or spell types that must be used. Possible values:
  - `"onehandsword"`: One-handed swords.
  - `"twohandsword"`: Two-handed swords.
  - `"onehandaxe"`: One-handed axes.
  - `"twohandaxe"`: Two-handed axes.
  - `"onehandmace"`: One-handed maces.
  - `"twohandmace"`: Two-handed maces.
  - `"dagger"`: Daggers.
  - `"ranged"`: Bows and crossbows.
  - `"staff"`: Staves.
  - `"handtohand"`: Unarmed attacks.
  - `"spell"`: Spells.
  - `"scroll"`: Scrolls.
  - `"shout"`: Shouts. **Note:** All shouts don't work with statics. Those shouts that are pure projectiles (e.g., Unrelenting Force) will work only with `furniture`, `flora`, `doors`, `containers` and `activators`.
  - `"ability"`: Racial or other abilities.
  - `"power"`: Greater powers.
  - `"lesserpower"`: Lesser powers.
  - `"explosion"`: Explosions.
  - `"total"`: Yet to discover. Supposedly matches all weapon types.
  - `"other"`: Miscellaneous types not covered above.
 
- **`weaponsTypesNot`**: An array of weapon or spell types that must *not* be used. Same format as `weaponsTypes`.

- **`weapons`**: An array of specific weapons or spells. Format: `"modName:formID"` (e.g., `"Skyrim.esm:0xABCDEF"`), `"EditorIDName"` (e.g., `"VendorItemFood"`), or a formlist's formID/editorID.

- **`weaponsNot`**: An array of specific weapons or spells that must *not* be used. Same format as `weapons`.

- **`weaponsKeywords`**: An array of keywords the weapon or spell must have. Format: `"modName:formID"` (e.g., `"Skyrim.esm:0xABCDEF"`), `"EditorIDName"` (e.g., `"VendorItemFood"`), or a formlist's formID/editorID.

- **`weaponsKeywordsNot`**: An array of keywords the weapon or spell must *not* have. Same format as `weaponsKeywords`.

- **`attacksTypes`** or **`attacks`**: An array of attack types. Possible values:
  - `"regular"` (default): Standard attacks.
  - `"power"`: Power attacks.
  - `"bash"`: Shield or weapon bashes.
  - `"charge"`: Charging attacks (e.g., sprinting strikes).
  - `"rotating"`: Continuous spinning attacks.
  - `"continuous"`: Concentration type spells (e.g., flames, frostbite).
  - `"constant"`: Contant type spells.
  - `"fireandforget"`: Fire-and-forget type spells.
  - `"ignoreweapon"`: Attacks that bypass weapon-specific mechanics.
  - `"overridedata"`: Attacks that override default data.

- **`attacksTypesNot`** or **`attacksNot`**: An array of attack types that must *not* be used. Same format as `attacksTypes`.

- **`deliveryTypes`**: An array of spell delivery types. Possible values:
  - `"self"`: Self-targeted spells.
  - `"aimed"`: Aimed spells requiring targeting.
  - `"targetactor"`: Actor-targeted spells.
  - `"targetlocation"`: Location-targeted spells.
  - `"touch"`: Touch-based spells.
  - `"total"`: Yet to discover. Supposedly matches all delivery types.
  - `"none"`: None.
 
- **`deliveryTypesNot`**: An array of spell delivery types that must *not* be used. Same format as `deliveryTypes`.

- **`allowProjectiles`**: An integer specifying whether projectiles are allowed. Possible values:
  - `0`: Projectiles are not allowed.
  - `1` (default): Projectiles are allowed.

- **`projectiles`**: An array of specific projectiles. Format: `"modName:formID"` (e.g., `"Skyrim.esm:0xABCDEF"`), `"EditorIDName"` (e.g., `"VendorItemFood"`), or a formlist's formID/editorID.

- **`projectilesNot`**: An array of specific projectiles that must *not* be used. Same format as `projectiles`.

---

## Effects: What Happens When the Rule Triggers

The `effect` field defines the outcome when a rule is triggered. Each effect has a mandatory `type` field and, for most types, an `items` array.

Here are all possible `type` values and their supported fields:

### Utility Effects
- **`ExecuteConsoleCommand`**: Executes a console command on nearby actors.
  - Supported fields: `string`, `chance`, `timer`.
 
- **`ExecuteConsoleCommandOnItem`**: Executes a console command on the target object.
  - Supported fields: `string`, `chance`, `timer`.
 
- **`ExecuteConsoleCommandOnSource`**: Executes a console command on an actor who interacted with the target object.
  - Supported fields: `string`, `chance`, `timer`.

- **`ShowNotification`**: Shows a notification.
  - Supported fields: `string`, `chance`, `timer`.

- **`ShowMessageBox`**: Shows a message box with "OK" button.
  - Supported fields: `string`, `chance`, `timer`.

### Object Management
- **`RemoveItem`**: Deletes the target object.
  - No `items` array required.

- **`EnableItem`**: Enables the target object.
  - No `items` array required.

- **`DisableItem`**: Disables the target object.
  - No `items` array required.

- **`UnlockItem`**: Unlocks the target object (with animation).
  - No `items` array required.
 
- **`LockItem`**: Locks the target object (with animation).
  - No `items` array required.

- **`ActivateItem`**: Activates the target object. Behaves the same as pressing `E` for all form types. Does *not* work with the `Activate` event to avoid collisions.
  - No `items` array required.
 
### Inventory Management
- **`SpillInventory`**: Spills the contents of the target container.
  - No `items` array required.

- **`AddContainerItem`**: Adds specified items to the contents of the target container.
  - Supported fields: `formID`, `editorID`, `formList`, `chance`, `timer`.
 
- **`AddActorItem`**: Adds specified items to the inventory of an actor who interacted with the target object.
  - Supported fields: `formID`, `editorID`, `formList`, `chance`, `timer`.
 
- **`RemoveContainerItem`**: Removes specified items from the contents of the target container.
  - Supported fields: `formID`, `editorID`, `formList`, `chance`, `timer`.
 
- **`RemoveActorItem`**: Removes specified items from the inventory of an actor who interacted with the target object.
  - Supported fields: `formID`, `editorID`, `formList`, `chance`, `timer`.

### Item Spawning & Swapping
- **`SpawnItem`**: Spawns specific items at the target object's location.
  - Supported fields: `formID`, `editorID`, `formList`, `count`, `scale`, `fade`, `spawnType`, `string`, `chance`, `timer`.

- **`SpawnLeveledItem`**: Spawns random leveled items at the target object's location (based on the player's level).
  - Supported fields: `formID`, `editorID`, `formList`, `count`, `scale`, `fade`, `spawnType`, `string`, `chance`, `timer`.

- **`SwapItem`**: Replaces the target object with another specified item.
  - Supported fields: `formID`, `editorID`, `formList`, `count`, `scale`, `fade`, `spawnType`, `string`, `nonDeletable`, `chance`, `timer`.

- **`SwapLeveledItem`**: Replaces the target object with a random leveled item (based on the player's level).
  - Supported fields: `formID`, `editorID`, `formList`, `count`, `scale`, `fade`, `spawnType`, `string`, `nonDeletable`, `chance`, `timer`.

### Actor Spawning & Swapping
- **`SpawnActor`**: Spawns specified actors at the target object's location.
  - Supported fields: `formID`, `editorID`, `formList`, `count`, `scale`, `fade`, `spawnType`, `string`, `chance`, `timer`.

- **`SpawnLeveledActor`**: Spawns random leveled actors at the target object's location (based on the player's level).
  - Supported fields: `formID`, `editorID`, `formList`, `count`, `scale`, `fade`, `spawnType`, `string`, `chance`, `timer`.

- **`SwapActor`**: Replaces the target object with specified actors.
  - Supported fields: `formID`, `editorID`, `formList`, `count`, `scale`, `fade`, `spawnType`, `string`, `nonDeletable`, `chance`, `timer`.

- **`SwapLeveledActor`**: Replaces the target object with random leveled actors (based on the player's level).
  - Supported fields: `formID`, `editorID`, `formList`, `count`, `scale`, `fade`, `spawnType`, `string`, `nonDeletable`, `chance`, `timer`.

### Magic Effects
- **`SpawnSpell`**: Casts specified spells on nearby actors.
  - Supported fields: `formID`, `editorID`, `formList`, `count`, `radius`, `chance`, `timer`.

- **`SpawnLeveledSpell`**: Casts random leveled spells on nearby actors (based on the player's level).
  - Supported fields: `formID`, `editorID`, `formList`, `count`, `radius`, `chance`, `timer`.

- **`SpawnSpellOnItem`**: Casts specified spells on the target object.
  - Supported fields: `formID`, `editorID`, `formList`, `chance`, `chance`, `timer`.

- **`SpawnLeveledSpellOnItem`**: Casts random leveled spells on the target object (based on the player's level).
  - Supported fields: `formID`, `editorID`, `formList`, `count`, `chance`, `timer`.

- **`ApplySpell`**: Applies the target object's spell (if it's a scroll or a spell tome) to nearby actors.
  - Supported fields: `radius`, `chance`, `timer`.

- **`ApplyIngestible`**: Applies the target object's effects (if it's an ingredient or an ingestible) to nearby actors.
  - Supported fields: `radius`, `chance`, `timer`.

- **`ApplyOtherIngestible`**: Applies effects from specified ingestibles to nearby actors. Can be used with any form type.
  - Supported fields: `formID`, `editorID`, `formList`, `radius`, `chance`, `timer`.
 
- **`AddActorSpell`**: Adds specified spells to the source actor.
  - Supported fields: `formID`, `editorID`, `formList`, `chance`, `timer`.

- **`RemoveActorSpell`**: Removes specified spells from the source actor.
  - Supported fields: `formID`, `editorID`, `formList`, `chance`, `timer`.

- **`AddActorPerk`**: Adds specified perks to the source actor.
  - Supported fields: `formID`, `editorID`, `formList`, `rank`, `chance`, `timer`.

- **`RemoveActorPerk`**: Removes specified perks from the source actor (regardless of the perk rank).
  - Supported fields: `formID`, `editorID`, `formList`, `chance`, `timer`.

### Visual & Audio Effects
- **`PlaySound`**: Plays a sound descriptor on the target object.
  - Supported fields: `formID`, `editorID`, `formList`, `count`, `chance`, `timer`.

- **`PlayIdle`**: Plays an animation on an actor who interacted with the target object. [List of available animation names](https://forums.nexusmods.com/topic/11007808-le-list-of-animation-events-for-debugsendanimationevent/?do=findComment&comment=105617168).
  - Supported fields: `string`, `duration`, `chance`, `timer`.
 
- **`SpawnImpactDataSet`**: Plays an impact data set on the target object (not to be confused with impacts).
  - Supported fields: `formID`, `editorID`, `formList`, `count`, `chance`, `timer`.

- **`SpawnExplosion`**: Triggers an explosion at the target object's location.
  - Supported fields: `formID`, `editorID`, `formList`, `count`, `fade`, `spawnType`, `chance`, `timer`.

- **`SpawnEffectShader`**: Spawns effect shaders on nearby actors.
  - Supported fields: `formID`, `editorID`, `formList`, `count`, `radius`, `duration`, `chance`, `timer`.

- **`SpawnEffectShaderOnItem`**: Spawns effect shaders on the target object.
  - Supported fields: `formID`, `editorID`, `formList`, `count`, `duration`, `chance`, `timer`.

- **`SpawnArtObject`**: Spawns art objects on nearby actors.
  - Supported fields: `formID`, `editorID`, `formList`, `count`, `radius`, `duration`, `chance`, `timer`.

- **`SpawnArtObjectOnItem`**:  Spawns art objects on the target object.
  - Supported fields: `formID`, `editorID`, `formList`, `count`, `duration`, `chance`, `timer`.
 
- **`ToggleNode`**: Toggles the target object's nodes on and off (scales to 0.00001 or 1.0).
  - Supported fields: `mode`, `strings`, `chance`, `timer`.

### Lighting Effects
- **`SpawnLight`**: Spawns a light at the target object's location.
  - Supported fields: `formID`, `editorID`, `formList`, `count`, `fade`, `spawnType`, `string`, `chance`, `timer`.

- **`RemoveLight`**: Deletes lights.
  - Supported fields: `radius`, `chance`, `timer`.

- **`DisableLight`**: Disables lights.
  - Supported fields: `radius`, `chance`, `timer`.

- **`EnableLight`**: Enables previously disabled lights.
  - Supported fields: `radius`, `chance`, `timer`.
 
---

### Configuring Effects with `items`

For effect types that support an `items` array, you can specify detailed configurations. Each item in the array can include:

- **`formID`**: A specific formID in `"modName:formID"` format (e.g., `"Skyrim.esm:0xF"` for a gold coin).

- **`editorID`**: A specific editorID in `"EditorIDName` format (e.g., `"VendorItemClutter"` for clutter).

- **`formList`**: An array of formlist entries.
  - **`formID`**: The formlist's formID in `"modName:formID"` format (use this or `editorID`).
  - **`editorID`**: The formlist's editorID in `"EditorIDName"` format (use this or `formID`).
  - **`index`** (optional): An integer specifying which item in the formlist to use:
    - `-1` (default): All items at once.
    - `-2`: Parallel relationship for mirrored formlists.
    - `-3`: Select one random object from the list.
    - Other values: Specific index in the formlist.
    
    Example:
    ```json
    "formList": [
        {"formID": "Skyrim.esm:0x123456", "index": 0},
        {"formID": "MyMod.esp:0x789ABC", "index": -3}
    ]
    ```

- **`chance`**: A number between 0 and 100 for the percentage chance this item is used (e.g., `"chance": 90.0`). Defaults to `100`. 
  **Optional** detailed entry (e.g., `"chance": {"min": 0.1, "max": 85.0}`):
  - **`min`**: Minimal random value.
  - **`max`**: Maximal random value.

- **`timer`**: A defined number of seconds before triggering the effect (e.g., `"timer": 1.0`). **Optional** detailed entry has:
  - **`time`**: Number of seconds. (e.g., `"timer": {"time": 1.0}`).
    **Optional** detailed entry (e.g., `"timer": {"time": {"min": 1.0, "max": 10.0}}`):
    - **`min`**: Minimal random value.
    - **`max`**: Maximal random value.
  - **`matchFilterRecheck`**: Whether the effect needs to be canceled if conditions were violated while waiting.
    - `0`: No re-check.
    - `1`: Re-check.

    ```json
    "timer": {"time": 1.0, "matchFilterRecheck": 1}
    ```

- **`count`**: An integer specifying how many instances to spawn or how many times to perform a particular action (e.g., `"count": 2`). Defaults to `1`. 
  **Optional** detailed entry (e.g., `"count": {"min": 1, "max": 5}`):
  - **`min`**: Minimal random value.
  - **`max`**: Maximal random value.

- **`radius`**: Specifies the radius in game units for effect application (e.g., `"radius": 100`). Defaults to `100`. 
  **Optional** detailed entry (e.g., `"radius": {"min": 50, "max": 150}`):
  - **`min`**: Minimal random value.
  - **`max`**: Maximal random value.
 
- **`scale`**: Used for `spawn`/`swap` functions only. Allows to select the scale of a spawned item (e.g., `"scale": 2.0`). By default, it is copied from the target object.
  **Optional** detailed entry (e.g., `"scale": {"min": 1.5, "max": 3.0}`):
  - **`min`**: Minimal random value.
  - **`max`**: Maximal random value.

- **`duration`**: For `PlayIdle`, defaults to 1.0 (lower values make animation faster). For **effect shaders** and **art objects**, specifies how long the effect lasts.

- **`string`**: Used for various effects. Takes one entry. For **spawn** and **swap** effects, used to take node name. For `PlayIdle`, used to take animation name.

- **`strings`**: Used for various effects. Takes multiple entries. For `ToggleNode`, used to take multiple node names.

- **`mode`**: Used for various effects. Currently used for `ToggleNode` effect to specify the disable or enable mode.
  - `0` (default): Disable.
  - `1`: Enable.
 
- **`nonDeletable`**: Used for `swap` functions only. During swap, the original object is deactivated and a new one appears in its place. This value determines whether the original object is deleted (`0`) or only disabled (`1`).

- **`fade`**: Used for `spawn`/`swap` functions only. Determines whether the object will have a fade effect upon creation:
  - `0`: Without fade effect (do **not** use with **explosions**).
  - `1` (default): With fade effect.

- **`spawnType`**: Used for `spawn`/`swap` functions only. Allows you to select the type of how the object should be spawned. Options:
  - `0`: Common PlaceAtMe().
  - `1`: PlaceAtMe() spawning the object at the center of the original.
  - `2`: PlaceAtMe() spawning at the top of the original.
  - `3`: PlaceAtMe() spawning at the bottom of the original.
  - `4` (default): Common PlaceAtMe() with buggy engine physics bypassing (e.g., when used with `Throw`/`Grab`/`Release`, the object will be spawned exactly to the landing point without position glitches).
  - `5`: Bypass with spawning the object at the center of the original.
  - `6`: Bypass with spawning at the top of the original.
  - `7`: Bypass with spawning at the bottom of the original.
  - `8`: Pin to the ground regardless of the landing location (e.g., when used with `Throw`, an object that lands on the wall will spawn a new one directly beneath it on the floor).
  - `9`: Pin to the specified node. Requires a node name or it's substring to be passed in `string`.

  **NOTE**: If you want to spawn **explosions**, use `0` or `4` only.

---

## Examples

1. **Spawn a Gold Coin on Activation**
   ```json
   [
       {
           "event": ["Activate"],
           "filter": {
               "formTypes": ["container"]
           },
           "effect": [{
               "type": "SpawnItem",
               "items": [{"formID": "Skyrim.esm:0xF"}]
           }]
       }
   ]
   ```
   - Activating a container spawns one gold coin.

2. **Delete an Object When Hit**
   ```json
   [
       {
           "event": ["Hit"],
           "filter": {
               "formTypes": ["static"]
           },
           "effect": [{
               "type": "RemoveItem"
           }]
       }
   ]
   ```
   - Hitting a static object removes it.

3. **Spawn Items with Chances and Custom Scale**
   ```json
   [
       {
           "event": ["Hit"],
           "filter": {
               "formTypes": ["container"]
           },
           "effect": [{
               "type": "SpawnItem",
               "items": [
                   {"formID": "Skyrim.esm:0xF", "count": 5, "chance": 50, "scale": 1.5},
                   {"formID": "Skyrim.esm:0xA", "count": 2, "chance": 30, "fade": 1}
               ]
           }]
       }
   ]
   ```
   - Hitting a container has a 50% chance to spawn 5 gold coins at 1.5x scale and a 30% chance to spawn 2 lockpicks with fade effect.

4. **Explode Trees on Hit**
   ```json
   [
       {
           "event": ["Hit"],
           "filter": {
               "formTypes": ["tree"]
           },
           "effect": [{
               "type": "SpawnExplosion",
               "items": [{"formID": "Skyrim.esm:0x123456"}]
           }]
       }
   ]
   ```
   - Hitting a tree triggers an explosion.

5. **Use FormLists with Random Selection**
   ```json
   [
       {
           "event": ["Activate"],
           "filter": {
               "formTypes": ["container"]
           },
           "effect": [{
               "type": "SpawnItem",
               "items": [
                   {
                       "formList": [
                           {"formID": "MyMod.esp:0x789ABC", "index": 2},
                           {"formID": "Skyrim.esm:0x123456", "index": -3}
                       ],
                       "count": 1,
                       "chance": 100
                   }
               ]
           }]
       }
   ]
   ```
   - Activating a container spawns an item from the formlist at index 2 in `MyMod.esp:0x789ABC` and one random item from `Skyrim.esm:0x123456`.

6. **Play Animation on Hit**
   ```json
   [
       {
           "event": ["Hit"],
           "filter": {
               "formTypes": ["activator"]
           },
           "effect": [{
               "type": "PlayIdle",
               "items": [{"string": "IdleName", "duration": 0.5}]
           }]
       }
   ]
   ```
   - Hitting an activator plays the "IdleName" animation at 0.5x speed.

7. **Spawn Effect Shader with Custom Duration**
   ```json
   [
       {
           "event": ["Activate"],
           "filter": {
               "formTypes": ["static"]
           },
           "effect": [{
               "type": "SpawnEffectShaderOnItem",
               "items": [
                   {
                       "formID": "Skyrim.esm:0x123456",
                       "duration": 5.0
                   }
               ]
           }]
       }
   ]
   ```
   - Activating a static object spawns an effect shader on it for 5 seconds.

8. **Weather and Actor Value Dependent Effects**
   ```json
   [
       {
           "event": ["Hit"],
           "filter": {
               "formTypes": ["tree"],
               "weathers": ["Skyrim.esm:0x123456"],
               "actorValues": ["Health >= 50"],
               "level": [">= 10"]
           },
           "effect": [{
               "type": "SpawnSpell",
               "items": [
                   {
                       "formID": "Skyrim.esm:0x789ABC",
                       "radius": 500
                   }
               ]
           }]
       }
   ]
   ```
   - Hitting a tree during specific weather by an actor with at least 50 health and level 10+ casts a spell on actors within 500 units.

9. **Quest Item Filtering with Inventory Check**
    ```json
    [
        {
            "event": ["Grab"],
            "filter": {
                "formTypes": ["misc"],
                "questItemStatus": 2,
                "hasItem": ["Skyrim.esm:0x123456"]
            },
            "effect": [{
                "type": "PlaySound",
                "items": [{"formID": "Skyrim.esm:0x456789"}]
            }]
        }
    ]
    ```
    - Grabbing a full-fledged quest item by an actor who has a specific item plays a sound.

10. **Apply Ingestible with Custom Radius**
    ```json
    [
        {
            "event": ["Activate"],
            "filter": {
                "formTypes": ["ingredient"]
            },
            "effect": [{
                "type": "ApplyIngestible",
                "items": [{"radius": 300, "chance": 75}]
            }]
        }
    ]
    ```
    - Activating an ingredient applies its effects to actors within 300 units with 75% chance.
