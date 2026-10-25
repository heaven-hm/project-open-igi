# The package-record sweep: `0x004CF050` and `0x004CED50`

Measured against retail `igi.exe` SHA-256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`, PE32 i386 image base `0x00400000`, `.text` VA `0x00401000` raw `0x1000`. Instrument: capstone 5.0.7 reachability decode from the verified entry (fallthrough plus every branch target, no fallthrough past a `ret`), instructions counted by distinct address, blocks/edges by splitting at every branch target and after every branch.

## 1. The two units

| target | extent | bytes | instructions | blocks | edges | `ret` | `90` nops after | file |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| `0x004CF050` | `0x004CF050`–`0x004CF072` | 35 | 13 | 5 | 8 | `0x004CF072` | 13 | `dynobj/native_4cf050.cpp` |
| `0x004CED50` | `0x004CED50`–`0x004CEE26` | 215 | 84 | 5 | 6 | `0x004CEE26` | 9 | `dynobj/native_4ced50.cpp` |

Both `ret` boundaries and both padding runs were read byte-by-byte from the image; both padding runs are entirely `0x90`. Call census from the image's `0xE8` enumeration: `0x004CF050` has **two** call sites (`0x004CF037` in `native_4cefa0.cpp`, `0x004CFB1F`); `0x004CED50` also has **two** (`0x004CED18`, and `0x004CF05E` in `native_4cf050.cpp`) — the second site of each was previously unknown to the tree's notes.

## 2. Three things preserved on the Lead's instruction

1. **The dead guard.** `0x004CF056`–`0x004CF05B` is `lea 0x14(%esi),%eax; test %eax,%eax; je 0x004CF066`. The tested value is a computed address inside a table based at `0x00A443A0`, so it can never be null, the branch is never taken, and every record reaches `0x004CED50`. The **source keeps the branch**; `-O2` proves it non-null and folds it, which is a codegen difference with no behavioral effect. Reported as measured rather than claimed as preserved in the object code.
2. **The second call site** of `0x004CF050` at `0x004CFB1F` is recorded in the provenance block.
3. **The bound is the comparison, not the count.** Retail writes `cmp $0x00A4EC60,%esi; jl 0x004CF056`, so the transcription writes the cursor comparison `record < 0x00A4EC60` with the `0x48` stride. The 600-record figure is derived from that bound (`(0x00A4EC60 - 0x00A443A0) / 0x48 = 0xA8C0 / 0x48 = 600`) and is recorded as a consequence, never as the loop's control.

## 3. The frame of `0x004CED50` aliases, and that forced the model

`0x004CED50` runs `sub $0x24,%esp` and then uses three slots:

| slot | retail reference | role |
| --- | --- | --- |
| `frame+0x00` | `[esp+0x10]` (and `[esp+0x18]` at `0x004CED93`, while two call arguments are still pushed) | the loaded count |
| `frame+0x04` | `[esp+0x14]` | the name buffer copied from `record+0x14` |
| `frame+0x08` | `[esp+0x18]` | the haystack passed to `0x004A5BB0` at `0x004CED89` |

The haystack is **four bytes into the name buffer**, and the last-character access at `0x004CEDF9` (`mov %dl,0x13(%esp,%ecx,1)`) lands **one byte below** the buffer when the name is empty. The transcription therefore models the frame as one `std::uint8_t frame[0x24]` and takes interior pointers, so both overlaps reproduce; two separate C++ objects could not. The same offset also explains a detail that otherwise looks like two variables: the counter is initialized as `mov %ebp,0x18(%esp)` but read in the loop as `0x10(%esp)` — one slot, addressed eight bytes apart because the two call arguments are still on the stack at the initialization.

`0x004A5BB0` was read (not written — it belongs to another writer) to establish that it is a **read-only substring search**: it takes its second argument's first byte and scans its first argument for it, returning a pointer into the haystack or null, and never writes through either pointer. The call's result is discarded and retail never reads that slot again; the call is nevertheless made because retail makes it.

## 4. What the generated code confirms

Compiled with `-O2`, both functions keep retail's shape. `0x004CF050`: base `10765216` (`0x00A443A0`), `addl $72` (`0x48`), `cmpl $10808416` (`0x00A4EC60`) with the backward branch. `0x004CED50`: `movl $0,36(%esi)` (`record+0x24 = 0`), the `repne scasb` scan and `rep movsd`/`rep movsb` copy modelled as loops, the haystack at exactly `name buffer + 4`, the counter zeroed after the search call, `movl $100`/`imull`/`idivl` for `100/(index*index)`, the scale word at `record+0x3C+2*index`, the loaded pointer at `record+4*(index-1)`, the byte increment read-modify-written at `name_buffer+length-1` (and at `name_buffer-1` for an empty name), `cmpl $6` for the five iterations, and both exits writing the low word of the count to `record+0x3C` **through the original argument** rather than through the advanced slot pointer — which is why retail re-reads `[esp+0x38]` at `0x004CEE02`/`0x004CEE12`.

**Codegen note:** `-O2` recognizes the explicit byte loops as `strlen`/`memcpy` and emits those library calls, which retail does not make (retail has the inline `repne scasb` and `rep movsd`/`rep movsb`). The source models retail's loops; the calls are the compiler's idiom recognition and are behaviorally identical (same length computed, same `length+1` bytes copied, source and destination disjoint). Recorded rather than hidden, and consistent with the existing `native_4bd220.cpp`, which models retail's `rep stosd`/`rep stosb` the same way.

## 5. Fail-closed status

| file | compile | link check | undefined in tree |
| --- | --- | --- | --- |
| `dynobj/native_4cf050.cpp` | `rc=0`, zero warnings | **`rc=1` NOT LINKABLE** | `DynObj_LoadRecordVariants_4CED50` — and nothing else |
| `dynobj/native_4ced50.cpp` | `rc=0`, zero warnings | **`rc=1` NOT LINKABLE** | `DynObj_ParseConfigFile_13`, `Str_FindSubstring_4A5BB0` — and nothing else |

Neither callee was stubbed. **Name coordination, because a mismatch looks like a missing implementation:** `0x004CEE30` is declared with its **manifest** name (`DynObj_ParseConfigFile_13`), so that VA's implementation must define exactly that symbol. `0x004A5BB0` has **no manifest record** and is declared with an address-suffixed placeholder derived from measured behavior (`Str_FindSubstring_4A5BB0`), so that name must be reconciled with the writer's file or the link will fail on two spellings of one routine.

## 6. Open questions

- `0x004CED50`'s placeholder name `DynObj_LoadRecordVariants_4CED50` records only the measured behavior (five successive name variants loaded into one record, with a `100/(i*i)` scale word per slot). It is a placeholder while the native is unverified.
- Why retail searches `name+4` for `"johan"` and discards the result is unexplained; recorded, not guessed.
- `0x004CED50`'s other caller at `0x004CED18` has not been attributed to a native.
