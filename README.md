# Object Impact Framework
ObjectImpactFramework (OIF) is a simple yet powerful SKSE plugin that lets you add custom effects to objects when they are activated or hit. You can make things happen like spawning items, casting spells, creating explosions, swapping objects, and more-all controlled with JSON files.

# Introduction

This guide explains how to set up those JSON files so you can customize the mod however you like.

---

## Where to Put Your JSON Files

- Place your JSON files in: `Data/SKSE/Plugins/ObjectImpactFramework/`.
- You can modify existing JSON files without quitting the game, edit the file and reload the save.
- **Everything is case sensitive.** Only events and effect types are capitalized, everything else should remain lower case.

---

## Basic Rule Structure

Every rule in your JSON file tells the mod what to do. Here’s what each rule needs:

- **event**: What triggers the rule. Use an array with `"Activate"` (ONLY for activators and talking activators) OR `"Hit"` (for all objects).
- **filter**: Conditions to decide if the rule should run, based on the object.
- **effect**: What happens when the rule triggers. This can be one effect or a list of effects.
- **chance** (optional): A number from 0 to 100 for the percentage chance the effects happen. If you skip this, it’s 100% (always happens).

Example of a simple rule:
```json
[
    {
        "event": ["Activate"],
        "filter": {
            "formIDs": ["MyMod.esp:0x123456"]
        },
        "effect": {
            "type": "SpawnItem",
            "formID": "Skyrim.esm:0x123456"
        }
    }
]
```

---

## Filters: Choosing Which Objects to Affect

The `filter` part decides which objects the rule applies to. Here are the options:

- **formTypes**: Types of objects to match (e.g., `"Weapon"`, `"Container"`). Use an array of these:
  - `"activator"`
  - `"talkingactivator"`
  - `"weapon"`
  - `"armor"`
  - `"ammo"`
  - `"ingredient"`
  - `"misc"`
  - `"book"`
  - `"scroll"`
  - `"soulgem"`
  - `"potion"`
  - `"furniture"`
  - `"flora"`
  - `"container"`
  - `"static"`
  - `"moveablestatic"`
  - `"tree"`

- **formIDs**: Specific object IDs in the format `"modName:formID"` (e.g., `"Skyrim.esm:0x123456"`). Use an array ([""]). The formID itself can be written as: `0x123456` if the plugin an esp/esm, `0x456` if esl/espfe, `00123456`, or `123456`.

- **keywords**: Keywords the object must have, also in `"modName:formID"` format. Use an array. (Note: Ignored for containers, statics, movable statics, and trees.)

For `"Hit"` events, you can add these extra filters:

- **weaponsTypes**: Types of weapons that must hit the object. Use an array:
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

- **weapons**: Specific weapon IDs that must be used, in `"modName:formID"` format. Use an array.

- **attacks**: Type of attack. Use an array:
  - `"regular"`
  - `"power"`
  - `"bash"`
  - `"projectile"`
 
- **projectiles**: Specific projectile IDs, in `"modName:formID"` format. Use an array.

---

## Effects: What Happens When the Rule Triggers

The `effect` part says what to do. You can use one effect or a list of effects. Each effect has:

- **type**: The kind of effect. Choose one:
  - `"RemoveItem"`: Deletes the object.
  - `"SpawnItem"`: Spawns one item.
  - `"SpawnMultipleItems"`: Spawns several items at once.
  - `"SpawnSpell"`: Casts a spell (the one who activated/hit becomes a target).
  - `"SpawnActor"`: Spawns an actor.
  - `"SpawnImpact"`: Plays an Impact Data Set (NOT a simple impact).
  - `"SpawnExplosion"`: Spawns an explosion.
  - `"SwapItem"`: Replaces the object with another.
  - `"SwapWithMultipleItems"`: Replaces the object with several items.

- **chance** (optional): A number from 0 to 100 for the chance this effect happens. If not set, it uses the rule’s `chance` or defaults to 100.

Depending on the `type`, add these:

