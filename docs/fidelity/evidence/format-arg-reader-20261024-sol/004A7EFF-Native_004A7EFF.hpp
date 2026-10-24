#pragma once
#include <cstdint>
#define NATIVE_DECL __cdecl
// Native: Native_004A7EFF; retail VA 0x004A7EFF; declaration only.
// Artifact: igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja, Radare2, angr; docs/fidelity/evidence/format-arg-reader-20261024-sol/.
// CFG/behavior: one 13-byte block in the CPP; argument addresses a cursor cell; result is raw DWORD bits.
// Status/limits: provisional ABI declaration; original identifiers and signedness unknown; no production intake.
extern "C" std::uint32_t NATIVE_DECL Native_004A7EFF(std::uint32_t** cursor);
#undef NATIVE_DECL
