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

## Round 152: 0x4AB84C landed and frozen; 0x4AADBB analysis staged

- Landed `0x4AB84C` (page marker, bn=4/5 **1.000**). Mode-2 bucket set is complete: `0x4AB6DD`,
  `0x4AB733`, `0x4AB7F5`, `0x4AB84C` + `mode2_bucket.hpp` + `mode2_bucket_globals.cpp`. Frozen
  `770d5b8d` (156 sources), Tester verified clean binding, unchanged sentinel, boundary unchanged.
- **The one remaining blocker on the free path is `0x4AADBB`** (809 bytes, 265 instructions). The exact
  listing is committed at `docs/fidelity/evidence/0x004AADBB-full-listing.txt` so the transcription does
  not have to re-derive it. Structure identified so far: 0x204-byte bucket stride arrays at
  `[eax + edi*4 + 0x44]` and `[eax + edi*4 + 0xC4]`, a 0x144 offset, per-size free lists unlinked
  through `[node+4]`/`[node+8]` pairs, `0x3F` clamps on `(word >> 4) - 1`, `not`/`shr` bit masks
  derived from the `0x80000000` constant, and KERNEL32 `VirtualFree` for page release. It must be
  transcribed as one unit - it does not compile or bind in halves - so it gets a dedicated pass with
  the whole listing in view.
- **Strategic note for whoever picks this up:** the deliberate fail-closed throw at `QTask_RegisterClasses`
  (`0x00402930`) exists because the AI graph registrar is not transcribed and the class ids after it
  would not match retail. That chain (`0x4EE970` -> `0x4F1180` -> `0x4F0C90` -> `0x4B0D10` ->
  `0x4A6A50` -> `0x4AADBB`) is therefore the actual path past startup, not optional allocator tidying:
  without it the executable cannot reach Level 1 data at all. Do not stub or catch that throw.

## OWNER DECISION (recorded 2026-10-24): CRT library-boundary exception — GRANTED

The owner approved a **CRT library-boundary exception** after it was escalated by the Tester:
routines that are statically linked C runtime primitives (the `memmove`/`memcpy`/`memset` shape:
jump-table-driven `rep movsd`/`rep stosd`, no game globals, imports, strings or object layout, and
callers that are generic copy sites) may be reached through the standard library equivalent instead
of being transcribed as game natives. Such routines are recorded as **CRT-classified, no native, no
CFG claim** - they are never presented as game logic and never given a CFG score.

- First application: `0x4ACD70`, called by `0x4AADBB`'s arena teardown. Evidence for the
  classification (Tester-audited): dst `[ebp+8]` / src `[ebp+0xC]` / count `[ebp+0x10]` returning dst,
  the standard overlap test with a backward `std; rep movsd; cld` path, forward and backward tail
  jump tables at `0x4ACEB8`/`0x4ACDD0`/`0x4ACEC8`/`0x4ACE4C` and `0x4AD050`/`0x4AD000`/`0x4ACF58`,
  821 bytes / 33 blocks, no game references, and exactly three generic copy callers
  (`0x4A7600`, `0x4ACBDC`, `0x4AB0AA`). Reproduced with `std::memmove(pool, tail, limit - tail)`.
- This exception does NOT cover anything that touches game state. A routine with game globals, game
  object layout, imports used for game behaviour, or game-specific semantics must still be transcribed
  as its own native at its retail address. Do not use this exception to skip a game routine.
- Do not write inline-assembly `rep movsd` merely to obtain a CFG number: that would present a CRT
  primitive as game logic, which is exactly what the exception exists to avoid.

## Round 151-152 status and the 0x4EE970 plan

Frozen now: `e915b766` (165 sources), superseding `dfc91818` and `8467e66c`. Tester signed off the data
extents; the BSS gap question it raised was resolved as 32-byte section alignment (array is 0xBC4, section
pads to 0xBE0), not masking.

Two defects were caught by audit rather than by the verifier in this span, both invisible to `cfg_score`:
`0x4F19C0`/`0x4F1BC0` had an 11-slot loop bound instead of 64, and `g_hash_buckets_a70ca8` was declared
`[0x2EF]` instead of `[0x2F1]` (a write-past-the-array waiting for the `0x4EF7C5` initializer). Standing
pre-freeze checklist item: re-derive every literal loop bound and every declared data extent from the
retail instruction that consumes it and cross-check it against the storage declaration.

`0x4EE970` dependencies are confirmed transcribed and wired: `0x4018F0` = `QTask_GetCurrentType`
(engine/src/retail/core/task_types.cpp:501), `0x401900` = `TaskType_Register` (task_types.cpp:360,
273 bytes / 18 blocks), and the `0x4F1B60`..`0x4F1BC0` property family. `0x4EE970` is a flat ~1000
instruction registrar: it clears 0x1000 dwords at 0x00A71890, registers classes through
`0x4018F0`+`0x401900` storing the returned word into 0x0054DD0C upward, and calls `0x4F1BC0` 100 times,
`0x4F19C0` 26 times, and each of `0x4F1B60`..`0x4F1BB0` 16-17 times. Listing:
docs/fidelity/evidence/0x004EE970-full-listing.txt. Transcribe it into
engine/src/retail/script/native_4ee970.cpp in reviewed sections (it must be complete before wiring), and
do not present it as transcribable until the section-by-section pass finishes.

## 0x4EE970 staging: ABI question resolved, data surface mapped (round 151)

**Resolved - no ABI defect.** The registration call site pushes two zero words before `0x4018F0` and
then `add esp,0x14` (five words), while `TaskType_Register` declares four parameters. Reading the callee
settles it: `0x401900` reads only `[esp+8]` (name), `[esp+0xC]` (object size), `[esp+0x18]` (parent type,
after the ebp/esi/edi pushes) and `[esp+0x1C]` (task-list argument). Four arguments. At a call site with
the stack frame `[ebx][ebx][eax|type][size][name]`, the two zeros supply `task_list_argument = 0` and one
extra word that nothing consumes. So the transcription calls `TaskType_Register(name, size,
QTask_GetCurrentType(), 0)` and retail's fifth word is a harmless artifact; do NOT add a fifth parameter.
`QTask_GetCurrentType` (0x004018F0) is a 6-byte leaf: `mov ax,[0x54F928]; ret`.

