// Native: FP_SetDefaultPrecision; retail VA 0x004A7157; signature x86 cdecl uint32(void).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja 0x004A7157; RE-Agent Radare2 ev_aafca3332923a2bee890a444e154b09ee31957d53d2041fdae7baf0cc9c69168;
// recovered through PE entry 0x004A50F5 -> 0x004A5AA8 -> [0x00543E8C] -> 0x004A5363 -> 0x004A7157; unicorn differential replay
// over eight control words by scripts/fpu_control_word_diff_runner.py (8/8), spawn-mode paired capture observes one startup call.
// Behavior/CFG: the CRT startup selects 53-bit precision. The body loads the request (flags 0x10000, mask 0x30000) and
// forwards to the no-denormal merge helper, which reads the live x87 control word, merges the converted flags under the
// mask, loads the resulting word and returns the merged flags. The only input is the x87 control word, which is why the
// differential drives that word instead of inventing stack arguments.
// Name: the catalog calls this address Camera_ApplyBandFilter_10; the body performs no camera work and the same address is
// reached from the CRT startup chain, so the FPU identity is the accepted one.
// Status/limits: canonical reference transcription for the record's engine implementation
// engine/src/retail/core/fpu_state.cpp. Not compiled into the candidate build.
#include <cstdint>

std::uint32_t FP_MergeControlWordNoDenormal(std::uint32_t flags, std::uint32_t mask);

std::uint32_t FP_SetDefaultPrecision()
{
    return FP_MergeControlWordNoDenormal(0x10000U, 0x30000U);
}
