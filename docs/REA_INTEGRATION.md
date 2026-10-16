# Project Open IGI — REA (re-agent) Integration Guide

This document describes the integration between **Project Open IGI** and the **re-agent (REA)** autonomous reverse-engineering engine located at `/Users/haseeb-mir/Documents/Code/rea` ([heaven-hm/re-agent](https://github.com/heaven-hm/re-agent.git)).

---

## 1. Overview & Architecture

REA (`re-agent`) provides an evidence-backed reverse engineering foundation combining:
- **Ghidra Headless Analyzer** (v12.1.3 via `ReaGhidraBridge.java` and `ReaGhidraNoReturnFix.java`)
- **Radare2 Engine** (CFG analysis, cyclomatic complexity, basic block gate scoring)
- **Binary Ninja MCP Integration** (Type systems, AST decompilation, variable renaming)
- **Frida & LLDB Tracers** (Runtime execution profiling, INT3 probes, instruction tracing)

```
   Retail IGI.EXE (PE32 x86, 1.38 MB)
                 │
                 ├──► REA Artifact Inspector (SHA-256, PE Header, Resource Table)
                 │
                 ├──► Ghidra 12.1.3 Headless + ReaGhidraBridge
                 │
                 ├──► Radare2 CFG Validation Gate (Threshold >= 0.85)
                 │
                 └──► Binary Ninja Decompiler + Type System
                               │
                               ▼
               IGINatives_VERIFIED.json (5,732 Natives)
                               │
               ┌───────────────┴───────────────┐
               ▼                               ▼
       formats/ (CSV, MAP, IDC)      C++20 Native Registry
                                  (engine/include/igi/native_registry.hpp)
```

---

## 2. Using the REA Runner in Project Open IGI

A dedicated runner is available in `scripts/rea_runner.sh` that automatically sets `GHIDRA_INSTALL_DIR` and binds to the local REA installation:

```sh
# Inspect PE artifact, SHA-256, and header
./scripts/rea_runner.sh inspect-igi

# Inspect PE embedded resources (icons, dialogs, version)
./scripts/rea_runner.sh resources-igi

# Run full Ghidra headless analysis against IGI.EXE
./scripts/rea_runner.sh analyze-igi

# Run any arbitrary REA command
./scripts/rea_runner.sh providers
./scripts/rea_runner.sh --help
```

---

## 3. Verified Native Coverage Summary

The integration achieves **100.00% coverage of the Binary Ninja binary view** (5,732 / 5,732 functions) and **93.72% coverage of the radare2 binary view** (5,732 / 6,116 functions):

| Engine Subsystem | Total Natives | CFG Gate PASS (>=0.85) | CFG Gate FAIL (Thunks/Null) |
|---|---|---|---|
| **System / Math** | 1,867 | 1,694 | 173 |
| **Render / Display** | 946 | 833 | 113 |
| **AI Engine** | 702 | 656 | 46 |
| **Physics & Vehicles** | 434 | 397 | 37 |
| **Script / QVM** | 370 | 338 | 32 |
| **Weather & Environment** | 341 | 323 | 18 |
| **UI & Menu Engine** | 325 | 308 | 17 |
| **Weapon Engine** | 316 | 298 | 17 |
| **Sound Engine** | 313 | 285 | 28 |
| **Animation Engine** | 118 | 112 | 6 |
| **Total** | **5,732** | **5,244** | **487** |

---

## 4. Querying the Native Registry in C++20

The C++20 engine includes a built-in diagnostic probe (`igi_native_probe`):

```sh
# Display registry statistics and subsystem breakdown
./build/engine/igi_native_probe --stats

# Lookup function by memory address
./build/engine/igi_native_probe 0x401000

# Lookup function by semantic name
./build/engine/igi_native_probe --name Config_SaveSoundOptions

# Filter all natives in a subsystem
./build/engine/igi_native_probe --subsystem Weapon
./build/engine/igi_native_probe --subsystem Sound
./build/engine/igi_native_probe --subsystem Physics
```

---

## 5. Artifact Directory Layout

```
project-open-igi/
├── assets/
│   └── BinaryNinja/
│       ├── IGINatives_VERIFIED.json      # Complete 5,732 verified native database
│       ├── C11/                          # ANSI C11 headers and bodies
│       ├── formats/                      # CSV, MAP, IDC, JSON symbol tables
│       └── natives/                      # Per-native headers (.h), bodies (.c), and docs (.md)
├── BinaryNinja/                          # Mirrored root catalog
├── engine/
│   ├── include/igi/native_registry.hpp   # C++20 Native Registry API
│   ├── src/native_registry.cpp           # Streaming loader & fast binary search
│   └── tools/native_probe.cpp            # Diagnostic CLI tool
├── scripts/
│   ├── rea_runner.sh                     # REA engine launcher
│   └── discovery/                        # Analysis, verification, and export scripts
└── docs/
    └── REA_INTEGRATION.md                # This document
```
