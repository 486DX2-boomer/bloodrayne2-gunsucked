## BloodRayne 1 (Terminal Cut, GoG) Reverse Engineering Notes

Research notes for the experimental "True Dhampir" mode, which tries to make BR2's Rayne play more like BR1's. The goal was to get reference numbers for BR1 Rayne's movement, jumping and attack speed, not to map BR1 in depth.

Everything here comes from static analysis of `rayne1.exe` in Ghidra (project `G:\Ghidra\projects\br-tc-gog`). **None of it has been confirmed in game yet.** BR1 is the "Nocturne" engine generation of Terminal Reality's tech, so the class layout is different from BR2, but naming (`CMainCharacter`, `MAINCHAR.CPP`) and many concepts carry over.

## Table of Contents

1. [Units](#1-units)
2. [Movement and Jump Constants](#2-movement-and-jump-constants)
3. [Attack Animation Durations](#3-attack-animation-durations)
4. [Rayne Tuning Globals](#4-rayne-tuning-globals)
5. [CMainCharacter Layout](#5-cmaincharacter-layout)
6. [Key Functions](#6-key-functions)
7. [Open Questions / Manual Tests Wanted](#7-open-questions--manual-tests-wanted)

---

## 1. Units

World gravity in BR1 is **32 units/s²** (Section 2). BR2's world gravity is also **32** (`*(0x007C07F8) + 0x3B0`, set from the constant at `0x006E1490`). 32 is the real-world value in feet/s², so both games most likely use **feet** and BR1 numbers can be compared to BR2 numbers directly. This still needs a sanity check in game (Section 7).

## 2. Movement and Jump Constants

### Confident (static analysis)

| Quantity | Value | Where | Notes |
|----------|-------|-------|-------|
| World gravity | `32.0` u/s² | `FUN_00477380` writes `game+0x338` (`game = *(0x0062D6D0)`) | Read by `FUN_004291C0` (generic actor gravity) and Rayne's think function |
| Run speed (full stick) | **`27.7` u/s** (measured in game) | `FUN_00499D90` × `FUN_004A1150` | `((1-s)*1.8 + s*28) * 1.1 * 0.9`, `s` = stick magnitude 0–1 (`FUN_0049A5E0`). The `0.9` is applied in the locomotion case of `FUN_004AD9D0`. Theory gives 27.72 |
| Run speed (Blood Rage) | `36.0` u/s game time, **17.5 u/s real time** (measured) | `FUN_004A1150` | Multiplier becomes `1.17` instead of `0.9` when `FUN_004A0180` is true: view mode `FUN_0049AF80() == 5` (Blood Rage, confirmed by the `viewModeBloodRage` → 5 mapping in `FUN_00470C40`) or Mentor rage. The measured speed implies Blood Rage runs game time at about **0.49×** (17.5 / 36.0). Rayne is 1.3× faster relative to the world but slower in real time. The time scale value itself wasn't located |
| Run speed (minimum) | `1.78` u/s | `FUN_00499D90` | Same formula at `s = 0` |
| Run speed (non-local hero) | `38.5` u/s | `FUN_00499D90` | Uses 35 instead of 28 when the character is not the local player and `+0x5B54` is set (probably AI-driven Rayne) |
| Jump takeoff vertical velocity | `42.0` u/s | `FUN_004ABD50`, anim event `0x7D1` (2001) | Sets velocity to `(0, 42, 0)`. Takeoff is timed by an event in the jump animation |
| Max movement speed clamp | `60.0` u/s | `DAT_01D1F450`, used in `FUN_004AD9D0` | Displacement per frame is clamped to `dt * 60` |
| Fast-fall threshold | `-40.0` u/s | `DAT_01D1F480`, fall state case `0x004AE518` | Fall state changes when vertical velocity drops below this. Exact meaning unknown (hard landing?) |

### Vertical Time Multiplier

Rayne's think function (`FUN_004AD9D0`) integrates vertical motion with a per-state multiplier `m`:

```
vel.y -= dt * m * gravity
pos   += vel * (dt * m)
```

| Motion state | `m` |
|--------------|-----|
| `0x2B` (jump), `0x31`, `0x3A` (falling), `0x41` (falling) | `2.0` |
| `0x3F`, `0x40` | `1.7` |
| everything else | `1.0` |

Because both the gravity and the position step use `m`, this is a **time scale** for vertical motion, not a gravity change. The jump is twice as fast, but the apex height is unaffected:

- **Apex height** = `42² / (2 * 32)` = **27.6 units** (about 8.4 m if the units are feet)
- **Time to apex** = `42 / (32 * 2)` = **0.66 s**
- **Air time** back to takeoff height = **1.31 s**
- Equivalent real-time values: launch at 84 u/s against an effective 128 u/s² of gravity

**Measured in game (2026-10-17):** standing jump with jump held, takeoff height `13.563`, peak `38.9–39.4`, so the apex is **about 26 units**. That's a bit below the theoretical 27.6, which fits the integration: velocity is updated before position each frame with a step of `m * dt`, and that loses about `v0 * m * dt / 2` of height (about 1.4 units at 30 fps). Variable frame times explain the spread between jumps. Back-to-back jumps go noticeably lower without a short pause (cause not found). **Don't copy that for True Dhampir.** Target for BR2: **26 units**.

**Variable jump height:** in the jump state (`0x2B`, case `0x004AE149`), while rising, gravity is swapped from 32 to `DAT_01D1F478` (**60**) unless `+0x850` equals 1. `+0x850` is probably "jump button held" (Guess). Letting go early cuts the jump short.

## 3. Attack Animation Durations

> Reference only. BR1 combos are single-button and contextual, so these durations aren't a target for BR2 (see Section 7).

### Confident (static analysis)

BR1 forces each attack animation to a fixed duration instead of using the frame rate it was authored at. `FUN_0049AF00(motionName, N)` sets the motion's playback rate:

```
motion->fps (+0x28) = motion->frameCount (+0x24) / N * 30 * 0.95
```

so every attack lasts **`N / 28.5` seconds**, whatever its frame count. Called from Rayne's setup function `FUN_004A24F0`:

| Motion | N | Duration |
|--------|---|----------|
| `attack_1n_l`, `attack_1n_r`, `attack_1s_l`, `attack_1s_r` | 12 | 0.42 s |
| `attack_2n`, `attack_2s` | 20 | 0.70 s |
| `attack_3n` | 25 | 0.88 s |
| `attack_4n`, `attack_4s` | 30 | 1.05 s |
| `attack_5n`, `attack_5s` | 35 | 1.23 s |
| `attack_1hn`, `attack_1hs` | 24 | 0.84 s |
| `attack_2hs` | 30 | 1.05 s |
| `attack_3hn`, `attack_3hs` | 40 | 1.40 s |
| `attack_4h` | 50 | 1.75 s |
| `attack_5h` | 60 | 2.11 s |

The number is the combo step (1–5). The `n`/`s`/`h` suffixes are guesses, possibly normal/strafe/heavy (or upgraded blades).

**Guess:** `+0x24` is the frame count and `+0x28` the playback fps. This fits the formula, but I haven't checked the motion struct.

### Combo Input

`FUN_004A82F0` handles attack input. It advances a combo counter at `+0x5B78` (`param_1[0x16DE]`), wraps it back to 1 past a maximum (`+0x5B4C` or `+0x5B50`, depending on mode), and plays the matching attack sound via `FUN_004A4370`. The combo step is also limited by `setComboLevel` (script command, writes `+0x5B4C`). I haven't looked at the timing windows.

## 4. Rayne Tuning Globals

`FUN_004A24F0` (Rayne setup) writes a block of globals at `0x01D1F440–0x01D1F4B8` from `.rdata` constants. The ones whose readers I traced:

| Global | Value | Read by | Meaning |
|--------|-------|---------|---------|
| `0x01D1F4AC` | 42 | `FUN_004ABD50`, jump case | Jump takeoff velocity (Confident) |
| `0x01D1F450` | 60 | `FUN_004AD9D0`, `FUN_0049EF80` | Max movement speed (Confident) |
| `0x01D1F480` | -40 | fall case `0x004AE518` | Fall velocity threshold |
| `0x01D1F478` | 60 | jump case | Gravity used while rising once jump is released (variable jump height) |
| `0x01D1F474` | 1.0 | `FUN_004AD9D0` | Velocity scale |
| `0x01D1F498` | 44.5 | `FUN_004A9710` | Harpoon/chain related (compared to chain length) |
| `0x01D1F458` | 186 | `FUN_004A9710` | Harpoon pull speed? |
| `0x01D1F45C` | 66 | `FUN_004A71F0` | Harpoon/rail related |
| `0x01D1F4A4` | 8 | `FUN_004A0EC0` | Stick-driven velocity scale (some special mode) |
| `0x01D1F440` | 6 | `FUN_0049EF80` | Unknown |
| `0x01D1F47C` | -32 | `FUN_0049EF80` | Unknown |
| `0x01D1F48C` | 1.0 | `FUN_0049EF80` | Unknown |
| `0x01D1F444` (5), `0x01D1F44C` (0.13), `0x01D1F454` (8), `0x01D1F468` (25), `0x01D1F488` (10), `0x01D1F490` (32), `0x01D1F4B8` (20) | | not traced | |

## 5. CMainCharacter Layout

### Confident (static analysis)

| Offset | Type | Description |
|--------|------|-------------|
| `+0x24 / +0x28 / +0x2C` | float[3] | Position (`+0x28` is height) |
| `+0x51C` | float | Ground height under Rayne |
| `+0x3AC / +0x3B0 / +0x3B4` | float[3] | Per-frame displacement accumulator, added to the move and zeroed every frame |
| `+0x528 / +0x52C / +0x530` | float[3] | Second per-frame displacement accumulator, also zeroed every frame (locomotion: stick direction × run speed × dt is added here) |
| `+0x534 / +0x538 / +0x53C` | float[3] | **Physics** velocity (jumps, falls, knockback), not locomotion. Horizontal is 0 during normal running (confirmed in game). `+0x538` is vertical velocity, set to 42 at takeoff, reduced by gravity, zeroed on landing |
| `+0x850` | int | Checked by the jump state. Probably "jump held" (Guess) |
| `+0x824 / +0x828` | float | Stick axes (move magnitude is `max(abs)` of these) |
| `+0x924` | float | `1.25` for Rayne (set by many actor classes, meaning unknown) |
| `+0x930` | float | `1.6001` for Rayne (set by many actor classes, meaning unknown) |
| `+0x5A98` | float | Jump animation frame count |
| `+0x5A9C` | int | Jump animation index (`"jump"`) |
| `+0x5AA0–0x5AAC` | int | Locomotion blend animations: `blend_walk_n`, `blend_dash_n`, `blend_dash_bigGun`, `blend_run_s` |
| `+0x5B4C` | int | Max combo level (2 for Rayne, 5 for Mentor and Butcheress hero types) |
| `+0x5B78` | int | Current combo step |

**View mode** (`FUN_0049AF80`): `*(*(+0x568) + 4)` if `+0x568` is set (it points at the global view-mode object `*(0x0062D6D4)` for the local hero), otherwise `+0x908`. Values from `FUN_00470C40`: 0 normal, 1 aura, 2 zoom, 3 slow-mo, 4 (returned when vtable `+0xD8` > 0), 5 Blood Rage. Cheat Engine: `"rayne1.exe"+184AECC`, offsets `568`, `4`.

The current motion state is read through `FUN_004DAF50()` → `+0x2C`.

**Hero pointer (confirmed in game):** `DAT_01C4AECC` is the first slot of the hero-pointer array (`CMainCharacter*`, indexed by hero number). It is filled by `CDemonMission::createOneHero` (`FUN_004DA240`, which builds the object with `FUN_004905D0("CMainCharacter")`), cleared by `FUN_004DA390`, and repointed by the `setHero` script command (`FUN_0050D5A0`). The `"HeroPos: %4.2f,%4.2f,%4.2f"` debug line (`FUN_00474850`) reads `[0x01C4AECC]+0x24/28/2C`.

`rayne1.exe` has ASLR enabled (DllCharacteristics `0x8140`), so in Cheat Engine use **`"rayne1.exe"+184AECC`** as the pointer base.

Running speed isn't stored as a velocity. Locomotion goes through the per-frame displacement accumulators above, so measure horizontal speed from position deltas.

## 6. Key Functions

| Address | Description |
|---------|-------------|
| `FUN_004A24F0` | Rayne setup: bones, animations, tuning globals, attack durations (`MAINCHAR.CPP`) |
| `FUN_004AD9D0` | Rayne think / movement state machine. The switch on motion state at `0x004ADE21` (table `0x004B08C0`, byte index table `0x004B096C`) is not recovered by the decompiler. Decompile individual cases by creating functions at the table targets |
| `FUN_004ABD50` | Rayne animation event handler (event `0x7D1` = jump takeoff, `0x3E9`, `4000`/`0xFA4` = chain) |
| `FUN_00499D90` | Run speed from stick magnitude |
| `FUN_004A2180` | Locomotion animation blending (walk/dash/run) |
| `FUN_0049AF00` | Force a motion's duration (attack speed) |
| `FUN_004A82F0` | Attack input / combo counter |
| `FUN_004291C0` | Generic actor gravity (`vel.y -= gravity * dt`) |
| `FUN_00477380` | Game settings defaults, including gravity `32` at `+0x338` |

## 7. Open Questions / Manual Tests Wanted

These need the game running, which I won't do myself:

1. **Units sanity check.** Compare Rayne's position against something of known size (a door or a crate she can stand on, using `+0x28` on the floor and on top). If a door is about 7 units tall, the units are feet.
2. ~~**Run speed.**~~ Done: 27.7 u/s on flat ground, measured from position deltas at 10 Hz (Section 2). The individual samples alternate between about 25.2 and 29.6 because the 100 ms timer window catches a different number of game frames each time. The average is what counts.
3. ~~**Jump apex.**~~ Done: about 26 units (Section 2).
4. ~~**Attack timing.**~~ Dropped (2026-10-17). BR1's one-button, context-driven combos don't map onto BR2's input-driven combat system, so BR2 attack speed will be tuned by feel. Original plan: time a full 5-hit combo. Static analysis says `0.42 + 0.70 + 0.88 + 1.05 + 1.23 ≈ 4.3 s` if each attack plays out fully (chaining probably cuts this short).
