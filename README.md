# Object Impact Framework

Object Impact Framework (OIF) is a SKSE plugin that lets you add custom effects to items when they are activated, grabbed, released, thrown, or hit. You can make things happen like spawning items, impact data sets, actors, casting spells, creating explosions, playing sounds, and swapping objects - all controlled with JSON files. Compatible with everything.

This guide explains how to set up those JSON files so you can customize the mod however you like. Examples are given below.

## Users Info

- Place your JSON files in: `Data/SKSE/Plugins/ObjectImpactFramework/`
- **Check the Log**: If something doesn't work, look at the mod's log file for error messages. The log can be found in `Documents/My Games/Skyrim Special Edition/SKSE/ObjectImpactFramework.log`

## Mod Authors Info

- **Filter Note**: An object must be defined by at least one of the three parameters - `formIDs`, `editorIDs`, `formLists`, `formTypes`, `keywords` - for the event to work.
- **FormIDs Note**: If you are not using the standard `0x` prefix and are copying from CK or SSEEdit, make sure you put `00` instead of the load order digits.
- **Identifiers Must Match**: Make sure the `formID` or `editorID` fits the effect (e.g., a spell ID for `"SpawnSpell"`, an item ID for `"SpawnItem"`).
- **Keywords Note**: For containers, doors, statics, movable statics, and trees, the `keywords` filter is ignored since they don't use keywords.
- **Priority Note**: Place effects with `"Remove"` and `"Swap"` (unless used with `"nonDeletable": 1` flag) prefixes at the very end of the event, otherwise the removed object may not have time to call other effects on itself before gets deleted.
- You can modify existing JSON files without quitting the game, edit the file and reload the save.

---

## Table of Contents

