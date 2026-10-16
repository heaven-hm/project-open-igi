# Project Open IGI — REA integration status

Checked against the current source tree on 2026-10-16.

## Source and reference locations

| Role | Location |
| --- | --- |
| Native game project | `/Users/haseeb-mir/Documents/Code/project-open-igi` |
| Local REA toolchain | `/Users/haseeb-mir/Documents/Code/rea` |
| Retail executable | `/Users/haseeb-mir/Downloads/IGI/igi.exe` |
| C# vanilla candidate reference | `/Users/haseeb-mir/Documents/Code/open-igi` |
| Research catalog | `/Users/haseeb-mir/Documents/Code/project-igi-natives/assets/BinaryNinja/IGINatives_VERIFIED.json` |

The retail executable SHA-256 is
`dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`.
Recheck it before reusing address evidence. REA upstream is
[heaven-hm/re-agent](https://github.com/heaven-hm/re-agent).

## Catalog status

The external research JSON contains 5,732 entries. Its recorded flags are:

| Recorded status | Entries |
| --- | ---: |
| CFG PASS | 5,244 |
| CFG FAIL | 487 |
| Candidate | 1 |
| L5 runtime flag true | 0 |

These are catalog metadata counts, not independently revalidated functions,
implemented C++ gameplay routines, or proof of complete binary coverage.
A name, address, signature, or CFG score does not establish equivalent behavior.
Semantic names and prototypes must be checked against actual instructions,
callers, memory layouts and runtime observations before translation.

## What the game currently builds

The root CMake project adds `engine/`. The shipped game targets include
`igi_gameplay`, `igi_formats`, `igi_assets`, `igi_runtime`, `igi_input`, and
`igi_game`. They do not consume the external 5,732-entry catalog automatically.

The current source has no `engine/include/igi/native_registry.hpp` or
`engine/src/native_registry.cpp`, and defines no `igi_native_probe` target.
`attic/legacy-tools/native_probe.cpp` is historical source referring to the
missing registry; it is not a working current build command.
The optional `if(TARGET iginatives)` clauses do not create such a target.

The scripting module has:

- `engine/include/igi/scripting/native_function_registry.hpp`
- `engine/include/igi/scripting/script_interpreter.hpp`
- `engine/src/scripting/native_function_registry.cpp`
- `engine/src/scripting/script_interpreter.cpp`

Its `igi_scripting` target currently exists under `BUILD_TESTING`. This registry
binds script names to C++ callbacks; it is different from a binary address catalog.
Tests execute original Level 1 script branches with recording callbacks. They do
not implement production alarm actions, task scheduling or complete mission logic.
The game still uses static AI patrol extraction, which cannot replace event-driven
script execution.

## Analysis tools and verification

Recent analysis used the local REA checkout with Ghidra **12.1.2**, independent
radare2 disassembly, and the Binary Ninja MCP endpoint
`http://127.0.0.1:24642/mcp` with authorization disabled. A past successful call
does not establish that a provider or endpoint is connected now; check availability
when starting a new capture. Frida/IGIProbe and CrossOver are runtime tools whose
successful attachment and trace coverage must be established separately.

The previously documented `scripts/rea_runner.sh` is absent from this checkout.
Use the local toolchain directly, for example:

```sh
node /Users/haseeb-mir/Documents/Code/rea/scripts/rea.mjs providers --format json
GHIDRA_INSTALL_DIR=/opt/homebrew/opt/ghidra/libexec \
  node /Users/haseeb-mir/Documents/Code/rea/scripts/rea.mjs \
  function /Users/haseeb-mir/Downloads/IGI/igi.exe 0x4bd430 \
  --provider ghidra --format json
```

For absent Ghidra function definitions, record the failure. Verify entry boundaries
and references before using an exact raw instruction window with an explicit load
address. Raw-window evidence must retain its parent executable offset and hash;
it must not be presented as full-image analysis or live runtime verification.

## Evidence and acceptance

- [Signed operand verification](fidelity/qvm-integer-operands.md)
- [Script runtime provenance and limits](fidelity/script_runtime_provenance.md)
- [Integer arithmetic correction](fidelity/script_integer_arithmetic.md)
- [Current gameplay status](../engine/MVP_STATUS.md)
- [Level 1 acceptance scope](../engine/MVP_PLAN.md)

Level 1 is incomplete. Catalog discovery, passing functional tests and CFG
agreement do not satisfy the required original-versus-port gameplay comparisons.
Sound and cutscenes remain deferred under the current milestone.
