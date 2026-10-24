# DeepSeek captain handoff

Paste the following into the captain's session. Give workers only their current task packet plus the common evidence rules, rather than copying the entire repository into every context.

```text
You are captain of the Level 1 gameplay MVP team.

Work only in /Users/haseeb-mir/Documents/Code/project-open-igi on
mvp/level1-integration. Create no worktree and do not switch branches.
Read AGENTS.md, MVP.md, tasks/plan.md and tasks/todo.md before assigning work.
MVP.md is the current owner-requested product scope. Older inspection-only
scope paragraphs are historical. Keep the mandatory evidence protocols.

FIRST DELIVERY / CURRENT MVP: original Level 1 3D models, terrain, textures and
placements load; player spawn/camera/controls, walk/run/crouch, jump/landing,
physics/collision and a required interaction work. P17/M2 ends this delivery;
close it with the P25 evidence packet. No loading bar, text or main menu is
required to accept the first playable version.

LATER ORDER: P18 sound -> P19/P20 enemies -> P21/P22 weapons/combat -> P23 actual
mission completion -> P24 original main menu/loading bar/loading text.
Do not build all later features before declaring the first playable milestone.
Enemy spawn/patrol/perception can precede weapons; armed encounters wait for
weapons. Unavoidable early initialization dependencies must still be recovered
and verified, rather than silently skipped or stubbed.

This is DECOMPILATION RECONSTRUCTION of retail IGI1. Preserve recovered original
native algorithms, ABI/layouts, calculations/quirks, branches, calls, state and
error/cleanup behavior. Ports and generated pseudocode are reference hypotheses.
Maintain actual retail entry -> startup -> direct-Level1 -> simulation/event ->
cleanup/exit method maps, required callees and candidate symbol/RVA bindings.
Compare original deterministic call order/branch/dispatch/state effects on the
supported path; mark deferred/unobserved paths uncovered. Do not claim the full
main-to-last-method game graph already matches when only a stage was checked.

HARD GLOBAL LIMITS: at most 3 active agents total = captain + 2 worker slots,
including team members and nested subagents. Maximum 3 simultaneous model calls,
never 4. Rotate the researcher/implementer/critic/runtime roles through those
slots; queue extra roles. Use one shared model-provider limiter: at most 30 API
request attempts in any rolling 60 seconds across ALL agents, including retries.
Reserve a global slot before dispatch, pace aggregate calls >=2.1 seconds apart,
and respect Retry-After/central backoff on 429. Never allocate 30 RPM per agent.
Configure/verify these in your actual harness before spawning; Markdown alone
does not enforce provider limits. No nested agent may bypass the captain slots.

Start with P00–P04 in tasks/plan.md. The repository has case-variant path
collisions and disputed evidence. Preserve Git/index/worktree variants and
raw reports before fixing paths. Read original Git blobs for colliding files;
filesystem reads alone cannot distinguish them on this macOS volume. Do not
bulk-add dirty evidence, invent a pass, or rehash stale artifacts to bless them.
The audit established zero natives with both current-source static evidence
and paired runtime against the current candidate; inherited labels are not
acceptance. Some work may be reusable after exact binding checks.

Binary Ninja MCP: http://127.0.0.1:24642/mcp, authorization disabled.
The retail database is already open and PE view_1 was activated. Confirm active
view/retail identity before analysis. A live handshake, 75-tool listing and
16-byte retail match succeeded. Do not treat this as native verification.
Use one selected decompiler per native: BN preferred, Ghidra only if unavailable
for that target. Also use Radare2 CFG, angr, semantic review, behavioral checks
and reviewed typed paired runtime as required by AGENTS.md. Every implemented
native/helper must have static checks AND its own focused Frida/IGI-Probe typed
comparison against original and current candidate: inputs, return, affected
state/memory and side effects, known branches and edge/error behavior. Publish
linked/implemented/required/static/runtime/accepted counts and missing addresses.
Claim 100% native-set coverage only if the full declared implemented set has
both checks; untested linked methods stay visible. Finite probes cannot prove
all possible paths or a byte-identical executable. Do not fabricate that claim.

You own task assignment, Git/index, manifests, shared config/CMake, build and
candidate deployment. Grant only one source implementation writer at a time.
Within the two worker slots, parallelize static/task/asset research and review.
Workers must
have a task ID, exact source/address scope, allowed files and definition of done.
They must preserve others' changes and return a concrete evidence packet.
A critic must be independent of the implementation author.

Appoint one runtime operator. Use the shared runtime lease; freeze source,
manifest, scenario, probe and candidate during capture. Retail first, candidate
second, same game cwd/level/config/actions/probe. Never terminate another user's
game or overlap captures. No worker independently rebuilds/copies the candidate.
Compile-only objects do not have candidate runtime coverage until linked.

Advance one declared native/feature slice at a time. Build current x86 source,
verify its receipt/symbol mapping, compare required typed inputs/outputs/state
changes and fix the earliest divergence. Preserve all failures. Missing tools,
unknown ABI, unobserved required calls, stale hashes, dropped sequences and
comparison mismatches block acceptance. Never approve a broader change using
a convenient one-address test or an empty changed set.

Update tasks/todo.md after each completed or blocked packet. Record exact next
action and unblock condition. If the same failure repeats without new inputs,
stop repeating it and work its dependency or another ready research task.
Do not spend unlimited runs repeating the first sorted native. Do not promise
universal 100% equivalence from finite scenarios.

At each milestone return: source revision/tree, candidate hash/receipt, required
native closure, evidence and scenario reports, independent findings, exact
accepted scope, open blockers and next ready task. Keep diagnostic builds and
the explicit historical diagnostic commit override out of acceptance claims.
Do not push or publish a release as part of this assignment.

Begin now: baseline preservation/path audit as P00; assign parallel read-only
research for Level 1 closure and compile-only dependencies. Then execute the
ordered plan autonomously, with a single implementation writer.
```