- [Basic Rule Structure](#basic-rule-structure)
- [Filters: Choosing Which Objects to Affect and How](#filters-choosing-which-objects-to-affect-and-how)
  - [General Filters](#general-filters)
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
  
- **`filter`**: Defines the conditions under which the rule applies. This is an object that specifies which objects or interactions the rule targets. At least one of `formTypes`, `formIDs`, `formLists`, or `keywords` must be provided to identify target objects.

- **`effect`**: Describes what happens when the rule is triggered. This can be a single effect (an object) or multiple effects (an array of objects). Each effect has a `type` and, for most types, an optional `items` array specifying what to spawn, swap, or apply.

Here's a basic example of a rule:

```json
[
    {
        "event": ["Activate", "Hit"],
        "filter": {
            "formTypes": ["activator"]
        },
        "effect": {
            "type": "SpawnItem",
            "items": [{"formID": "Skyrim.esm:0xF"}]
        }
    }
]
```

This rule triggers when an activator (e.g., a lever) is activated or hit, spawning a single gold coin (`Skyrim.esm:0xF`).

---

## Filters: Choosing Which Objects to Affect and How

The `filter` object determines which objects and interactions trigger a rule. It's highly customizable, allowing precise control over when effects occur. Below are all possible filter parameters:

### General Filters

- **`formTypes`** (optional): An array of strings specifying the types of objects the rule applies to. Possible values:
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
  - `"static"`: Non-movable world objects (e.g., statues).
  - `"moveablestatic"`: Movable static objects.
  - `"tree"`: Trees.
  - `"light"`: Lights with 3D models (e.g., torches).
 
- **`formTypesNot`** (optional): An array of strings specifying the types of objects that the rule should *not* apply to, in the same format as `formTypes`.

- **`formIDs`** (optional): An array of strings identifying specific objects by their Form ID in the format `"modName:formID"`. Examples:
  - `"Skyrim.esm:0x123456"` (for esp/esm plugins).
  - `"MyMod.esl:0x456"` (for esl/espfe plugins).
  - `"Dawnguard.esm:00123456"` (alternate format with leading zeros (esp/esm)).
  - `"MyMod.esp:FE000800"` (alternate format with leading FE prefix (esl/espfe)).

- **`formIDsNot`** (optional): An array of strings identifying specific objects that the rule should *not* apply to, in the same format as `formIDs`.

- **`editorIDs`** (optional): An array of strings identifying specific objects by their Editor ID in the format `"EditorIDName"`. Example: `"VendorItemClutter"`.

- **`editorIDsNot`** (optional): An array of strings identifying specific objects that the rule should *not* apply to, in the same format as `editorIDs`.

- **`formLists`** (optional): An array of objects referencing formlists (lists of forms defined in a mod). Each entry has:
  - **`formID`**: The formlist's form ID in `"modName:formID"` format (required (or editorID)).
  - **`editorID`**: The formlist's editor ID in `"EditorIDName"` format (required (or formID)).
  - **`index`** (optional): An integer specifying which item in the formlist to use. Example:
 
    ```json
    "formLists": [
        {"formID": "Skyrim.esm:0x123456", "index": 0},
        {"editorID": "ExampleFormList"}
    ]
    ```
    - **Index `-1`** is default, means that all the items at once will be used. 
    - **Index `-2`** defines the parrallel relationship for mirrored formlists: matching positions between filter and items formlists correspond directly, allowing ordered transformations (raw meat at position 0 in formlist 1 becomes cooked meat at position 0 in formlist 2).

- **`formListsNot`** (optional): An array of formlist objects that the rule should *not* apply to, in the same format as `formLists`.

- **`keywords`** (optional): An array of keywords that the object must have. Format can be `"modName:formID"` (e.g., `"Skyrim.esm:0xABCDEF"`) or a keyword name (e.g., `"VendorItemFood"`). Ignored for containers, doors, statics, movable statics, and trees.

- **`keywordsNot`** (optional): An array of keywords the object must *not* have, in the same format as `keywords`.

- **`questItemStatus`** (optional): An integer specifying quest item status requirements. Only works with **ACTIVE** player quests:
  - `0`: Object must not be a quest item (default).
  - `1`: Object must be a quest alias only.
  - `2`: Object must be a full-fledged quest item.
  - `3`: All objects allowed.
 
- **`isInitiallyDisabled`** (optional): An integer specifying whether the object has the `kInitiallyDisabled` flag.
  - `0`: Object is not initially disabled.
  - `1`: Object is initially disabled.
  - `2`: All objects allowed (default).

- **`locations`** (optional): An array of Form IDs or formlist Form IDs referencing cells, locations, or worldspaces where the rule should apply. Format: `"modName:formID"`.

- **`locationsNot`** (optional): An array of Form IDs or formlist Form IDs referencing cells, locations, or worldspaces where the rule should not apply. Same format as `locations`.

- **`weathers`** (optional): An array of weather Form IDs or formlist Form IDs containing weathers that must be active for the rule to apply. Format: `"modName:formID"`.

- **`weathersNot`** (optional): An array of weather Form IDs or formlist Form IDs that must *not* be active, in the same format as `weathers`.

- **`perks`** (optional): An array of perk Form IDs that the event source actor must have. Format: `"modName:formID"`.

- **`perksNot`** (optional): An array of perk Form IDs that the event source actor must *not* have, in the same format as `perks`.

- **`actorValues`** (optional): An array of actor value conditions that the event source actor must meet. Format: `["Health >= 10", "Aggression = 0"]`.

- **`actorValuesNot`** (optional): An array of actor value conditions that the event source actor must *not* meet, in the same format as `actorValues`.

- **`level`** (optional): An array of level conditions that the event source actor must meet. Format: `[">= 10", "= 20"]`.

- **`levelNot`** (optional): An array of level conditions that the event source actor must *not* meet, in the same format as `level`.

- **`hasItem`** (optional): An array of item Form IDs that the event source actor must have in their inventory. Format: `"modName:formID"`.

- **`hasItemNot`** (optional): An array of item Form IDs that the event source actor must *not* have, in the same format as `hasItem`.

- **`isPluginInstalled`** (optional): An array of plugin names (e.g., `"MyMod.esp"`, `"Skyrim.esm"`) that must be loaded for the rule to apply.

- **`isPluginNotInstalled`** (optional): An array of plugin names that must *not* be loaded.

- **`isDllInstalled`** (optional): An array of DLL filenames (e.g., `"MyPlugin.dll"`) that must be present in `Data/SKSE/Plugins/`.

- **`isDllNotInstalled`** (optional): An array of DLL filenames that must *not* be present.

- **`chance`** (optional): A number between 0 and 100 representing the percentage chance the rule triggers. Defaults to 100 if omitted.

- **`interactions`** (optional): An integer specifying how many interactions (e.g., hits or activations) are required before the effect triggers. Defaults to 1. Now works with all event types.

- **`limit`** (optional): An integer setting the maximum number of times the rule can trigger per object. No limit if omitted. Now works with all event types.

### Hit-Specific Filters

For rules with the `"Hit"` event, additional filters can refine which attacks trigger the rule:

- **`weaponsTypes`** (optional): An array of weapon or spell types that must be used. Possible values:
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
 
- **`weaponsTypesNot`** (optional): An array of weapon or spell types that must *not* be used, in the same format as `weaponsTypes`.

- **`weapons`** (optional): An array of specific weapon or spell Form IDs in `"modName:formID"` format (e.g., `"Skyrim.esm:0x1A2B3C"`).

- **`weaponsNot`** (optional): An array of specific weapon or spell Form IDs that must *not* be used, in the same format as `weapons`.

- **`weaponsFormLists`** (optional): An array of formlist Form IDs, containing specific weapons or spells in `"modName:formID"` format (e.g., `"Skyrim.esm:0x1A2B3C"`).

- **`weaponsFormListsNot`** (optional): An array of formlist Form IDs, containing specific weapons or spells that must *not* be used, in the same format as `weaponsFormLists`.

- **`weaponsKeywords`** (optional): An array of keywords the weapon or spell must have, in `"modName:formID"` or `"KeywordName"` format.

- **`weaponsKeywordsNot`** (optional): An array of keywords the weapon or spell must *not* have.

- **`attacksTypes`** or **`attacks`** (optional): An array of attack types. Possible values:
  - `"regular"`: Standard attacks.
  - `"power"`: Power attacks.
  - `"bash"`: Shield or weapon bashes.
  - `"charge"`: Charging attacks (e.g., sprinting strikes).
  - `"rotating"`: Continuous spinning attacks.
  - `"continuous"`: Concentration type spells (e.g., flames, frostbite).
  - `"constant"`: Contant type spells.
  - `"fireandforget"`: Fire-and-forget type spells.
  - `"ignoreweapon"`: Attacks that bypass weapon-specific mechanics.
  - `"overridedata"`: Attacks that override default data.

- **`attacksTypesNot`** or **`attacksNot`** (optional): An array of attack types that must *not* be used, in the same format as `attacksTypes`.

- **`deliveryTypes`** (optional): An array of spell delivery types. Possible values:
  - `"self"`: Self-targeted spells.
  - `"aimed"`: Aimed spells requiring targeting.
  - `"targetactor"`: Actor-targeted spells.
  - `"targetlocation"`: Location-targeted spells.
  - `"touch"`: Touch-based spells.
  - `"total"`: All delivery types.
 
- **`deliveryTypesNot`** (optional): An array of spell delivery types that must *not* be used, in the same format as `deliveryTypes`.

- **`allowProjectiles`** (optional): Controls whether projectiles are allowed. Possible values:
  - `0`: Projectiles are not allowed.
  - `1`: Projectiles are allowed (default).

- **`projectiles`** (optional): An array of specific projectile Form IDs in `"modName:formID"` format (e.g., `"Skyrim.esm:0xDEF123"`).

- **`projectilesNot`** (optional): An array of specific projectile Form IDs that must *not* be used, in the same format as `projectiles`.

- **`projectilesFormLists`** (optional): An array of formlist Form IDs, containing specific projectiles, in `"modName:formID"` format.

- **`projectilesFormListsNot`** (optional): An array of formlist Form IDs, containing specific projectiles that must *not* be used, in the same format as `projectilesFormLists`.

---

## Effects: What Happens When the Rule Triggers

The `effect` field defines the outcome when a rule is triggered. It can be a single effect (an object) or multiple effects (an array of objects). Each effect has a mandatory `type` field and, for most types, an optional `items` array.

Here are all possible `type` values and their supported fields:

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

- **`ActivateItem`**: Activates the target object. Behaves the same as pressing `E` for all form types.
  - No `items` array required.

- **`ToggleNode`**: Toggles nodes on and off (scales to 0.00001 or 1.0).
  - Supported fields: `mode`, `nodeNames`.
 
### Inventory Management
- **`SpillInventory`**: Spills the contents of a container.
  - No `items` array required.

- **`AddContainerItem`**: Adds specified items to the contents of a container.
  - Supported fields: `formID`, `formList`, `chance`.
 
- **`AddActorItem`**: Adds specified items to the inventory of an actor who interacted with the object.
  - Supported fields: `formID`, `formList`, `chance`.
 
- **`RemoveContainerItem`**: Removes specified items from the contents of a container.
  - Supported fields: `formID`, `formList`, `chance`.
 
- **`RemoveActorItem`**: Removes specified items from the inventory of an actor who interacted with the object.
  - Supported fields: `formID`, `formList`, `chance`.

### Item Spawning & Swapping
- **`SpawnItem`**: Spawns specific items at the object's location.
  - Supported fields: `formID`, `formList`, `chance`, `count`, `scale`, `fade`, `spawnType`.

- **`SpawnLeveledItem`**: Spawns random leveled items based on the player's level.
  - Supported fields: `formID`, `chance`, `count`, `scale`, `fade`, `spawnType`.

- **`SwapItem`**: Replaces the target object with another specific item.
  - Supported fields: `formID`, `formList`, `chance`, `count`, `nonDeletable`, `scale`, `fade`, `spawnType`.

- **`SwapLeveledItem`**: Replaces the target object with a random leveled item.
  - Supported fields: `formID`, `chance`, `count`, `nonDeletable`, `scale`, `fade`, `spawnType`.

### Actor Spawning & Swapping
- **`SpawnActor`**: Spawns specific actors at the object's location.
  - Supported fields: `formID`, `formList`, `chance`, `count`, `scale`, `fade`, `spawnType`.

- **`SpawnLeveledActor`**: Spawns random leveled actors.
  - Supported fields: `formID`, `chance`, `count`, `scale`, `fade`, `spawnType`.

- **`SwapActor`**: Replaces the target object with specific actors.
  - Supported fields: `formID`, `formList`, `chance`, `count`, `nonDeletable`, `scale`, `fade`, `spawnType`.

- **`SwapLeveledActor`**: Replaces the target object with random leveled actors.
  - Supported fields: `formID`, `chance`, `count`, `nonDeletable`, `scale`, `fade`, `spawnType`.

### Magic Effects
- **`SpawnSpell`**: Casts spells on nearby actors.
  - Supported fields: `formID`, `formList`, `chance`, `count`, `radius`.

- **`SpawnLeveledSpell`**: Casts random leveled spells on nearby actors.
  - Supported fields: `formID`, `chance`, `count`, `radius`.

- **`SpawnSpellOnItem`**: Casts spells on the target object.
  - Supported fields: `formID`, `formList`, `count`, `chance`.

- **`SpawnLeveledSpellOnItem`**: Casts random leveled spells on the target object.
  - Supported fields: `formID`, `chance`, `count`.

- **`ApplyIngestible`**: Applies the target object's effects (if it's an ingredient or ingestible) to nearby actors.
  - Supported fields: `chance`, `radius`.

- **`ApplyOtherIngestible`**: Applies effects from specified ingestibles to nearby actors. Can be used with any form type.
  - Supported fields: `formID`, `formList`, `chance`, `radius`.

### Visual & Audio Effects
- **`SpawnImpactDataSet`**: Plays an impact data set (not to be confused with impacts).
  - Supported fields: `formID`, `formList`, `chance`, `count`.

- **`SpawnExplosion`**: Triggers an explosion at the object's location.
  - Supported fields: `formID`, `formList`, `chance`, `count`, `fade`, `spawnType`.

- **`SpawnEffectShader`**: Spawns effect shaders on nearby actors.
  - Supported fields: `formID`, `formList`, `chance`, `count`, `radius`, `duration`.

- **`SpawnEffectShaderOnItem`**: Spawns effect shaders on the target object.
  - Supported fields: `formID`, `formList`, `chance`, `count`, `duration`.

- **`PlaySound`**: Plays a sound descriptor.
  - Supported fields: `formID`, `formList`, `chance`, `count`.

- **`PlayIdle`**: Plays an animation on an actor who interacted with the object.
  - Supported fields: `string`, `duration`.

### Lighting Effects
- **`SpawnLight`**: Spawns a light at the object's location.
  - Supported fields: `formID`, `formList`, `chance`, `count`, `fade`, `spawnType`.

- **`RemoveLight`**: Deletes lights.
  - Supported fields: `formID`, `formList`, `chance`, `radius`.

- **`DisableLight`**: Disables lights.
  - Supported fields: `formID`, `formList`, `chance`, `radius`.

- **`EnableLight`**: Enables previously disabled lights.
  - Supported fields: `formID`, `formList`, `chance`, `radius`.
 
---

### Configuring Effects with `items`

For effect types that support an `items` array, you can specify detailed configurations. Each item in the array can include:

- **`formID`** (optional): A specific Form ID in `"modName:formID"` format (e.g., `"Skyrim.esm:0xF"` for a gold coin).

- **`editorID`** (optional): A specific Editor ID in `"EditorIDName` format (e.g., `"VendorItemClutter"` for clutter).

- **`formList`** (optional): An array of formlist entries.
  - **`formID`**: The formlist's form ID in `"modName:formID"` format.
  - **`editorID`**: The formlist's editor ID in `"EditorIDName"` format.
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

- **`chance`** (optional): A number between 0 and 100 for the percentage chance this item is used. Defaults to 100.

- **`count`** (optional): An integer specifying how many instances to spawn or swap. Defaults to 1.

- **`radius`** (optional): Specifies the radius in game units for effect application. Defaults vary by effect type.

- **`duration`** (optional): Used for `PlayIdle`, `SpawnEffectShader`, and `SpawnEffectShaderOnItem` effects. For `PlayIdle`, defaults to 1.0 (lower values make animation faster). For effect shaders, specifies how long the effect lasts.

- **`string`** (optional): Used for various effects. Takes one entry. Currently used for `PlayIdle` effect to specify animation name (e.g., `"AnimationName"`). [List of available animation names](https://forums.nexusmods.com/topic/11007808-le-list-of-animation-events-for-debugsendanimationevent/?do=findComment&comment=105617168).

- **`strings`** (optional): Used for various effects. Takes multiple entries. Currently used for `ToggleNode` effect to specify node names substrings.

- **`mode`** (optional): Used for various effects. Currently used for `ToggleNode` effect to specify the disable or enable mode.
  - `0`: Disable (default).
  - `1`: Enable.
 
- **`nonDeletable`** (optional): Used for **swap** functions only. During swap, the original object is deactivated and a new one appears in its place. This value determines whether the original object is deleted (`0`) or only disabled (`1`).

- **`scale`** (optional): Used for **spawn/swap** functions only. Allows you to select the scale of the spawned item. By default, it is copied from the target object.

- **`fade`** (optional): Used for **spawn/swap** functions only. Determines whether the object will have a fade effect upon creation:
  - `0`: Without fade effect (do **not** use with **explosions**).
  - `1` (default): With fade effect.

- **`spawnType`** (optional): Used for **spawn/swap** functions only. Allows you to select the type of how the object should be spawned. Options:
  - `0`: Common PlaceAtMe().
  - `1`: PlaceAtMe() spawning the object at the center of the original.
  - `2`: PlaceAtMe() spawning at the top of the original.
  - `3`: PlaceAtMe() spawning at the bottom of the original.
  - `4` (default): Common PlaceAtMe() with buggy engine physics bypassing (e.g., when used with `Throw`/`Grab`/`Release`, the object will be spawned exactly to the landing point without position glitches).
  - `5`: Bypass with spawning the object at the center of the original.
  - `6`: Bypass with spawning at the top of the original.
  - `7`: Bypass with spawning at the bottom of the original.
  - `8`: Pin to the ground regardless of the landing location (e.g., when used with `Throw`, an object that lands on the wall will spawn a new one directly beneath it on the floor).

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
           "effect": {
               "type": "SpawnItem",
               "items": [{"formID": "Skyrim.esm:0xF"}]
           }
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
           "effect": {
               "type": "RemoveItem"
           }
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
           "effect": {
               "type": "SpawnItem",
               "items": [
                   {"formID": "Skyrim.esm:0xF", "count": 5, "chance": 50, "scale": 1.5},
                   {"formID": "Skyrim.esm:0xA", "count": 2, "chance": 30, "fade": 1}
               ]
           }
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
           "effect": {
               "type": "SpawnExplosion",
               "items": [{"formID": "Skyrim.esm:0x123456"}]
           }
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
           "effect": {
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
           }
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
           "effect": {
               "type": "PlayIdle",
               "items": [{"string": "IdleName", "duration": 0.5}]
           }
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
           "effect": {
               "type": "SpawnEffectShaderOnItem",
               "items": [
                   {
                       "formID": "Skyrim.esm:0x123456",
                       "duration": 5.0
                   }
               ]
           }
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
           "effect": {
               "type": "SpawnSpell",
               "items": [
                   {
                       "formID": "Skyrim.esm:0x789ABC",
                       "radius": 500
                   }
               ]
           }
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
            "effect": {
                "type": "PlaySound",
                "items": [{"formID": "Skyrim.esm:0x456789"}]
            }
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
            "effect": {
                "type": "ApplyIngestible",
                "items": [{"radius": 300, "chance": 75}]
            }
        }
    ]
    ```
    - Activating an ingredient applies its effects to actors within 300 units with 75% chance.
