## Unreleased

### `0x3A91A2B4` is the float `0.0011111111380159855F`, and it appears twice

chain-worker's GCC finding was that a decimal float literal folded into a long
double emits a mantissa with a non-zero low word. I verified the constant it
concerns is real and present, and it is in the image **at two addresses**:

```
0x00533584   b4a2913a   0x3A91A2B4   -> float 0.0011111111380159855
0x005338BC   b4a2913a   0x3A91A2B4   -> float 0.0011111111380159855
0x005338C4   4f6732c2   0xC232674F   -> float -44.6008...
0x00533678   8988083d   0x3D088889   -> float 0.0333333351...
```

So the constant is not a one-off: **two independent sites hold the identical
32-bit pattern**, and chain-worker's hex-literal fix (`0x1.234568p-10L`) emits
exactly `b4 a2 91 3a`. The fix reproduces the image bytes, which is the check that
matters - a fix that produced a *different* bit pattern would be a behaviour
change dressed as a correction.

`0x00533678` = `0.0333333351`, which is 1/30 to float precision - consistent with
the 30-iteration loop in `0x00449D30` and with `0x00449E50`'s use of the same
table.

### Independently checked: chain-worker's `0x00449E50` numbers

Its block-size list (21, 1, 13, 4, 4, 3, 196, 6, 2, 4, 3, 4) sums to **261** across
**12** blocks, matching its claimed instruction and block counts. That internal
consistency is the check that catches a dropped block, and it holds.

## Unreleased

### The link checker's math-helper list was one name where it needed a class

chain-worker found that `check_native_linkable.py` reported `sincosl`/`sincosf`/
`llrintl` as "undefined in tree" for files that link cleanly - GCC emits these
itself when it fuses or widens floating-point work. **Two committed files were
affected**: `core/native_4192e0.cpp` (`sincosf`) and `core/native_419630.cpp`
(`llrintl`, `sincosl`).

My earlier fix added `sincosl` alone, which was a name where the problem was a
class. Widened to the family - `sincos`/`sincosf`/`sincosl`, the `llrint`/`lrint`/
`llround`/`round` long-double set, and `sinl`/`cosl`/`powl`/`fmodl`/`atan2l`/
`floorl`/`ceill`/`truncl`. Both flagged files now report linkable, and a sweep of
the tree finds **no file still citing a math helper as undefined**.

This is the `___mingw_sprintf` artifact for the third time: a runtime routine
counted as a missing native, which reads as NOT LINKABLE for code that links. **A
no-link result is a statement about the checker's symbol list as much as about the
tree**, and I have now twice fixed one name instead of the class.

### chain-worker: GCC folds decimal float literals with more precision than the float carries

Found while transcribing `0x00449D30`: `static_cast<long double>(0.0011111111380159855F)`
emits the 80-bit constant `69 00 00 00 00 b4 a2 91 f5 3f` (mantissa
`0x91A2B40000000069`) instead of the exact widening `00 00 00 00 00 b4 a2 91 f5 3f`
(`0x91A2B40000000000`). The fix is a hex literal - `0x1.234568p-10L` emits exactly
`b4 a2 91 3a`.

**Any file feeding an x87 float constant into a long double chain can carry the same
off-by-last-bit error**, and it is invisible in the source because the decimal
literal looks like the right value. This is a bit-exactness defect, not a style
issue, and it belongs in the sweep for the `0x5335xx` constants and the `fcos/fsin`
precedents.

## Unreleased

### 18 natives whose CFG claim disagrees with the measured CFG

After correcting `0x00504A30`'s "1 block, 0 edges" against a measured 6 nodes /
7 edges, I swept for the class. **18 files carry a provenance CFG claim that
disagrees with the CFG measured from the image:**

```
0x0042db00 claims  5b/ 8e  measured  7n/10e     0x004abc46 claims  7b/10e  measured 11n/14e
0x00401b20 claims  4b/ 4e  measured  5n/ 6e     0x004ab733 claims 20b/22e  measured 20n/33e
0x004c0170 claims 13b/17e  measured 16n/24e     0x004aad90 claims  1b/ 2e  measured  6n/ 7e
0x004ef990 claims  3b/ 0e  measured  1n/ 0e     0x004b0d10 claims  1b/ 2e  measured  1n/ 0e
0x004bac30 claims  3b/ 2e  measured  3n/ 4e     0x004ab84c claims  7b/ 8e  measured  4n/ 5e
0x004bcae0 claims 15b/18e  measured 15n/22e     0x004ab7f5 claims  7b/ 8e  measured  9n/12e
0x004f0c90 claims  1b/ 3e  measured  1n/ 0e     0x004aad48 claims  1b/ 0e  measured  3n/ 2e
0x004f1180 claims  3b/ 3e  measured  3n/ 2e     0x00504a30 claims  1b/ 0e  measured  6n/ 7e
0x004f11a0 claims  8b/10e  measured  8n/11e
0x004f1a80 claims  4b/ 3e  measured  4n/ 4e
```

**Recorded as disagreements, not as 18 wrong claims.** The measurement can be wrong
too - `afi` has been wrong about extent by a factor of 1,000 on the guard and has
reported a frame for a body with no `sub esp`. What is established is that the
comment and the measurement do not agree, so at least one of them is not evidence,
and neither may be cited for these 18 until the disagreement is resolved.

**Some are likely a boundary difference rather than an error:** several addresses
are not 16-byte aligned (`0x004abc46`, `0x004ab733`, `0x004ab84c`, `0x004ab7f5`,
`0x004aad90`, `0x004aad48`), which is consistent with mid-function entries where
the two tools disagree about where the function starts. That is a hypothesis, not a
finding, and it needs the same treatment as the rest.

**Method note, because the first sweep was vacuous.** My initial run compared the
claim against `cfg` on the *manifest record* and returned **0 disagreements** - the
manifest record has no `cfg` field at all, so every comparison was skipped by an
`if nodes` guard and the zero was a statement about my pattern, not about the files.
The CFG lives in `docs/fidelity/evidence/<addr>-binaryninja.json`. Fourth instance
today of a zero from a pattern that cannot match, and the second one that was mine.

## Unreleased

### Nine natives had no manifest record, and one record contradicted its own file

19 natives landed this session; **9 had no manifest record**, so the gate could not
account for them: `0x00450CC0`, `0x00401F60`, `0x00401F80`, `0x0044EEB0`,
`0x004B88C0`, `0x00504A30`, `0x004B3B60`, `0x004B3BA0`, `0x004B3BE0`. All nine are
now built, every one at `score=1.000`.

**And building them exposed a wrong claim I had been repeating.**
`native_504a30.cpp`'s provenance says **"93 bytes, 1 block, 0 edges"**. The extent is
right - `afi` agrees on 93 - but the body contains **3 branches**, so it cannot be
one block with no edges. The measured CFG is **6 nodes, 7 edges**, and Binary Ninja
and Radare2 agree on that independently (`bn=6/7 r2=6/7`).

I had recorded "1 block, 0 edges" in my own notes and repeated it. It came from the
file's comment, which is a claim about a past state, and I treated it as evidence
about the present - the sixth rule, broken by me, on a file I had already reviewed.

**The general lesson, now with a measurement behind it:** a CFG claim inside a
provenance block is not verified by the block containing it. `build_native_records.py`
measures the CFG from the image; the comment asserts it. Where they disagree, the
measurement wins, and the disagreement is itself a finding about the file.

## Unreleased

### The trampoline family: `0x00508FA0` is a separate entry, and the pair is byte-justified

handler-abi-worker resolved the duplicate question from the provenance blocks, and I
confirmed it from the raw bytes:

```
0x00508F80  e84bf40000  call 0x5183d0
0x00508F85  e916000000  jmp  0x508fa0     <- 10 bytes total, then NOP padding
0x00508F90  e8bb060000  call 0x509650
0x00508F95  e9b6f50000  jmp  0x518550     <- a SECOND trampoline, same shape
```

`native_508f80.cpp` claims `0x00508F80`-`0x00508F89`, **10 bytes, 2 instructions** -
byte-accurate. `native_508fa0.cpp` claims `0x00508FA0`-`0x00509292`, 755 bytes.
`afi`'s 787 for `0x00508F80` is the **merge of the two plus padding**, not
containment: 10 + 755 + 22 = 787. **Neither file is a duplicate. Do not withdraw
either.** My containment reading was wrong and the extent field was the wrong
instrument, as suspected.

**This is a family, not an isolated case.** `0x00508F90` is a second two-instruction
trampoline (`call` then `jmp` into a body elsewhere), which is consistent with the
unbuilt `5092a0`/`509440`/`509520`/`509610` files. **Every member needs the guard's
treatment: resolve each `jmp` target rather than reading an extent field.** A count
of "missing" for this family is not a count of missing routines until each `jmp` is
resolved.

### The guard is blocked at depth 2, not merely unbuilt

Both trampoline files are on disk and neither can enter the build:

- `native_508f80.cpp`: undefined `AIGraph_InitChain_5183D0` (`0x005183D0`, no file)
  and `AIGraph_Register_508FA0` (the sibling, itself unbuilt).
- `native_508fa0.cpp`: undefined `AIGraph_Handler_509570`, `AIGraph_Handler_509610`,
  `GraphStream_RegisterNode_4F9DF0`.

So the corrected chain is: `0x00402930` (guard) -> `0x00508F80` (file exists, **cannot
be built**) -> `0x005183D0` (**no implementation anywhere**). "Unbuilt rather than
missing" is right about the file and still leaves the guard blocked, because the file
it needs is itself blocked. `0x005183D0` remains the one real hole and stays
unassigned, recorded as blocked with its callee list.

### A zero from a pattern that cannot match, third instance - this time in tooling

handler-abi-worker searched for a `TaskType_Register` site pushing `0x00508FA0` by
grepping the image for the absolute immediate `a0 8f 50 00`, and found nothing.
**The search could not have succeeded**: `jmp 0x508fa0` is encoded `e9` + a relative
displacement, so no absolute literal exists anywhere in `.text`. The target is
established by resolving the relative branch, not by finding the address as data.

Reported as a wrong search rather than a null result, which is the distinction that
matters - the same shape as a count of zero from an unverified pattern.

## Unreleased

### My guard figure was wrong: 28 missing, not 32 - and "claimed" is not "built"

I have reported "122 of 154 in build, 32 missing" for several ticks. Re-measured
with the strict test - a provenance block carrying `retail VA 0x...`, or a
`native_<hex>.cpp` filename - the answer is **126 claimed, 28 missing**:

`0x4190E0 0x4199F0 0x41A1E0 0x41AAF0 0x41B310 0x41B8E0 0x41C700 0x41D080
0x41DC50 0x41E690 0x41F340 0x420300 0x420CC0 0x421430 0x421750 0x4223D0
0x422B40 0x4230A0 0x423FC0 0x424E80 0x425C80 0x426630 0x426D90 0x4272A0
0x427830 0x427D90 0x427FF0 0x42ACC0`

That is handler-abi-worker's census, address for address - 27 in its band plus
`0x0042ACC0`. It derived the same set independently and flagged its own number as
a floor because its test ("mentioned on any line") was looser than a provenance
claim. The stricter test returns the same 28, so 28 is the figure and my 32 was
wrong.

**And the 126 is not a build figure.** Sweeping tracked files against
`CMakeLists.txt` finds **36 tracked natives absent from the build** - committed, on
disk, never compiled. So "claimed" splits into built and unbuilt, and the guard's
126 includes files the compiler has never seen. The honest statement is:
**28 addresses have no implementation anywhere; of the remaining 126, some are not
in the build.**

Three different questions were being answered by one number - does a file exist,
is it compiled, is it complete - and I had been reporting the first as though it
answered all three.

## Unreleased

### The guard's blocker is one missing address, not two - and my note was wrong

I told the lane that `0x00508F80` "calls `0x005183D0` and `0x00508FA0`, both
absent". handler-abi-worker checked the body instead of the note and half of that
was wrong:

```
0x00508F80  call fcn.005183d0
0x00508F85  jmp  0x508fa0
0x00508FA0  sub esp,0xc0        <- the jump's TARGET, reached from the same body
```

`0x00508FA0` is reached by a `jmp`, not a `call`. **A missing symbol and a missing
routine are different claims** - the `Flow_Start` lesson, second instance.

I then verified it: `pdf @ 0x508f80` shows exactly one reference to `0x508fa0`,
the `jmp` at `0x00508f85`. And `afi` gives `0x00508F80` an extent of 787 bytes,
ending at `0x00509293`, which would place `0x00508FA0` inside it.

**But `afi`'s `size` is a derived field** - the same field that reported `837131`
for the 780-byte guard and 64 for a body with no `sub esp` at all. It cannot settle
whether these are one function or two registered entries sharing a tail, and the
difference decides whether `native_508fa0.cpp` is a rule-13 duplicate.

**Recorded as unresolved, with both readings.** If `0x00508FA0` is a block of
`0x00508F80`, the existing `native_508fa0.cpp` is a second implementation of the
same retail code and must be withdrawn. If it is a separate registered entry that
`0x00508F80` tail-jumps into, both files are legitimate and the `jmp` is the
dispatch. The deciding evidence is whether the registrar tables register
`0x00508FA0` as its own entry - not the extent field.

### Corrected blocker chain for the guard

`0x00402930` -> `0x00508F80` (file exists, untracked, not in CMake) ->
`0x005183D0` (**no implementation anywhere in the tree**).

One missing address at the end, not two. So the guard is not blocked on two
absent routines; it is blocked on `0x005183D0`, and `0x00508F80` is merely
unbuilt. Those are different reports and they imply different next actions.

Also from that body: `0x00508FA0` is the `AIGraph` registrar - `sub esp,0xc0`
with `TaskType_Register` given object size `0xc0`. Frame and registered size
coincide here; they are different quantities. 192 bytes is under the page
threshold, so no `0x004A54A0` probe site, checked against the actual `sub esp`
rather than `afi`.

## Unreleased

### 30 natives are partial ports the gate counts as implemented

`Flow_Start` is not one file with a self-contradicting comment. A sweep for
`Status/limits:` lines that themselves admit an untranscribed part or carry a
`TODO` returns **30 natives**:

`0x00402FF0`, `0x00407D60`, `0x0040FE30`, `0x004123F0`, `0x00413DF0`,
`0x00415070`, `0x00419820`, `0x0042DAC0`, `0x0042DB00`, `0x0042FE40`,
`0x0042FE80`, `0x00460C30`, `0x0046BA60`, `0x0046DD00`, `0x0046DF10`,
`0x00472A70`, `0x00476110`, `0x0047B4B0`, `0x0047E700`, `0x004825C0`,
`0x0048F970`, `0x004978F0`, `0x004B16D0`, `0x004DA1D0`, `0x004E68D0`,
`0x00508FA0`, `0x005093B0`, `0x00509450`, `0x00509480`, `0x00509570`

Every one compiles, links, has a manifest record, and is counted by the gate as
implemented. Each is missing part of its retail body, and the only place that is
recorded is a comment inside the file itself.

**This is a measurement, not a defect list.** The distinction matters: some of
these may be legitimately blocked on an untranscribed dependency, which is the
honest state under 10.13 rule 1 ("leave it blocked and record the gap"). What is
wrong is that the *gate* cannot tell a complete port from a partial one, so
"implemented" currently means "a file exists". Any progress figure computed from
file counts - including the guard's 122 of 154 - inherits that ambiguity.

Found while chasing `Flow_Start` after handler-abi-worker flagged it. Not fixed
here: completing 30 bodies is transcription work, and deciding what the gate may
count is a separate decision that should be made once and applied to all 30.

## Unreleased

### A provenance block that contradicts itself: Flow_Start is a partial port

