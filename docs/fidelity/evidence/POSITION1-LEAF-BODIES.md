# POSITION-1 LEAF BODIES - dependency-free bottom of the 0x508F80 registrar tree

Read-only static packet for `QTask_RegisterClasses` position 1 (`0x00508F80`, AI graph). No `engine/` writes, no builds. Every body below is transcribed from retail bytes; nothing is invented, and every body that would not link today is flagged in section 4.

| item | value |
|---|---|
| retail artifact | `/Users/haseeb-mir/Downloads/IGI/igi.exe` |
| retail SHA-256 | `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01` |
| single decompiler | Binary Ninja 6.0.10601, MCP `http://127.0.0.1:24642/mcp` (no Ghidra used for any target) |
| structural check | radare2 6.2.0 (abi 132, darwin-arm_64) |
| metadata | rabin2 6.2.0 |
| registration evidence | `docs/fidelity/evidence/retail-registration-per-registrar.json` (probe SHA-256 `5a66146be7e995bc0c3fbf819628ce75886cf5205ff9041c93f29d4953ae0798`); position 1 registrar `0x508f80`, 123 classes, ids 0..122 |
| artifact dir | `build/audit/main-0x48F530/level1-closure/20261024T052720Z/POSITION1-LEAVES` (this directory) |

Note: the filename named in the task, `tasktype-registration-trace.json`, does not exist under `docs/fidelity/evidence/`; the live capture is `retail-registration-per-registrar.json` plus `0x00401900-retail-registration-order.json` (scope `retail-tasktype-registration-order-v1`), both bound to the same retail SHA-256.

## 1. Selection rule (defensible bottom-up order)

The position-1 tree holds 537 functions (see `POSITION1-REGISTRAR-INVENTORY.md`). 214 of them resolve to a real `.text` extent (0x401000-0x533000, valid BN highest address, >=1 block) with **no direct callees**; 181 of those callee-free functions are still unlanded in `engine/`. Only two classes of body are dependency-free today:

1. **Registrar shells whose every direct callee is already landed in `engine/`** - callees are exactly `0x00401900` `TaskType_Register`, `0x00401400` `QTask_LinkQTaskEventHandler`, `0x00401530` `QTask_LinkQTaskEventHandlerNoContextFn`, `0x004018F0` `QTask_GetCurrentType`, `0x004017C0` `QTask_AllocateEvent` (all declared in `engine/include/igi/retail/core/task_types.hpp`) plus `0x004F1A60` `Script_Get_4F1A60` (`engine/include/igi/retail/script/script_state.hpp`). Exactly eight position-1 registrars satisfy this and stay under 120 bytes; they are ordered by their **observed class id** (2, 3, 23, 31, 32, 39, 46, 117).
2. **Leaf getters with no calls at all** - the twelve `Display_*` thunks `0x00491DB0`-`0x00491E60` (6 bytes each: `a1 <slot> c3`, i.e. `mov eax,[slot]; ret`). They read image dwords in `.rdata` (0x00542310-0x00542340), so they need no engine global and link as-is. Ordered by VA.

Excluded and listed in section 4: registrar functions whose bodies call not-yet-landed natives (the other 113 of the 123 position-1 ids), and the 181 unlanded callee-free thunks whose only operand is an unwritten engine global.

### 1.1 Order and observed ids

| # | VA | BN alias | size | blocks/edges | observed id | class name | engine state before this packet |
|---|---|---|---|---|---|---|---|
| 1 | `0x4e5370` | `DynCubeObj_Init` | 48 B | 1/0 | 2 | `DynCubeObj` | absent |
| 2 | `0x4da1d0` | `Hazinglayers_Init` | 108 B | 1/0 | 3 | `Hazinglayers` | absent |
| 3 | `0x4ebc60` | `SoundSysMicrophone_Init` | 103 B | 1/0 | 23 | `SoundSysMicrophone` | absent |
| 4 | `0x4e7c50` | `CameraControlTask_Init` | 64 B | 1/0 | 31 | `CameraControlTask` | absent |
| 5 | `0x5310e0` | `FreeCamera_Init` | 67 B | 1/0 | 32 | `FreeCamera` | absent |
| 6 | `0x530a80` | `FlashTask_Init` | 74 B | 1/0 | 39 | `FlashTask` | absent |
| 7 | `0x4ed510` | `InputPort_Init` | 71 B | 1/0 | 46 | `InputPort` | absent |
| 8 | `0x51a1d0` | `SplinePathDynCubeObj_Init` | 96 B | 1/0 | 117 | `SplinePathDynCubeObj` | absent |
| 9 | `0x491db0` | `Display_SetGammaRamp_4` | 6 B | 1/0 | - | - | absent |
| 10 | `0x491dc0` | `Display_CreateSurface_4` | 6 B | 1/0 | - | - | absent |
| 11 | `0x491dd0` | `Display_OpenDevice_5` | 6 B | 1/0 | - | - | absent |
| 12 | `0x491de0` | `Display_CloseDevice_5` | 6 B | 1/0 | - | - | absent |
| 13 | `0x491df0` | `Display_SetDisplayMode_5` | 6 B | 1/0 | - | - | absent |
| 14 | `0x491e00` | `Display_SyncVSync_5` | 6 B | 1/0 | - | - | absent |
| 15 | `0x491e10` | `Display_SwapBuffers_5` | 6 B | 1/0 | - | - | absent |
| 16 | `0x491e20` | `Display_ClearSurfaces_5` | 6 B | 1/0 | - | - | absent |
| 17 | `0x491e30` | `Display_RestoreDevice_5` | 6 B | 1/0 | - | - | absent |
| 18 | `0x491e40` | `Display_CheckCapabilities_5` | 6 B | 1/0 | - | - | absent |
| 19 | `0x491e50` | `Display_SetGammaRamp_5` | 6 B | 1/0 | - | - | absent |
| 20 | `0x491e60` | `Display_CreateSurface_5` | 6 B | 1/0 | - | - | absent |

Position-1 id coverage: ids 0 `QTask` (`QTask_Init` `0x004010D0`, `task_types.cpp`) and 45 `CamTask` (`CamTask_Init` `0x00530220`, `cam_task.cpp`) are already landed; ids 2, 3, 23, 31, 32, 39, 46, 117 are delivered here; the remaining 113 ids belong to registrars that call unlanded natives and are blocked (section 4). All eight registrars below are **unconditional** call sites inside the position-1 subtree, so these are observed ids, not inferred ones.

## 2. Bodies (paste-ready: provenance block then body)

Each block is a complete file. `NATIVE_DECL` is not used: like `engine/src/retail/core/native_489560.cpp`, the functions sit in an `extern "C"` block and the file defines the retail globals it owns as `extern "C"` objects carrying their retail VAs. Strings are byte-identical to the retail `.rdata` bytes recorded in the evidence line.

### 1. `0x004E5370` - `DynCubeObj_Init` (observed id 2: `DynCubeObj`)

- size 48 B; BN extent `0x004E5370`-`0x004E539F`; 1 block, 0 edges (BN and r2 agree; the linear sweep ends at the `ret`).
- BN definition: `int32_t()`
- BN pseudocode:

```c
int32_t DynCubeObj_Init()

{
    int32_t var_4 = 0;
    g_54875c = TaskType_Register("DynCubeObj", 0x68, g_548638, 0);
    char result = QTask_UnableToAllocateEven(0x181);
    g_54875e = result;
    return result;
}
```

- r2 disassembly (bytes | opcode), clipped to the BN extent:

```
004e5370 66a138865400         mov ax, word [0x548638]
004e5376 6a00                 push 0
004e5378 6a00                 push 0
004e537a 50                   push eax
004e537b 6a68                 push 0x68
004e537d 6860875400           push 0x548760
004e5382 e879c5f1ff           call 0x401900
004e5387 6881010000           push 0x181
004e538c 66a35c875400         mov word [0x54875c], ax
004e5392 e829c4f1ff           call 0x4017c0
004e5397 83c418               add esp, 0x18
004e539a a25e875400           mov byte [0x54875e], al
004e539f c3                   ret
```

- body:

