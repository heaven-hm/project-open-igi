# Allocator differential observations

Checkpoint: 2026-10-19. This improves verification tooling; no native implementation or verification status is changed.

The allocator harness executes retail PE32 instructions and a freshly compiled MinGW x86 candidate through angr/VEX. The retail image SHA-256 is `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`. Allocation boundaries return a shared synthetic address or null. The candidate adapter installs the fixture inputs before calling the native.

In addition to exact returns and requested allocation sizes, the harness observes final values of six globals: `0x00936250`, `0x00936254`, `0x00C3242C`, `0x00C32428`, `0x00C321FC`, and `0x005466D4`. An executed x86 adapter resolves the corresponding candidate storage; the harness reads actual final memory. The reported before-values are configured fixture inputs, not a separate pre-call memory measurement. A 64-byte synthetic guard initialized at the allocation address is also read after execution. Recorded allocation requests provide boundary-event observations.

The actual gate launcher accepted all three updated reports: 13 cases for `0x004AA670`, 16 for `0x004AA682`, and 29 for `0x004AA6AE`. All 58 passed. Two intentional candidate mutations confirmed sensitivity:

- Changing the pool-threshold global in Mem_AllocChunk was rejected in all 13 cases despite identical return values.
- Changing the first allocation byte was rejected in all 12 successful-allocation cases despite identical returns; the null-allocation case remained unchanged.

Generated records are in `build/gate-repair/allocator-gate-launch.json`, `allocator-global-mutation-report.json`, and `allocator-memory-mutation-report.json`. The latter two compile temporary mutated copies; production source is untouched. The 28 Python regression tests also pass. Independent read-only review confirmed the observations and their limits.

This does not establish complete memory effects, real heap behavior, custom-pool branches, successful-new-handler retries, or live game integration. Stale implementation and semantic-review bindings remain blocking. Run the configured `scripts/native_abi_diff_runner.py` through `scripts/native_gate.py` for fresh observations; the strict production command remains `./build.sh`.
