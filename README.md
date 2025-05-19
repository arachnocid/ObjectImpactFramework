# Object Impact Framework

Object Impact Framework (OIF) is a SKSE plugin that lets you add custom effects to items when they are activated, grabbed, released, thrown, or hit. You can make things happen like spawning items, impact data sets, actors, casting spells, creating explosions, playing sounds, and swapping objects - all controlled with JSON files. Compatible with everything.

This guide explains how to set up those JSON files so you can customize the mod however you like. Examples are given in the more detailed documentation below.

## Users Info

- Place your JSON files in: `Data/SKSE/Plugins/ObjectImpactFramework/`
- **Check the Log**: If something doesn’t work, look at the mod’s log file for error messages. The log can be found in `Documents/My Games/Skyrim Special Edition/SKSE/ObjectImpactFramework.log`

## Mod Authors Info

- **Filter Note**: An object must be defined by at least one of the three parameters - `formIDs`, `formLists`, `formTypes`, `keywords` - for the event to work.
- **Form IDs Must Match**: Make sure the `formID` fits the effect (e.g., a spell ID for `"SpawnSpell"`, an item ID for `"SpawnItem"`).
- **Keywords Note**: For containers, doors, statics, movable statics, and trees, the `keywords` filter is ignored since they don’t use keywords.
- You can modify existing JSON files without quitting the game, edit the file and reload the save.

---

## Basic Rule Structure

Each rule in your JSON file defines a specific behavior for the mod. Rules are written as an array of objects, where each object contains three main fields: `event`, `filter`, and `effect`. Here's what each field does:

- **`event`**: Specifies the trigger(s) for the rule. This is an array of strings, allowing multiple events to trigger the same rule. Valid values are:
  - `"Activate"`: Triggered when an object is activated (e.g., opened, used).
  - `"Hit"`: Triggered when an object is hit by a weapon, spell, or projectile.
  - `"Grab"`: Triggered when an object is picked up by the player.
  - `"Release"`: Triggered when a grabbed object is dropped.
  - `"Throw"`: Triggered when a grabbed object is thrown (requires the **Grab And Throw** mod by powerofthree).
  
- **`filter`**: Defines the conditions under which the rule applies. This is an object that specifies which objects or interactions the rule targets. At least one of `formTypes`, `formIDs`, `formLists`, or `keywords` must be provided to identify target objects.

- **`effect`**: Describes what happens when the rule is triggered. This can be a single effect (an object) or multiple effects (an array of objects). Each effect has a `type` and, for most types, an optional `items` array specifying what to spawn, swap, or apply.

Here’s a basic example of a rule:

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

The `filter` object determines which objects and interactions trigger a rule. It’s highly customizable, allowing precise control over when effects occur. Below are all possible filter parameters:

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

- **`formIDs`** (optional): An array of strings identifying specific objects by their Form ID in the format `"modName:formID"`. Examples:
  - `"Skyrim.esm:0x123456"` (for esp/esm plugins).
  - `"MyMod.esl:0x456"` (for esl/espfe plugins).
  - `"Dawnguard.esm:00123456"` (alternate format with leading zeros (esp/esm only)).

- **`formLists`** (optional): An array of objects referencing formlists (lists of forms defined in a mod). Each entry has:
  - **`formID`**: The formlist ID in `"modName:formID"` format (required).
  - **`index`** (optional): An integer specifying which item in the formlist to use. Use `-1` or omit to include all items. Example:
    ```json
    "formLists": [
        {"formID": "Skyrim.esm:0x123456", "index": 0},
        {"formID": "MyMod.esp:0x789ABC"}
    ]
    ```

- **`keywords`** (optional): An array of keywords that the object must have. Format can be `"modName:formID"` (e.g., `"Skyrim.esm:0xABCDEF"`) or a keyword name (e.g., `"VendorItemFood"`). Ignored for containers, doors, statics, movable statics, and trees.

- **`keywordsNot`** (optional): An array of keywords the object must *not* have, in the same format as `keywords`.

- **`isPluginInstalled`** (optional): An array of plugin names (e.g., `"MyMod.esp"`, `"Skyrim.esm"`) that must be loaded for the rule to apply.

- **`isPluginNotInstalled`** (optional): An array of plugin names that must *not* be loaded.

- **`isDllInstalled`** (optional): An array of DLL filenames (e.g., `"MyPlugin.dll"`) that must be present in `Data/SKSE/Plugins/`.

- **`isDllNotInstalled`** (optional): An array of DLL filenames that must *not* be present.

- **`chance`** (optional): A number between 0 and 100 representing the percentage chance the rule triggers. Defaults to 100 if omitted.

- **`interactions`** (optional): An integer specifying how many interactions (e.g., hits or activations) are required before the effect triggers. Defaults to 1.

- **`limit`** (optional): An integer setting the maximum number of times the rule can trigger per object. No limit if omitted.

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
  - `"shout"`: Shouts.
  - `"ability"`: Racial or other abilities.
  - `"power"`: Greater powers.
  - `"lesserpower"`: Lesser powers.
  - `"total"`: Yet to discover. Supposedly matches all weapon types.
  - `"other"`: Miscellaneous types not covered above.

- **`weapons`** (optional): An array of specific weapon or spell Form IDs in `"modName:formID"` format (e.g., `"Skyrim.esm:0x1A2B3C"`).