```cpp
// Native: DynCubeObj_Init_4E5370 (placeholder until VERIFIED); retail VA 0x004E5370-0x004E539F (48 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 a037d19f3e9618e40d1da0fbd9226e3ee048d6ad2becee76eef358303dcaf2d1.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (66 a1 38 86 54 00 ...); calls 0x00401900, 0x004017C0; string "DynCubeObj" at 0x00548760.
// Behavior/CFG: reads the 16-bit parent at 0x00548638, registers "DynCubeObj" (0x68 bytes) -> observed class id 2, stores the id word at 0x0054875C, runs QTask_AllocateEvent(0x181), stores its AL result at 0x0054875E and returns that result in EAX. Single block, no branches.
// Status/limits: TRANSCRIBED (static only); runtime UNOBSERVED in the port; observed id 2 in the position-1 capture. ABI: retail loads the parent with `mov ax,[0x548638]`, so the pushed EAX upper bits are caller residue; the port passes a 16-bit value, which is all TaskType_Register stores.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" {
std::uint16_t g_548638;  // retail 0x00548638: current parent class id (writer is outside this batch)
std::uint16_t g_54875c;  // retail 0x0054875C: DynCubeObj class id, written here
std::uint8_t g_54875e;   // retail 0x0054875E: QTask_AllocateEvent result, written here

std::int32_t DynCubeObj_Init_4E5370()
{
    g_54875c = TaskType_Register("DynCubeObj", 0x68U, g_548638, 0U); // 0x004E537D: push 0x548760; push 0x68; push eax; push 0; push 0
    std::uint8_t const event = QTask_AllocateEvent(0x181U);          // 0x004E5387: push 0x181; call 0x4017C0
    g_54875e = event;                                                // 0x004E539A: mov byte [0x54875E], al
    return static_cast<std::int32_t>(event);                         // 0x004E539F: ret (EAX = allocate result)
}
} // extern "C"
```

### 2. `0x004DA1D0` - `Hazinglayers_Init` (observed id 3: `Hazinglayers`)

- size 108 B; BN extent `0x004DA1D0`-`0x004DA23B`; 1 block, 0 edges (BN and r2 agree; the linear sweep ends at the `ret`).
- BN definition: `int32_t()`
- BN pseudocode:

```c
int32_t Hazinglayers_Init()

{
    int32_t var_4 = 0;
    int16_t eax_1 = TaskType_Register("Hazinglayers", 0x90, g_548638, 0);
    g_a5e304 = eax_1;
    QTask_RemoveEventCallback(eax_1, 0x11, PhysicsRigid_GetContext_4DA240);
    QTask_RemoveEventCallback(g_a5e304, 0x12, PhysicsRigid_DefaultHandler_4DA2A0);
    QTask_RemoveEventCallback(g_a5e304, 4, PhysicsRigid_ProcessStep_4DA270);
    return QTask_RemoveEventCallback(g_a5e304, 5, PhysicsRigid_Invoke2);
}
```

- r2 disassembly (bytes | opcode), clipped to the BN extent:

```
004da1d0 66a138865400         mov ax, word [0x548638]
004da1d6 6a00                 push 0
004da1d8 6a00                 push 0
004da1da 50                   push eax
004da1db 6890000000           push 0x90
004da1e0 686c865400           push 0x54866c
004da1e5 e81677f2ff           call 0x401900
004da1ea 6840a24d00           push 0x4da240
004da1ef 6a11                 push 0x11
004da1f1 50                   push eax
004da1f2 66a304e3a500         mov word [0xa5e304], ax
004da1f8 e80372f2ff           call 0x401400
004da1fd 668b0d04e3a500       mov cx, word [0xa5e304]
004da204 68a0a24d00           push 0x4da2a0
004da209 6a12                 push 0x12
004da20b 51                   push ecx
004da20c e8ef71f2ff           call 0x401400
004da211 668b1504e3a500       mov dx, word [0xa5e304]
004da218 6870a24d00           push 0x4da270
004da21d 6a04                 push 4
004da21f 52                   push edx
004da220 e8db71f2ff           call 0x401400
004da225 66a104e3a500         mov ax, word [0xa5e304]
004da22b 68c0a24d00           push 0x4da2c0
004da230 6a05                 push 5
004da232 50                   push eax
004da233 e8c871f2ff           call 0x401400
004da238 83c444               add esp, 0x44
004da23b c3                   ret
```

- body:

```cpp
// Native: Hazinglayers_Init_4DA1D0 (placeholder until VERIFIED); retail VA 0x004DA1D0-0x004DA23B (108 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 82a328c5d6e36ab15015e1b44e3fb02f7277dbde4343885d257b6b739132a1fc.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (66 a1 38 86 54 00 ...); calls 0x00401900 and 0x00401400 (x4); string "Hazinglayers" at 0x0054866C.
// Behavior/CFG: registers "Hazinglayers" (0x90 bytes) with the 16-bit parent at 0x00548638 -> observed id 3, stores the id at 0x005AE304, then links four phase handlers (0x11/0x12/4/5 -> 0x004DA240/0x004DA2A0/0x004DA270/0x004DA2C0) through 0x00401400 and returns the last link result. Single block.
// Status/limits: TRANSCRIBED (static only); the four handlers are not transcribed yet, so their addresses are passed as retail-VA literals (the exact retail push immediates); runtime UNOBSERVED in the port.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" {
std::uint16_t g_548638;  // retail 0x00548638: current parent class id (writer is outside this batch)
std::uint16_t g_a5e304;  // retail 0x005AE304: Hazinglayers class id, written here

std::int32_t Hazinglayers_Init_4DA1D0()
{
    g_a5e304 = TaskType_Register("Hazinglayers", 0x90U, g_548638, 0U);  // 0x004DA1E5: call 0x401900
    QTask_LinkQTaskEventHandler(g_a5e304, 0x11U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004DA240)); // 0x004DA1F8: push 0x4DA240; push 0x11; push eax
    QTask_LinkQTaskEventHandler(g_a5e304, 0x12U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004DA2A0)); // 0x004DA20C
    QTask_LinkQTaskEventHandler(g_a5e304, 4U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004DA270));    // 0x004DA220
    return static_cast<std::int32_t>(QTask_LinkQTaskEventHandler(g_a5e304, 5U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004DA2C0))); // 0x004DA233
}
} // extern "C"
```

### 3. `0x004EBC60` - `SoundSysMicrophone_Init` (observed id 23: `SoundSysMicrophone`)

- size 103 B; BN extent `0x004EBC60`-`0x004EBCC6`; 1 block, 0 edges (BN and r2 agree; the linear sweep ends at the `ret`).
- BN definition: `int32_t()`
- BN pseudocode:

```c
int32_t SoundSysMicrophone_Init()

{
    int32_t var_4 = 0;
    g_54db68 = TaskType_Register("SoundSysMicrophone", 0x68, QTask_OpenEvent_2(), 0);
    g_54db6a = QTask_UnableToAllocateEven(0);
    QTask_RemoveEventCallback(g_54db68, 0, AIGraph_LoadParameters);
    QTask_LinkQTaskEventHandlerNoContextFn(g_54db68, 1, SoundSysMicrophone_Setup);
    return QTask_LinkQTaskEventHandlerNoContextFn(g_54db68, 2, AIGraph_LoadParameters);
}
```

- r2 disassembly (bytes | opcode), clipped to the BN extent:

```
004ebc60 6a00                 push 0
004ebc62 6a00                 push 0
004ebc64 e8875cf1ff           call 0x4018f0
004ebc69 50                   push eax
004ebc6a 6a68                 push 0x68
004ebc6c 686cdb5400           push 0x54db6c
004ebc71 e88a5cf1ff           call 0x401900
004ebc76 6a00                 push 0
004ebc78 66a368db5400         mov word [0x54db68], ax
004ebc7e e83d5bf1ff           call 0x4017c0
004ebc83 a26adb5400           mov byte [0x54db6a], al
004ebc88 66a168db5400         mov ax, word [0x54db68]
004ebc8e 6810374d00           push 0x4d3710
004ebc93 6a00                 push 0
004ebc95 50                   push eax
004ebc96 e86557f1ff           call 0x401400
004ebc9b 668b0d68db5400       mov cx, word [0x54db68]
004ebca2 68d0bc4e00           push 0x4ebcd0
004ebca7 6a01                 push 1
004ebca9 51                   push ecx
004ebcaa e88158f1ff           call 0x401530
004ebcaf 668b1568db5400       mov dx, word [0x54db68]
004ebcb6 6810374d00           push 0x4d3710
004ebcbb 6a02                 push 2
004ebcbd 52                   push edx
004ebcbe e86d58f1ff           call 0x401530
004ebcc3 83c43c               add esp, 0x3c
004ebcc6 c3                   ret
```