- For `"SpawnItem"`, `"SpawnActor"`, `"SpawnImpact"`, `"SpawnExplosion"`, `"SwapItem"`:
  - **formID**: The ID of the thing to spawn or swap with, in `"modName:formID"` format.
  - **count** (optional): How many to spawn (default is 1). Used for `"SpawnItem"` and `"SpawnActor"`.

- For `"SpawnSpell"`:
  - **formID**: The spell’s ID, in `"modName:formID"` format.

- For `"SpawnMultipleItems"` and `"SwapWithMultipleItems"`:
  - **items**: A list of objects to spawn, each with:
    - **formID**: The ID, in `"modName:formID"` format.
    - **count** (optional): How many of that item (default is 1).

- `"RemoveItem"` doesn’t need extra fields.

---

## Examples to Get You Started

### 1. Spawn a Gold Coin When Activating Something
```json
[
    {
        "event": ["Activate"],
        "filter": {
            "formIDs": ["MyMod.esp:0x12345"]
        },
        "effect": {
            "type": "SpawnItem",
            "formID": "Skyrim.esm:0xF",
            "count": 1
        }
    }
]
```
- When you activate the object `MyMod.esp:0x12345`, it drops a gold coin.

### 2. Delete an Object When Hit
```json
[
    {
        "event": ["Hit"],
        "filter": {
            "formIDs": ["MyMod.esp:0x67890"]
        },
        "effect": {
            "type": "RemoveItem"
        }
    }
]
```
- When you hit an item `MyMod.esp:0x67890`, it disappears.

### 3. Spawn Items with a Chance
```json
[
    {
        "event": ["Hit"],
        "filter": {
            "formType": ["container"]
        },
        "effect": [
            {
                "type": "SpawnItem",
                "formID": "Skyrim.esm:0xF",
                "count": 5,
                "chance": 50
            },
            {
                "type": "SpawnItem",
                "formID": "Skyrim.esm:0xA",
                "count": 2,
                "chance": 30
            }
        ]
    }
]
```
- When you open a container:
  - 50% chance to get 5 gold coins.
  - 30% chance to get 2 lockpicks.

### 4. Explode When Hitting all Staticks
```json
[
    {
        "event": ["Hit"],
        "filter": {
            "formType": ["static"]
        },
        "effect": {
            "type": "SpawnExplosion",
            "formID": "Skyrim.esm:0x12345"
        }
    }
]
```
- Hitting any static object (like a rock) causes an explosion.

### 5. Activate AND Hit event for any Activator
```json
[
    {
        "event": ["Activate"],
        "filter": {
            "formType": ["activator"]
        },
        "effect": {
            "type": "SwapWithMultipleItems",
            "chance": 75.0,
            "items": [
              {"formID": "Skyrim.esm:00064B31", "count": 1},
              {"formID": "Skyrim.esm:00064B32", "count": 1},
              {"formID": "Skyrim.esm:000669A3", "count": 1}
            ]    
        },
    },
    {
        "event": ["Hit"],
        "filter": {
            "formType": ["activator"]
        },
        "effect": {
            "type": "SwapWithMultipleItems",
            "chance": 75.0,
            "items": [
              {"formID": "Skyrim.esm:00064B31", "count": 1},
              {"formID": "Skyrim.esm:00064B32", "count": 1},
              {"formID": "Skyrim.esm:000669A3", "count": 1}
            ]
        }
    }
]
```
- Hitting or activating any activator spawns different cheeses.

---

## Extra Tips

- **Where Stuff Appears**: Spawned items, actors, and effects happen at the object’s location. Spells cast from the object toward the player or attacker.
- **Form IDs Must Match**: Make sure the `formID` fits the effect (e.g., a spell ID for `"SpawnSpell"`, an item ID for `"SpawnItem"`).
- **Check the Log**: If something doesn’t work, look at the mod’s log file for error messages. The log can be found inside `Documents\My Games\Skyrim Special Edition\SKSE`.
- **Keywords Note**: For containers, statics, movable statics, and trees, the `keywords` filter is ignored since they don’t use keywords.