`flow.cpp` says two different things about `Flow_Start` (`0x00402070`):

- `:305` `// Native: Flow_Start; retail VA 0x00402070.` and a header comment
  describing it as a complete port;
- `:211` `// TODO(0x0040208B): Flow_Start must store the record 0x004012A0
  returns at +0x48; the port's Flow_Start does not yet, so this walk finds no list.`
- `:309` `// Status/limits: STRUCTURALLY_VERIFIED for the two stores and the call.
  The record link and the +0x48 store are not transcribed yet (see the TODO above).`

Retail's `0x00402070` is **460 bytes**. The port has the two stores and the call,
and explicitly not the record link or the `+0x48` store.

**The gate sees `Flow_Start` as implemented.** The extractor returns it, the
manifest has a record, it compiles and links - and part of its retail body is
missing. This is the "green signal that means less than it appears" class, and the
only thing that caught it was reading all five records from one file together
rather than checking them one at a time.

Found by handler-abi-worker while running the extractor across its own files. Not
fixed here: correcting the body is a transcription task, and correcting the status
is a decision about what the gate is allowed to count.

### 0x00402780: an unclaimed address between two implemented neighbours

The extractor returns five records from `flow.cpp` - `Flow_Init` `0x00401FC0`,
`Flow_Start` `0x00402070`, `Flow_RunHandler` `0x00402260`, `Flow_Delete`
`0x00402740`, `Flow_CreateHandler` `0x00402800`. `0x00402780` is not among them,
and it sits **between `Flow_Delete` and `Flow_CreateHandler`**, inside a
contiguous run of implemented members of the same handler set.

That is stronger than the adjacency argument that first flagged it: it is not just
near a body on the critical path, it is a gap in a run whose neighbours are all
present - the same shape as `0x00567C5C` inside the `0x00567Cxx` run. Two
independent instances in one session of a missing member inside a populated run.

Also corrected: `flow.cpp`'s real names are `Flow_RunHandler` and `Flow_Delete`,
not the `_Handler`-suffixed forms recorded earlier. Those came from inferring a
suffix off the `FrameHandler` type rather than reading the file - a name supplied
from a pattern, which is the same error as a filename used as a proxy.

## Unreleased

### Binary Ninja evidence records: content verified, provenance not

`scripts/build_native_records.py` writes `<addr>-binaryninja.json` and
`<addr>-radare2.json`. For `0x004FDEF0`, `0x004F0F10` and `0x00450C80` the
manifest records now point at the real port paths instead of
`scripts/GameNatives/...` candidate paths, and the CFG shape in both files
(1 node, 0 edges) matches an **independent** r2 measurement of the same entry
(`size: 6`, 1 block, 0 edges) - so the content is consistent with reality.

**But I could not confirm Binary Ninja produced the `binary_ninja` records**, and
am recording that rather than letting the filename stand as the claim:

- `python3 -c "import binaryninja"` fails locally.
- `scripts/bn_mcp.py call bn_function_basic_blocks '{"function":"0x004FDEF0"}'`
  prints what is a **parameter echo** - `tool`, `function`, `arch`, `name`,
  `platform`, `offset`, `limit` - and no CFG. That is a request, not a result.

So `"tool": "binary_ninja"` in those files is a provenance claim I have not
verified, on a target where the preferred decompiler may be unavailable. The
record is still useful - its shape agrees with r2 - but it is **not** independent
decompiler evidence, and no native should be promoted on it as though it were.
Resolve before any status above `BUILD_VERIFIED` depends on it.

## Unreleased

### A transient snapshot is not the state: I reported two false facts from one

Two claims in this session came from a `git diff` and a build run taken while
another agent was mid-operation, and both were wrong about the tree:

1. **"975 lines deleted, 17 tracked natives gone, build red."** The build really
   did fail at that instant (`exit=2`, 16 `No such file or directory` errors,
   with all 17 still listed in `CMakeLists.txt`). Minutes later: zero deletions,
   files restored, build `exit=0`. I had a snapshot of an operation in progress.
2. **"The tree carries both 8-digit and 6-digit duplicates."** Measured now: 35
   files match `native_00*.cpp`, and **zero** of their VAs have a 6-digit
   counterpart. There is no duplication. The earlier `ls` that "found" 6-digit
   files was seeing a rename in flight that was then reverted.

So the correction is not that the build was fine or that the files were
duplicates - it is that **`git diff`, `git status` and a build are measurements of
a moment in a workspace with three concurrent writers, and I reported them as
measurements of the state.** This is the stale-archive failure one level up: there
the input was old, here the input was mid-flight. Both produce a confident,
specific, wrong number.

The rule this earns: **before reporting a fact about the tree, take the same
measurement twice with a gap, and report a disagreement instead of the first
reading.** Both false claims above would have been caught by one repeat.

Also recorded from this tick: `check_native_linkable.py` answered from
`build/engine/libigi_retail_engine.a` at mtime Oct 20 - five days stale - and
returned `NOT LINKABLE` for two natives that link cleanly. handler-abi-worker
reported both results rather than the favourable one, and quoted path, mtime, size
and date. A checker that answers from a stale archive needs to refuse rather than
answer.

# Changelog

## Release policy

A version is published **only** when the built `igi1.exe` starts, keeps running,
and a screenshot of the running game is captured and attached to that version.
A build that fails closed at a documented boundary (for example the registrar
chain throw at `QTask_RegisterClasses 0x402930`) is an **internal build**: it is
recorded under "Unreleased" with its evidence, and it never gets a version
number and never counts as a release.

## Unreleased

### 2026-10-25 - the 34 was wrong too: I dispatched two natives that were already built

The 34-callee list I gave `handler-abi-worker` contained `0x00401FC0` and
`0x00415530`. **Both were already implemented, tracked, in CMake and built** —
`core/flow.cpp` defines `Flow_Init` for `0x00401FC0` and `core/game_flow.cpp`
defines `Game_Init` for `0x00415530`, and `nm` on the built archive shows
`T _Flow_Init` and `T _Game_Init` already present. The worker wrote a second
`Flow_Init` in `native_401fc0.cpp` before I caught it. Adding it is a duplicate
symbol and a §10.15 rule-13 violation, so it is on disk, unintegrated.

**Cause of my error:** I matched the guard's call table against filenames of the
form `native_<hex>.cpp`. A callee implemented in a *named* file — `flow.cpp`,
`game_flow.cpp`, `game_flow.cpp` — therefore read as missing while being
implemented and built. The pattern described the naming convention, not the
world.

Re-measured against `retail VA 0x...` provenance blocks in *every* source file:

| measure | figure |
| --- | --- |
| distinct guard callees | 154 |
| have a provenance claim in some file | 121 |
| **no file claims the VA** | **33** |

**That figure was wrong too, and correcting it is the useful part.** A fourth
measurement, using the union of three independent populations, gives the answer.
"In build" = (a CMake-listed `native_<hex>.cpp`, VA from the filename) OR (a
CMake-listed file whose provenance says `VA 0x...`) OR (a bare
`0x00XXXXXX-0x00XXXXXX` range on a provenance line). Each measure alone misses a
real file:

- filename-only missed `flow.cpp` and `game_flow.cpp` — named files, implemented and built;
- provenance-only missed `native_440710.cpp` and `native_440c20.cpp`, which carry their
  VA **only in the filename** and on a bare `0x00440710-0x0044083B.` line, never as
  `VA 0x00440710`. Those two files' only `VA 0x` tokens are section addresses —
  `0x00533000` and `0x00536000` — mentioned in prose, which is how a pattern-based
  reader gets a confident wrong answer from a real file.

**Final: 122 of 154 in build, 32 missing** — 27 in the `0x00400000`-`0x0042FFFF`
band, `0x0042ACC0` for the Lead, and `0x00449D30` / `0x0044C8D0` / `0x0045E450` /
`0x00508F80` for the `0x00430000`+ band. `0x00508F80` is blocked rather than
assigned: its file is tracked and compiles, but its body calls `0x005183D0` and
`0x00508FA0`, both absent, so adding it to CMake would break the link.

Of the four I had flagged that did have a file: `0x00401FC0` and `0x00415530` are
tracked and built; `0x0044C8D0` is untracked; `0x00508F80` is tracked but its
`native_508f80.cpp` is **not in CMakeLists.txt** and so is not in the build.

**Third bad count from me this session**, after the eight-hex-digit filename
pattern and the two-tables-numbered-from-one split. All three are the same
failure: I reported a number produced by a pattern whose match I had not
verified, and the number was confident enough to dispatch work off.

`native_401fc0.cpp` versus `flow.cpp` is now a real question — two transcriptions
of one VA, 80 lines against 537. One of them is wrong or they cover different
scopes, and that needs deciding from the bytes rather than by deleting the
newer one.

### 2026-10-25 - a third class of entry point: handler-only, and the 34 was not the whole gap

`handler-abi-worker` found that `0x00401FC0` pushes four code addresses as handler
arguments — `0x00402070`, `0x00402260`, `0x00402740`, `0x00402780` — none of which
is the target of a direct `call`. I checked all four against the guard's 155-call
target column: **none is in it.** I also checked `0x004170A0`'s two successors
(`0x00417260`, `0x004191F0`): **neither is in it either**, while `0x004170A0`
itself is.

So these are a **third class**, not missing guard callees. They are reachable only
through handler dispatch: the guard calls the registrar, the registrar links the
handler, and no direct call edge to the handler exists anywhere. A call graph
cannot find them and the guard's own table does not list them.

**Scale, as an upper bound.** A byte scan of `.text` for `push imm32` whose
immediate falls inside `.text` (`0x00401000`-`0x00532510`) gives **2515 sites,
2001 distinct code addresses**. Of those distinct addresses, 44 are in the build.
That yields an upper bound of **1956 handler-only entry points not built**.

This is an upper bound and is labelled as one: not every `push` of a code address
is a handler link — function-pointer tables, jump tables and other data can hold
one too. What it establishes is the order of magnitude, and that the number is
not 34. The 34 remains the correct count for *getting startup through the guard*;
the handler-only class is what gets reached progressively as events fire.

**My error, caught by the worker.** I told it `0x004B8810`'s shape was
`(name, 0x181)`, generalised from `QTask_Init`'s single call. Its five sites push
1-5. The shape `(name, value)` holds; the value is per-site and `0x181` is one
body's value, not the callee's signature. I generalised a constant from one
observation into an interface while in the same message warning about counts.

**A tool-artefact error the worker caught in itself.** It reported `stackframe: 64`
for `0x00401FC0`; the body contains no `sub esp` and no callee-saved pushes at
all. `afi`'s `stackframe` is a derived field and must be confirmed against an
actual `sub esp` before being quoted, the same class of error as reading a
comment as evidence. Its conclusion survived and got cleaner: with no frame,
`0x54` is unambiguously the registered heap object's size.

**The rule confirmed on a second body, independently.** I read `0x0042ACC0` (a
guard callee in the Lead's band) and r2 reports `stackframe: 2316` for it. The
actual prologue is `sub esp,0x8E0`, i.e. **2272**. The difference, 44 = `0x2C`, is
the pushed-argument space r2 folds into the field. So `afi`'s `stackframe` is not
the frame: it is the frame plus outgoing argument space, and it must be confirmed
against the prologue's `sub esp` in every body, not only the ones with no frame.

`0x0042ACC0` itself: 870 bytes, 1 block, 0 edges, 45 call sites over 24 distinct
targets, `cdecl`, no arguments. It registers the class `"Train"` (literal
`0x0053BD1C`) through `TaskType_Register` at `0x00401900` with a registered object
size of `0x8E0` — **equal to the frame here**, unlike `0x00401FC0` where the
registered size had no frame to be confused with. The id goes to `0x0057BCB8`
with a 16-bit store, and `0x00429F80` is linked as a handler. Reconnaissance only;
no file written.

### 2026-10-25 - startup guard re-measured: 120 of 154 callees, not 8

`0x00402930` is the boundary this build fails closed at, and it is the only thing
between the tree and a window. Its coverage had never been measured against the
tree, only asserted in a comment, and the assertion was badly wrong.

| source | claimed |
| --- | --- |
| `class_registration.cpp` provenance | 8 of 154 callees implemented |
| `startup-402930-call-sequence.md` status column | 22 implemented |
| **measured against `CMakeLists.txt`** | **120 of 154 in the build, 34 missing** |

The measurement matches the doc's ordered 155-call table (rows before line 186 —
the doc holds a *second*, 154-call sequence at `0x00402C40` after that) against
the `native_*.cpp` entries in `CMakeLists.txt`. Distinct callees, not call sites:
154.

Two attempts at this measurement returned zero before the real one, both by the
mechanism this project keeps rediscovering. The first matched `native_*.cpp` with
an eight-hex-digit pattern when the filenames carry six (`native_401d30.cpp`), so
every lookup missed. The second split the doc's two call tables by row number
when both number from 1, reading 155 + 154 rows as one 309-row table. A zero from
a text pattern is a statement about the pattern.

The 34 missing callees are assigned: 29 to the `0x00400000`-`0x0042FFFF` band,
`0x00449D30` / `0x0044C8D0` / `0x0045E450` to the `0x00430000`+ band, and
`0x0042ACC0` / `0x00508F80` to the Lead.

### 2026-10-25 - two registrars integrated, raw-address and initialiser sweep

Added `Task_DefinePatrolPathCommand` (0x004533B0) and `Task_DefinePatrolPath`
(0x00453550) to the build. Both are compile-only, both linkable, and both confirmed
present in `igi1.exe` by symbol lookup rather than by a green build — the check that
matters here, because a stale cmake configure reports zero errors while never compiling
an added file.

Fixed ten sites across four files that dereferenced or stored a retail image address
while the port defined a mirror of it (`native_46e370`, `native_473ed0`,
`native_4813e0` x7, `native_485330`). All ten compiled and linked clean, because the
defect is not a missing definition but a definition that exists and is bypassed.

Gave five globals their retail image initialisers, all previously zero:
`g_54f7a0`, `g_54ed4e`, `g_54875c` to `0x181U` and `g_54875e` to `0xFFU`. Zero-init meant
"class 0" and "event 0" before registration instead of the unregistered sentinels. This
image has no `.bss`, so the image bytes fix the initialiser.

The discriminator for the whole raw-address class was wrong at first: "address
>= 0x00500000 is data" flags correct code, because `.text` runs to `0x00532510`.
`0x0051B940` is `.text` — a real handler — and appears as one in `native_42d2b0.cpp:48`
and `native_42beb0.cpp:52`; both were left alone. The predicate is PE section membership,
not magnitude. `scripts/check_native_raw_addresses.py` now uses membership and reports
788 scanned / 17 flagged.

Five remaining gaps stay blocked rather than filled: `0x00A969E0` (extent proven at
252 B, definition and owner missing), `0x0057B5E8` / `0x0057B1A8` (extent unestablished;
the `++::g_57bab4` walk degrades into unmapped memory each call), the eleven render
getters (owner is the untranscribed writer `0x00491610`), and `native_44c8d0.cpp` (held
untracked, five undefined symbols). An invented size or constant would make the reads
look correct while producing plausible wrong data.

### 2026-10-25 - script caller ABI recovery

Traced all 81 retail GOInRemap calls to the indirect dispatch at 0x004BD8FE in
0x004BD880. Fresh BN/r2 CFG similarity is 1.0 (12 blocks), with exact angr entry
recovery. Recorded the three machine argument slots, caller-owned 24-byte result,
frame construction and result-type branches in
`docs/fidelity/evidence/remap-caller-20261025/`. This establishes why the existing
simplified config frame cannot substitute for this VM path. Retail-only research;
the indexed argument reader and candidate implementation remain to be checked.