- body:

```cpp
// Native: SoundSysMicrophone_Init_4EBC60 (placeholder until VERIFIED); retail VA 0x004EBC60-0x004EBCC6 (103 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 0b19afa8e3bc59f6740af55af2f060aa44a811ee2e679429f9adfc40e22b64e3.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (6a 00 6a 00 e8 ...); calls 0x004018F0, 0x00401900, 0x004017C0, 0x00401400, 0x00401530 (x2); string "SoundSysMicrophone" at 0x0054DB6C.
// Behavior/CFG: parent = QTask_GetCurrentType(); registers "SoundSysMicrophone" (0x68 bytes) -> observed id 23, stores the id at 0x0054DB68, stores QTask_AllocateEvent(0) at 0x0054DB6A, links phase 0 (0x004D3710) through 0x00401400 and phases 1/2 (0x004EBCD0, 0x004D3710) through 0x00401530, returning the last result. Single block.
// Status/limits: TRANSCRIBED (static only); handler natives 0x004D3710 and 0x004EBCD0 remain untranscribed and are passed as retail-VA literals; runtime UNOBSERVED in the port.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" {
std::uint16_t g_54db68;  // retail 0x0054DB68: SoundSysMicrophone class id, written here
std::uint8_t  g_54db6a;  // retail 0x0054DB6A: QTask_AllocateEvent result, written here

std::int32_t SoundSysMicrophone_Init_4EBC60()
{
    std::uint16_t const parent = QTask_GetCurrentType();                   // 0x004EBC64: call 0x4018F0
    g_54db68 = TaskType_Register("SoundSysMicrophone", 0x68U, parent, 0U); // 0x004EBC71: call 0x401900
    g_54db6a = QTask_AllocateEvent(0U);                                    // 0x004EBC7E: call 0x4017C0
    QTask_LinkQTaskEventHandler(g_54db68, 0U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004D3710)); // 0x004EBC96
    QTask_LinkQTaskEventHandlerNoContextFn(g_54db68, 1U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004EBCD0)); // 0x004EBCAA
    return static_cast<std::int32_t>(QTask_LinkQTaskEventHandlerNoContextFn(g_54db68, 2U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004D3710))); // 0x004EBCBE
}
} // extern "C"
```

### 4. `0x004E7C50` - `CameraControlTask_Init` (observed id 31: `CameraControlTask`)

- size 64 B; BN extent `0x004E7C50`-`0x004E7C8F`; 1 block, 0 edges (BN and r2 agree; the linear sweep ends at the `ret`).
- BN definition: `int32_t()`
- BN pseudocode:

```c
int32_t CameraControlTask_Init()

{
    int32_t var_4 = 0;
    uint16_t event_id = TaskType_Register("CameraControlTask", 0x2c, QTask_OpenEvent_2(), 0);
    g_54d974 = event_id;
    QTask_LinkQTaskEventHandlerNoContextFn(event_id, 1, CameraControlTask_LoadParameters);
    return QTask_LinkQTaskEventHandlerNoContextFn(g_54d974, 2, AIGraph_LoadParameters);
}
```

- r2 disassembly (bytes | opcode), clipped to the BN extent:

```
004e7c50 6a00                 push 0
004e7c52 6a00                 push 0
004e7c54 e8979cf1ff           call 0x4018f0
004e7c59 50                   push eax
004e7c5a 6a2c                 push 0x2c
004e7c5c 6878d95400           push 0x54d978
004e7c61 e89a9cf1ff           call 0x401900
004e7c66 68907c4e00           push 0x4e7c90
004e7c6b 6a01                 push 1
004e7c6d 50                   push eax
004e7c6e 66a374d95400         mov word [0x54d974], ax
004e7c74 e8b798f1ff           call 0x401530
004e7c79 66a174d95400         mov ax, word [0x54d974]
004e7c7f 6810374d00           push 0x4d3710
004e7c84 6a02                 push 2
004e7c86 50                   push eax
004e7c87 e8a498f1ff           call 0x401530
004e7c8c 83c42c               add esp, 0x2c
004e7c8f c3                   ret
```

- body:

```cpp
// Native: CameraControlTask_Init_4E7C50 (placeholder until VERIFIED); retail VA 0x004E7C50-0x004E7C8F (64 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 b3bae5933dbb03a42cb443672f3a8d85f65eca84fa790540003b7890a51eba74.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (6a 00 6a 00 e8 97 9c f1 ff ...); calls 0x004018F0, 0x00401900, 0x00401530 (x2); string "CameraControlTask" at 0x0054D978.
// Behavior/CFG: parent = QTask_GetCurrentType(); registers "CameraControlTask" (0x2C bytes) -> observed id 31, stores the id at 0x0054D974, links phase 1 (0x004E7C90) and phase 2 (0x004D3710) through 0x00401530, returns the last result. Single block.
// Status/limits: TRANSCRIBED (static only); the two handlers are passed as retail-VA literals until transcribed; runtime UNOBSERVED in the port.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" {
std::uint16_t g_54d974;  // retail 0x0054D974: CameraControlTask class id, written here

std::int32_t CameraControlTask_Init_4E7C50()
{
    std::uint16_t const parent = QTask_GetCurrentType();                  // 0x004E7C54: call 0x4018F0
    g_54d974 = TaskType_Register("CameraControlTask", 0x2CU, parent, 0U); // 0x004E7C61: call 0x401900
    QTask_LinkQTaskEventHandlerNoContextFn(g_54d974, 1U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004E7C90)); // 0x004E7C74
    return static_cast<std::int32_t>(QTask_LinkQTaskEventHandlerNoContextFn(g_54d974, 2U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004D3710))); // 0x004E7C87
}
} // extern "C"
```

### 5. `0x005310E0` - `FreeCamera_Init` (observed id 32: `FreeCamera`)

- size 67 B; BN extent `0x005310E0`-`0x00531122`; 1 block, 0 edges (BN and r2 agree; the linear sweep ends at the `ret`).
- BN definition: `int32_t()`
- BN pseudocode:

```c
int32_t FreeCamera_Init()

{
    int32_t var_4 = 0;
    uint16_t event_id = TaskType_Register("FreeCamera", 0x90, QTask_OpenEvent_2(), 0);
    g_54f7d0 = event_id;
    QTask_LinkQTaskEventHandlerNoContextFn(event_id, 1, FreeCamera_LoadParameters);
    return QTask_LinkQTaskEventHandlerNoContextFn(g_54f7d0, 2, AIGraph_LoadParameters);
}
```

- r2 disassembly (bytes | opcode), clipped to the BN extent:

```
005310e0 6a00                 push 0
005310e2 6a00                 push 0
005310e4 e80708edff           call 0x4018f0
005310e9 50                   push eax
005310ea 6890000000           push 0x90
005310ef 68d4f75400           push 0x54f7d4
005310f4 e80708edff           call 0x401900
005310f9 6830115300           push 0x531130
005310fe 6a01                 push 1
00531100 50                   push eax
00531101 66a3d0f75400         mov word [0x54f7d0], ax
00531107 e82404edff           call 0x401530
0053110c 66a1d0f75400         mov ax, word [0x54f7d0]
00531112 6810374d00           push 0x4d3710
00531117 6a02                 push 2
00531119 50                   push eax
0053111a e81104edff           call 0x401530
0053111f 83c42c               add esp, 0x2c
00531122 c3                   ret
```

- body:

```cpp
// Native: FreeCamera_Init_5310E0 (placeholder until VERIFIED); retail VA 0x005310E0-0x00531122 (67 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 099189b718a252a62b5cbe0a2d2664e7f55ddcf09f20bca452431bd2fb36f0e7.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (6a 00 6a 00 e8 07 08 ed ff ...); calls 0x004018F0, 0x00401900, 0x00401530 (x2); string "FreeCamera" at 0x0054F7D4.
// Behavior/CFG: parent = QTask_GetCurrentType(); registers "FreeCamera" (0x90 bytes) -> observed id 32, stores the id at 0x0054F7D0, links phase 1 (0x00531130) and phase 2 (0x004D3710) through 0x00401530, returns the last result. Single block.
// Status/limits: TRANSCRIBED (static only); handlers passed as retail-VA literals until transcribed; runtime UNOBSERVED in the port.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" {
std::uint16_t g_54f7d0;  // retail 0x0054F7D0: FreeCamera class id, written here

std::int32_t FreeCamera_Init_5310E0()
{
    std::uint16_t const parent = QTask_GetCurrentType();           // 0x005310E4: call 0x4018F0
    g_54f7d0 = TaskType_Register("FreeCamera", 0x90U, parent, 0U); // 0x005310F4: call 0x401900
    QTask_LinkQTaskEventHandlerNoContextFn(g_54f7d0, 1U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00531130)); // 0x00531107
    return static_cast<std::int32_t>(QTask_LinkQTaskEventHandlerNoContextFn(g_54f7d0, 2U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004D3710))); // 0x0053111A
}
} // extern "C"
```

