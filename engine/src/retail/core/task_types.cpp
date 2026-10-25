#include "dev_log.hpp"
#include "igi/retail/core/task_types.hpp"

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

#include "igi/retail/core/retail_error.hpp"
#include "igi/retail/mem/task_list.hpp"

#include <cstring>
#include <stdexcept>
#include <string>

namespace {


// Port-side replay of the retail task-type registration sequence. The retail
// assigns class ids in registration order, and the per-frame dispatch indexes
// the handler table by class id, so one skipped class shifts every later id.
// The records below are the live capture of that sequence from igi.exe
// (level 1) taken by hooking 0x00401900 with scripts/frida_register_trace.js,
// kept in docs/fidelity/evidence/0x00401900-retail-registration-order.json.
// parent_type is the recorded parent's low 16 bits, the part the retail
// stores at slot+0x08; the recorded upper 16 bits are stack residue and the
// retail stores only the low word. task_list_argument is the captured fourth
// stack slot: the retail creates a class task list through 0x004B0B00 when it
// is non-zero, a path this port does not transcribe yet, so the value is
// passed through and recorded but not acted on. Ids 0 (QTask), 124 (Game) and
// 125 (Flow) stay with QTask_Init, Game_Init and Flow_Init, which register
// them through the same native; the startup replay calls this table for the
// ranges 1..123 and 126..278 around those two inits.
struct RetailRegistrationRecord
{
    std::uint16_t class_id;
    const char* name;
    std::uint32_t object_size;
    std::uint16_t parent_type;
    std::uint32_t task_list_argument;
};

constexpr RetailRegistrationRecord kRetailRegistrationOrder[] = {
    {0, "QTask", 32U, 0x181U, 0x0U}, // id 0: QTask_Init registers this one
    {1, "QObj", 80U, 0x0U, 0x0U}, // id 1
    {2, "DynCubeObj", 104U, 0x1U, 0x0U}, // id 2
    {3, "Hazinglayers", 144U, 0x1U, 0x0U}, // id 3
    {4, "TransparencyList", 128U, 0x2U, 0x0U}, // id 4
    {5, "PointlightDynCubeObj", 128U, 0x2U, 0x0U}, // id 5
    {6, "SpotlightDynCubeObj", 144U, 0x2U, 0x0U}, // id 6
    {7, "Sprite3D", 136U, 0x2U, 0x0U}, // id 7
    {8, "RigidDynCubeObj", 240U, 0x2U, 0x0U}, // id 8
    {9, "MagicObj", 312U, 0x8U, 0x0U}, // id 9
    {10, "BoneDynCubeObj", 216U, 0x2U, 0x0U}, // id 10
    {11, "PolyListDynCubeObj", 184U, 0x2U, 0x0U}, // id 11
    {12, "SplineObjSegment", 1464U, 0x8U, 0x0U}, // id 12
    {13, "Shadow", 292U, 0x0U, 0x0U}, // id 13
    {14, "BoneMagicObj", 304U, 0xAU, 0x0U}, // id 14
    {15, "Sector", 328U, 0x8U, 0x0U}, // id 15
    {16, "SectorObject", 416U, 0xFU, 0x0U}, // id 16
    {17, "SectorInteriorObject", 240U, 0x8U, 0x0U}, // id 17
    {18, "MagicObjContainer", 36U, 0x0U, 0x0U}, // id 18
    {19, "Rain", 22224U, 0x2U, 0x0U}, // id 19
    {20, "SoundSysController", 32U, 0x0U, 0x0U}, // id 20
    {21, "SoundSysDef", 160U, 0x0U, 0x0U}, // id 21
    {22, "SoundSysDefParent", 160U, 0x0U, 0x0U}, // id 22
    {23, "SoundSysMicrophone", 104U, 0x0U, 0x0U}, // id 23
    {24, "SoundDefGroup", 96U, 0x0U, 0x200U}, // id 24
    {25, "SoundDefSound", 304U, 0x0U, 0x200U}, // id 25
    {26, "SoundDefGraph", 392U, 0x0U, 0x200U}, // id 26
    {27, "SoundDefTriggerOnce", 312U, 0x0U, 0x200U}, // id 27
    {28, "Screen", 32U, 0x0U, 0x0U}, // id 28
    {29, "ViewportQTask", 200U, 0x0U, 0x0U}, // id 29
    {30, "TerraDrawTask", 36U, 0x0U, 0x0U}, // id 30
    {31, "CameraControlTask", 44U, 0x0U, 0x0U}, // id 31
    {32, "FreeCamera", 144U, 0x0U, 0x0U}, // id 32
    {33, "DebugText", 32U, 0x0U, 0x0U}, // id 33
    {34, "FadeOutStart", 124U, 0x0U, 0x0U}, // id 34
    {35, "FadeOutEnd", 124U, 0x0U, 0x0U}, // id 35
    {36, "GlowTask", 32U, 0x0U, 0x0U}, // id 36
    {37, "ParticleSystemDynCubeObj", 184U, 0x2U, 0x0U}, // id 37
    {38, "SpriteParticleSystemDynCubeObj", 216U, 0x25U, 0x0U}, // id 38
    {39, "FlashTask", 136U, 0x0U, 0x2U}, // id 39
    {40, "LineFilter", 56U, 0x0U, 0x0U}, // id 40
    {41, "WideScreenFilter", 68U, 0x0U, 0x0U}, // id 41
    {42, "BandFilter", 116U, 0x0U, 0x0U}, // id 42
    {43, "HandiCamFilter", 84U, 0x0U, 0x0U}, // id 43
    {44, "Particle2DDynCubeObjTask", 256U, 0x2U, 0x19U}, // id 44
    {45, "CamTask", 136U, 0x0U, 0x0U}, // id 45
    {46, "InputPort", 32U, 0x0U, 0x0U}, // id 46
    {47, "Sync", 2452U, 0x0U, 0x0U}, // id 47
    {48, "MenuItemTask", 68U, 0x0U, 0x0U}, // id 48
    {49, "MenuItem", 84U, 0x0U, 0x0U}, // id 49
    {50, "PushButton", 124U, 0x31U, 0x0U}, // id 50
    {51, "SimBoneDynCubeObj", 320U, 0xAU, 0x0U}, // id 51
    {52, "Level", 56U, 0x0U, 0x0U}, // id 52
    {53, "Static", 32U, 0x0U, 0x0U}, // id 53
    {54, "Dynamic", 32U, 0x0U, 0x0U}, // id 54
    {55, "Idle", 32U, 0x0U, 0x0U}, // id 55
    {56, "CameraBase", 32U, 0x0U, 0x0U}, // id 56
    {57, "LevelScript", 40U, 0x0U, 0x0U}, // id 57
    {58, "SkyTask", 120U, 0x0U, 0x0U}, // id 58
    {59, "PhysicsObj", 1824U, 0x8U, 0x0U}, // id 59
    {60, "PhysicsMagicObj", 1896U, 0x3BU, 0x0U}, // id 60
    {61, "EditorRigidObj", 32U, 0x0U, 0x0U}, // id 61
    {62, "DiscardTerrain", 344U, 0x3DU, 0x0U}, // id 62
    {63, "DiscardTerrainArea", 120U, 0x8U, 0x0U}, // id 63
    {64, "CubeModifier", 376U, 0x3DU, 0x0U}, // id 64
    {65, "AnimTask", 131424U, 0x3DU, 0x0U}, // id 65
    {66, "TagItem", 292U, 0x0U, 0x0U}, // id 66
    {67, "TagItemInt32", 296U, 0x42U, 0x0U}, // id 67
    {68, "TagItemReal32", 296U, 0x42U, 0x0U}, // id 68
    {69, "TagItemString16", 308U, 0x42U, 0x0U}, // id 69
    {70, "TagItemString256", 548U, 0x42U, 0x0U}, // id 70
    {71, "SplineObj", 104U, 0x0U, 0x0U}, // id 71
    {72, "SplineObjWaypoint", 424U, 0x8U, 0x0U}, // id 72
    {73, "EditRigidObj", 416U, 0x11U, 0x0U}, // id 73
    {74, "TextureModifier", 240U, 0x2U, 0x0U}, // id 74
    {75, "Water", 19528U, 0x8U, 0x0U}, // id 75
    {76, "EditHazinglayer", 136U, 0x3DU, 0x0U}, // id 76
    {77, "LODSettings", 64U, 0x0U, 0x0U}, // id 77
    {78, "ModelLODSettings", 68U, 0x0U, 0x0U}, // id 78
    {79, "ModelLODSettingsContainer", 32U, 0x0U, 0x0U}, // id 79
    {80, "EditRigidObjAttachedToBone", 312U, 0x8U, 0x0U}, // id 80
    {81, "CutScene", 632U, 0x3DU, 0x0U}, // id 81
    {82, "AnimScaleTask", 131432U, 0x41U, 0x0U}, // id 82
    {83, "AnimRigidDynCubeObj", 344U, 0x8U, 0x0U}, // id 83
    {84, "Graph", 128U, 0x3DU, 0x0U}, // id 84
    {85, "GraphNode", 280U, 0x3DU, 0x0U}, // id 85
    {86, "AnimCam", 544U, 0x2DU, 0x0U}, // id 86
    {87, "AreaActivate", 184U, 0x3DU, 0x0U}, // id 87
    {88, "Vehicle", 1864U, 0x3BU, 0x0U}, // id 88
    {89, "GlobalLight", 440U, 0x0U, 0x0U}, // id 89
    {90, "GlobalLightKeyframe", 420U, 0x0U, 0x0U}, // id 90
    {91, "Dirlight", 100U, 0x0U, 0x0U}, // id 91
    {92, "DirlightKeyframe", 84U, 0x0U, 0x0U}, // id 92
    {93, "LightmapInfo", 224U, 0x0U, 0x0U}, // id 93
    {94, "Lensflare", 152U, 0x0U, 0x0U}, // id 94
    {95, "LensflareItem", 64U, 0x0U, 0x0U}, // id 95
    {96, "Container", 32U, 0x0U, 0x0U}, // id 96
    {97, "Origo", 304U, 0x8U, 0x0U}, // id 97
    {98, "NoiseQTask", 304U, 0x3DU, 0x0U}, // id 98
    {99, "MoveRigidObj", 520U, 0x8U, 0x0U}, // id 99
    {100, "SmoothQTask", 256U, 0x3DU, 0x0U}, // id 100
    {101, "MipMapControl", 48U, 0x0U, 0x0U}, // id 101
    {102, "ScreenGrab", 40U, 0x0U, 0x0U}, // id 102
    {103, "FlatSky", 208U, 0x0U, 0x0U}, // id 103
    {104, "FlatSkyLayer", 620U, 0x0U, 0x0U}, // id 104
    {105, "FlatWater", 144U, 0x0U, 0x0U}, // id 105
    {106, "FlatWaterLayer", 620U, 0x0U, 0x0U}, // id 106
    {107, "SoundManager", 68U, 0x0U, 0x0U}, // id 107
    {108, "EditCamera", 232U, 0x3DU, 0x0U}, // id 108
    {109, "LevelTimer", 272U, 0x3DU, 0x0U}, // id 109
    {110, "EditorMagicObj", 320U, 0x9U, 0x0U}, // id 110
    {111, "AnimSound", 4U, 0x0U, 0x0U}, // id 111
    {112, "RainDynCubeObj", 17040U, 0x2U, 0x0U}, // id 112
    {113, "TerrainLightMap", 272U, 0x2U, 0x0U}, // id 113
    {114, "SimpointObjContainer", 100U, 0x0U, 0x20U}, // id 114
    {115, "SimpointObj", 256U, 0x8U, 0x0U}, // id 115
    {116, "SplinePathNodeQTask", 200U, 0x0U, 0x0U}, // id 116
    {117, "SplinePathDynCubeObj", 512U, 0x2U, 0x0U}, // id 117
    {118, "SplinePathGuideQTask", 68U, 0x0U, 0x0U}, // id 118
    {119, "HeightMap", 256U, 0x2U, 0x0U}, // id 119
    {120, "ShadowTask", 48U, 0x0U, 0x0U}, // id 120
    {121, "GFXCapsViewer", 32U, 0x181U, 0x0U}, // id 121
    {122, "AIGraph", 192U, 0x54U, 0x0U}, // id 122
    {123, "LevelFlow", 296U, 0x3DU, 0x0U}, // id 123
    {124, "Game", 344U, 0x0U, 0x0U}, // id 124: Game_Init registers this one
    {125, "Flow", 84U, 0x0U, 0x0U}, // id 125: Flow_Init registers this one
    {126, "EditVariable", 224U, 0x3DU, 0x0U}, // id 126
    {127, "SubLevel", 288U, 0x0U, 0x0U}, // id 127
    {128, "GuardGenerator", 128U, 0x0U, 0x0U}, // id 128
    {129, "ConditionalContainer", 276U, 0x60U, 0x0U}, // id 129
    {130, "IntroScreen", 56U, 0x0U, 0x0U}, // id 130
    {131, "StatusScreen", 1264U, 0x0U, 0x0U}, // id 131
    {132, "StatusMessageScreen", 99392U, 0x0U, 0x0U}, // id 132
    {133, "StatusMessage", 480U, 0x3DU, 0x0U}, // id 133
    {134, "GameScreen", 40U, 0x0U, 0x0U}, // id 134
    {135, "HumanCamera", 296U, 0x0U, 0x0U}, // id 135
    {136, "ExplodeObject", 568U, 0x11U, 0x0U}, // id 136
    {137, "Explosion0", 136U, 0x0U, 0x20U}, // id 137
    {138, "FireBall", 280U, 0x8U, 0x60U}, // id 138
    {139, "Fragment", 280U, 0x73U, 0xA0U}, // id 139
    {140, "ExplosionSmoke", 72U, 0x0U, 0x40U}, // id 140
    {141, "FlashBang", 72U, 0x0U, 0x4U}, // id 141
    {142, "BulletHole", 57672U, 0x0U, 0x0U}, // id 142
    {143, "WeaponConfigContainer", 292U, 0x0U, 0x0U}, // id 143
    {144, "WeaponConfig", 1800U, 0x0U, 0x0U}, // id 144
    {145, "GunShot", 3128U, 0x0U, 0x0U}, // id 145
    {146, "Gun", 432U, 0x8U, 0x0U}, // id 146
    {147, "GunCasing", 296U, 0x8U, 0x32U}, // id 147
    {148, "GunFlame", 320U, 0x9U, 0x0U}, // id 148
    {149, "GunClip", 320U, 0x9U, 0x0U}, // id 149
    {150, "GunMovingPart", 320U, 0x9U, 0x0U}, // id 150
    {151, "Grenade", 1848U, 0x3BU, 0x0U}, // id 151
    {152, "M203", 296U, 0x8U, 0x0U}, // id 152
    {153, "Mine", 592U, 0x88U, 0x0U}, // id 153
    {154, "ProximityMine", 432U, 0x92U, 0x0U}, // id 154
    {155, "StationaryGun", 544U, 0x8U, 0x0U}, // id 155
    {156, "GunMP5SD", 440U, 0x92U, 0x0U}, // id 156
    {157, "GunM16A2", 432U, 0x92U, 0x0U}, // id 157
    {158, "GunDRAGUNOV", 440U, 0x92U, 0x0U}, // id 158
    {159, "GunSPAS12", 432U, 0x92U, 0x0U}, // id 159
    {160, "GunCABarrel", 312U, 0x9U, 0x0U}, // id 160
    {161, "GunCAClip", 312U, 0x9U, 0x0U}, // id 161
    {162, "BinocularOverlay", 11776U, 0x0U, 0x0U}, // id 162
    {163, "BinocularOverlayHolder", 120U, 0x0U, 0x0U}, // id 163
    {164, "Binocular", 456U, 0x92U, 0x0U}, // id 164
    {165, "Medipack", 432U, 0x92U, 0x0U}, // id 165
    {166, "AIStationaryGun", 488U, 0x9U, 0x0U}, // id 166
    {167, "AIStationaryGunHolder", 9888U, 0x8U, 0x0U}, // id 167
    {168, "KnifeStab", 32U, 0x58U, 0x0U}, // id 168
    {169, "Gunx2", 312U, 0x9U, 0x0U}, // id 169
    {170, "AddAmmo", 72U, 0x0U, 0x0U}, // id 170
    {171, "AmmoPickup", 312U, 0x11U, 0x0U}, // id 171
    {172, "GunPickup", 344U, 0x11U, 0x0U}, // id 172
    {173, "GenericPickup", 288U, 0x11U, 0x0U}, // id 173
    {174, "Computer", 408U, 0x8U, 0x0U}, // id 174
    {175, "ComputerTextItem", 144U, 0x31U, 0x0U}, // id 175
    {176, "ComputerButtonItem", 148U, 0x31U, 0x0U}, // id 176
    {177, "ComputerMenu", 60U, 0x0U, 0x0U}, // id 177
    {178, "ComputerMap", 172U, 0x0U, 0x0U}, // id 178
    {179, "ComputerObjectives", 56U, 0x0U, 0x0U}, // id 179
    {180, "ComputerLog", 36U, 0x0U, 0x0U}, // id 180
    {181, "ComputerHilight", 1248U, 0x3DU, 0x0U}, // id 181
    {182, "ComputerObject", 36U, 0x0U, 0x0U}, // id 182
    {183, "DefineComputerObjective", 1336U, 0x8U, 0x0U}, // id 183
    {184, "DefineComputerLimit", 64U, 0x8U, 0x0U}, // id 184
    {185, "ComputerStartup", 144U, 0x0U, 0x0U}, // id 185
    {186, "DeathZone", 312U, 0x9U, 0x0U}, // id 186
    {187, "HumanAttach", 320U, 0x50U, 0x0U}, // id 187
    {188, "HumanView", 808U, 0xAU, 0x0U}, // id 188
    {189, "Human", 3320U, 0xAU, 0x0U}, // id 189
    {190, "HumanPlayer", 3568U, 0xBDU, 0x0U}, // id 190
    {191, "HumanPlayerInput", 484U, 0x0U, 0x0U}, // id 191
    {192, "HumanSoldier", 3408U, 0xBDU, 0x0U}, // id 192
    {193, "HumanSoldierPistol", 3408U, 0xBDU, 0x0U}, // id 193
    {194, "HumanSoldierRPG", 3392U, 0xBDU, 0x0U}, // id 194
    {195, "HumanSoldierFemale", 3408U, 0xBDU, 0x0U}, // id 195
    {196, "PatrolPath", 52U, 0x0U, 0x0U}, // id 196
    {197, "PatrolPathCommand", 40U, 0x0U, 0x0U}, // id 197
    {198, "HumanAI", 16080U, 0x0U, 0x0U}, // id 198
    {199, "HumanAIConfig", 384U, 0x0U, 0x0U}, // id 199
    {200, "HumanAIConfigItem", 680U, 0x0U, 0x0U}, // id 200
    {201, "AmbientArea", 528U, 0x3DU, 0x0U}, // id 201
    {202, "GlassShatter", 176200U, 0x0U, 0x0U}, // id 202
    {203, "Glass", 328U, 0x9U, 0x0U}, // id 203
    {204, "Siren", 728U, 0x88U, 0x0U}, // id 204
    {205, "RadarDish", 384U, 0x9U, 0x0U}, // id 205
    {206, "Building", 496U, 0x10U, 0x0U}, // id 206
    {207, "GenericTBA", 680U, 0x88U, 0x0U}, // id 207
    {208, "Door", 808U, 0x8U, 0x0U}, // id 208
    {209, "Switch", 552U, 0x11U, 0x0U}, // id 209
    {210, "Cabinet", 312U, 0x11U, 0x0U}, // id 210
    {211, "Drawer", 352U, 0x9U, 0x0U}, // id 211
    {212, "Terminal", 672U, 0x88U, 0x0U}, // id 212
    {213, "SCamera", 456U, 0x11U, 0x0U}, // id 213
    {214, "SCameraLens", 9704U, 0x11U, 0x0U}, // id 214
    {215, "SCameraLensViewCone", 312U, 0x9U, 0x0U}, // id 215
    {216, "SCameraControl", 760U, 0xD4U, 0x0U}, // id 216
    {217, "AlarmControl", 848U, 0xD4U, 0x0U}, // id 217
    {218, "Ladder", 320U, 0x9U, 0x0U}, // id 218
    {219, "Fence", 376U, 0x8U, 0x0U}, // id 219
    {220, "Wire", 312U, 0x8U, 0x0U}, // id 220
    {221, "GenericPhysicsObj", 1824U, 0x3BU, 0x0U}, // id 221
    {222, "GenericPhysicsMagicObj", 1896U, 0x3CU, 0x0U}, // id 222
    {223, "Elevator", 1608U, 0x8U, 0x0U}, // id 223
    {224, "ElevatorFloorIndicator", 584U, 0x8U, 0x0U}, // id 224
    {225, "ExplodeMagicObj", 336U, 0x9U, 0x0U}, // id 225
    {226, "SimpointObjGenerator", 344U, 0x8U, 0x0U}, // id 226
    {227, "Generator", 704U, 0x88U, 0x0U}, // id 227
    {228, "Radio", 744U, 0x88U, 0x0U}, // id 228
    {229, "MineField", 208U, 0x3DU, 0x0U}, // id 229
    {230, "AlarmLight", 504U, 0x11U, 0x0U}, // id 230
    {231, "AlarmLightReflector", 320U, 0x9U, 0x0U}, // id 231
    {232, "RotatingObject", 512U, 0x8U, 0x0U}, // id 232
    {233, "PlaceExplosiveTBA", 688U, 0xCFU, 0x0U}, // id 233
    {234, "GunShotEffect", 32U, 0x0U, 0x0U}, // id 234
    {235, "Smoke", 376U, 0x3DU, 0x0U}, // id 235
    {236, "EnvironmentSound", 92U, 0x0U, 0x0U}, // id 236
    {237, "RainEffect", 136U, 0x0U, 0x0U}, // id 237
    {238, "Rudder", 320U, 0x9U, 0x0U}, // id 238
    {239, "Hatch", 320U, 0x9U, 0x0U}, // id 239
    {240, "Gear", 320U, 0x9U, 0x0U}, // id 240
    {241, "CockpitShield", 320U, 0x9U, 0x0U}, // id 241
    {242, "Plane", 2184U, 0x58U, 0x0U}, // id 242
    {243, "Heli", 11520U, 0x58U, 0x0U}, // id 243
    {244, "Wheel", 344U, 0x9U, 0x0U}, // id 244
    {245, "Car", 11992U, 0x58U, 0x0U}, // id 245
    {246, "Rotor", 320U, 0x9U, 0x0U}, // id 246
    {247, "Afterburner", 320U, 0x9U, 0x0U}, // id 247
    {248, "Train", 2272U, 0x58U, 0x0U}, // id 248
    {249, "Missile", 2200U, 0x58U, 0x0U}, // id 249
    {250, "HitZone", 328U, 0x9U, 0x0U}, // id 250
    {251, "RocketPod", 320U, 0x9U, 0x0U}, // id 251
    {252, "HeliDoor", 328U, 0x9U, 0x0U}, // id 252
    {253, "HeliWheel", 320U, 0x9U, 0x0U}, // id 253
    {254, "CarDoor", 328U, 0x9U, 0x0U}, // id 254
    {255, "CarAI", 132U, 0x0U, 0x0U}, // id 255
    {256, "ConditionalSound", 296U, 0x0U, 0x0U}, // id 256
    {257, "MenuManager", 10392U, 0x0U, 0x0U}, // id 257
    {258, "Cursor", 64U, 0x0U, 0x0U}, // id 258
    {259, "MenuWindow", 84U, 0x0U, 0x0U}, // id 259
    {260, "ControlsMenu", 868U, 0x103U, 0x0U}, // id 260
    {261, "ModelViewBox", 200U, 0x103U, 0x0U}, // id 261
    {262, "MenuFrame", 180U, 0x103U, 0x0U}, // id 262
    {263, "MenuScreen", 420U, 0x103U, 0x0U}, // id 263
    {264, "MenuButton", 152U, 0x103U, 0x0U}, // id 264
    {265, "MenuText", 368U, 0x103U, 0x0U}, // id 265
    {266, "MenuTextConditional", 536U, 0x103U, 0x0U}, // id 266
    {267, "SlideBar", 592U, 0x103U, 0x0U}, // id 267
    {268, "ListBox", 3384U, 0x103U, 0x0U}, // id 268
    {269, "ToggleBox", 332U, 0x103U, 0x0U}, // id 269
    {270, "InputBox", 424U, 0x103U, 0x0U}, // id 270
    {271, "AsciiBox", 248U, 0x103U, 0x0U}, // id 271
    {272, "ScrollListBox", 5116U, 0x103U, 0x0U}, // id 272
    {273, "PictureBox", 2556U, 0x103U, 0x0U}, // id 273
    {274, "TextBox", 8772U, 0x103U, 0x0U}, // id 274
    {275, "TypeWriterBox", 136U, 0x103U, 0x0U}, // id 275
    {276, "DialogWindow", 92U, 0x103U, 0x0U}, // id 276
    {277, "BackgroundFX", 36U, 0x0U, 0x0U}, // id 277
    {278, "MenuLoader", 292U, 0x0U, 0x0U}, // id 278
};

} // namespace