**Class table owned by 0x401900, not by the catalogue.** Its prologue scans `0x00AF5F05` in 0x30-byte
strides for a zero name byte, bounded at `0x00AFA705`: (0xAFA705 - 0xAF5F05) / 0x30 = 384 slots. Per-slot
fields seen in the found branch: +0x00 name byte (at 0xAF5F05 + slot*0x30), +0x04 and +0x08 (two dwords
stored from args 2 and 1), +0x0C word (parent type, `cmp di,0x181` special case storing 0x200 instead of
copying the parent's word), +0x28 (position passed to 0x4B0B00 with arg4). The catalogue must not define
storage for this table; `0x401900` owns it.

**Data surface the catalogue touches (from the committed listing, all operands extracted):**
- `0x0054DD0C`, `0x0054DD0E`, `0x0054DD10`, `0x0054DD12`, `0x0054DD14` - class ids, word stores, first five
  registrations only (Level, Static, Dynamic, Idle, CameraBase). Then the pattern changes.
- `0x0054DDA4`, `0x0054DDA5` (bytes), `0x0054DDA8`, `0x0054DDAC`, `0x0054DDB0` (dwords).
- `0x00A70C68`, `6C`, `70`, `78`, `7C`, `80`, `84`, `88`, `8C`, `90`, `9C`, `A0`, `A4` - scattered dwords just
  below the bucket table at `0x00A70CA8`, i.e. the same QHash control block the earlier batch modelled.
- `0x00A7186C`, `70`, `74`, `78`, `7C`, `80`, `84`, `88`, `8C`, and the 0x1000-dword clear at `0x00A71890`.
  Note `0x00A7186C` is exactly the end of the bucket table (see the [0x2F1] extent fix), so the catalogue
  continues that structure immediately after it.
- `0x00A75890`, `94`, `98`, `A0`, `BD`, `BE`, `C0`, `C4` - the parameter table just below `0x00A758E0`.

**Consequence for transcription:** none of these regions has storage in the tree yet, and this is exactly
where the last three data defects came from. Do not transcribe statements against them until each range's
extent is derived from the instruction that consumes it and cross-checked against the next object's start.
The catalogue's own clear (`0x1000` dwords at `0x00A71890`) is an inline `rep stosd`, not a call.

## 0x4EE970 is blocked on 83 untranscribed handler natives (round 151 finding)

The catalogue is not a section-by-section transcription job. It embeds **83 distinct retail code pointers**
as `push <VA>` immediates (handler function addresses handed to 0x401530 / 0x401400 / 0x4CEA20 and friends),
and **0 of those 83 have a transcribed native** in `engine/src/retail`. In this candidate the image is
`DYNAMIC_BASE`, so a retail absolute code address cannot be pushed at all: each one must become a reference
to the candidate symbol of that native. The catalogue therefore cannot be finished before its handlers exist.

Distinct handler VAs (83), from docs/fidelity/evidence/0x004EE970-full-listing.txt:
0x427d70 0x427f80 0x487d90 0x487e20 0x488700 0x4d3710 0x4ef7f0 0x4ef830 0x4ef850 0x4ef860 0x4ef8b0
0x4ef8c0 0x4ef990 0x4ef9b0 0x4ef9d0 0x4ef9e0 0x4efa10 0x4efa20 0x4efa90 0x4efaa0 0x4efab0 0x4efb00
0x4efb10 0x4efb60 0x4efb80 0x4efba0 0x4efc10 0x4efc90 0x4efcb0 0x4efcd0 0x4efce0 0x4efd50 0x4efee0
0x4eff00 0x4eff20 0x4eff30 0x4eff60 0x4eff90 0x4effb0 0x4effd0 0x4f0020 0x4f0060 0x4f0080 0x4f00a0
0x4f00b0 0x4f00e0 0x4f0110 0x4f0140 0x4f0170 0x4f0190 0x4f01b0 0x4f01c0 0x4f0270 0x4f02b0 0x4f0320
0x4f0350 0x4f0380 0x4f03a0 0x4f03d0 0x4f0400 0x4f0430 0x4f0460 0x4f04a0 0x4f04d0 0x4f0500 0x4f0530
0x4f0560 0x4f0570 0x4f0580 0x4f0600 0x4f0610 0x4f0620 0x4f0640 0x4f0660 0x4f06e0 0x4f06f0 0x4f0700
0x4f0710 0x4f0720 0x4f0730 0x4f0740 0x4f0780 0x4f64e0

Most are small: median distance to the next listed handler is 0x30 bytes, and the bulk of the 0x4EF7F0..
0x4F0790 range is a dense family of QVM/task callbacks. A few are large (0x4F0780, 0x487E20, 0x4D3710) and
three are far outside the family (0x427D70, 0x427F80, 0x487D90, 0x488700).

Recommended order: transcribe the dense 0x4EF7F0-0x4F0790 family first (each small, each independently
bindable), then the outliers, then 0x4EE970 last with a symbol reference per pointer. Section 1 of
0x4EE970 is already committed (unwired, partial) and does not depend on any of the 83.

## Handler catalogue progress (round 151)

8 of the 83 handler natives are transcribed and bound: 0x4EF830, 0x4EF850, 0x4EF8B0, 0x4EF990, 0x4EF9B0,
0x4EF9D0, 0x4EFA10, 0x4EFA90 - all bn=1/0 at 1.000. Frozen pair b2adaddc (173 compiled sources), Tester
confirmed d6320c6b at artifact level and the six-handler batch is pending its check.

Standing pre-freeze check added after 0x4EF830 lost its load to -O3: **disassemble the emitted object for every
new native and confirm the retail-observable operations survived**. cfg_score cannot see an eliminated load.

Next three handlers are NOT mechanical and were deliberately deferred rather than rushed:
- 0x4EF9E0 (41 B): calls 0x4F1C20(1, esi), then 0x4B8A50([esi],[esi+4]) and stores the x87 result with
  `fstp dword [ecx]`, then increments [esi]. Float ABI and x87 stack behaviour must be modelled exactly.
- 0x4EF860 (66 B): pushes 0xB and calls 0x4F1C20, then loops 11 times calling 0x4B8A50([ebx]+esi, [ebx+4])
  and storing each float result to arg2's buffer, finally advancing [ebx] by 0xB. Same float concerns plus a
  literal loop bound of 11 that must be re-derived from `cmp esi,0xb` (already observed) and cross-checked
  against the 11 dwords the sibling 0x4EF990 copies.
- 0x4EFA20 (100 B, 3 blocks): string walk with `repne scasb` to a terminator, `fld dword`/`fstp qword` for a
  vararg double, then `call 0x4A53B3` with the format string at 0x00547A60. Needs 0x4A53B3 identified first.

## The frozen hash is a per-build identity, not a content identity (round 151)

Rebuilding the tree after comment-only edits produced a different sha256 (b2adaddc -> c91dc17a) even though no
code changed. Comparing the two binaries byte by byte: **4 bytes differ**, all inside the PE header, the
`TimeDateStamp` at 0x88 and the derived `checksum` at 0xD8. Sizes identical, all 14 MB of code and data
identical. So the candidate build is reproducible; only the link timestamp moves.

Consequence: a frozen pair's sha256 identifies one build, not one code state, and two pairs that differ only by
their timestamp are the same code. Use `scripts/candidate_normalised_hash.py <a> [<b>]` to
- print the plain and the normalised (TimeDateStamp/checksum zeroed) sha256, and
- fail loudly if two builds differ anywhere outside those two fields, so a real code difference can never be
  mistaken for a timestamp.

b2adaddc and c91dc17a share normalised sha256 acfde02d4aa49678a5dd9a044432602f8cc4e9c306407b4c0f386b2277415f34.

Freeze protocol from here: record both hashes for every pair, and treat the normalised hash as the code identity
when comparing pairs or reproducing an audit.

## Freeze-record corrections after the ca13 audit (round 151)

1. `0x4F1C20` passed a pointer's value where retail passes the pointer. My transcription typed
   `context[1]` as the available-count integer and passed `&available`, so the emitted code handed a stack
   local to `0x4B8B00`, which dereferences its first argument. Retail compares `base+i` against the dword AT
   `context[1]`. `context[1]` is a pointer (caller 0x004EF860 passes `[context+4]` into `0x4B8A50`, which uses
   it as a context pointer). Fixed and reloaded inside the loop, where retail reloads it. Emitted code now
   `mov 0x4(%edi),%eax ; mov %eax,(%esp) ; call`. Sixth CFG-invisible defect class: **wrong indirection**, and
   again cfg_score saw 6/8 either way.
2. My normalised hash for ca13e4a8 was wrong: `9a8ddc85...` is the normalised hash of the older
   `efc3cd15` build. The cause is my own shortcut: I computed the hash by `ls build/artifacts | tail -1`
   instead of by the tag I had just computed. Correct ca13e4a8 normalised hash:
   `ec4a9c63cd7faa002646d2d3e8262cb80f0139ad529566e2b6eaa369d4293b21`. Rule from now on: compute the
   normalised hash from the exact artifact path, never by listing the directory.
3. The Tester's c91dc17a runtime trio is invalid: ca13e4a8 was deployed inside that run window, so the timings
   cannot be attributed to one binary. c91 static checks stand; its runtime record does not.

Current frozen pair: **8939c4b7**, plain `8939c4b72f4a3707aee4a8d6d9ac661ad687ad7e328ebeb32698561f296bfc9e`,
normalised `8b95a4b8764038ad3c5f1bd5ecd58a8355d36b9ac1c21cc9bd3694d7bdb3dec8`, 175 compiled sources,
committed 04ffb82a.

## Float callbacks 0x4EF9E0 / 0x4EF860 fully derived, not yet written (round 151)

Both are now understood from the listing; next round writes them. `Qvm_CheckParameterCount_4F1C20` is correct
under them as of 8939c4b7, and the Tester confirms the emitted indirection.

`0x4EF9E0` (41 B, 1 block) - signature `(float* destination, std::int32_t* context)`:
  push esi; esi = [esp+0xc] = arg2 (context)
  Qvm_CheckParameterCount_4F1C20(context, 1)                      // push 1; push esi
  Qvm_GetFloatArg(context[1], context[0])                         // push [esi]; push [esi+4]; call 0x4b8a50
  *destination = that float                                       // fstp dword [ecx], ecx = [esp+0x18] = arg1
  context[0] += 1                                                 // mov eax,[esi]; inc eax; mov [esi],eax
  push/pop balance: the single `add esp,0x10` covers the four pushed words (1, esi, [esi], [esi+4]).

`0x4EF860` (66 B, 3 blocks) - signature `(float* destination, std::int32_t* context)`:
  push ebx; ebx = [esp+0xc] = arg2 (context); push ebp/esi/edi; edi = [esp+0x14] = arg1 (destination)
  Qvm_CheckParameterCount_4F1C20(context, 0xB)                    // push 0xb; push ebx
  base = context[0]                                               // mov ebp,[ebx]
  for (i = 0; i < 0xB; i += 1) {                                  // xor esi,esi / cmp esi,0xb / jl
      destination[i] = Qvm_GetFloatArg(context[1], base + i)      // push esi+ebp; push [ebx+4]; call 0x4b8a50; fstp dword [edi]
      edi += 4
  }
  context[0] += 0xB, then pop edi/esi/ebp/ebx and return.          // tail after 0x4EF899 must still be read to confirm order

Open item before writing: read the 0x4EF899 tail to confirm the exact store-back and pop order, and confirm
`Qvm_GetFloatArg`'s declared signature in engine/include/igi/retail/script/qvm_execute.hpp (it is transcribed
and structurally verified at 0x004B8A50, 3 blocks/2 edges, but the header must be checked rather than assumed).
The literal loop bound 0xB must be re-derived from `cmp esi,0xb` (done) and cross-checked against the 11 dwords
that 0x4EF990 copies - they agree.

Tester's low-severity residuals accepted and documented, not "fixed": base is read inside the count>0 path in
0x4F1C20 (both known callers pass 1 or 11), the omitted add esp,0xc on the non-returning path, and the header
edge count (now 8).

Protocol in force: announce each freeze, wait for the Tester's ack, then deploy.