### 6. `0x00530A80` - `FlashTask_Init` (observed id 39: `FlashTask`)

- size 74 B; BN extent `0x00530A80`-`0x00530AC9`; 1 block, 0 edges (BN and r2 agree; the linear sweep ends at the `ret`).
- BN definition: `int32_t()`
- BN pseudocode:

```c
int32_t FlashTask_Init()

{
    int32_t var_4 = 0;
    uint16_t event_id = TaskType_Register("FlashTask", 0x88, QTask_OpenEvent_2(), 2);
    g_54f7a0 = event_id;
    QTask_LinkQTaskEventHandlerNoContextFn(event_id, 2, RenderLight_InvokeAction);
    int32_t result = QTask_LinkQTaskEventHandlerNoContextFn(g_54f7a0, 1, FlashTask_LoadParameters);
    g_a845ec = 1;
    return result;
}
```

- r2 disassembly (bytes | opcode), clipped to the BN extent:

```
00530a80 6a00                 push 0
00530a82 6a02                 push 2
00530a84 e8670eedff           call 0x4018f0
00530a89 50                   push eax
00530a8a 6888000000           push 0x88
00530a8f 68a4f75400           push 0x54f7a4
00530a94 e8670eedff           call 0x401900
00530a99 68400c5300           push 0x530c40
00530a9e 6a02                 push 2
00530aa0 50                   push eax
00530aa1 66a3a0f75400         mov word [0x54f7a0], ax
00530aa7 e8840aedff           call 0x401530
00530aac 66a1a0f75400         mov ax, word [0x54f7a0]
00530ab2 68d00a5300           push 0x530ad0
00530ab7 6a01                 push 1
00530ab9 50                   push eax
00530aba e8710aedff           call 0x401530
00530abf 83c42c               add esp, 0x2c
00530ac2 c605ec45a80001       mov byte [0xa845ec], 1
00530ac9 c3                   ret
```

- body:

```cpp
// Native: FlashTask_Init_530A80 (placeholder until VERIFIED); retail VA 0x00530A80-0x00530AC9 (74 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 f786e2e9796327e4514f9d1ff77809332bcc898f9e09dc8064f084d27b032ade.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (6a 00 6a 02 e8 67 0e ed ff ...); calls 0x004018F0, 0x00401900, 0x00401530 (x2); string "FlashTask" at 0x0054F7A4; written byte 0x00A845EC.
// Behavior/CFG: parent = QTask_GetCurrentType(); registers "FlashTask" (0x88 bytes) with task-list argument 2 -> observed id 39, stores the id at 0x0054F7A0, links phase 2 (0x00530C40) then phase 1 (0x00530AD0), sets the byte at 0x00A845EC to 1, returns the last link result. Single block.
// Status/limits: TRANSCRIBED (static only); the non-zero 4th TaskType_Register argument selects the retail task-list build path at 0x004B0B00 that the port does not transcribe yet (recorded in task_types.hpp limits); handlers passed as retail-VA literals; runtime UNOBSERVED in the port.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" {
std::uint16_t g_54f7a0;  // retail 0x0054F7A0: FlashTask class id, written here
std::uint8_t  g_a845ec;  // retail 0x00A845EC: flag byte set to 1 here

std::int32_t FlashTask_Init_530A80()
{
    std::uint16_t const parent = QTask_GetCurrentType();          // 0x00530A84: call 0x4018F0
    g_54f7a0 = TaskType_Register("FlashTask", 0x88U, parent, 2U); // 0x00530A94: call 0x401900 (task-list argument 2)
    QTask_LinkQTaskEventHandlerNoContextFn(g_54f7a0, 2U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00530C40)); // 0x00530AA7
    std::int32_t const result = static_cast<std::int32_t>(QTask_LinkQTaskEventHandlerNoContextFn(g_54f7a0, 1U, reinterpret_cast<igi::retail::core::FrameHandler>(0x00530AD0))); // 0x00530ABA
    g_a845ec = 1U;                                                // 0x00530AC2: mov byte [0xA845EC], 1
    return result;                                                // 0x00530AC9: ret
}
} // extern "C"
```

### 7. `0x004ED510` - `InputPort_Init` (observed id 46: `InputPort`)

- size 71 B; BN extent `0x004ED510`-`0x004ED556`; 1 block, 0 edges (BN and r2 agree; the linear sweep ends at the `ret`).
- BN definition: `int32_t()`
- BN pseudocode:

```c
int32_t InputPort_Init()

{
    int32_t var_8 = 0;
    uint16_t event_id = TaskType_Register("InputPort", 0x20, QTask_OpenEvent_2(), 0);
    g_a5efa0 = event_id;
    int32_t result = QTask_LinkQTaskEventHandlerNoContextFn(event_id, 1, InputPort_LoadParameters);
    g_bc20a0 = 0;
    g_bc2150 = 0;
    g_bc2200 = 0;
    g_bc22b0 = 0;
    return result;
}
```

- r2 disassembly (bytes | opcode), clipped to the BN extent:

```
004ed510 56                   push esi
004ed511 33f6                 xor esi, esi
004ed513 56                   push esi
004ed514 56                   push esi
004ed515 e8d643f1ff           call 0x4018f0
004ed51a 50                   push eax
004ed51b 6a20                 push 0x20
004ed51d 68d0db5400           push 0x54dbd0
004ed522 e8d943f1ff           call 0x401900
004ed527 6860d54e00           push 0x4ed560
004ed52c 6a01                 push 1
004ed52e 50                   push eax
004ed52f 66a3a0efa500         mov word [0xa5efa0], ax
004ed535 e8f63ff1ff           call 0x401530
004ed53a 83c420               add esp, 0x20
004ed53d 8935a020bc00         mov dword [0xbc20a0], esi
004ed543 89355021bc00         mov dword [0xbc2150], esi
004ed549 89350022bc00         mov dword [0xbc2200], esi
004ed54f 8935b022bc00         mov dword [0xbc22b0], esi
004ed555 5e                   pop esi
004ed556 c3                   ret
```

- body:

```cpp
// Native: InputPort_Init_4ED510 (placeholder until VERIFIED); retail VA 0x004ED510-0x004ED556 (71 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 73ecb764d01f316904b4b523f529c2df0a70e20fc986fd9f954bd68f2d7a93da.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (56 33 f6 56 56 e8 ...); calls 0x004018F0, 0x00401900, 0x00401530; string "InputPort" at 0x0054DBD0.
// Behavior/CFG: zeroes ESI, parent = QTask_GetCurrentType(), registers "InputPort" (0x20 bytes) -> observed id 46, stores the id at 0x00A5EFA0, links phase 1 (0x004ED560) through 0x00401530, then zeroes the four dwords at 0x00BC20A0/0x00BC2150/0x00BC2200/0x00BC22B0, and returns the link result. Single block.
// Status/limits: TRANSCRIBED (static only); handler 0x004ED560 passed as a retail-VA literal; runtime UNOBSERVED in the port.
#include "igi/retail/core/task_types.hpp"

#include <cstdint>

extern "C" {
std::uint16_t g_a5efa0;    // retail 0x00A5EFA0: InputPort class id, written here
std::uint32_t g_bc20a0;    // retail 0x00BC20A0: zeroed here
std::uint32_t g_bc2150;    // retail 0x00BC2150: zeroed here
std::uint32_t g_bc2200;    // retail 0x00BC2200: zeroed here
std::uint32_t g_bc22b0;    // retail 0x00BC22B0: zeroed here

std::int32_t InputPort_Init_4ED510()
{
    std::uint32_t const zero = 0U;                                        // 0x004ED511: xor esi,esi
    std::uint16_t const parent = QTask_GetCurrentType();                  // 0x004ED515: call 0x4018F0
    g_a5efa0 = TaskType_Register("InputPort", 0x20U, parent, 0U);         // 0x004ED522: call 0x401900
    std::int32_t const result = static_cast<std::int32_t>(QTask_LinkQTaskEventHandlerNoContextFn(g_a5efa0, 1U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004ED560))); // 0x004ED535
    g_bc20a0 = zero;                                                      // 0x004ED53D: mov [0xBC20A0], esi
    g_bc2150 = zero;                                                      // 0x004ED543
    g_bc2200 = zero;                                                      // 0x004ED549
    g_bc22b0 = zero;                                                      // 0x004ED54F
    return result;                                                        // 0x004ED555: pop esi; ret
}
} // extern "C"
```

