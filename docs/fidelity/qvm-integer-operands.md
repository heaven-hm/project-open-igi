# QVM integer operand verification

Date: 2026-10-16. Repository: `project-open-igi`, branch `feature/gameplay-prototype`.

## Scope and result

The initializer previously decoded PUSH, PUSHB and PUSHW numeric operands as unsigned.
An exhaustive regression failed before correction at PUSHB encoded byte 128:
expected -128, obtained 128. Integer conversion now preserves the signed retail value.
String/symbol table indices remain unsigned. No native registry, general QVM runtime,
gameplay policy, renderer, audio or cutscene behavior is implemented by this change.

The C# reference is `open-igi/src/OpenIGI.Scripting/Runtime/QvmInterpreter.cs`,
whose PUSH/PUSHB/PUSHW handlers read signed 32/8/16-bit operands. The selected
comparison application uses `-vanilla -window -size 1024x768`; its parser sets
`gfx_preset vanilla`, `snd_preset vanilla` and `g_preset vanilla`.
The retail machine instructions independently establish the operand behavior.

## Static evidence

Retail file: `/Users/haseeb-mir/Downloads/IGI/igi.exe`.
SHA-256: `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`.

REA was run from `/Users/haseeb-mir/Documents/Code/rea/scripts/rea.mjs`,
the local `heaven-hm/re-agent` checkout, with Ghidra 12.1.2.
The registered Codex REA server instead identifies an npm-cache installation;
the results below came from a separate local-checkout MCP session.

| Opcode | Retail entry | Decisive operation | Ghidra/r2 blocks | REA evidence |
| --- | --- | --- | --- | --- |
| PUSHB (3) | `0x004BE870` | `MOVSX EDX, CL` at `0x004BE895`; advance PC by 2 | 4 / 4 | `ev_525cd776a8495eda80ea12db17de1d69e7a5f9805a77b6eb27a122af8515c6c7` |
| PUSHW (4) | `0x004BE8B0` | `MOVSX EDX, CX` at `0x004BE8D6`; advance PC by 3 | 4 / 4 | `ev_ca94b4f3642c7a0364647a6090fc3df0fecd0bcd02f4eda11b34959acdf8f332` |
| PUSH (2) | `0x004BE8F0` | Pass unchanged dword to integer-stack helper; advance PC by 5 | 6 / 6 | `ev_fa1cef661b9e5d2a10bc23c8a929ee0a33f87b695f33af555d993bb26f8ef70d` |

The block-count similarity is 1.0 for each routine. That score alone does not
establish semantic equivalence. Instruction review and the differential run
below provide the additional evidence for the valid integer paths.

The integer-stack helper at `0x004BDA00` decrements VM word/type cursors at
offsets `0x18`/`0x1C`, writes integer tag 1 via pointer `0x28`, and writes the
operand via pointer `0x24`. The runtime harness checks those writes.

Binary Ninja MCP at `http://127.0.0.1:24642/mcp` independently decompiled the
dispatch loop, execution registrar `0x004BD430`, and byte/word handlers.
Its open database was `project-igi-natives/assets/igi.exe.bndb`.
Its existing semantic symbol labels are provisional: for example,
`0x004BECB0` formats opcode mnemonic names and does not execute PUSH.
No symbols, database bytes or retail file bytes were modified.

## Controlled runtime and memory comparison

`engine/tools/qvm_retail_operands_probe.cpp` is an x86 Windows analysis tool,
not part of the game runtime or a general PE loader. It maps original section
bytes privately without resolving imports or starting the game entrypoint.
For these valid paths, calls to the integer-stack helper are relative and
the executed instructions need no absolute-address relocation or byte patches.
Wrong-opcode/error paths and PUSHA are outside this comparison.

CrossOver's stack reservation prevented allocation at the original base.
The accepted run mapped unchanged section bytes at `0x00D30000` and called
the verified entries by their original RVA. This is controlled instruction
execution under CrossOver, not a naturally reached full-game script capture.

The harness executed all 256 byte encodings, all 65,536 word encodings, and
10 dword boundaries, including signed minimum, signed maximum and -1.
For every call it verified PC advance, both cursor changes, integer tag 1,
all other VM fields, and unchanged surrounding value/type stack canaries.
The portable C++ probe emitted results from the actual initializer CALL argument.

Both traces contain 65,802 rows and compare byte for byte with SHA-256
`bebd2fe314a9cd0b3c8a0be201ed6fac26bac8d4b8afb637a8b5bfc7cd741d96`.
Raw REA dossiers, traces, harness executable and logs are retained locally in
`/tmp/igi-qvm-retail-evidence/`. Temporary artifacts must be regenerated if removed.

## Reproduction

Build the game and portable probe through CMake. The regression is named
`qvm_signed_integer_operands`. Pass one optional CSV output path to the probe.

```sh
cmake -S . -B /tmp/igi-qvm-check -DCMAKE_BUILD_TYPE=Debug \
  -DIGI_GAME_PATH=/Users/haseeb-mir/Downloads/IGI
cmake --build /tmp/igi-qvm-check --parallel 4
/tmp/igi-qvm-check/engine/qvm_integer_operands_probe /tmp/port.csv
i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra -Wpedantic -Werror -static \
  -Wl,--image-base,0x10000000,--disable-dynamicbase \
  engine/tools/qvm_retail_operands_probe.cpp -o /tmp/retail-operands.exe
env LC_ALL=en_US.UTF-8 LANG=en_US.UTF-8 \
  /Users/haseeb-mir/Applications/CrossOver.app/Contents/SharedSupport/CrossOver/bin/wine \
  --bottle project-igi --cx-app 'Z:\tmp\retail-operands.exe' \
  'Z:\Users\haseeb-mir\Downloads\IGI\igi.exe' 'Z:\tmp\retail.csv'
cmp /tmp/retail.csv /tmp/port.csv
```

## Validation and remaining work

Fresh Debug build succeeded. All 17 non-retail CTests passed; after adding
trace output the rebuilt focused operand CTest also passed. The exhaustive
operand probe passed ASan/UBSan with leak detection disabled on macOS.
Clang static analysis of the changed initializer reported no diagnostics.
The retail executable hash was rechecked unchanged after the runtime run.

This verifies the recorded integer values and controlled original stack writes.
It does not verify a complete QVM interpreter, native dispatch, lazy evaluation,
runtime mission scripting, natural gameplay traces or all 32-bit operand values.
The broad retail gameplay gate remains unfulfilled.
