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