## Handoff — round 140 (matching decompilation of igi.exe, mvp/level1-integration)

State: 28 natives landed, compiled (receipt compiled_sources 134) and Tester-verified; startup fails
closed at QTask_RegisterClasses, position 1 / id 0, exit 3. Last frozen pair `e0b4e70b`
(sha256 e0b4e70b97ac10ac948e03c9cee988b9060215aa7090ce6387787cff2e5c4bdc), deployed and verified.
No playable Level 1; no release, version, screenshot or window claim.

Landed this arc (all exact CFG, 1.000 unless noted): 0x4F1B20, 0x4F1B50, 0x4F1840, 0x4F18C0, 0x4F19C0,
0x4F1A40, 0x4F1BC0, plus the earlier hash/container/error group. Storage ownership is settled:
0xA758E0 (11 slots x 19 dwords, used flag dword 18, parent dword 1) owned by 0x4F1840; 0xA758C8
16-slot table owned by 0x4F1B20; 0x54E0E4..EB owned by 0x4F1A60.

Remaining chain, bottom-up (ownership-driven, each needs the one below):
1. 0x4EE970 (3452 B catalogue) - block at 0x4EF565..0x4EF5A0 registers "Graph" via 0x4F19C0(-1, "Graph")
   and stores the handle at 0xA70CA0; it also reads 0xA70C90 and calls 0x4F1BC0/0x4F1B60.
2. 0x4F1020 (6 B getter of 0xA70CA0) - linkable once 1 lands.
3. 0x4FAA60 (837 B stream init, owner of 0xA774A0/0xA76D18) + 0x4F9DF0 (120 B emitter) - mutual pair,
   must land together; both listings captured in docs/fidelity/evidence/.
4. 0x5183D0-region init sequence (calls 0x4EE970 from 0x51840C, 0x4FAA60 from 0x518484, 0x5186C0).
5. 0x508FA0 (755 B registrar, 42 call sites, listing captured) + 0x508F80 thunk (call 0x5183D0; jmp 0x508FA0).

Method rules learned the hard way this arc:
- Boundaries: radare2 `af` accepts a seeded address as an entry, so seeded extents lie. Reliable
  technique is "find preceding c3 90 90 90, then skip nops"; when that fails, ask the Tester for a
  retail-only Frida call-target probe (this resolved 0x4FAA60 and 0x4EE970 in one step each).