### 2026-10-25 - retail input-remap ABI capture

Recorded 81 retail GOInRemap calls and their 243 ordered argument-reader calls.
The caller-buffer return, third-argument frame identity, masked table writes and
counter increments all matched the reviewed instructions. Retail output words
1 and 5 contained nonzero copied stack residue, confirming another difference
from the candidate's private zero-filled buffer. Preserved the raw capture,
probe, hashes, cleanup and correction requirements in
`docs/fidelity/evidence/remap-abi-20261025/`. Retail-only research: no candidate
acceptance, native status promotion or movement claim.

### 2026-10-25 - instance-handle controlled comparison

The single-command verifier rebuilt the candidate and passed the `0x0048F0D0`
static/semantic gate (CFG 1.0, exact angr entry) and all eleven controlled paired
Frida cases with the unified probe. Natural startup capture then blocked because
compiler inputs no longer matched the build receipt. Overall acceptance remains
pending; no verified database entry or gameplay claim was added. Reports:
`build/native-auto/runs/20261025T064528Z-c23776a4/0x0048F0D0/attempt-1/` and
`build/native-one/0x0048F0D0/20261025T064620Z-3df9b325/`.
The private Frida service passed RPC health and was terminated with its listener
closed. Updated `TOOLS.md` to identify the autonomous verifier's actual candidate
path separately from the diagnostic deployment.

### 2026-10-25 - current configuration observation and second startup probe

Decoded the installed configuration rather than reusing the old 27-binding scenario.
Current `config.qvm` SHA-256 is `4743dd505e4bfce8aa50b0f5d281974daf24d21fe2f771a3faaa86e68ca60e0d`.
Converter versions 1.10.1 and 1.11.0 produced identical decoded text. A fresh
retail-only pre-resume Frida run observed all 81 remap calls, with 27 sequential
table writes for each of three profile-count values. The player QVM is unchanged.
Raw traces, configuration/tool hashes, selected BN plus r2/angr analysis and owned
process/server cleanup are retained under `docs/fidelity/evidence/current-config-20261025/`.
Active-profile dispatch and candidate movement remain unverified. The existing
engine `GOInRemap` interface/private return block differs from retail and must be
corrected before it can be accepted.

Added a unified controlled/natural probe for the existing `StoreInstanceHandle`
native (`0x0048F0D0`), retaining the eleven controlled inputs and requiring the real
WinMain instance/store/return aliases plus unchanged neighboring bytes. No engine
implementation changed. Fresh static analysis completed; three full-command
attempts were invalidated by concurrent compiler/source changes, including
`native_440fd0.cpp` and `native_4170a0.cpp`. The probe remains provisional and no
native acceptance was added. The earlier annotation-only source proof is retained;
historical runtime reports were neither rewritten nor transferred to a new image.

### 2026-10-25 - retry concurrent edits inside the native verification command

The automatic verifier now checks the input fingerprint inside its bounded attempt
loop. Previously, a native could pass all subphases and then be invalidated by a
concurrent checkout edit outside that loop, leaving the remaining attempts unused.
It now rejects that attempt, captures fresh inputs, and reruns the complete pipeline.
A changed or ambiguous native mapping blocks instead of silently retargeting the test.
The final input check and verified-only database requirements remain enabled.

Private Frida cleanup also identifies the detached Wine server by its exact
private endpoint and executable command, excluding pre-existing PIDs and the
launcher. It rechecks ownership before signalling and checks that the listener
closed; launcher exit alone no longer counts as successful cleanup.

Validation: 42 automation/repair/gate/database infrastructure tests passed, including
fresh-input retry, mapping-change rejection and owned-server cleanup. Two live `0x0048F0C0` runs passed
their static/semantic/controlled/paired-runtime subphases for their tested images,
then correctly remained pending after concurrent input changes. These observations
do not accept the current tree or establish playable Level 1.

The repaired command completed all three attempts in
`build/native-auto/runs/20261025T060637Z-a627d4c7/0x0048F0C0/`:
each comparison passed for its tested image and each attempt stayed pending after
input drift. The third live run confirmed actual private-server termination and
listener closure in `build/native-one/0x0048F0C0/20261025T061116Z-d21f74d0/frida-service.json`.
No native was added to the verified-only database.

### 2026-10-25 - 21 provenance comments asserted load addresses the image does not contain

A real evidence defect, found by the agent that introduced it and verified here against
the image before being accepted. Twenty-one getter provenance comments claimed a global
address that retail does not load:

| native | claimed | actual |
|---|---|---|
| `Core_GetRenderWordA_4F0FA0` | `0xA71944` | `0xA70C7C` |
| `Sys_GetGlobal32_4F1010` | `0xA70D10` | `0xA70C88` |
| `Sys_GetGlobal32_4F0F70` | `0xA71878`/`0xA718D0` | `0xA70C6C` |
| `Sys_GetGlobal32_4F0F60` | `0xA71880` | `0xA70C68` |
| `Sys_GetGlobal32_4F0F20` | `0xA71730` | `0xA70C84` |

The cause was writing the comments from a memorised table instead of reading each
getter's bytes. One claim was literally its neighbour's address — an off-by-one in a
memorised table, which is how this class of error is born.

**Scope: comment-only, independently verified.** The port bodies load the correct
globals, so no port code was wrong, no call census was affected and no build changed.
But a reviewer asking "does this getter match retail" would have read a false answer
from the provenance block, and the provenance block is the evidence trail. A wrong
comment is not a wrong program, but it is a wrong record, and this project's acceptance
depends on the record.

All 21 corrected, and all 89 getter comments across that lane's 30 files are now
machine-checked against r2 with zero mismatches. That check is now part of the pre-write
routine rather than something done after a defect is found.

### 2026-10-25 - scoped angr evidence reuse, and the guard tests it broke

Five gate scripts gained a **scoped angr reuse** path. It does not skip verification:
`reuse_analysis` in `scripts/angr_analyze.py` accepts a prior capture only when the
report's SHA-256 matches, the recorded helper hash equals the current helper's hash, the
interpreter path and hash equal the current interpreter's, the imported angr version
matches the recorded one, the binary hash and mapped base (0x400000) match, and every
requested address has an exact function entry with `has_unresolved_jumps is False`.
`scripts/workflow_tools.py` also hashes helper and interpreter before and after the
capture and **fails the capture if either changed during the run**.

The change added a `warnings` parameter to `verify_record()` and broke two guard tests,
which then failed on a signature mismatch rather than on the property they guard. They
were fixed by supplying the argument, not by relaxing the assertion: the
phase-order test still asserts that a failed static or semantic check leaves the
runtime runner **not called**, and the wrong-candidate test still asserts that an
unbound source blocks without hashing a different implementation. 80 tests pass.

The two scripts that were never pinned are now pinned:
`scripts/angr_analyze.py` and `scripts/workflow_tools.py`. The first is the script that
produces angr evidence and the second orchestrates the capture, so both belong under the
pin and their absence was a gap rather than a decision. 36 files pinned; gate exits 0.

Integrated the upper lane's `Task_DefineRadio` (`0x0043AD80`),
`Task_DefineGenerator` (`0x0043B2F0`) and `Task_DefineSimpointObjGenerator`
(`0x0043B850`). Green at 14,379,887 bytes.

### 2026-10-25 - the container argument is a capacity, not the record count

**A correction to a rule stated earlier in this file, made by the agent that proposed
it and verified here against both callee bodies.** The three-number rule for
multi-block bodies said the allocation count witnesses the loop trip count. It does
not, in the form stated.

`Script_TaskContainer_Alloc` (`0x004F1950`) computes `size = count*16 + 12`, allocates,
then stores `[base+4] = count`, **`[base] = 0`** and `[base+8] = base+12`.
`Script_TaskContainer_Append` (`0x004F1A80`) reads `esi = [base]`, shifts it by 4, writes
the 16-byte record at `payload + [base]*16`, and then **increments `[base]`**.

So `[base]` is a live record count starting at zero and `[base+4]` is the capacity. **The
allocation argument is an upper bound.** The correct check is `records <= capacity`, not
`records == capacity`.

The evidence that forced it: `0x0043B850` allocates with 13 and appends 12 records in a
single basic block, with no loop that could reconcile the difference. In
`0x004397C0` (8), `0x0043AD80` (6), `0x0043A840` (12) and `0x0043B2F0` (5) the two
numbers happened to meet exactly, which is why the check looked like a witness. In those
bodies it still was a real check, because a wrong trip count leaves records *below*
capacity and breaks the equality — but it is a check that can only fail, never one that
proves a count on its own, and here it does not even hold.

**The honest form: capacity bounds the records, and a loop's trip count is confirmed
only when the two meet exactly.** A mismatch is not by itself an error, and
`0x00407290` — 52 appends over two allocations — must not be read as one if it has
slack.

The mechanism also explains a field recorded as unexplained in several files: the
capacity at `+4` is written by `Alloc` and never read by `Append`, so the container
carries its capacity without enforcing it. Nothing in either lane reads it; if a handler
does, that is where the bound becomes load-bearing.

### 2026-10-25 - the frame shift is a per-prologue fact: three bodies, three answers

Three consecutive registrar bodies in one lane, one family and one `lea`-into-frame
idiom, produced **three different saved-register shifts**:

| body | prologue | saved registers | shift |
|---|---|---|---|
| `0x004397C0` | `sub esp,0x1F8` then `push esi; push edi` | 2 | **8** |
| `0x0043AD80` | `sub esp,0x2E8` then `push ebx; push esi; push edi` | 3 | **12** |
| `0x0043B2F0` | `sub esp,0x2C0`, no saved-register push | 0 | **0** |

Every one of those bodies resolves `lea reg,[esp+K]` into a frame offset, so the shift
must be applied to all of them and its value differs in each. The wrong shift yields
displacements that look entirely plausible - `0x004397C0`'s retail 0x104/0x11C/0x1B8
become frame+0xF0/0x108/0x1A4 with the shift applied, and frame+0x104/0x11C/0x1B8
without it, and nothing in the file would look wrong either way.

The useful refinement is that the shift is a property of the **prologue alone**, so it
is cheap to read and expensive to guess. It is now a separate step before any `lea` is
resolved, and the shift is recorded explicitly in each file so a reader can tell
whether the author accounted for it.

Recorded because a single instance would have looked like a quirk. Three instances in
one family make it a rule with a cost attached.

### 2026-10-25 - two more registrars, and three gap getters that exposed a 0xFF initialiser bug

Integrated `core/native_438390.cpp` (`Task_DefineGunShotEffect`, 163 B, retail 11/8 =
port 11/8) and `core/native_4388b0.cpp` (`Task_DefinePlaceExplosiveTBA`, 234 B, retail
15/11 = port 15/11).

**The first build of these failed**, and the reason is a cross-lane dependency rather
than an error in either file: `native_438390.cpp` calls the byte getters
`0x00408320`/`0x00408330`/`0x00408340`, which are in the 0x0040xxxx gap band assigned
to the other lane and did not exist. They are three six-byte natives,
`mov al,byte [0x00538518/19/1A]; ret`, and are now transcribed with their own
provenance in `core/native_408320.cpp`, `native_408330.cpp`, `native_408340.cpp`.

Transcribing them exposed a **fidelity bug in an existing file**.
`render/native_00407d60.cpp` is the writer of all three bytes and declared each
`= 0U`. Their **image values are all `0xFF`**. The initialiser decides what a reader
sees before the writer runs, so a getter read before `0x00407D60` executed returned 0
in the port and 0xFF in retail. All three are now `0xFFU`. The three new getters only
`extern` the storage, because the ownership rule puts it with the writer.

