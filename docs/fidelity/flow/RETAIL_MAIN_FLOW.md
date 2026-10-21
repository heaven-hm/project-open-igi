# Retail igi.exe call flow (transcription, not implementation)

Source: radare2 (aa) on ~/Downloads/IGI/igi.exe, 2026-10-21. Order is call order inside each function. Names are not established; addresses only. angr CFGFast cross-check and Binary Ninja confirmation still pending.

## Level 0: entry
- entry 0x4A511B: CRT startup (GetVersion, GetCommandLine, GetStartupInfo, GetModuleHandle) then call main

## Level 1: main 0x48F544
1. CreateMutex (single instance), CoInitialize, LoadIcon, LoadCursor, RegisterClass
2. small init calls in order: 0x48F0C0, 0x48F0D0, 0x48F0F0, 0x48F100, 0x48F140, 0x48F120, 0x48F1A0, 0x48F1C0, 0x48F1E0, 0x48F240, 0x48F260, 0x48F280, 0x48F290, 0x48F2A0, 0x48F210, 0x48F2B0, 0x48F300, 0x48F200
3. 0x48F360 x4, then 0x498480, then CreateWindowEx
4. game init: 0x401000 (once)
5. per frame: 0x498460, 0x48F170, 0x401050, with PeekMessage and Sleep between

## 0x401000 (game init), call order
0x48F100 (SetWindowTextA wrapper) -> 0x48F330 -> 0x402930 (big init, below) -> 0x402800 -> 0x48F220 -> 0x4028A0 (branch, two ret paths)
- 0x402800 -> 0x4012A0 (lookup 0x4012F0, calls 0x4AF8F0 on result, then dispatch via table at 0xA96AE0 indexed by word [obj+0x1C])

## 0x401050 (per-frame), call order
0x4A5000 -> 0x402880 -> (second) 0x402880 -> indirect call via esi -> ret; alternate path: 0x48F480 -> ret

## 0x402930 (init sequence, 791 bytes), call order
```
0x508F80 0x489560 0x407D60 0x488BB0 0x402FF0 0x415530 0x4C1780 0x401FC0 0x488800 0x488620 0x487BE0 0x4872C0 0x
415070 0x486900 0x485A40 0x485330 0x4851B0 0x484F30 0x4825C0 0x4813E0 0x4801B0 0x47E700 0x47CF70 0x47CF20 0x47
CAB0 0x413DF0 0x407290 0x47B4B0 0x477E30 0x477220 0x476730 0x476110 0x475AC0 0x475380 0x473ED0 0x4739D0 0x4733
F0 0x472D00 0x472A70 0x472670 0x470FA0 0x470610 0x470410 0x46F2B0 0x46E370 0x46DF10 0x46DD00 0x46DB40 0x46D420
 0x46CB70 0x46C0C0 0x46BA60 0x46B7F0 0x46B510 0x46AB60 0x4696B0 0x468E20 0x4688B0 0x4682F0 0x467700 0x466F70 0
x466E40 0x4663D0 0x465BC0 0x465A10 0x464260 0x45E450 0x40FE30 0x45DB70 0x45CAB0 0x459880 0x456AD0 0x454970 0x4
53550 0x4533B0 0x44C8D0 0x406450 0x44C130 0x449D30 0x449970 0x449360 0x448D30 0x4485E0 0x447DB0 0x4464C0 0x444
BF0 0x443ED0 0x443680 0x440FD0 0x440C20 0x440710 0x4403E0 0x43F900 0x43EFF0 0x43E8D0 0x43E4F0 0x43CC70 0x43C6D
0 0x43BFD0 0x43B850 0x43B2F0 0x43AD80 0x43A840 0x4397C0 0x438DF0 0x4388B0 0x438390 0x437510 0x436B10 0x436780 
0x4361D0 0x435D90 0x435870 0x4351E0 0x4330F0 0x4C1780 0x4310F0 0x42FF00 0x42FE40 0x42DCF0 0x42DAC0 0x42D2B0 0x
42BEB0 0x42ACC0 0x427FF0 0x427D90 0x427830 0x4272A0 0x426D90 0x426630 0x425C80 0x424E80 0x4170A0 0x424B90 0x42
3FC0 0x4230A0 0x422B40 0x4223D0 0x421750 0x421430 0x420CC0 0x420300 0x41F340 0x41E690 0x41DC50 0x41D080 0x41C7
00 0x41B8E0 0x41B310 0x41AAF0 0x41A1E0 0x4199F0 0x419760 0x4190E0 0x402F50 0x4CEF30 0x4C1780
```
Note: 0x402930 looks like a table of per-class registration calls (descending addresses). Next: trace what each registers, then Level_Load from 0x401050 path.