- Ownership: a global is declared only by its retail writer; readers go extern. This is what keeps
  blocking attempted out-of-order transcriptions - and it is why 0x4FAA60/0x4F9DF0 cannot be written
  before 0x4EE970.
- Landing: a native counts as landed only when it appears in receipt compilation.compiled_sources,
  not inputs.sources. 18 natives were silently unwired for several rounds because a str.replace
  anchor never matched; always verify the anchor and print the count before/after.
- Strings must stay byte-identical; non-obvious retail orders (0x4F1840's EB,EA,E7,E8,E9,E6 slot
  stores) and sentinel values (0x181, 0xFF) are preserved, never normalised.

## Round 148-149 (resumed after the blocked round): 36 natives, frozen d136e0b3

- Landed this arc: `0x4F1B60`, `0x4F1B70`, `0x4F1B80`, `0x4F1B90`, `0x4F1BA0`, `0x4F1BB0`
  (six parameter-event state getters over `g_54e0e4[8]` owned by `0x4F1A60`, indices 6/2/7/5/3/4),
  `0x4B8890` (QHash node register wrapper forwarding to the landed `0x4BAAC0` as
  `(arg0, arg1, arg2, arg3, 1, 0, 0)`), and `0x4EE960` (`arg0 % 251` unsigned bucket helper).
  All bind 1/0 at 1.000.
- Frozen and deployed: `d136e0b3` (`d136e0b30ca55470bf01e0635ad6844b52e3202ae56a21188a5f42ce048d0933`),
  `compiled_sources` 142, Tester-verified: binding confirmed independently, boundary unchanged
  (position 1 / id 0, exit 3), the eight new VAs absent from stderr and the `+seh` unwind. One
  unexplained cold outlier (28.6 s) that did not reproduce (6.17 s / 10.11 s).
- `0x4EE970` (catalogue, `0x4EE970`-`0x4EF7EC`, ~1000 instructions) has a full captured listing at
  `docs/fidelity/evidence/0x4EE970-full-listing.txt` (1010 lines). Its call inventory is complete and
  every callee is landed EXCEPT three: `0x4CEA10`, `0x4CEA20` and `0x4F1180`.
- `0x4CEA10`/`0x4CEA20` are 6-byte getters of `0x548176`/`0x548177`; those bytes are written at
  `0x4C5983`/`0x4C5992` inside an as-yet-unbounded registration routine in `0x4C58xx`-`0x4C59xx`
  (static boundary recovery fails there; a retail call-target probe was requested from the Tester).
- `0x4F1180` (27 B) chains: `0x4F11A0` (68 B) -> `0x4EE960` (landed) + `0x4F0C90` (31 B) ->
  `0x401B20` (98 B, virtual-array free-list over `0x567C38/3C/40/48/4C`) + `0x4B0D10` (42 B) ->
  `0x4A6A50` (105 B) -> the allocator internals `0x4AAD90` (43 B), `0x4AADBB` (809 B),
  `0x4AB7F5` (87 B), `0x4AB84C` (69 B). Those internals touch the mem subsystem already partly
  transcribed in `engine/src/retail/mem/` (`0x4AA670`, `0x4AA682`, `0x4AA6AE`, `0x4AD722`,
  `0x4AF8F0`, `0x4AF960`, `0x4B0BA0`, `0x4B0C60`, `0x4B0D40`, `0x4B0D50`, `0x4B0D60`, `0x4B0D90`,
  `0x4B0B00`), so the allocator block is the next real frontier.
- New tool: `scripts/linkable_frontier.py` lists routines in a range whose direct callees are all
  landed. It works well in regions of small padded routines (it enumerated the six `0x4F10xx`
  getters correctly) but its `ret`+nop splitter mis-groups dense big functions (it reports
  696/1827-instruction bodies in the `0x4AA000`-`0x4AC000` allocator region), so treat its output
  there as a lower bound, not boundaries.
