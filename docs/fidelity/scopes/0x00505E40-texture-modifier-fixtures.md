# TextureModifier_Push fixture scope (0x00505E40)

This scope covers exactly the twelve named replay inputs in `docs/fidelity/evidence/texture-modifier-push-20261020/after.json` and the ten nonfatal inputs invoked through the same Frida probe in `level1-paired.json`, `level4-paired.json`, and `level7-paired.json` in that directory. It does not assert equivalence for every valid context or every stack counter.

The replay compares return values, the full 512-byte context, the 80-byte modifier stack, count and packed mask. Two fatal fixtures observe the error-message argument and an unconditional self-loop after a modeled Error_Show return; Error_Show internals are outside this scope. Live tests call the native explicitly on Frida agent threads before main-thread resume and verify native-global restoration.

Baseline replay: 2/12 passed. Corrected replay: 12/12 passed. Live comparisons: 10/10 per level, 30 paired cases in total. No natural calls occurred in the short observation windows on either executable. Natural invocation, host wiring, terrain/material/texture appearance, arbitrary aliases, invalid pointers and negative stack counters remain unverified. Three level arguments do not prove scene integration.

The independent critic reviewed the DWORD writes/reload ordering and both probe corrections. Build evidence was refreshed by compiling the current source for Windows x86. Exact binary/source/tool identities are retained in the replay, build and live reports.
