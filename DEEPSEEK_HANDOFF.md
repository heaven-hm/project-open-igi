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