- `0x4F1000`, `0x4F1010`, `0x4F1020` are 6-byte getters of `0xA70C90`, `0xA70C88`, `0xA70CA0`; all
  three globals are written inside `0x4EE970` (at `0x4EF4DA`, `0x4EF767`, and the `"Graph"` handle
  store), which is exactly why the catalogue must land before them.

## Round 150: registration chain landed; frozen b59ecbc9; two integrity findings

- Landed and compiled: `0x4C5900` (QTask_InitPhaseHandlers, owner of `0x548176`/`0x548177` and the
  17 dwords at `0xA44348`), `0x401D30` (phase/slot byte setter + tail `jmp 0x4014B0`),
  `0x4CEA10`/`0x4CEA20` (the two state getters), and `0x401860`, `0x4D9730`, `0x4E5370`, which had
  provenance files but no CMakeLists entry and had therefore never been compiled.
- `0x4C5900`'s entry and zero-argument ABI came from a retail-only Frida range probe: entered by a
  real `E8` at `0x531DB5` (return `0x531DBA`), 0 stack arguments, eax 0; both writers
  (`0x4C5983`, `0x4C5992`) are fallthrough inside it, and `[esp]` at those sites holds the constant
  pushed at `0x4C597E` and the callback pointer pushed at `0x4C598D` - not return addresses.
- `0x401D30` binds r2=6/7, score **0.058**, because retail ends in an unconditional
  `jmp 0x4014B0` and r2's seeded recovery follows the jump into the callee. The transcription keeps
  both byte stores and calls the landed `TaskType_RefreshHandlerMask`; retail leaves the two cdecl
  arguments for the caller's `add esp,0x40` while the call pops them. Status stays TRANSCRIBED with a
  documented structural divergence; do not present it as verified and do not "fix" the score.
- Frozen pair: `b59ecbc9`
  (`b59ecbc968179ef990fa29623e9988d466f121135b15a933f2a52c57a3c4b013`), `compiled_sources` **149**
  after a clean rebuild; deployed hash-verified; Tester asked to re-verify independently.
- FINDING (important for freezing): `scripts/candidate_build_receipt.py` collects any `*.obj` in
  `build-win32-no-verify-gate/CMakeFiles/igi.dir` that still has a matching `*.obj.d`, so objects
  orphaned by a wire/unwire experiment keep counting toward `compiled_sources`. A mass-wiring trial
  left 41 orphan objects and inflated the count to 187. Always `rm -rf build-win32-no-verify-gate`
  and rebuild before freezing, and treat any count jump that does not match the CMakeLists delta as
  this bug, not as landed natives.
- FINDING (open task): 41 native sources under `engine/src/retail` have no CMakeLists entry, so they
  are not compiled. Mass-wiring them fails the link with duplicate definitions (`g_548638`,
  `g_54875c`) and undefined globals (`g_5c8e1c`, `g_c28f80/81`, `g_5c8e0c/08/10`). This is a separate
  integrity task: each needs its ownership resolved before wiring. `engine/src/retail/input/native_48fa80.cpp`
  was patched in this round only to make its case labels compile (narrowing); it is still unwired.
- Still missing for `0x4EE970`: `0x4F1180` and its lookup chain (`0x4F11A0`, `0x4F0C90`, `0x401B20`,
  `0x4B0D10`, `0x4A6A50`, then the allocator internals `0x4AAD90`, `0x4AADBB`, `0x4AB7F5`,
  `0x4AB84C`). `0x4CEA10`/`0x4CEA20` are now landed, closing the other two-thirds of that gap.

## Round 148-149: pool chain started; frozen c7450b78 (151 sources, Tester-verified clean)

- Landed: `0x4AAD48` (small-block pool initializer, owns `0x00C321E4/EC/F0/F4/F8`, `0x00C321FC`;
  bn=3/2 r2=3/2 **1.000**) and `0x4AAD90` (pool entry finder, bn=6/7 r2=6/7 **1.000**). Storage is
  reached through `MemGlobals` accessors in `engine/include/igi/retail/mem/mem_alloc.hpp`, which
  gained `pool_slot_count_c321e4`, `pool_block_c321ec`, `pool_used_c321f0`, `pool_next_c321f4`,
  `pool_base_c321f8` rather than new standalone globals - the verified `0x4B0C60` native already
  reads the same struct.
