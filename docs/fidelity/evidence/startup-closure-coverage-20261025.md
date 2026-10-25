# Closure coverage, 2026-10-25

Measured with `scripts/native_coverage.py --spans` (r2 retail listing + engine source scan; PE import table read with `rabin2 -i`).
This replaces hand counters: an earlier count of `0x00402930`'s gap said 146 absent because it trusted the manifest's
`implementation.path`, which points at external candidate files for many records.

## WinMain `0x0048F530..0x0048F912`: direct call closure complete

- 52 call sites, 46 distinct callees
- engine 28, import (IAT) 17, CRT 1, candidate-only 0, absent 0
- **46/46 accounted for**, nothing missing: every game native WinMain calls is transcribed.
- The last game native in that list, `0x00498480 Display_ProbeDirectXVersion`, was linked and paired-run this session
  (docs/fidelity/evidence/00498480-paired-runtime.md), which is also where the two swapped IIDs were caught.

## Startup sequence `0x00402930..0x00402C3C`: the real gap

- 155 call sites, 154 distinct callees
- engine 14 (9.1%), candidate-only 140, absent 0
- Every one of the missing entries has an external candidate in `scripts/GameNatives`; none is a mystery body.

### The 140 remaining callees, in retail call order

| # | retail VA | behaviour-derived name from the candidate file |
| --- | --- | --- |
| 1 | `0x00402F50` | 00402F50-Config_YmbeAfp |
| 2 | `0x00402FF0` | 00402FF0-Config_FillPlayerProfileListBox_ |
| 3 | `0x00406450` | HumanAIConfig_Init |
| 4 | `0x00407290` | WeaponConfig_Init |
| 5 | `0x0040FE30` | 0040FE30-HumanPlayer_Init |
| 6 | `0x00413DF0` | 00413DF0-WEAPONTYPE_GUN |
| 7 | `0x00415070` | IntroScreen_Init |
| 8 | `0x004170A0` | 004170A0-MenuManager_Init |
| 9 | `0x004190E0` | 004190E0-MenuLoader_Init |
| 10 | `0x004199F0` | 004199F0-DialogWindow_Init |
| 11 | `0x0041A1E0` | 0041A1E0-TypeWriterBox_Init |
| 12 | `0x0041AAF0` | 0041AAF0-TextBox_Init |
| 13 | `0x0041B310` | 0041B310-PictureBox_Init |
| 14 | `0x0041B8E0` | 0041B8E0-ScrollListBox_Init |
| 15 | `0x0041C700` | 0041C700-AsciiBox_Init |
| 16 | `0x0041D080` | 0041D080-InputBox_Init |
| 17 | `0x0041DC50` | 0041DC50-ToggleBox_Init |
| 18 | `0x0041E690` | 0041E690-ListBox_Init |
| 19 | `0x0041F340` | 0041F340-SlideBar_Init |
| 20 | `0x00420300` | MenuTextConditional_Init |
| 21 | `0x00420CC0` | 00420CC0-MenuText_Init |
| 22 | `0x00421430` | MenuButton_Init |
| 23 | `0x00421750` | MenuScreen_Init |
| 24 | `0x004223D0` | 004223D0-MenuFrame_Init |
| 25 | `0x00422B40` | 00422B40-ModelViewBox_Init |
| 26 | `0x004230A0` | 004230A0-ControlsMenu_Init |
| 27 | `0x00423FC0` | 00423FC0-MenuWindow_Init |
| 28 | `0x00424E80` | 00424E80-ConditionalSound_Init |
| 29 | `0x00425C80` | 00425C80-CarAI_Init |
| 30 | `0x00426630` | CarDoor_Init |
| 31 | `0x00426D90` | 00426D90-HeliWheel_Init |
| 32 | `0x004272A0` | 004272A0-HeliDoor_Init |
| 33 | `0x00427830` | RocketPod_Init |
| 34 | `0x00427D90` | 00427D90-HitZone_Init |
| 35 | `0x00427FF0` | 00427FF0-Missile_Init |
| 36 | `0x0042ACC0` | 0042ACC0-Train_Init |
| 37 | `0x0042BEB0` | 0042BEB0-Afterburner_Init |
| 38 | `0x0042D2B0` | 0042D2B0-Rotor_Init |
| 39 | `0x0042DCF0` | 0042DCF0-Car_Init |
| 40 | `0x0042FF00` | 0042FF00-Wheel_Init |
| 41 | `0x004310F0` | 004310F0-Heli_Init |
| 42 | `0x004330F0` | 004330F0-Plane_Init |
| 43 | `0x004351E0` | 004351E0-CockpitShield_Init |
| 44 | `0x00435870` | Gear_Init |
| 45 | `0x00435D90` | 00435D90-Hatch_Init |
| 46 | `0x004361D0` | 004361D0-Rudder_Init |
| 47 | `0x00436780` | RainEffect_Init |
| 48 | `0x00436B10` | 00436B10-EnvironmentSound_Init |
| 49 | `0x00437510` | Smoke_Init |
| 50 | `0x00438390` | GunShotEffect_Init |
| 51 | `0x004388B0` | 004388B0-PlaceExplosiveTBA_Init |
| 52 | `0x00438DF0` | 00438DF0-RotatingObject_Init |
| 53 | `0x004397C0` | 004397C0-AlarmLight_Init |
| 54 | `0x0043A840` | 0043A840-MineField_Init |
| 55 | `0x0043AD80` | 0043AD80-Radio_Init |
| 56 | `0x0043B2F0` | 0043B2F0-Generator_Init |
| 57 | `0x0043B850` | 0043B850-SimpointObjGenerator_Init |
| 58 | `0x0043BFD0` | 0043BFD0-ExplodeMagicObj_Init |
| 59 | `0x0043C6D0` | 0043C6D0-ElevatorFloorIndicator_Init |
| 60 | `0x0043CC70` | 0043CC70-Elevator_Init |
| 61 | `0x0043E4F0` | 0043E4F0-GenericPhysicsMagicObj_Init |
| 62 | `0x0043E8D0` | 0043E8D0-GenericPhysicsObj_Init |
| 63 | `0x0043EFF0` | 0043EFF0-Wire_Init |
| 64 | `0x0043F900` | 0043F900-Fence_Init |
| 65 | `0x004403E0` | 004403E0-Ladder_Init |
| 66 | `0x00440710` | AlarmControl_Init |
| 67 | `0x00440C20` | 00440C20-SCameraControl_Init |
| 68 | `0x00440FD0` | 00440FD0-SCamera_Init |
| 69 | `0x00443680` | Terminal_Init |
| 70 | `0x00443ED0` | 00443ED0-Cabinet_Init |
| 71 | `0x00444BF0` | 00444BF0-Switch_Init |
| 72 | `0x004464C0` | 004464C0-Door_Init |
| 73 | `0x00447DB0` | 00447DB0-GenericTBA_Init |
| 74 | `0x004485E0` | 004485E0-Building_Init |
| 75 | `0x00448D30` | 00448D30-RadarDish_Init |
| 76 | `0x00449360` | Siren_Init |
| 77 | `0x00449970` | Glass_Init |
| 78 | `0x00449D30` | 00449D30-GlassShatter_Init |
| 79 | `0x0044C130` | 0044C130-AmbientArea_Init |
| 80 | `0x0044C8D0` | 0044C8D0-HumanAI_Init |
| 81 | `0x004533B0` | 004533B0-PatrolPathCommand_Init |
| 82 | `0x00453550` | PatrolPath_Init |
| 83 | `0x00454970` | HumanSoldierFemale_Init |
| 84 | `0x00456AD0` | 00456AD0-HumanSoldierRPG_Init |
| 85 | `0x00459880` | HumanSoldierPistol_Init |
| 86 | `0x0045CAB0` | 0045CAB0-HumanSoldier_Init |
| 87 | `0x0045DB70` | 0045DB70-HumanPlayerInput_Init |
| 88 | `0x0045E450` | 0045E450-Human_Init |
| 89 | `0x00464260` | HumanView_Init |
| 90 | `0x00465A10` | 00465A10-HumanAttach_Init |
| 91 | `0x00465BC0` | 00465BC0-DeathZone_Init |
| 92 | `0x004663D0` | 004663D0-ComputerStartup_Init |
| 93 | `0x00466E40` | 00466E40-DefineComputerLimit_Init |
| 94 | `0x00466F70` | 00466F70-DefineComputerObjective_Init |
| 95 | `0x00467700` | ComputerObject_Init |
| 96 | `0x004682F0` | 004682F0-ComputerHilight_Init |
| 97 | `0x004688B0` | 004688B0-ComputerLog_Init |
| 98 | `0x00468E20` | 00468E20-ComputerObjectives_Init |
| 99 | `0x004696B0` | 004696B0-ComputerMap_Init |
| 100 | `0x0046AB60` | 0046AB60-ComputerMenu_Init |
| 101 | `0x0046B510` | 0046B510-ComputerButtonItem_Init |
| 102 | `0x0046B7F0` | 0046B7F0-ComputerTextItem_Init |
| 103 | `0x0046C0C0` | 0046C0C0-GenericPickup_Init |
| 104 | `0x0046CB70` | 0046CB70-GunPickup_Init |
| 105 | `0x0046D420` | 0046D420-AmmoPickup_Init |
| 106 | `0x0046DB40` | 0046DB40-AddAmmo_Init |
| 107 | `0x0046E370` | 0046E370-AIStationaryGunHolder_Init |
| 108 | `0x0046F2B0` | 0046F2B0-AIStationaryGun_Init |
| 109 | `0x00470410` | Medipack_Init |
| 110 | `0x00470610` | Binocular_Init |
| 111 | `0x00470FA0` | 00470FA0-BinocularOverlay_Init |
| 112 | `0x00472670` | GunCABarrel_Init |
| 113 | `0x00472A70` | 00472A70-GunSPAS12_Init |
| 114 | `0x00472D00` | 00472D00-GunDRAGUNOV_Init |
| 115 | `0x004733F0` | 004733F0-GunM16A2_Init |
| 116 | `0x004739D0` | 004739D0-GunMP5SD_Init |
| 117 | `0x00473ED0` | 00473ED0-StationaryGun_Init |
| 118 | `0x00475380` | ProximityMine_Init |
| 119 | `0x00475AC0` | 00475AC0-Mine_Init |
| 120 | `0x00476110` | M203_Init |
| 121 | `0x00476730` | Grenade_Init |
| 122 | `0x00477220` | GunClip_Init |
| 123 | `0x00477E30` | 00477E30-Gun_Init |
| 124 | `0x0047CAB0` | 0047CAB0-Ammo_TypeOpen |
| 125 | `0x0047CF20` | 0047CF20-Weapon_AmmoId |
| 126 | `0x0047CF70` | 0047CF70-BulletHole_Init |
| 127 | `0x0047E700` | 0047E700-FlashBang_Init |
| 128 | `0x004801B0` | 004801B0-Explosion0_Init |
| 129 | `0x004813E0` | 004813E0-ExplodeObject_Init |
| 130 | `0x004825C0` | 004825C0-HumanCamera_Init |
| 131 | `0x00484F30` | 00484F30-FX_Definemission |
| 132 | `0x004851B0` | 004851B0-GameScreen_Init |
| 133 | `0x00485330` | StatusMessage_Init |
| 134 | `0x00485A40` | 00485A40-StatusMessageScreen_Init |
| 135 | `0x00486900` | StatusScreen_Init |
| 136 | `0x004872C0` | 004872C0-ConditionalContainer_Init |
| 137 | `0x00487BE0` | 00487BE0-GuardGenerator_Init |
| 138 | `0x00488620` | SubLevel_Init |
| 139 | `0x00488800` | EditVariable_Init |
| 140 | `0x00488BB0` | 00488BB0-LevelFlow_Init |

## Why this matters for the MVP

These are almost all QTask class registrars (`*_Init`: Door, Gun, Human, Car, Heli, menus, pickups, AI, effects, HUD screens).
They are the startup sequence that registers every game class the level script can instantiate, which is why the guard at
`0x00402930` blocks the playable path: with 8.4% of them transcribed the candidate aborts there rather than reaching Level 1.
They are also uniform in shape (TaskType_Register + handler links through `0x00401400`/`0x00401530`), which is why the
startup-chain lane can take them a few at a time with the `add esp` accounting check.