## Analyst deliverable: 84 handlers (not 83) and proven data extents (round 151)

`docs/fidelity/evidence/0x4EE970-data-extents.md` (730 lines, committed) supersedes my earlier counts.

Handlers: **84, not 83.** The 83 list missed `0x0051BAA0` (11 B: reads [esp+8], writes 0xC through it,
registered once as `Real32x3.kindE9`). `0x0054DF10` is the `"RGB"` string, not a handler - that is why a naive
count gives 85. All 84 are clean `C3`-terminated cdecl entries: no `C2 imm16`, no thunks, no interior pointers.

ABI is proven from the dispatcher, not guessed: `0x4F1AF0(type, kind, value, arg0)` builds `19*type+kind` and
calls `push value; push arg0; call [edx*4+0xa758e8]`, so **every attribute handler is `fn(arg0, value)`** - two
cdecl arguments - and `0x4F1BC0` stores the handler at `[(19*type + (int8)kind)*4 + 0xA758E8]`. Slot-read
distribution: 55 read both `[esp+4]` and `[esp+8]`, 13 only `[esp+8]`, 9 only `[esp+4]`, 6 none, and `0x4F0780`
reads three. Suggested transcription order: 6 zero-arg, then 13 `[esp+8]`-only, then 9 `[esp+4]`-only, then the
55 two-arg converters; `0x4F0780`, `0x4EFD50`, `0x4EF8C0` last.

Data extents proven (R1-R8), with the clipping traps that matter: R1 `0x54DD0C..0x54DD16` is 5 class words PLUS
a flag byte at +0x0A (a `u16[5]` clips it - my current `g_54dd0c[5]` is therefore too small once the flag byte's
writer is transcribed); R2 `0x54DDA4..0x54DDB3` keeps a 2-byte hole; R3 `0xA70C68..0xA70CA7` is 0x40 bytes of 16
dwords with fields at +0x0C/+0x2C/+0x30; R4 the 251x3 bucket table; R5 `0xA7186C..0xA7188F` 9 handles; R6 the
0x4000-byte array; R7 keeps its hole; R8 has a hole plus write-only dwords.

Do NOT declare an object starting at `0xA70C68`: its bottom is unprovable (0xA70C48 is passed as an argument at
0x4EE88B and 0xA70C60/0xA70C64 are live globals of other functions). This is the region the analyst is least
confident about and the one most likely to produce the next clipping defect.

Also required before the initialiser can be faithful: `0x401530`, `0x401400`, `0x4F1B60..0x4F1BB0`, `0x4F1BC0`,
`0x4F1AF0`, `0x4B8890`, `0x4F19C0`, `0x4017C0`/`0x401810`, `0x401B20`.

## Correction: the four handler "return" changes were byte-identical (round 151)

My commit 7c9a011a claimed 0x4EF850 / 0x4EF8B0 / 0x4EF9D0 / 0x4EFA10 "now return the value argument" and that
"the emitted bodies differ from what you signed off in b2adaddc". The Tester compared the bytes: all four are
identical between b2adaddc and c3e61a74 (8b442408 c700<imm32> c3). The older `void` versions already left EAX
equal to the value argument, because the store went through `[eax]` and nothing overwrote it before `ret`.

So the change was documentation-level only, not a code fix, and my claim of a behavioural difference was wrong.
The explicit return stays (it is a guarantee rather than an accident of codegen), but it must be described
honestly: it does not change the artifact. Lesson: assert what the emitted bytes show, not what the source
change looks like it should do - the same error as the 0x4EF830 comment that vouched for a deleted load, in the
opposite direction.

## Next increment: the class channel (agreed with the Tester)

23 of 84 handlers now exist and none can execute. Further handler batches add coverage without reachability, so
the next work is the `0x401530` / `0x401400` invocation path and the class table, with class-method signatures
kept blocked until the invocation sites are evidenced.

## Class channel: first structural facts (round 152)

`0x401530` is `0x00DF` bytes, 7 blocks, and it is the class-property/method registrar the six class-method
handlers and the thirteen constants are waiting on. What its body establishes:

- **Class record table: base `0x00AF5EE8`, stride `0x30`, 384 records.** The loop runs `edi` from `0xAF5EE8` to
  `0xAFA6E8` (`add edi,0x30` / `cmp edi,0xa...e8` / `jl`), i.e. `(0xAFA6E8-0xAF5EE8)/0x30 = 384`. It reads
  `word [edi]` (the class word) and `byte [edi+0x1D]` (the name byte). This is the SAME table `0x401900` scans:
  `0x401900` walks from `0xAF5F05` (= record base + 0x1D) to `0xAFA705` (= last record base + 0x1D). So the two
  registrars share one 384 x 0x30 class table with the name at +0x1D, the class word at +0x00, and the
  parent/derived links the earlier auditors found at +0x04/+0x08/+0x0C/+0x28. Any storage claimed for this table
  must be one object of 384 x 0x30, not two.
- **Handler/state table: `0x00A96AE0`, indexed by `id * 4`**, receives `edx` (the handler or state value) at
  `0x0040156E: mov dword [eax*4 + 0xa96ae0], edx`.
- **Flag bytes at `0x0054FF34 + id`**, written as `cl` where `cl = (registration_counter == 1)`
  (`cmp ecx,1; sete cl`), and read back inside the subclass-propagation loop as
  `byte [esi + ebx + 0x54ff34]` to skip already-propagated records. This region is NOT yet modelled and is an
  eight-bit store - the exact class that produced the very first CFG-invisible defect.
- **Registration counter at `0x00567C70`** incremented on entry and decremented on exit; its value seeds the id
  range. **Derived-class counter at `0x00536148`** (`inc ebp` / `mov [0x536148],ebp`).
- **Propagation:** when a record's class word matches the argument and its flag byte is still zero, it calls
  itself recursively with `(ebx, ebp, arg3)`, so a property registered on a base class is applied to each
  derived class as well. The recursion is why the flag bytes exist.
- **`call 0x4014B0` after the loop** - a rebuild/invalidate step that must be transcribed before the registrar's
  behaviour is complete, since it runs on every registration.

Still to read before transcription: the exact argument order (arg1 class word via `and eax,0xffff`; `ebp` from
`[esp+0x14]`; `edx` from `[esp+0x18]`; another from `[esp+0x1c]` that is forwarded in the recursive call), and
`0x401400`'s body. Do not guess the signature: the earlier `0x401900` episode showed the argument count is
readable directly from the callee's own `[esp+N]` accesses.

## 0x401530 and 0x401400 argument frames derived from their own bodies (round 153)

Re-derived the stack frames from the callees' own `[esp+N]` reads rather than from call sites, the method that
settled the 0x401900 question.

`0x401530` (158 B, 7 blocks) - **three arguments**, not five:
- after `push ebx`/`push ebp` (8 bytes) `mov eax,[esp+0xc]` is **arg1**; it is `and eax,0xffff` and used as the
  id/index into the handler table at `0x00A96AE0` (`mov dword [eax*4+0xa96ae0], edx`).
- after `push esi` (12 bytes) `mov edx,[esp+0x18]` is **arg3**, stored into that handler table - so arg3 is the
  handler/state value.