### 8. `0x0051A1D0` - `SplinePathDynCubeObj_Init` (observed id 117: `SplinePathDynCubeObj`)

- size 96 B; BN extent `0x0051A1D0`-`0x0051A22F`; 1 block, 0 edges (BN and r2 agree; the linear sweep ends at the `ret`).
- BN definition: `int32_t()`
- BN pseudocode:

```c
int32_t SplinePathDynCubeObj_Init()

{
    int32_t var_4 = 0;
    int16_t eax_1 = TaskType_Register("SplinePathDynCubeObj", 0x200, g_54875c, 0);
    g_54ed4e = eax_1;
    QTask_RemoveEventCallback(eax_1, 0, WeatherWaterSky_DefaultHandler_51A240);
    QTask_LinkQTaskEventHandlerNoContextFn(g_54ed4e, 2, AIGraph_LoadParameters);
    int32_t eax_2 = Script_ParseEnumInt32_8();
    return QTask_RemoveEventCallback(g_54ed4e, eax_2, WeatherWaterSky_GetContext_51A230);
}
```

- r2 disassembly (bytes | opcode), clipped to the BN extent:

```
0051a1d0 66a15c875400         mov ax, word [0x54875c]
0051a1d6 6a00                 push 0
0051a1d8 6a00                 push 0
0051a1da 50                   push eax
0051a1db 6800020000           push 0x200
0051a1e0 68a4ed5400           push 0x54eda4
0051a1e5 e81677eeff           call 0x401900
0051a1ea 6840a25100           push 0x51a240
0051a1ef 6a00                 push 0
0051a1f1 50                   push eax
0051a1f2 66a34eed5400         mov word [0x54ed4e], ax
0051a1f8 e80372eeff           call 0x401400
0051a1fd 668b0d4eed5400       mov cx, word [0x54ed4e]
0051a204 6810374d00           push 0x4d3710
0051a209 6a02                 push 2
0051a20b 51                   push ecx
0051a20c e81f73eeff           call 0x401530
0051a211 83c42c               add esp, 0x2c
0051a214 6830a25100           push 0x51a230
0051a219 e84278fdff           call 0x4f1a60
0051a21e 668b154eed5400       mov dx, word [0x54ed4e]
0051a225 50                   push eax
0051a226 52                   push edx
0051a227 e8d471eeff           call 0x401400
0051a22c 83c40c               add esp, 0xc
0051a22f c3                   ret
```

- body:

```cpp
// Native: SplinePathDynCubeObj_Init_51A1D0 (placeholder until VERIFIED); retail VA 0x0051A1D0-0x0051A22F (96 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 565a08dd85a44918b2a3615962c144408431c2c43bb1256e276ef27ed1aa028b.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (66 a1 5c 87 54 00 ...); calls 0x00401900, 0x00401400 (x2), 0x00401530, 0x004F1A60; string "SplinePathDynCubeObj" at 0x0054EDA4.
// Behavior/CFG: parent = word at 0x0054875C (the DynCubeObj id written by 0x004E5370); registers "SplinePathDynCubeObj" (0x200 bytes) -> observed id 117, stores the id at 0x0054ED4E, links phase 0 (0x0051A240) through 0x00401400, phase 2 (0x004D3710) through 0x00401530, reads the context byte through 0x004F1A60, and links that context to 0x0051A230 through 0x00401400, returning the last result. Single block.
// Status/limits: TRANSCRIBED (static only); handlers 0x0051A240/0x0051A230 are passed as retail-VA literals; the parent word is defined by DynCubeObj_Init_4E5370 in this batch; runtime UNOBSERVED in the port.
#include "igi/retail/core/task_types.hpp"
#include "igi/retail/script/script_state.hpp"

#include <cstdint>

extern "C" {
std::uint16_t g_54875c;  // retail 0x0054875C: DynCubeObj class id, defined by DynCubeObj_Init_4E5370
std::uint16_t g_54ed4e;  // retail 0x0054ED4E: SplinePathDynCubeObj class id, written here

std::int32_t SplinePathDynCubeObj_Init_51A1D0()
{
    g_54ed4e = TaskType_Register("SplinePathDynCubeObj", 0x200U, g_54875c, 0U); // 0x0051A1E5: call 0x401900
    QTask_LinkQTaskEventHandler(g_54ed4e, 0U, reinterpret_cast<igi::retail::core::FrameHandler>(0x0051A240)); // 0x0051A1F8
    QTask_LinkQTaskEventHandlerNoContextFn(g_54ed4e, 2U, reinterpret_cast<igi::retail::core::FrameHandler>(0x004D3710)); // 0x0051A20C
    std::uint32_t const context = Script_Get_4F1A60();                        // 0x0051A219: call 0x4F1A60
    return static_cast<std::int32_t>(QTask_LinkQTaskEventHandler(g_54ed4e, context, reinterpret_cast<igi::retail::core::FrameHandler>(0x0051A230))); // tail link
}
} // extern "C"
```

### 9. `0x00491DB0` - `Display_SetGammaRamp_4` (no registration)

- size 6 B; BN extent `0x00491DB0`-`0x00491DB5`; 1 block, 0 edges (BN and r2 agree; the linear sweep ends at the `ret`).
- BN definition: `int32_t()`
- BN pseudocode:

```c
int32_t Display_SetGammaRamp_4()

{
    return g_542340;
}
```

- r2 disassembly (bytes | opcode), clipped to the BN extent:

```
00491db0 a140235400           mov eax, dword [0x542340]
00491db5 c3                   ret
```

- body:

```cpp
// Native: Display_SetGammaRamp_4_491DB0 (placeholder until VERIFIED); retail VA 0x00491DB0-0x00491DB5 (6 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 e10d4c10bd579beed103b274bd84efc384ddf16a1fe90b86908971e28a91b7b1.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (a1 40235400 c3); image dword at 0x00542340 holds 0xFFFFFFFF.
// Behavior/CFG: mov eax, dword [0x00542340]; ret - single block, no callees, returns the .rdata slot value.
// Status/limits: TRANSCRIBED (static only); reads an image constant slot, so no engine global and no runtime write is involved; runtime UNOBSERVED in the port.
#include <cstdint>

extern "C" {

std::int32_t Display_SetGammaRamp_4_491DB0()
{
    return *reinterpret_cast<std::int32_t const*>(0x00542340); // a1 40235400 c3
}
} // extern "C"
```

### 10. `0x00491DC0` - `Display_CreateSurface_4` (no registration)

- size 6 B; BN extent `0x00491DC0`-`0x00491DC5`; 1 block, 0 edges (BN and r2 agree; the linear sweep ends at the `ret`).
- BN definition: `int32_t()`
- BN pseudocode:

```c
int32_t Display_CreateSurface_4()

{
    return g_542338;
}
```

- r2 disassembly (bytes | opcode), clipped to the BN extent:

```
00491dc0 a138235400           mov eax, dword [0x542338]
00491dc5 c3                   ret
```

- body:

```cpp
// Native: Display_CreateSurface_4_491DC0 (placeholder until VERIFIED); retail VA 0x00491DC0-0x00491DC5 (6 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 89c203bdae46768199c6ca35f3aff0a0616e1aaf88fac80821173a95e10c9fcd.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (a1 38235400 c3); image dword at 0x00542338 holds 0xFFFFFFFF.
// Behavior/CFG: mov eax, dword [0x00542338]; ret - single block, no callees, returns the .rdata slot value.
// Status/limits: TRANSCRIBED (static only); reads an image constant slot, so no engine global and no runtime write is involved; runtime UNOBSERVED in the port.
#include <cstdint>

extern "C" {

std::int32_t Display_CreateSurface_4_491DC0()
{
    return *reinterpret_cast<std::int32_t const*>(0x00542338); // a1 38235400 c3
}
} // extern "C"
```