Also recorded, from chain-worker's report and verified here against the PE section
table: **an address alone cannot classify a global.** `.rdata` is bounded by
VirtualSize `0x28F6`, ending at `0x005358F6`, while its raw size is `0x3000` and
file-aligned tooling (including Radare2's `iS`) reports the larger figure. Most of the
`0x0053xxxx` range -- including the `"IGI"` title string at `0x00536144` and every
`0x005690xx` global -- is `.data`, not `.rdata`. There is also **no `.bss` section in
this image**; `.data`'s tail is zero-filled, so a zero-initialised global is `.data`
with a zero initialiser and `B`/`D` is a naming convention rather than a section fact.
See `docs/fidelity/evidence/pe-section-table-20261025.md`.

Green at 14,369,089 bytes.

### 2026-10-25 - 0x00406450 lands, and the handler-ABI lane closes at 15/15

`core/native_406450.cpp` transcribes retail `0x00406450` (`Task_DefineHumanAIConfig`,
placeholder name), the function the handler-ABI lane has been decoding all session.
It is the last piece of the guard's first call group that was blocked on analysis
rather than on missing work.

Its 35 `call 0x4F1A80` appends, two registrations, nine handler links, two container
allocations and four config stores were derived **mechanically** rather than from the
hand-built table: `pd` over the exact image with esp simulated from `sub esp,0x428`,
replaying every `push`, `lea reg,[esp+K]` and `mov reg,[global]` and taking the last
six pushes at each append. The independent derivation reconciles with the retail call
census on all fifteen targets:

    0x4F1A80 x35  0x4F0F50 x31  0x4F1C70 x13  0x401400 x6   0x4F1C90 x6
    0x4F1CB0 x5   0x401530 x3   0x4018F0 x2   0x401900 x2   0x4F1A60 x2
    0x4F1A70 x2   0x4F1950 x2   0x4F0FF0 x2   0x4F0EF0 x1   0x4F0FA0 x1

and on the arg6 distribution -- 0x4F1C70's return x13, 0x4F1C90's x6, 0x4F1CB0's x5,
the literal 0x5690C0 x7, the literal 0x5690B8 x1, zero x3 -- which is the same
13/6/5/7/1/3 the lane had already reconciled by hand.

**Three corrections to the hand-built table, all found by the independent derivation.**

The table's first group spans two appends, and its aggregate row gives `arg5=7` for
both. `0x004065ED` is actually `arg5=3`, `arg3=frame+0x128`, `arg2=frame+0x00` -- it
is the first of the `0x4F0FF0`-producer pair, not a sibling of `0x00406522`. That is
the second time this session a group boundary has hidden a per-site difference, after
the 35-vs-34 split and the `0x4F1C90` 5-vs-6 correction.

The lane's stated split of the nine links was `0x00401400` x5 + `0x00401530` x4. The
image says **6 + 3**. The total of nine was right and the arithmetic argument built on
it still holds; only the split was wrong, and the split is what identifies which link
is the context variant.

`0x00406522`'s six pushes are not cleaned by a local `add esp,0x18` -- they are cleared
by the `add esp,0x3C` at `0x00406564`, together with the three links that follow it.
The site is still an append, but its cleanup is shared, exactly as `0x00406B48`'s is
shared with the `add esp,0x440` epilogue.

Green at 14,367,160 bytes with the symbol at 0x00476800. Status is
STRUCTURALLY_VERIFIED, not higher: the static reconciliation is exact, but no runtime
observation of this native exists in either binary.

Also integrated: `core/native_437510.cpp`, chain-worker's `Task_DefineSmoke`
(1,471 B / 370 instructions / 65 calls, retail 65/22 = port 65/22), landed in one turn.
It carries a finding worth keeping: one of its nine links registers a **NULL handler**
(`0x0043771F`, event 4), which no handler-immediate scan can see. What located it was
arithmetic -- the container's `add esp,0x1C` closes only if two links plus the count
push are outstanding.

### 2026-10-25 - the entry function's retail callees become natives instead of local stand-ins

Three rule-9/13 violations on the MVP's first real function, found while mapping
the entry chain, and removed.

`core/qtask_create_task.cpp` transcribes retail `0x00401000`, the third statement
of which is the class-registration guard. Its body called three retail routines
that existed only as anonymous-namespace helpers inside the same translation
unit: `SetAppTitle` `0x0048F100`, `SetForcePrecompiled` `0x0048F330` and
`GetStartupMode` `0x0048F220`. A helper standing in for a retail routine is
exactly what rules 2 and 9 forbid, and `SetAppTitle` was worse than a stand-in -
it was a **second** implementation of `0x0048F100`, shadowing the real
`extern "C"` native in `core/app_context_extra.cpp` inside that unit, and it read
a second variable for the retail window handle `0x005C8BC4` (`g_retail_window`)
where the real native reads `g_window_handle`. Two variables for one retail
address is the duplicate-implementation smell in rule 13.

Now `0x0048F330` and `0x0048F220` are transcribed as their own natives with
their own provenance blocks:

  `core/native_48f330.cpp` `SetForcePrecompiled` 9 bytes `8b 44 24 04 a3 f0 8b 5c 00 c3`
  `core/native_48f220.cpp` `GetStartupMode`      6 bytes `a1 08 8c 5c 00 c3`

and `0x00401000` calls the real `SetAppTitle`. The single caller of each is the
entry function itself (`0x0040100C` and `0x00401020`), so their caller sets are
fully enumerated rather than sampled.

Storage ownership follows the writer, as with `0x005C8BEC` before it: the
`0x005C8BF0` flag was defined inside the anonymous namespace of
`script/qvm_native_loader.cpp`, which is the *reader* side, so no other unit
could bind to it. It now lives with `0x0048F330` and `qvm_native_loader.cpp`
declares it `extern "C"` - one definition, two users.

Green at 14,354,342 bytes. Statuses stay STRUCTURALLY_VERIFIED: the call
sequence is now the retail sequence, but `QTask_RegisterClasses 0x00402930` still
throws on its untranscribed registrars before the flow calls run, so nothing here
is promoted on the strength of a build.

### 2026-10-25 - the startup-chain keystone lands and the hold-out drops from ten files to two

- **`0x004AF9D0` is transcribed and in the build** (`sys/native_4af9d0.cpp`, 1190 bytes / 68 blocks / 329 instructions / 97 edges).
  It is a **pattern compiler**: it hands the pattern to `0x004AFFB0` for translation, fills the class-record area through
  `0x004AFF90`, then compiles the translated pattern into 0x14-byte nodes in argument 1, with two switches decoded from the image -
  ten metacharacter classes (`^ $ . * ? [ ] ( ) { } / \`) and a backslash-escape handler (`N/n`, `R/r`, `T/t`), plus a character-class
  parser with `~` negation and `x-y` ranges and a 32-entry deduplicated class-record table.
- **The hold-out falls from ten files to two.** The linker, not a guess, released them one by one: `0x004AF9D0` released
  `sys/native_4f1db0.cpp`, `core/native_413df0.cpp` and `core/native_47cf20.cpp`, and through them `413b50`/`413b60`/`413de0` and
  `477e30`; `0x0047CF10` released `core/native_46db40.cpp` and `core/native_46d420.cpp`. Remaining: `core/native_412420.cpp`
  (needs `0x0045E060`) and `core/native_4696b0.cpp` (needs `0x004B7F40`, a new gap).
- **A name mismatch caught by the linker, which is the only thing that could have caught it.** `sys/native_4f1db0.cpp` declared the
  keystone as `Sys_InitRecordPattern_4AF9D0` returning `void` while the owner file defines `Sys_CompilePattern_4AF9D0` returning
  `std::int32_t`. C linkage made both silently plausible; the link failed on the name. The caller is aligned to the owner's
  evidence-derived name and the return type corrected, because the body returns 1 on success and 0 at its fail block and a `void`
  declaration would have dropped that silently.
- **The parked `413b20/413b30/413b40` fix is applied.** Those three had been reading a private `0xFF` byte where retail reads the
  global `g_5391ac` owned by `413df0`; with `413df0` linking, the fix is committable and the patch file that carried it is retired.
- **Honest CFG position on the keystone, recorded rather than smoothed over**: the object is 88 blocks / 396 instructions / 130 edges
  against retail's 68 / 329 / 97. That is **not** structural equivalence and no such claim is made. What is shown is a per-case
  constant signature proving all fifteen case bodies appear exactly once each, and an instruction-level explanation for every
  constant divergence. The keystone is `BUILD_VERIFIED`, not `STRUCTURALLY_VERIFIED`.
- **Build green, 14,340,995 bytes.** Also added: `core/native_46cb70.cpp` (GunPickup) and `core/native_46c0c0.cpp` (GenericPickup).

### 2026-10-25 - `0x004F1A80` six-argument backfill, and three new natives

- **Backfill complete**: 11 caller files, 52 call sites, all now passing the sixth argument. Nine files were mechanical; the two `%eax`
  sites were read by hand and both turned out to be **real argument values, not leftovers** - `0x0047819F` passes the constant
  `0x0056E228` returned by `0x00413DE0`, and `0x00488CEE` passes `0x00A76BE8` returned by `0x004F1CB0`. `native_477e30.cpp` had been
  discarding the first value with a comment asserting the slot is never read; that comment was my wrong claim propagating into a file,
  and both the code and the comment are corrected.
- **Falsifiable shape recorded for the sixth argument**: of 52 sites, 45 pass `0` and 7 pass an address, and in three of four files the
  same registrar body zeroes that global immediately before the appends. So the sixth word is either NULL or the **address of a static
  object** - never a type id and never a small integer. A future site passing a small non-address value falsifies one of the readings.
- **Three natives added to the build**: `core/native_46e370.cpp` (AIStationaryGunHolder, size `0x26A0`, eight-entry container, per-target
  call table 33/18 against retail's 34/19 with `0x004A54A0` the only difference - the MSVC 6 stack probe, which the C++ frame replaces,
  following the precedent recorded for `0x00512C30` in `terrain_draw.cpp`), `sys/native_4aff90.cpp` (fills 256 bytes with `0x01`, stores
  1, **zero undefined symbols**), and `sys/native_4affb0.cpp` (`Sys_TranslateWildcardPattern_4AFFB0`, 576 bytes / 41 blocks / 187 insns,
  **zero undefined and zero calls**).
- **`0x004AFFB0` decoded from its tables rather than guessed**: the 86-byte class table at `0x004B0218` assigns class 0 "(", 1 ")", 2 "*",
  3 "+", 4 "/" and "\", 5 "?", 6 "[", 7 "{", 8 "}", 9 the remaining 76 characters, and the ten-entry jump table at `0x004B01F0` maps those
  classes to ten targets. Both tables were re-read programmatically by the Lead and match exactly; my own first eyeball read of the class
  table was wrong, which is why data gets decoded rather than read. The tables are the compiler's lowering of a switch and are recorded in
  the native's provenance, not claimed as separate data owners.
- **Build green**, 14,328,796 bytes, sha256 `0d3e21bd...`. `core/native_46db40.cpp` (Task_DefineAddAmmo, 15 calls / 11 distinct) is held
  out of the build: it calls `Sys_GetGlobalPointer_47CF10`, which has no implementation yet - a new gap, not a transcription error.

### 2026-10-25 - `0x004F1A80` takes six arguments, not five: a wrong store corrected

- **A worker's re-derivation corrected a claim I had written.** `docs/fidelity/evidence/type-id-slots-and-ordering-20261025.md`
  asserted that `Script_TaskContainer_Append` (`0x004F1A80`) reads only four arguments and that the constant `7` and a sixth
  pushed slot were dead. The body reads `[esp+0x20]` **twice**, with a `pop edi` between the reads, so the same textual offset
  addresses two different slots: `0x004F1AB1 mov bl, byte [esp+0x20]` is arg5 (byte) and `0x004F1AC9 mov esi, dword [esp+0x20]`
  is arg6. Both are stored - arg6 at record+`0x08`, byte(arg5) at record+`0x0C`.
- **Confirmed from the call sites as well as the callee**, which is what makes it independent: at `0x00509136` the sequence is
  `push $0x0; push $0x7; call 0x004F0F80` and the append cleans `add esp,0x18` - six words, with the two words pushed for the
  preceding call reused as arg5 (`7`) and arg6 (`0`). The cleanup size settles the arity without reading the callee.
- **The implementation was writing a wrong value**: it stored arg5 in both fields, so every append produced `{delta, type, 7, 7}`
  where retail produces `{delta, type, arg6, 7}`. Fixed in `script/native_4f1a80.cpp` (six parameters, correct stores) and in
  `core/native_508fa0.cpp` (all seven call sites pass the sixth word: `0`, except `0x00A7B2B8` in the fourth and `0x00A7B2A8`
  in the seventh). Build green, 14,324,250 bytes.
- **Remaining work, recorded rather than left implicit**: the other eleven caller files (107 call sites in the `0x0046xxxx-0x0048xxxx`
  lane) still pass five arguments and must be backfilled; each site also has to adjust its `add esp` by 4, and some share a deferred
  cleanup. That window is static-only - startup still aborts at the `0x00402930` registrar guard, so none of this executes yet.
- **Method rule recorded**: two reads of the same textual stack offset are not two reads of the same slot unless `esp` is unchanged
  between them. An argument-count claim must be made against a stack-state trace, not against a set of distinct offsets.

### 2026-10-25 - 35 more natives in the build, and a link closure that was measured rather than guessed

- **Intake: 35 new native files added to the build target**, taking the candidate from 14,283,882 to **14,324,250** bytes with a green link
  (`build-win32-no-verify-gate/igi1.exe`, sha256 `93be1011d1e915f4...`). All 42 untracked natives were first syntax-checked together
  (42/42 clean under `-Wall -Wextra`), checked for one-VA-per-file (clean), and checked for duplicate global definitions (121 defined globals
  across 39 files, **zero** collisions) before any of them was added.
- **The held-out set is an exact link closure, not a judgement call.** `nm` on every new file was used to compute the transitive closure of files
  that call an untranscribed callee: 8 files, blocked on exactly two retail addresses - `0x004AF9D0` (1190 bytes) and `0x0045E060` (858 bytes).
  They are excluded from the target with the reason recorded in `CMakeLists.txt`. Everything else links.
- **The dependency graph is deeper than it first looked**: `0x004AF9D0` -> `sys/native_4f1db0.cpp` -> `core/native_413df0.cpp`, which owns
  `g_5391ac[7]` and `g_56e228`, which six further files read. The recon that followed found `0x004AF9D0` is itself three functions
  (`0x004AFF90` 9 instructions -> `0x004AFFB0` 187 -> `0x004AF9D0` 329), so the keystone is a three-native chain, not one file.
- **A pattern became a test.** Id slots holding image value `0x0181` cannot live in `.bss`; they are `.data`. Verified on both sides: the image
  holds `0x0181` at `0x005402FC`/`0x005402FE`/`0x005401F8`/`0x0053FEC0`, and the candidate reports `D` for those slots against `B` for
  image-zero slots - including both classes in a single translation unit. The prediction is falsifiable in both directions and has held on
  three independent units across four clusters (`0x005402xx`, `0x005401xx`, `0x0053FExx`, `0x005385xx`).
- **An ABI question answered by measurement, not by widening a typedef.** `0x004108B0` reads its second argument at `[esp+8]` while the phase-1
  dispatcher pushes only one. Reading both dispatchers settled it: phase 0 (`0x004012C9`, table base `0xa96ae0`) pushes **two** arguments and
  null-checks the slot; phase 1 (`0x0040106E`, base `0xa970e0`) pushes **one** and does not. So `0x00401400`-linked handlers take two arguments
  and `0x00401530`-linked handlers take one, and the tree's `FrameHandler` typedef describes only the no-context path.
- **Method rule recorded**: an address-byte search finds absolute immediates only and is structurally blind to `call`/`jmp` sites, which are
  encoded as relative displacements. The `0x4D3710` population is 150 absolute references plus 13 relative calls; the two numbers answer
  different questions.
- **Manifest gap, recorded rather than papered over**: `scripts/build_native_records.py` requires a per-native Binary Ninja evidence JSON
  (`docs/fidelity/evidence/0x...-binaryninja.json`) and none exists for the r2-based natives, so the new files have **no manifest records yet**.
  The generator was not worked around and no records were hand-authored. The existing manifest change (the `_`-prefixed `candidate_symbol`
  correction for 23 records) is included.
- Nothing here advances a native past `BUILD_VERIFIED`; startup still fails closed at the `0x00402930` registrar guard.

### 2026-10-25 - Three more task classes, one with no event-0 link at all

- Linked: `0x0046B510` (ComputerButtonItem), `0x0046B7F0` (ComputerTextItem) and `0x0043E4F0` (GenericPhysicsMagicObj, filed under `dynobj/` because it
  is a dynamic-object class and that folder already hosts the other DynCube classes). `igi1.exe` 14,273,177 -> **14,274,624** bytes, green.
- **Verified independently rather than taken on report**: `0x0046B510` links the same handler `0x004D3710` twice, to event 0 and event 1. The lane
  flagged it as looking like its own copy-paste slip and checked twice; I read the image as well (`push 0x4d3710; push 0; push eax; call 0x401400` at
  `0x0046B529`, then `push 0x4d3710; push 1; push eax; call 0x401400` at `0x0046B542`). Retail really does register one handler for two events, and it
  is reproduced as-is.
- Observations recorded without promoting them to rules: the parent type comes from a different getter in every family (never hardcoded); the event id
  comes from a getter whose pushed argument the getter ignores, so it is neither constant nor always the same width; and `0x0043E4F0` is the first
  registrar with no event-0 link, so event numbers are read per file.
- Noted for one deliberate look later, not guessed at now: two of these id globals hold `0x00000181` (385) in the image (`0x00540B08`, `0x0053C46C`).
  If that value keeps recurring it is likely a retail build-time task-type constant.

### 2026-10-25 - Six task-class registrars linked, and the playable block observed at runtime

- Linked: `0x00472A70` (GunSPAS12), `0x004688B0` (ComputerLog), `0x00468E20` (ComputerObjectives), `0x004851B0` (GameScreen), `0x00446AB60`-family
  `0x0046AB60` (ComputerMenu, also registers the `COMPUTER:` -> `LOCAL:computer/` alias) and `0x00485A40` (StatusMessageScreen, object size 0x18440,
  the largest in the batch). `igi1.exe` 14,269,804 -> **14,273,177** bytes, green.
- **The "register-plus-LOCAL:" pattern was wrong as stated and is retracted by its author**: it had two witnesses, both script-command registrars, and
  `0x00472A70` broke it immediately (no `LOCAL:` dispatch, no `0x4B1420` call, no dword zeroing). The startup chain contains at least two registrar
  shapes, and the honest statement is only that `0x4B1420`-plus-`LOCAL:` identifies the script-command/alias registrars while `TaskType_Register`
  identifies the task classes. Handler addresses are shared by some task classes and not others, so nothing about them can be inferred from shape.
- **`0x004688B0` confirms in bytes** what was previously a structural negative: the Computer registrar links four handlers and calls no
  `Script_QHashNodeRegister`, so there is no `TASKTYPE_` name for it.
- **First runtime observation of the current candidate** (`docs/fidelity/evidence/candidate-startup-guard-20261025.md`): under CrossOver/Wine with
  `igi1.exe window level1` the candidate reaches the startup init sequence and terminates with the fail-closed `std::logic_error` for `0x00402930`.
  No window is created, so there is nothing to screenshot, and the 154 callees stay `BUILD_VERIFIED` with runtime unobserved - none of them was reached.
- Runner detail worth keeping: the candidate must be launched by absolute path under this CrossOver wrapper; a relative `igi1.exe` fails with
  `winewrapper.exe:error: cannot execute L"igi1.exe window level1"`.
- Recorded hazard: the guard's message hardcodes "155 ordered ... calls" and "146 of its 154 distinct callees", numbers that drift as registrars land.
  A stale count inside a diagnostic string is a claim that has become false, so the guard should derive or refresh them.

### 2026-10-25 - The AIGraph handler that was link-blocked on one dependency is linked

- `0x005093C0` (`AIGraph_Handler_5093C0`) added to the build target: `igi1.exe` 14,269,528 -> **14,269,804** bytes, green. Its whole undefined set
  (`Graph_Update_4F9180`, `List_FindIndex_4F1050`, `List_FirstFree_4F1240`, `List_Release_4F1090`) is now transcribed, so the block that kept this
  file out of the target is gone rather than worked around.
- The AIGraph phase handler still has one open dependency in the same wave (`0x004F9CD0`, which `0x5092A0` calls), so that file stays source-only
  until it closes. Linking order follows the dependency, not the file's readiness.

### 2026-10-25 - A partial transcription was sitting in the build target

- `engine/src/retail/script/native_4ee970.cpp` (`0x004EE970`, 3709 bytes) transcribes section 1 only and its provenance said it was "deliberately NOT wired
  into the build", while `CMakeLists.txt:439` lists it. The listing is the fact: the object compiles and links. Its comment now says so, and the manifest
  status is `TRANSCRIBED` rather than `BUILD_VERIFIED`, because BUILD_VERIFIED would imply a complete native. Recorded as an owner decision: keep the
  partial object in the target, or remove it until the body is finished.
- This surfaced from a mass operation and is worth naming as a class of risk: binding every linked native's record to its engine source in one pass is
  correct for 300+ files but it will happily stamp BUILD_VERIFIED onto a file whose own text says otherwise. The one file that disagreed was found by
  reading the statuses it wrote, not by trusting the pass.

### 2026-10-25 - 0x004AF960 has one implementation again, and the handler wave closes

- **The duplicate at `0x004AF960` is resolved, and the invented aliases went with it.** `mem/heap_list.cpp`'s `HeapList_Remove` and
  `mem/heap_list.hpp`'s declaration plus the `Camera_ApplyHandiCam_33` alias are deleted; the only implementation is
  `engine/src/retail/sys/native_4af960.cpp` (`Sys_UnlinkListNode_4AF960`, 33 bytes, node layout +0x00 forward / +0x04 back).
- All eight call sites were retargeted in the same change, because they were the reason the duplication stayed silent: the two symbols had
  different names, so both linked. Files touched: `core/native_48f4f0.cpp`, `core/native_4015d0.cpp`, `core/native_4f9df0.cpp` (whose BLOCKED
  note this removes), `script/native_4f0c90.cpp`, `mem/native_4b0d10.cpp`, `resource/sys_resource.cpp`, `terrain/terrain_draw.cpp`.
  Two of those sites only surfaced when the build failed, which is the argument for deleting the definition first and letting the compiler
  enumerate the callers rather than grepping for them.
- **Handler wave linked**: `0x004F1050`, `0x004FA850`, `0x004F90B0`, `0x005092E0`, `0x00509340`, `0x00509390`, `0x00401AE0`, `0x00401CF0`,
  `0x004F1030`, `0x004F1240`, `0x004F1090`, `0x004F9180`, `0x004F92E0`, `0x004F93B0`; `0x004FA850`'s host stack-protector defect fixed in source
  (array local replaced), `0x004B0C60` correctly routed to the existing `Mem_Alloc` instead of being transcribed twice.
- **Chain lane linked**: `0x004CEBA0` (model/MTP domain), `0x0047CAB0`, `0x00484F30`, `0x00402F50`. The last is not a registrar: it is the
  `YMBE.AFP`/CD presence check, and it carries two retail quirks reproduced rather than tidied - the A-to-Z loop never varies the relative path
  (26 identical retries, not a drive scan) and the `GetDriveTypeA` result is compared then discarded before any branch uses it.
- **Verification infrastructure**: the lane found it had been syntax-checking with host clang while the tree cross-compiles with
  `i686-w64-mingw32-g++`; all its files were re-checked under the cross-compiler, where `windows.h` and the real `__imp__GetDriveTypeA@4`
  import resolve. Records were regenerated for 89 linked natives so the implementation digests match the normalised sources again, and my
  per-native limitation notes were merged back rather than lost to the regeneration.
- `igi1.exe` 14,261,044 -> **14,269,528** bytes, green.

### 2026-10-25 - Provenance blocks conform to the gate's five-line rule, mechanically and verifiably

- `scripts/native_gate.py:check_provenance_comment` requires exactly five consecutive `//` lines starting at `// Native:`, with
  `Artifact:` / `Tools/evidence:` / `Behavior/CFG:` / `Status/limits:` at offsets 1-4. Wrapped block text from the last two days of work made
  blocks 7-10 lines long, so the gate reported missing labels even though the information was present. Format drift, not missing evidence.
- Added `scripts/normalize_provenance.py`: joins each labelled section back onto one line and **refuses** any rewrite whose comment content is
  not character-identical after removing whitespace and comment markers. 149 of 423 committed native files were non-conforming; all 149 rewritten,
  this direction only, no content invented or dropped.
- Added `scripts/sync_provenance_status.py`: copies the manifest status into the `Status/limits:` line, but only for files that are actually in the
  build target, because `BUILD_VERIFIED` means "the object is part of igi1.exe" and nothing else. 341 status lines corrected, 0 files falsely promoted.
- Build green after the pass: `[100%] Built target igi`, `igi1.exe` unchanged at 14,261,044 bytes (comment-only change, as expected).
- **Consequence recorded honestly:** rewriting 150 files invalidated the implementation digest each manifest record binds to, so those records now
  need regeneration before the gate can accept them. That regeneration is running and will land as its own commit; until it does, those records are
  stale rather than wrong, and the gate's "implementation changed since its evidence record was reviewed" is the correct response.

### 2026-10-25 - Two sys entrants, and a rule for the calls the compiler adds

- `igi1.exe` 14,260,556 -> **14,261,044** bytes, green. `0x004978D0 Sys_AddSlotName_4978D0` (one-argument forwarder, 14 B) and
  `0x004978F0 Sys_EnumRenderModesPair_4978F0` (38 B, passes callback `0x00497920` and an 8-byte local to `Sys_EnumRenderModes_4B0EE0`), both
  `BUILD_VERIFIED`, runtime UNOBSERVED.
- **New rule recorded in docs/fidelity/evidence/compiler-emitted-symbols.md**, from two independent sightings: a `call` can exist in an object with no
  counterpart in retail. Retail-side witness: `0x004A54A0` is `__chkstk`, compiler-emitted, must be CRT-classified and never transcribed.
  Candidate-side witness: a first version of `0x004978F0` used an array local, and taking its address made clang emit `___stack_chk_fail` /
  `___stack_chk_guard` into our own object. That is a source defect, not a build detail.
- Rule: every deliverable states its object's undefined-symbol set; every symbol must be either a transcribed native or a declared CRT routine, and
  anything else must be fixed in the source rather than linked away.
- Argument order for `0x004978F0` was resolved from the stack at the call (`[esp+4]` = callback, `[esp+8]` = local address), not from naming, since
  that class of mistake has already happened twice in this lane.

### 2026-10-25 - The 0x0053F316 byte gets an owner, and the CRT sweep confirms the category does not recur

- `igi1.exe` 14,260,288 -> **14,260,556** bytes, green. `0x00460C30 Core_GetGlobal8_460C30` (`mov al,[0x0053F316]; ret`) linked,
  `BUILD_VERIFIED`, runtime UNOBSERVED.
- **My earlier "0x0053F316 looks unread" note was wrong**, and the correction is recorded here rather than quietly dropped: an aligned reference
  search (`/r 0x53f316`) finds one writer (`0x0045E492`, untranscribed) and three readers (`0x0045E812`, `0x00460B5E`, `0x00460C30`). The byte block
  `0x0053F314`-`0x0053F317` therefore has four owners now, one per address.
- **CRT/compiler-emitted sweep result**: byte-exact fingerprints over the whole image found exactly one `__chkstk` (`0x4A54A0`), zero `__ftol2`, zero
  `__security_check_cookie`, zero `__EH_prolog3`, and four SEH-prolog matches (`0x4A50F5`, `0x4A5DEE`, `0x4AD225`, `0x4AD4D3`). Cross-checking the
  four by reference put three of them inside the CRT cluster only, and `0x4A50F5` had no references at all - so it is recorded as an uncertain
  fingerprint, not a finding. Conclusion: no game registrar calls anything compiler-emitted except the stack probe, so that misclassification does
  not recur in the named set.
- **Method error in the first sweep, recorded because it is instructive**: it inspected the direct callees of `0x00402930`, but the stack probe is
  called one level deeper, from the registrars. The first answer ("no CRT fingerprint among the missing callees") was true and worthless; the
  image-wide, reference-cross-checked sweep is the one that means anything.

### 2026-10-25 - The named blocker set is cleared: five more leaves and one owner/extern pair

- `igi1.exe` 14,258,968 -> **14,260,288** bytes, green. Five natives linked, all `BUILD_VERIFIED`, runtime UNOBSERVED:
  `0x00460BE0` (`mov ax,[0x005BDAF8]`), `0x00460C40` (`[0x0053F317]`), `0x00460C50` (`mov eax,[0x005BDC18]`), `0x0047E990` (`[0x00540914]`) and
  `0x00460C10` (the fourth lazy event-id allocator with the retail `0xFF` sentinel).
- Every body was spot-verified against the image before integration, not accepted from the report.
- `0x0047E990` is the ownership rule demonstrated from the other side: it reads the byte that the FlashBang registrar `0x0047E700` allocates and
  writes, so it declares that global `extern` and defines nothing. `nm` shows `undefined _g_540914` and no data symbol in the object.
- Caveat recorded for the block: `0x0053F316` has no reader among the transcribed bodies while `0x0053F314`, `0x0053F315` and `0x0053F317` each have
  exactly one owner - so if anything consumes that byte it is still unowned, and the gap is named rather than assumed empty.
- With this, the named leverage blockers from the earlier analysis are all either transcribed or reclassified as CRT (`0x004A54A0` = `__chkstk`).

### 2026-10-25 - Two config leaf bytes, and a blocker that turned out to be the compiler's stack probe

- `igi1.exe` 14,258,430 -> **14,258,968** bytes, green. `0x00413B30` and `0x00413B40` linked (`mov al,[0x005391AF]; ret` and `[0x005391B0]`),
  `BUILD_VERIFIED`, runtime UNOBSERVED. Their AL-only store versus C++ EAX zero-extension is recorded as a codegen deviation, not papered over.
- **`0x004A54A0` is MSVC `__chkstk`, not a game native.** Verified against the image (`0x004A54A0..0x004A54CE`), sitting between the known CRT routines
  `0x004A53B3` (sprintf) and `0x004A5500` (strncpy). It is now classified CRT in `scripts/native_coverage.py` and must never be hand-transcribed.
  The 7 registrars counted behind it are therefore free, but each has a frame larger than 0x1000 and owns a real >=4 KB local array that must be
  reproduced from the retail frame size rather than invented.
- **Tool blind spot recorded**: `build/retail-dis.txt` does not render calls to `0x4A54A0` at all, while `r2 /r` lists callers at 0x004170A5,
  0x0041AAF5, 0x0041B8E5, 0x0042DCF5, 0x004310F5, 0x0044C8D5, 0x0046E375 and 0x00485DB5. Callee counters that trust the listing cannot see it.
- Global naming normalised: `g_413b20` (named after its function) became `g_5391ae` (named after its data address, the convention used everywhere
  else), so the three-byte block 0x005391AE/AF/B0 now has one owner per address and one naming rule.

### 2026-10-25 - The three highest-leverage startup blockers

- `igi1.exe` 14,257,059 -> **14,258,430** bytes, green. Three blockers linked, all `BUILD_VERIFIED`, runtime UNOBSERVED:
  `0x00401FA0 TaskType_SetInheritedFlags_401FA0`, `0x00460BF0 Task_GetCachedEventId_460BF0`, `0x004E6610 Task_GetCachedEventId_4E6610`.
- Two of the three are the same 28-byte lazy event-id shape as `0x00467D90` (retail `0xFF` sentinel, one `QTask_AllocateEvent` path), which is why
  they were cheap despite blocking 8 and 9 registrars respectively: the leverage table measured how many callers a body has, not how hard it is.
- That is now three cached-event-id bodies in the tree with identical retail shape; the fourth, if one appears, should be transcribed by comparison
  against these rather than from scratch - but still verified per instruction.
- Each closure was checked by compiling the object and looking at `nm -u` rather than by reading the source, and every undefined symbol resolves in
  the tree (`QTask_TypeTable`, `QTask_AllocateEvent`).

### 2026-10-25 - FlashBang and M203 registrars, and the first registrar that passes a non-zero fourth argument

- `igi1.exe` 14,255,796 -> **14,257,059** bytes, green. `0x0047E700 Task_DefineFlashBang` (129 B / 1 block / 34 instructions) and
  `0x00476110 Task_DefineM203` (150 B / 1 block / 39 instructions) linked, both `BUILD_VERIFIED`, runtime UNOBSERVED.
- **Finding worth carrying to every later registrar**: FlashBang passes **4** as the fourth `TaskType_Register` argument, not 0. Verified in the
  image at `0x0047E711` (`push 4`) with the stack layout `name, 0x48, parent, 4, 0`; `0x00401900` tests that word and calls `0x004B0B00` when it
  is non-zero, so transcribing 0 there would change behaviour. Every other registrar landed so far passes 0.
- **Second structural negative recorded**: FlashBang has no `Script_QHashNodeRegister` call, so it receives no `TASKTYPE_` script name - the same
  shape as Computer. Two negatives now, so "every registrar registers a script name" is demonstrably false and must not be assumed.
- Both cleanups close exactly: FlashBang `add esp,0x40` = 16 dwords (2 register slots + 3 register arguments + 3*3 link arguments + 2 registry
  lookup arguments); M203 `add esp,0x38` = 14 and `add esp,0x18` = 6.
- Branch lists for the earlier branchy bodies were recorded with the branch condition behind each jump (`0x004169D0`'s 13 blocks per-block counts,
  `0x00467D90`'s single sentinel test), so those two are no longer "walked, probably fine" entries.

### 2026-10-25 - The 17-registrar unblocker, and two config tables that are not leaf getters

- `igi1.exe` 14,254,193 -> **14,255,796** bytes, green. Four natives linked, all `BUILD_VERIFIED`, runtime UNOBSERVED:
  `0x00467D90 Task_GetCachedEventId_467D90`, `0x00416930` / `0x004169D0 Sys_LookupOrAppendRecord_*`, `0x00416870 Sys_GetSettingByte_416870`.
- `0x00467D90` is 28 bytes / 3 blocks: load `0x0053F582`, and only if it is still retail's `0xFF` sentinel call `QTask_AllocateEvent(0x181)` and
  store the low byte back. Verified against the image directly, so the "blocks 17 registrars" entry is a cheap lazy initialiser, not a knot.
- `0x00416930` (table `0x0057B5E8`, stride `0x3C`, count `0x0057BAB4`) and `0x004169D0` (table `0x0057B1A8`, stride `0x30`, count `0x0057BAB8`) are
  13-block record lookups with an append path, not config leaves - both spot-verified against the image's own table/count references. The
  `0x00416xxx` range is therefore not uniform, and prior sizing assumptions there are not to be reused.
- **Declared deviation, recorded rather than hidden**: retail inlines `strlen` (`repne scasb`) and the record copy (`rep movsd` + `rep movsb`); the
  transcriptions call `std::strlen`/`std::memcpy` with byte count `strlen+1`. Behaviourally equivalent, not instruction-equivalent. The record is
  marked with that limitation, and neither table is given an invented bound because their extents are not statically established.
- Closure moved: `0x0047E700` and `0x00476110` are now closed (every callee transcribed), and the four registrars previously blocked only by the
  config range are unblocked.

### 2026-10-25 - GunShot and HumanCamera registrars, plus two self-corrections from the lane

- `igi1.exe` 14,252,981 -> **14,254,193** bytes, green. `0x0047B4B0 Task_DefineGunShot` ("GunShot", 0xC38, id 0x005BE3CE,
  script name `TASKTYPE_GUNSHOT`) and `0x004825C0 Task_DefineHumanCamera` ("HumanCamera", 0x128, id 0x00540990, plus a do-while
  clearing 128 eight-byte records at 0x005C1188). Both `BUILD_VERIFIED`, runtime UNOBSERVED.
- `g_5be3cc` (0x005BE3CC) was already defined in `sys/native_47b100.cpp`, so the new registrar declares it `extern` and adds no second
  definition - the single-writer rule held without a link error this time.
- **Tooling correction from the lane**: its batch closure scan parsed `pdfj` JSON, which emits call targets as `fcn.00416930` rather than
  `0x416930`, so the regex matched nothing and the scan reported "ABSENT: none" unconditionally. Re-checked with the `pd` text form,
  `0x47E700` and `0x476110` are **blocked**, not closed: they call `0x416930`/`0x4169D0`, which have no transcription anywhere.
- **Recount correction**: the doc's `0x4825C0` sizing (131 B / 3 blocks / 34 instructions) is right; the earlier 27-instruction count was the
  stop-at-first-`ret` artifact. Per-block counting (`af` then `afbj`) is now the only accepted method for multi-block bodies.
- Consequence for scheduling: `0x416930`/`0x4169D0` sit in the `0x00415xxx`-`0x00419xxx` range that has been reserved, so the two blocked
  registrars queue behind opening that range rather than behind the startup walk.

### 2026-10-25 - Two more task-type registrars, and a registrar that deliberately has no script name

- `igi1.exe` 14,251,830 -> **14,252,981** bytes, green. `0x0046DD00 Task_DefineGunx2` ("Gunx2", 0x138, id -> 0x0053FE68) and
  `0x0046BA60 Task_DefineComputer` ("Computer", 0x198, id -> 0x005BE2C4) are linked, both `BUILD_VERIFIED`, runtime UNOBSERVED.
- Both bodies are single-block and their `add esp` cleanups were used as an independent check on the call set: 0x38 = 14 pushes
  (2 shared + 3 register args + 3*3 link args), then 0xC three times (pre-pushed handler slot + 2 link args), then 0x14
  (handler slot + 2 link args + 2 script-registration args).
- **Structural finding, recorded so nobody "fixes" it later**: `0x0046BA60` has no `Script_QHashNodeRegister` call at all - "Computer" never
  receives a `TASKTYPE_` script name from this routine. `0x0046DD00` does register `"TASKTYPE_GUNX2"`. Symmetry is not evidence.
- The linkage lesson from `0x0046DF10` held on the next two files: their registrar declarations now copy the `extern "C"` linkage from the
  definitions, and `nm` shows `_Script_QHashNodeRegister`, not a mangled name.

### 2026-10-25 - Second startup wave: five registered natives and the knife-stab task type

- `igi1.exe` 14,249,775 -> **14,251,830** bytes, green. Six natives added to the link: the other lane's `0x00402820`, `0x00402870`,
  `0x00490370`, `0x004E6030`, `0x00495CC0` (stable across two checks before integration, one VA per file, provenance present, no duplicate VAs),
  and `0x0046DF10 Task_DefineKnifeStab` from the startup-chain worker.
- Integration fix worth recording: `0x0046DF10` declared `Script_QHashNodeRegister` without `extern "C"` while the transcription defines it
  `extern "C"`, so the link failed with `undefined reference to Script_QHashNodeRegister(void*, void*)`. Fixed at the declaration; the
  definition was correct. A companion-signature check at write time would have caught this before the shared build.
- `0x0046DF10` also shows why callee bodies beat call-site shapes: the `push 0x0046DF70` before a getter that ignores its stack is the
  pre-pushed third argument of the later `QTask_LinkQTaskEventHandler`, and both `add esp,0x14` cleanups independently measure five dwords.
- All six are `BUILD_VERIFIED` with `runtime UNOBSERVED` recorded; none of them is reachable from startup yet because `0x00402930` still aborts.

### 2026-10-25 - The DX gate is real now, and a paired runtime check caught a GUID bug static reading missed

- `0x00498480 Display_ProbeDirectXVersion` is wired into WinMain at its retail site (`0x0048F6E2`) with the retail `< 0x700` requirement path
  (`sprintf` 0x541E68, `MessageBoxA` "Error" `MB_ICONERROR`, `exit(1)`), and `0x00407D60 Material_DefineScriptNatives` is linked
  (`igi1.exe` 14,244,376 -> **14,249,775** bytes, green).
- **Paired runtime finding**: with the gate wired, the candidate showed the retail requirement dialog while retail `igi.exe`, run in the same CrossOver
  bottle with the same arguments, passed the gate and opened its window. Cause: our transcription of `IID_IDirectDraw2` (`0x00534A60`) and
  `IID_IDirectDrawSurface3` (`0x00534A40`) had `Data2`/`Data3` byte-swapped, so both `QueryInterface` calls failed and the tier stalled below 0x700.
  The image bytes are `... 43 2b cf 11 ..` and `... b2 69 d0 11 ..`, i.e. little-endian words `0x2B43`/`0x11CF` and `0x69B2`/`0x11D0`. Both corrected
  from the image, and the candidate now passes the gate and reaches `QTask_RegisterClasses` exactly like retail does.
- **This is the first divergence the runtime side found that the static side had already "verified"**: the probe's provider block claimed byte-exact
  GUIDs and 38/38 blocks, and it was still wrong on two GUIDs. Reading a GUID in a decompiler listing is not the same as decoding the image bytes in
  memory order; the evidence now says which of the two happened.
- `0x00498480` status is `RUNTIME_TESTED` with a documented scope (the tier outcome under CrossOver, paired with retail; COM internals not traced).
  `0x00407D60` is `BUILD_VERIFIED`/runtime UNOBSERVED - it is entry #3 of the `0x00402930` sequence.
- **Concurrent-writer incident, reported not hidden**: `CMakeLists.txt` gained five entries (`native_402870`, `native_402820`, `sys/native_490370`,
  `core/native_4e6030`, `render/native_495cc0`) from another agent's lane; two of those files do not compile (`Game_FrameDispatchRecord` and two
  `Native_*` declarations missing), which broke the shared build. The entries were removed from `CMakeLists.txt` (files left on disk, nothing deleted) so
  the build is green again; they belong to that lane's integration, not this one.

### 2026-10-25 - BackgroundFX wave linked (10 natives) and the 0x00A94E84 dispatch blocker closed

- `igi1.exe` 14,231,896 -> **14,244,376** bytes, build green with no undefined and no duplicate symbols. The wave adds `BackgroundFX_EmitAxisRuns_419330`
  (520 B/144 instrs), `EmitFan_419630` (290/90), `EmitSegments_419540` (230/77), `BuildAxes_419860`, `UpdateRecord_419820`, `EmitRecord_419950`,
  `Init_419760` (the class registrar itself), `ZeroField20_4197F0`, `AdvanceField20_419800` and `BuildRotation_4192E0`.
- The `0x00A94E84` render-backend dispatch that blocked this wave is now usable from transcribed code through the external-`extern "C"` table, with the retail
  form `g_a94e84[g_a84a54]`. The index word `0x00A84A54` is loaded as a defined zero dword rather than folded to a literal, so the memory access retail
  performs is still present.
- **Data increment transcribed**: `0x00539AF0`-`0x00539C17` as typed objects with real pointers - two 5-entry draw lists, the circle/axis-pair arrays, and
  the three draw records. A throwaway host check that *executed* the compiled initialisers against the image dump caught two wrong `g_539b50` entries
  (`{200,100,200,-100}`, `{0,-400,0,150}`) before they reached a commit. Reading the table was not enough; running it was.
- Recorded limits, not smoothed over: `0x00419630`'s x87 `fistpll` is modelled with `std::llrintl` and low-32-bit truncation, GCC folds `cos`/`sin` into one
  `_sincosl` import where retail has separate `fcos`/`fsin`, so **last-bit equality is not claimed**; and retail leaves one value on the x87 register stack on
  that function's `count > 0` path (unobservable through memory, return or ABI).
- Statuses are `BUILD_VERIFIED` with runtime UNOBSERVED: `0x00419760` is entry #153 of the `0x00402930` sequence, so the startup pass still cannot reach the
  wave even though it now executes in the binary.

### 2026-10-25 - 23 orphaned natives linked; two duplicate class-id owners fixed

- 23 engine sources that no build target contained are now compiled and linked (`igi1.exe` 14,222,853 -> **14,231,896** bytes, build green). They cover the
  `0x00491Dxx` render-handler set, the `0x00490xxx` and `0x004E7C50` render entries, the `0x004B6Cxx` model pair, `0x00418xx`-adjacent core entries, the
  `0x004EBC60` audio entry, `0x0051A1D0` dynobj and `0x005186C0` script. All 23 already had manifest records; status moves to `BUILD_VERIFIED` with runtime
  UNOBSERVED recorded per record.
- **Two duplicate owners found, because these files had never been linked together before:** `g_548638` (retail `0x00548638`, parent class id) was defined
  both by `core/native_4d9730.cpp` (`QObj_Init`, the writer) and by `render/native_004da1d0.cpp` (reader only); `g_54875c` (retail `0x0054875C`, DynCubeObj
  class id) was defined both by `dynobj/native_004e5370.cpp` (`DynCubeObj_Init`, the writer) and by `dynobj/native_0051a1d0.cpp` (reader only). The readers now
  declare `extern`, the writers keep the single definition, and the provenance text saying otherwise was corrected.
- This is exactly the failure mode "not in the build" hides: a duplicate definition is silent while nobody links the files, and it appears as a link error the
  moment they are integrated. It also means the earlier `0 of 52 duplicate-VA` result was about VAs, not about symbols.
- The remaining 29 orphans are `input/` (17 files, blocked on the missing half of `InputPort`) and the ten `native_419xxx` files the BackgroundFX wave is
  still writing.

### 2026-10-25 - Audit: 52 engine sources are not in the build

- `docs/fidelity/evidence/unbuilt-engine-sources.md`: 52 of 466 engine `.cpp` files are neither compiled nor linked (414 listed). An unbuilt native is not a
  transcription - it cannot be runtime-checked, it violates the no-dead-code rule, and it shrinks every "remaining work" count silently.
- **0 duplicate-VA files**, so no rule-13 violation: each unlisted file either carries a VA no linked file covers (49 files) or has no retail VA at all (3 port
  scaffolding files in `input/`).
- Integration test on 42 of them (all but the ten `native_419xxx` files the BackgroundFX wave is still writing): compiles clean, fails only at link with **10
  unresolved symbols** - `InputPort::Run`/`SetKeyDown`/`SetMouseButtonDown`/`AddMouseDelta`/`Remap`/`ActionPressed`/`ActionDown` plus `g_54875c`,
  `g_548638`. The backlog is close to linkable; the input subsystem is missing the other half of its own class, not an unknown dependency.
- Trigger for the audit: a worker reported `0x00498460` as "transcribed but unlinked". Checking the whole tree beat trusting one file's claim - the real number
  was 52, not one.

### 2026-10-25 - Startup sequence sized: 154 callees, 67,560 bytes in retail call order

- Added `docs/fidelity/evidence/startup-402930-callee-sizes.md`: every distinct callee of `0x00402930` in retail call order with its radare2 extent,
  block/instruction counts and whether an engine implementation exists. 154 callees, sizes recovered for all, **67,560 bytes total**; 36 are <= 200 B,
  101 are 201-800 B, 17 are > 800 B, and 8 have an engine implementation.
- The order matters more than the total: the sequence's **first** callee is the AI graph registrar `0x00508F80` (787 B / 198 instrs) and its first
  callee `0x005183D0` reports 4985 B over 90 instructions - a jump-table/data-bearing extent, so the AI graph is a subsystem wave, not a one-pass job.
  Because the guard stops at the first untranscribed registrar, nothing after `0x00508F80` becomes observable until that cluster lands.
- This replaces guessing about "277 small registrars" with measured buckets. Sizes are r2 extents and are planning data: a function whose tail r2 merges
  with a neighbour can read large.

### 2026-10-25 - Startup guard's own gap figure corrected: 146 absent callees, not 134

- Re-measured `0x00402930`'s callee closure against the tree rather than against the manifest's candidate paths: of **154 distinct callees, 8 have
  an engine implementation** (`0x00401FC0`, `0x00415530`, `0x00419760`, `0x00424B90`, `0x0042DAC0`, `0x0042FE40`, `0x00489560`, `0x004C1780`) and
  **146 do not**. The fail-closed guard text and the provenance block said 134, which understated the remaining work by a dozen natives; both now
  carry the measured figure and the old one is marked retracted.
- The same provenance line gave the function extent as `0x00402930..0x00402A40`. The real extent is `0x00402930..0x00402C3C` (780 bytes, 156
  five-byte instructions: 155 calls plus the tail `jmp 0x004CEF30` at `0x00402C37`), matching the ordered inventory in
  `docs/fidelity/evidence/startup-402930-call-sequence.md`.
- Why the numbers drifted: earlier counts used the manifest's `implementation.path`, which for many records points at an external `scripts/GameNatives`
  candidate rather than engine code. A candidate file is not a transcription. Measured here as "does an engine source implement this VA".

### 2026-10-25 - Render-backend table 0x00A94E84 gains external C linkage

- `engine/src/retail/dynobj/local_models_mef.cpp` defined the 16-entry backend init table as internal-linkage storage in an anonymous namespace, so a
  transcribed core native could not index it the way retail does (`call *0x00A94E84(,%reg,4)`). The definition keeps its single object and now has
  `extern "C"` linkage; `engine/include/igi/retail/dynobj/local_models_mef.hpp` declares it. No getter and no wrapper were added - the table is data
  retail itself exposes, and a helper would have been a non-retail abstraction.
- Build green; `igi1.exe` 14,222,891 -> 14,222,853 bytes (linkage/COMDAT difference only, no code change), and `nm` shows `B _g_a94e84` external.
  A same-named internal object in `texture/` remains internal and unaffected.

### 2026-10-25 - QVM evaluator cluster and its transcription set link into the candidate (27 natives, internal build)

- **27 natives listed in one batch** and linked: `igi1.exe` grows 14,206,798 -> 14,222,891 bytes, build green. This closes the QVM closure that had
  blocked the script layer since the accessor work started: the last three undefined symbols (`0x004B9120`, `0x004B8CE0`, `0x004B9660`) now have
  in-tree definitions, and the accessor trio (`0x004B8A20`, `0x004B8A50`, `0x004B8A80`) links with them. Piecemeal integration was never possible -
  tracked TUs already reference all three accessors, which a link test proved before this batch.
- **Two CRT routines identified instead of transcribed**: `0x005169B0` is statically linked MSVC `strncat` (291 B, leaf, zero callees and data
  references, one call site in the whole `.text`) and `0x004A5500` is MSVC `strncpy` (254 B, 35 blocks); both join `0x004A53B3` sprintf as CRT
  closure reductions with no implementation unit.
- **ABI correction from bytes**: `0x004B8CE0` takes three arguments, not four. `0x004B9040` pushes four words but the first is a retained stack word
  consumed by the following `strncpy` after retail's deliberately short `add $0xc`; the declaration, the call and the comment were corrected rather
  than silently changed.
- **Cross-file name mismatch fixed**: `native_4b8a80.cpp` still declared `0x004B8CE0` under its previous placeholder name and passed an untyped
  destination; the declaration now matches the definition, so the batch links instead of failing with one undefined symbol.
- Verification status for every native in the batch is `BUILD_VERIFIED` with `runtime UNOBSERVED` recorded in its limitations. The changed-native gate
  still blocks them, and correctly so: its behavioral runner reports `unsupported native address` for these VAs because no paired runtime observation
  exists - these functions execute from script and QTask dispatch that the candidate does not reach yet. A successful link is not behavioral evidence.

### 2026-10-25 - QTask_DeleteQTaskType (0x00401A20) transcribed and linked (internal build, no release)

- `engine/src/retail/core/native_401a20.cpp` (50 B, 15 instructions, 3 blocks, 3 edges) is listed in `CMakeLists.txt` and links into the candidate;
  `igi1.exe` grows 14,206,047 -> 14,206,798 bytes with the build green. It is `TaskType_Register`'s counterpart: masks the id, tests slot+0x24,
  frees through `0x004B0B70` when set, then clears slot+0x25 on both paths without clearing +0x24.
- Its two dependencies were already in the build - `QFile_ReleaseBlock_4B0B70` (`qfile/native_4b0b70.cpp`) and `QTask_TypeTable` - so the only new
  file in this chain is this one. No duplicate was created for the two callees that already existed.
- The manifest record `0x00401a20` previously pointed at the external `scripts/GameNatives` candidate and carried a `VERIFIED` status inherited from
  that catalog's "100% human-verified" claim. The record now names the engine transcription with its SHA-256 and status `BUILD_VERIFIED`; the old
  claim is preserved as a limitation that explicitly applies to that candidate, not to this transcription. Runtime remains UNOBSERVED.

### 2026-10-25 - automatic native intake and verification, repository cleanup (internal tooling, no release)

- Added one entry point, `scripts/native_autoverify.py`: changed/all/staged/commit-range discovery, a persistent sequential watch queue, source/build/probe/evidence invalidation, static BN/r2/angr capture, and the existing strict semantic/behavioral/paired-runtime checker. It never assigns native verification statuses or invents probes.
- Enabled staged native checks in the configured Git hook; added CMake changed/all verification targets and a self-hosted runtime CI workflow. Unknown addresses, incomplete staging, stale images/receipts, missing custom probes, unknown ABI or mismatching outputs/side effects block approval. CI is configured, not claimed to have run remotely.
- Validation: eight infrastructure tests, real SetConfigFileName static capture (runtime correctly blocked for missing reviewed probe registration), CMake target discovery, compatibility CLI checks and the gate-integrity check. Evidence: `docs/fidelity/evidence/native-auto-20261025/validation.json`.
- Audited the preserved demo image `9e7cdced...` against `game-engine-flow`: 50 engine-native records have matching historical source hashes; zero fresh paired runtime passes are available for that image. The reuse allowlist is empty. This is not a gameplay comparison or a zero-percent-correctness claim; no demo code was copied.
- Removed five byte-identical duplicate root documents, retaining canonical copies under `docs/`; consolidated two duplicated runner implementations into compatibility entry points. Updated references without deleting historical evidence. Removed 118 tracked generated Python cache files; source scripts and ignore rules remain.
- Fixed chronological discovery: game QTask addresses are no longer excluded as CRT; default walk begins at WinMain; the full 3,073-row direct-call order is retained. Function/disassembly inputs are now private and image/hash-bound instead of relying on shared `/tmp/afl.json`. Static direct-call order is not universal runtime coverage.


### 2026-10-25 - Startup init sequence measured (entry corrected after review; the guard was right)

- **Evidence added**: `docs/fidelity/evidence/startup-402930-call-sequence.md` - the ordered call inventory of `0x00402930..0x00402F50`.
- **Measured twice, agreeing**: 309 `call` instructions through `build/retail-dis.txt` and an independent
  `objdump -d --start-address=0x402930 --stop-address=0x402F50`; the region is a straight-line call sequence plus `nop` padding.
- **Correction (this entry was initially wrong)**: an earlier version of this entry claimed the candidate's guard ("155 ordered calls ending in a
  tail jump to 0x004CEF30") was inaccurate, based on 309 calls over 303 distinct callees. That measurement ran from `0x00402930` to the next
  *manifest* native `0x00402F50`, which spans **two functions**. `0x00402930` really does end at `0x00402C3B`, with the last call at `0x00402C32`
  to `0x00402F50` (`Config_YmbeAfp`) and the tail `jmp 0x004CEF30` at `0x00402C37` - exactly what the guard says. Independently
  `objdump --start-address=0x402930 --stop-address=0x402C3B` reports **155 calls**. The worker who flagged this was right and this entry is
  corrected rather than left standing.
- **Real closure for `0x00402930`**: 155 call sites, 154 distinct callees, 22 with an implementation file, **132 absent** - consistent with the
  guard's 134 within counting-method noise. The neighbour at `0x00402C40` is a separate, unrecorded 154-call sequence (150 distinct, 145 absent)
  with no manifest record, no work-order entry and no direct caller found by `axt`; its first calls are the small `0x004192B0`/`0x004199B0`
  routines that call `0x00401A20`, so it is plausibly a companion register/unregister pass - a hypothesis, not a finding.
- Counted strictly: a callee whose source exists but has no manifest record yet (the pending `0x42D*`/`0x42F*` handler files) counts as absent here.

### 2026-10-25 - TaskType_Register (0x00401900) moved to its own native and linked (internal build, no release)

- **Atomic move, as the no-duplication rule requires**: the `TaskType_Register` definition was removed from
  `engine/src/retail/core/task_types.cpp` (82 lines) and the existing transcription listed in `CMakeLists.txt` as
  `engine/src/retail/core/native_401900.cpp`. The build is green and `igi1.exe` links with the native in the target (14,206,047 bytes).
- **Linkage settled by experiment, not assumption**: the header declares the native inside its `extern "C"` block (line 74), so the
  definition keeps C linkage. Defining it in `namespace igi::retail::core` produced `undefined reference to TaskType_Register` from six
  call sites (cam_task, native_424b90, native_4d9730, native_004e5370, flow, game_flow) - evidence that the 279 callers bind the unmangled
  symbol. The first attempt was wrong for exactly that reason and is recorded here rather than hidden.
- **Manifest**: the `0x00401900` record now names `native_401900.cpp` with its SHA-256 and status `BUILD_VERIFIED`, limitations recorded
  (runtime UNOBSERVED; tail consumers `0x00401A20`/`0x004012F0` and the `0x004B0B70` free path still untranscribed).
- **Worker correction accepted**: my claim that `Debug_Error` (0x004AF7B0) and `Mem_AllocIndexedBlock` (0x004B0B00) had no engine
  implementation was wrong - they are `retail_error.cpp:29` and `mem/task_list.cpp:13`, both already in the igi source list. The worker was
  right not to duplicate them (rule 10.15). Stale "not transcribed yet" texts in `task_types.hpp` and `native_00530a80.cpp` were corrected.

### 2026-10-25 - QVM core dependency table; shared state words given external linkage (internal build, no release)

- **Evidence added**: `docs/fidelity/evidence/qvm-core-dependency-table.md` - per-VA sizes, r2 CFG, ABI, callers, callees and the wave ordering
  for the QVM closure, including the finding that the three evaluators (`0x004B9120`, `0x004B9660`, `0x004B8CE0`) are one mutually
  recursive cluster through `0x004B9040` with no acyclic order inside it.
- **Linkage defect fixed in `engine/src/retail/script/qvm_native_loader.cpp`**: `g_a965a0`, `g_a96598` and `g_a965a8` were defined inside the
  file's anonymous namespace, so the `extern` declarations in `qvm_execute.cpp` could never resolve - the CMake note calling these words
  "not ported" was wrong; they were ported but with internal linkage. Moved to namespace scope with the retail evidence for each word
  (writers/readers by VA) recorded at the definition. A duplicate definition in `qvm_execute.cpp` was never an option: it would have split
  the state that `Qvm_ReportError` and `Qvm_SetGlobalState` update.
- **Verified, not assumed**: `nm` on separately compiled objects shows the three words defined `D/D/B` in `qvm_native_loader.o` and referenced
  `U` in `qvm_execute.o`, so the pair now resolves; the full build stays green at 14,206,388 bytes. This fixes the linkage, **not** the
  evaluator transcription - `0x004B9120`/`0x004B9660`/`0x004B8CE0` remain unimplemented.

### 2026-10-25 - QVM argument accessors transcribed and measured; the script-layer wall mapped (internal build, no release)

- **Evidence added**: `docs/fidelity/evidence/qvm-accessors-4b8a20-4b8a50-4b8a80.md`. Three accessors measured on radare2 6.2.0
  `af`/`pd` against the retail image (all three 3 blocks / 2 branch edges, no prologue, plain `ret`):

  | VA | bytes | instructions | r2 nargs/frame | retail call sites |
  | --- | --- | --- | --- | --- |
  | `0x004B8A20` `Script_EvalArgInt` | 46 | 17 | 2 / 8 | 231 |
  | `0x004B8A50` `Qvm_GetFloatArg` | 44 | 17 | 2 / 8 | 201 |
  | `0x004B8A80` `Sys_ResolveSymbol_23` | 66 | 25 | 4 / 16 | 75 |

- **Correction recorded, not inherited**: for `0x004B8A20` and `0x004B8A50` the historical Binary Ninja/angr candidate disagrees with
  the retail CFG (5 blocks, splitting at the calls) while Binary Ninja and radare2 on the retail bytes agree on 3 blocks / 2 edges; the
  document states which parts agree (entry, flag at `+0x04`, scale-4 table at `+0x08`, the two x87 returns) and which do not.
- **The wall is now explicit**: each accessor is a two-branch wrapper whose work sits in untranscribed callees, so linking the three
  replaces three undefined symbols with six - `0x004BCAB0` (41 B, 14 instrs), `0x004B9120` (1188 B, 65 blocks, 484 instrs),
  `0x004BD010`, `0x004B9660`, `0x004BD040`, `0x004B8CE0`. `0x004BD010`/`0x004B9660` are already transcribed inside `qvm_execute.cpp`, but
  that file is the **unlinked** `igi_qvm_execute_object` target, gated on eight further callees plus three QVM state words.
- **Not committed, still untracked by design**: the three accessor `.cpp` files and the six handler `.cpp` files. None of the nine can join
  the `igi` link until that closure exists; the build stays green and the nine files are backed up under `build/pending/`.

### 2026-10-25 - QTask handler ABI settled and independently re-verified (internal build, no release)

- **Evidence added**: `docs/fidelity/evidence/qtask-handler-abi.md` (635 lines) settles the handler ABI from retail instructions: the
  second handler argument is a **caller-supplied pointer to caller-owned storage**, not a record field and not the QVM argument frame -
  size callbacks take `std::uint32_t*` out-params, parse callbacks take `void*` to a three-word `{block, argument frame, next index}`
  record that loader `0x004EDD20` builds at `E-0x18`. `FrameHandler` is valid as the table's **storage** type only; as a call signature
  it is proven incomplete (two-word dispatch sites carry integer second words).
- **Two structural claims re-checked by the Lead against `build/retail-dis.txt`, both exact**: the class registrator `0x00401900` copies
  the parent row with `0xFE` iterations and a `0x600`-byte stride (`lea 0xa96ae0(,%ebp,4)` destination, `lea 0xa96ae0(,%edi,4)` source,
  `add $0x600` on both, `dec %edx / jne`), which proves `entry(class, phase) = 0xA96AE0 + class*4 + phase*0x600`, i.e. the 0x180-entry
  phase stride this tree already uses; and the image references that base with exactly **791 refs across 13 folded phase bases**
  (588/119/15/1/21/2/13/5/7/5/4/7/4) - the census in the document is accurate, and the 725 dispatch sites split 457 two-word / 268 one-word.
- **Six handler natives written but deliberately NOT committed**: `0x0042DAC0`, `0x0042DB00`, `0x0042DCE0`, `0x0042FE40`, `0x0042FE80`,
  `0x0042FEF0`. They are syntax-clean and stable, but adding them to the build fails at link with three undefined callees
  (`0x004B8A20` 8x, `0x004B8A50` 6x, `0x004B8A80` 3x). Per the no-dead-code rule they stay out of the tree and out of CMake until those
  three natives land; a safety copy is at `build/pending/handler-set-20261025.tar.gz`. The three accessors are now the assigned unblock.

### 2026-10-25 - Wave A of the startup chain: Cursor registrar and its handlers (internal build, no release)

- **Integrated 4 new natives**, each with the five-line provenance block, one file per retail VA, CFG measured on both engines:
  | VA | native | size / CFG (Binary Ninja = radare2) |
  | --- | --- | --- |
  | `0x00424280` | `Task_WalkAndDispatchEvent_424280` | 117 B, 10 blocks / 14 edges |
  | `0x00424B90` | `Cursor_Init_424B90` (registrar called from `0x00402BC9`) | 121 B, 1 block / 0 edges |
  | `0x00424C10` | `Cursor_InitState_424C10` (phase-0 handler) | 47 B, 1 block / 0 edges |
  | `0x00424CA0` | `Cursor_WinProcMouseMove_424CA0` (WM_MOUSEMOVE handler) | 50 B, 4 blocks / 5 edges |
  Score: **1.000** on all four (node/edge Jaccard, Binary Ninja view vs radare2 `afbj`), status `STRUCTURALLY_VERIFIED`, build green.
- **ABI facts recorded, not papered over**: `0x00424C10` reads a second stack argument, so it belongs to the two-word phase-0 dispatch at
  `0x004012DE` (push context, push record, clean 8), not the one-word `0x00402481` site; `0x00424CA0` is a four-word window-procedure
  frame; `0x00424280` is one-word and `FrameHandler`-compatible. `FrameHandler` itself is unchanged.
- **Globals** `0x0057BC58`/`0x0057BC5C` (cursor x/y) and `0x0057BC60` (Cursor class id) had no owner in the tree; each is now defined
  exactly once, in the file of the native that owns it, as image zero-fill with the retail signedness.
- **Header correction**: `task_types.hpp` still called the link-native arguments `event_id, context`; the corrected bodies take
  `class_id` first and the event word second (`0x00401400` masks it to its low byte, `0x00401530` uses the full word), so the parameters,
  their documentation and the two natives now agree.
- **Still blocked, with the reason recorded**: `0x00424C40` needs `0x004B4E00` (absent from the manifest; it is a 317 B single-block leaf,
  the cheapest unblock), `0x00424CE0` needs `0x00491CF0`, and the Wave B registrars need `0x00416920`/`0x004528F0`/`0x00460BE0`/
  `0x004B3790`/`0x004CC2A0`/`0x00491610`/`0x004905A0`/`0x00490D10`/`0x004903B0`. `docs/fidelity/evidence/startup-chain-next-callees.md` §9
  carries the per-address blocker table.
- **Reachability unchanged**: the candidate still stops at the `0x00402930` guard, so these four natives are built and structurally
  verified but not yet reached at runtime; no runtime claim is made.

### 2026-10-25 - correction: the candidate "scene" capture was the desktop, not candidate output (internal build, no release)

- **Correction of the 2026-10-25 paired scene entry above**: the candidate image was not the candidate's output. Captured region
  `523,286 634x476` is **pixel-identical** to the same rectangle captured after the candidate process died (identical fraction 1.0,
  mean absolute difference 0.0), so the window contributed no pixels at all and the image shows the desktop behind it.
- **Why**: the candidate creates its window (`owner igi1.exe`, title `Innerloop`, `634x476` at `523,286`) and then terminates at the
  `0x00402930` guard within milliseconds - before DirectDraw/Direct3D initialisation and before any frame is painted. `screencapture -l`
  additionally reports `could not create image from window`, which is the same fact from the window server side: no composited surface
  exists because nothing was drawn.
- **Tooling gap and fix**: `matches_window` only compares the window's bounds with the image size - it proves the capture rectangle is
  the window, **not** that the window drew anything. `scripts/tester/game_scene_capture.py` now has `--verify-painted`: it re-captures the
  same rectangle after terminating the process and compares. Measured on this run: retail `painted: true` (`painted_fraction` 1.0),
  candidate `painted: false` (`painted_fraction` 0.0).
- **Artifact renamed** to `screenshots/pair-2026-10-25/candidate-level1-window-unpainted.png` so it cannot be mistaken for game evidence.
  It records window geometry only. No candidate render exists for Level 1 until the startup sequence runs.

### 2026-10-25 - paired level 1 scene capture, both games, window-scoped (internal build, no release)

- **Tool fix**: `scripts/tester/game_scene_capture.py` now supports an open title (`--title ''`), a minimum window area, and a
  configurable poll interval. The candidate's window appears for well under a second before the `0x00402930` guard terminates the
  process, so the default 200 ms poll missed it entirely; at a 10 ms poll the window is found and captured.
- **Retail capture**: `igi.exe window level1`, Escape sent to skip the intro, window `igi.exe` title `IGI`, id `11744`, pid `45132`,
  bounds `1024x668`, captured by CGWindowID at `2048x1336` (scale 2, `matches_window: true`) ->
  `screenshots/pair-2026-10-25/retail-level1-scene.png`.
- **Candidate capture**: `igi1.exe window level1`, window owner `igi1.exe`, **title `Innerloop`** (the retail `WNDCLASSA` class name the
  candidate transcribes), id `11763`, pid `46311`, bounds `634x476` at `523,286`, captured at `1268x952` (scale 2,
  `matches_window: true`) -> `screenshots/pair-2026-10-25/candidate-level1-scene.png`. The window capture fell back to a region capture
  of the window rectangle because `screencapture -l` cannot build an image for a wine Direct3D-backed window; the region is exactly the
  window's bounds, which is why `matches_window` is true.
- **Comparison**: the images are not comparable as renders - the candidate dies at the guard before it draws a frame, so its capture is a
  window shell, not Level 1. Different pixel content is expected and no equivalence is claimed. The capture tooling itself is verified on
  both programs: pid, window id, bounds, image size and scale agree for each.

### 2026-10-25 - reverse-engineering skills applied: retail registrar names mined (internal build, no release)

- **Selected skills** (recorded in `docs/agents/re-skills-applied.md`): `igi-auto-re-agent` for registrar push-pair mining,
  `igi-validation` for the dual-engine Gate/Ghidra+radare2 rule, `reverse-engineering` as the technique catalogue. Checked and
  recorded: radare2 present; **Ghidra headless MCP on `127.0.0.1:8771` is not answering**, so no dual-engine score can be claimed yet;
  `igi-validation`'s documented image path does not exist on this machine (this repo's authority image is the Downloads copy).
- **Added**: `scripts/registrar_pairs.py` - for every `call` into the four retail registrars (`0x4B8890`, `0x4BAAC0`, `0x4B88C0`,
  `0x4B88F0`) it back-decodes the preceding pushes (cdecl: the name string is pushed last, so the push nearest the call is the name),
  reads the C string from the image and cross-references the function with the manifest.
- **Result**: **167 call sites, 166 unique registered natives, 88 already implemented here, 78 missing** (19 call sites undecoded).
  Table: `docs/fidelity/evidence/registrar-pairs.md`. This is the only source of authoritative retail names for these routines:
  `Flow_RequestEvent` (`0x00402780`), `GOStart` (`0x00403660`), `Game_SetMission` (`0x00415890`), the `MenuManager_*` family,
  `MovieSubtitle_RegisterFont/RegisterText`, `Game_SetMusicVolume/SetSFXVolume`, and 70 more.
- **Structural finding**: none of the missing named natives is a direct callee of `0x00402930` and none appears in the call-order walk -
  they are script natives invoked indirectly by the QVM through the registry, so static call-graph work can never reach them; they have to
  be landed from the script/mission requirements. This is a gap the call-order list alone cannot show.

### 2026-10-25 - corrected the two QTask link natives from retail operands (internal build, no release)

- **Corrected (fidelity defect, found by `chain-worker`, fixed by Lead)**: `QTask_LinkQTaskEventHandler` (`0x00401400`) had its two
  argument roles swapped. Retail `0x00401408` loads the SECOND argument into `ebp` and takes the phase from its low byte, while
  `0x00401429` loads the FIRST argument and masks it to get the index base; `0x0040145F` compares the type record's first word against
  the FIRST argument and `0x00401474`-`0x0040147A` recurse with the SECOND. The tree body used the first argument for phase/recursion and
  the second for the comparison, which with the registrar call shape `(class id, event id, handler)` made every phase link collide on
  `class + (class & 0xff) * 0x180`. Both bodies now match retail; `0x00401530` also had its record comparison against the wrong argument,
  and retail does not low-byte-mask its stride argument - both corrected.
- **Re-measured**: `0x00401400` and `0x00401530` both rebuild green and score **CFG 1.000** (7 blocks / 10 edges each) against retail.
- **Why it matters**: the class registrars pushed by the startup sequence call these two natives; the wrong operand made the handler
  table land in the wrong phase rows, so the startup-chain transcription could not be validated against retail registration state.
- **Process**: `docs/agents/subagent-roles.md` records the owner's subagent split (Lead integrator, implementer, verifier/tester, research)
  with the mandatory "no self-verification" rule and the parallel-vs-sequential decision rule.

### 2026-10-25 - window-only scene capture: launch, load, Escape, capture (internal build, no release)

- **Fixed**: the earlier captures were full-screen grabs of the desktop (they showed the editor, not the game). New
  `scripts/tester/capture_game_window.py` enumerates windows by `CGWindowID`, raises the game window, captures it with
  `screencapture -l <window id>` only, and reports pid/window id/bounds/image size/scale so the image is provably that window
  (`matches_window: true`), with a region fallback when a window has no image backing.
- **Added**: `scripts/tester/game_scene_capture.py` - the owner's scene-check sequence in one command: launch
  `<exe> window level<N>` from the game directory through the CrossOver wine launcher, poll until the game window exists, wait the
  load time, bring it to the front, send Escape to skip the intro/cutscene, capture only the window, then terminate the process.
- **Verified (retail)**: `igi.exe window level1`, Escape sent (`CGEventPostToPid` + `osascript`), window `igi.exe` title `IGI`
  id `11715` pid `41085` bounds `1024x668` at `328,177`, captured `2048x1336` (scale 2, `matches_window: true`) ->
  `screenshots/pair-2026-10-25/retail-level1-scene.png`.
- **Candidate**: `igi1.exe window level1` through the same sequence produced **no window**: it still fails closed at
  `0x00402930` before a capturable game window exists. No candidate scene screenshot is claimed; T1 remains blocked on T0.

### 2026-10-25 - diagnostic logging + paired same-level capture (internal build, no release)

- **Added**: `engine/include/igi/diagnostic_log.hpp` - diagnostic-only logging infrastructure (owner rule 3(a)): never changes control
  flow, values or ordering, writes `$TMPDIR/igi1-diagnostic.log`, one line per call, flushed. First call site: the fail-closed guard in
  `class_registration.cpp`, which now logs `fail-closed: startup init sequence not transcribed, stopping at AI graph` before throwing.
  Verified live: the candidate ran under CrossOver wine and the log line was produced at
  `[0x00402930] fail-closed: startup init sequence not transcribed, stopping at AI graph`.
- **Captured**: same-level runs of retail `igi.exe` and candidate `igi1.exe` with `window level1`, screenshots by the macOS
  `screencapture` tool: `screenshots/pair-2026-10-25/retail-level1.png` and `screenshots/pair-2026-10-25/candidate-level1.png`.
- **Result**: retail ran (process alive at capture); the candidate reached the guard and terminated, so the candidate capture is not a
  level render. No release; T1 still blocked on T0.

### 2026-10-25 - T0/T1 start: flow order tool, dead-code and port-constant fixes (internal build, no release)

- **Task state**: T1 (3D objects + terrain visible) is **not** started because T0 blocks it: the candidate still fails closed at
  `QTask_RegisterClasses (0x00402930)`, which is the retail startup init sequence, not a registrar. Nothing in this entry is a release.
- **Added**: `scripts/flow_order.py` and `docs/fidelity/evidence/chronological-native-order.md` - the retail call flow from the PE entry
  point in call order: 3022 functions, 678 implemented, 2344 missing. This is the work order for not missing any method in the flow.
- **Added**: `docs/fidelity/MVP-task-breakdown.md` - the owner's small stacked tasks T0-T7 (startup chain, 3D objects, terrain,
  human player, weapons, AI, sound, menu) with the static + runtime + screenshot + changelog acceptance rule for each.