- after `push edi` (16 bytes) `mov eax,[esp+0x1c]` is also **arg3**, forwarded to the recursive self-call.
- the recursive call at `0x004015A5` passes `(ebx, ebp, arg3)` = (class-record index, the word compared against
  the record's class word, handler value), so arg2 is the class word and the record word at `[edi]` is the
  candidate derived class.
- returns the decremented registration counter and stores it back at `0x00567C70`.
- `call 0x4014B0` runs after the propagation loop.

`0x401400` (164 B, 7 blocks) is the sibling registrar and takes **three arguments** too: `mov ebp,[esp+0x10]`
after `push ebx`/`push ebp` (8) is arg2, `and eax,0xff` masks it to eight bits, and `mov edx,[esp+0x18]` after
`push esi` (12) is arg3. It uses the same derived-class counter `0x00536148` and its own registration counter at
`0x00567C6C` (note: 0x401530 uses 0x00567C70 - two separate counters, do not merge them).

Both remain untranscribed. Open before writing: where arg2 is read in `0x401530` (it does not appear in the
filtered instruction set - likely consumed by the same propagation comparison the recursive call uses), and the
full body of `0x4014B0` which both call.

## 0x401530 frames settled; 0x4014B0 is the class-id assignment pass (round 154)

**arg2 is found.** The earlier filtered scan missed it because the same displacement `[esp+0x14]` is read twice
at different stack depths: at `0x00401589` (`mov dx, word [esp+0x14]`) the frame already has ebx, ebp and esi
pushed (12 bytes), so that read is **arg2**, and it is compared against the record's class word at `word [edi]`.
So `0x401530` is:

    void Registrar(std::uint16_t id, std::uint16_t class_word, std::uint32_t handler)

- arg1 = id (`and eax,0xffff`), indexes the handler table at `0x00A96AE0` (`mov [eax*4+0xa96ae0], edx`).
- arg2 = class word, compared against each record's word at `[edi]` to find the classes to apply to.
- arg3 = handler/state value, stored in the handler table and forwarded to the recursive call.
- the recursive call passes `(record_index, arg2, arg3)`, i.e. the same property applied to each derived class
  with that class's own index as the id. The flag byte at `0x54FF34 + id` prevents re-application.
- returns the decremented counter and stores it back to `0x00567C70`.

**`0x4014B0` (43 B, 5 blocks) is the class-id assignment pass** that runs after every registration:
for each record from name byte `0x00AF5F05` stepping 0x30 to `0x00AFA708`, if the name byte is non-zero it calls
`0x4014E0(record_index)` and stores the returned **word** at `record+0x20` (`mov word [esi],ax` with
`esi = 0xAF5F08 = base+0x20`). So class ids live at record offset +0x20 and are assigned by `0x4014E0`, not by
the registrars. Dependency order for transcription is therefore `0x4014E0` -> `0x4014B0` -> `0x401530`/`0x401400`.

Remaining unknown before writing the registrars: `0x4014E0`'s body (the id allocator) and the initialiser at
`0x4EF7??` that calls `0x4F1840`. Do not transcribe the registrars until `0x4014E0` exists, since their post-loop
step calls `0x4014B0` on every invocation.

## 0x4014E0 (class-id builder) read, and why it cannot be written yet (round 155)

`0x4014E0` is 75 bytes, 5 blocks:

    push esi; esi = arg1; eax = 0; push edi; ecx = 0
    loop slot = 0..0xF:
        dl = byte [slot + 0x54FF24]          // exts: the file holds sixteen 0xFF bytes
        if (dl == 0xFF) continue
        edx = (dl & 0xFF) * 0x180 + (arg1 & 0xFFFF)   // lea edx,[edx+edx*2]; shl edx,7; add edx,edi
        if (dword [edx*4 + 0xA96AE0] != 0) eax |= 1 << slot
    or ah,0x80                               // sets bit 15
    ret                                       // returns the word in AX

So class ids are a 16-bit bitmap: one bit per occupancy slot plus bit 15 always set, over the byte table at
`0x0054FF24` (16 bytes, 0xFF in the image) and the dword table at `0x00A96AE0` - the same table `0x401530`
writes via `mov [eax*4+0xa96ae0],edx`.

**Why this is not transcribed yet:** `0x00A96AE0` has no proven extent. `0x401530` writes it at an index derived
from its own record arithmetic and `0x4014E0` reads it at `(slot_byte * 0x180 + (arg & 0xFFFF)) * 4`, which with
the 0xFF sentinel would reach roughly 0x9A000 bytes - almost certainly sparse, with real slot bytes below 8, but
that is inference, not evidence. Declaring storage for it from the reader's arithmetic would repeat the clipped
array defect in the opposite direction (over-declaring and colliding with the neighbouring globals). The rule is
that only the retail writer defines a global, so the extent must come from the write sites, which are in
`0x401530` (untranscribed) and its callers.

Order therefore stands: `0x401530` first to establish the table's true indexing, then `0x4014E0`, then `0x4014B0`,
then `0x401400`. The class-id assignment cannot be first because its table has no owner yet.

## §10/§11: the handler table extent is PROVEN, and the current port's 0x401530 is not faithful (round 156)

Analyst appended §10/§11 to docs/fidelity/evidence/0x4EE970-data-extents.md (+368 lines). Committed with this
note. Six corrections that must be folded in before ANY code lands; four of them contradict what I recorded.

**Handler/state table `0x00A96AE0` - extent proven, no longer a blocker.**
`[0x00A96AE0, 0x00AF5EE0)` = 0x17D00 dwords (97,536) = 0x5F400 bytes = 254 phases x 384 classes.
Element index = **class_id + phase*0x180** (row stride 0x600 bytes). Proof trio: QTask_Init `0x40117D
mov ecx,0x17d00` / `0x401183 mov edi,0xa96ae0` / `0x40118F rep stosd`; independent proof from
TaskType_Register's column loops (`0x4019C1 mov edx,0xFE`, stride 0x600, column 0..383); third proof by
adjacency, since `0xA96AE0 + 0x5F400 = 0xAF5EE0`, the type-table base, whose clear loop is separately provable,
and the lower neighbour is the 256-byte map `[0xA969E0,0xA96AE0)`.

Writers: exactly three, all one subsystem - the two handler stores (`0x40156E`, `0x401444`) plus QTask_Init's
zero-fill and TaskType_Register's column copy/zero. No fifth writer in 588 refs. Registrar bounds cannot prove
the extent (0x401530 has no finite mask; 0x401400 would over-run capacity by 65,920 dwords), so the initialiser
is the only valid source - which is why the earlier "only the writer defines a global" rule pointed at the right
place but the wrong writer.

**Corrections that invalidate my rounds 152-155 notes:**
1. Type-table OBJECT base is **0xAF5EE0**, not 0xAF5EE8 (that is record+8). Object = `[0xAF5EE0,0xAFA6E0)`,
   384 x 0x30. The byte at `0xAF5F05` is the IN-USE byte at +0x25, not the name byte; +0x00 is the name pointer.
2. `mov dx,word [esp+0x14]` at 0x401589 reads **arg1**, not arg2 - 16 bytes are already pushed. Rounds 153/154
   swapped them. Real semantics: **arg1 = class id (propagation root/column), arg2 = phase (row/multiplier)**,
   and the recursion forwards phase unchanged while passing the matched record index as the new arg1.
3. The index formula I wrote is transposed: it is `class_id + phase*0x180`, not `class_word*0x180 + phase`.
4. **The existing `task_types.cpp` handler-link code is NOT a faithful 0x401530**: it lacks the whole propagation
   loop (0x401582-0x4015B7) and the tail `call 0x4014B0` (0x4015B9), returns the composed index instead of
   retail's pre-call counter (`[0x567C70]-1`), and applies `phase & 0xFF` which retail applies only in 0x401400
   (0x401417). It is also a shared helper, which the one-native-per-VA and no-helper rules forbid. Treat the
   current `link_handler` as unverified, not as a baseline.
5. The QTask_Init port omits the 0x17D00-dword zero-fill - the very block that proves the extent - plus the
   0xAFA6E0 / 0xA969E0 / 0xAFA7E0 / 0x54F92C / 0x567C38 / QTASK_TYPE_UNDEFINED initialisers, so initial state
   cannot match without them.
6. `QTask_HandlerTable` / `QTask_FirstLinkFlag` must be 0x17D00 elements each (254*384), not 384*384. The flag
   array `[0x54FF34,0x567C34)` is 0x17D00 bytes, and the port over-sizes it by 49,920 elements.

**Two silent-failure modes to preserve rather than sanitise:** the flag at 0x54FF34+id marks DIRECT registration,
not visited state (recursive entries always write 0), so a self-parent record or parent cycle recurses forever
and nothing in 0x401530 guards it; and passing the root sentinel 0x0181 as arg1 composes
`phase*0x180+385 = (phase+1)*0x180+1`, an in-bounds write into another class's slot while the compare matches
every root record. Both are real arithmetic in retail and must be transcribed as-is.

Next: agent-check corrections 1-6 against the current task_types.cpp before editing, then implement QTask_Init's
zero-fill and make 0x401530 a proper native at its own VA. Do not treat any of the above as verified until the
existing code is diffed against it.

## First verification of the six corrections against the tree (round 157)

Spot-checked the analyst's claims in the current source rather than trusting them:

- **Index formula: the PORT is right, my note was wrong.** `task_types.cpp:22`, `:476`, `:491`, `flow.cpp:180`,
  `game_flow.cpp:32` and `terrain_storage.cpp:139` all already describe `class_id + phase * 384`. Since
  0x180 = 384 those agree with the proven index `class_id + phase*0x180`. The transposition was in my handoff
  note only, not in engine code. Correction 3 is withdrawn as a code defect.
- **The 0x17D00 zero-fill is genuinely absent.** No `0x17D00` appears anywhere under `engine/`; the only
  occurrences of the table address are comments. So QTask_Init's `mov ecx,0x17d00 / mov edi,0xa96ae0 / rep stosd`
  block - the very instruction trio that proves the extent - is not transcribed, and no storage of 0x17D00
  elements exists. Corrections 5 and 6 stand and are the next concrete code work.
- Still to verify before editing: the actual array declarations behind the handler/flag tables (the comments
  say `class_id + phase*384`, so a 0x17D00-element declaration may already be right in size while the initialiser
  is missing), and whether `link_handler` matches 0x401530 (analyst says it does not: no propagation loop, no
  0x4014B0 tail, wrong return value, extra mask).

Rule for this verification pass: take each correction to the file and line that would show it, publish only what
the source shows, and withdraw the ones the source disproves - as with correction 3 here.

## QTask_Init phase-link sequence, read byte by byte (round 160)

The links retail performs between the current-type store (0x401138) and the handler-table clear (0x40117B), with
the exact instruction order. They are hardcoded per phase, not a loop, and the two link natives differ:

    0x40113E push 0           0x40114E push 1           0x40115E push 2           0x40116E push 3
    0x401140 call 0x4018F0    0x401150 call 0x4018F0    0x401160 call 0x4018F0    0x401170 call 0x4018F0
    0x401145 push eax         0x401155 push eax         0x401165 push eax         0x401175 push eax
    0x401146 call 0x401860    0x401156 call 0x401890    0x401166 call 0x401890    0x401176 call 0x401890
    0x40114B add esp,8        0x40115B add esp,8        0x40116B add esp,8

So: phase 0 links through 0x401860, phases 1..3 link through 0x401890, and each pair is
`0x401890(0x4018F0(phase))` with the handler returned in EAX pushed straight back as the argument. 0x4018F0 takes
the phase (0..3) as a dword and returns the phase handler; the link call takes that handler.

Consequences for the transcription, recorded before writing it:
- The four links must be four explicit call pairs at retail order, never a `for (phase = 0; phase < 4; ...)` loop.
  A loop is the obvious tidier form and would be a fabricated control flow.
- Phase 0 uses a DIFFERENT link native (0x401860) from phases 1..3 (0x401890); a single shared call would be a
  fidelity defect even though both take one argument.
- `add esp,8` after each pair shows both pushes are caller-cleaned in the same group, so the two calls compose as
  one expression, not two statements with separate cleanups.
- The clear at 0x40117B follows the four pairs and precedes 0x4B0B00 (0x401191) and 0x4B8810 (0x4011A5), which is
  exactly what the INTERMEDIATE PLACEMENT note in task_types.cpp already says must happen once these land.

Also confirmed from the same window: the registration at 0x401130 is `0x401900(str.QTask, 0x20, 0x181)` with the
three pushes at 0x401122-0x401129 and `add esp,0x14` at 0x401135, and the current-type store at 0x401138 is
`mov word [0x54F928], ax` - a WORD store, so the port's `QTask_CurrentType` must stay 16-bit.

## CORRECTION to the phase-link note above: 0x4018F0 takes no arguments (round 161)

The previous section says "0x4018F0 takes the phase (0..3) as a dword and returns the phase handler". That is
wrong, and the port's own header disproves it: `engine/include/igi/retail/core/task_types.hpp:165` records
0x004018F0 as `QTask_GetCurrentType()`, an already-transcribed native, and its retail body is
`mov ax,[0x54F928]; ret` - no stack argument at all.

So the four `push 0`/`push 1`/`push 2`/`push 3` instructions at 0x40113E, 0x401150, 0x401160, 0x401170 are dead
caller-side padding, exactly like the fifth pushed word at the 0x401900 registration that was settled earlier
(0x401900 reads only [esp+0x8], [esp+0xC], [esp+0x18], [esp+0x1C]). The word is pushed, never read, and the four
calls all return the same value: the QTask class id just stored at 0x401138.

That changes what the link sequence means: it is not "link each phase handler", it is four identical
`link(0x401860 or 0x401890, current_type)` pairs where only the link native differs between phase 0 and phases
1..3. Before wiring, the port must confirm what 0x401860 and 0x401890 actually do with that single argument - and
since the pushed phase is dead, a solution that reads the phase from the stack would be adding logic retail does
not have.

Rule restated for the third time in this stretch: a dead pushed argument is not an argument. Verify the callee's
own [esp+N] reads before assigning meaning to a push, and cite the callee, not the call site.

## CORRECTION 2: the phase pushes are NOT dead - they are the link call's second argument (round 162)

Round 161 called the four `push 0/1/2/3` instructions dead caller-side padding. They are not. Read the two link
natives' own stack accesses:

`0x401860` (41 bytes, 1 block):
    eax = [esp+8]; dx = word [esp+4]; eax &= 0xff
    eax = eax*3; eax <<= 1                  // eax = phase * 6
    byte [eax+0x54F932] = 1
    word [eax+0x54F930] = dx
    byte [eax+0x54F933] = 1
    ret

`0x401890` (36 bytes, 1 block): identical except it does NOT mask with 0xff and it writes
`byte [eax+0x54F933] = 0` instead of 1.

So the link call takes TWO arguments: arg1 = `word [esp+4]` (the current type returned by 0x4018F0 and pushed
back), arg2 = `dword [esp+8]` (the phase pushed before the 0x4018F0 call). The stack explains it:

    push phase            ; sits on the stack
    call 0x4018F0         ; takes NO argument, so it does not disturb the phase dword
    push eax              ; current type becomes arg1, phase is now [esp+8] = arg2
    call 0x401860/0x401890
    add esp,8             ; both pushes cleaned together

0x4018F0 being argument-less is exactly what makes the phase survive into the next call. My round-161 reading
("dead padding") was the mirror error of the same mistake: I verified the callee's reads for 0x4018F0 but stopped
there instead of following the stack into the call that consumes it.

Semantics established: the pair writes a six-byte record per phase at 0x54F930 + phase*6 -
- `word +0x00` = current type (the QTask class id),
- `byte +0x02` = 1 for both link natives,
- `byte +0x03` = 1 via 0x401860 and 0 via 0x401890.

So phases 1..3 differ from phase 0 only in that third byte. That is the real distinction the two natives encode,
and it is why phase 0 uses 0x401860 while 1..3 use 0x401890.

## §12 delivered: WinMain flow corrected, and 0x401A60 is blocked on its global's owner (round 160-164)

flow-analyst appended §12 (952 lines) to docs/fidelity/evidence/0x4EE970-data-extents.md. Corrections that
invalidate my own records:

- **C1: 0x4010B0 is NOT a direct WinMain callee.** Its call site 0x48F9D4 is inside WndProc 0x48F970 (WM_DESTROY),
  reached by dispatch through the WNDCLASS lpfnWndProc stored at 0x48F59E. My mvp-call-flow row for it is a WndProc
  site, not a WinMain call, so the "70 call sites of WinMain" table mixes both. Do not cite it as a direct edge.
- **C2:** 0x4010B0 has exactly one predecessor; the BN fall-through from 0x4010A0 is unreachable because 0x4A507C's
  only ret sits behind an unconditional jmp [edx+0x14].
- **C3:** 0x498480 takes 2 arguments (two write-only out-pointers), not r2's 3; the [esp+0x130] slot is counted twice
  by afij. Frame 0x11C is exactly full.
- **C4:** 0x402C40 is a separate 775-byte zero-arg entry reached only by the tail jmp from 0x4010B0 - 154 zero-arg
  calls and a jmp 0x508F90. Do not merge it with 0x4010B0.
- **C5:** 0x498480 is a pure platform probe (8 imports, COM/DirectX vtable calls, 2 GetProcAddress pointers); no
  engine globals and no engine-native callees, so it adds nothing to the native queue.
- **C6/C7:** 0x4A5AD5 and 0x4A5AE6 are real function entries (CRT exit wrappers over 0x4A5AF7), unlisted in the
  manifest, and the catalogue name Camera_ApplyBandFilter_6 for that address is contradicted.

Queue in call order from 0x4010B0: 0x401A60 (6 B) -> 0x4015F0 (391 B, 25 bb, recursive, 120 call sites) ->
0x402C40 (775 B) + 0x4C1780 (1-byte ret no-op, called repeatedly).

**0x401A60 is written but deliberately NOT wired.** engine/src/retail/script/native_401a60.cpp holds the exact body
(`mov eax,[0x567c58]; ret`), but the global 0x00567C58 has NO transcribed writer anywhere in the tree. Under the
only-the-writer-defines-a-global rule I cannot define it, and an extern-only declaration will not link. So the file
stays out of CMakeLists until the writer is transcribed - the same blocker shape as 0xA96AE0 before QTask_Init's
zero-fill was found. Do not "fix" it by defining the storage in the reader: that is the invented-global defect.

## §15 delivered: Graph_Init 0x4FAA60 scope, and my teardown order was wrong

analyst appended §15 (404 lines) to docs/fidelity/evidence/0x4EE970-data-extents.md. 0x4FAA60 is 837 bytes, 225
instructions, sole caller 0x518484 in the registration cascade from 0x5183D0, with 0x518489 (GraphNode 0x518760)
immediately after it consuming the ids it allocates.

Corrections to what I had recorded:
- **Teardown order (I had it wrong).** Retail 0x4FB110 is: (1) free [0xA76D10] through 0x4C1830 FIRST, (2) then ten
  0x4018C0 slot clears in the order E368,369,36A,36B,36C,36D,36E,370,36F,371, (3) then 0x401A20 unregister. My
  "clear slots -> release array -> unregister" had the first two steps swapped. It never zeroes [0xA76D10] and never
  rewrites slot bytes.
- **Ten slot writes are NOT in slot order.** 0x4FAA88->E368 ... 0x4FAABF->E36F, 0x4FAACA->E371, 0x4FAAD5->E370,
  0x4FAAE0->E36D, 0x4FAAEB->E36E. Preserve that order.
- **0x54E366 is dead**: image 0x0181, zero references of any width. No writer, no reader, keep the image value.
- **0xA76D10 confirmed**: 0x4F1950(5), 16-byte elements, 5 entries, header {count+0, capacity+4, ptr+0xC+8};
  element {+0 offset-like, +4 type handle, +8 id dword, +0xC id byte}. Only [0xA70C90] (Bool8) is identified; the
  other three handle slots and element +0 semantics are unresolved.
- **0x401A60 is NOT unblocked by Graph_Init.** 0x567C58 is touched by 0x4012FB/0x40130D/0x4015F2/0x4015FB (type-system
  routines) and 0x401A61; 0x4FAA60 touches none of them. Still waiting on flow-analyst's §14 for 0x4015F0.
- **Eight stand-ins to delete** once Graph_Init exists: g_54e364, g_a76d10, g_54e369, g_54e36b/c/d/e/f; slots
  0x54E368/36A/370 gain real storage for the first time; that is what makes 0x4F9270 a real reader instead of a
  reader of invented storage.

## §16 delivered: 0x401530 completed, 0x567C58 identified, and the 0x401A60 blocker narrowed

flow-analyst appended §16 (730 lines). Key results:

- **0x401530 is 3-arg cdecl, 158 B**: (class, phase, handler). Its 0xAF5EE8 walk is 384 iterations of stride 0x30
  bound 0xAFA6E8; per iteration it matches record+8 (parent) against arg1, tests the per-(class,phase) flag byte at
  0x54FF34, and **recurses into itself** at 0x4015A5 with (i, phase, handler). The 0x4015B9 call to 0x4014B0 takes
  ZERO arguments (nothing pushed, no add esp after). The flag byte is `cl = (ecx==1)` computed AFTER `inc ecx` at
  0x401542, so it marks only the outermost call as "own registration" - §11's prose read the compare as if on the
  pre-increment value; the code there is right and the label was wrong.
- **0x567C70 = balanced recursion depth counter restored on the single ret; 0x536148 = monotonic invocation counter,
  stored and never restored.** They are distinct; my "derived-class counter" label was too narrow (it counts
  recursive entries) and it is not private to 0x401530.
- **0x567C58 = QTask_LiveTaskCount** (4 bytes), exactly five .text refs: increment 0x401307 in the allocator 0x4012F0,
  decrement 0x4015F7 in the destructor 0x4015F0, getter 0x401A61. Zero-initialised. So the global now has exactly one
  incrementing and one decrementing writer - the meaning is proven. **0x401A60 still cannot be wired until 0x4012F0
  is transcribed**, because that is the writer that must define the storage. Record the order; do not define the
  global in the reader.
- **0x4015F0 = the QTask instance destructor** (391 B, 25 blocks, 1 arg, recursive): decrement the live count,
  rewrite the 0xAFA6E0 slot if the instance sits there (purpose unproven), invoke handler-table phase 3 then phase 2,
  release children tail-to-head, unlink via 0x4015D0, free via pool or Mem_Free. 120 call sites: 68 object teardown,
  42 script/QVM - the game object channel, not the startup channel.
- **0x4012F0 = allocator** (189 B, 2 args: owner, type id) and its 0x4012A0 wrapper has indegree 110; it is on the
  critical path of every task creation. Transcribing 0x4012F0 + 0x4014B0/0x4014E0 is what unlocks the task globals.
- Numbering: §16 holds both the 0x401530 completion and the 0x4015F0 material requested as "§14"; there is no §14.

## §17 delivered: the allocator exists under the WRONG NAME with dropped behaviour (2026-10-24, round 360)

flow-analyst appended §17 (463 lines). Identity: 0x4012F0 = QTask_AllocateTaskInstance(void* owner, uint16 typeId),
189 B, 6 blocks, 2 callers; 0x4012A0 = append-as-last wrapper (72 B, indegree 110); 0x4013B0 = insert-as-first
(72 B, 1 caller); 0x4014B0 = TaskType_RefreshHandlerMask, ZERO args, 4 calls + 2 tail jumps; 0x4014E0 =
TaskType_HandlerPhaseMask(uint32 classIndex), 1 arg, exactly one caller (0x4014B0).

**0x567C58 increment ordering (my question answered):** 0x4012F9 load -> 0x401307 `inc esi` -> 0x40130B store. That
is BEFORE the pool test, the pool claim, Mem_Lock/Mem_Alloc/Mem_Unlock and all nine header writes, and there is no
failure path or restore. So the increment is unconditional and a reader can observe count N while instance N still has
an uninitialised header. Retail header write order is NOT ascending and must be preserved: +0x04, +0x00, +0x14
(=owner), +0x1E (byte pooled flag), +0x1C (word type id), +0x18, +0x08, +0x0C, +0x10.

**Wrapper handler ABI:** both wrappers push arg3 then the object and call with 2 cdecl arguments (handler arg1 = the
new task, arg2 = the wrapper's third argument), cleaning 8 bytes. The handler index is read from the OBJECT's own
header word *(uint16*)(obj+0x1C), not from arg2.

**BLOCKING FINDINGS — the tree already has these VAs, wrongly:**
1. `native-verification.json` names 0x4012F0/0x4012A0 `QTask_AllocateInputPort`/`QTask_RegisterInputPortHandler` in
   `input/input_port_process.cpp` and calls them STRUCTURALLY_VERIFIED; `scripts/GameNatives/VERIFICATION.md` calls the
   same VA `QTask_LinkEventHandler` (PASS/VERIFIED). Three accounts of one VA.
2. That file's body diverges from retail: pool-recycle branch dropped (unconditional Mem_Alloc), Mem_Lock/Mem_Unlock
   dropped, pool-tail write and poolUsed++ dropped, empty-list init replaced with offsetof constants, pooled flag
   taken from the type's pool-present byte (so a heap instance can be marked pooled), an INVENTED null check, and
   0x567C58 declared and initialised to 0xFFFFFFFF under the name QTask_InputPortCounter - retail's initial value is
   0 and 0xFFFFFFFF is 0x536148's image value.
3. Its wrapper swaps the allocator arguments: passes (arg2, arg3) where retail passes (arg1, arg2), and guards the
   insert on arg3 where retail tests arg1 (0x4012B6).
4. `FrameHandler = void (*)(void*)` in task_types.hpp:50 is a ONE-argument handler type, but retail phase-0 dispatch
   pushes two arguments and cleans 8 bytes (0x4012DD-0x4012E0, also 0x401D80). That ABI gap touches task_types.hpp,
   the registrar transcriptions and dispatch together and must be fixed as one change.

**So my premise was wrong**: the 0x567C58 writer is not missing, it is present under the wrong name and initialiser
with a dropped pool branch. What is genuinely absent from the manifest is the reader 0x401A60, plus 0x4013B0,
0x4014B0 and 0x4014E0.

**Next increment (ordered):** (1) rewrite 0x4012F0/0x4012A0/0x4013B0 from §17 bytes under retail names and semantics,
deleting input_port_process.cpp's wrong version in the same change (one implementation per VA); (2) add
0x4014B0/0x4014E0; (3) fix the FrameHandler ABI and every registrar call site as one change; (4) only then wire
0x401A60, whose storage comes from the rewritten allocator.

## Gate is vacuous when run after commit; targeted gate exposes the real evidence gaps (round 166)

`scripts/native_gate.py --changed` examines STAGED, UNSTAGED and UNTRACKED native files only. Because this session
commits each change before gating, every `--changed` run reported "0 changed native(s) passed" - it checked nothing.
That is a process defect, not a pass: §10.7 says the gate selects edited native sources, and after a commit there are
no edited sources. Run it PRE-COMMIT, or target the change explicitly with
`--file <source> --address <VA>` (repeatable `--address`) / `--tree`.

First non-vacuous run, on the allocator changed today:
`python3 scripts/native_gate.py --root . --file engine/src/retail/core/task_types.cpp --address 0x004017C0
 --threshold VERIFIED --cfg-threshold 0.85 --require-semantic-evidence --require-behavioral-tests`
=> **BLOCKED, 6 issues**: status STRUCTURALLY_VERIFIED is below the required VERIFIED; the implementation changed
since its evidence record was reviewed; and the required structural/semantic artifacts do not exist:
docs/fidelity/evidence/0x004017C0-binaryninja.json, -radare2.json, -semantic-review.json.

So 0x004017C0 is behaviour-verified by hand but not gate-verified. Closing that gap is the Tester's work: produce the
BN graph JSON, the radare2 graph JSON, a semantic-review JSON authorising the runtime contract, then promote only if
the gate passes. Also note the gate treats a multi-line Behavior/CFG section as a malformed five-line block; the
allocator's provenance is now a genuine five-line block.

## 0x004017C0 evidence closed to the honest limit (round 167)

Gate progression on `--file engine/src/retail/core/task_types.cpp --address 0x004017C0 --threshold VERIFIED`:
6 issues -> 2 -> 11 (after the semantic-review reference made the gate start validating its contents) -> 2.

Fixed along the way: generated docs/fidelity/evidence/0x004017C0-binaryninja.json and -radare2.json with
build_native_records.py (bn=6/7 r2=6/7, CFG 1.000), and wrote 0x004017C0-semantic-review.json with result passed and
all eight checks the gate demands: function_identity, cfg_match, argument_mapping, calls_and_side_effects,
constants_and_scaling, floating_point_semantics (not_applicable), memory_layout_and_access, return_and_error_paths.

The remaining two issues are honest and must NOT be papered over:
1. `status STRUCTURALLY_VERIFIED is below required VERIFIED`;
2. `semantic review has no authorized runtime_contract`.
Both follow from one fact: the startup boundary throws at QTask_RegisterClasses (0x00402930) before this native is ever
reached, so no Frida observation can exist. Authoring a runtime_contract with a probe SHA and "typed required
observables" for an unobservable native would fabricate execution evidence - exactly the class of false record this
session has already withdrawn four times (QTask_RegisterClasses, FramesSet, 0x401530, 0x4012F0).

Consequence for the process: pre-boundary natives cannot pass a VERIFIED threshold. Either the gate is run with
`--threshold STRUCTURALLY_VERIFIED` for them, or the boundary must be lifted by transcribing the registrar.

Do not manufacture a runtime contract for an unobservable native; record the blocker instead.

## 0x4012F0 decoded instruction-by-instruction; ONE instruction unresolved before it can be written (round 168)

Registers: entry pushes ebx/ebp/esi/edi. `ebp = [esp+0x10]` = arg2 (typeId, word used). `[esp+0x14]` later =
arg1 (owner). `edi = (typeId & 0xFFFF) * 3 << 4` = typeId * 0x30 = byte offset into the type table at 0xAF5EE0.

Type-record field map proven from the absolute offsets (base = 0xAF5EE0 + typeId*0x30):
  +0x00/0x02 word   (parent class word, readers elsewhere)
  +0x04  dword  object size            (pushed to Mem_Alloc as (size, 4))
  +0x0C  dword  pool base pointer      ('add esi, ecx' at 0x40134C)
  +0x10  dword  pool slot-index array  ('mov esi,[ecx+eax*4]' at 0x40133C)
  +0x14  dword  poolUsed               (compare at 0x40132A, inc at 0x401357, store 0x401358)
  +0x18  dword  pool capacity          (compare at 0x40132A)
  +0x1C  dword  element size           ('imul esi,[edi+0xaf5efc]')
  +0x20  dword  second pointer base    (loaded at 0x401334 into edx)
  +0x24  byte   pool-present flag      ('mov al,[edi+0xaf5f04]', test at 0x40131A)

Control flow: live count load/inc/store is at 0x4012F9/0x401307/0x40130B, i.e. before every branch. Pool path taken
only when +0x24 != 0 AND poolUsed < capacity; it sets bl = 1 (pooled) and jumps to the header writes at 0x401381.
Otherwise 0x401360 clears bl = 0 and falls into the heap path: Mem_Lock(1) (0x4B0D60), Mem_Alloc(+0x04 size, 4)
(0x4B0C60), add esp,0xc, Mem_Unlock (0x4B0D90). Header writes in retail order: [esi+4]=0, [esi+0]=0, [esi+0x14]=owner,
eax=esi+0xC, ecx=esi+8, [esi+0x1E]=bl (byte), [esi+0x1C]=bp (word typeId), [esi+0x18]=0, [ecx]=eax, [eax]=0,
[esi+0x10]=ecx; return esi.

**UNRESOLVED — do not write the function until this is settled:** `0x0040134E  mov dword [edx + esi], eax`. esi has just
become base+slot*elemSize (an address), so [edx+esi] as a store target is not self-consistent with edx = [edi+0xAF5F00]
being a base pointer. Likely readings: (a) the second pointer at +0x20 is not a base but a negative/typed offset, (b)
r2's decode of the ModRM is wrong and the store is [esi+something] with edx as the value, or (c) +0x20 holds a pointer to
the per-slot table and esi is an index at that point (i.e. the imul/add at 0x401345/0x40134C produce a slot offset, not
an address). Settle by reading the raw bytes of 0x00401345-0x00401352 with a second decoder and by finding the reader
of the value stored there. Writing it either way now would be guessing, which is what produced the four false records
already withdrawn.

Round 168 follow-up on the ambiguous store: the two fields involved are read elsewhere, which narrows the reading.
  rec+0x20 (0xaf5f00) is loaded as a POINTER at 0x4016E4, 0x401729 and 0x40175B (destructor/delete paths); the
  allocator loads it into edx and stores through it at 0x40134E.
  rec+0x0C (0xaf5eec) is loaded at 0x401716 and its ADDRESS is taken with lea at 0x40198E and 0x401A3A (type
  registration/unregistration paths), which fits an embedded list head rather than a pool base.
So the most likely reading is: +0x0C begins an embedded list head, the pooled object is found by slot*elemSize from
some other source, and +0x20 is the pool base used for the tail store. That is still not proven - the type
registration writer 0x401900/0x4014B0 chain must be derived to establish which field is the base. Do not write
0x4012F0 from this reading alone.

## FrameHandler ABI blast radius measured (round 169)

Retail evidence (section 17): the wrappers call the phase-0 handler with TWO cdecl arguments and clean 8 bytes -
`push arg3; push obj; call eax; add esp,8` at 0x4012D8-0x4012E0, and 0x401D80 does the same. The tree's type is
`using FrameHandler = void (*)(void*)` (task_types.hpp:50), one argument.

Measured scope of the correction:
- 32 references to FrameHandler across engine/src and engine/include (declarations, the handler table, flow registry)
- 2 direct `(void*)`-only handler signatures
- 9 natives declared as `void NATIVE_DECL ...(void* ...)` with a single pointer argument

That is the full set that must change in ONE commit, because a partial change leaves handlers with mismatched ABIs:
some would receive the second argument in a register the callee does not expect, and the phase-0 dispatch would push
two words into a one-argument prototype (the x86 cdecl caller cleans, so it links but reads garbage). Before writing it,
confirm for each of the nine natives that its retail body actually reads a second argument at [esp+8]; handlers that
read only [esp+4] are still ABI-compatible with a two-argument type (they simply ignore the second), so the type change
is safe for them and only the call sites' cleanliness has to be right.

## The FrameHandler ABI change is NOT mechanical: the second argument is unknown at three dispatch sites (round 172)

Widening the type to `void (*)(void*, std::uint32_t)` produced 13 compile errors across three files, and that
enumeration is itself the finding. The errors split into two groups:

1. Six registered handlers must gain the second parameter: game_flow.cpp:72-74 (Level_Start, Game_Setup,
   Game_DeleteHandler) and flow.cpp:267-269 (Flow_Start, Flow_RunHandler_Handler, Flow_Delete_Handler). Mechanical -
   the retail bodies of these handlers do not read the second argument, so they ignore it.
2. Three dispatch sites CALL a handler with one argument and the compiler now demands the second:
   flow.cpp:147 (0x00402365), flow.cpp:193 and flow.cpp:238. **What retail pushes there is not established.**

Point 2 is the blocker. The allocator wrapper's second argument is its own third argument (proven in section 17), but
the Flow walk at 0x00402365 is a different dispatcher and no evidence sheet states what it pushes as the second word.
Filling in 0 would be an invented constant inside a native's flow - rule 1/10.13 - and the tree would then look
complete while the dispatch ABI is wrong.

So the change was reverted and the tree is green again. Before the ABI lands, each of the three Flow dispatch sites
must be transcribed from its own retail instructions: 0x00402365 in the Flow node walk, plus the two in
Flow_RunHandler. That is the next analysis task, not a compilation exercise.

## The handler ABI question is settled against the tree's one-argument type (round 173)

Retail handlers have no single signature - every dispatch site pushes what it wants and cleans what it pushed:
  0x00402364 push edx ; 0x00402365 call ecx ; 0x0040236C add esp,4   -> ONE argument (Flow phase walk)
  0x004023CE and 0x00402481 likewise push the node and clean 4 bytes
  0x004012DC-0x004012E0 in the allocator wrapper pushes TWO words (arg3 then the object) and cleans 8

So widening `FrameHandler` to two arguments would have been WRONG for the three Flow walk sites: they call handlers
with one argument and clean 4, and a two-argument prototype at those sites would describe an ABI the retail dispatcher
does not use. My round-172 plan (widen the type, then fill the missing second argument at three sites) was therefore
based on a false premise: those sites do not have a missing argument, they have one argument.

Correct representation, now implemented and documented at both places: keep the one-argument `FrameHandler` and cast at
the one site that genuinely pushes two words (the allocator wrapper 0x004012A0), with the retail pushes cited. The
"ABI gap" recorded in section 17.6 is real for that wrapper only, not for the handler table as a whole.

## Why 0x004014E0 still has no CFG artifacts: it is absent from the v4 catalog (round 177)

`python3 scripts/build_native_records.py 0x004014E0 --force` prints only "built 0 record(s)" and no diagnostic. The
loop at scripts/build_native_records.py:215 explains it: the script iterates `discover()` (the v4 catalog) and skips
every address it did not yield, so an address the catalog does not list is never considered - not even to print a
warning. 0x4014E0 is therefore not a generator failure but a catalog gap.

Consequence: 0x4014B0 got artifacts and STRUCTURALLY_VERIFIED because the catalog lists it, while 0x4014E0 cannot get
any through this script and must stay TRANSCRIBED until the catalog entry exists. Adding the entry needs the signature
and source mapping the generator expects; the retail body is fully established (16-iteration loop over 0x54FF24,
handler-table presence test, bit per non-0xFF phase, `or ah,0x80`), so only the catalog metadata is missing.

Do not read "built 0 record(s)" as "nothing to do": for an address outside the catalog it always means the script
never looked.

## Correction to the round-177 "catalog gap" explanation (round 178)

Round 177 concluded that 0x004014E0 cannot get CFG artifacts because it is missing from the v4 catalog that
build_native_records.py iterates. That explanation is NOT established, and the check below contradicts part of it:

    ng.implementation_natives(Path('engine/src/retail/core/native_4014b0.cpp'), ...) -> []
    ng.implementation_natives(Path('engine/src/retail/core/native_4014e0.cpp'), ...) -> []

Both files parse to no natives at all, yet 0x00401 4B0 DID receive artifacts from the same script. So the provenance
parser is not what produced that record, and "the catalog lacks the address" was an inference from reading the loop at
line 215, not from observing the candidate list. The file provenance blocks for both files have the same shape ("VA
0x0040XXXX-0x0040YYYY (N bytes)"), so format divergence is not the difference either.

What is actually known: the script prints nothing at all for 0x004014E0 and "built 0 record(s)"; for 0x004014B0 it
printed a BN/r2 block comparison and wrote artifacts. The discriminator is somewhere in discover()/canonical_source()
or in how the address is matched, and it has not been found. Do not repeat the catalog claim as fact; the next
investigation step is to print `discover()` and check whether 0x004014E0 appears in the candidate list at all.

## The destructor listing is NOT complete and the tabulation was withdrawn (round 184)

Tabulating 0x4015F0-retail-listing.txt produced a 32-row table ending at 0x00401777. Checking the arithmetic instead
of shipping it: the function extent is 0x004015F0-0x00401777 = 391 bytes, so 32 instructions would average 12 bytes per
instruction, and the same file holds 97 addressed lines of which 65 sit between 0x00401777 and 0x0040180A - a range that
belongs to the next function (0x004017C0 is the event allocator). So the listing is not a complete linear decode of
0x4015F0 and the table derived from it was wrong; it has been deleted rather than committed as evidence.

Actionable correction for the next pass: do not use `pd N @ <addr>` for this function. Use a real function-scoped
disassembly (`af` then `pdf`, or `pdr` with the function boundary from the 391-byte extent) and verify the row count
against the byte length before transcribing. The dependency note (0x4015F0-dependencies.md) is unaffected: it read call
targets, and the four it lists all appear in the listing.

## The deployed igi1.exe is STALE relative to HEAD (round 186)

Checked integrity directly:
  deployed /Users/haseeb-mir/Downloads/IGI/igi1.exe SHA-256 prefix 96fc1442d712bc86
  HEAD 7b9557b5 (evidence and docs only since the freeze)

The deployed binary is the artifact frozen before the following engine changes, all of which are in HEAD but NOT in the
deployed executable:
  993116bf  pooled-flag derivation fixed in the allocator
  1f3d2917  allocator 0x004012F0 and wrapper 0x004012A0 rewritten from the retail instructions
  e04ebd71  unlink native 0x004015D0 transcribed
  cf618047  live-count reader 0x00401A60 wired

Consequence: any runtime statement about "the candidate" made from the current deployment describes code that no longer
matches HEAD, so the boundary-at-exit-3 result must be re-established on a fresh artifact before it is cited again. The
freeze/deploy step needs a coordinated pass: build, candidate_build_receipt.py, freeze under build/artifacts/, deploy the
exact frozen copy, and only then hand to the Tester for the paired runtime observation.

Also outstanding from earlier rounds and unchanged: the tracked scripts/__pycache__/*.pyc churn, and the frozen
artifacts igi1-fbd69730.exe / igi1-ff02eb9c.exe which are neither HEAD nor deployed.

## Fresh artifact frozen and deployed for HEAD (round 187)

  frozen   build/artifacts/igi1-b071553f.exe
  receipt  build/artifacts/igi1-b071553f-receipt.json
  plain    b071553f76c932b639b5099c1cfd9f396edead5614e7a48e4e275242d8eb3ebd
  normalised 2dd3a951461478bc9d92156434d676c9752578e0f81a4a31b97f79f46a37a880
  deployed /Users/haseeb-mir/Downloads/IGI/igi1.exe  (plain b071553f76c932b6)

This supersedes the stale 96fc1442 deployment and is built from HEAD 23c18e69, so it contains the allocator rewrite,
the unlink native and the wired live-count reader.

IMPORTANT: candidate_build_receipt.py wrote build/diagnostic-candidate-build.json, i.e. this is a DIAGNOSTIC build, not a
strict-gate release attestation. It is deployed for observation only and must not be presented as a verified release.
The boundary-at-exit-3 result therefore has to be re-established on this artifact by the Tester before any runtime claim
about the current source is repeated.

## Runtime lease blocked by two orphaned game sessions (round 188)

The Tester could not take the lease for b071553f because game processes were already running. Verified independently:

  41746  PPID 1  14m05s  Ss  build-format-arg-candidate-sol/igi1.exe window level1   - nobody's: no listed teammate
                                                                                      owns it, no children, from a
                                                                                      build directory no one is using
  93434  PPID 1  17h53m  S   bash -c cd .../project-open-igi && RUN="$PWD/$(cat /tmp/tester-run...)"
  93436  93434   17h53m  S   CrossOver x86_64 wine loader (child of 93434)

The 93434/93436 tree reads /tmp/tester-run*, so it is the Tester's OWN earlier run, orphaned 17+ hours ago and still
holding the mutex. Both trees have PPID 1, i.e. they were reparented to launchd - nothing ties them to a live session.

Disposition given to the Tester: record the PIDs and command lines first, then clear 41746 (unowned) and the 93434 tree
(its own orphan, parent then child), leave winedevice/CrossWine/CrossOver GUI processes alone as shared infrastructure,
and take the lease only when `pgrep -f 'igi1?\.exe'` is empty. If anything still holds the mutex and cannot be
attributed, report and stop rather than force a second instance.

Process lesson for the one-process rule: a crashed or abandoned run reparents to launchd and becomes invisible to
`list_agents` while still holding the mutex. The lease protocol needs a liveness check that includes PPID-1 game
processes, not just teammate status - otherwise every future runtime pass can be silently blocked by a session nobody
can see.

## b071553f boundary observation: PASS, diagnostic (round 188, Tester report)

Deployed b071553f (HEAD f9b418aa; no engine diff since) observed four times with the game directory as cwd:

  cold 1  4 s  exit 3    cold 2 (60 s cooldown)  2 s  exit 3
  warm    1 s  exit 3    +seh                    1 s  exit 3

stderr is the same fail-closed throw every time:
  terminate called after throwing an instance of 'std::logic_error'
    what():  QTask_RegisterClasses (0x00402930): the AI graph registrar is outside the MVP scope and is not
             transcribed, so the class ids after it do not match the retail's
+seh: 71 x 0x80000026, no 0x6ba. stdout 0 bytes in all four runs.

Position 1 / id 0 confirmed: ReportNotTranscribed("AI graph") is the first statement of QTask_RegisterClasses, no class
is registered, and nothing after the boundary runs - no Flow_CreateHandler, no registrar, model, terrain or player line.

Caveat carried into the record: the receipt is diagnostic-candidate-build, so this is a diagnostic observation and NOT a
strict-gate release attestation. It establishes that the boundary result survived the allocator rewrite, the unlink
native and the reader wiring; it does not verify any of them behaviourally, because none of them is reachable before the
throw.

Lease hygiene resolved the same round: 41746 was a stale sol candidate (5f749d26, not ours), and 93434/93436/93439 were
the Tester's own orphaned frida-server tree (d4b0b8af). Cleared with evidence recorded first; winedevice/CrossOver GUI
and the shared wineserver left alone.

## Session close-out state (round 188 end)

Tree: clean except one uncommitted tester script (scripts/tester/frida_probe_driver.py, the Tester's own work - not
mine to revert). The tracked scripts/__pycache__/*.pyc churn that has been carried for many rounds is cleared.

Agreed state of the world for the next session:
  HEAD 906762cb; build green; deployed and frozen candidate b071553f (diagnostic receipt, HEAD f9b418aa source).
  Runtime observation on that artifact: exit 3 at QTask_RegisterClasses 0x00402930, position 1 / id 0, four runs
  (cold, cold after cooldown, warm, +seh), stdout empty, 71 x 0x80000026 in the SEH run.

Work landed this session (engine):
  0x004012F0 allocator and 0x004012A0 wrapper rewritten from the retail instructions, including the pool branch, the
  memory flag-stack pair, the unconditional pre-branch live-count increment and the retail header store order pinned
  with volatile after GCC reordered it.
  0x004015D0 unlink native transcribed (calls HeapList_Remove 0x004AF960).
  0x00401A60 live-count reader wired: it now declares storage that its retail writer defines.

Work landed this session (evidence/tooling):
  Six natives newly recorded after the provenance-format sweep (the generator only accepts
  '// Native: <name>; retail VA 0x........'); 0x4014B0 and 0x4014E0 now STRUCTURALLY_VERIFIED from their own files;
  0x489560 and 0x4C5810 held at TRANSCRIBED because their generated CFG artifacts are degenerate.
  Withdrawn after checking: the 'catalog gap' explanation for 0x4014E0, the 32-row destructor table, and the
  two-argument FrameHandler plan (real call sites push one word and clean 4; the allocator wrapper is the only
  two-word site and is cast there).

Next increment, in order:
  1. Transcribe 0x004015F0 (88 instructions, 25 blocks, 391 bytes, table committed at
     docs/fidelity/evidence/0x4015F0-instruction-table.md; every direct dependency already in the tree).
  2. Then 0x00402930's remaining dependency chain, which is what actually moves the startup wall.
