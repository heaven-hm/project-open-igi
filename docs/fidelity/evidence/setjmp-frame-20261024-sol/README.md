# 0x004A5000 — frame save and matching restore evidence

The previous camera label and inferred seven-argument source signature were
incorrect. Retail 0x401054/056 pushes count 0 and buffer 0x54F8E0 before the call
at 0x40105B. The executable is the same retail SHA-256 recorded in `capture.json`.

Binary Ninja is the single selected decompiler. Its raw pseudocode incorrectly
treats entry EBP/ESI/EDI values as source parameters; disassembly establishes
that they are saved incoming machine state. Radare2 and Binary Ninja both recover
10 blocks/14 edges, similarity 1.0. Angr recovers the exact entry with an explicit
REP loop split (11 nodes), without unresolved jumps. No Ghidra analysis was used.

The candidate's compiled 123-byte body exactly matches retail, SHA-256
`c24e8aaed4de58907df11c265260404d68e8b92e1a6373d11e2db8e73238e755`.
Source-level original names are unavailable; `Native_004A5000` remains a placeholder.
The matching body is a catalog candidate, not linked gameplay acceptance.

## Live retail capture

`retail.json` and `retail.jsonl` retain two complete naturally reached count=0
calls in a 15-second `igi.exe window level1` observation. Each passed six checks:
EBP/EBX/EDI/ESI/ESP/return-EIP match the caller state, cookie is 0x56433230,
the unwind callback word is zero, saved scope matches the observed SEH record's
offset 0x0C, bytes 0x28..0x3F remain unchanged, and EAX is zero.

The probe hooks 0x401054 before both argument pushes and 0x401060 after return.
It leaves the CALL instruction and the frame-builder's return handling untouched;
an entry/onLeave hook could contaminate the saved return address. Both saved EIPs
were 0x401060. No pending calls, overflow, script failures or failed state checks
were observed. Only the owned game process was terminated, under the shared lease.

## Restore path

Radare2 finds exactly the buffer references at 0x401056 and 0x4010A2. The latter
belongs to 0x4010A0, which calls 0x4A507C at 0x4010A7. The retained Binary Ninja
restore disassembly shows 0x4A507C checking the same cookie, processing SEH/unwind,
restoring EBX/EDI/ESI/ESP and jumping through buffer+0x14, with zero coerced to a
nonzero result. This is direct static support that 0x54F8E0 is jump-buffer storage.

The restore native's database camera name is also erroneous. This research does
not add or accept a restore implementation. Its unwind callees and actual nonlocal
transfer need their own static/runtime packets. Nonzero count, sentinel-SEH and
nonlocal-resume paths remain unobserved. The candidate does not reach this frame
builder yet; no paired runtime acceptance or VERIFIED claim is made.