- **Fixed**: `flow.cpp` per-frame dispatch called a hardcoded `phase = 1U`; retail `0x00402462` calls `0x004E7530`, which is already
  transcribed as `Core_GetEventWord_4E7530`, so the dispatch now calls it. One port-only constant removed from the dispatch path.
- **Fixed**: removed the unused `link_handler` helper from `task_types.cpp` (dead code, rule 16); no native called it.
- **Launch attempt**: the candidate was copied to the game directory and started through the CrossOver wine launcher; no running game
  window and no 3D output were verified, and the capture `screenshots/internal/2026-10-25-igi1-t0-launch-attempt.png` is recorded as a
  launch attempt only, not as game evidence. No screenshot claim is made for T1.


### 2026-10-24 — startup-prefix internal build (no release)
- **State**: builds and starts, then terminates by design at `QTask_RegisterClasses`
  (`0x00402930`): the AI-graph registrar is not transcribed, so class ids after it
  would not match retail. Not runnable as a game, therefore **not a release**.
- **Achieved internally**: `WinMain` at retail `0x0048F530` as the only entry;
  startup prefix reproduced (mutex/duplicate path, `CoInitialize`, WNDCLASSA
  `"Innerloop"` style 3, 16 startup setters in retail order, four parser
  registrations, `CreateWindowExA` style `0x37C80000`, 640x480 at X=520/Y=285,
  `ShowCursor(0)`, `ShowWindow(10)`, `StoreWindowHandle`); B3 WinProc
  handler-chain container init (`0x48F674-0x48F6A6`) with the retail 12-byte
  `{next,+prev_link,+handler}` entry layout.
