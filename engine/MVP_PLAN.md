# Project IGI C++ Native Engine — Master Replication & MVP Plan

**Document Version:** 2.0  

**First acceptance milestone, owner update 2026-10-16:** Level 1 only, matched
against the original retail `igi.exe`. Movement, jumping, building entry,
weapons and damage, health, AI/navigation, gameplay scripting/objectives,
original models/textures, graphics, and player/weapon/enemy animations remain
required. Other levels are later milestones.

**Deferred:** Sound/audio implementation, movies and movie animation,
cutscenes, subtitles, smoke, optional effects, and menu presentation.
Gameplay animation is not part of the movie/cutscene deferral.
After Level 1 passes the relevant implementation and retail comparisons,
hand off the build and wait for the owner's manual test before expanding scope.
The historical phases below do not authorize starting those deferred systems.
Use the C# `-vanilla` configuration as a candidate reference and retail IGI.exe
as the authority. CFG similarity is one static check, not proof of behavior.

**First verified correction:** Signed QVM integer initializer operands; see
[evidence and controlled runtime comparison](../docs/fidelity/qvm-integer-operands.md).
The complete script VM, task hierarchy, combat/physics, AI, animation and rendering
requirements remain open. No subsystem-wide parity percentage is established.
**Target Projects:**
- **Source Under Development (C++):** `/Users/haseeb-mir/Documents/Code/project-open-igi`
- **Reference Specification (C#):** `/Users/haseeb-mir/Documents/Code/open-igi`
- **Ground Truth Binary:** `IGI.exe` (PE32 Retail v1.0, Base `0x00400000`, MD5: verified retail)
- **REA Toolchain:** Auto-RE-Agent (`/Users/haseeb-mir/Documents/Code/auto-re-agent`), Ghidra 12.1.x, radare2 6.x

---

## 1. Executive Strategy: 100% Binary Decompilation Mandate (Zero Approximations)

Your goal is not a "behavioral approximation" or a "look-alike" project. The goal is a **100% exact mathematical and architectural replica of Project I.G.I. (v1.0 retail `IGI.exe`)**, reverse-engineered directly from the machine code, while isolating and modernizing only the legacy Windows OS-dependent wrappers.

### The Non-Negotiable Boundary: What to Abstract vs. What MUST Match 100%

```
 ┌────────────────────────────────────────────────────────────────────────┐
 │                   PLATFORM-DEPENDENT LAYER (ABSTRACTED)                │
 │  Win32 Windowing, Message Loop, Direct3D 7 Device Init, DirectSound   │
 │                                   │                                    │
 │  Mapped to: SDL2/platform renderer backends, miniaudio.h, POSIX/VFS     │
 └───────────────────────────────────┬────────────────────────────────────┘
                                     │ Inputs, Audio Buffers, Render Commands
                                     ▼
 ┌────────────────────────────────────────────────────────────────────────┐
 │           CORE GAME ENGINE (100% EXACT DECOMPILED BINARY REPLICA)      │
 │  • Struct layouts, offsets, and field strides (IGINatives.json)        │
 │  • Math formulas: slope constants, 3-sample accuracy cone, fall damage │
 │  • Fixed 30 Hz clock and intrusive Qtask hierarchy                     │
 │  • Complete LOOP 8.5 QVM bytecode execution and 302 native hooks       │
 │  • AI vision cones, detection timers, alert states, alarm networks     │
 │  • Skeletal Hermite splines, root motion, and AnimTrigger audio tags   │
 │                                   │                                    │
 │  NO APPROXIMATIONS • NO ARTISTIC GUESSES • REA VERIFIED (Score >= 0.85)│
 └────────────────────────────────────────────────────────────────────────┘
```

#### What is Abstracted (Platform API Layer Only):
1. **Win32 OS APIs**: `CreateWindowExA`, `PeekMessage`, `DispatchMessage`, `GetCursorPos`, and Windows Registry keys are cleanly replaced by **SDL3 / SDL2**.
2. **Direct3D 7 / DirectDraw**: Keep graphics APIs behind renderer backends. A future Windows vanilla backend may use D3D7/DirectDraw for retail fixed-function parity; macOS/Linux use platform-supported backends implementing the same scene and material behavior. No D3D7 backend is implemented or verified yet.
3. **DirectSound / Miles Sound System Init**: Hardware driver initialization is replaced by **`miniaudio.h`** (which outputs through CoreAudio on macOS, WASAPI on Windows, and ALSA/PulseAudio on Linux).
4. **Win32 File Handles**: `CreateFileA` / `ReadFile` are mapped through standard portable C++20 `std::filesystem` and memory-mapped archives.

#### What MUST Match 100% Exactly (Core Engine & Logic):
1. **Binary Memory Layouts**: Struct fields, offsets, and array strides must match the proven static types in `project-igi-natives/assets/IGINatives.json` (`ProfileRecord` at `0x00BC2388`, `Soldier` offsets, `HumanPlayer` offsets).
2. **Mathematical Equivalence**: Every mathematical formula (e.g. `0x478BA0` weapon accuracy, `0x408460` fall damage, `0x498420` slope sliding acceleration) must match the machine code disassembly—down to identical float constants, integer shifts, and order of operations.
3. **Task & State Sequencing**: The 30 Hz tick schedule (`0x418B00`), task execution order in `Qtask_Update` (`0x4B8810`), and state transitions must be identical.
4. **QVM Virtual Machine Execution**: Every opcode, stack operation, call frame, and native registrar invocation (`0x4B8890` etc.) must evaluate identically to the retail VM.
5. **AI Logic & Perception**: Vision cone formulas, detection timers, alert states, and alarm propagation networks must behave exactly as in `IGI.exe`.
6. **No "Approximate" Mechanics**: We reject any third-party code or "look-alike" logic that merely tries to guess how IGI worked. If a routine does not pass the REA decompiler verification gate, it does not enter the C++ codebase.

---

### The Three-Tier Truth Model

```
 ┌──────────────────────────────────────────────────────────────┐
 │                TIER 1: RETAIL IGI.EXE (GROUND TRUTH)         │
 │   Exact machine code, x86 assembly, registers, binary offsets │
 └──────────────────────────────┬───────────────────────────────┘
                                │ Verified by REA (Ghidra + r2)
                                ▼
 ┌──────────────────────────────────────────────────────────────┐
 │             TIER 2: C# OPEN-IGI (REFERENCE MANUAL)           │
 │   Living high-level algorithms, math formulas, data flow     │
 └──────────────────────────────┬───────────────────────────────┘
                                │ Filter out remaster drift
                                ▼
 ┌──────────────────────────────────────────────────────────────┐
 │           TIER 3: C++ PROJECT-OPEN-IGI (NATIVE REPLICA)      │
 │   Pure C++20, cross-platform, hardware-accelerated, native   │
 └──────────────────────────────────────────────────────────────┘
```

1. **Do not reverse-engineer from scratch**: C# `open-igi` has already decrypted 90% of the game's file formats, animation structures, and mathematical formulas. Use it as your readable **algorithm blueprint**.
2. **Do not copy C# blindly**: C# `open-igi` introduced modern enhancements ("remaster drift"—custom HDR shaders, Saira modern fonts, synthetic clock options, modernized UI layout). We strip away all remaster drift and keep **only the authentic vanilla game mechanics**.
3. **Enforce REA Verification Gates**: Every critical formula (weapon spread, fall damage, slope slide, QVM opcode) must match the disassembly of `IGI.exe` verified with Auto-RE-Agent (CFG similarity $\ge 0.85$).

---

## 2. Cross-Platform Build Architecture

The engine must build and run out-of-the-box on **macOS** (Apple Silicon M-series & Intel), **Linux** (x86_64 / arm64), and **Windows** (MSVC / Clang) using a modern CMake structure.

### Minimal Dependency Policy
To ensure long-term maintenance and zero DLL hell, use only rock-solid cross-platform foundational libraries:
* **Windowing & Input:** SDL3 (or SDL2)
* **Graphics API:** Modern OpenGL 3.3 Core (via `glad`) or cross-platform Vulkan (via `MoltenVK` on macOS)
* **Audio Backend:** `miniaudio.h` (single-file, header-only, zero dependencies, supports CoreAudio, WASAPI, ALSA, PulseAudio)
* **Math:** `glm` (matches GLSL & SIMD layouts)
* **Compiler:** C++20 standard (`-std=c++20` / `/std:c++20`)

### Platform Build Matrix

| Platform | Compiler | Graphics Backend | Audio Backend | Build Command |
| :--- | :--- | :--- | :--- | :--- |
| **macOS (Apple Silicon / Intel)** | Apple Clang / LLVM 16+ | OpenGL 3.3 Core / Metal (MoltenVK) | CoreAudio (miniaudio) | `cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j` |
| **Linux (Ubuntu / Fedora / Arch)** | GCC 13+ / Clang 17+ | OpenGL 3.3 Core / Vulkan | ALSA / PulseAudio (miniaudio) | `cmake -B build -DCMAKE_BUILD_TYPE=Release && cmake --build build -j` |
| **Windows 10 / 11** | MSVC 2022 / Clang-cl | DirectX 11/12 or OpenGL 3.3 | WASAPI (miniaudio) | `cmake -B build -G "Visual Studio 17 2022" && cmake --build build --config Release` |

---

## 3. Subsystem-by-Subsystem Technical Porting Blueprint

### Subsystem 1: Script Engine & Native Call System (QVM)

#### Ground Truth (`IGI.exe`)
* **Format:** Innerloop `LOOP 8.5` bytecode files (`.qvm`).
* **Registrars:** Four native registration entry points:
  * `0x4B8890`: Game & mission state handlers.
  * `0x4BAAC0`: Weapon and actor definitions.
  * `0x4B88C0`: AI perception and task handlers.
  * `0x4B88F0`: Sound and music volume natives.
* **Catalog:** 302 statically proven natives recorded in `project-igi-natives/assets/IGINatives.json`.

#### Where to Look in C# `open-igi`
* **Path:** `src/OpenIGI.Scripting/`
* **Key Files:**
  * `QvmVirtualMachine.cs`: Full fetch-decode-execute instruction loop.
  * `QvmInstruction.cs`: Complete table of opcodes.
  * `QvmStack.cs`: Value and call-frame evaluation stack.
  * `OpenIGI.Game/Natives/`: C# implementations of all retail native functions.

#### What C# Did Right (Port This)
* The instruction decoder handles binary jumps, function parameters, stack frames, local variables, and string table lookups.
* Clean native binding interface: `RegisterNative(string name, Func<QvmArgs, QvmValue> handler)`.

#### What C# Did Wrong / Remaster Drift (Avoid This)
* C# wraps script state in heavy LINQ expressions and delegate allocations.
* Use a flat `std::vector<QvmValue>` evaluation stack and C++ function pointers (`using NativeFn = QvmValue(*)(QvmContext&)`).

#### Actionable C++ Implementation Plan
* **Target Files:**
  * `engine/include/igi/script/qvm_vm.hpp`
  * `engine/src/script/qvm_vm.cpp`
  * `engine/src/script/qvm_natives.cpp`
* **Step 1:** Expand current `qvm_initializer.cpp` from 8 opcodes to the full instruction set (arithmetic, branches, calls, stack pops, string pool dereferencing).
* **Step 2:** Implement the native registrar dispatch table binding the 302 natives from `IGINatives.json`.
* **Step 3:** Execute `LOCAL:menusystem/mainmenu.qvm` and mission `.qvm` scripts at runtime.

---

### Subsystem 2: 30 Hz Game Loop & Hierarchical Task System

#### Ground Truth (`IGI.exe`)
* **Tick Rate:** Exactly **30.0 Hz fixed tick rate** (`0x418B00`, `0x418BD0`).
* **Task Architecture:** `Qtask_Update` (`0x4B8810`) walks an intrusive hierarchical tree of `TaskTypeRecord` nodes (`0x00AF5EE8`). Every soldier, door, elevator, camera, alarm, and weapon is an active task node.

#### Where to Look in C# `open-igi`
* **Path:** `src/OpenIGI.Engine/Tasks/` and `src/OpenIGI.Engine/Flow/`
* **Key Files:**
  * `GameClock.cs` (Line 15–40): Defines `DefaultTickRate = 30`.
  * `TaskGraph.cs`: Hierarchical entity graph and node walk.
  * `FlowTask.cs` (Lines 240–280): Exact state machine for `FlowEvent.MainMenu`, `FlowEvent.Game`, `FlowEvent.EndGame`.

#### What C# Did Right (Port This)
* The 30 Hz fixed step with variable-rate visual interpolation: physics and game state advance in exact 1/30s increments, while rendering interpolates between previous and current state.
* The state transition pipeline cleanly isolates `Intro -> MainMenu -> Briefing -> Game -> Outro`.

#### What C# Did Wrong / Remaster Drift (Avoid This)
* Introduced `-lockstep` and synthetic benchmarking hooks that bypass standard Windows message pumping.
* C++ should stick to strict delta accumulator timing:
  ```cpp
  constexpr double TickDuration = 1.0 / 30.0;
  accumulator += frameDelta;
  while (accumulator >= TickDuration) {
      game_tick();
      accumulator -= TickDuration;
  }
  ```

#### Actionable C++ Implementation Plan
* **Target Files:**
  * `engine/include/igi/task_graph.hpp`
  * `engine/src/task_graph.cpp`
  * Replace ad-hoc sequential updates in `game_simulation.cpp` with a proper task walk.

---

### Subsystem 3: Combat, Ballistics & Weapon Accuracy

#### Ground Truth (`IGI.exe`)
* **Firing Gate (`0x478BA0`)**:
  * **3-Sample Accuracy Cone**: Generates 3 independent random uniform floats $[0.0, 1.0]$ and multiplies them together:
    $$\text{radiusSample} = R_1 \times R_2 \times R_3$$
    This mathematical product strongly biases bullet spread toward the center of the crosshair.
  * **Burst Spread Gate**: A strict step function—`roundsThisBurst > 3`. Rounds 1 to 4 use `MinimumSpread`; round 5+ snaps directly to `MaximumSpread` (no linear ramp!).
* **Recoil Model (`0x479780`)**: Injects immediate pitch-up and randomized yaw impulses directly into the view angles.
* **Hit-Zone Table (`0x5412A4`)**:
  * Head: $\times 3.0$ damage multiplier (fatal for standard guards).
  * Chest: $\times 1.0$ multiplier.
  * Stomach: $\times 0.8$ multiplier.
  * Limbs: $\times 0.5$ multiplier.

#### Where to Look in C# `open-igi`
* **Path:** `src/OpenIGI.Game/Weapons/`
* **Key Files:**
  * `GunFiring.cs` (Lines 29–115): Transcribed byte-for-byte from `0x478BA0`.
  * `HeldGun.cs` (Lines 140–280): Transcribed from `0x479780` (recoil impulses and cooldown ticks).
  * `GunTiming.cs`: Exact cyclic fire rates and reload state delays.
  * `GunShotBallistics.cs` & `BulletImpact.cs`: Raycast trajectory and material penetration.

#### What C# Did Right (Port This)
* The exact constants and random distribution in `GunFiring.cs`:
  ```csharp
  public const int RoundsAtMinimumSpread = 3;
  public const int ConeRadiusSamples = 3;
  public const float DegreesToRadians = 0.017453292f;
  ```
* Hit-zone parsing and damage calculation.

#### What C# Did Wrong / Remaster Drift (Avoid This)
* Added customized dynamic crosshair scaling and hit-marker visual shaders not present in retail.
* Keep the authentic retail static green HUD reticle.

#### Actionable C++ Implementation Plan
* **Target Files:**
  * `engine/include/igi/combat_engine.hpp`
  * `engine/src/game_combat.cpp`
* **Step 1:** Replace current basic raycast in `game_combat.cpp` with the 3-sample multiplicative cone from `GunFiring.cs`.
* **Step 2:** Add burst counter tracking to switch from `minimum_spread` to `maximum_spread` after 4 rounds.
* **Step 3:** Implement the `0x5412A4` body-zone damage multipliers in `capsule_intersection`.

---

### Subsystem 4: Human Movement, Slopes & Fall Impact Physics

#### Ground Truth (`IGI.exe`)
* **Fall Damage (`0x408460`)**:
  * Vertical landing speed measured in metres per second.
  * $v \le 14\text{ m/s}$: **Zero damage** (safe landing).
  * $14\text{ m/s} < v \le 27\text{ m/s}$: Linear damage scaling up to 100% of maximum health.
  * $v > 27\text{ m/s}$: Instant fatality.
  * Camera kick: Displacement proportional to landing speed (up to 1024 engine units).
  * Hearing radius: Safe landing generates 5m (20,480 units) sound; damaging landing generates 10m (40,960 units) AI alert.
* **Slope Slide Acceleration Constants**:
  * Decompiled float literals: `18.962963f`, `56.888889f`, `948.0f`, `1137.0f`, damping `0.9f`.

#### Where to Look in C# `open-igi`
* **Path:** `src/OpenIGI.Game/Player/`
* **Key Files:**
  * `HumanFallImpact.cs` (Lines 22–72): Exact implementation of `0x408460`.
  * `HumanLocomotionAnimator.cs`: Integration between slope slide, movement vectors, and collision responses.

#### What C# Did Right (Port This)
* The mathematical formula for damage:
  ```csharp
  float speed = MathF.Abs(verticalVelocityUnitsPerTick) * (1f / 4096f) * 30f;
  float damage = MathF.Max(0f, speed - 14f) * (1f / 13f) * maximumHealth;
  ```
* Thresholds for `player_fall_1` and `player_fall_2` sound effects.

#### What C++ Already Has
* `human_movement_integrator.cpp` in `project-open-igi` already contains the authentic slope acceleration constants.

#### Actionable C++ Implementation Plan
* **Target Files:**
  * `engine/src/human_movement_integrator.cpp`
  * `engine/src/game_simulation.cpp`
* **Step 1:** Add vertical velocity tracking upon ground contact.
* **Step 2:** Insert the `HumanFallDamage` formula into `GameSimulation::tick` when the player transitions from airborne to grounded.

---

### Subsystem 5: Skeletal Animation & AnimTrigger Event System

#### Ground Truth (`IGI.exe`)
* **Skeleton & Clips:** IFF `.anm` files storing bone hierarchies and cubic Hermite splines.
* **AnimTriggers (`0x4EC070`)**:
  * Native `DefineAnimTrigger(int eventId, string taskType, string soundResource, bool flag)`.
  * Animation files contain `BOED` event tags at specific timestamps. When the animation playback reaches a tag, it dispatches an event that triggers footsteps, reload sounds, or shell drops.

#### Where to Look in C# `open-igi`
* **Path:** `src/OpenIGI.Formats/Animation/` and `src/OpenIGI.Game/Player/`
* **Key Files:**
  * `BoneAnimation.cs`: Hermite quaternion interpolation.
  * `BoneAnimationTags.cs`: Parsing `BOED` event tags from `.anm` streams.
  * `AnimTriggerRegistry.cs`: Mapping event IDs to sound effects.

#### What C# Did Right (Port This)
* Tag parsing and trigger dispatch: connects animation playback directly to sound playback.

#### What C++ Already Has
* `skeletal_animation.cpp` in `project-open-igi` already successfully decodes bone hierarchies and root motion splines.

#### Actionable C++ Implementation Plan
* **Target Files:**
  * `engine/include/igi/skeletal_animation.hpp`
  * `engine/src/skeletal_animation.cpp`
* **Step 1:** Add `BOED` tag reading to the IFF parser.
* **Step 2:** When advancing animation time in `advance_view_animation`, check for crossed timestamp tags and trigger audio playback.

---

### Subsystem 6: Authentic 32-Voice Sound & Music Engine

#### Ground Truth (`IGI.exe`)
* **Mixer Specifications:**
  * 32 simultaneous voices at 22,050 Hz 16-bit PCM.
  * Flat hand-computed stereo panning (`0x4E7200`, `SoundOptionsRecord`).
  * 3D distance attenuation: Linear drop-off from source position to hearing threshold.
* **Asset Formats:** Raw WAV files packed in `.dat` archives.

#### Where to Look in C# `open-igi`
* **Path:** `src/OpenIGI.Audio/`
* **Key Files:**
  * `SoundMixer.cs`: 32-voice mixing loop with volume and flat stereo panning.
  * `SoundRegistry.cs`: Loading and caching WAV assets from VFS.
  * `AudioNatives.cs`: Natives for `Music_SetVolume`, `Music_SetSfxVolume`, `Sound_EventTrigger`.

#### What C# Did Right (Port This)
* The vanilla mixing model: 32 voices, volume clamping, 22 kHz sample rate, and flat hand-computed panning formula.

#### What C# Did Wrong / Remaster Drift (Avoid This)
* C# defaults to modern OpenAL 3D spatialized HRTF filters. We want the authentic vanilla preset.

#### Actionable C++ Implementation Plan
* **Target Files:**
  * `engine/third_party/miniaudio.h` (single-file library)
  * `engine/include/igi/audio_engine.hpp`
  * `engine/src/audio_engine.cpp`
* **Step 1:** Integrate `miniaudio.h` to initialize the OS sound device (CoreAudio on Mac, WASAPI on Windows, ALSA on Linux).
* **Step 2:** Create a 32-voice audio buffer mixer that mixes active sounds at 22,050 Hz.
* **Step 3:** Hook weapon firing, footsteps (`AnimTrigger`), and impact sounds into the mixer.

---

### Subsystem 7: AI Perception, Alarms & NavGraph Combat

#### Ground Truth (`IGI.exe`)
* **AI Architecture:** 55 native functions (`AIFunction_*`, `AIAction_*`).
* **Perception:** Vision cones parameterized by field-of-view angle, view length, detection schedules, and alert states (Relaxed, Suspicious, Combat).
* **Networked Systems:**
  * Alarm consoles: Guards run to alarm buttons when suspicious.
  * Security cameras: Swiveling cameras that trigger sirens.
  * Guard generators: Spawn reinforcement soldiers when an alarm is active.

#### Where to Look in C# `open-igi`
* **Path:** `src/OpenIGI.Game/Ai/`
* **Key Files:**
  * `AiSoldier.cs` (180 KB): Core state machine handling patrol, investigation, combat, and death.
  * `AiViewCone.cs` & `AiVision.cs`: Line-of-sight checks and detection timers.
  * `AlarmSpeakerRegistry.cs` & `AiAlarmControlState.cs`: Siren management.
  * `SecurityCameraRegistry.cs`: CCTV camera movement and detection.

#### What C# Did Right (Port This)
* The state machine flow: `AiPatrolCombat.cs`, `AiGuardCombat.cs`, and `AiGunnerCombat.cs`.
* Detection schedule math: how fast a guard spots Jones based on crouching, running, and distance.

#### What C# Did Wrong / Remaster Drift (Avoid This)
* Extensive helper abstractions that split AI into dozens of micro-classes. In C++, keep soldiers in a streamlined structure.

#### Actionable C++ Implementation Plan
* **Target Files:**
  * `engine/include/igi/game_ai.hpp`
  * `engine/src/game_ai.cpp`
* **Step 1:** Expand `ActorState` to include alert levels (Relaxed, Alert, Combat) and vision cone angles.
* **Step 2:** Implement detection timers before soldiers immediately start shooting.
* **Step 3:** Connect alarm consoles so guards trigger mission-wide alarms upon detecting bodies or gunshots.

---

### Subsystem 8: Hardware GPU Rendering & Byte-for-Byte Retail Menus

#### Ground Truth (`IGI.exe`)
* **Renderer:** Hardware-accelerated Direct3D 7 / DirectDraw fixed-function pipeline.
* **Terrain:** Innerloop chunked quadtree heightfield engine with level-of-detail mipmapping.
* **Menus:** Driven completely by `LOCAL:menusystem/mainmenu.qvm` and `missionsprites.res`, rendering the authentic green radar border, textured background, and mission selection.

#### Where to Look in C# `open-igi`
* **Path:** `src/OpenIGI.Rendering.Vulkan/` and `apps/OpenIGI.Desktop/DesktopGame.Menu.cs`
* **Key Files:**
  * `DesktopMenuHost.cs`: Connecting QVM script execution to menu rendering.
  * `MenuRenderer.cs`: Loading sprites, buttons, and strings from `mainmenu.qvm`.
  * `TerrainRenderer.cs`: Quadtree chunk traversal and LOD transitions.

#### What C# Did Right (Port This)
* `MenuRenderer.cs` executes the exact bytecode from `mainmenu.qvm` to render the authentic retail menus.
* The quadtree terrain meshing algorithm.

#### What C++ Currently Has (The Major Bottleneck)
* C++ `project-open-igi` currently uses a **CPU software rasterizer** (`scene_renderer.cpp`) drawing to a software pixel buffer with custom placeholder green HUD text.

#### Actionable C++ Implementation Plan
* **Target Files:**
  * `engine/src/gpu_renderer.cpp` (using SDL3 + modern OpenGL 3.3 Core or Vulkan)
  * `engine/src/menu_system.cpp`
* **Step 1:** Replace the CPU software rasterizer with hardware OpenGL 3.3 Core / Vulkan.
  * Upload textures to GPU once via `glTexImage2D`.
  * Draw terrain and models using VBOs/VAOs and standard vertex/fragment shaders.
* **Step 2:** Port `MenuRenderer.cs` into C++ so `mainmenu.qvm` renders the original **Project I.G.I. v1.0** title screen and menus.

---

## 4. The REA Parity Verification Gate (Mandatory Rules)

To ensure your C++ engine remains an exact mathematical replica of `IGI.exe`, every critical function must pass the **REA dual-engine verification loop**:

```
 ┌──────────────────────────────────────────────────────────────┐
 │ 1. Ghidra Decompilation: Recover original C logic from IGI.exe│
 └──────────────────────────────┬───────────────────────────────┘
                                ▼
 ┌──────────────────────────────────────────────────────────────┐
 │ 2. radare2 CFG Validation: Generate basic block count & asm  │
 └──────────────────────────────┬───────────────────────────────┘
                                ▼
 ┌──────────────────────────────────────────────────────────────┐
 │ 3. Parity Formula Evaluation:                                │
 │    Score = 1 - (|G_blocks - R_blocks| / max(G, R, 1) + ...)  │
 │    Rule: Must achieve Score >= 0.85                          │
 └──────────────────────────────┬───────────────────────────────┘
                                ▼
 ┌──────────────────────────────────────────────────────────────┐
 │ 4. Port verified math into C++ (with unit test regression)   │
 └──────────────────────────────────────────────────────────────┘
```

### Key Verification Addresses in `IGI.exe`

| Subsystem | Function Name | Address | Verification Command |
| :--- | :--- | :--- | :--- |
| **Weapons** | `Gun_Fire` (3-sample cone) | `0x478BA0` | `re-agent parity --address 0x478BA0` |
| **Weapons** | `HeldGun_Recoil` | `0x479780` | `re-agent parity --address 0x479780` |
| **Damage** | `HitZone_Table` | `0x5412A4` | `r2 -q -c "px 32 @ 0x5412A4" IGI.exe` |
| **Player** | `Human_FallImpact` | `0x408460` | `re-agent parity --address 0x408460` |
| **Movement** | `Slope_Acceleration` | `0x498420` | `r2 -q -c "pd 20 @ 0x498420" IGI.exe` |
| **Scripting** | `Registrar_Natives` | `0x4B8890` | `re-agent parity --address 0x4B8890` |

---

## 5. Sequential Execution Phases

```
PHASE 1: Hardware Graphics & Sound Foundation
├── Replace CPU software rasterizer with SDL3 + OpenGL 3.3 Core (Instant 120+ FPS)
└── Integrate miniaudio.h for 32-voice WAV audio playback

PHASE 2: Full QVM Script Virtual Machine
├── Expand QvmInitializer to complete instruction set (branches, loops, stack frames)
└── Wire native dispatcher to 302 natives from IGINatives.json

PHASE 3: Retail Combat & Fall Physics Parity
├── Implement 0x478BA0 3-sample accuracy cone and burst spread gate
├── Implement 0x408460 fall damage calculation and view kick
└── Apply 0x5412A4 localized body-zone damage multipliers

PHASE 4: Authentic Menu & UI System
├── Port MenuRenderer to execute LOCAL:menusystem/mainmenu.qvm
└── Render authentic Project I.G.I. v1.0 main menu and mission selection screens

PHASE 5: Complete AI Perception & Alarm Network
├── Implement AiViewCone detection timers and alert states
└── Wire alarm consoles, security cameras, and reinforcement spawning

PHASE 6: REA Parity Verification & CI Pipeline
├── Run automated differential tests comparing C++ state against IGI.exe traces
└── Enforce zero-regression gates across macOS, Linux, and Windows builds
```

---

## 6. Definition of Done (Retail Equivalence)

The project reaches **100% Verified Retail Parity** when:
1. The game launches natively on macOS, Linux, and Windows from a single C++20 binary without third-party runtimes.
2. The authentic retail **Project I.G.I. v1.0** main menu loads from `mainmenu.qvm` and is fully interactive.
3. Audio plays identically to retail (32 voices at 22 kHz, flat-panned stereo, footsteps from `AnimTrigger`).
4. Weapon spread, burst timing, and recoil match `IGI.exe` (`0x478BA0`, `0x479780`) verified by REA.
5. Guards exhibit full vision cone detection, investigate noises, operate alarm panels, and engage in coordinated combat.
6. Missions 1 through 14 can be played from briefing to debriefing using the original game assets.
