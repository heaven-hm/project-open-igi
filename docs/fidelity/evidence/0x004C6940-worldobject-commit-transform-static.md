# `WorldObject_CommitTransform` (`0x004C6940`) static/runtime gap

## Retail identity

- Image: `/Users/haseeb-mir/Downloads/IGI/igi.exe`
- SHA-256: `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`
- Format/architecture/base: PE32, x86, image base `0x00400000`
- Binary Ninja view: PE/x86/windows-x86; function `0x004C6940..0x004C6B1A`, 475 bytes, 13 basic blocks. Prototype returned by BN: `int32_t(void* arg1, void* arg2, double* arg3, float arg4)`.
- Radare2 6.2.0 evidence: REA evidence `ev_c4fff8e0631b9544e736488f2fe9fb6387312208e1cf7adefb7744f558021a3f`, complete 150-instruction decode request over the same SHA-256. The entry establishes x86 stack arguments and calls `0x004B3360`; decode alone does not establish a CFG.

## Structure observed in Binary Ninja

The function compares the incoming transform vector at `arg3` against the prior vector at `arg2 + 0x20` by calling `Sys_ResolveSymbol_11` (`0x004B3360`). An unchanged vector follows the float comparison on `arg4` versus `arg2 + 0x44`; otherwise, the update path:

1. Quantizes the supplied radius and six transform coordinates through x87 integer conversions.
2. Adds the world origin fields at `arg1 + 0xDE0`, `+0xDE4`, and `+0xDE8` to the new position.
3. Writes the transform components to `arg2 + 0x20..0x34` and the radius to `arg2 + 0x44`.
4. Computes dynamic-cube bounds using `DynObj_LinkDynamicCube_7` (`0x004C67E0`), inserts/updates the hash structure using `DynObj_InsertHashItem_8` (`0x004C6CE0`) and possibly `DynObj_FindSymbol_8` (`0x004C6B20`), then returns through `DynObj_LookupHashItem_8` (`0x004C6D20`).

Binary Ninja reports 84 callers, including `HumanPlayer_CheckGround` (`0x0040E740`), `Human_MovementIntegrator` (`0x004614B0`), `Fence_Update` (`0x0043FEA0`), and `Building_Process` (`0x00448890`). This makes the native directly relevant to player/world contact and object placement. This summary is structural evidence, not proof that a candidate implementation is equivalent.

## Candidate/runtime status

- `scripts/GameNatives/worldobject/004C6940-WorldObject_CommitTransform.cpp` is a decompiler-style candidate with documented unresolved semantics; it uses raw byte pointer arithmetic and is not directly compilable as C++.
- There is no corresponding engine implementation under `engine/src`, `engine/include`, or `engine/platform/windows` at this inspection. The paired Level 1 Frida capture at `build/runtime-traces/world-level1-20261020T175331Z/comparison.json` reports `WorldObject_CommitTransform (no engine implementation symbol)` on the candidate side.
- Retail runtime capture observed 47,281 calls to this native during the recorded Level 1 interval. The candidate had no mapped implementation, so there is no paired argument/return/state comparison. Object-transform parity is unverified.
- The identified callees and their context layouts must be reconstructed from retail evidence before integrating this native. Do not replace the missing path with an independently invented transform/index algorithm.

## Next evidence required

Recover and compare the exact caller/callee state for this native using the same Level 1 scene on both executables. Resolve the ABI and pointed-to structure fields from retail bytes and runtime observations, verify the CFG against Radare2 and angr, then implement and run the focused typed differential before treating object placement or player/world interaction as accepted.
