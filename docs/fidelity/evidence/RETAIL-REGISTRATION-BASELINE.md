# Retail class-registration baseline (level 1)

Source: spawn-suspended retail `igi.exe` (sha256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`), typed probe `5a66146be7e995bc0c3fbf819628ce75886cf5205ff9041c93f29d4953ae0798`; installed before the first instruction. No candidate code, no game-memory writes.

- **153** registrar positions executed before the `0x00402930` tail jump; **279** TaskType_Register calls observed, class ids **0..278 strictly sequential**.
- Static linear disassembly shows 314 `call` instructions to the first `ret`, but at `0x00402C37` the executed path does `jmp 0x004CEF30`; the calls after that point in the linear listing are dead for this level-1 path and were not observed. This is the key order caveat for CHAIN.md-style linear lists.
- `0x4C1780` is a one-byte `ret` and cannot be intercepted; retail registers nothing there. It appears in `unhooked_positions` in the JSON.
- Observed return-site order matches the executed static prefix exactly (0 mismatches).

## CamTask check

Retail `0x00530220` (CamTask) registers class id **45 CamTask** (size 136), observed under registrar **0x508f80** with TaskType_Register callsite `0x530239`.

## Per-registrar ordered class ids

| # | registrar VA | QTask return site | classes | ordered ids (name) |
| --- | --- | --- | --- | --- |
| 1 | `0x508f80` | `0x402935` | 123 | 0 QTask, 1 QObj, 2 DynCubeObj, 3 Hazinglayers, 4 TransparencyList, 5 PointlightDynCubeObj, 6 SpotlightDynCubeObj, 7 Sprite3D, 8 RigidDynCubeObj, 9 MagicObj, 10 BoneDynCubeObj, 11 PolyListDynCubeObj, 12 SplineObjSegment, 13 Shadow, 14 BoneMagicObj, 15 Sector, 16 SectorObject, 17 SectorInteriorObject, 18 MagicObjContainer, 19 Rain, 20 So... |
| 2 | `0x489560` | `0x40293a` | 0 | (none) |
| 3 | `0x407d60` | `0x40293f` | 0 | (none) |
| 4 | `0x488bb0` | `0x402944` | 1 | 123 LevelFlow |
| 5 | `0x402ff0` | `0x402949` | 0 | (none) |
| 6 | `0x415530` | `0x40294e` | 1 | 124 Game |
| 7 | `0x401fc0` | `0x402958` | 1 | 125 Flow |
| 8 | `0x488800` | `0x40295d` | 1 | 126 EditVariable |
| 9 | `0x488620` | `0x402962` | 1 | 127 SubLevel |
| 10 | `0x487be0` | `0x402967` | 1 | 128 GuardGenerator |
| 11 | `0x4872c0` | `0x40296c` | 1 | 129 ConditionalContainer |
| 12 | `0x415070` | `0x402971` | 1 | 130 IntroScreen |
| 13 | `0x486900` | `0x402976` | 1 | 131 StatusScreen |
| 14 | `0x485a40` | `0x40297b` | 1 | 132 StatusMessageScreen |
| 15 | `0x485330` | `0x402980` | 1 | 133 StatusMessage |
| 16 | `0x4851b0` | `0x402985` | 1 | 134 GameScreen |
| 17 | `0x484f30` | `0x40298a` | 0 | (none) |
| 18 | `0x4825c0` | `0x40298f` | 1 | 135 HumanCamera |
| 19 | `0x4813e0` | `0x402994` | 1 | 136 ExplodeObject |
| 20 | `0x4801b0` | `0x402999` | 4 | 137 Explosion0, 138 FireBall, 139 Fragment, 140 ExplosionSmoke |
| 21 | `0x47e700` | `0x40299e` | 1 | 141 FlashBang |
| 22 | `0x47cf70` | `0x4029a3` | 1 | 142 BulletHole |
| 23 | `0x47cf20` | `0x4029a8` | 0 | (none) |
| 24 | `0x47cab0` | `0x4029ad` | 0 | (none) |
| 25 | `0x413df0` | `0x4029b2` | 0 | (none) |
| 26 | `0x407290` | `0x4029b7` | 2 | 143 WeaponConfigContainer, 144 WeaponConfig |
| 27 | `0x47b4b0` | `0x4029bc` | 1 | 145 GunShot |
| 28 | `0x477e30` | `0x4029c1` | 2 | 146 Gun, 147 GunCasing |
| 29 | `0x477220` | `0x4029c6` | 3 | 148 GunFlame, 149 GunClip, 150 GunMovingPart |
| 30 | `0x476730` | `0x4029cb` | 1 | 151 Grenade |
| 31 | `0x476110` | `0x4029d0` | 1 | 152 M203 |
| 32 | `0x475ac0` | `0x4029d5` | 1 | 153 Mine |
| 33 | `0x475380` | `0x4029da` | 1 | 154 ProximityMine |
| 34 | `0x473ed0` | `0x4029df` | 1 | 155 StationaryGun |
| 35 | `0x4739d0` | `0x4029e4` | 1 | 156 GunMP5SD |
| 36 | `0x4733f0` | `0x4029e9` | 1 | 157 GunM16A2 |
| 37 | `0x472d00` | `0x4029ee` | 1 | 158 GunDRAGUNOV |
| 38 | `0x472a70` | `0x4029f3` | 1 | 159 GunSPAS12 |
| 39 | `0x472670` | `0x4029f8` | 2 | 160 GunCABarrel, 161 GunCAClip |
| 40 | `0x470fa0` | `0x4029fd` | 2 | 162 BinocularOverlay, 163 BinocularOverlayHolder |
| 41 | `0x470610` | `0x402a02` | 1 | 164 Binocular |
| 42 | `0x470410` | `0x402a07` | 1 | 165 Medipack |
| 43 | `0x46f2b0` | `0x402a0c` | 1 | 166 AIStationaryGun |
| 44 | `0x46e370` | `0x402a11` | 1 | 167 AIStationaryGunHolder |
| 45 | `0x46df10` | `0x402a16` | 1 | 168 KnifeStab |
| 46 | `0x46dd00` | `0x402a1b` | 1 | 169 Gunx2 |
| 47 | `0x46db40` | `0x402a20` | 1 | 170 AddAmmo |
| 48 | `0x46d420` | `0x402a25` | 1 | 171 AmmoPickup |
| 49 | `0x46cb70` | `0x402a2a` | 1 | 172 GunPickup |
| 50 | `0x46c0c0` | `0x402a2f` | 1 | 173 GenericPickup |
| 51 | `0x46ba60` | `0x402a34` | 1 | 174 Computer |
| 52 | `0x46b7f0` | `0x402a39` | 1 | 175 ComputerTextItem |
| 53 | `0x46b510` | `0x402a3e` | 1 | 176 ComputerButtonItem |
| 54 | `0x46ab60` | `0x402a43` | 1 | 177 ComputerMenu |
| 55 | `0x4696b0` | `0x402a48` | 1 | 178 ComputerMap |
| 56 | `0x468e20` | `0x402a4d` | 1 | 179 ComputerObjectives |
| 57 | `0x4688b0` | `0x402a52` | 1 | 180 ComputerLog |
| 58 | `0x4682f0` | `0x402a57` | 1 | 181 ComputerHilight |
| 59 | `0x467700` | `0x402a5c` | 1 | 182 ComputerObject |
| 60 | `0x466f70` | `0x402a61` | 1 | 183 DefineComputerObjective |
| 61 | `0x466e40` | `0x402a66` | 1 | 184 DefineComputerLimit |
| 62 | `0x4663d0` | `0x402a6b` | 1 | 185 ComputerStartup |
| 63 | `0x465bc0` | `0x402a70` | 1 | 186 DeathZone |
| 64 | `0x465a10` | `0x402a75` | 1 | 187 HumanAttach |
| 65 | `0x464260` | `0x402a7a` | 1 | 188 HumanView |
| 66 | `0x45e450` | `0x402a7f` | 1 | 189 Human |
| 67 | `0x40fe30` | `0x402a84` | 1 | 190 HumanPlayer |
| 68 | `0x45db70` | `0x402a89` | 1 | 191 HumanPlayerInput |
| 69 | `0x45cab0` | `0x402a8e` | 1 | 192 HumanSoldier |
| 70 | `0x459880` | `0x402a93` | 1 | 193 HumanSoldierPistol |
| 71 | `0x456ad0` | `0x402a98` | 1 | 194 HumanSoldierRPG |
| 72 | `0x454970` | `0x402a9d` | 1 | 195 HumanSoldierFemale |
| 73 | `0x453550` | `0x402aa2` | 1 | 196 PatrolPath |
| 74 | `0x4533b0` | `0x402aa7` | 1 | 197 PatrolPathCommand |
| 75 | `0x44c8d0` | `0x402aac` | 1 | 198 HumanAI |
| 76 | `0x406450` | `0x402ab1` | 2 | 199 HumanAIConfig, 200 HumanAIConfigItem |
| 77 | `0x44c130` | `0x402ab6` | 1 | 201 AmbientArea |
| 78 | `0x449d30` | `0x402abb` | 1 | 202 GlassShatter |
| 79 | `0x449970` | `0x402ac0` | 1 | 203 Glass |
| 80 | `0x449360` | `0x402ac5` | 1 | 204 Siren |
| 81 | `0x448d30` | `0x402aca` | 1 | 205 RadarDish |
| 82 | `0x4485e0` | `0x402acf` | 1 | 206 Building |
| 83 | `0x447db0` | `0x402ad4` | 1 | 207 GenericTBA |
| 84 | `0x4464c0` | `0x402ad9` | 1 | 208 Door |
| 85 | `0x444bf0` | `0x402ade` | 1 | 209 Switch |
| 86 | `0x443ed0` | `0x402ae3` | 2 | 210 Cabinet, 211 Drawer |
| 87 | `0x443680` | `0x402ae8` | 1 | 212 Terminal |
| 88 | `0x440fd0` | `0x402aed` | 3 | 213 SCamera, 214 SCameraLens, 215 SCameraLensViewCone |
| 89 | `0x440c20` | `0x402af2` | 1 | 216 SCameraControl |
| 90 | `0x440710` | `0x402af7` | 1 | 217 AlarmControl |
| 91 | `0x4403e0` | `0x402afc` | 1 | 218 Ladder |
| 92 | `0x43f900` | `0x402b01` | 1 | 219 Fence |
| 93 | `0x43eff0` | `0x402b06` | 1 | 220 Wire |
| 94 | `0x43e8d0` | `0x402b0b` | 1 | 221 GenericPhysicsObj |
| 95 | `0x43e4f0` | `0x402b10` | 1 | 222 GenericPhysicsMagicObj |
| 96 | `0x43cc70` | `0x402b15` | 1 | 223 Elevator |
| 97 | `0x43c6d0` | `0x402b1a` | 1 | 224 ElevatorFloorIndicator |
| 98 | `0x43bfd0` | `0x402b1f` | 1 | 225 ExplodeMagicObj |
| 99 | `0x43b850` | `0x402b24` | 1 | 226 SimpointObjGenerator |
| 100 | `0x43b2f0` | `0x402b29` | 1 | 227 Generator |
| 101 | `0x43ad80` | `0x402b2e` | 1 | 228 Radio |
| 102 | `0x43a840` | `0x402b33` | 1 | 229 MineField |
| 103 | `0x4397c0` | `0x402b38` | 2 | 230 AlarmLight, 231 AlarmLightReflector |
| 104 | `0x438df0` | `0x402b3d` | 1 | 232 RotatingObject |
| 105 | `0x4388b0` | `0x402b42` | 1 | 233 PlaceExplosiveTBA |
| 106 | `0x438390` | `0x402b47` | 1 | 234 GunShotEffect |
| 107 | `0x437510` | `0x402b4c` | 1 | 235 Smoke |
| 108 | `0x436b10` | `0x402b51` | 1 | 236 EnvironmentSound |
| 109 | `0x436780` | `0x402b56` | 1 | 237 RainEffect |
| 110 | `0x4361d0` | `0x402b5b` | 1 | 238 Rudder |
| 111 | `0x435d90` | `0x402b60` | 1 | 239 Hatch |
| 112 | `0x435870` | `0x402b65` | 1 | 240 Gear |
| 113 | `0x4351e0` | `0x402b6a` | 1 | 241 CockpitShield |
| 114 | `0x4330f0` | `0x402b6f` | 1 | 242 Plane |
| 115 | `0x4310f0` | `0x402b79` | 1 | 243 Heli |
| 116 | `0x42ff00` | `0x402b7e` | 1 | 244 Wheel |
| 117 | `0x42fe40` | `0x402b83` | 0 | (none) |
| 118 | `0x42dcf0` | `0x402b88` | 1 | 245 Car |
| 119 | `0x42dac0` | `0x402b8d` | 0 | (none) |
| 120 | `0x42d2b0` | `0x402b92` | 1 | 246 Rotor |
| 121 | `0x42beb0` | `0x402b97` | 1 | 247 Afterburner |
| 122 | `0x42acc0` | `0x402b9c` | 1 | 248 Train |
| 123 | `0x427ff0` | `0x402ba1` | 1 | 249 Missile |
| 124 | `0x427d90` | `0x402ba6` | 1 | 250 HitZone |
| 125 | `0x427830` | `0x402bab` | 1 | 251 RocketPod |
| 126 | `0x4272a0` | `0x402bb0` | 1 | 252 HeliDoor |
| 127 | `0x426d90` | `0x402bb5` | 1 | 253 HeliWheel |
| 128 | `0x426630` | `0x402bba` | 1 | 254 CarDoor |
| 129 | `0x425c80` | `0x402bbf` | 1 | 255 CarAI |
| 130 | `0x424e80` | `0x402bc4` | 1 | 256 ConditionalSound |
| 131 | `0x4170a0` | `0x402bc9` | 1 | 257 MenuManager |
| 132 | `0x424b90` | `0x402bce` | 1 | 258 Cursor |
| 133 | `0x423fc0` | `0x402bd3` | 1 | 259 MenuWindow |
| 134 | `0x4230a0` | `0x402bd8` | 1 | 260 ControlsMenu |
| 135 | `0x422b40` | `0x402bdd` | 1 | 261 ModelViewBox |
| 136 | `0x4223d0` | `0x402be2` | 1 | 262 MenuFrame |
| 137 | `0x421750` | `0x402be7` | 1 | 263 MenuScreen |
| 138 | `0x421430` | `0x402bec` | 1 | 264 MenuButton |
| 139 | `0x420cc0` | `0x402bf1` | 1 | 265 MenuText |
| 140 | `0x420300` | `0x402bf6` | 1 | 266 MenuTextConditional |
| 141 | `0x41f340` | `0x402bfb` | 1 | 267 SlideBar |
| 142 | `0x41e690` | `0x402c00` | 1 | 268 ListBox |
| 143 | `0x41dc50` | `0x402c05` | 1 | 269 ToggleBox |
| 144 | `0x41d080` | `0x402c0a` | 1 | 270 InputBox |
| 145 | `0x41c700` | `0x402c0f` | 1 | 271 AsciiBox |
| 146 | `0x41b8e0` | `0x402c14` | 1 | 272 ScrollListBox |
| 147 | `0x41b310` | `0x402c19` | 1 | 273 PictureBox |
| 148 | `0x41aaf0` | `0x402c1e` | 1 | 274 TextBox |
| 149 | `0x41a1e0` | `0x402c23` | 1 | 275 TypeWriterBox |
| 150 | `0x4199f0` | `0x402c28` | 1 | 276 DialogWindow |
| 151 | `0x419760` | `0x402c2d` | 1 | 277 BackgroundFX |
| 152 | `0x4190e0` | `0x402c32` | 1 | 278 MenuLoader |
| 153 | `0x402f50` | `0x402c37` | 0 | (none) |

## Observed vs missed

- Observed: every TaskType_Register call in the executed QTask_RegisterClasses path and all 153 registrar entries, including the final `0x402F50` position.
- Not observed: `0x4C1780` (single-byte ret, no registrations); linear static calls after the `jmp 0x004CEF30` are dead on this path and were not executed.
- No late attach: spawn-suspended injection before first instruction; the old attach-after-launch limitation does not apply.
- Idle observation continues after registration; grouping stops at the tail jump, so post-startup replay calls are excluded.

## Order check

Reference: executed prefix of the `0x00402930` disassembly (call VA + 5). The observed return-site sequence matches that prefix after removing `0x4C1780`; mismatch count 0. There is no `CHAIN.md` in this checkout, so this disassembly-derived prefix is the authoritative static reference. **Caveat:** any static order that simply lists all 314 linear calls is wrong after `0x00402C37`; the retail level-1 path jumps to `0x004CEF30` there.
