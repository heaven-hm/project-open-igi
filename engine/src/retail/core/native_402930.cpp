// Native: QTask_RegisterClasses; retail VA 0x00402930.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: radare2 6.2.0 af/pdf and capstone 5.0.7 over the raw bytes read at the VA through the PE section table. VA
//   0x00402930-0x00402C3B (780 bytes, 156 instructions of exactly 5 bytes each: 155 `call`s and one tail `jmp 0x004CEF30` at 0x00402C37).
//   Measured by disassembling 0x00402930..0x00402C3B: "155 call sites", span ends exactly at 0x00402C3B, exit 0. One basic block - there is
//   no branch and no `ret` anywhere in the extent. The 155 calls take NO arguments: every instruction in the extent is a call, so nothing is
//   pushed and no register is set up before any of them, and no call reads its predecessor's return value.
// Behavior/CFG: retail's startup init sequence. The six opening calls are 0x00508F80 (AI graph), 0x00489560 (AI detection), 0x00407D60
//   (material defines), 0x00488BB0 (level flow), 0x00402FF0 (game options) and 0x00415530 (Game_Init), at 0x00402930, 0x00402935, 0x0040293A,
//   0x0040293F, 0x00402944 and 0x00402949. Retail reaches the AI graph BEFORE Game_Init and this transcription does too. The sweep then runs
//   the task-class registrars in retail's order - including 0x004C1780, the ret-only registrar that registers nothing, called because retail
//   calls it - and ends by tail-jumping to 0x004CEF30, so this function never returns to its caller.
// Status/limits: TRANSCRIBED; FAIL-CLOSED. All 154 distinct callees have an implementation in this tree and 152 are symbols in the current
//   build; the two exceptions are 0x00508F80 (engine/src/retail/core/native_508f80.cpp, written but absent from CMakeLists.txt) and
//   0x0044C8D0 (engine/src/retail/core/native_44c8d0.cpp, likewise). The owner ruled on 2026-10-25 that the AI graph is transcribed rather
//   than recorded as a gap, so 0x00508F80 is called first and in order and this file will not link until that cluster lands; that failure is
//   the finding and must not be worked around. The port's earlier body - ReportNotTranscribed, AI_Detection_Init, RegisterGameClasses and
//   RegisterFlowClass - is deleted in the same change; no stand-in survives. The predecessor's claim that "146 of 154 callees have no engine
//   implementation" is wrong by 146 and is retracted here.
#include <cstdint>
#include "dev_log.hpp"

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