### 11. `0x00491DD0` - `Display_OpenDevice_5` (no registration)

- size 6 B; BN extent `0x00491DD0`-`0x00491DD5`; 1 block, 0 edges (BN and r2 agree; the linear sweep ends at the `ret`).
- BN definition: `int32_t()`
- BN pseudocode:

```c
int32_t Display_OpenDevice_5()

{
    return g_54233c;
}
```

- r2 disassembly (bytes | opcode), clipped to the BN extent:

```
00491dd0 a13c235400           mov eax, dword [0x54233c]
00491dd5 c3                   ret
```

- body:

```cpp
// Native: Display_OpenDevice_5_491DD0 (placeholder until VERIFIED); retail VA 0x00491DD0-0x00491DD5 (6 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 d4e64415b20f7894d93a20d75b489320b2fa3f2c0255b7e05c25ca9ec44747e7.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (a1 3c235400 c3); image dword at 0x0054233C holds 0xFFFFFFFF.
// Behavior/CFG: mov eax, dword [0x0054233C]; ret - single block, no callees, returns the .rdata slot value.
// Status/limits: TRANSCRIBED (static only); reads an image constant slot, so no engine global and no runtime write is involved; runtime UNOBSERVED in the port.
#include <cstdint>

extern "C" {

std::int32_t Display_OpenDevice_5_491DD0()
{
    return *reinterpret_cast<std::int32_t const*>(0x0054233C); // a1 3c235400 c3
}
} // extern "C"
```

### 12. `0x00491DE0` - `Display_CloseDevice_5` (no registration)

- size 6 B; BN extent `0x00491DE0`-`0x00491DE5`; 1 block, 0 edges (BN and r2 agree; the linear sweep ends at the `ret`).
- BN definition: `int32_t()`
- BN pseudocode:

```c
int32_t Display_CloseDevice_5()

{
    return g_542310;
}
```

- r2 disassembly (bytes | opcode), clipped to the BN extent:

```
00491de0 a110235400           mov eax, dword [0x542310]
00491de5 c3                   ret
```

- body:

```cpp
// Native: Display_CloseDevice_5_491DE0 (placeholder until VERIFIED); retail VA 0x00491DE0-0x00491DE5 (6 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 c2e1a0056243a3f0ae9be868c4cfb9c06d0929348feff0ba86216c7605eb9fe4.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (a1 10235400 c3); image dword at 0x00542310 holds 0xFFFFFFFF.
// Behavior/CFG: mov eax, dword [0x00542310]; ret - single block, no callees, returns the .rdata slot value.
// Status/limits: TRANSCRIBED (static only); reads an image constant slot, so no engine global and no runtime write is involved; runtime UNOBSERVED in the port.
#include <cstdint>

extern "C" {

std::int32_t Display_CloseDevice_5_491DE0()
{
    return *reinterpret_cast<std::int32_t const*>(0x00542310); // a1 10235400 c3
}
} // extern "C"
```

### 13. `0x00491DF0` - `Display_SetDisplayMode_5` (no registration)

- size 6 B; BN extent `0x00491DF0`-`0x00491DF5`; 1 block, 0 edges (BN and r2 agree; the linear sweep ends at the `ret`).
- BN definition: `int32_t()`
- BN pseudocode:

```c
int32_t Display_SetDisplayMode_5()

{
    return g_542314;
}
```

- r2 disassembly (bytes | opcode), clipped to the BN extent:

```
00491df0 a114235400           mov eax, dword [0x542314]
00491df5 c3                   ret
```

- body:

```cpp
// Native: Display_SetDisplayMode_5_491DF0 (placeholder until VERIFIED); retail VA 0x00491DF0-0x00491DF5 (6 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 4607fc20dd57ef72b64969e9ba2565feb4911798e4deb1545c23e181e647ecfa.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (a1 14235400 c3); image dword at 0x00542314 holds 0xFFFFFFFF.
// Behavior/CFG: mov eax, dword [0x00542314]; ret - single block, no callees, returns the .rdata slot value.
// Status/limits: TRANSCRIBED (static only); reads an image constant slot, so no engine global and no runtime write is involved; runtime UNOBSERVED in the port.
#include <cstdint>

extern "C" {

std::int32_t Display_SetDisplayMode_5_491DF0()
{
    return *reinterpret_cast<std::int32_t const*>(0x00542314); // a1 14235400 c3
}
} // extern "C"
```

### 14. `0x00491E00` - `Display_SyncVSync_5` (no registration)

- size 6 B; BN extent `0x00491E00`-`0x00491E05`; 1 block, 0 edges (BN and r2 agree; the linear sweep ends at the `ret`).
- BN definition: `int32_t()`
- BN pseudocode:

```c
int32_t Display_SyncVSync_5()

{
    return g_542318;
}
```

- r2 disassembly (bytes | opcode), clipped to the BN extent:

```
00491e00 a118235400           mov eax, dword [0x542318]
00491e05 c3                   ret
```

- body:

```cpp
// Native: Display_SyncVSync_5_491E00 (placeholder until VERIFIED); retail VA 0x00491E00-0x00491E05 (6 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 82d2b7bbfa2b365e34e2dc02391666941c6c7101b88196548c31adba59ad37d1.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (a1 18235400 c3); image dword at 0x00542318 holds 0xFFFFFFFF.
// Behavior/CFG: mov eax, dword [0x00542318]; ret - single block, no callees, returns the .rdata slot value.
// Status/limits: TRANSCRIBED (static only); reads an image constant slot, so no engine global and no runtime write is involved; runtime UNOBSERVED in the port.
#include <cstdint>

extern "C" {

std::int32_t Display_SyncVSync_5_491E00()
{
    return *reinterpret_cast<std::int32_t const*>(0x00542318); // a1 18235400 c3
}
} // extern "C"
```

### 15. `0x00491E10` - `Display_SwapBuffers_5` (no registration)

- size 6 B; BN extent `0x00491E10`-`0x00491E15`; 1 block, 0 edges (BN and r2 agree; the linear sweep ends at the `ret`).
- BN definition: `int32_t()`
- BN pseudocode:

```c
int32_t Display_SwapBuffers_5()

{
    return g_54231c;
}
```

- r2 disassembly (bytes | opcode), clipped to the BN extent:

```
00491e10 a11c235400           mov eax, dword [0x54231c]
00491e15 c3                   ret
```

- body:

```cpp
// Native: Display_SwapBuffers_5_491E10 (placeholder until VERIFIED); retail VA 0x00491E10-0x00491E15 (6 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 86c1e83ddd7b8c496527adafef85f1c623225791359d8bf8adbd9ad652122cd9.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (a1 1c235400 c3); image dword at 0x0054231C holds 0xFFFFFFFF.
// Behavior/CFG: mov eax, dword [0x0054231C]; ret - single block, no callees, returns the .rdata slot value.
// Status/limits: TRANSCRIBED (static only); reads an image constant slot, so no engine global and no runtime write is involved; runtime UNOBSERVED in the port.
#include <cstdint>

extern "C" {

std::int32_t Display_SwapBuffers_5_491E10()
{
    return *reinterpret_cast<std::int32_t const*>(0x0054231C); // a1 1c235400 c3
}
} // extern "C"
```

### 16. `0x00491E20` - `Display_ClearSurfaces_5` (no registration)

- size 6 B; BN extent `0x00491E20`-`0x00491E25`; 1 block, 0 edges (BN and r2 agree; the linear sweep ends at the `ret`).
- BN definition: `int32_t()`
- BN pseudocode:

```c
int32_t Display_ClearSurfaces_5()

{
    return g_542320;
}
```

- r2 disassembly (bytes | opcode), clipped to the BN extent:

```
00491e20 a120235400           mov eax, dword [0x542320]
00491e25 c3                   ret
```

- body:

```cpp
// Native: Display_ClearSurfaces_5_491E20 (placeholder until VERIFIED); retail VA 0x00491E20-0x00491E25 (6 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 cc0802644f55e0c8cc177f618e27139a7d602b03476385bac26736e90554a283.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (a1 20235400 c3); image dword at 0x00542320 holds 0xFFFFFFFF.
// Behavior/CFG: mov eax, dword [0x00542320]; ret - single block, no callees, returns the .rdata slot value.
// Status/limits: TRANSCRIBED (static only); reads an image constant slot, so no engine global and no runtime write is involved; runtime UNOBSERVED in the port.
#include <cstdint>

extern "C" {

std::int32_t Display_ClearSurfaces_5_491E20()
{
    return *reinterpret_cast<std::int32_t const*>(0x00542320); // a1 20235400 c3
}
} // extern "C"
```

