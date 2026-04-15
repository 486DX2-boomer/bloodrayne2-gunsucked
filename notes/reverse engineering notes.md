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