extern "C" {

void NATIVE_DECL AIGraph_Init_508F80();   // 0x00508F80
void NATIVE_DECL AI_Detection_Init();   // 0x00489560
void NATIVE_DECL Material_DefineScriptNatives();   // 0x00407D60
void NATIVE_DECL Task_DefineLevelFlow();   // 0x00488BB0
void NATIVE_DECL Config_RegisterGameOptions_402FF0();   // 0x00402FF0
void NATIVE_DECL Game_Init();   // 0x00415530
void NATIVE_DECL AnimSound_LoadParameters();   // 0x004C1780
void NATIVE_DECL Flow_Init();   // 0x00401FC0
void NATIVE_DECL Task_DefineEditVariable();   // 0x00488800
void NATIVE_DECL Task_DefineSubLevel();   // 0x00488620
void NATIVE_DECL Task_DefineGuardGenerator();   // 0x00487BE0
void NATIVE_DECL Task_DefineConditionalContainer();   // 0x004872C0
void NATIVE_DECL Task_DefineIntroScreen_415070();   // 0x00415070
void NATIVE_DECL Task_DefineStatusScreen();   // 0x00486900
void NATIVE_DECL Task_DefineStatusMessageScreen();   // 0x00485A40
void NATIVE_DECL Task_DefineStatusMessage();   // 0x00485330
void NATIVE_DECL Task_DefineGameScreen();   // 0x004851B0
void NATIVE_DECL Core_RegisterMission_484F30();   // 0x00484F30
void NATIVE_DECL Task_DefineHumanCamera();   // 0x004825C0
void NATIVE_DECL Task_DefineExplodeObject();   // 0x004813E0
void NATIVE_DECL Task_DefineExplosionTypes();   // 0x004801B0
void NATIVE_DECL Task_DefineFlashBang();   // 0x0047E700
void NATIVE_DECL Task_DefineBulletHole();   // 0x0047CF70
void NATIVE_DECL Task_DefineAmmoPatterns();   // 0x0047CF20
void NATIVE_DECL Core_RegisterAmmoType_47CAB0();   // 0x0047CAB0
void NATIVE_DECL Task_DefineWeaponType_413DF0();   // 0x00413DF0
void NATIVE_DECL Task_DefineWeaponConfig_407290();   // 0x00407290
void NATIVE_DECL Task_DefineGunShot();   // 0x0047B4B0
void NATIVE_DECL Task_DefineGun();   // 0x00477E30
void NATIVE_DECL Task_DefineGunParts();   // 0x00477220
void NATIVE_DECL Task_DefineGrenade();   // 0x00476730
void NATIVE_DECL Task_DefineM203();   // 0x00476110
void NATIVE_DECL Task_DefineMine();   // 0x00475AC0
void NATIVE_DECL Task_DefineProximityMine();   // 0x00475380
void NATIVE_DECL Task_DefineStationaryGun();   // 0x00473ED0
void NATIVE_DECL Task_DefineGunMP5SD();   // 0x004739D0
void NATIVE_DECL Task_DefineGunM16A2();   // 0x004733F0
void NATIVE_DECL Task_DefineGunDragunov();   // 0x00472D00
void NATIVE_DECL Task_DefineGunSPAS12();   // 0x00472A70
void NATIVE_DECL Task_DefineGunCABarrelAndClip();   // 0x00472670
void NATIVE_DECL Task_DefineBinocularOverlayAndHolder();   // 0x00470FA0
void NATIVE_DECL Task_DefineBinocular();   // 0x00470610
void NATIVE_DECL Task_DefineMedipack();   // 0x00470410
void NATIVE_DECL Task_DefineAIStationaryGun();   // 0x0046F2B0
void NATIVE_DECL Task_DefineAIStationaryGunHolder();   // 0x0046E370
void NATIVE_DECL Task_DefineKnifeStab();   // 0x0046DF10
void NATIVE_DECL Task_DefineGunx2();   // 0x0046DD00
void NATIVE_DECL Task_DefineAddAmmo();   // 0x0046DB40
void NATIVE_DECL Task_DefineAmmoPickup();   // 0x0046D420
void NATIVE_DECL Task_DefineGunPickup();   // 0x0046CB70
void NATIVE_DECL Task_DefineGenericPickup();   // 0x0046C0C0
void NATIVE_DECL Task_DefineComputer();   // 0x0046BA60
void NATIVE_DECL Task_DefineComputerTextItem();   // 0x0046B7F0
void NATIVE_DECL Task_DefineComputerButtonItem();   // 0x0046B510
void NATIVE_DECL Task_DefineComputerMenu();   // 0x0046AB60
void NATIVE_DECL Task_DefineComputerMap();   // 0x004696B0
void NATIVE_DECL Task_DefineComputerObjectives();   // 0x00468E20
void NATIVE_DECL Task_DefineComputerLog();   // 0x004688B0
void NATIVE_DECL Task_DefineComputerHilight();   // 0x004682F0
void NATIVE_DECL Task_DefineComputerObject();   // 0x00467700
void NATIVE_DECL Task_DefineComputerObjective();   // 0x00466F70
void NATIVE_DECL Task_DefineComputerLimit();   // 0x00466E40
void NATIVE_DECL Task_DefineComputerStartup();   // 0x004663D0
void NATIVE_DECL Task_DefineDeathZone();   // 0x00465BC0
void NATIVE_DECL Task_DefineHumanAttach();   // 0x00465A10
void NATIVE_DECL Task_DefineHumanView();   // 0x00464260
void NATIVE_DECL Task_DefineHuman();   // 0x0045E450
void NATIVE_DECL Task_DefineHumanPlayer_40FE30();   // 0x0040FE30
void NATIVE_DECL Task_DefineHumanPlayerInput();   // 0x0045DB70
void NATIVE_DECL Task_DefineHumanSoldier();   // 0x0045CAB0
void NATIVE_DECL Task_DefineHumanSoldierPistol();   // 0x00459880
void NATIVE_DECL Task_DefineHumanSoldierRPG();   // 0x00456AD0
void NATIVE_DECL Task_DefineHumanSoldierFemale();   // 0x00454970
void NATIVE_DECL Task_DefinePatrolPath();   // 0x00453550
void NATIVE_DECL Task_DefinePatrolPathCommand();   // 0x004533B0
void NATIVE_DECL Task_DefineHumanAI();   // 0x0044C8D0
void NATIVE_DECL Task_DefineHumanAIConfig();   // 0x00406450
void NATIVE_DECL Task_DefineAmbientArea();   // 0x0044C130
void NATIVE_DECL Task_DefineGlassShatter_449D30();   // 0x00449D30
void NATIVE_DECL Task_DefineGlass();   // 0x00449970
void NATIVE_DECL Task_DefineSiren();   // 0x00449360
void NATIVE_DECL Task_DefineRadarDish();   // 0x00448D30
void NATIVE_DECL Task_DefineBuilding();   // 0x004485E0
void NATIVE_DECL Task_DefineGenericTBA();   // 0x00447DB0
void NATIVE_DECL Task_DefineDoor();   // 0x004464C0
void NATIVE_DECL Task_DefineSwitch();   // 0x00444BF0
void NATIVE_DECL Task_DefineCabinetAndDrawer();   // 0x00443ED0
void NATIVE_DECL Task_DefineTerminal();   // 0x00443680
void NATIVE_DECL Task_DefineSCameraTypes();   // 0x00440FD0
void NATIVE_DECL Task_DefineSCameraControl();   // 0x00440C20
void NATIVE_DECL Task_DefineAlarmControl();   // 0x00440710
void NATIVE_DECL Task_DefineLadder();   // 0x004403E0
void NATIVE_DECL Task_DefineFence();   // 0x0043F900
void NATIVE_DECL Task_DefineWire();   // 0x0043EFF0
void NATIVE_DECL Task_DefineGenericPhysicsObj();   // 0x0043E8D0
void NATIVE_DECL Task_DefineGenericPhysicsMagicObj();   // 0x0043E4F0
void NATIVE_DECL Task_DefineElevator();   // 0x0043CC70
void NATIVE_DECL Task_DefineElevatorFloorIndicator();   // 0x0043C6D0
void NATIVE_DECL Task_DefineExplodeMagicObj();   // 0x0043BFD0
void NATIVE_DECL Task_DefineSimpointObjGenerator();   // 0x0043B850
void NATIVE_DECL Task_DefineGenerator();   // 0x0043B2F0
void NATIVE_DECL Task_DefineRadio();   // 0x0043AD80
void NATIVE_DECL Task_DefineMineField();   // 0x0043A840
void NATIVE_DECL Task_DefineAlarmLightAndReflector();   // 0x004397C0
void NATIVE_DECL Task_DefineRotatingObject();   // 0x00438DF0
void NATIVE_DECL Task_DefinePlaceExplosiveTBA();   // 0x004388B0
void NATIVE_DECL Task_DefineGunShotEffect();   // 0x00438390
void NATIVE_DECL Task_DefineSmoke();   // 0x00437510
void NATIVE_DECL Task_DefineEnvironmentSound();   // 0x00436B10
void NATIVE_DECL Task_DefineRainEffect();   // 0x00436780
void NATIVE_DECL Task_DefineRudder();   // 0x004361D0
void NATIVE_DECL Task_DefineHatch();   // 0x00435D90
void NATIVE_DECL Task_DefineGear();   // 0x00435870
void NATIVE_DECL Task_DefineCockpitShield();   // 0x004351E0
void NATIVE_DECL Task_DefinePlane();   // 0x004330F0
void NATIVE_DECL Task_DefineHeli();   // 0x004310F0
void NATIVE_DECL Task_DefineWheel();   // 0x0042FF00
void NATIVE_DECL QTask_LinkClassHandlersB_42FE40();   // 0x0042FE40
void NATIVE_DECL Task_DefineCar();   // 0x0042DCF0
void NATIVE_DECL QTask_LinkClassHandlersA_42DAC0();   // 0x0042DAC0
void NATIVE_DECL Task_DefineRotor();   // 0x0042D2B0
void NATIVE_DECL Task_DefineAfterburner();   // 0x0042BEB0
void NATIVE_DECL Train_Register_42ACC0();   // 0x0042ACC0
void NATIVE_DECL Missile_Register_427FF0();   // 0x00427FF0
void NATIVE_DECL HitZone_Register_427D90();   // 0x00427D90
void NATIVE_DECL RocketPod_Register_427830();   // 0x00427830
void NATIVE_DECL HeliDoor_Register_4272A0();   // 0x004272A0
void NATIVE_DECL HeliWheel_Register_426D90();   // 0x00426D90
void NATIVE_DECL CarDoor_Register_426630();   // 0x00426630
void NATIVE_DECL CarAI_Register_425C80();   // 0x00425C80
void NATIVE_DECL ConditionalSound_Register_424E80();   // 0x00424E80
void NATIVE_DECL Task_DefineMenuManager_4170A0();   // 0x004170A0
void NATIVE_DECL Cursor_Init_424B90();   // 0x00424B90
void NATIVE_DECL MenuWindow_Register_423FC0();   // 0x00423FC0
void NATIVE_DECL ControlsMenu_Register_4230A0();   // 0x004230A0
void NATIVE_DECL ModelViewBox_Register_422B40();   // 0x00422B40
void NATIVE_DECL MenuFrame_Register_4223D0();   // 0x004223D0
void NATIVE_DECL MenuScreen_Register_421750();   // 0x00421750
void NATIVE_DECL MenuButton_Register_421430();   // 0x00421430
void NATIVE_DECL MenuText_Register_420CC0();   // 0x00420CC0
void NATIVE_DECL MenuTextConditional_Register_420300();   // 0x00420300
void NATIVE_DECL Task_DefineSlideBar_41F340();   // 0x0041F340
void NATIVE_DECL Task_DefineListBox_41E690();   // 0x0041E690
void NATIVE_DECL Task_DefineToggleBox_41DC50();   // 0x0041DC50
void NATIVE_DECL Task_DefineInputBox_41D080();   // 0x0041D080
void NATIVE_DECL Task_DefineAsciiBox_41C700();   // 0x0041C700
void NATIVE_DECL Task_DefineScrollListBox_41B8E0();   // 0x0041B8E0
void NATIVE_DECL Task_DefinePictureBox_41B310();   // 0x0041B310
void NATIVE_DECL Task_DefineTextBox_41AAF0();   // 0x0041AAF0
void NATIVE_DECL Task_DefineTypeWriterBox_41A1E0();   // 0x0041A1E0
void NATIVE_DECL Task_DefineDialogWindow_4199F0();   // 0x004199F0
void NATIVE_DECL BackgroundFX_Init_419760();   // 0x00419760
void NATIVE_DECL MenuLoader_Register_4190E0();   // 0x004190E0
void NATIVE_DECL Core_CheckCdPresent_402F50();   // 0x00402F50

void NATIVE_DECL Core_CopyState_4CEF30();   // 0x004CEF30 - the tail jump target

void NATIVE_DECL QTask_RegisterClasses()
{
    NATIVE_LOG_ENTER("QTask_RegisterClasses", "0x402930");
    AIGraph_Init_508F80();   // 0x00402930: call 0x00508F80
    AI_Detection_Init();   // 0x00402935: call 0x00489560
    Material_DefineScriptNatives();   // 0x0040293A: call 0x00407D60
    Task_DefineLevelFlow();   // 0x0040293F: call 0x00488BB0
    Config_RegisterGameOptions_402FF0();   // 0x00402944: call 0x00402FF0
    Game_Init();   // 0x00402949: call 0x00415530
    AnimSound_LoadParameters();   // 0x0040294E: call 0x004C1780
    Flow_Init();   // 0x00402953: call 0x00401FC0
    Task_DefineEditVariable();   // 0x00402958: call 0x00488800
    Task_DefineSubLevel();   // 0x0040295D: call 0x00488620
    Task_DefineGuardGenerator();   // 0x00402962: call 0x00487BE0
    Task_DefineConditionalContainer();   // 0x00402967: call 0x004872C0
    Task_DefineIntroScreen_415070();   // 0x0040296C: call 0x00415070
    Task_DefineStatusScreen();   // 0x00402971: call 0x00486900
    Task_DefineStatusMessageScreen();   // 0x00402976: call 0x00485A40
    Task_DefineStatusMessage();   // 0x0040297B: call 0x00485330
    Task_DefineGameScreen();   // 0x00402980: call 0x004851B0
    Core_RegisterMission_484F30();   // 0x00402985: call 0x00484F30
    Task_DefineHumanCamera();   // 0x0040298A: call 0x004825C0
    Task_DefineExplodeObject();   // 0x0040298F: call 0x004813E0
    Task_DefineExplosionTypes();   // 0x00402994: call 0x004801B0
    Task_DefineFlashBang();   // 0x00402999: call 0x0047E700
    Task_DefineBulletHole();   // 0x0040299E: call 0x0047CF70
    Task_DefineAmmoPatterns();   // 0x004029A3: call 0x0047CF20
    Core_RegisterAmmoType_47CAB0();   // 0x004029A8: call 0x0047CAB0
    Task_DefineWeaponType_413DF0();   // 0x004029AD: call 0x00413DF0
    Task_DefineWeaponConfig_407290();   // 0x004029B2: call 0x00407290
    Task_DefineGunShot();   // 0x004029B7: call 0x0047B4B0
    Task_DefineGun();   // 0x004029BC: call 0x00477E30
    Task_DefineGunParts();   // 0x004029C1: call 0x00477220
    Task_DefineGrenade();   // 0x004029C6: call 0x00476730
    Task_DefineM203();   // 0x004029CB: call 0x00476110
    Task_DefineMine();   // 0x004029D0: call 0x00475AC0
    Task_DefineProximityMine();   // 0x004029D5: call 0x00475380
    Task_DefineStationaryGun();   // 0x004029DA: call 0x00473ED0
    Task_DefineGunMP5SD();   // 0x004029DF: call 0x004739D0
    Task_DefineGunM16A2();   // 0x004029E4: call 0x004733F0
    Task_DefineGunDragunov();   // 0x004029E9: call 0x00472D00
    Task_DefineGunSPAS12();   // 0x004029EE: call 0x00472A70
    Task_DefineGunCABarrelAndClip();   // 0x004029F3: call 0x00472670
    Task_DefineBinocularOverlayAndHolder();   // 0x004029F8: call 0x00470FA0
    Task_DefineBinocular();   // 0x004029FD: call 0x00470610
    Task_DefineMedipack();   // 0x00402A02: call 0x00470410
    Task_DefineAIStationaryGun();   // 0x00402A07: call 0x0046F2B0
    Task_DefineAIStationaryGunHolder();   // 0x00402A0C: call 0x0046E370
    Task_DefineKnifeStab();   // 0x00402A11: call 0x0046DF10
    Task_DefineGunx2();   // 0x00402A16: call 0x0046DD00
    Task_DefineAddAmmo();   // 0x00402A1B: call 0x0046DB40
    Task_DefineAmmoPickup();   // 0x00402A20: call 0x0046D420
    Task_DefineGunPickup();   // 0x00402A25: call 0x0046CB70
    Task_DefineGenericPickup();   // 0x00402A2A: call 0x0046C0C0
    Task_DefineComputer();   // 0x00402A2F: call 0x0046BA60
    Task_DefineComputerTextItem();   // 0x00402A34: call 0x0046B7F0
    Task_DefineComputerButtonItem();   // 0x00402A39: call 0x0046B510
    Task_DefineComputerMenu();   // 0x00402A3E: call 0x0046AB60
    Task_DefineComputerMap();   // 0x00402A43: call 0x004696B0
    Task_DefineComputerObjectives();   // 0x00402A48: call 0x00468E20
    Task_DefineComputerLog();   // 0x00402A4D: call 0x004688B0
    Task_DefineComputerHilight();   // 0x00402A52: call 0x004682F0
    Task_DefineComputerObject();   // 0x00402A57: call 0x00467700
    Task_DefineComputerObjective();   // 0x00402A5C: call 0x00466F70
    Task_DefineComputerLimit();   // 0x00402A61: call 0x00466E40
    Task_DefineComputerStartup();   // 0x00402A66: call 0x004663D0
    Task_DefineDeathZone();   // 0x00402A6B: call 0x00465BC0
    Task_DefineHumanAttach();   // 0x00402A70: call 0x00465A10
    Task_DefineHumanView();   // 0x00402A75: call 0x00464260
    Task_DefineHuman();   // 0x00402A7A: call 0x0045E450
    Task_DefineHumanPlayer_40FE30();   // 0x00402A7F: call 0x0040FE30
    Task_DefineHumanPlayerInput();   // 0x00402A84: call 0x0045DB70
    Task_DefineHumanSoldier();   // 0x00402A89: call 0x0045CAB0
    Task_DefineHumanSoldierPistol();   // 0x00402A8E: call 0x00459880
    Task_DefineHumanSoldierRPG();   // 0x00402A93: call 0x00456AD0
    Task_DefineHumanSoldierFemale();   // 0x00402A98: call 0x00454970
    Task_DefinePatrolPath();   // 0x00402A9D: call 0x00453550
    Task_DefinePatrolPathCommand();   // 0x00402AA2: call 0x004533B0
    Task_DefineHumanAI();   // 0x00402AA7: call 0x0044C8D0
    Task_DefineHumanAIConfig();   // 0x00402AAC: call 0x00406450
    Task_DefineAmbientArea();   // 0x00402AB1: call 0x0044C130
    Task_DefineGlassShatter_449D30();   // 0x00402AB6: call 0x00449D30
    Task_DefineGlass();   // 0x00402ABB: call 0x00449970
    Task_DefineSiren();   // 0x00402AC0: call 0x00449360
    Task_DefineRadarDish();   // 0x00402AC5: call 0x00448D30
    Task_DefineBuilding();   // 0x00402ACA: call 0x004485E0
    Task_DefineGenericTBA();   // 0x00402ACF: call 0x00447DB0
    Task_DefineDoor();   // 0x00402AD4: call 0x004464C0
    Task_DefineSwitch();   // 0x00402AD9: call 0x00444BF0
    Task_DefineCabinetAndDrawer();   // 0x00402ADE: call 0x00443ED0
    Task_DefineTerminal();   // 0x00402AE3: call 0x00443680
    Task_DefineSCameraTypes();   // 0x00402AE8: call 0x00440FD0
    Task_DefineSCameraControl();   // 0x00402AED: call 0x00440C20
    Task_DefineAlarmControl();   // 0x00402AF2: call 0x00440710
    Task_DefineLadder();   // 0x00402AF7: call 0x004403E0
    Task_DefineFence();   // 0x00402AFC: call 0x0043F900
    Task_DefineWire();   // 0x00402B01: call 0x0043EFF0
    Task_DefineGenericPhysicsObj();   // 0x00402B06: call 0x0043E8D0
    Task_DefineGenericPhysicsMagicObj();   // 0x00402B0B: call 0x0043E4F0
    Task_DefineElevator();   // 0x00402B10: call 0x0043CC70
    Task_DefineElevatorFloorIndicator();   // 0x00402B15: call 0x0043C6D0
    Task_DefineExplodeMagicObj();   // 0x00402B1A: call 0x0043BFD0
    Task_DefineSimpointObjGenerator();   // 0x00402B1F: call 0x0043B850
    Task_DefineGenerator();   // 0x00402B24: call 0x0043B2F0
    Task_DefineRadio();   // 0x00402B29: call 0x0043AD80
    Task_DefineMineField();   // 0x00402B2E: call 0x0043A840
    Task_DefineAlarmLightAndReflector();   // 0x00402B33: call 0x004397C0
    Task_DefineRotatingObject();   // 0x00402B38: call 0x00438DF0
    Task_DefinePlaceExplosiveTBA();   // 0x00402B3D: call 0x004388B0
    Task_DefineGunShotEffect();   // 0x00402B42: call 0x00438390
    Task_DefineSmoke();   // 0x00402B47: call 0x00437510
    Task_DefineEnvironmentSound();   // 0x00402B4C: call 0x00436B10
    Task_DefineRainEffect();   // 0x00402B51: call 0x00436780
    Task_DefineRudder();   // 0x00402B56: call 0x004361D0
    Task_DefineHatch();   // 0x00402B5B: call 0x00435D90
    Task_DefineGear();   // 0x00402B60: call 0x00435870
    Task_DefineCockpitShield();   // 0x00402B65: call 0x004351E0
    Task_DefinePlane();   // 0x00402B6A: call 0x004330F0
    AnimSound_LoadParameters();   // 0x00402B6F: call 0x004C1780
    Task_DefineHeli();   // 0x00402B74: call 0x004310F0
    Task_DefineWheel();   // 0x00402B79: call 0x0042FF00
    QTask_LinkClassHandlersB_42FE40();   // 0x00402B7E: call 0x0042FE40
    Task_DefineCar();   // 0x00402B83: call 0x0042DCF0
    QTask_LinkClassHandlersA_42DAC0();   // 0x00402B88: call 0x0042DAC0
    Task_DefineRotor();   // 0x00402B8D: call 0x0042D2B0
    Task_DefineAfterburner();   // 0x00402B92: call 0x0042BEB0
    Train_Register_42ACC0();   // 0x00402B97: call 0x0042ACC0
    Missile_Register_427FF0();   // 0x00402B9C: call 0x00427FF0
    HitZone_Register_427D90();   // 0x00402BA1: call 0x00427D90
    RocketPod_Register_427830();   // 0x00402BA6: call 0x00427830
    HeliDoor_Register_4272A0();   // 0x00402BAB: call 0x004272A0
    HeliWheel_Register_426D90();   // 0x00402BB0: call 0x00426D90
    CarDoor_Register_426630();   // 0x00402BB5: call 0x00426630
    CarAI_Register_425C80();   // 0x00402BBA: call 0x00425C80
    ConditionalSound_Register_424E80();   // 0x00402BBF: call 0x00424E80
    Task_DefineMenuManager_4170A0();   // 0x00402BC4: call 0x004170A0
    Cursor_Init_424B90();   // 0x00402BC9: call 0x00424B90
    MenuWindow_Register_423FC0();   // 0x00402BCE: call 0x00423FC0
    ControlsMenu_Register_4230A0();   // 0x00402BD3: call 0x004230A0
    ModelViewBox_Register_422B40();   // 0x00402BD8: call 0x00422B40
    MenuFrame_Register_4223D0();   // 0x00402BDD: call 0x004223D0
    MenuScreen_Register_421750();   // 0x00402BE2: call 0x00421750
    MenuButton_Register_421430();   // 0x00402BE7: call 0x00421430
    MenuText_Register_420CC0();   // 0x00402BEC: call 0x00420CC0
    MenuTextConditional_Register_420300();   // 0x00402BF1: call 0x00420300
    Task_DefineSlideBar_41F340();   // 0x00402BF6: call 0x0041F340
    Task_DefineListBox_41E690();   // 0x00402BFB: call 0x0041E690
    Task_DefineToggleBox_41DC50();   // 0x00402C00: call 0x0041DC50
    Task_DefineInputBox_41D080();   // 0x00402C05: call 0x0041D080
    Task_DefineAsciiBox_41C700();   // 0x00402C0A: call 0x0041C700
    Task_DefineScrollListBox_41B8E0();   // 0x00402C0F: call 0x0041B8E0
    Task_DefinePictureBox_41B310();   // 0x00402C14: call 0x0041B310
    Task_DefineTextBox_41AAF0();   // 0x00402C19: call 0x0041AAF0
    Task_DefineTypeWriterBox_41A1E0();   // 0x00402C1E: call 0x0041A1E0
    Task_DefineDialogWindow_4199F0();   // 0x00402C23: call 0x004199F0
    BackgroundFX_Init_419760();   // 0x00402C28: call 0x00419760
    MenuLoader_Register_4190E0();   // 0x00402C2D: call 0x004190E0
    Core_CheckCdPresent_402F50();   // 0x00402C32: call 0x00402F50
    Core_CopyState_4CEF30();   // 0x00402C37: jmp 0x004CEF30 - the tail jump; this function never returns
}

} // extern "C"

#undef NATIVE_DECL