extern "C" {

igi::retail::core::TaskTypeSlot QTask_TypeTable[igi::retail::core::task_type_slot_count] = {};

// Native data: retail 0x0054FF24, 16-byte phase map (initial contents not yet
// captured; writers are the 0x00401400/0x00401530 link natives).
std::uint8_t g_54ff24[16] = {};
igi::retail::core::FrameHandler QTask_HandlerTable[igi::retail::core::task_type_slot_count *
                                       igi::retail::core::task_phase_count] = {};
std::uint8_t QTask_EventTable[igi::retail::core::event_table_entry_count * 6U] = {};
std::uint16_t QTask_CurrentType = 0U;
std::uint8_t QTask_FirstLinkFlag[igi::retail::core::task_type_slot_count *
                           igi::retail::core::task_phase_count] = {};
std::uint32_t QTask_LinkedHandlerCounter = 0xFFFFFFFFU;
std::uint32_t QTask_ContextHandlerCount = 0U;
std::uint32_t QTask_NoContextHandlerCount = 0U;
// 0x00A969E0: byte-indexed table, filled with -1 by QTask_Init at 0x00401105-0x0040112E
// (`mov edi,0xa969e0` / `or eax,-1` / `rep stosd` with ecx=0x3F / `stosw word es:[edi],ax`),
// so 0x3F dwords then one word: 254 bytes are written. The readers index it as a BYTE and
// sign-extend - 0x004C788D and 0x004C8523 are both `movsx ecx, byte [ecx + 0xa969e0]` followed
// by `cmp ecx,-1` - and native_401d30 stores a byte at 0x00401D43. A byte index admits 0..255,
// so the extent is declared 0x100. Measured: the 254 bytes the fill writes. The last two bytes
// are left as the image leaves them, which is what retail does too.
std::uint8_t g_a969e0[0x100] = {};


// Port-side startup replay bound to the live 0x00401900 registration capture;
// not a retail native. Registers every recorded class whose id falls in the
// inclusive range and fails when the registry hands out a different id, so a
// missing or extra registration surfaces instead of silently shifting the ids
// the per-frame dispatch indexes.
void TaskTypes_RegisterRetailClasses(std::uint16_t first_id, std::uint16_t last_id)
{
    for (const RetailRegistrationRecord& record : kRetailRegistrationOrder)
    {
        if (record.class_id < first_id || record.class_id > last_id)
        {
            continue;
        }
        const std::uint16_t registered = TaskType_Register(
            record.name, record.object_size, record.parent_type,
            record.task_list_argument);
        if (registered != record.class_id)
        {
            throw std::logic_error(
                "TaskTypes_RegisterRetailClasses: registered class id " +
                std::to_string(registered) + " where the retail capture recorded " +
                std::to_string(record.class_id) + " for " +
                (record.name != nullptr ? record.name : "?"));
        }
    }
}

// Native: QTask_LinkQTaskEventHandlerNoContextFn; retail VA 0x00401530.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 on retail IGI.EXE; VA 0x00401530..0x004015CD (decoded instruction by instruction in
//                 evidence section 20 history; the propagation loop and the recursive call are read from the listing).
// Behavior/CFG: increments the no-context handler count at 0x00567C70 and the linked-handler counter at 0x00536148,
//           computes the handler index as (event_id & 0xffff) + context * 0x180, stores the handler at
//           0x00A96AE0 + index * 4 and the first-link byte ((count == 1) ? 1 : 0) at 0x0054FF34 + index, then walks the
//           type records from 0x00AF5EE8 by 0x30 while below 0x00AFA6E8 and, for every record whose +0x1D byte is set and
//           whose first word equals the context word and whose own flag byte is clear, calls ITSELF with
//           (record index, context, handler); afterwards calls 0x004014B0 and decrements 0x00567C70, returning it.
// Status/limits: STRUCTURALLY_VERIFIED (CFG 7 blocks / 10 edges, score 1.000); runtime UNOBSERVED. The loop bounds are retail's own
//           (0x00AF5EE8 to 0x00AFA6E8 stride 0x30); the 0x004014B0 callee is transcribed separately.
std::uint32_t NATIVE_DECL QTask_LinkQTaskEventHandlerNoContextFn(
    std::uint16_t class_id, std::uint32_t event_id, igi::retail::core::FrameHandler handler)
{
    NATIVE_LOG_ENTER("QTask_LinkQTaskEventHandlerNoContextFn", "~0x00401530");
    const std::uint32_t count = ++QTask_NoContextHandlerCount;              // 0x00401530/0x00401542/0x0040155C
    ++QTask_LinkedHandlerCounter;                                          // 0x00401538/0x00401543/0x00401545
    // 0x0040153E: eax = the FIRST argument, 0x00401553 masks its LOW WORD. 0x00401558/0x00401562 build the product from the
    // SECOND argument WITHOUT the low-byte mask 0x00401400 applies, so the whole second argument feeds the stride here.
    const std::uint32_t index =
        (static_cast<std::uint32_t>(class_id) & 0xFFFFU) + event_id * 0x180U;
    const std::uint8_t first_link = (count == 1U) ? 1U : 0U;                // 0x00401567/0x0040156A
    QTask_HandlerTable[index] = handler;                                   // 0x0040156E
    QTask_FirstLinkFlag[index] = first_link;                               // 0x00401575

    std::uint8_t* record = reinterpret_cast<std::uint8_t*>(&QTask_TypeTable[0]) + 8U; // 0x0040157D: edi = 0x00AF5EE8
    std::uint8_t* const record_end =
        reinterpret_cast<std::uint8_t*>(&QTask_TypeTable[0]) + 0x1C08U;    // 0x004015B1: cmp edi, 0x00AFA6E8
    std::uint32_t slot = 0;                                                // 0x0040157B: ebx = 0
    while (record < record_end)                                            // 0x004015B7: jl 0x00401582
    {
        if (record[0x1D] != 0U)                                            // 0x00401582/0x00401585/0x00401587
        {
            if (*reinterpret_cast<std::uint16_t*>(record) ==
                static_cast<std::uint16_t>(class_id))                      // 0x00401589/0x0040158E/0x00401591: the FIRST argument
            {
                if (QTask_FirstLinkFlag[event_id * 0x180U + slot] == 0U)     // 0x00401593/0x0040159A/0x0040159C
                {
                    QTask_LinkQTaskEventHandlerNoContextFn(
                        static_cast<std::uint16_t>(slot), event_id, handler); // 0x004015A2/0x004015A3/0x004015A4/0x004015A5
                }
            }
        }
        record += 0x30U;                                                   // 0x004015AD: add edi, 0x30
        ++slot;                                                            // 0x004015B0: inc ebx
    }

    TaskType_RefreshHandlerMask();                                         // 0x004015B9: call 0x004014B0
    --QTask_NoContextHandlerCount;                                         // 0x004015BE/0x004015C4/0x004015C7
    return QTask_NoContextHandlerCount;                                    // 0x004015CD: ret
}

// Native: QTask_LinkQTaskEventHandler; retail VA 0x00401400.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 on retail IGI.EXE; VA 0x00401400..0x004014A3, decoded instruction by instruction.
// Behavior/CFG: the context variant. Increments the context handler count at 0x00567C6C and the linked-handler counter at
//           0x00536148; derives the phase stride from the LOW BYTE of the first argument (and eax,0xff at 0x00401417) while
//           the table index uses its LOW WORD (and eax,0xffff at 0x00401430), so index = (arg1 & 0xffff) + (arg1 & 0xff) *
//           0x180; stores the handler at 0x00A96AE0 + index*4 and the first-link byte ((count==1)?1:0) at
//           0x0054FF34 + index; walks type records 0x00AF5EE8..0x00AFA6E8 stride 0x30 and, for each record whose +0x1D byte
//           is set, whose first word equals the SECOND argument's word and whose flag at (arg1 & 0xff) * 0x180 + slot is
//           clear, recurses with (slot, arg1, handler); then calls 0x004014B0, decrements 0x00567C6C and returns it.
// Status/limits: TRANSCRIBED (full body, statically derived); runtime UNOBSERVED.
std::uint32_t NATIVE_DECL QTask_LinkQTaskEventHandler(
    std::uint16_t class_id, std::uint32_t event_id, igi::retail::core::FrameHandler handler)
{
    NATIVE_LOG_ENTER("QTask_LinkQTaskEventHandler", "~0x00401530");
    const std::uint32_t count = ++QTask_ContextHandlerCount;                // 0x00401400/0x00401415/0x00401435
    ++QTask_LinkedHandlerCounter;                                          // 0x0040140D/0x00401416/0x0040141C
    // 0x00401408: ebp = [esp+0x10], the SECOND argument; 0x00401413/0x00401417 take the phase from its LOW BYTE.
    const std::uint32_t phase = event_id & 0xFFU;
    // 0x00401429: eax = [esp+0x10], the FIRST argument; 0x00401430 masks its LOW WORD and 0x0040143B adds the phase stride.
    const std::uint32_t index =
        (static_cast<std::uint32_t>(class_id) & 0xFFFFU) + phase * 0x180U;
    const std::uint8_t first_link = (count == 1U) ? 1U : 0U;                // 0x0040143D/0x00401440
    QTask_HandlerTable[index] = handler;                                   // 0x00401444
    QTask_FirstLinkFlag[index] = first_link;                               // 0x0040144B

    std::uint8_t* record = reinterpret_cast<std::uint8_t*>(&QTask_TypeTable[0]) + 8U; // 0x00401453: edi = 0x00AF5EE8
    std::uint8_t* const record_end =
        reinterpret_cast<std::uint8_t*>(&QTask_TypeTable[0]) + 0x1C08U;    // 0x00401487: cmp edi, 0x00AFA6E8
    std::uint32_t slot = 0;                                                // 0x00401451: ebx = 0
    while (record < record_end)                                            // 0x0040148D: jl 0x00401458
    {
        if (record[0x1D] != 0U)                                            // 0x00401458/0x0040145B/0x0040145D
        {
            if (*reinterpret_cast<std::uint16_t*>(record) ==
                static_cast<std::uint16_t>(class_id))                       // 0x0040145F/0x00401464/0x00401467
            {
                if (QTask_FirstLinkFlag[phase * 0x180U + slot] == 0U)       // 0x00401469/0x00401470/0x00401472
                {
                    QTask_LinkQTaskEventHandler(
                        static_cast<std::uint16_t>(slot), event_id, handler);   // 0x00401478: push handler (arg3)
                                                                               // 0x00401479: push ebp = arg1
                                                                               // 0x0040147A: push ebx = slot
                }
            }
        }
        record += 0x30U;                                                   // 0x00401483: add edi, 0x30
        ++slot;                                                            // 0x00401486: inc ebx
    }

    TaskType_RefreshHandlerMask();                                         // 0x0040148F: call 0x004014B0
    --QTask_ContextHandlerCount;                                           // 0x00401494/0x0040149A/0x0040149D
    return QTask_ContextHandlerCount;                                      // 0x004014A3: ret
}

// Native: QTask_GetCurrentType; retail VA 0x004018F0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004018F0..0x004018F7.
// Behavior/CFG: loads the 16-bit current task type at 0x0054F928 into AX and returns it.
// Status/limits: STRUCTURALLY_VERIFIED; the load and the AX return match the retail exactly.
std::uint16_t NATIVE_DECL QTask_GetCurrentType()
{
    NATIVE_LOG_ENTER("QTask_GetCurrentType", "~0x00401530");
    return QTask_CurrentType;
}

// Native: QTask_AllocateEvent; retail VA 0x004017C0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004017C0..0x0040180C.
// Behavior/CFG: scans the 6-byte table from 0x0054F9F2 while record+2 is set; marks +2=1, stores the value word at +0, sets +3=1, returns the id in EAX; exhaustion pushes "Unable to allocate event", calls Debug_Error (0x004AF7B0) and jmp $ at 0x004017E8.
// Status/limits: STRUCTURALLY_VERIFIED; scan bounds, store order (+2, one 16-bit +0, +3), the dword id return and the exhaustion spin match retail; runtime unobserved for the allocation path because the startup boundary precedes it.
std::uint32_t NATIVE_DECL QTask_AllocateEvent(std::uint16_t value)
{
    NATIVE_LOG_ENTER("QTask_AllocateEvent", "~0x00401530");
    std::uint32_t event_id = igi::retail::core::first_event_id;
    while (event_id < igi::retail::core::event_table_entry_count &&
           QTask_EventTable[event_id * 6U + 2U] != 0U)
    {
        ++event_id;
    }
    if (event_id >= igi::retail::core::event_table_entry_count)
    {
        // 0x004017DB-0x004017E8: push str.Unable_to_allocate_event; call Debug_Error (0x004AF7B0);
        // add esp,4; then `jmp $` at 0x004017E8 - the retail never returns from exhaustion.
        Debug_Error("Unable to allocate event");
        for (;;)
        {
        }
    }
    const std::size_t entry = static_cast<std::size_t>(event_id) * 6U;
    // Retail store order and widths, 0x004017F7 / 0x004017FD / 0x00401804: byte +2 = 1, then ONE 16-bit store of
    // the value word at +0 (`mov word [ecx+0x54F930], si`), then byte +3 = 1. Do not split the word store.
    // The three stores are volatile so the compiler keeps the retail's emission order: GCC reordered a plain
    // sequence to +2, +3, +0 (defect found in artifact b4585618 at 0x41a3fc/0x41a403/0x41a40a). Volatile adds no
    // instruction; it only forbids the reordering that separated the 0x004017FD word store from them.
    volatile std::uint8_t* const record = &QTask_EventTable[entry];
    record[2U] = 1U;
    *reinterpret_cast<volatile std::uint16_t*>(const_cast<std::uint8_t*>(record)) = value;
    record[3U] = 1U;
    // Retail keeps the full id in EAX (inc eax from 0x20 at 0x004017D2) and returns it; callers read AL.
    return static_cast<std::uint32_t>(event_id);
}

// Native: QTask_ClearEventTable; retail VA 0x004011D0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004011D0..0x004011E3.
// Behavior/CFG: starts at the event table's used byte at 0x0054F932, clears it
//           and steps six bytes at a time until 0x0054FF26.
// Status/limits: STRUCTURALLY_VERIFIED; the start, the stride and the end are
//           the retail's.
void NATIVE_DECL QTask_ClearEventTable()
{
    NATIVE_LOG_ENTER("QTask_ClearEventTable", "~0x00401530");
    for (std::size_t entry = 0U; entry < igi::retail::core::event_table_entry_count; ++entry)
    {
        QTask_EventTable[entry * 6U + 2U] = 0U;
    }
}

// Native: QTask_ClearTaskTypes; retail VA 0x004011B0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004011B0..0x004011C7.
// Behavior/CFG: walks the task type table from 0x00AF5EE0 in 0x30-byte steps
//           to 0x00AFA6E0 and clears the name dword at slot + 0x00 and the
//           in-use byte at slot + 0x25.
// Status/limits: STRUCTURALLY_VERIFIED; the base, the stride, the end and
//           the two cleared fields are the retail's.
void NATIVE_DECL QTask_ClearTaskTypes()
{
    NATIVE_LOG_ENTER("QTask_ClearTaskTypes", "~0x00401530");
    for (std::size_t slot = 0U; slot < igi::retail::core::task_type_slot_count; ++slot)
    {
        QTask_TypeTable[slot].name = 0U;
        QTask_TypeTable[slot].in_use = 0U;
    }
}

// Retail natives called by QTask_Init's phase links, declared here because their headers do not carry them.
// Each is its own transcription at its own VA; these are declarations, not additions.
void TaskEventTable_MarkSlot(std::uint16_t value, std::uint32_t index);
void TaskEventTable_MarkSlotNoMask(std::uint16_t value, std::uint32_t index);

// Native: QTask_Init; retail VA 0x004010D0.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja MCP and Radare2 on retail IGI.EXE; VA 0x004010D0..0x004011B0.
// Behavior/CFG: zeroes the 0x40 dwords at 0x00AFA6E0, clears the word at
//           0x00AFA7E0 and the dword at 0x0054F92C, clears the event table
//           through 0x004011D0 and the task type table through 0x004011B0, and only after the class
//           registration (0x00401130) and the phase links (0x0040113E-0x00401176) clears the
//           0x17D00-dword handler table at 0x00A96AE0 (0x0040117D-0x0040118F), the order retail uses,
//           fills the 0x3F dwords at 0x00A969E0 with -1, registers the
//           "QTask" class with a 0x20 byte object and the parent 0x0181, keeps
//           the class id at 0x0054F928, and links the phase handlers.
//           CORRECTION (measured from the disassembly, not from this note's earlier wording): the
//           -1 fill of 0x00A969E0 sits at 0x00401105-0x0040112E, which is BEFORE the registration
//           at 0x00401130 and before the 0x17D00 clear at 0x0040117B-0x00401190. An earlier
//           version of this block placed it after the clear, which the bytes do not show.
// Status/limits: STRUCTURALLY_VERIFIED; the clears, the class registration
//           and the current type write are the retail's. The retail's
//           handler links for the QTask class run through the natives at
//           0x00401860 and 0x00401890, which the port does not use for its
//           own phase table. The phase 0..3 links at 0x0040113E-0x00401176 are transcribed here, so the
//           handler-table clear sits at its retail position: after those links and before the 0x4B0B00 and
//           0x4B8810 calls, which are still missing. The six-byte phase records the links write at
//           0x0054F930 + phase*6 are not yet runtime-observed.
void NATIVE_DECL QTask_Init()
{
    NATIVE_LOG_ENTER("QTask_Init", "~0x00401530");
    QTask_ClearEventTable();
    QTask_ClearTaskTypes();
    // 0x00401105 mov edi,0xa969e0 / 0x0040111d or eax,-1 / 0x00401120 rep stosd with ecx=0x3F /
    // 0x0040112e stosw word es:[edi],ax. The table is filled with -1 BEFORE the registration at
    // 0x00401130 and before the 0x17D00 clear at 0x0040117B-0x00401190 - not after it, which is
    // what this file's provenance block previously said and what the disassembly does not show.
    // 0x3F dwords then one word, so 254 bytes, and the dword/word split is retail's, not tidied.
    for (std::size_t table_slot = 0U; table_slot < 0x3FU; ++table_slot) {
        *reinterpret_cast<std::uint32_t*>(&g_a969e0[table_slot * 4U]) = 0xFFFFFFFFU;
    }
    *reinterpret_cast<std::uint16_t*>(&g_a969e0[0x3FU * 4U]) = 0xFFFFU;
    QTask_CurrentType = TaskType_Register("QTask", 0x20U, 0x0181U, 0U);
    // 0x0040113E-0x0040114B: push 0 / call 0x004018F0 / push eax / call 0x00401860 / add esp,8.
    // The phase dword pushed first is the link call's second argument, not padding: 0x004018F0 takes no
    // argument, so the phase survives to [esp+8] while EAX (the current type) becomes [esp+4].
    TaskEventTable_MarkSlot(QTask_GetCurrentType(), 0U);
    // 0x0040114E-0x0040115B: same pair for phase 1 through 0x00401890.
    TaskEventTable_MarkSlotNoMask(QTask_GetCurrentType(), 1U);
    // 0x0040115E-0x0040116B: phase 2.
    TaskEventTable_MarkSlotNoMask(QTask_GetCurrentType(), 2U);
    // 0x0040116E-0x00401176: phase 3.
    TaskEventTable_MarkSlotNoMask(QTask_GetCurrentType(), 3U);
    // 0x0040117B xor eax,eax / 0x0040117D mov ecx,0x17d00 / 0x00401183 mov edi,0xa96ae0 /
    // 0x0040118F rep stosd - the handler/state table is zeroed here, after the registration and after the
    // phase 0..3 links at 0x0040113E-0x00401176 (transcribed above), and before the 0x4B0B00 and 0x4B8810
    // calls at 0x00401196/0x004011A5, which are not transcribed yet. 254 phases x 384 classes = 0x17D00 dwords.
    for (std::size_t handler_index = 0U; handler_index < 0x17D00U; ++handler_index) {
        QTask_HandlerTable[handler_index] = nullptr;
    }
}

} // extern "C"

#undef NATIVE_DECL
