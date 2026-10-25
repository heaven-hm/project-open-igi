# Retail script handler call ABI

This packet follows the GOInRemap observations in `../remap-abi-20261025/`.
It is retail-only dependency research, not acceptance of an engine native.

All 81 observed GOInRemap invocations returned to retail VA **0x004BD901**,
inside the function at **0x004BD880**. Their caller-owned 24-byte output buffer
was at handler-entry ESP+64. All six ABI/table checks in the previous probe
also passed. The retail executable/configuration hashes stayed unchanged;
owned game/server cleanup succeeded. See `observation.json` for artifact hashes.

Fresh Binary Ninja and Radare2 recover 12 blocks with CFG similarity 1.0;
angr recovers the exact entry and reports no unresolved jumps. This does not
resolve every possible indirect-call target or establish source equivalence.
BN's existing name `Sys_CloseFileStream_29` is a misleading placeholder; the
observed code dispatches a script handler. No engine symbol was renamed.

## Machine interface at 0x004BD8FE

The call uses `[registration + 0x10]` as its target. In push order:

1. 0x004BD8F3 pushes a pointer to the locally constructed argument frame.
2. 0x004BD8F8 pushes the word loaded from `[registration + 0x18]`.
3. 0x004BD8F9 pushes the local output-buffer pointer.

Thus the handler sees output destination at entry ESP+4, registration payload at
ESP+8 and argument frame at ESP+12. The caller cleans 12 bytes at 0x004BD90F.
EAX supplies the returned buffer pointer; 0x004BD901–0x004BD912 copy six dwords
from it to the caller's local output storage. The GOInRemap captures establish
that EAX aliases its supplied destination. A hidden structure-return parameter
is a plausible source-level explanation, not a recovered original declaration.

The frame construction at 0x004BD8C6–0x004BD8EC establishes these offsets:

| Offset | Written value |
|---|---|
| +0x00 | word read at current bytecode position +1 |
| +0x04 | 1 |
| +0x08 | 0 |
| +0x0C | current VM/context pointer in EBX |
| +0x10 | current bytecode position +5 |

These observations do not support the candidate `QvmFrameView` interpreting
+0x04 as an argument pointer or directly indexing +0x08 for this path. The
retail reader at 0x004B8A20 selects its indexed-reader branch when +0x04 is
nonzero. Preserve that callee dependency and its actual frame interpretation.

## Returned storage consumers

After the handler, the caller checks its context error word at +0x20. On the
no-error path it restores context fields, advances the bytecode position and
branches on `[registration + 4]`:

| Registered value | Observed instruction use of output buffer |
|---|---|
| 3 | Loads word +0x10 and passes it to 0x004BCE40 with context and value 3 |
| 2 | Loads a qword at +0x08 as x87 floating point, converts to single precision, calls 0x004BDA30 |
| other | Loads word +0x00 and passes it to 0x004BDA00 with context |

No instruction in this caller's shown result-selection paths reads output
words +0x04 or +0x14 independently, although the six-dword copy includes them.
Their copied stack contents must not be reclassified as fixed zeros. Original
type names, the meaning of registration payload +0x18 and the complete
result-consumer semantics remain unresolved.

## Next implementation dependency

Recover 0x004BCAB0's indexed frame interpretation and the selected handler's
registration/candidate mapping before changing GOInRemap. Keep the call-site
ABI, 24-byte result storage and runtime comparison contract explicit. The current
engine configuration decoder's simplified frame and one-argument handler call
are incompatible with this observed path. None of these findings authorizes
copying arbitrary stack residue constants into C++ or accepting the whole VM.
