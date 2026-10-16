# Existing C++ QVM runtime reference

Copied from `/Users/haseeb-mir/Documents/Code/project-igi-editor/source/level/`,
upstream `https://github.com/heaven-hm/project-igi-editor.git`, checkout
`c506cd8305aad03dd3d9f24ea1546cbdef689e97`, on 2026-10-16.
The runtime is organized under the engine scripting module. The editor working tree, rather
than an assumed pristine commit, supplied these exact source hashes:

| File | Source SHA-256 |
| --- | --- |
| qvm_interpreter.h | `c5dac1e8181f517e409a6b90b9dc076484a5d34140b5fde2c615ca3403731ec1` |
| qvm_interpreter.cpp | `3260c9b8f8c3a3ae139b2add8d7eb6565ac5cee5cf0670800d624d90cd1407c1` |
| qvm_native_registry.h | `b48ea4892ce146cee1e2a357d820b2ddfc7ca086a75f75fc8400494b5cbd0012` |
| qvm_native_registry.cpp | `e66e9cd13804612d2aadcc136087f874c7876798551520b71d9cc0b6a72e5a97` |

Applicable notices are retained in `../../engine/third_party/licenses/igi-editor-ref/`, including
the source project's license, scope notice and previous license.
The owner explicitly requested reuse across these projects. This import
does not change the licensing of third-party source or proprietary assets.

## Local adaptations

- Use the converter parser already present in the engine. No parser was copied again.
- Adapt missing `signed_operand` metadata using the verified signed integer
  conversion. Missing inline-string metadata fails explicitly instead of
  fabricating text. Full inline operand integration remains open.
- Remove automatic placeholder Print and `std::rand` Random registrations.
  Required native bindings must be supplied explicitly.

The copied runtime is compiled as `igi_scripting` only under
BUILD_TESTING. It is not linked into `igi_game`: incomplete arithmetic,
native behavior and scheduler integration must not silently replace gameplay.
Its normalized synthetic instruction path is inherited editor code and is
not the retail LOOP 8.5 input path exercised by the Level 1 probe.

## Level 1 script evidence

The converter `build-macos-cli/bin/igi1conv` produced separate `.qsc` and
.qas views of retail `MISSIONS/location0/LEVEL1/AI/2208.QVM`.
Script SHA-256: `28284952134aaf0eca6c406eb6d70972ab132115d230d32930bd81855fd9d4df`.
The source requests alarm control 98 on CREATE, patrol 2408 on IDLE,
patrol 2608 on ALARMON, and the default handler on COMBAT or other events.
The probe executes these original bytecode branches and their lazy arguments;
its callbacks record calls instead of simulating game-native effects.

REA from the local `/Users/haseeb-mir/Documents/Code/rea` checkout recovered
the actual AI constants registrar at `0x0044EEB0`, Ghidra 12.1.2 evidence
`ev_4f04000a618559a3eb543454a3262a9dbf3a2f6b32799286039c8a6005ca29da`.
It confirms CREATE=0, IDLE=4, COMBAT=7, ALARMON=8, and ACTIONFLAG_NONE=0.
The C# vanilla reference agrees in `AiEvent.cs` and `AiScriptApi.cs`.
Binary Ninja independently recovered the same literal registrations;
radare2 and Ghidra both report three blocks for the 417-byte registrar.

Validation in the current game checkout: both copied implementation files
compiled individually with Clang C++20 and warnings as errors. The CMake
reference target and Level 1 probe built successfully. Three focused CTests
passed: signed operands, this Level 1 script, and original Level 1 gameplay
loading. The reused runtime and probe also passed ASan/UBSan for these five
event scenarios, with macOS leak detection disabled. Those are functional
checks, not an original-versus-port gameplay trace comparison.

Passing this probe establishes reuse, compilation and observed script dispatch
in this runtime. It does not establish complete retail QVM parity or working
alarm/native behavior in the game. Production bindings, other Level 1 scripts,
floating arithmetic/RNG, error details and original-versus-port event traces
remain required. Missing Ghidra definitions were recorded explicitly; exact
raw instruction windows were analyzed separately and are not mislabeled as
full-binary evidence. Artifacts remain under `/tmp/igi-qvm-retail-evidence/`
and `/tmp/igi-qvm-reuse.zkkHIg/`.

## Engine source locations

- `engine/include/igi/scripting/script_interpreter.hpp`
- `engine/include/igi/scripting/native_function_registry.hpp`
- `engine/src/scripting/script_interpreter.cpp`
- `engine/src/scripting/native_function_registry.cpp`

The hashes above identify the original editor files before local adaptations and renaming.