- `0x401D30` corrected: `88 88 e0 69 a9 00` is `mov [eax+0xA969E0], cl`, i.e. the low byte of arg2
  (slot), NOT of arg1. The first store (`88 81 ...`) does use `al`. Still r2 6/7 score 0.058 because
  r2's `af` merges the retail tail `jmp 0x4014B0` (`minaddr 0x4014B0`, size 2206). The Tester bounded
  the true structure at 1 block / 0 conditional branches / 1 outgoing tail edge and confirmed the
  6/7 is a tail-merge artifact, but did not produce a re-score: **open reviewer item**, do not accept
  `0x401D30` on the 0.058 number and do not "fix" the score by editing the verifier.
- Frozen `c7450b78` (`c7450b787c8d2796dea2b8b0c0025674cfa9b1514fb80915b9f6173769f94cf1`),
  `compiled_sources` **151**, clean rebuild (no orphan objects - Tester confirmed 151 obj / 151 obj.d /
  0 orphan / 0 missing), deployed hash-verified, boundary unchanged (position 1 / id 0, exit 3), the
  two pool natives absent from stderr and `+seh`.
- NEXT: `0x4AADBB` (809 bytes, ~340 instructions) - the pool allocator proper. Reading has begun; it
  uses the heap header at `[ecx+0xC]`/`[ecx+0x10]`, a `0x204`-byte stride, a `0x144` offset, bit tests
  on the low bit of header words, a `0x3F` clamp on `(block_word >> 4) - 1`, `[ebx+4] != [ebx+8]`
  free-list checks, and a `0x80000000` mask (r2 renders it as the `DirectSoundCreate` import symbol,
  which is just an address constant here - do not treat it as a call). Then `0x4AB7F5`/`0x4AB84C`
  (mode-2 pair), then `0x4A6A50` (heap-free dispatcher, body already fully read and documented:
  mode 3 -> `0x4AAD90`+`0x4AADBB`, mode 2 -> `0x4AB7F5`+`0x4AB84C`, else KERNEL32 `HeapFree` on
  `g_c32428`), then `0x4B0D10` (`Mem_DeAllocFn`), which finally unblocks `QTaskList_Delete` (`0x4C1830`,
  documented as blocked in `engine/src/retail/core/qtask_list.cpp`) and the `0x4F1180` branch of the
  `0x4EE970` catalogue.

## Round 150: mode-2 bucket finder landed; sentinel PE defect caught and fixed

- Landed `0x4AB7F5` (`engine/src/retail/mem/native_4ab7f5.cpp`), bn=9/12 r2=9/12 **1.000**: walks the
  bucket list headed by the `0x005446B0` sentinel for `[node+0x10] < value < [node+0x14]`, rejects
  unaligned values and `(value & 0xFFF) < 0x100`, then writes the node and `(value & 0xF000)` through
  two out-pointers and returns `(value - base - 0x100)/16 + base + 8`. The `0x100` immediate is retail's
  own operand - r2 renders it as a PE symbol name, exactly like the `0x80000000` mask in `0x4AADBB`.
- **Sentinel PE defect (Tester-caught):** the first version hard-coded retail's `0x005446C8` for the
  sentinel's `+0x08`/`+0x0C` payload pointers. The candidate is `DYNAMIC_BASE` and its sentinel lands
  at `0x00567038`, so `sentinel+0x18` is `0x00567050` in the candidate; the hard-coded retail address
  had no relocation and was stale. Fixed by modelling the payload area as a real trailing member and
  initialising both payload words with `&g_5446b0.payload[0]`; the object's `.data` now carries four
  `dir32` relocations (offsets 0x00/0x04/0x08/0x0C) with addend 0x18. **Rule:** whenever retail static
  data contains pointers, model them as candidate-relative addresses (relocatable), never as
  hard-coded retail VAs, unless the field is genuinely a non-pointer constant. Self-links via `&object`
  are correct; hard-coded image VAs are not.
