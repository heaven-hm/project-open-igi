# Startup formatting closure: rejected source acceptance

WinMain's DirectX <0x700 path calls 0x004A53B3 at 0x0048F702 before showing the
requirement message and invoking 0x004A5AD5. That requires the actual formatting
native, not an unrelated library implementation that shares its declaration.

Fresh retail captures for 0x004A53B3 and sibling 0x004A5702 are retained in the
address subdirectories. Binary Ninja is the selected decompiler, independently
checked by Radare2 and angr; no Ghidra analysis was invoked. Each has four retail
blocks, BN/r2 similarity 1.0 and exact angr recovery. Retail identity is the
project's dea34892... executable; full SHA-256 is inside each capture.

The 82-byte variadic entry constructs a 32-byte local stream, sets its buffer
and current pointer, sets flags 0x42 and counter 0x7FFFFFFF, passes the variadic
stack address to 0x004A7720, then decrements the stream counter. A nonnegative
counter writes the terminating zero through the updated current pointer; a
negative counter calls 0x004A760B with character zero and the stream address.
It returns the saved formatting result. The 81-byte va_list sibling performs
the same sequence with its third argument as the variadic pointer.

The Binary Ninja database includes a historical user comment describing
0x004A53B3 as Game_DataSymbolLoad and interpreting 0x42 as a per-name limit. The
instructions do not establish that claim. The wrapper pushes its second argument
as the formatting string, and 0x42 is stored in the stream flags word. Such user
annotations and decompiler parameter guesses are retained raw, never accepted
as original names or semantics. The sibling's inherited "bounded" annotation
likewise does not establish a destination bound supplied by the caller.

The current candidate header imports `::sprintf`, and its String_VFormat body
calls `std::vsprintf`; neither preserves the retail stream/callee flow. The local
counter in the latter remains 0x7FFFFFFF through the library call, so its later
decrement cannot represent updates made by the retail formatting engine. This
does not claim a measured output mismatch for all ordinary formats: it rejects
matching-decompilation acceptance of this source.

`prior-records.json` preserves the manifest before retraction. The sprintf record
claimed VERIFIED despite its own no-paired-runtime limitation and a missing
semantic artifact. The va_list sibling's cited review has a different hash and
talks about ILFF forms/chunk traversal instead of formatting. It also declares
no_side_effects despite destination-buffer writes. That raw artifact is preserved
here; its runtime contract is not authorized. The replacement semantic reports
explicitly fail and bind the current source hash. Both statuses are ANALYZED,
and their previous review claims cannot be used by the strict gate.

Another active agent committed the initial status withdrawals in 20c6a85e while
this audit was in progress. This packet preserves those edits and supplies the
referenced evidence; it does not overwrite that agent's input-code changes.

Unresolved implementation closure: actual 0x004A7720 formatter and 0x004A760B
stream flush path, their data/callee dependencies, focused typed buffer-output
comparisons and paired runtime against a candidate containing those bodies.
The 0x004A5AD5 exit wrapper and its callee 0x004A5AF7 require their own evidence.
No replacement native is added or accepted here; no engine source is changed.
