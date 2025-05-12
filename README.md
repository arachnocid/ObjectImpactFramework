# Object Impact Framework
ObjectImpactFramework (OIF) is a SKSE plugin that lets you add custom effects to objects when they are activated or hit. You can make things happen like spawning items, impact data sets, actors, casting spells, creating explosions, playing sounds, swapping objects - all controlled with JSON files.

# Introduction

This guide explains how to set up those JSON files so you can customize the mod however you like.

---

## Important Notes

- Place your JSON files in: `Data/SKSE/Plugins/ObjectImpactFramework/`.
- You can modify existing JSON files without quitting the game, edit the file and reload the save.
- **Check the Log**: If something doesn’t work, look at the mod’s log file for error messages. The log can be found inside `Documents\My Games\Skyrim Special Edition\SKSE`.
- **Form IDs Must Match**: Make sure the `formID` fits the effect (e.g., a spell ID for `"SpawnSpell"`, an item ID for `"SpawnItem"`).
- **Keywords Note**: For containers, doors, statics, movable statics, and trees, the `keywords` filter is ignored since they don’t use keywords.

---

## Basic Rule Structure

Every rule in your JSON file tells the mod what to do. Here’s what each rule needs:

- **event**: What triggers the rule. Use an array with `"Activate"` and/or `"Hit"` (for all objects).
- **filter (optional)**: Conditions to decide if the rule should run, based on the object.
- **effect**: What happens when the rule triggers. This can be one effect or a list of effects.

Example of a simple rule:
```json
[
    {
        "event": ["Activate", "Hit"],
        "filter": {
            "formTypes": ["activator", "talkingactivator"]
        },
        "effect": {
            "type": "SpawnItem",
            "items": [{"formID": "Skyrim.esm:0xF"}]
        }
    }
]
```

---

## Filters: Choosing Which Objects to Affect

The `filter` part decides which objects the rule applies to. Here are the options:

- **formTypes** (optional): Types of objects to match (e.g., `"Weapon"`, `"Container"`). Use an array of these:
  - `"activator"`
  - `"talkingactivator"`
  - `"weapon"`
  - `"armor"`
  - `"ammo"`
  - `"ingredient"`
  - `"misc"`
  - `"key"`
  - `"book"`
  - `"note"`
  - `"scroll"`
  - `"soulgem"`
  - `"potion"`     (potions, poisons, food, and drinks are covered)
  - `"furniture"`
  - `"door"`
  - `"flora"`
  - `"container"`
  - `"static"`
  - `"moveablestatic"`
  - `"tree"`

- **formIDs** (optional): Specific object IDs in the format `"modName:formID"` (e.g., `"Skyrim.esm:0x123456"`). Use an array ([""]). The formID itself can be written as: `0x123456` if the plugin an esp/esm, `0x456` if esl/espfe, `00123456`, or `123456`.

- **keywords** (optional): Keywords the object must have, also in `"modName:formID"` or `"KeywordName"` (non-case sensitive) formats. Use an array. (Note: Ignored for containers, statics, movable statics, and trees.)

- **chance** (optional): A number from 0 to 100 for the chance this effect happens. If not set, it uses the rule’s `chance` or defaults to 100.

- **interactions** (optional): A number from 0 to 100 which determines how many interactions it will take for the event to fire. If you skip this, it's 1 (every interaction).

For `"Hit"` events, you can add these extra filters:

- **weaponsTypes** (optional): Types of weapons that must hit the object. Use an array:
  - `"onehandsword"`
  - `"twohandsword"`
  - `"onehandaxe"`
  - `"twohandaxe"`
  - `"onehandmace"`
  - `"twohandmace"`
  - `"dagger"`
  - `"ranged"`
  - `"staff"`
  - `"handtohand"`
  - `"spell"`
  - `"ability"`
  - `"power"`
  - `"lesserpower"`
  - `"total"`
  - `"other"`

- **weapons** (optional): Specific weapon IDs that must be used, in `"modName:formID"` format. Use an array.

- **weaponsKeywords** (optional): Keywords that the weapon/spell must have, in `"modName:formID"` or `"KeywordName"` (non-case sensitive) formats. Use an array.

- **attacks** (optional): Type of attack. Use an array:
  - `"regular"`
  - `"power"`
  - `"bash"`
  - `"projectile"`
  - `"charge"`
  - `"rotating"`
  - `"continuous"`
 
- **projectiles** (optional): Specific projectile IDs, in `"modName:formID"` format. Use an array.

---

## Effects: What Happens When the Rule Triggers

The `effect` part says what to do. You can use one effect or a list of effects. Each effect has:

- **type**: The kind of effect. Choose one:
  - `"RemoveItem"`: Deletes the object.
  - `"SpawnItem"`: Spawns items.
  - `"SpawnSpell"`: Casts spells (the one who activated/hit becomes a target).
  - `"SpawnSpellOnItem"`: Casts spells on the target object.
  - `"SpawnActor"`: Spawns actors.
  - `"SpawnImpact"`: Plays Impact Data Sets (NOT a simple impact).
  - `"SpawnExplosion"`: Spawns explosions.
  - `"SwapItem"`: Replaces the object with another.
  - `"SwapActor"`: Replaces the object with actors.
  - `"PlaySound"`: Plays provided sound descriptors.
  - `"SpillInventory"`: Spills a container's inventory out.

Depending on the `type`, add these:

- For each of the `type` except for `"RemoveItem"` and `"SpillInventory"`:
  - **items**: A list of objects to spawn or swap, each with:
    - **formID**: The ID, in `"modName:formID"` format.
    - **count** (optional): How many of that item (default is 1).
    - **chance** (optional): A number from 0 to 100 for the chance this effect happens.

- `"RemoveItem"` and `"SpillInventory"` don’t need extra fields.

---

## Examples to Get You Started

### 1. Spawn a Gold Coin When Activating Something
```json
[
    {
        "event": ["Activate"],
        "effect": {
            "type": "SpawnItem",
            "items": [{"formID": "Skyrim.esm:0xF"}]
        }
    }
]

```
- When you activate an object, it drops a gold coin.

### 2. Delete an Object When Hit
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
- When you hit a static item, it disappears.

### 3. Spawn Items with a Chance
```json
[
    {
        "event": ["Hit"],
        "filter": {
            "formTypes": ["container"]
        },
        "effect": [
            {
                "type": "SpawnItem",
                "items": [
                    {"formID": "Skyrim.esm:0xF", "count": 5, "chance": 50},
                    {"formID": "Skyrim.esm:0xA", "count": 2, "chance": 30}
                ]
            }
        ]
    }
]
```
- When you open a container:
  - 50% chance to get 5 gold coins.
  - 30% chance to get 2 lockpicks.

### 4. Explode When Hitting all Trees
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
- Hitting any tree causes an explosion.

### 5. Activate AND Hit event for any Activator
```json
[
    {
        "event": ["Hit", "Activate"],
        "filter": {
            "formTypes": ["activator", "container"],
            "chance": 75.0
        },
        "effect": {
            "type": "SwapItem",
            "items": [
              {"formID": "Skyrim.esm:00064B31", "count": 1},
              {"formID": "Skyrim.esm:00064B32", "count": 1},
              {"formID": "Skyrim.esm:000669A3", "count": 1}
            ]    
        }
    }
]
```
- Hitting or activating any activator + hitting any container spawns different cheeses.

---

