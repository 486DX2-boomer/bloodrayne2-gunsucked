## Table of Contents

1. [Static Base Addresses](#1-static-base-addresses)
2. [Rayne (CMainCharacter)](#2-rayne-cmaincharacter)
3. [Weapon System (CMultigun)](#3-weapon-system-cmultigun)
4. [Camera System (CGameView)](#4-camera-system-cgameview)
5. [Time Factor / Game Speed](#5-time-factor--game-speed)
6. [HUD System](#6-hud-system)
7. [Outfit / Costume System](#7-outfit--costume-system)
8. [POD File System](#8-pod-file-system)
9. [Key Functions](#9-key-functions)
10. [Known Classes](#10-known-classes)
11. [Miscellaneous Addresses](#11-miscellaneous-addresses)
12. [Unconfirmed Notes](#12-unconfirmed-notes)
13. [Scripting Engine](#13-scripting-engine)
14. [Entity System](#14-entity-system)
15. [UI System](#15-ui-system)
16. [v1.1 Findings](#16-v1.1-findings)
17. [v1.2 Findings (Weapon Wheel)](#17-v1.2-findings-weapon-wheel)
18. [True Dhampir Research (Melee, Movement, Knockdown)](#18-true-dhampir-research-melee-movement-knockdown)

---

## 1. Static Base Addresses

### Confirmed

| Address | Type | Description | Notes |
|---------|------|-------------|-------|
| `rayne2.exe+5D276EC` | Pointer | Rayne (CMainCharacter) base address | Works consistently from Roof level onwards |
| `rayne2.exe+5B0AAFC` | Pointer | CMultigun base address | Valid after Rayne obtains the Dragons |
| `rayne2.exe+3C6A70` | Pointer | Time Factor manager object | Static pointer to game speed controller |
| `0x05E34EE0` | Object | CGameView (camera controller) | Static address for camera system |
| `0x007BF4BC` | Pointer | Unlocked weapons flag pointer | Follow pointer, then +0x34 for bitmask |

### Confident

| Address | Type | Description | Notes |
|---------|------|-------------|-------|
| `rayne2.exe+5B0AAF0` | Pointer | Context-dependent pointer | Points to Rayne in Entry/Courtyard, CMultigun after Ballroom |
| `0x05DCC0A0` | Array | File handler function pointer array | Two entries: POD handler, then loose file handler |

### Guesses

| Address | Type | Description | Notes |
|---------|------|-------------|-------|
| `0x05FEFEA8` | Array | Entity/update list | See [Section 14](#14-entity-system-guesses) |

---

## 2. Rayne (CMainCharacter)

**Base Address:** `rayne2.exe+5D276EC` (dereference to get object pointer)

**Suspected Struct Size:** ~36KB (0x8CFC observed, but possibly up to 48KB with unused padding)

**VTable Address:** Used for validation. Check vtable pointer matches expected value before installing hooks.

### Confirmed Offsets

| Offset | Type | Description | Range/Notes |
|--------|------|-------------|-------------|
| `0x0038` | float | Position Z (forward/back) | World coordinates |
| `0x003C` | float | Position Y (up/down) | World coordinates |
| `0x0040` | float | Position X (left/right) | World coordinates |
| `0x0148` | Pointer | CMultigun reference | Rayne's weapon system |
| `0x39C8` | float | Current Health | 0–1000 (upgradeable to 2000) |
| `0x39CC` | float | Maximum Health | 1000–2000 |
| `0x8360` | float | Current Rage | 0–1000 (upgradeable to 2000) |
| `0x8364` | float | Maximum Rage | 1000–2000 |
| `0x1139C` | int | Current Weapon Index | 0–5 (writable, but use native function) |

### Confident Offsets

| Offset | Type | Description | Notes |
|--------|------|-------------|--------|
| `0x01B0` | char[32] | Costume .DFM filename | String, max ~22 chars observed |
| `0x0178` | Pointer | Skeleton (.SKL) reference | Rayne's model skeleton |
| `0x47C0` | Pointer | Left gun reference | |
| `0x47C4` | Pointer | Left blade reference | |
| `0x47C8` | Pointer | Left auto-aim target | Pointer to targeted enemy |
| `0x4838` | Pointer | Right gun reference | |
| `0x483C` | Pointer | Right blade reference | |
| `0x4840` | Pointer | Right auto-aim target | |
| `0x8180` | int | Targetable enemy count | Number of enemies in targeting range |

### Guesses

| Offset | Type | Description | Notes |
|--------|------|-------------|--------|
| `0x05AC` | Pointer | dxCCylinder (collider?) | Possibly Rayne's collision cylinder |
| `0x3A00–0x3A08` | Vec3 | Unknown vector | Purpose unclear |
| `0x8368` | Pointer | CChain reference | Harpoon system? |
| `0x8454` | Pointer | Secondary SKL reference | |
| `0x8CF4` | varies | Harpoon target data | Contains data when enemy is harpooned |
| `0x8D0D` | bool | Flinch from bullets | Referenced in debug function |

---

## 3. Weapon System (CMultigun)

**Base Address:** `rayne2.exe+5B0AAFC` (dereference to get object pointer)

**Initialisation Function:** `FUN_00585480` at `0x00585480`

**Weapon data base offset within CMultigun:** `0x2BC`

**Each weapon mode struct size:** `0x74` bytes (116 bytes)

### Confirmed Offsets (Global)

| Offset | Type | Description |
|--------|------|-------------|
| `0x7FC` | float | Current ammunition pool | 0–1000 |
| `0x800` | float | Maximum ammunition (suspected) |

### Weapon Mode Layout

Each weapon occupies 0x74 bytes starting at its base offset:

| Weapon | Base Offset | Name |
|--------|-------------|------|
| Blood Shot | `0x2BC` | Single/pistol |
| Blood Stream | `0x330` | Rapid fire |
| Blood Spray | `0x3A4` | Scatter/shotgun |
| Blood Bomb | `0x418` | Grenade launcher |
| Blood Flame | `0x48C` | Incendiary |
| Blood Hammer | `0x500` | Rocket launcher |

### Confirmed Weapon Stat Offsets (Relative to Weapon Base)

| Relative Offset | Type | Description |
|-----------------|------|-------------|
| `+0x00` | char[32] | Weapon name string |
| `+0x20` | float | Ammo Cost Level 1 |
| `+0x24` | float | Ammo Cost Level 2 |
| `+0x28` | float | Ammo Cost Level 3 |
| `+0x2C` | float | Damage Level 1 |
| `+0x30` | float | Damage Level 2 |
| `+0x34` | float | Damage Level 3 |
| `+0x38` | float | Fire Rate Level 1 |
| `+0x3C` | float | Fire Rate Level 2 |
| `+0x40` | float | Fire Rate Level 3 |
| `+0x50` | float | Effective Range |
| `+0x54` | int | Type ID |

### Default Weapon Stats

#### Ammo Costs

| Weapon | L1 | L2 | L3 | Max Shots L1 | Max Shots L3 |
|--------|----|----|-----|--------------|--------------|
| Blood Shot | 25.0 | 18.0 | 15.0 | 40 | 66 |
| Blood Stream | 25.0 | 22.0 | 20.0 | 40 | 50 |
| Blood Spray | 100.0 | 50.0 | 50.0 | 10 | 20 |
| Blood Bomb | 200.0 | 80.0 | 67.0 | 5 | 14 |
| Blood Flame | 200.0 | 80.0 | 67.0 | 5 | 14 |
| Blood Hammer | 300.0 | 131.0 | 116.0 | 3 | 8 |

#### Damage Values

| Weapon | L1 | L2 | L3 |
|--------|----|----|-----|
| Blood Shot | 50.0 | 55.0 | 60.0 |
| Blood Stream | 50.0 | 55.0 | 60.0 |
| Blood Spray | 13.33 | 8.33 | 10.0 |
| Blood Bomb | 100.0 | 50.0 | 50.0 |
| Blood Flame | 100.0 | 50.0 | 50.0 |
| Blood Hammer | 300.0 | 325.0 | 350.0 |

#### Fire Rates

| Weapon | L1 | L2 | L3 |
|--------|----|----|-----|
| Blood Shot | 4.0 | 5.0 | 6.0 |
| Blood Stream | 8.0 | 10.0 | 12.0 |
| Blood Spray | 1.0 | 1.0 | 1.0 |
| Blood Bomb | 1.0 | 2.0 | 1.0 |
| Blood Flame | 1.0 | 1.0 | 1.0 |
| Blood Hammer | 1.0 | 1.0 | 1.0 |

#### Ranges

| Weapon | Range |
|--------|-------|
| Blood Shot | 80.0 |
| Blood Stream | 80.0 |
| Blood Spray | 80.0 |
| Blood Bomb | 50.0 |
| Blood Flame | 10.0 |
| Blood Hammer | 200.0 |

### Confirmed Weapon Switching

| Address | Type | Description |
|---------|------|-------------|
| `rayne2.exe+5B2FAEC` | int | Current weapon index (read-only) |
| `rayne2.exe+5B2FAF0` | int | Previous weapon index (read-only) |
| `rayne2.exe+5B2FAF4` | int | Next weapon index (read-only) |

**Weapon Switch Function:** `FUN_005B5070` — Call this function instead of writing directly to the weapon index to ensure proper sound effects, animations, and validation.

**Unlocked Weapons Bitmask:** Follow pointer at `0x007BF4BC`, then offset `+0x34`:
- `0x03` = Blood Shot + Blood Stream unlocked
- `0x3F` = All six weapons unlocked

---

## 4. Camera System (CGameView)

**Base Address:** `0x05E34EE0` (static)

**VTable Address:** `0x006F9AEC` (use for validation)

### Confirmed Offsets

| Offset | Type | Description | Notes |
|--------|------|-------------|-------|
| `+0x004` | float | Camera X position | |
| `+0x008` | float | Camera Z position | Note: XZY coordinate system |
| `+0x00C` | float | Camera Y position | |
| `+0x010` | float | Camera Pitch | |
| `+0x014` | float | Camera Yaw | |
| `+0x01C` | float | Field of View | Great for photo mode |
| `+0x208` | Pointer | Camera focus target | Usually points to Rayne |

### Confirmed Render Globals

These are written by `FUN_005e1a10` and read by the renderer:

| Address | Type | Description |
|---------|------|-------------|
| `0x06121F04` | float | FOV |
| `0x06121F34` | float | Render Camera X |
| `0x06121F38` | float | Render Camera Y |
| `0x06121F3C` | float | Render Camera Z |
| `0x061049EC` | float[9] | 3×3 Rotation Matrix |
| `0x06122008` | float | Audio Listener X |
| `0x0612200C` | float | Audio Listener Y |
| `0x06122010` | float | Audio Listener Z |

### Confident

| Offset/Address | Type | Description | Notes |
|----------------|------|-------------|-------|
| `0x05E3473D` | bool | Disable Push Camera | Set to 1 to allow camera outside bounds |
| `0x05E3473C` | bool | Debug Focus Point | Draws blue rectangles on focus objects |
| `0x05E350F4` | int | Camera Mode | See modes below |
| `0x05E350E8` | Pointer | Camera Focused Object | Override to focus on different entity |

**Camera Modes:**
- 0 — Normal (default)
- 15 — Focus on entity base (Rayne's feet)
- 22 (0x16) — Path/cutscene
- 24 (0x18) — Orbit
- 25 (0x19) — OrbitNew (kill cams)
- 26 (0x1a) — Fixed (reverts to 0 when set manually)

All modes maintain a focus point. No "free" camera mode exists.

### Guesses

| Address | Type | Description |
|---------|------|-------------|
| `0x05E34EF8` | varies | Cutscene-related flag | Non-zero during cutscenes |
| `0x05E35138` | float | Camera path tween timer | Increments during cutscenes |
| `0x05E3515C` | Pointer | CCameraPathActor pointer | |

### Camera Configuration Globals

These static addresses control camera behaviour:

| Address | Description | Default |
|---------|-------------|---------|
| `0x007C0A94` | OffScreen H Lag | |
| `0x007C0A98` | OffScreen P Lag | |
| `0x007C0A9C` | Lock-on H Centre Tendency | |
| `0x007C0AA0` | Lock-on P Centre Tendency | |
| `0x007C0AA4` | Offscreen H max deg/sec | |
| `0x007C0AA8` | Offscreen P max deg/sec | |
| `0x007C0A74` | UserRot lag | |
| `0x007C0A78` | UserRot lag (release) | |
| `0x007C0A68` | Rotate behind movement | |
| `0x007C0A8C` | idleCameraTime | |
| `0x05E34740` | Nav dist | |
| `0x05E34744` | Combat dist | |
| `0x05E34748` | Default pitch | |
| `0x05E3474C` | Combat vert offset | |

---

## 5. Time Factor / Game Speed

### Confirmed

| Address | Type | Description |
|---------|------|-------------|
| `0x0619FB68` | float | Time Factor (static address) |
| `rayne2.exe+3C6A70` + `0x2E8` | float | Time Factor (via pointer) |

**Notes:**
- Value of `1.0` = normal speed
- Values `0.1–4.0` are allowed by the in-game cheat menu
- Can be set to any value (including 0 for freeze, or >4.0 for fast-forward)
- Negative values produce undefined behaviour

---

## 6. HUD System

### Confirmed

| Address | Type | Description |
|---------|------|-------------|
| `rayne2.exe+5B2FA58` | bool | HUD draw toggle | 0 = hidden, 1 = visible |

**HUD Update Function:** The function at `0x00592069` writes to this address every frame. Hook this function to persistently override HUD state.

---

## 7. Outfit / Costume System

### Confirmed

| Address | Type | Description |
|---------|------|-------------|
| `0x05E339B4` | int | Rayne Outfit Index | Current selected costume slot |

### Confident

The outfit system loads costume data from `.DFM` files. The game's asset loading uses a two-handler system:

1. **POD Handler** (`FUN_005A3FD0`): Loads from packed archives
2. **Loose File Handler** (`FUN_004937B0`): Loads from filesystem directories

**File Handler Array:** `0x05DCC0A0`
- Entry 0: POD handler function pointer
- Entry 1: Loose file handler function pointer

**Modding Strategy:** Swap the handler order at runtime to prioritise loose files over POD archives, enabling drop-in asset replacement.

---

## 8. POD File System

### Confident

**POD File Format:** Version 3 (`'POD3'` magic bytes)

**Structure:**
- 4-byte magic header
- 0x120 byte metadata header
- File entry table (0xC bytes per entry)
- String table (filenames)

**POD Manager:** Global object manages up to 100 mounted POD archives.

**Mount Function:** `FUN_005A4B20` — Allocates 0x440-byte POD objects.

### File Entry Structure (0xC bytes)

| Offset | Type | Description |
|--------|------|-------------|
| `+0x00` | int | String table offset (converted to pointer) |
| `+0x04` | int | File offset or size |
| `+0x08` | int | File size or offset |

---

## 9. Key Functions

### Confirmed (Hooked in Mod)

| Address | Signature | Purpose |
|---------|-----------|---------|
| `0x00585480` | `void __fastcall(void* thisPtr)` | CMultigun weapon stat initialisation |
| `0x005E1A10` | `void __stdcall(float* pos, float* angles, float fov, float* listener)` | Camera render output |
| `0x005B5070` | `void(int weaponIndex)` | Switch weapon mode |

### Confident

| Address | Description |
|---------|-------------|
| `0x00484230` | Build rotation matrix from Euler angles |
| `0x004FE860` | Camera tick/update function |
| `0x004FD620` | Apply camera state to renderer |
| `0x005A3FD0` | POD file read handler |
| `0x004937B0` | Loose file read handler |

### Character / Combat

| Address | Description | Notes |
|---------|-------------|-------|
| `0x00549BF0` | Rayne's constructor | Maps ability costs, max HP, sound effects |
| `0x00678880` | Rayne object initialiser | References foot/blade damage, collision, skeletons |
| `0x00446360` | Enemy behaviour flags | `perception`, `guardDistance`, `canFeedOnMe` |
| `0x00661050` | Max Bite Distance | Could extend feeding range |
| `0x00666FF0` | Rage/lock-on | "Add to rage", "Lock-on move speed" |
| `0x00677690` | Lock-on settings | "Lock on nearby range", "Lock on to off-screen targets" |
| `0x00658EF0` | Rail/pole "suck zone" | Could increase grab distance |

### Weapons

| Address | Description | Notes |
|---------|-------------|-------|
| `0x00585250` | Gun system function | References `increaseROF`, `increaseDamage`, `addLaserSight` |
| `0x00586E40` | Weapon multiplier assignment | Ammo cost, ROF, damage multipliers; "Aiming error" |
| `0x005898A0` | Weapon name lookup | Called with "Blood Shot" etc. in stat initialiser |

### Sound

| Address | Description | Notes |
|---------|-------------|-------|
| `0x00621CC0` | Sound playback by filename | |
| `0x00622AE0` | Sound event binding | Called with .wav filenames |
| `0x00408A40` | Combat sound bindings | `blade-hit?.wav`, `rayne-scissor.wav`, etc. |
| `0x00591F70` | UI sound | Contains `ui-confirm.wav` binding |

### Scripting / Actors

| Address | Description | Notes |
|---------|-------------|-------|
| `0x004CABC0` | Script dispatch (~5000 lines) | Contains `setCameraModeFixed`, `createActor`, etc. |
| `0x004F4560` | Actor spawning | May handle Tute (tutorial) actors |
| `0x005DE960` | Message box related | |

### Debug / Developer

| Address | Description | Notes |
|---------|-------------|-------|
| `0x0067A350` | Stubbed debug registration | References reveal struct layouts |
| `0x004F9E20` | Camera debug registration | Registers camera config variables |

### Other

| Address | Description | Notes |
|---------|-------------|-------|
| `0x0050AF60` | "HDR FX" with film grain | May be for Aura Vision or unused |
| `0x004A4F40` | Shadow timing | "Time to wait before heading to shadow" — Ephemera boss? |

---

## 10. Known Classes

C++ classes identified through RTTI data.

### Class Hierarchy

```
CActor (base class for game entities)
├── CVampire
│   └── CMainCharacter (Rayne)
├── CPunkMain
│   ├── CZPunk
│   └── CFPunk
├── CTrigger
│   └── CSpawnTrigger
└── CSpawn

CWeapon
└── CMultigun

CBaseView
└── CGameView (camera controller)

CCameraPathActor (scripted camera movements)

CAnimationController (component attached to animated entities)

CChain (harpoon system, presumed)
```

### Key Classes

| Class | VTable | Details |
|-------|--------|---------|
| CMainCharacter | `0x007034E4` | See [Section 2](#2-rayne-cmaincharacter) |
| CMultigun | `0x0070BE4C` | See [Section 3](#3-weapon-system-cmultigun) |
| CGameView | `0x006F9AEC` | See [Section 4](#4-camera-system-cgameview) |
| CBaseView | `0x006DE990` | Parent of CGameView |
| CAnimationController | `0x00718148` | Constructors: `FUN_00605F90`, `FUN_00605FD0` |

### Other Classes

#### CActor
Base class for most game entities. Enemies share the health offset at `0x39C8` with Rayne, suggesting it's defined here.

#### CVampire
Intermediate class between CActor and CMainCharacter. May be used for dhampir and boss characters.

#### CCameraPathActor
Scripted camera movements during cutscenes. Pointer at CGameView + 0x27C. Null during normal gameplay. Referenced by camera modes 22 and 26.

#### CTrigger / CSpawnTrigger
| Property | Value |
|----------|-------|
| Constructor | `FUN_0062B790` |
| Allocator | `FUN_0062C020` (allocates 0x29C bytes) |

The `@CSpawnTrigger` string pattern suggests a class registration system for the LVL parser.

#### CChain
Rayne's harpoon system. Possible pointer at CMainCharacter + 0x8368.

### Class Discovery Notes

**RTTI:** Class names found by searching for `.?AV` prefix. Example: `.?AVCMainCharacter@@`

**VTables:** Constructors write vtable address to offset 0x00. Finding vtable xrefs leads to constructors.

**Entity Factory:** The `@ClassName` pattern is part of a class registration table used by the LVL parser.

---

## 11. Miscellaneous Addresses

### Confirmed

| Address | Type | Description |
|---------|------|-------------|
| `rayne2.exe+5AF4AE8` | int | Menu selected row | Menu navigation |

### Confident

| Address | Type | Description |
|---------|------|-------------|
| `0x05E337C0` | int | Camera vertical axis setting | 1 = normal, 0 = inverted |
| `0x05EEE484` | int | Camera vertical axis (inverted) | 0 = normal, 1 = inverted |
| `0x05EEE994` | float | Camera horizontal sensitivity | 0.0–1.0 slider |

### Guesses

| Address | Description |
|---------|-------------|
| `0x007C5130` | Pointer to global POD manager object |
| `0x05FF3D28` | Visible entity count |

---

## 12. Unconfirmed Notes

### Unknown Weapon Stat Offsets

Each weapon mode struct contains fields with unknown purpose:

| Relative Offset | Type | Observed Values | Notes |
|-----------------|------|-----------------|-------|
| `+0x44` | float | 0.0 (most weapons), 1.0 (Bomb, Hammer) | Unknown 1 — possibly explosion-related? |
| `+0x48` | float | 0.0 (most weapons), 1.0 (Bomb, Hammer) | Unknown 2 — same pattern as +0x44 |
| `+0x4C` | float | 0.0 (most weapons), 1.0 (Bomb, Hammer) | Unknown 3 — same pattern as +0x44 |
| `+0x58` | int | 101 (Shot, Stream, Spray), 112 (Bomb, Hammer), 102 (Flame) | Unknown 4 — possibly sound/effect ID? |
| `+0x5C` | int | 0 | Unknown 5 |
| `+0x60` | byte | 0 | Unknown 6 |
| `+0x64` | int | 1 (Shot, Stream), 4 (Spray, Flame), 5 (Bomb, Hammer) | Unknown 7 — possibly projectile count or behaviour mode? |
| `+0x68` | int | 0 | Unknown 8 |
| `+0x6C` | int | 0 | Unknown 9 |
| `+0x70` | int | 0 | Unknown 10 |

**Observations:**
- Blood Bomb and Blood Hammer share identical values for Unknowns 1–3 (all 1.0), so these may relate to explosive/projectile behaviour
- Unknown 7 correlates loosely with weapon type: 1 for hitscan, 4 for spread/continuous, 5 for explosive
- Unknown 4 varies by weapon and may be an ID reference to sound effects or visual effects
- Unknowns 5, 6, 8, 9, 10 are zero across all weapons and may be unused/reserved

These fields are written by `FUN_00585480` but their downstream consumers have not been identified.

### Rayne Ability Cost Addresses

The stubbed debug function `FUN_0067a350` reveals static addresses for Rayne's ability costs.

| Address | Description | Notes |
|---------|-------------|-------|
| `0x007C1B38` | Max Block Angle | |
| `0x007C1B3C` | SlowMo Cost | Rage cost for Dilated Perception |
| `0x007C1B40` | Super Speed Cost | Rage cost for Blood Rage speed |
| `0x007C1B44` | Freeze Time Cost | Rage cost for time freeze |
| `0x007C1B48` | Blood Rage Cost | |
| `0x007C1B4C` | Blood Fury Cost | |
| `0x007C1B50` | Ghost Feed Cost | |
| `0x007C1B54` | Possession Cost | |
| `0x05EC0768` | AuraVision Cost | Note: Different address range |

Found in `FUN_00549BF0` (Rayne's constructor)

### Time Factor Manager Object

The object at `rayne2.exe+3C6A70` (static address `0x007C6A70`) may be a global configuration/cheat manager:

| Offset | Type | Description |
|--------|------|-------------|
| `+0x2E8` | float | Time Factor |
| `+0x300` | float | Unknown counter — ticks once per second, resets on level load and ability use |
| `+0x330` | float | Total play time counter — does not reset |

### Rayne Base Address Anomaly

Two static pointers reference Rayne:
- `rayne2.exe+5B0AAF0`: Only valid for Entry and Courtyard levels; becomes CMultigun pointer after Ballroom
- `rayne2.exe+5D276EC`: Consistent from Roof level onwards

The first pointer continues tracking position data after Courtyard, but ~4 metres above Rayne's true position, probably a hitbox or collider rather than Rayne herself.

### Orphaned Multiplier System

Static addresses at `0x007C48C0–0x007C48F8` were labelled as "Ammo Cost Multiplier", "Fire Rate Multiplier", "Damage Multiplier" in the debug registration function, but nothing reads them in the retail build. These may be left over from a levelling system that was replaced with hardcoded per-level values.

### Cut/Unused Features

| Evidence | Description |
|----------|-------------|
| `"addLaserSight"` string in `FUN_00585250` | Laser sight for guns, cut feature? |
| `"Aiming error"` in `FUN_00586E40` | Weapon accuracy system, may be functional but I did not test |
| `"increaseROF"`, `"increaseDamage"` | Weapon upgrade strings |

### Enemy Health Offset

Enemies share health offset `0x39C8` with Rayne (inherited from CActor). Enemy addresses obtained via Rayne's auto-aim target pointers.

### Coordinate System

The game uses an XZY coordinate system internally. Render globals expect XYZ. CGameView stores XZY.

### Quiet Thunder Ammo Bug

The "Quiet Thunder" gun kata move consumes ammunition even when the infinite ammo cheat is enabled.

---

## 13. Scripting Engine

The game uses an external scripting system for level logic, enemy spawns, tutorials, and camera control. Script files are stored in `WORLD\` directory in POD archives. See Appendix B for file format list.

### Script Dispatch Function

`FUN_004CABC0` — A large (~5000 line) function that appears to be the scripting engine's command dispatcher.

### Known Script Commands

| Command | Example | Purpose |
|---------|---------|---------|
| `createActor` | `createActor(Trigger_Tute_FeedTutorial)` | Spawn an actor by name |
| `deleteActor` | `deleteActor(Trig_Tute_Move1)` | Remove an actor |
| `setCameraPathActor` | `setCameraPathActor(cpa_raynebegincombattute1, 1, 0)` | Set cutscene camera |
| `setCameraModeNormal` | | Reset camera |
| `setCameraModeFixed` | | Fixed camera angle |
| `dbBoxedDisplay` | `dbBoxedDisplay(ost_tute_combat1, 999)` | Show tutorial message |
| `dbtimedDisplay` | `dbtimedDisplay(ost_cleartext)` | Timed message display |

### LVL File Format (Partial)

Enemy spawn entries follow this structure:
```
{ CZPunk "ZPunk1"
    -186.171,-113,-39    // pos (x, y, z)
    0,0,0                // orient (pitch, yaw, roll)
    0                    // enabled
    ""                   // group
    0                    // attachCount
    300                  // hitPoints
    0                    // numCarryHands
    1                    // sizeScale
    300                  // maxHitPoints
    0                    // invulnerableFlag
    "(none)"             // scriptWalkDest
    3                    // scriptWalkDist
    1e+030               // scriptRunDist
    -1                   // scriptWalkTimer
    1.01567              // speed
    0                    // poolMe
    1                    // randomizeMe
    "(none)"             // scriptVictim
    ""                   // patrolNamePattern
    300                  // guardDistance
    150                  // victimHeight
    "(none)"             // victim
    0.5                  // perception
    "(none)"             // ourPost
    0                    // postModeFlag
    0                    // holdPosFlag
    -1                   // currentPatrolPointIndex
    2                    // spawnType
    120                  // peripheral
} CZPunk "ZPunk1"
```

### ABL Gun Mode Names

```
gunmode: single       // Blood Shot
gunmode: rapid        // Blood Stream
gunmode: scatter      // Blood Spray
gunmode: grenade      // Blood Bomb
gunmode: flamethrower // Blood Flame
gunmode: rocket       // Blood Hammer
```

---

## 14. Entity System

### Entity List Entry Format

```
Offset 0x00: void* entityPointer
Offset 0x04: float  distanceFromCamera (for culling)
```

Null pointer indicates end of list. Distance value likely used for LOD and update culling. Entities too far from camera may not be simulated.

### Addresses

| Address | Description |
|---------|-------------|
| `0x05FEFEA8` | Entity/update list (8-byte entries: pointer + distance) |
| `0x05FEFEC0` | Stable heap pointer for props, triggers, characters |
| `0x05FF3E38` | Secondary entity region (updates on spawn/death) |
| `0x05FF3D28` | Visible entity count |
| `0x05FF3DF0` | BSP root node pointer |
| `0x05FF3DF4` | BSP node count |

BSP data is rewritten on level load. Some nodes update during gameplay (dynamic/destructible objects?), many are static.

---

## 15. UI System

### Health Bar

The health bar displays two components:
1. Red bar — current health
2. White bar — damage taken (fades over time)

So the game tracks previous frame health, current frame health, and damage indicator lifetime.

### Menu System

| Address | Description |
|---------|-------------|
| `0x05EEE940` | Options/cheats menu base struct |

Menu rows include non-selectable headers and spacers that still occupy index positions.

---

## File Formats

| Extension | Description |
|-----------|-------------|
| `.POD` | Package archive (POD3 format) |
| `.DFM` | Costume/outfit definition |
| `.SKL` | Skeleton file |
| `.LVL` | Level script (actor spawns, triggers) |
| `.SCB` | Script file (game logic) |
| `.ABL` | Ability definition (combos, powers per level) |

---

## 16. v1.1 Findings

Observations made when working on v1.1, mostly to implement Casual Mode.

### Game State Object — PTR_DAT_007c07f8

The static pointer at `0x007c07f8` points to an object that holds  persistent settings (serialized to `rayne2.ini`) and runtime game state. This is the same object documented in Sections 5 and 11.

#### Settings Layout (Confirmed via INI serialization function FUN_005188d0)

`FUN_005188d0` writes settings to `%APPDATA%\BloodRayne2\settings\rayne2.ini` using `SHGetFolderPathA`. The helper functions are: `FUN_00518020` (write int), `FUN_005180e0` (write float), `FUN_00517f30` (write string), `FUN_00518240` (write boolean/flag). Section headers are set by `FUN_00517e80`.

**Graphics section:**

| Offset | INI Key | Type |
|--------|---------|------|
| +0x00 | gamePIXX | int (resolution width) |
| +0x04 | gamePIXY | int (resolution height) |
| +0x08 | gameBPP | int (bits per pixel) |
| +0x0C | subtitleMode | int |
| +0x10 | bloodFlag | int |
| +0x1C | quimbyFlag | int |
| +0x24 | showTutorials | int |
| +0x2C | autoLockOnAttacker | int |

**Control section:**

| Offset | INI Key | Type |
|--------|---------|------|
| +0x30–0xB3 | keyButton[0..32] | int array (primary key bindings) |
| +0xB4–0x137 | alternateKeyButton[0..32] | int array (alternate bindings) |
| +0x138–0x1BB | gamepadButton[0..32] | int array (gamepad bindings) |
| +0x1BC–0x1CB | gamepadAxis[0..3] | int array |
| +0x1CC–0x1DB | invertAxis[0..3] | int array |
| +0x1DC | autoCenterPitch | int |
| +0x1E0 | mouseSensitivityX | float |
| +0x1E4 | mouseSensitivityY | float |
| +0x1EC | gameControl | int |

There is a gap between `+0x1EC` and `+0x390` that may contain additional fields I haven't found yet. I haven't attempted to manipulate these offsets yet at runtime to see what the effects are.

#### Runtime Game State Flags

These offsets are on the same object, at higher offsets than the above settings.

| Offset | Type | Field | Values |
|--------|------|-------|--------|
| +0x390 | int | Cutscene/scripted sequence active | 0 = gameplay, 1 = cutscene. Does NOT change during Bink video playback. |
| +0x39C | int | Hero controls disabled | 0 = controls enabled, 1 = controls disabled. Mirrored to static global `DAT_06032060`. |

Note: Both `+0x390` and `+0x39C` go to 1 during cutscenes and return to 0 during normal gameplay.

| Offset | Type | Field | Values |
|--------|------|-------|--------|
| +0x394 | int | allowHeroDamage | Written by script handler `FUN_005d6930`. Stores raw return from boolean parser `FUN_004caac0`. |
| +0x398 | int | allowEnemyAttack | Written by script handler `FUN_005d6820`. Same pattern as +0x394. |

The boolean sense of `+0x394` and `+0x398` may be inverted relative to `+0x39C`. The `allowHeroControls` handler inverts the parser return with `(iVar3 == 0)`, while `allowHeroDamage` and `allowEnemyAttack` store the raw return value. This has not been fully verified at runtime.

#### Time-Related Offsets

| Offset | Type | Field | Notes |
|--------|------|-------|-------|
| +0x3D8 | float | Gameplay-scaled delta time | ~0.0167 at 60fps under normal gameplay. Scales down proportionally with Dilated Perception (~0.0067 observed). Goes to 0 when Cheat Menu Time Factor is set to 0 (photo mode). Does NOT go to 0 when game is paused via pause menu. |
| +0x3DC | float | Raw frame delta time | ~0.0167 at 60fps. Unaffected by Dilated Perception — stays at ~0.0167 regardless of time scaling. Stays non-zero when Cheat Menu Time Factor is 0. |

| Offset | Type | Field | Notes |
|--------|------|-------|-------|
| +0x3D4 | int/float | World-Time scale intermediate value | ~1092 (as int) during normal gameplay, ~436 during Dilated Perception. Possibly a fixed-point representation or intermediate calculation value. |

### Pause State

| Address | Type | Field | Values |
|---------|------|-------|--------|
| `0x05E33358` | int | Pause state | 0 = unpaused, 256 (0x100) = paused. Static address — does not change across restarts. |

This is a static global, not accessed via any pointer in the mod's code. The value 0x100 (bit 9 set) suggests this may be a bitfield rather than a simple boolean. Other bits may represent other menu states, but I haven't seen that in testing.

Located near a region of input state flags (e.g., `0x05E33368` appears to track forward movement / W key state).

Written by two code paths:
- `006A54CB`: `mov [ecx], eax` — generic register write, likely the input state update
- `004F8CA8`: `mov edi, 05E33358` — hardcoded address load, likely the pause activation path

### Time Factor System

The game uses hierarchy of time factors to derive in-game speed.

#### Cheat Menu Time Factor (Global Override)

- Static address: `0x0619FB68` (also accessible via `rayne2.exe+3C6A70` + `0x2E8`, documented in Section 5)
- Normal value: `1.0`
- Range allowed by cheat menu: `0.1–4.0`
- Used for mod's photo mode
- Applied as a global multiplier at the engine level, upstream of all other time calculations
- Independent of Dilated Perception — changing this does not affect Dilated Perception's behavior, and vice versa.

#### World-Time Scale

Manipulated by the `setTimeFactor` script command and by Dilated Perception. Is interpolated when activating time powers for a time "ramping down" and time "ramping up" effect.

#### Gameplay-Scaled Delta (Final time factor)

The final product at game state object `+0x3D8`: raw frame delta × world-time scale × cheat menu factor. This is what the player update function (`FUN_005520E0`) uses as `fVar2` for all gameplay timing — health drain, rage costs, ability timers, movement, animations.

For Casual Mode health regeneration, `+0x3D8` is the value used as the delta because it encodes frame rate normalization, Dilated Perception scaling, and cheat menu override. When combined with the game state checks (`+0x390`, `+0x39C`) and pause state (`0x05E33358`), this provides correct regen behavior in all observed game states. In other words, health doesn't regenerate during cutscenes or when the game is paused, and it regenerates at a slower rate when Rayne has her time powers active.

#### Time System Functions

| Function | Purpose | Confidence |
|----------|---------|------------|
| `FUN_006498E0` | Returns gameplay-scaled delta: reads `+0x3DC` (raw delta) and multiplies by time channel scale from `FUN_006498C0` | Confirmed (decompiled and runtime-verified) |
| `FUN_00649940` | `setTimeFactor` entry point. Sets override flag at `+0x324`, snapshots state from `+0x30C`–`+0x320` to pointer at `+0x2E4`, initializes lerp via `FUN_00648ed0` | Confident (decompiled) |
| `FUN_00648ed0` | Initializes a TimeLerp struct: current value, target, duration, elapsed (0), and two timestamps | Confident (decompiled) |
| `FUN_00648d50` | Records two timestamps into the lerp struct at `+0x10` and `+0x14`, likely from `QueryPerformanceCounter` or similar via `FUN_0069bdf0` | Suspected |
| `FUN_00649780` | Retrieves current time factor for a given channel index. Called with index 5 from `setTimeFactor` handler | Confident (decompiled) |

### Scripting VM

`FUN_005db5a0` is `CScript::step`, the main script interpreter. Source file reference: `..\\core\\SCRIPT.CPP`. It is a command parser/dispatcher that reads instructions and calls handler functions.

#### VM Architecture

The script execution context is accessed at `this+0x5AC`. The instruction array pointer is at `this+0x5B4`.

| Field | Location | Description |
|-------|----------|-------------|
| Instruction pointer | context+0x20 | Current instruction index |
| Call stack depth | context+0x2C | Current gosub depth |
| Return address stack | context+0x30 | 8 entries (max depth 8), each 4 bytes |
| Finished flag | context+0x50 | Set to 1 when script terminates via return with empty call stack |

Each instruction is a 12-byte struct:

| Offset | Type | Description |
|--------|------|-------------|
| +0x00 | int | Source line number |
| +0x04 | char* | Command string pointer |
| +0x08 | int | Cached jump target (for if/else/goto, resolved on first execution) |

`DAT_06032058` controls the mode. When non-zero, the system performs a validation pass — resolving labels and checking for duplicates without executing commands. `DAT_0603060c` is derived as `(DAT_06032058 == 0)` and gates actual command execution in each handler. Nearly every handler checks `DAT_0603060c` before performing side effects.

#### Script Commands (Dispatch Table)

The following commands were identified from the dispatch function. All string comparisons use `__strnicmp` (case-insensitive) followed by an `_isalnum` check on the next character to prevent partial matches (e.g., "if" must not match "idle").

##### Control Flow (handled inline)

| Command | Handler | Description |
|---------|---------|-------------|
| `{ }` | inline | Block delimiters (treated as no-ops) |
| `:label` | inline | Label definition. Validated for alphanumeric + underscore characters. Checked for duplicates via `FUN_005d9780`. |
| `if` | inline | Conditional. Parses condition via `FUN_005d6710` and `FUN_004caac0`. On false, skips to next instruction (or past else block). |
| `else` | inline | Skips to end of else block. Uses cached jump target. |
| `end` | inline | Script termination. Calls `FUN_0054dda0` conditionally. |
| `goto` | inline | Unconditional jump to label. Label resolved via `FUN_005d9780`, cached in instruction struct. |
| `gosub` | inline | Subroutine call. Pushes return address onto call stack (max depth 8). Overflow produces error. |
| `return` | inline | Returns from gosub. Pops call stack. If stack empty, sets finished flag at context+0x50. |
| `idle` | inline | Pauses script advancement by not incrementing the instruction pointer. Sets `DAT_06030608 = 0`. |
| `breakPoint` | inline | Debug breakpoint. Displays message via `FUN_004f4560` and sets `this+0xC = 2`. |

##### Game State Commands

| Command | Handler | Description |
|---------|---------|-------------|
| `allowEnemyAttack` | `FUN_005d6820` | Writes to game state object +0x398 |
| `allowHeroControls` | `FUN_005d68a0` | Writes to game state object +0x39C (inverted). Mirrors to `DAT_06032060`. |
| `allowHeroDamage` | `FUN_005d6930` | Writes to game state object +0x394 |
| `setTimeFactor` | `FUN_005d7bc0` | Parses `(factor, duration)` format. Factor clamped 0.0–4.0. Duration optional (default 0). Delegates to `FUN_00649940`. |
| `setCharacterHealth` | `FUN_005d7590` | |
| `setCharacterRage` | `FUN_005d7680` | |
| `setHero` | `FUN_005d79c0` | |
| `setVictim` | `FUN_005d7f20` | |
| `setDeathAltitude` | `FUN_005d7950` | |
| `setConveyorSpeed` | `FUN_005d7770` | |
| `setPlatformSpeed` | `FUN_005d7860` | |
| `setPlatformParam` | `FUN_005d8cf0` | |
| `setTurretTarget` | `FUN_005d7d50` | |
| `setWalkTimeout` | `FUN_005d80f0` | |
| `setSayTimeOverride` | `FUN_005d7af0` | |
| `setSkipLabel` | `FUN_005dae70` | |
| `setLevelDescription` | `FUN_005dad90` | |

##### Actor/Entity Commands

| Command | Handler | Description |
|---------|---------|-------------|
| `attachActorToActor` | `FUN_005d69b0` | |
| `detachActorFromActor` | `FUN_005d6af0` | |
| `dismember` | `FUN_005d6bd0` | |
| `fireweapon` | `FUN_005d6f30` | |
| `gesture` | `FUN_005d7010` | |
| `getImpalee` | `FUN_005d7210` | |
| `getTeslaCoilActor` | `FUN_005d7330` | |
| `snapToFace` | `FUN_005d82a0` | |
| `turnToFace` | `FUN_005d8550` | |
| `lookAt` | `FUN_005d89c0` | |
| `movePlatform` | `FUN_005d8b80` | |
| `raise` | `FUN_005d7450` | |
| `shutUp` | `FUN_005d81f0` | |

##### Dialogue/Display Commands

| Command | Handler | Description |
|---------|---------|-------------|
| `dbConversation` | `FUN_005da1d0` | |
| `dbSay` | `FUN_005da2e0` | |
| `dbStartSay` | `FUN_005da480` | |
| `dbTimedDisplay` | `FUN_005da590` | |
| `dbBoxedDisplay` | `FUN_005daa30` | |
| `startSay` | `FUN_005db010` | |
| `timedDisplay` | `FUN_005d8450` | |

##### Cinematic/Level Commands

| Command | Handler | Description |
|---------|---------|-------------|
| `chainToLevel` | via `FUN_004f5a30` | Level transition. Checks current level state before chaining. |
| `letterbox` | `FUN_005db1b0` | Toggle cinematic letterbox bars |
| `playCinemat` | `FUN_005d8e20` | |
| `queueVideo` | `FUN_005d8fa0` | |
| `rollCredits` | `FUN_005d7500` | |

##### Timing/Wait Commands

| Command | Handler | Description |
|---------|---------|-------------|
| `wait` | `FUN_005db130` | |
| `waitFor` | `FUN_005d88c0` | |

##### Debug/Special Commands

| Command | Handler | Description |
|---------|---------|-------------|
| `debug` | `FUN_005d6680` | Argument evaluation/debug output |
| `gtfo` | `FUN_005d6680` then terminate | Logs "Script GTFO at line %d", sets `this+0x8 = 1` to terminate the situation |

If no command matches, the dispatcher falls through to either `FUN_004d5700` (execution mode) or `FUN_004d56c0` (validation mode). These may handle extended/plugin commands or report unknown commands.

#### Utility Functions

| Function | Purpose | Confidence |
|----------|---------|------------|
| `FUN_00639580` | Skip whitespace / advance to next token. Called after every command match to position at arguments. | Confirmed (consistent usage across all handlers) |
| `FUN_005d6710` | Argument validation. Returns error string pointer on failure, null on success. | Confirmed (used by if, debug, chainToLevel, and multiple handlers) |
| `FUN_004caac0` | Boolean argument parser. Takes a 100-byte buffer (for error output), reads from `DAT_06031850`. Returns 0 for "true" (C success convention). | Confident (decompiled, matches observed behavior) |
| `FUN_005d9780` | Label lookup. Searches instruction array for a label, returns instruction index. Returns negative on failure. | Confirmed (used by goto, gosub, label validation) |
| `FUN_005d97f0` | Forward instruction scan. Used by if/else to find jump targets. Takes current instruction index and a skip count. | Confident (decompiled) |
| `FUN_005898a0` | String localization. Binary search through sorted table at `DAT_05f2e9e8`, returns localized string from `DAT_05f2d9e8`. Returns input unchanged if no match. Not pause-related despite proximity to "Enter Pause Menu" string. | Confirmed (decompiled) |

### Player Update Function FUN_005520E0

A tick/update method on the player object.

#### Confirmed (from decompilation and runtime testing)

- `fVar2` (the main delta time variable) is obtained from `FUN_006498E0()` at the top of the function — this is the gameplay-scaled delta documented above
- `param_1[0xE72]` (byte offset `0x39C8`) is current health — matches Section 2
- `param_1[0xE73]` (byte offset `0x39CC`) is max health — matches Section 2
- `param_1[0x20D8]` (byte offset `0x8360`) is current rage — matches Section 2
- `param_1[0x20D9]` (byte offset `0x8364`) is max rage — matches Section 2
- `PTR_DAT_007c07f8 + 0x390` is checked — when non-zero, various subsystems are reset (weapons, state)
- `PTR_DAT_007c07f8 + 0x304` and `+0x350` are checked in the rage drain section — when non-zero, rage is set to max. These may be cheat/invulnerability flags, purpose not confirmed.

#### Confident (from decompilation but not runtime tested)

- Health drain from water/acid: when `local_3c == 6`, health drains at `fVar2 * 50.0` per frame. Spawns `rayne_damage_water.fx` particle effect.
- Health regeneration on blood: when `local_38 != 0` and health > 0, health increases at `fVar2 * 100.0` per frame, capped at max. This is the game's native blood-feeding health recovery.
- Death check: when health reaches 0 and state is not 0xD4 or 0xD5 (death states), calls `FUN_0054dc50`.
- Player state at `param_1[0x5F]` (byte offset `0x17C`) uses enum values including 0xD4/0xD5 (death), 0x105 (idle), 0x10A and 0x1D3–0x1D8 (power activation states).
- Three powers are selected via virtual call at `vtable+0x230`. Availability checked by `FUN_0054ef50` (has enough rage) and `FUN_0054ef90`/`FUN_0054efd0` (power available). Error messages "Not enough rage points" and "Power not available" are displayed via the localization function `FUN_005898a0`.
- Rage drain rates per difficulty use `PTR_DAT_007c07f8 + 0x3DC` (raw frame delta) multiplied by the ability cost constants at `DAT_007c1b3c`–`DAT_007c1b4c` (documented in Section 12).
- Stuck detection: when `+0x390 == 0` and state is 0x105, tracks player position. If player doesn't move more than 0.25 units for 10 seconds, kills the player (health set to 0).

---

## 17. v1.2 Findings (Weapon Wheel)

Findings made while building the weapon wheel: drawing our own UI, borrowing the game's art, and blocking the right stick.

### Renderer

The game renders with **Direct3D 8** (`rayne2.exe` imports `Direct3DCreate8` from `d3d8.dll`). The GoG build ships DXVK (v2.6.2) as `d3d8.dll`/`d3d9.dll`, which translates D3D8 → D3D9 → Vulkan, but the game only ever talks to the D3D8 interface. The Windows SDK no longer ships `d3d8.h`, so the mod declares the vtable indices and constants it needs (see `Overlay.h`).

#### Confirmed

| Address | Type | Description | Notes |
|---------|------|-------------|-------|
| `0x05D74620` | `IDirect3DDevice8*` | The game's D3D device | Written by `CreateDevice` (IDirect3D8 vtable `+0x3C`) in `FUN_00490690`. Null until the renderer initializes. |
| `0x05D7461C` | `IDirect3D8*` | The D3D8 object | Returned by `Direct3DCreate8(0xDC)` |
| `FUN_0048F820` | function | Present wrapper | `int __cdecl (void)`. Runs once per frame on the render thread. Presents through the device (vtable `+0x3C`) or, in one windowed path, a secondary swap chain (`+0x8014` → `+0xC`). **Hooked by the mod's overlay**; we draw right before the original runs. Confirmed in game on GoG at 1024x768 and 1440p, draws over menus too. |
| `FUN_004905D0` | function | Device reset wrapper | Calls device `Reset` (vtable `+0x38`), then rebuilds resources. Not hooked: the overlay uses managed textures, which survive a Reset. |
| `FUN_00490690` | function | Device creation | Source file `E:\projects\br2\directx\rendd3d.cpp` |

- The device is created with behavior flags `0x42` (hardware vertex processing | FPU preserve) and **without** `D3DCREATE_MULTITHREADED`, so all D3D calls must happen on the render thread.
- `BeginScene`/`EndScene` are called from several functions per frame (`FUN_00488000`, `FUN_0048F600`, `FUN_00489480`, `FUN_00489520`), so `EndScene` is not a once-per-frame hook point.

### .TEX Texture Format

| Offset | Type | Description |
|--------|------|-------------|
| `+0x00` | uint32 | Always 3 in observed files |
| `+0x04` | uint32 | Always 3 in observed files |
| `+0x08` | uint32 | Width |
| `+0x0C` | uint32 | Height |
| `+0x1C` | uint32 | `0x0019D318` in every observed file |
| `+0x20` | bytes | Raw 32bpp pixels, B, G, R, A byte order (= `D3DFMT_A8R8G8B8` in memory), uncompressed |

File size is always `0x20 + width * height * 4` for the files checked (HUD and select wheel textures).

### POD3 Index (on disk)

Supplements Section 8, which describes the in-memory structures. On disk:

| Offset | Type | Description |
|--------|------|-------------|
| `0x058` | uint32 | File count |
| `0x108` | uint32 | Index offset |

The index is `file count` entries of **20 bytes**: `{ name offset, size, data offset, timestamp, checksum }`. The string table (null-terminated names such as `ART\HUD.TEX`) follows directly after the index. File data is stored uncompressed.

### Unused Select Wheel Art

`W32ART.POD` contains art for a radial selector that `rayne2.exe` never references by name (likely a cut feature):

| File | Size | Contents |
|------|------|----------|
| `ART\SELECTWHEEL_BASE.TEX` | 256x256 | Bladed wheel with 8 rings, clockwise from the top. Ring centers at radius ~82px from the center; ring holes ~26px across. |
| `ART\SELECTWHEEL_ICON.TEX` | 128x128 | 4x4 grid of 32x32 icons. The first six are the weapon modes in index order (Shot, Stream, Spray, Bomb, Flame, Hammer); the rest appear to be powers. |
| `ART\SELECTWHEEL_CENTER.TEX` | 64x32 | Center piece, unused so far |

`ART\HUD.TEX` holds the regular HUD, including the six pink weapon-mode icons down its right edge.

### Gamepad Polling

`FUN_0068DDB0` polls the gamepads once per frame. It loops over **4 pads**, calling one of two poll functions per pad (chosen by the flag at `0x061B5A18 + pad`):

| Function | API | Notes |
|----------|-----|-------|
| `FUN_0068C550` | DirectInput (`GetDeviceState`, 0x50 bytes = `DIJOYSTATE`) | Reports buttons 0–27, then the POV hat as buttons 28–31. With no device it still reports every button as released, then zeroes the analog values. |
| `FUN_0068CA20` | XInput (`XInputGetState`) | Returns early (no button calls) if the pad isn't connected. Reports buttons in the order A, B, X, Y(0), RB, LB, Start, Back, ... d-pad; RT/LT (5/4) are reported as buttons when past a threshold. Sticks pass through a deadzone function, `FUN_00467AA0`, before buttons are reported. |

Both poll functions **write every analog value before reporting any button**, and report button 0 exactly once per poll. Nothing reads the analog values until the poll returns.

#### Per-Pad Input State

Each pad has a state block at `0x007E10A0 + pad * 0x1E4`. This is the `this` pointer of the button state function `FUN_00467600` (already hooked by the mod's gamepad support).

| Offset | Type | Description |
|--------|------|-------------|
| `+0x00` | int | Device type (checked against 0, 3, 4) |
| `+0x28` | float | Left stick X (pad 0: `0x007E10C8`) |
| `+0x2C` | float | Left stick Y (pad 0: `0x007E10CC`) |
| `+0x30` | float | Right stick X (pad 0: `0x007E10D0`) |
| `+0x34` | float | Left trigger (pad 0: `0x007E10D4`) |
| `+0x38` | float | Right trigger (pad 0: `0x007E10D8`) |
| `+0x3C` | float | Right stick Y (pad 0: `0x007E10DC`) |
| `+0x40` | float | 7th axis (DirectInput only) |
| `+0x4C` | int[32] | Button "changed" values |
| `+0xE0` | int[32] | Button pressed state (written by `FUN_00467600`) |
| `+0x160` | int[32] | Previous frame's button state |

#### Blocking the Right Stick

The game never reads the right stick globals directly; consumers go through the `gamepadAxis` mapping (Section 16, `rayne2.ini`). Lowering the cheat menu time factor does **not** slow camera turning (confirmed in game), so the stick has to be blocked while the weapon wheel is open.

The mod does this inside the existing button hook: on the button 0 call for pad 0, it copies the right stick into its own atomics, then zeroes the globals if blocking is on. The camera sees a centered stick, while the mod still reads the real values.

### Hero List (Rayne Pointer)

`0x061276EC` (the mod's `Rayne2::RayneBasePtr`) is not a standalone pointer: it is slot 0 of a hero/player list at `*(0x007C5D50) + 0x3F0C`, 11 slots of 8 bytes each, scanned by functions such as `FUN_00457AD0` and `FUN_004548F0` (which stop at the first empty slot). Nothing writes the global directly, so the list is filled through the base pointer.

The mod treats Rayne as valid when slot 0 is non-null and its vftable is `0x007034E4` (`CMainCharacter`, see `FUN_00549B50`). **Scripted events can briefly fail this check.** Confirmed in game: the car explosion at the start of Meatpacking District (O'Leary's Cow) fails it for a moment while Rayne stays controllable. Code that runs across several frames (like the weapon wheel) should only require the check at the moment it acts on Rayne, not cancel when it fails mid-way.

---

## 18. True Dhampir Research (Melee, Movement, Knockdown)

Research for the experimental "True Dhampir" mode (BR1-style Rayne: faster, more acrobatic, faster attacks, no knockdown). BR1 reference values are in `br1 reverse engineering notes.md`. **Everything in this section comes from static analysis and has not been tested in game.**

### Rayne vtable Overrides

`CMainCharacter::vftable` is at `0x007034E4`, and its parent `CVampire::vftable` is at `0x00724054`. The slots Rayne overrides that matter here:

| Slot | Offset | Function | Description |
|------|--------|----------|-------------|
| 2 | `+0x008` | `FUN_005520E0` | Per-frame update (health/rage regen, rage drain, sets time class) |
| 15 | `+0x03C` | `FUN_00551160` | **Receive hit / damage handler**, picks the hit reaction |
| 23 | `+0x05C` | `FUN_0054F340` | Debug info display (prints controller, position, velocities) |
| 50 | `+0x0C8` | `FUN_00549BF0` | Setup (registers the "Main Character" debug tunables) |
| 87 | `+0x15C` | `FUN_0054B990` | Incoming attack warning → auto-block (state `0x79`) |
| 135 | `+0x21C` | `FUN_0054CA30` | **Start move** (move ID parameter) |
| 140 | `+0x230` | `FUN_0054DF00` | Rage mode: `*(*(Rayne+0x11374) + 4)`. 1 = Blood Rage, 2 = Blood Fury, 3 = unknown |

`FUN_00674230` is Rayne's large state update (switch on state `+0x17C`), and `FUN_00672A30` is her numeric animation event handler.

### Rayne Offsets (new)

| Offset | Type | Description | Confidence |
|--------|------|-------------|------------|
| `+0x0050–0x0070` | float[9] | Orientation matrix | Confident |
| `+0x0158` | struct | Embedded `CAnimationController` (vtable at `+0x158`, so `*(Rayne+0x158)` is its vtable). `vtable+4` = `setState(animIndex, 0, -1.0f)`; wrapper `FUN_00658EB0`. `+0x178` = skeleton pointer, `+0x17C` = current animation index (the "state"). See "Minion Knockdowns" below | Confident |
| `+0x017C` | int | Current state ID | Confident |
| `+0x020C` | int | Active power ID (see Power IDs) | Confident |
| `+0x0408 / 0x040C / 0x0410` | float[3] | Body velocity (`bVel`, `+0x40C` vertical) | Confident |
| `+0x0414` | int | Gravity enable. `-1` = no gravity (set by the `nogravity` event and knockdown states) | Confident |
| `+0x0590` | struct | Time class struct (`{int class, ...}`). Rayne uses class 0 | Confident |
| `+0x3A44` | int | If non-zero, hit reactions below severity 4 are ignored (hyper armor?). Setter not found | Guess |
| `+0x3A4C / 50 / 54 / 58` | float | Controller move / movex / strafe / turn | Confident (debug display) |
| `+0x3D10–0x3D18` | float[3] | Move/jump velocity in local space (`bJumpVel`). Lunge velocity is written here by `startMove` | Confident |
| `+0x48F8–0x4900` | float[3] | Jump stick velocity (`bJumpStickVel`) | Confident (debug display) |
| `+0x8354` | int | **Queued next move ID** (started by anim event `0x4B1`) | Confident |
| `+0x8D0D` | bool | Flinch From Bullets (debug tunable). Checked by `FUN_00668980` | Confident |
| `+0x11374` | Pointer | Rage state object (`+4` = rage mode) | Confident |

### Power IDs

`FUN_004FCA50` returns the active power. It returns `*(*(0x007C5928) + 0x18)` unless that is `0x8000`, in which case it returns `Rayne+0x20C`. `FUN_004F8820` (input) writes the pending power to `DAT_05E34714`.

| ID | Power |
|----|-------|
| `0x1001–0x1003` | Aura vision family (`0x1000` bit = aura vision active) |
| `0x2001` | Dilated Perception (slow motion) |
| `0x2002` | Super Speed |
| `0x2003` | Freeze Time |
| `0x4001` | Blood Rage |
| `0x4002` | Blood Fury |
| `0x4003` | Unknown (rage family) |

Rage drain per power is in `FUN_005520E0`: `rage -= rawDt * cost`, using the costs at `0x007C1B3C–0x007C1B4C`.

### Knockdown

The loading screen tip *"POWERS: Blood Rage and Blood Fury prevent Rayne from being knocked down"* led here.

#### Damage Message (`param_1` of `FUN_00551160`)

| Offset | Type | Description |
|--------|------|-------------|
| `+0x00` | int | Damage type (`0x65`–`0x87` seen; `0x67`/`0x84` blade, `0x7E` special, `0x87` = ignored) |
| `+0x08` | int | **Severity, 1–5** |
| `+0x0C` | float | Damage amount |
| `+0x24` | Pointer | Attacker actor |
| `+0x28` | float[3] | Hit position (used when there is no attacker) |

#### Reaction Selection

`FUN_00551160` subtracts health, then picks a reaction state from the severity and the hit direction. The direction comes from `FUN_00668890`: 1 front, 2 right?, 3 back, 4 left?, split at ±45°/±135°.

| Severity | Reaction states | Meaning |
|----------|-----------------|---------|
| 1–2 | `0x9B`–`0x9E` | Light flinch |
| 3 | `0x9F`–`0xA2` | Medium flinch |
| 4 | `0xA3`–`0xA6` | Heavy stagger |
| 5 | `0xAF` (front/sides), `0xB3` (back) | **Knockdown** |
| 5 while raging | `0xA3`–`0xA6` | Downgraded to heavy stagger |

The severity switch is `JMP [EAX*4 + 0x00551B5C]` at `0x005516BD` (`EAX = severity - 1`):

| Table entry | Address | Target |
|-------------|---------|--------|
| severity 1 | `0x00551B5C` | `0x005516C4` |
| severity 2 | `0x00551B60` | `0x00551727` |
| severity 3 | `0x00551B64` | `0x00551772` |
| severity 4 | `0x00551B68` | `0x005517BD` |
| severity 5 | `0x00551B6C` | `0x00551807` |

The severity-5 case starts with `CALL FUN_00660DA0` (is raging: rage mode 1, 2 or 3), then `JZ 0x00551859` at `0x00551810` (`74 47`) jumps to the knockdown branch when not raging.

The reaction is only applied if `FUN_00668980(type, severity)` ("can react") returns true. It returns false when:

- Rayne is dead (`vtable+0x130`)
- Rage mode is 1 or 2, unless the damage type is `0x70`. **So during Blood Rage/Fury, Rayne doesn't react to hits at all**, which is stronger than the loading screen tip suggests
- the damage type is `6`, `0xD`, `0x69`, `0x6B`–`0x6D` or `0x71`
- she's in certain states (harpoon, finishers, etc.)
- some attack states (`0x79`, `0x7A`, `0xD3`, `0x18D`–`0x19F`, ...) are only interrupted by severity ≥ 4 or 5
- it's a bullet hit and Flinch From Bullets (`+0x8D0D`) is off

While raging, damage is taken from rage first and the severity is reduced by 1.

#### Knockdown State Chains

The update function `FUN_00674230` handles eight 4-state knockdown chains, `0xA7`–`0xC6`: fall, land, down, get up. `0xAF`→`0xB2` and `0xB3`→`0xB6` are the two that the damage handler enters. Their "land" states check the `allow_recovery` animation event to allow a recovery roll. The other six (`0xA7`, `0xAB`, `0xB7`, `0xBB`, `0xBF`, `0xC3`) were not traced to an entry point yet. Immediate-value hits for some of them exist in `FUN_0055EB10`, `FUN_005665A0` and `FUN_00483500`, but they are unverified. They may be entered by grabs/throws or physics.

#### Candidate Patches (untested)

1. **Knockdown → heavy stagger (smallest):** change the dword at `0x00551B6C` from `0x00551807` to `0x005517BD`. Severity 5 then plays the severity-4 stagger. The equivalent alternative is to NOP the `JZ` at `0x00551810` (`74 47` → `90 90`), which forces the "raging" path for severity 5 without the rage side effects.
2. **Knockdown → no reaction at all:** hook `FUN_00668980` (it's `__thiscall`, `this` = Rayne) and return false when severity is 5.
3. **Physics and explosions:** if they arrive as damage messages with severity 5, patch 1 covers them. If Rayne still gets knocked down by something, that source is one of the untraced chains above.

#### Candidate Patch 1 Failed In Game

**Confirmed in game (2026-10-18):** with the dword at `0x00551B6C` patched to `0x005517BD` (checked in memory, applied both in level and from the main menu), minion melee attacks **still knock Rayne down**. Timing isn't the problem: the table is read live by `JMP [EAX*4 + 0x551B5C]` at `0x005516BD`. Static re-check:
- The table entry is the only reference to `0x00551807`.
- `0x00551859` (states `0xAF`/`0xB3`) is reached only from the `JZ` at `0x00551810`.
- A scan of `0x540000–0x680000` for immediates `0xA7–0xC6` finds no other code that starts those knockdown states.

So the minion knockdown must use a state outside `0xA7–0xC6`, or get Rayne into a knockdown some other way. **Resolved below:** the heavy reactions `0xA3–0xA6` chain into the down states through animation data.

`FUN_0055EB10` pairs IDs with strings, but those are **front-end menu IDs**, not Rayne states. Dead end for state names.

#### Minion Knockdowns: Heavy Hit Animations Chain Into The Down State

**State log from a real knockdown (2026-10-18, patch 1 active).** Rayne's state goes `0x9B → 0x9F → 0xA3 → 0xC8 → 0xC9 → 0`, or `0x9B → 0xA2 → 0xA5 → 0xCB → 0x14`. The minion combo lands hits of rising severity (1 → 3 → 4). The severity-4 "heavy" reactions **`0xA3–0xA6` lead into the down states by themselves**. No code does this; the animation data does.

**Rayne's "state" is her animation index.** `+0x158` is her `CAnimationController` (not a separate state machine). `+0x17C` (controller `+0x24`) is the current animation index, and `setState` is the controller's vtable `+4`. Animation entries:
- **Pointer path:** skeleton = `[controller+0x20]` = `[Rayne+0x178]` (`FUN_00605FF0`). Table = `[skeleton+0x241C]`. Entry = `table + index * 0xA4` (`FUN_00606040` / `FUN_00606020`).
- **Entry fields:** `+0x68` frame count, `+0x6C` fps, **`+0x74` next animation** (`-1` = hold last frame, `-2` = loop, otherwise `setState(next)` when it ends, in `FUN_00607C20`), `+0x78` end frame, `+0x80` event count, `+0x84` event list (8 bytes each, `short` frame first).
- These come from animation files, so they're runtime data (patch after load, like the weapon tables).

**Down states** (handled in `FUN_00674230`):
- `199` (`0xC7`) / `200` (`0xC8`) = lying on the ground.
- Anim event `0x3F7` sets `200` from `0xB4`, otherwise `199`.
- On the ground, stick input picks a get-up roll: `0xCA–0xCD` from `200`, `0xCF–0xD2` from `199`. Otherwise she stands up with `0xC9` / `0xCE` after 1 s (`+0x81CC` timer).

**Knockdown chains `0xA7–0xC6`:**
- Start state `X` (only `0xAF`/`0xB3` are set by code, from the hit handler).
- `X+1` falls or is in the air. If the jump button is pressed during the `0x723638` event window, `X+1 → X+2`, a recovery flip.
- `X+3` is the end.
- Transitions between them come from animation data.

**Patch 1b (confirmed in game 2026-10-18): map severities 4 and 5 to the severity-3 (medium) reaction.** Sustained minion melee and a Blood Hammer rocket hitting the wall next to Rayne only stagger her, with no knockdown. Feed repel (`0xF1`) is a separate path and still knocks her down. The severity jump table at `0x00551B5C` holds `005516C4, 00551727, 00551772, 005517BD, 00551807` (severities 1–5). Write **`0x00551772`** to both **`0x00551B68`** and **`0x00551B6C`**. The medium reactions `0x9F–0xA2` return to idle (seen in the log: `0x9F → 0`).

**Data-side alternative:** set the `+0x74` next field of entries `0xA3–0xA6` (and the knockdown chain entries) to `0`. Riskier: those animations probably end in a falling pose.

#### Feed Repel Knockdown (State `0xF1`)

**Confirmed in game (2026-10-18):** candidate patch 1 does **not** stop the knockdown Rayne gets when she tries to feed on an enemy holding a melee weapon. That knockdown is state **`0xF1` (241)**, and it happens during Blood Rage/Fury too. It doesn't go through the damage handler `FUN_00551160` at all. It's a separate "feed repelled" path:

- `FUN_00663CC0` is the feed state handler (states `0xE2–0xFE`). In the feed grab states `0xE5/0xE6`, Rayne sends the victim a type-3 message (victim vtable `+0xAC`, victim pointer at `+0x83C0`). If the reply says the victim repels (result 1 plus a flag), it calls `setState(0xF1)` (**`PUSH 0xF1` at `0x00663ED9`**), then the victim's vtable `+0x1B4` (its repel attack).
- `FUN_00672A30` does the same on anim event **`0x645`** (type-2 message) at **`PUSH 0xF1` at `0x006738CE`**. It also calls Rayne's vtable `+0x00` with the victim position.
- `FUN_00669540` ("is in hit reaction") counts states `0x9B–0xD5` **and `0xF1`**, so the game treats `0xF1` as a hit reaction. `FUN_00551160` also special-cases state `0xF1` (`0x005511A1`).
- Other feed outcomes from the same handler (not knockdowns, player-triggered from the feed hold `0xE7/E8`, or `0xF5/F6` while raging): `0xEB/EC`, `0xED/EE`, `0xEF/F0`, `0xE9`, and the rage variants `0xF9–0xFE`, `0xF7/F8`.

**Patch 3 (confirmed in game 2026-10-18; first tested with `9B`, final value `A2`):** with patches 1b and 3 together, Rayne seems fully immune to knockdown. Side effects:
- **Normal minions** still play their feed-counter animation, but Rayne stays put in the light flinch, and the two models clip into each other. (`0xF1` presumably carries the knockback movement that normally separates them.)
- **Ninjas (Twisted Park)** die instantly when Rayne tries to feed on them. The cause isn't traced; their repel logic may expect Rayne to be in `0xF1`.

**Final choice (2026-10-18): `0xA2` at both addresses** (`0x00663EDA` and `0x006738CF`: `F1` → `A2`). With this medium stagger, minions still counterattack, but Rayne staggers back and the clipping mostly goes away, so it looks intended. Ninjas still die instantly on a feed attempt; **the user is keeping that on purpose as an easter egg.**

Original candidate: change the state pushed at both sites from `0xF1` to a stagger. The immediate byte is at **`0x00663EDA`** and **`0x006738CF`**: `F1` → `A3` (heavy stagger) or `9B` (light flinch). The victim still plays its repel attack, but Rayne staggers instead of falling. Risks:
- The stagger states normally get their direction from the hit handler.
- The feed target `+0x83C0` may not be cleared on this path.

A cleaner, harder alternative: make the victim's reply never say "repel" (track the victim's `+0xAC` handler for message types 2 and 3).

### Time Scale (Attack and Movement Speed)

Every actor's delta time goes through the time manager at `*(0x007C6A70)` (the Time Factor object from Section 5). An actor's update calls `FUN_00648DF0(class)` on its time struct, then `FUN_006498E0()`:

```
dt = rawDt (game+0x3DC) * table[mode][class] * timeFactor (+0x2E8)
```

Rayne's update (`FUN_005520E0`) uses her time struct at `Rayne+0x590` with **class 0**. `mode` comes from `FUN_00648E10(activePower)`: 0 normal, 1 Dilated Perception, 2 Super Speed, 3 Freeze Time, 4 Blood Fury. Blood Rage maps to 0.

The table is 30 entries of `0x18` bytes at the start of the time manager, indexed by `class + mode * 6`. Each entry is `{start, target, rampTime, elapsed, ...}`, and the value used is `target` (`+0x04`) once the ramp finishes. Initialized by `FUN_00648F60` (assuming the 30 calls fill entries in order):

| Mode | Class 0 (Rayne) | Classes 1–5 |
|------|-----------------|-------------|
| 0 Normal | 1.0 | 1.0 |
| 1 Dilated Perception | 0.4 | 0.4 |
| 2 Super Speed | 0.8 | 0.4 |
| 3 Freeze Time | 1.0 | 0.5 → 0.02 over 1 s (class 3: 0.05) |
| 4 Blood Fury | 0.8 | 0.8 |

**Experiment:** write a value above 1.0 (e.g. 1.25) to `[*(0x007C6A70)] + 0x00` and `+0x04` (mode 0, class 0). This should speed up everything Rayne does (animations, attacks, combo timing, movement, jump arc) without affecting enemies. `FUN_00648F60` runs from the manager's init (`FUN_00649910`), so the value probably needs reapplying after level loads. Things to check: whether her weapons, projectiles or the camera also use class 0, and whether Super Speed/Fury should get their class 0 entries scaled by the same factor.

**Tuning approach (2026-10-17):** BR1's one-button, context-driven combos don't compare to BR2's input-driven combat, so attack speed has no BR1 target and will be tuned **by feel**. Movement still has BR1 targets (run 27.7 u/s, jump apex about 26 units). The class 0 time scale speeds up attacks and movement together, so a speed-up chosen by feel for attacks will also change movement. If the two need different amounts, movement can be corrected separately (run speed, takeoff velocity, gravity), or attacks need a per-animation playback rate (not found yet).

`startMove` multiplies lunge velocity by 0.667 during Blood Rage and 0.5 during Fury. That suggests rage was once meant to speed Rayne up as well.

### Combos

- `FUN_0054CA30(moveId)` (vtable `+0x21C`) starts a move: sets up lunge velocity (`+0x3D10`), target tracking, etc. Move IDs go from `0x01` to `0x3D`.
- Attack input during a move stores the next move in **`+0x8354`**. When the current animation hits **event `0x4B1`**, `FUN_00672A30` calls `startMove(+0x8354)`. That event's frame is the combo window, so attack recovery time is set by where `0x4B1` sits in each animation.
- Named animation events are queried with `FUN_006061B0(name)`. Names seen: `combocontinue`, `ragecontinue`, `allowharpoon`, `allowTurning`, `allow_recovery`, `nosliding`, `jumpTwistLand`, `check_hit_wall`, `nogravity`, `victim`. During a `combocontinue` or `allowharpoon` window the harpoon (`FUN_00663690`, button state `+0x3A68`) can cancel the move.

**Animation cancel experiment:** in a per-frame hook, if `+0x8354` is non-zero and Rayne is in an attack state, call `startMove(+0x8354)` once the current animation is some fraction of the way through instead of waiting for event `0x4B1`. The animation progress fields are not mapped yet.

### Jumping and Gravity

- World gravity is `32` u/s² (`*(0x007C07F8) + 0x3B0`, from the constant at `0x006E1490`), the same as BR1.
- Rayne's airborne gravity is **3× world gravity (96 u/s²)**. In `FUN_00674230` at `0x006749E8`: `FMUL double ptr [0x006DE838]` (3.0). That constant is probably shared, so patch the instruction to point at a new constant rather than changing it.
- The takeoff velocity is not a constant. Anim events `0x3E9` and `0x3F7` in `FUN_00672A30` call `FUN_00606290`, which computes a velocity from the animation's root motion, then write the vertical part to `+0x40C` and the rest to `+0x3D10`.
- BR1 reference: apex about 26 units, measured in game (27.6 in theory). Matching it under 96 u/s² gravity needs a takeoff of about `sqrt(2 * 96 * 26)` ≈ 71 u/s. **Measured in BR2 (2026-10-17):** standing jump apex is **10 units** (49.19 → about 59.1, so Cheat Engine sampling probably misses the true peak of exactly 10). Under 96 u/s² that implies a takeoff of about **43.8 u/s**, close to BR1's 42. The gap is BR2's 3× airborne gravity, not the launch speed.
- Options to reach 26 units:
  - Lower the gravity multiplier to about 1.15 and keep the takeoff. Air time becomes about 2.4 s against BR1's 1.31 s, so floaty.
  - Raise the takeoff to about 79 u/s and gravity to about 121 u/s² (multiplier about 3.8). This matches both BR1's apex and its air time.

### Mod Implementation (`TrueDhampir.h`)

Implemented 2026-10-18. Enabled with `enableTrueDhampir` in `[features]`; tuned in the `[trueDhampir]` section of `gunsucked.ini`. Everything is applied at startup (all code, no runtime data):
- **Knockdown immunity:** the four verified byte patches (`0x00551B68`, `0x00551B6C` → `0x00551772`; `0x00663EDA`, `0x006738CF` → `0xA2`). Original bytes are checked first, and nothing is written if any of them differ.
- **Run speed and jump hang time:** MinHook on `FUN_00669A40` (only caller: `0x00672A60`, plain `RET`, `__thiscall` with no args). After the original runs, multiply `+0x48B0` by `runSpeedMultiplier` in animations `0x0A`/`0x0B`, or by `jumpAnimationRate` in `0xFF`. This is equivalent to the Cheat Engine injection at `0x00672A65`, which reads `+0x48B0` right after this call.
- **Jump height:** MinHook on `FUN_00672A30` (`__thiscall(float)`, `RET 4`; callers `0x00552BC3` and `0x0047A374`). After the original runs, multiply `+0x160` by `jumpHeightMultiplier` in animation `0xFF`. Equivalent to the injection at `0x00673D7C` (the function has a single `RET`, after the root-motion scale).
- **Attack speed:** `attackSpeedMultiplier` (default 1.8, tuned by feel). Uses the same `FUN_00669A40` hook. `+0x48B0` scales the delta time for **all** of Rayne's animations in `FUN_00672A30` (`FUN_00669A40` resets it to 1.0 every frame and only lowers it in walk/run), so multiplying it in attack animations speeds up the swing and moves its events earlier, including the combo window `0x4B1` (shorter recovery).
  - Attack animations, from startMove's move → animation table `FUN_006660F0`: `0x1A–0x2D` (moves `0x09–0x14`), `0x57–0x77` (moves `0x29–0x3C`), `0x18D–0x19F` (moves `0x18–0x28`), `0x1C5`/`0x1C6` (Blood Rage blade and kick, moves `0x43`/`0x44`).
  - Variant ranges `FUN_00666880` maps back onto those: `0x43–0x56` → `0x1A–0x2D`, `0x7B–0x98` → `0x57–0x74`, `0x1B1–0x1BC` → `0x18D–0x19A`.
  - Other `FUN_006660F0` entries (not attacks): moves `1/6/0x15` → `0xFF` (jump), `2` → `0x109` (double-jump roll), `3/4/7/8` → `0x34/0x3B/0x35/0x3C`, `0x16` → `0x38`, `0x17` → `0x41`, `0x3D–0x42` → `0x2E–0x33`. Meanings not confirmed.
- **Airborne gravity:** `gravityMultiplier` (default 3.0 = stock). The 4-byte address in the `FMUL double ptr [0x006DE838]` at `0x006749E8` (operand at `0x006749EA`, original `38 E8 6D 00`) is repointed at the mod's own `double`, so the shared 3.0 constant stays untouched. Affects only the physics air states (double-jump roll `0x109`, fall `0x105`), not the animation-driven single jump.
- Both hooks only act when `this` is the hero in slot 0 (`*0x061276EC`).
- **Defaults tuned by feel in game (2026-10-18):** `runSpeedMultiplier` 1.62, `jumpHeightMultiplier` 1.8, `jumpAnimationRate` 0.75, `gravityMultiplier` 2.6, knockdown immunity on. That's a standing jump of about 18 units with about 1.0 s of air time; the BR1-matching values (2.6 height, 0.57 rate) leave the camera.

### BR1 Reference Values ("True" BR1 Rayne)

Measured in BR1 by the user with Cheat Engine, or derived from `rayne1.exe` code (details in `br1 reverse engineering notes.md`). **These are reference points, not 1:1 targets.** BR2's jump is animation-driven and its camera and proportions differ, so the True Dhampir values get tuned by feel toward these.

| Quantity | BR1 value | Source | BR2 stock |
|----------|-----------|--------|-----------|
| Run speed (full stick) | **27.7 u/s** | Measured; code: `28 × 1.1 × 0.9` | 17.1 u/s (measured) |
| Run speed, Blood Rage | 36.0 u/s game time (17.5 u/s real time, world at about 0.49×) | Measured + code (`×1.17` instead of `×0.9`) | — |
| Standing jump height | **about 26 units** (27.6 in theory) | Measured | 10 units (measured) |
| Jump takeoff speed | 42 u/s | Code (`DAT_01D1F4AC`) | No takeoff speed (animation root motion) |
| Gravity | 32 u/s² world gravity, jump states run at **2×** speed (effectively 84 u/s launch against 128 u/s²) | Code | 32 world gravity, ×3 in physics air states |
| Time to apex | **0.66 s** | Derived | about 0.37 s (half of `0xFF`) |
| Total air time (standing jump) | **1.31 s** | Derived | about 0.75 s (state `0xFF`, measured) |
| Jump arc shape | Parabolic (gravity) | Code | Nearly linear up and down (animation) |
| Variable jump height | Yes: releasing jump while rising switches gravity to 60 | Code | — |
| Attack timing | Reference only, single-button contextual combos (Section 3 of BR1 notes) | Code | Tune by feel |

### Single Jump Is Animation-Driven (Jump Height Patch)

**State logs (2026-10-18):**
- Single jump: `0xFF` (jump animation, ~750 ms) → `0x105` (physics fall, one sample) → `0x103` (landing, ~500 ms) → `0`.
- Double jump: `0xFF` (~150 ms) → `0x109` (mid-air roll) → `0x105` → `0x103`.

**The single jump has no takeoff velocity or gravity.** The `0xFF` animation's vertical root motion moves Rayne. The physics path (takeoff event `0x3E9` writing `+0x40C`, ×3 gravity at `0x006749E8`) only applies in the physics-driven air states such as the double-jump roll `0x109` and the fall `0x105`. **Confirmed in game:** patching the takeoff and gravity changed only the double jump. A lower gravity multiplier made the roll float or climb, and the single jump didn't change.

**Patch (confirmed in game 2026-10-18):** inject at **`0x00673D7C`** (`FMUL [ESI+0x160]`, bytes `D8 8E 60 01 00 00`, inside `FUN_00672A30`'s root-motion scale) and multiply the vertical root motion `+0x160` by `jumpY` while `+0x17C == 0xFF`.
- With `jumpY = 2.6`, the standing jump measured **25.5 units** at 10 Hz sampling (49.13 → 74.62). That's about 26 with sampling loss, matching BR1.
- The vertical speed profile is roughly linear (about +83 u/s up, about -80 u/s down), not parabolic, because it's animation-driven.
- Timing is unchanged (~0.75 s against BR1's 1.31 s), so it reads as higher and faster than BR1.
- Hang-time option: slow the `0xFF` animation's playback (state check in the `0x00672A65` rate injection). Height is unaffected, because total root motion doesn't depend on rate.

### Measured Movement (BR2 vs BR1)

| Quantity | BR2 (measured) | BR1 (measured) | Ratio |
|----------|----------------|----------------|-------|
| Full-stick run speed | **about 17.1 u/s** | 27.7 u/s | ×1.62 |
| Standing jump apex | **10 units** | about 26 units | ×2.6 |

BR2 run speed is the average of 10 Hz position-delta samples on flat-ish ground. The samples alternate between about 15.7 and 18.3, a 6 vs 7 frame split per 100 ms window.

### Run Speed Comes From Animation Root Motion

BR2 has **no run speed constant**. Rayne moves by the root motion of her locomotion animations (`walk_forward_start`, `walk_forward`, `run_start`, `run_forward`; indices stored at `+0x8BCC/8BD0/8BD4/8BD8` by `FUN_0066D310`). The ~17.1 u/s is baked into `run_forward`'s data.

Per-frame flow (Confident, static analysis):

1. `FUN_00672A30(dt)` (Rayne's animation advance) first calls `FUN_00669A40`, which sets:
   - `+0x48B0`: locomotion **playback rate**. Default 1.0; reduced in states 10/11 for partial stick to blend walk and run.
   - `+0x48B4`: root-motion **distance scale**. Default 1.0; derived from stick magnitude.
   - `+0x48AC`: walk/run blend.
   
   At full stick it returns early and leaves all three at their defaults (rate 1, scale 1).
2. `0x00672A65`: `FLD [ESI+0x48B0]` / `FMUL [EBP+8]`. The animation dt is multiplied by the playback rate, then `FUN_00607C20` advances the animations and accumulates root motion into **`+0x15C/160/164`** (this frame's displacement).
3. `0x00673D5A`: the displacement `+0x15C–164` is multiplied by `+0x48B4`.
4. `FUN_00674230` (movement/physics): for locomotion animations with forward stick (`+0x3A4C > 0`), it keeps the displacement's **length** but points it along the stick direction. It then adds `+0x3F0–3F8`, writes the move to `+0x3A0C–3A14`, and calls the collision mover `FUN_00691710`.

Ways to speed up running (and only running):
- **Scale the playback rate in states 10/11** (inject at `0x00672A65`, or hook `FUN_00669A40` and multiply `+0x48B0` afterwards). The run cycle plays faster and covers proportionally more ground, so no foot sliding. Footsteps speed up too. Target factor: 27.7 / 17.1 ≈ **1.62** (divided by any class 0 time scale already applied). **Confirmed in game (2026-10-18)** with a Cheat Engine injection at `0x00672A65` (multiply when `+0x17C` is `0x0A` or `0x0B`) at ×1.62. Rayne runs visibly faster and feels right. Run-to-jump, run-to-attack, running while shooting and run-to-feed transitions all look normal, with no glitches or timing problems.
- **Scale the distance only** (multiply `+0x48B4` after `FUN_00669A40`). This keeps the leg cycle and slides the feet.

`+0x4904` is **not** run speed. It's `18 / duration(anim 0xFF)`: the forward-jump horizontal speed used for air steering (`FUN_0066CB80`, `FUN_00669F00`).

### Manual Tests Wanted

1. ~~Jump apex~~ and ~~run speed~~: done, see the table above. Optional: read `Rayne+0x40C` right after takeoff to confirm the implied 43.8 u/s.
3. Get knocked down by a basic enemy, an explosion and a thrown object, and note the state ID (`+0x17C`) each time. This tells us which knockdown chains are in play and whether they all come through `FUN_00551160`.