- **How**: matching decompilation of retail `dea34892…` with Binary Ninja (single
  decompiler), Radare2 CFG, angr exact entry; paired Frida prefix probes
  (retail `PREFIX_RETAIL_MATCH 178/179`; the chain side effect was the only real
  divergence and B3 now implements it).
- **Internal candidate**: `igi1.exe` sha256 `30c39039…` (deployed by
  `build_no_verify.sh`).
- **Evidence screenshot (internal, not a release)**: `screenshots/internal/2026-10-24-igi1-startup-prefix.png`.
- **Blocking next step**: transcribe the registrar chain starting at
  `0x00508F80` so `QTask_CreateTask` returns and the window stays alive.


## 2026-10-25 - handler/AIGraph closure linked; reachability audited

- **Change**: 23 natives added to the Windows x86 target: the 22-file handler/AIGraph
  closure (`0x004663D0`, `0x004F9CD0`, `0x004F9EB0`, `0x004FA4E0`-`0x004FAA10`) plus the
  surviving `0x004F9E70` implementation.
- **Build**: `build-win32-no-verify-gate/igi1.exe`, 14,283,882 bytes, sha256
  `8f0de1c9fe0448538613975cf44b6c4be89368c32c71f6d3f34cba2f4a14eeb6`. All 23 symbols
  resolve in the linked image; the deleted duplicate `Script_StringHash` is gone.
