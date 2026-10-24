# 0x004A7EFF: corrected catalog candidate, recorded-input replay only

Work began 2026-10-24; the successful replay completed 2026-10-25. This is a
dependency of formatter 0x004A7720, which WinMain's DirectX error path reaches
through 0x004A53B3. It is not a camera transform. Original source identifiers
remain unknown, so the candidate keeps the provisional name Native_004A7EFF.

The retail body has exactly 13 bytes and one basic block:

```asm
mov eax, [esp+4]
add dword [eax], 4
mov eax, [eax]
mov eax, [eax-4]
ret
```

The argument addresses a cursor cell. The function advances that cell by four
bytes BEFORE reading the preceding DWORD and returns the raw bits in EAX. It
does not validate pointers or call another routine. ADD flags and this ordering
are preserved by explicit assembly; no compiler epilogue or UD2 is added. The
old generated C++ used invalid pointer typing. All five prior catalog variants
are archived with paths/hashes in prior-catalog.json; one canonical CPP and HPP
replace them. No engine source, production entry point or root build list is
changed by this increment.

Static capture uses Binary Ninja 6.0.10601 as the only decompiler, Radare2 6.2.0
and exact angr CFGFast recovery. BN/r2 both recover one block, no edges, score
1.0. Complete retail body bytes are inside the 16-byte BN sample and agree with
Radare2 and the selected PE. The live loaded candidate body is also identical:
`8b4424048300048b008b40fcc3`, SHA-256
`639148f2f162bda27c1f66ab90d68794bac59a533c7cbf65580a2c6853ef771f`.
Decompiler names and its int32_t* annotation are not original-source evidence.

## Actual runtime test and its limits

Retail ran first with window level1 and cwd Downloads/IGI. Every admitted call
in each 15-second window was retained. The first successful capture contained
27,447 entries and returns, 2,145 distinct input words, including 90 zero words
and two words with the high bit set. The final repeat, with the current
provenance-bearing header and driver, contains 16,441 complete calls. Both
successful runs are retained; the earlier one is under successful-first-27447/.
Different loading progress in a finite window is not a full-game parity claim. The
window closed with zero pending calls; no overflow, script failures or missing
returns occurred. The first owned process was confirmed gone before the second.

The candidate was built from the exact source/hash now in the canonical catalog
as an import-free x86 DLL with entry point zero. In a second retail host, the
same probe loaded that DLL, verified its actual export's 13 bytes, and directly
invoked it on owned memory for ALL recorded inputs in order in each run. Batches are
transport boundaries only; inputs are not sampled, deduplicated or truncated.
Return bits, cursor advance of four and unchanged source word match on every
call. comparison.json retains the diagnostic result, explicitly verified=false.
Both owned processes were confirmed gone. The actual loaded module/base/export
are in the candidate hook and byte-and-scope-summary.json.

The earlier comparison's candidate_va field recorded the preferred link address.
successful-first-27447/address-binding-correction.json records the actual runtime
export address and preserves that report. The final report separates preferred
and actual runtime addresses. The checker compares typed
values and pointer relationships, never those cross-image code addresses.

This is NOT a natural reconstructed-game call, and the replay thread is Frida's
RPC thread. Each recorded input is marshalled into fresh owned storage. Cross-call
cursor-chain composition, overlapping storage, invalid pointers, concurrency,
the complete formatter and game startup remain unverified. No production
runtime_contract or native intake is approved; status stays BUILD_VERIFIED.
The strict changed-native gate is retained and blocks acceptance.
Its current header scan also treats the provenance-bearing declaration as an
implementation and reports a path mismatch against the canonical CPP. That is
recorded rather than hidden or "fixed" by adding a second record for the VA.

## Reproducibility and failed attempts

The full reports, input sequence and raw message streams use deterministic gzip.
compressed-retention.json lists their uncompressed SHA-256 as well as the gzip
file SHA-256. Read with Python gzip.open or decompress before using the archived
capture script. Source/probe/driver snapshots retain the EXACT versions used by
the successful run; later metadata corrections do not rewrite that history.

Earlier attempts remain under build/runtime-traces/format-arg-reader-*:
capacity overflow at 4,096 calls; a closure-boundary probe error; whole-candidate
suspended setup timeouts; and DLL setup timeouts with a large input array in one
RPC. All were rejected. After bounded case streaming, the complete replay passed.
Owned PID journals, timeout cleanup and stage logging prevent silently leaving
failed captures active. The diagnostic early-resume flag explicitly marks
earlier startup unobserved and never changes the default startup capture mode.

A normal new-candidate launch reached the explicit 0x00402930 startup exception;
the UCRT imports were therefore not demonstrated to cause the setup timeouts.
No CRT compatibility fix is claimed. Frida was restored and provider health was
checked separately; RE-Agent CLI/module checks still fail and are not hidden.

To reproduce this diagnostic (never production acceptance):

```sh
env PYTHONDONTWRITEBYTECODE=1 '/Users/haseeb-mir/Library/Application Support/pipx/venvs/frida-tools/bin/python' scripts/tester/capture_format_arg_pair.py --dll-replay --out build/runtime-traces/format-arg-reader-new-unique-run
```

The captured compilation source was staged under build/native-analysis before
publication. Its content SHA-256 equals the canonical catalog CPP exactly. Full
formatter linkage and paired natural candidate-game observations are still needed.