### 17. `0x00491E30` - `Display_RestoreDevice_5` (no registration)

- size 6 B; BN extent `0x00491E30`-`0x00491E35`; 1 block, 0 edges (BN and r2 agree; the linear sweep ends at the `ret`).
- BN definition: `int32_t()`
- BN pseudocode:

```c
int32_t Display_RestoreDevice_5()

{
    return g_542328;
}
```

- r2 disassembly (bytes | opcode), clipped to the BN extent:

```
00491e30 a128235400           mov eax, dword [0x542328]
00491e35 c3                   ret
```

- body:

```cpp
// Native: Display_RestoreDevice_5_491E30 (placeholder until VERIFIED); retail VA 0x00491E30-0x00491E35 (6 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 3123a5a435c4ca8dd2bf4923878809a98e47d839c5800424427cb3fd8b18caa7.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (a1 28235400 c3); image dword at 0x00542328 holds 0xFFFFFFFF.
// Behavior/CFG: mov eax, dword [0x00542328]; ret - single block, no callees, returns the .rdata slot value.
// Status/limits: TRANSCRIBED (static only); reads an image constant slot, so no engine global and no runtime write is involved; runtime UNOBSERVED in the port.
#include <cstdint>

extern "C" {

std::int32_t Display_RestoreDevice_5_491E30()
{
    return *reinterpret_cast<std::int32_t const*>(0x00542328); // a1 28235400 c3
}
} // extern "C"
```

### 18. `0x00491E40` - `Display_CheckCapabilities_5` (no registration)

- size 6 B; BN extent `0x00491E40`-`0x00491E45`; 1 block, 0 edges (BN and r2 agree; the linear sweep ends at the `ret`).
- BN definition: `int32_t()`
- BN pseudocode:

```c
int32_t Display_CheckCapabilities_5()

{
    return g_54232c;
}
```

- r2 disassembly (bytes | opcode), clipped to the BN extent:

```
00491e40 a12c235400           mov eax, dword [0x54232c]
00491e45 c3                   ret
```

- body:

```cpp
// Native: Display_CheckCapabilities_5_491E40 (placeholder until VERIFIED); retail VA 0x00491E40-0x00491E45 (6 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 2d4e2406b9bad707de93c76c006c9a98b2d74e158e7d4d6ed33c1d70b93d6b07.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (a1 2c235400 c3); image dword at 0x0054232C holds 0xFFFFFFFF.
// Behavior/CFG: mov eax, dword [0x0054232C]; ret - single block, no callees, returns the .rdata slot value.
// Status/limits: TRANSCRIBED (static only); reads an image constant slot, so no engine global and no runtime write is involved; runtime UNOBSERVED in the port.
#include <cstdint>

extern "C" {

std::int32_t Display_CheckCapabilities_5_491E40()
{
    return *reinterpret_cast<std::int32_t const*>(0x0054232C); // a1 2c235400 c3
}
} // extern "C"
```

### 19. `0x00491E50` - `Display_SetGammaRamp_5` (no registration)

- size 6 B; BN extent `0x00491E50`-`0x00491E55`; 1 block, 0 edges (BN and r2 agree; the linear sweep ends at the `ret`).
- BN definition: `int32_t()`
- BN pseudocode:

```c
int32_t Display_SetGammaRamp_5()

{
    return g_542330;
}
```

- r2 disassembly (bytes | opcode), clipped to the BN extent:

```
00491e50 a130235400           mov eax, dword [0x542330]
00491e55 c3                   ret
```

- body:

```cpp
// Native: Display_SetGammaRamp_5_491E50 (placeholder until VERIFIED); retail VA 0x00491E50-0x00491E55 (6 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 840d57fe0a4f10bbfe6a1b8435edf98b8bed8a5b41ce9756c7eedcd104d86bb3.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (a1 30235400 c3); image dword at 0x00542330 holds 0xFFFFFFFF.
// Behavior/CFG: mov eax, dword [0x00542330]; ret - single block, no callees, returns the .rdata slot value.
// Status/limits: TRANSCRIBED (static only); reads an image constant slot, so no engine global and no runtime write is involved; runtime UNOBSERVED in the port.
#include <cstdint>

extern "C" {

std::int32_t Display_SetGammaRamp_5_491E50()
{
    return *reinterpret_cast<std::int32_t const*>(0x00542330); // a1 30235400 c3
}
} // extern "C"
```

### 20. `0x00491E60` - `Display_CreateSurface_5` (no registration)

- size 6 B; BN extent `0x00491E60`-`0x00491E65`; 1 block, 0 edges (BN and r2 agree; the linear sweep ends at the `ret`).
- BN definition: `int32_t()`
- BN pseudocode:

```c
int32_t Display_CreateSurface_5()

{
    return g_542334;
}
```

- r2 disassembly (bytes | opcode), clipped to the BN extent:

```
00491e60 a134235400           mov eax, dword [0x542334]
00491e65 c3                   ret
```

- body:

```cpp
// Native: Display_CreateSurface_5_491E60 (placeholder until VERIFIED); retail VA 0x00491E60-0x00491E65 (6 bytes, 1 block, 0 edges).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; capture SHA256 115900e59199027eb6bea677a6e2bda8553bd688aa9e2815546db5955ef699ef.
// Tools/evidence: Binary Ninja 6.0.10601 decompile + radare2 6.2.0 disassembly (a1 34235400 c3); image dword at 0x00542334 holds 0xFFFFFFFF.
// Behavior/CFG: mov eax, dword [0x00542334]; ret - single block, no callees, returns the .rdata slot value.
// Status/limits: TRANSCRIBED (static only); reads an image constant slot, so no engine global and no runtime write is involved; runtime UNOBSERVED in the port.
#include <cstdint>

extern "C" {

std::int32_t Display_CreateSurface_5_491E60()
{
    return *reinterpret_cast<std::int32_t const*>(0x00542334); // a1 34235400 c3
}
} // extern "C"
```

## 3. Dependency check per body

`task_types.hpp` = `engine/include/igi/retail/core/task_types.hpp`; `script_state.hpp` = `engine/include/igi/retail/script/script_state.hpp`. "defined here" means the body file itself defines the global as an `extern "C"` object, matching `engine/src/retail/core/native_489560.cpp`.