- **Rule that worked twice:** the Tester's independent disassembly/PE read caught two real defects that
  bound 1.000 and would pass static-only review (`0x401D30`'s `cl` vs `al` store, this sentinel). Treat
  a PE/data audit as mandatory for any native that models retail static data or uses `mov r8` stores.
- Frozen sequence this round: `efc3cd15` (152 sources, bucket finder first cut) then `18103c6e`
  (`18103c6e63c4cf61858408d8195c3da6154e92f3c1565458319347f824fb7437`, 152 sources, sentinel fixed),
  both deployed hash-verified; Tester asked to re-audit the relocations on `18103c6e`.
- Next: `0x4AB733` (194 bytes, 20 blocks - the bucket purge; body uses VirtualFree(addr, 0x1000,
  MEM_DECOMMIT) at `0x4AB771`, `0x3ff000`/`0x1000` strides over a `0x2010`-byte node, `[0x936200]`
  counter, and `[esi+0x0C]`/`[esi+0x18]` bound maintenance), then `0x4AB84C`, then `0x4AADBB`,
  `0x4A6A50`, `0x4B0D10`.

## Round 151-152: mode-2 bucket heap complete; frozen 770d5b8d (156 sources)

- Landed the whole mode-2 bucket set: `0x4AB6DD` (node free, bn=5/5 **1.000**), `0x4AB733` (page purge,
  bn=20/33 **1.000**, matching retail's 20 blocks), `0x4AB7F5` (finder, 9/12 **1.000**), `0x4AB84C`
  (page marker, 4/5 **1.000**). Layout now lives in `engine/include/igi/retail/mem/mode2_bucket.hpp`
  and the static data (sentinel `0x005446B0`, `0x005466D0`, `0x00936200`) in
  `engine/src/retail/mem/mode2_bucket_globals.cpp`; every pointer is candidate-relative so the linker
  emits relocations. Tester verified: sentinel self-consistent at `0x00567040`, payload at
  `0x00567058`, four HIGHLOW relocations, and no retail VA anywhere in runtime data sections.
- **Decoded traps recorded:** `or dword [edi],0xFFFFFFFF` writes -1 to the slot flag; the 0xF1 store at
  `entry+4` in `0x4AB84C` sits *between* the `cmp` and the `jne`, so it is unconditional; `0x5446C0`
  that `0x4AB6DD` writes is the sentinel's `+0x10` bound word, not a separate global; the `0x2010`
  offsets mean the node is `0x2018` bytes with entries walked backwards from `node+0x2010` to
  `node+0x18`, page `0x3FF-i` for entry `i`.
- **Settled by Tester:** direct `::VirtualFree`/`::HeapFree` calls are the fidelity-correct choice; a
  `Mem_*` wrapper would be a non-retail abstraction inside a native flow.
- Frozen `770d5b8d` (`770d5b8deaa6f1f89da35f928899de98e82cdd092c8e7e0c9733eaabc96b47eb`), 156 sources,
  clean rebuild, deployed hash-verified.
- NEXT (single blocker): `0x4AADBB`, 809 bytes / ~340 instructions, the mode-3 small-block pool
  allocator. Needs a dedicated careful pass: 0x204-byte bucket stride, 0x144 offset, header bit tests,
  `0x3F` clamp on `(word >> 4) - 1`, `[ebx+4] != [ebx+8]` free-list checks, and the `0x80000000` mask
  that r2 renders as the `DirectSoundCreate` import symbol (an address constant, NOT a call). After it:
  `0x4A6A50` (body already fully read: mode 3 -> `0x4AAD90`+`0x4AADBB`, mode 2 -> `0x4AB7F5`+`0x4AB84C`,
  else KERNEL32 `HeapFree` on `g_c32428`), then `0x4B0D10` (`Mem_DeAllocFn`), which unblocks
  `QTaskList_Delete` (`0x4C1830`, documented blocked in `engine/src/retail/core/qtask_list.cpp`) and
  the `0x4F1180` branch of the `0x4EE970` catalogue.
