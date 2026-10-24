# Reverse-engineering skills selected for this project, and how they are applied

Owner instruction (2026-10-25): use the best installed reverse-engineering/decompilation skills. This file records which ones were
selected, why, what their prerequisites are on this machine, and what each contributes to the matching decompilation.

## Selected

| Skill | Why it is the right fit | Applied here |
| --- | --- | --- |
| `igi-auto-re-agent` | The only skill written for IGI1 native discovery: mines the retail registrar call sites by backwards-decoding the `(name string, function pointer)` push pair. It is the one place retail hands a function its own name. | Adopted as `scripts/registrar_pairs.py` against this repository's authority image. |
| `igi-validation` | Dual-engine (Ghidra + radare2) control-flow scoring with an explicit reject rule (`score >= 0.85`, up to 16 repair attempts) for "retail does X" claims. | Adopted as the independent-engine gate for natives whose CFG measurement matters. |
| `reverse-engineering` | General technique catalogue for the analysis phases (entry recovery, call-graph reconstruction, ABI inference) when a specific question is not IGI-specific. | Reference only. |
| `auto-re-agent` | The underlying Ghidra/LLM function-reconstruction loop the IGI skill builds on, usable for non-IGI binaries in this workflow. | Standby; not needed while radare2 + measurement answer the question. |
| `reverse-skill-router` | Only an entry-point router; the task is already known to be IGI1 decompilation, so it is not invoked. | Not used. |

## Machine prerequisites, checked

| Requirement | State |
| --- | --- |
| Retail image | `/Users/haseeb-mir/Downloads/IGI/igi.exe`, SHA-256 `dea34892…9915a01`, base `0x00400000` |
| radare2 | present at `/opt/homebrew/bin/r2` |
| Ghidra headless MCP (`127.0.0.1:8771`) | **not answering** - the `igi-validation` decompiler leg is unavailable, so no dual-engine score can be claimed yet |
| `igi-validation`'s image path `/Users/haseeb-mir/Documents/pc/IGI.exe` | absent on this machine; this repo's image is the Downloads copy above |
| `project-igi-internals` catalog + `auto-re-agent` venv | present, but that repo is a separate catalogue; this checkout keeps its own single owner `docs/fidelity/native-verification.json` |

## What the registrar mining produced

`scripts/registrar_pairs.py` scans every `call` into the four retail registrars (`0x4B8890`, `0x4BAAC0`, `0x4B88C0`, `0x4B88F0`),
decodes the preceding pushes backwards (cdecl: the name is the last argument, so the push nearest the call is the name), reads the
C string from the image, and cross-references the function against the manifest.

Result: **167 call sites, 166 unique registered functions, 88 already implemented here, 78 missing**, plus 19 call sites whose pushes did
not decode. Table: `docs/fidelity/evidence/registrar-pairs.md`; machine-readable `build/registrar-pairs.json`.

### The structural fact it establishes

None of the 78 missing named natives is a direct callee of the startup sequence `0x00402930`, and none appears in the call-order walk
(`docs/fidelity/evidence/chronological-native-order.md`). That is expected once you see how retail works: these functions are **script
natives**, registered at startup and then invoked indirectly by the QVM through the registry. Static call-order work will therefore never
reach them, and they must be landed from the script/mission requirements instead - which is exactly the gap that made the candidate stop
at the startup sequence while the level still could not run.

## Next use

1. Land the registered natives the level-1 path needs by name, starting with `Flow_RequestEvent` (`0x00402780`), `GOStart` (`0x00403660`)
   and `Game_SetMission` (`0x00415890`), each with the normal provenance/measurement rules.
2. When the Ghidra headless MCP answers, add its decompile/CFG to the radare2 score for each landed native using the `igi-validation`
   formula; until then the CFG evidence is radare2-only and must be labelled as such.