- **Defects found by compiling, not by reading**: 20 of the 22 files declared their callees
  outside `extern "C"` and so referenced C++-mangled names
  (`__Z29QFile_InvokeDriverSlot_4B1700Piiii`) against C-linkage definitions; 21 declared
  with `NATIVE_DECL` before that macro was defined; 20 included `<cstdint>` after first use.
  All 22 now compile clean with zero mangled undefined symbols.
- **Corrected in the same change**: `0x004B1700` now forwards `argument2`/`argument3`
  instead of `argument3`/`argument4` (retail `0x004B170C` loads arg2 and `0x004B1717`
  pushes it); its provenance line had claimed arg2 was never read, which was false. The 11
  AIGraph tag writers were shifted to `(context, record, length, 0)` with the length
  recovered from each retail push (`0x004FA503` push 2, `0x004FA786` push %ebx, `0x004FA7C9`
  push %edi, ...). `Sys_AddSlot_4B0E70` and `Sys_AddSlotName_4978D0` now return
  `std::uint32_t` (the slot index in EAX) instead of `void`.
- **Deletion**: `engine/src/retail/script/native_4f9e70.cpp` removed under rules 13/15/16 -
  duplicate VA of `engine/src/retail/core/native_4f9e70.cpp`, zero callers, wrong domain.
  The manifest record was rebound to the core file and keeps `Script_StringHash` as an
  alias.