- **`weaponsKeywords`** (optional): An array of keywords the weapon or spell must have, in `"modName:formID"` or `"KeywordName"` format.

- **`weaponsKeywordsNot`** (optional): An array of keywords the weapon or spell must *not* have.

- **`attacks`** (optional): An array of attack types. Possible values:
  - `"regular"`: Standard attacks.
  - `"power"`: Power attacks.
  - `"bash"`: Shield or weapon bashes.
  - `"projectile"`: Ranged projectile attacks (e.g., arrows).
  - `"charge"`: Charging attacks (e.g., sprinting strikes).
  - `"rotating"`: Continuous spinning attacks.
  - `"continuous"`: Sustained attacks (e.g., spell streams).

- **`projectiles`** (optional): An array of specific projectile Form IDs in `"modName:formID"` format (e.g., `"Skyrim.esm:0xDEF123"`).

---

## Effects: What Happens When the Rule Triggers

The `effect` field defines the outcome when a rule is triggered. It can be a single effect (an object) or multiple effects (an array of objects). Each effect has a mandatory `type` field and, for most types, an optional `items` array.

### Effect Types

Here are all possible `type` values:

- **`RemoveItem`**: Deletes the target object.
- **`SpawnItem`**: Spawns specific items at the object’s location.
- **`SpawnLeveledItem`**: Spawns random leveled items based on the player’s level.
- **`SwapItem`**: Replaces the target object with another specific item.
- **`SwapLeveledItem`**: Replaces the target object with a random leveled item.
- **`SpawnSpell`**: Casts spells on nearby actors (within 350 units).
- **`SpawnLeveledSpell`**: Casts random leveled spells on nearby actors (within 350 units).
- **`SpawnSpellOnItem`**: Casts spells on the target object.
- **`SpawnLeveledSpellOnItem`**: Casts random leveled spells on the target object.
- **`SpawnActor`**: Spawns specific actors at the object’s location.
- **`SpawnLeveledActor`**: Spawns random leveled actors.
- **`SwapActor`**: Replaces the target object with specific actors.
- **`SwapLeveledActor`**: Replaces the target object with random leveled actors.
- **`SpawnImpact`**: Plays an Impact Data Set (e.g., visual effects like sparks).
- **`SpawnExplosion`**: Triggers an explosion at the object’s location.
- **`PlaySound`**: Plays a sound descriptor.
- **`SpillInventory`**: Spills the contents of a container.
- **`ApplyIngestible`**: Applies the target object’s effects (if it’s an ingredient or ingestible) to nearby actors (within 150 units).
- **`ApplyOtherIngestible`**: Applies effects from specified ingestibles to nearby actors (within 150 units). Can be used with any form type (e.g., activator, tree), unlike `"ApplyIngestible"`.
- **`SpawnLight`**: Spawns a light at the object’s location, will appear in the middle of an object.
- **`RemoveLight`**: Deletes lights within a specified radius.
- **`DisableLight`**: Disables lights within a specified radius.
- **`EnableLight`**: Enables previously disabled lights within a specified radius.

### Configuring Effects with `items`

For most effect types (except `RemoveItem`, `SpillInventory`, and `ApplyIngestible`), you can specify an `items` array to define what to spawn, swap, or apply. Each item in the array can include:

- **`formID`** (optional): A specific Form ID in `"modName:formID"` format (e.g., `"Skyrim.esm:0xF"` for a gold coin). Can be used alone or with `formList`.
- **`formList`** (optional): An array of formlist entries, each with:
  - **`formID`**: The formlist ID in `"modName:formID"` format.
  - **`index`** (optional): An integer specifying which item in the formlist to use. Use `-1` or omit for all items. Example:
    ```json
    "formList": [
        {"formID": "Skyrim.esm:0x123456", "index": 0},
        {"formID": "MyMod.esp:0x789ABC"}
    ]
    ```
- **`count`** (optional): An integer specifying how many instances to spawn or swap. Defaults to 1. For `RemoveLight`, `DisableLight`, and `EnableLight`, this is replaced by:
    - **`radius`**: The radius (in game units) within which lights are affected. Defaults to 100.
- **`chance`** (optional): A number between 0 and 100 for the percentage chance this item is used. Defaults to 100.

**Notes:**
- Effects like `RemoveItem`, `SpillInventory`, and `ApplyIngestible` don’t require an `items` array.
- You can mix `formID` and `formList` in the same `items` array for flexibility.

### Examples

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

3. **Spawn Items with Chances**
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
                   {"formID": "Skyrim.esm:0xF", "count": 5, "chance": 50},
                   {"formID": "Skyrim.esm:0xA", "count": 2, "chance": 30}
               ]
           }
       }
   ]
   ```
   - Hitting a container has a 50% chance to spawn 5 gold coins and a 30% chance to spawn 2 lockpicks.

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

5. **Use FormLists for Random Spawns**
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
                           {"formID": "Skyrim.esm:0x123456"}
                       ],
                       "count": 1,
                       "chance": 100
                   }
               ]
           }
       }
   ]
   ```
   - Activating a container spawns an item from the formlist at index 2 in `MyMod.esp:0x789ABC` or any item from `Skyrim.esm:0x123456`.
  