## Deferred step: single-instance mutex (main 0x48F544 step 1)
Retail order is unchanged: CreateMutex (IGIMUTEX, FX_Igimutex 0x0048F530, "IGI is already running") stays step 1 in the retail flow above.
In the open-igi build the mutex is deferred: the code in scripts/GameNatives/fx/0048F530-FX_Igimutex.cpp is commented out behind IGI_ENABLE_SINGLE_INSTANCE_MUTEX (not deleted), so retail igi.exe and open-igi.exe can run side by side under the same Frida probes. Restore later by defining IGI_ENABLE_SINGLE_INSTANCE_MUTEX. Keep it undefined for all testing.

## 0x402930 registrations (r2, first string literal in each callee, call order)
Method: pd 40 at each callee, first string literal. Heuristic, so a few callees that register through a helper show "-" or a nearby literal. Names are what the callee registers, not verified native names. Callee addresses are in the call list above.
- engine/AI/misc: 0x508F80 AIGraph; 0x489560 (no literal); 0x407D60 SetActiveMaterial; 0x488BB0 306_01_1; 0x402FF0 GOStart; 0x415530 Level (also registers class Game, see below); 0x4C1780 (QTaskList is full text lives here, returns immediately); 0x401FC0 Flow
- level/script classes: 0x488800 EditVariable; 0x488620 SubLevel; 0x487BE0 GuardGenerator; 0x4872C0 ConditionalContainer; 0x415070 IntroScreen; 0x486900 StatusScreen; 0x485A40 StatusMessageScreen; 0x485330 StatusMessage; 0x4851B0 GameScreen; 0x4825C0 DefineMission / HumanCamera region; 0x4813E0 ExplodeObject; 0x4801B0 Explosion0 / FlashBang; 0x47E700 BulletHole
- weapons and ammo (out of MVP scope): 0x47CF70 AMMO_ID_.*; 0x47CF20 DefineAmmoType; 0x47CAB0 DefineWeaponType; 0x413DF0 WeaponConfigContainer; 0x407290 GunShot; 0x47B4B0 Gun; 0x477E30 GunFlame; 0x477220 Grenade; 0x476730 M203; 0x476110 Mine; 0x475AC0 ProximityMine; 0x475380 StationaryGun; 0x473ED0 GunMP5SD; 0x4739D0 GunM16A2; 0x4733F0 GunDRAGUNOV; 0x472D00 GunSPAS12; 0x472A70 GunCABarrel; 0x472670 BinocularOverlay; 0x470FA0 Binocular; 0x470610 Medipack; 0x470410 AIStationaryGun; 0x46F2B0 AIStationaryGunHolder; 0x46E370 KnifeStab; 0x46DF10 Gunx2; 0x46DD00 AddAmmo
- pickups and computer: 0x46DB40 AmmoPickup; 0x46D420 GunPickup; 0x46CB70 GenericPickup; 0x46C0C0 Computer; 0x46BA60 ComputerTextItem; 0x46B7F0 ComputerButtonItem; 0x46B510 ComputerMenu; 0x46AB60 ComputerMap; 0x4696B0 ComputerObjectives; 0x468E20 ComputerLog; 0x4688B0 ComputerHilight; 0x4682F0 ComputerObject; 0x467700 DefineComputerObjective; 0x466F70 DefineComputerLimit; 0x466E40 ComputerStartup; 0x4663D0 DeathZone
- humans (MVP humanplayer): 0x465BC0 HumanAttach; 0x465A10 HumanView; 0x464260 Human; 0x45E450 HumanPlayer; 0x40FE30 HumanPlayerInput; 0x45DB70 HumanSoldier; 0x45CAB0 HumanSoldierPistol; 0x459880 (literal not text); 0x456AD0 HumanSoldierFemale; 0x454970 PatrolPath / PatrolPathCommand; 0x453550 and 0x4533B0 HumanAI; 0x44C8D0 HumanAIConfig; 0x406450 johan; 0x44C130 (no literal)
- world objects: 0x449D30 Glass; 0x449970 Siren; 0x449360 RadarDish; 0x448D30 Building; 0x4485E0 GenericTBA; 0x447DB0 Door; 0x4464C0 Switch; 0x444BF0 Cabinet; 0x443ED0 Terminal; 0x443680 SCamera; 0x440FD0 SCameraControl; 0x440C20 AlarmControl; 0x440710 Ladder; 0x4403E0 (no literal); 0x43F900 Fence; 0x43EFF0 Wire; 0x43E8D0 GenericPhysicsObj; 0x43E4F0 GenericPhysicsMagicObj; 0x43CC70 Elevator; 0x43C6D0 ElevatorFloorIndicator; 0x43BFD0 ExplodeMagicObj; 0x43B850 SimpointObjGenerator; 0x43B2F0 Generator; 0x43AD80 Radio; 0x43A840 MineField; 0x4397C0 AlarmLight; 0x438DF0 RotatingObject; 0x4388B0 PlaceExplosiveTBA; 0x438390 GunShotEffect; 0x437510 1729_01_1; 0x436B10 EnvironmentSound; 0x436780 RainEffect
- vehicles: 0x4361D0 Rudder; 0x435D90 Hatch; 0x435870 Gear; 0x4351E0 CockpitShield; 0x4330F0 Plane; 0x4310F0 Heli; 0x42FF00 Wheel; 0x42DCF0 Car; 0x42D2B0 Rotor; 0x42BEB0 Afterburner; 0x42ACC0 Train; 0x427FF0 Missile; 0x427D90 HitZone; 0x427830 RocketPod; 0x4272A0 HeliDoor; 0x426D90 HeliWheel; 0x426630 CarDoor; 0x425C80 CarAI; 0x424E80 ConditionalSound
- menu system: 0x4170A0 LOCAL:menusystem\ ; 0x424B90 Cursor; 0x423FC0 MenuWindow; 0x4230A0 ControlsMenu; 0x422B40 ModelViewBox; 0x4223D0 MenuFrame; 0x421750 MenuScreen; 0x421430 MenuButton; 0x420CC0 MenuText; 0x420300 MenuTextConditional; 0x41F340 SlideBar; 0x41E690 ListBox; 0x41DC50 ToggleBox; 0x41D080 InputBox; 0x41C700 AsciiBox; 0x41B8E0 ScrollListBox; 0x41B310 PictureBox; 0x41AAF0 TextBox; 0x41A1E0 TypeWriterBox; 0x4199F0 DialogWindow; 0x419760 BackgroundFX; 0x4190E0 MenuLoader
- tail: 0x402F50 (".\" path string); 0x4CEF30 (stores [0xA4EDEC] into [0xA4EDF0]); 0x4C1780 (ret) again

## Game class registration (0x415530, called from 0x402930)
0x415530 -> 0x48F360("Level", 0x415B10) -> 0x401900("Game", size 0x158, class id kept in word [0x57B1A2]) -> init 0x4017C0 loop for byte flags 0x539540..0x53954E -> three handler registrations into the table at 0xA96AE0:
- 0x401400(classId, phase 0, handler 0x415B30)  mission start / level load
- 0x401530(classId, phase 1, handler 0x416260)  per-frame update
- 0x401530(classId, phase 2, handler 0x416550)  shutdown
Table index is classId + phase*384 (dword entries), so phase 1 starts at 0xA970E0, which is the table 0x401050 reads. After the registrations 0x415530 allocates buffers with 0x4C1800 (sizes 0x200, 0x40, 0x100, 0x100, 0x10, 0x80 ...).

## Per-frame path 0x401050 down to Level_Load
Per-frame (every iteration of main): 0x401050 -> 0x4A5000 (setjmp, buffer 0x54F8E0) -> if 0: 0x402880 (current game object = [0x567C8C]) -> handler = table 0xA970E0[word [obj+0x1C]] -> call handler(obj) = Game phase 1 handler 0x416260. If setjmp returns non-zero (longjmp from a script error): 0x48F480(0) clears status byte [0x5C8DE8].
Level load is phase 0, not phase 1. It is reached at game init: 0x401000 -> 0x402930 (registers Game phase 0 handler 0x415B30) -> 0x402800 -> 0x4012A0 (lookup 0x4012F0, dispatch via table 0xA96AE0[word [obj+0x1C]]) -> 0x415B30.
0x415B30 (frame 0x280, "The mission ID %d does not exist.") call order: 0x402820(30), 0x4028D0, 0x485170(mission), set "LOCAL:%s/" and "MISSION:" paths (0x4B12F0), store mission ptr [0x57BABC], 0x453270, 0x4D9950, 0x416010, 0x406390, 0x48A440(9), 0x4C5B20(3500) -> [mission+0x20], 0x408390, 0x4C1780, terrain load "%sterrain/terrain" via 0x4C5F00 and 0x4C6240, 0x4B2610, 0x4B25B0, 0x4CEEA0, "LOCAL:%s/" 0x4CFBD0, "LOCAL:common/" 0x4CFC00, 0x4CEFA0, 0x4F0CE0, 0x4B25F0, 0x4B2610, 0x4CEA60 (+ call through table 0xA94E84), "LOCAL:common/sounds" and "MISSION:sounds" 0x4E68D0, "LOCAL:common/sprites/sprites.res" 0x4B5F00 then 0x4161D0, "LOCAL:common/textures/textures.res" 0x4B5F00 then 0x416130, 0x46B070, 0x4868A0, 0x402890, 0x485300, 0x416D40, 0x4F0CB0, 0x4E6060, sound dirs 0x4E6550, 0x416920, 0x4E7D40, 0x401AE0, 0x4E7D70, 0x4D2D30, 0x4061C0, 0x4061E0, 0x4ED5E0, 0x4F80E0, 0x437500, 0x4EDCC0, 0x4F65B0, 0x4F65D0(30), 0x416D40, 0x485320, 0x4F65C0, then Level_Load wrapper 0x4F0E10(path buffer 0x57B568, 30) at 0x415F1A, then 0x416920, 0x4CE830.
0x4F0E10 builds "%s/objects.qsc" (0x4A53B3 sprintf) and calls 0x4F0D40.
Level_Load = 0x4F0D40(script path, arg): frees entries of the table 0xA71890..0xA75890 (0x401B20 each), Task_New via 0x4B8930, loads the script with 0x4B80B0; on failure jumps to the 0x4F0E00 stub which logs "Level_Load(): Couldn't load script: %s" (0x54E068, via 0x4AF7B0) and spins. On success: 0x4F0E50(1,...), 0x4B85B0, 0x4B83D0, 0x4F1A70, 0x401D80, then writes [0xA758A4+0x24/0x28/0x2C/0x30] and returns. A second caller of 0x4F0E10 is at 0x41703E (restart path, with 0x4F0E90 before it).
Order observation for the MVP demos: terrain (0x4C5F00/0x4C6240) comes before textures.res, and both come before objects.qsc via Level_Load.
Status: radare2 only. angr CFGFast and Binary Ninja confirmation of this chain, and the Frida call-order check on retail, are still pending. Class names for 0x41xxxx menu callees and a few "-" entries are not resolved.

## Mutex update (supersedes the deferred-mutex note above)
open-igi keeps the single-instance mutex step in the retail order (main 0x48F544 step 1, FX_Igimutex 0x0048F530) but names it OPENIGIMUTEX_TEST instead of retail IGIMUTEX, so both games run side by side under the same Frida probes. The IGI_ENABLE_SINGLE_INSTANCE_MUTEX switch is gone. TODO: set OPEN_IGI_MUTEX_NAME back to IGIMUTEX after side-by-side testing.