| # | VA | symbols referenced by the body | engine state | link verdict | flags |
|---|---|---|---|---|---|
| 1 | `0x4e5370` | `TaskType_Register` (0x401900), `QTask_AllocateEvent` (0x4017C0), `g_548638` read, `g_54875c`/`g_54875e` defined here | both natives landed; `g_548638` has no definition in `engine/` | **not linkable today** | unresolved external `g_548638`; its retail writer is outside this batch |
| 2 | `0x4da1d0` | `TaskType_Register`, `QTask_LinkQTaskEventHandler` (0x401400) x4, `g_548638` read, `g_a5e304` defined here | natives landed; `g_548638` absent | **not linkable today** | unresolved external `g_548638`; four handler VAs are raw literals |
| 3 | `0x4ebc60` | `TaskType_Register`, `QTask_GetCurrentType` (0x4018F0), `QTask_AllocateEvent`, `QTask_LinkQTaskEventHandler`, `QTask_LinkQTaskEventHandlerNoContextFn` (0x401530) x2, `g_54db68`/`g_54db6a` defined here | all landed | **linkable today** | handlers 0x4D3710/0x4EBCD0 raw literals |
| 4 | `0x4e7c50` | `TaskType_Register`, `QTask_GetCurrentType`, `QTask_LinkQTaskEventHandlerNoContextFn` x2, `g_54d974` defined here | all landed | **linkable today** | handlers 0x4E7C90/0x4D3710 raw literals |
| 5 | `0x5310e0` | `TaskType_Register`, `QTask_GetCurrentType`, `QTask_LinkQTaskEventHandlerNoContextFn` x2, `g_54f7d0` defined here | all landed | **linkable today** | handlers 0x531130/0x4D3710 raw literals |
| 6 | `0x530a80` | `TaskType_Register`, `QTask_GetCurrentType`, `QTask_LinkQTaskEventHandlerNoContextFn` x2, `g_54f7a0`/`g_a845ec` defined here | all landed | **linkable today** | 4th register argument = 2 opens the untranscribed 0x4B0B00 task-list path; handlers raw literals |
| 7 | `0x4ed510` | `TaskType_Register`, `QTask_GetCurrentType`, `QTask_LinkQTaskEventHandlerNoContextFn`, `g_a5efa0`/`g_bc20a0`/`g_bc2150`/`g_bc2200`/`g_bc22b0` defined here | all landed | **linkable today** | handler 0x4ED560 raw literal |
| 8 | `0x51a1d0` | `TaskType_Register`, `QTask_LinkQTaskEventHandler`, `QTask_LinkQTaskEventHandlerNoContextFn`, `Script_Get_4F1A60` (0x4F1A60, `script_state.hpp`), `g_54875c` (defined by body 1), `g_54ed4e` defined here | all landed; parent global defined by body 1 of this batch | **linkable today, with body 1** | without body 1: unresolved `g_54875c`; handlers 0x51A240/0x51A230 raw literals |
| 9 | `0x491db0` | `mov eax,[0x00542340]` - no symbols, no calls | image constant, nothing to resolve | **linkable today** | none |
| 10 | `0x491dc0` | `mov eax,[0x00542338]` - no symbols, no calls | image constant, nothing to resolve | **linkable today** | none |
| 11 | `0x491dd0` | `mov eax,[0x0054233C]` - no symbols, no calls | image constant, nothing to resolve | **linkable today** | none |
| 12 | `0x491de0` | `mov eax,[0x00542310]` - no symbols, no calls | image constant, nothing to resolve | **linkable today** | none |
| 13 | `0x491df0` | `mov eax,[0x00542314]` - no symbols, no calls | image constant, nothing to resolve | **linkable today** | none |
| 14 | `0x491e00` | `mov eax,[0x00542318]` - no symbols, no calls | image constant, nothing to resolve | **linkable today** | none |
| 15 | `0x491e10` | `mov eax,[0x0054231C]` - no symbols, no calls | image constant, nothing to resolve | **linkable today** | none |
| 16 | `0x491e20` | `mov eax,[0x00542320]` - no symbols, no calls | image constant, nothing to resolve | **linkable today** | none |
| 17 | `0x491e30` | `mov eax,[0x00542328]` - no symbols, no calls | image constant, nothing to resolve | **linkable today** | none |
| 18 | `0x491e40` | `mov eax,[0x0054232C]` - no symbols, no calls | image constant, nothing to resolve | **linkable today** | none |
| 19 | `0x491e50` | `mov eax,[0x00542330]` - no symbols, no calls | image constant, nothing to resolve | **linkable today** | none |
| 20 | `0x491e60` | `mov eax,[0x00542334]` - no symbols, no calls | image constant, nothing to resolve | **linkable today** | none |

Consequence: 17 of the 20 body files compile and link as-is (12 leaves + 5 registrars). The three AI/DynCube registrars (`0x4E5370`, `0x4DA1D0`, and transitively `0x51A1D0` if body 1 is not added) fail link on the single unresolved symbol `g_548638`; the body text is still retail-exact and the missing piece is a data definition, not behaviour.

## 4. Blocked entries (deliberately not written as bodies)

### 4.1 The other 113 position-1 ids

Of the 123 ids the retail capture assigns to position 1, ids 0 `QTask` (`QTask_Init` `0x004010D0`, `task_types.cpp`) and 45 `CamTask` (`CamTask_Init` `0x00530220`, `cam_task.cpp`) are already landed, 8 are delivered above, and **113 are blocked** because their registrar function calls natives that are not in `engine/`; writing a body would mean inventing those callees, which the rules forbid.

| id | class | registrar VA | first unlanded callees (blocking) |
|---|---|---|---|
| 1 | `QObj` | `0x4d9730` | `0x401860`, `0x401860`, `0x401860`, `0x401860` |
| 4 | `TransparencyList` | `0x531fa0` | `0x4978d0`, `0x4978d0`, `0x4978f0`, `0x4950e0` |
| 8 | `RigidDynCubeObj` | `0x4c3450` | `0x4e55b0`, `0x4cea10`, `0x4b8890`, `0x401f60` |
| 9 | `MagicObj` | `0x4d9a00` | `0x4c48c0`, `0x4c56f0`, `0x4d9fb0`, `0x4c5700` |
| 122 | `AIGraph` | `0x508fa0` | `0xc3504c`, `0x4f1980`, `0x4f1a80`, `0x4f1a80` |

The full ordered list of the 123 id-bearing call sites is in `POSITION1-REGISTRAR-INVENTORY.md` section 2; each entry names its registrar function, whose callees are listed in section 3 of that document.

### 4.2 The 181 unlanded callee-free thunks

They are the same 5-byte `mov al/ax/eax,[global]; ret` shape, but their operand is a `.data` runtime global that no landed native writes and no header declares. Writing them would require inventing a definition for that global (and guessing its width/lifetime), so they stay blocked until the writer native lands. Examples:

| VA | BN alias | operand | blocking writer |
|---|---|---|---|
| `0x4F9280` | `Script_EvaluateCondition_22` | `0x0054E369` | `Graph_Init` `0x4FAA60` (untranscribed) |
| `0x4F9230` | `Script_ParseEnumInt32_22` | `0x0054E36C` | same |
| `0x4CEA10` | `DynObj_UpdateDynTree_12` | `0x00548176` | DynObj update path (untranscribed) |
| `0x4E55B0` | `SoundSys_PlaySoundSample_3` | `0x00A5E408` | SoundSys registration path (untranscribed) |
| `0x4D9FB0` | `MagicObj_GetEventId` | `0x0054864A` | MagicObj registration path (untranscribed) |

The `Script_*` thunks whose globals are already transcribed are landed already (for example `0x4F9220` in `engine/src/retail/script/native_4f9220.cpp`) and are not part of this packet; the same file pattern is used above whenever a global does exist.

### 4.3 Open items that do not block the bodies

- `TaskType_Register` 4th argument = 2 in `FlashTask_Init` `0x530A80` selects the retail task-list build (`0x004B0B00`) that the port does not transcribe; the body records the argument value and flags the behavioural gap.
- Handler addresses are passed as retail-VA literals because the 13 handler natives (`0x4DA240`, `0x4DA2A0`, `0x4DA270`, `0x4DA2C0`, `0x4D3710`, `0x4EBCD0`, `0x4E7C90`, `0x531130`, `0x530C40`, `0x530AD0`, `0x4ED560`, `0x51A240`, `0x51A230`) are still untranscribed. The immediate is exactly the retail push operand, so links and handler-table contents are right; only the handler bodies are missing.
- `g_548638` (`0x00548638`) is the "current parent id" read by the DynCube and Hazinglayers registrars. No landed native writes it, so its writer must be identified from retail before bodies 1 and 2 link. If it always holds the previously registered class id the writer sits in the untranscribed QObj registration path - that is a hypothesis, not evidence, and is not encoded in the bodies.
- ABI caveat: retail loads the parent with `mov ax,[...]`, so the pushed 32-bit value has caller-residue high bits. `TaskType_Register` stores only the low 16 bits (`task_types.hpp`), so the body passes a 16-bit value; no id can differ.
- All 20 bodies are static-only (`TRANSCRIBED`); no runtime observation was made in this packet and none is claimed.

## 5. Artifacts

Directory `build/audit/main-0x48F530/level1-closure/20261024T052720Z/POSITION1-LEAVES`: 60 per-VA artifacts (`<va>.json` = BN info + BN decompile + r2 opcodes + retail bytes; `bn-dec-<va>.c`; `r2-dis-<va>.txt`) plus this document and `artifact-manifest.json` with per-file SHA-256, the retail SHA-256 and tool versions. File count before the manifest: 60.

## 6. Reproduction

```
# BN (the single decompiler for every VA in this packet):
python3 -c "import sys; sys.path.insert(0, 'scripts'); from bn_client import BN; print(BN().call('bn_function_decompile', {'function': '0x4e5370'}))"
# r2 structural check (bytes and opcodes):
r2 -2 -q -e scr.color=0 -c 'pdj 40 @ 0x4e5370' /Users/haseeb-mir/Downloads/IGI/igi.exe
```

Both run against retail SHA-256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`; the per-VA artifacts are the exact captures the bodies cite.