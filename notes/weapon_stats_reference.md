# BloodRayne 2 Weapon Stats Reference

## Overview

All weapon stats are initialized in `FUN_00585480` at address `0x00585480`. The function writes hardcoded values to the CMultigun object. Each weapon mode occupies approximately 0x74 bytes (116 bytes) within the CMultigun structure.

The base offset for weapon data within CMultigun is `0x2BC` (where Blood Shot's name string begins).

## Weapon Mode Offsets (Absolute within CMultigun)

| Weapon | Name Offset | Data Start | Data End |
|--------|-------------|------------|----------|
| Blood Shot | 0x2BC | 0x2BC | 0x32F |
| Blood Stream | 0x330 | 0x330 | 0x3A3 |
| Blood Spray | 0x3A4 | 0x3A4 | 0x417 |
| Blood Bomb | 0x418 | 0x418 | 0x48B |
| Blood Flame | 0x48C | 0x48C | 0x4FF |
| Blood Hammer | 0x500 | 0x500 | 0x573 |

---

## Blood Shot (Single) - Base Offset 0x2BC

| Property | Offset | Hex | Value |
|----------|--------|-----|-------|
| Name | 0x2BC | - | "Blood Shot" |
| Ammo Cost L1 | 0x2DC | 0x41C80000 | 25.0 |
| Ammo Cost L2 | 0x2E0 | 0x41900000 | 18.0 |
| Ammo Cost L3 | 0x2E4 | 0x41700000 | 15.0 |
| Damage L1 | 0x2E8 | 0x42480000 | 50.0 |
| Damage L2 | 0x2EC | 0x425C0000 | 55.0 |
| Damage L3 | 0x2F0 | 0x42700000 | 60.0 |
| Fire Rate L1 | 0x2F4 | 0x40800000 | 4.0 |
| Fire Rate L2 | 0x2F8 | 0x40A00000 | 5.0 |
| Fire Rate L3 | 0x2FC | 0x40C00000 | 6.0 |
| Unknown 1 | 0x300 | 0x00000000 | 0 |
| Unknown 2 | 0x304 | 0x00000000 | 0 |
| Unknown 3 | 0x308 | 0x00000000 | 0 |
| Range | 0x30C | 0x42A00000 | 80.0 |
| Type ID | 0x310 | 0x15 | 21 |
| Unknown 4 | 0x314 | 0x65 | 101 |
| Unknown 5 | 0x318 | 0x00000000 | 0 |
| Unknown 6 | 0x31C | 0x00 | 0 (byte) |
| Unknown 7 | 0x320 | 0x00000001 | 1 |
| Unknown 8 | 0x324 | 0x00000000 | 0 |
| Unknown 9 | 0x328 | 0x00000000 | 0 |
| Unknown 10 | 0x32C | 0x00000000 | 0 |

---

## Blood Stream (Rapid) - Base Offset 0x330

| Property | Offset | Hex | Value |
|----------|--------|-----|-------|
| Name | 0x330 | - | "Blood Stream" |
| Ammo Cost L1 | 0x350 | 0x41C80000 | 25.0 |
| Ammo Cost L2 | 0x354 | 0x41B00000 | 22.0 |
| Ammo Cost L3 | 0x358 | 0x41A00000 | 20.0 |
| Damage L1 | 0x35C | 0x42480000 | 50.0 |
| Damage L2 | 0x360 | 0x425C0000 | 55.0 |
| Damage L3 | 0x364 | 0x42700000 | 60.0 |
| Fire Rate L1 | 0x368 | 0x41000000 | 8.0 |
| Fire Rate L2 | 0x36C | 0x41200000 | 10.0 |
| Fire Rate L3 | 0x370 | 0x41400000 | 12.0 |
| Unknown 1 | 0x374 | 0x00000000 | 0 |
| Unknown 2 | 0x378 | 0x00000000 | 0 |
| Unknown 3 | 0x37C | 0x00000000 | 0 |
| Range | 0x380 | 0x42A00000 | 80.0 |
| Type ID | 0x384 | 0x16 | 22 |
| Unknown 4 | 0x388 | 0x65 | 101 |
| Unknown 5 | 0x38C | 0x00000000 | 0 |
| Unknown 6 | 0x390 | 0x00 | 0 (byte) |
| Unknown 7 | 0x394 | 0x00000001 | 1 |
| Unknown 8 | 0x398 | 0x00000000 | 0 |
| Unknown 9 | 0x39C | 0x00000000 | 0 |
| Unknown 10 | 0x3A0 | 0x00000000 | 0 |

---

## Blood Spray (Scatter) - Base Offset 0x3A4

| Property | Offset | Hex | Value |
|----------|--------|-----|-------|
| Name | 0x3A4 | - | "Blood Spray" |
| Ammo Cost L1 | 0x3C4 | 0x42C80000 | 100.0 |
| Ammo Cost L2 | 0x3C8 | 0x42480000 | 50.0 |
| Ammo Cost L3 | 0x3CC | 0x42480000 | 50.0 |
| Damage L1 | 0x3D0 | 0x41555555 | 13.33 |
| Damage L2 | 0x3D4 | 0x41055555 | 8.33 |
| Damage L3 | 0x3D8 | 0x41200000 | 10.0 |
| Fire Rate L1 | 0x3DC | 0x3F800000 | 1.0 |
| Fire Rate L2 | 0x3E0 | 0x3F800000 | 1.0 |
| Fire Rate L3 | 0x3E4 | 0x3F800000 | 1.0 |
| Unknown 1 | 0x3E8 | 0x00000000 | 0 |
| Unknown 2 | 0x3EC | 0x00000000 | 0 |
| Unknown 3 | 0x3F0 | 0x00000000 | 0 |
| Range | 0x3F4 | 0x42A00000 | 80.0 |
| Type ID | 0x3F8 | 0x0E | 14 |
| Unknown 4 | 0x3FC | 0x65 | 101 |
| Unknown 5 | 0x400 | 0x00000000 | 0 |
| Unknown 6 | 0x404 | 0x00 | 0 (byte) |
| Unknown 7 | 0x408 | 0x00000004 | 4 |
| Unknown 8 | 0x40C | 0x00000000 | 0 |
| Unknown 9 | 0x410 | 0x00000000 | 0 |
| Unknown 10 | 0x414 | 0x00000000 | 0 |

---

## Blood Bomb (Grenade) - Base Offset 0x418

| Property | Offset | Hex | Value |
|----------|--------|-----|-------|
| Name | 0x418 | - | "Blood Bomb" |
| Ammo Cost L1 | 0x438 | 0x43480000 | 200.0 |
| Ammo Cost L2 | 0x43C | 0x42A00000 | 80.0 |
| Ammo Cost L3 | 0x440 | 0x42860000 | 67.0 |
| Damage L1 | 0x444 | 0x42C80000 | 100.0 |
| Damage L2 | 0x448 | 0x42480000 | 50.0 |
| Damage L3 | 0x44C | 0x42480000 | 50.0 |
| Fire Rate L1 | 0x450 | 0x3F800000 | 1.0 |
| Fire Rate L2 | 0x454 | 0x40000000 | 2.0 |
| Fire Rate L3 | 0x458 | 0x3F800000 | 1.0 |
| Unknown 1 | 0x45C | 0x3F800000 | 1.0 |
| Unknown 2 | 0x460 | 0x3F800000 | 1.0 |
| Unknown 3 | 0x464 | 0x3F800000 | 1.0 |
| Range | 0x468 | 0x42480000 | 50.0 |
| Type ID | 0x46C | 0x0F | 15 |
| Unknown 4 | 0x470 | 0x70 | 112 |
| Unknown 5 | 0x474 | 0x00000000 | 0 |
| Unknown 6 | 0x478 | 0x00 | 0 (byte) |
| Unknown 7 | 0x47C | 0x00000005 | 5 |
| Unknown 8 | 0x480 | 0x00000000 | 0 |
| Unknown 9 | 0x484 | 0x00000000 | 0 |
| Unknown 10 | 0x488 | 0x00000000 | 0 |

---

## Blood Flame (Flamethrower) - Base Offset 0x48C

| Property | Offset | Hex | Value |
|----------|--------|-----|-------|
| Name | 0x48C | - | "Blood Flame" |
| Ammo Cost L1 | 0x4AC | 0x43480000 | 200.0 |
| Ammo Cost L2 | 0x4B0 | 0x42A00000 | 80.0 |
| Ammo Cost L3 | 0x4B4 | 0x42860000 | 67.0 |
| Damage L1 | 0x4B8 | 0x42C80000 | 100.0 |
| Damage L2 | 0x4BC | 0x42480000 | 50.0 |
| Damage L3 | 0x4C0 | 0x42480000 | 50.0 |
| Fire Rate L1 | 0x4C4 | 0x3F800000 | 1.0 |
| Fire Rate L2 | 0x4C8 | 0x3F800000 | 1.0 |
| Fire Rate L3 | 0x4C4 | 0x3F800000 | 1.0 |
| Unknown 1 | 0x4D0 | 0x00000000 | 0 |
| Unknown 2 | 0x4D4 | 0x00000000 | 0 |
| Unknown 3 | 0x4D8 | 0x00000000 | 0 |
| Range | 0x4DC | 0x41200000 | 10.0 |
| Type ID | 0x4E0 | 0x05 | 5 |
| Unknown 4 | 0x4E4 | 0x66 | 102 |
| Unknown 5 | 0x4E8 | 0x00000000 | 0 |
| Unknown 6 | 0x4EC | 0x00 | 0 (byte) |
| Unknown 7 | 0x4F0 | 0x00000004 | 4 |
| Unknown 8 | 0x4F4 | 0x00000000 | 0 |
| Unknown 9 | 0x4F8 | 0x00000000 | 0 |
| Unknown 10 | 0x4FC | 0x00000000 | 0 |

---

## Blood Hammer (Rocket) - Base Offset 0x500

| Property | Offset | Hex | Value |
|----------|--------|-----|-------|
| Name | 0x500 | - | "Blood Hammer" |
| Ammo Cost L1 | 0x520 | 0x43960000 | 300.0 |
| Ammo Cost L2 | 0x524 | 0x43030000 | 131.0 |
| Ammo Cost L3 | 0x528 | 0x42E80000 | 116.0 |
| Damage L1 | 0x52C | 0x43960000 | 300.0 |
| Damage L2 | 0x530 | 0x43A28000 | 325.0 |
| Damage L3 | 0x534 | 0x43AF0000 | 350.0 |
| Fire Rate L1 | 0x538 | 0x3F800000 | 1.0 |
| Fire Rate L2 | 0x53C | 0x3F800000 | 1.0 |
| Fire Rate L3 | 0x540 | 0x3F800000 | 1.0 |
| Unknown 1 | 0x544 | 0x3F800000 | 1.0 |
| Unknown 2 | 0x548 | 0x3F800000 | 1.0 |
| Unknown 3 | 0x54C | 0x3F800000 | 1.0 |
| Range | 0x550 | 0x43480000 | 200.0 |
| Type ID | 0x554 | 0x0A | 10 |
| Unknown 4 | 0x558 | 0x70 | 112 |
| Unknown 5 | 0x55C | 0x00000000 | 0 |
| Unknown 6 | 0x560 | 0x00 | 0 (byte) |
| Unknown 7 | 0x564 | 0x00000005 | 5 |
| Unknown 8 | 0x568 | 0x00000000 | 0 |
| Unknown 9 | 0x56C | 0x00000000 | 0 |
| Unknown 10 | 0x570 | 0x00000000 | 0 |

---

## Summary Comparison Tables

### Ammo Costs (Lower = More Shots per Pool)

| Weapon | Level 1 | Level 2 | Level 3 | Max Shots L1 | Max Shots L3 |
|--------|---------|---------|---------|--------------|--------------|
| Blood Shot | 25.0 | 18.0 | 15.0 | 40 | 66 |
| Blood Stream | 25.0 | 22.0 | 20.0 | 40 | 50 |
| Blood Spray | 100.0 | 50.0 | 50.0 | 10 | 20 |
| Blood Bomb | 200.0 | 80.0 | 67.0 | 5 | 14 |
| Blood Flame | 200.0 | 80.0 | 67.0 | 5 | 14 |
| Blood Hammer | 300.0 | 131.0 | 116.0 | 3 | 8 |

### Damage Values

| Weapon | Level 1 | Level 2 | Level 3 |
|--------|---------|---------|---------|
| Blood Shot | 50.0 | 55.0 | 60.0 |
| Blood Stream | 50.0 | 55.0 | 60.0 |
| Blood Spray | 13.33 | 8.33 | 10.0 |
| Blood Bomb | 100.0 | 50.0 | 50.0 |
| Blood Flame | 100.0 | 50.0 | 50.0 |
| Blood Hammer | 300.0 | 325.0 | 350.0 |

### Fire Rate Values

| Weapon | Level 1 | Level 2 | Level 3 |
|--------|---------|---------|---------|
| Blood Shot | 4.0 | 5.0 | 6.0 |
| Blood Stream | 8.0 | 10.0 | 12.0 |
| Blood Spray | 1.0 | 1.0 | 1.0 |
| Blood Bomb | 1.0 | 2.0 | 1.0 |
| Blood Flame | 1.0 | 1.0 | 1.0 |
| Blood Hammer | 1.0 | 1.0 | 1.0 |

### Range Values

| Weapon | Range |
|--------|-------|
| Blood Shot | 80.0 |
| Blood Stream | 80.0 |
| Blood Spray | 80.0 |
| Blood Bomb | 50.0 |
| Blood Flame | 10.0 |
| Blood Hammer | 200.0 |

---

## Weapon Struct Template (Relative Offsets)

Each weapon mode follows this approximate structure (0x74 bytes):

| Relative Offset | Size | Type | Description |
|-----------------|------|------|-------------|
| 0x00 | 52 | char[] | Weapon name string |
| 0x20 | 4 | float | Ammo Cost Level 1 |
| 0x24 | 4 | float | Ammo Cost Level 2 |
| 0x28 | 4 | float | Ammo Cost Level 3 |
| 0x2C | 4 | float | Damage Level 1 |
| 0x30 | 4 | float | Damage Level 2 |
| 0x34 | 4 | float | Damage Level 3 |
| 0x38 | 4 | float | Fire Rate Level 1 |
| 0x3C | 4 | float | Fire Rate Level 2 |
| 0x40 | 4 | float | Fire Rate Level 3 |
| 0x44 | 4 | float | Unknown 1 |
| 0x48 | 4 | float | Unknown 2 |
| 0x4C | 4 | float | Unknown 3 |
| 0x50 | 4 | float | Effective Range |
| 0x54 | 4 | int | Type ID |
| 0x58 | 4 | int | Unknown 4 |
| 0x5C | 4 | int | Unknown 5 |
| 0x60 | 1 | byte | Unknown 6 |
| 0x64 | 4 | int | Unknown 7 |
| 0x68 | 4 | int | Unknown 8 |
| 0x6C | 4 | int | Unknown 9 |
| 0x70 | 4 | int | Unknown 10 |

---

## Key Addresses

| Description | Address |
|-------------|---------|
| Weapon Init Function | 0x00585480 |
| CMultigun Constructor | 0x005843D0 |
| CMultigun vtable | 0x0070BE4C |
| Debug Registration (stubbed) | 0x0067A350 |
| Gun Mode Lookup Table | 0x007BFA38 |
| Current Weapon Index | 0x05B2FAEC |
| CMultigun Pointer (post-mansion) | 0x05B0AAFC |

---

## Notes

1. The ammo pool is always 1000.0. Max shots = 1000 / Ammo Cost.

2. Blood Spray's damage decreases at L2 (13.33 → 8.33) then increases at L3 (→ 10.0). Possible design quirk or bug.

3. Blood Bomb and Blood Flame have identical stats except for range (50 vs 10).

4. Fire rate values for shotgun/explosive weapons are all 1.0, while hitscan weapons (Shot, Stream) scale with level.

5. The orphaned multiplier system at 0x007C48C0-0x007C48F8 is unused in the retail build.