- **Reachability audit** (`docs/fidelity/evidence/link-reachability-audit-20261025.md`):
  20 of 23 are statically reachable; 3 are linked but unreferenced anywhere in the image
  (`0x004F9EB0`, `0x004F9CD0`, `0x004663D0`), each with its retail caller recorded. A first
  pass that counted only direct `call`s wrongly reported 17 unreachable: the AIGraph writer
  dispatches through the jump table at `0x574ED4` with tail jumps, so calls alone are not a
  reachability measure.
- **Blocker identified**: `0x00402930`, the retail startup init sequence (155 ordered calls
  plus `jmp 0x004CEF30`), is implemented as a fail-closed stub that throws. Every
  transcribed registrar has **zero** references in the candidate image
  (`QTask_Register_4688B0`, `QTask_Register_46B510`, `QTask_Register_468E20`), so the
  candidate throws before any registrar runs and creates no window. Transcribing that
  sequence in retail order is the frontier for playable Level 1.
- **Status**: all 23 records at `BUILD_VERIFIED` with fresh source hashes, build evidence
  (`docs/fidelity/evidence/<addr>-build.json`), candidate address, call/tail/pointer
  counts, and the limitation that runtime is UNOBSERVED.
- **Autoverify**: `python3 scripts/native_autoverify.py run --scope changed --retry
  --max-attempts 3 --level 1` ran the 22-file scope before this change and returned
  `passed_count: 0`, every attempt blocked with `required source is absent from the build
  receipt` - correct behaviour for sources that were not yet in the target. Re-run after
  this change.

### Planned

- Follow the Level 1 playable-demo sequence in MVP.md and plan.md: Windows x86 build, OBJECTS.QVM model placement, textured terrain, complete visual verification, then playable simulation and gameplay systems including sound.

### Current implementation state

- The Windows x86 target now enters at the retail WinMain (`0x0048F530`) and reproduces the retail startup prefix; it still terminates at the documented registrar boundary (`QTask_RegisterClasses 0x402930`) and is therefore not a playable build.
- Resource, MTP, MEF container, and LOOP texture parsing have focused tests.
- Level 1 QVM object execution, 3D/terrain rendering, player and AI simulation, combat integration, and sound playback are not yet connected into a playable mission.
