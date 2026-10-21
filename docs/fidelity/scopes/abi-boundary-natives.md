# ABI-boundary native verification scope

This document records the ABI-boundary family. Each record must name its own non-empty `verified_observables` and `excluded_observables`, and pass a fresh retail-versus-candidate runner over at least eight cases for the named fields. The runner must execute both sides and bind its output to the current retail binary and implementation hashes. A missing runner or comparison keeps the record below `VERIFIED_WITH_DOCUMENTED_SCOPE`.

`Mem_AllocChunk`, `Mem_AllocWithRetry`, and the fallback paths of `Mem_AllocateRawBlock` now have fresh x86 angr comparisons under [allocator-default-heap.md](allocator-default-heap.md). Their scoped statuses do not cover custom-pool allocations, a non-null new-handler, or absolute host allocation addresses. `HeapList_Insert` now has a direct PE32 retail-vs-candidate comparison covering the x86 list splice. Four ABI-boundary entries remain inconclusive.

| Address | Native |
| --- | --- |
| `0x004AA670` | `Mem_AllocChunk` |
| `0x004AA682` | `Mem_AllocWithRetry` |
| `0x004AA6AE` | `Mem_AllocateRawBlock` |
| `0x004B0C60` | `Mem_Alloc` |
| `0x004B23B0` | `Texture_UploadMipmapPacket` |
| `0x004B5F80` | `Resource_RegisterBuffer` |
| `0x004B7CF0` | `Texture_UploadPalettePacket` |

## Per-native blockers and required scoped evidence

The following entries remain below scoped verification. Rows for the three allocator natives above and `HeapList_Insert` are removed here because their stated observable sets have been tested and are bound to their evidence.

| Address | Known issue beyond an incomparable pointer/result | Minimum behavior a future runner must compare |
| --- | --- | --- |
| `0x004B0C60` | Adds input normalization/guards and changes allocation size and header/back-pointer geometry on 64-bit. | Define and test the valid input domain; compare allocation requests, alignment, relative header fields, initialization/list effects, and failure accounting. |
| `0x004B23B0` | 64-bit packet size differs; guards can skip callbacks that retail invokes unconditionally. | Compare packet slot values/order, both callback invocations and ordering, and return pass-through. Exclude only callback return value and absolute pointer identity. |
| `0x004B5F80` | Can skip retail's unconditional setup callback and uses host-adapted node/list layouts. The return is an alias of the input and is comparable as such. | Compare setup callback, branch kind, loaded flags, child count/names/sizes/relative payload offsets, list relations, and return alias. |
| `0x004B7CF0` | 64-bit packet size differs; callback guard changes unconditional dispatch. Stored BN/R2 artifacts lack semantic detail, and the canonical GameNatives CFG score is below threshold. | Refresh semantic/CFG evidence first; then compare packet fields, callback index/invocation, and return pass-through. Exclude only callback return value and absolute pointer identity. |

## Verified observables

The per-address table identifies candidate observables. A native's record may name only fields its fresh runner actually compares after the listed blockers are resolved or the tested preconditions are explicitly narrowed. Static review and build evidence are also required, but cannot substitute for the scoped behavioral comparisons.

## Excluded observables

Exact host allocation addresses, CRT heap layout, allocator metadata, and host callback return values are excluded. These values differ by process, architecture, allocator, and graphics backend, so equality with the retail process is not a meaningful comparison.

## Why a live retail differential is unavailable

The retail functions cross the CRT heap or graphics/resource callback boundaries. Host addresses and host callback return values are process-specific, and ESIL cannot emulate the retail CRT heap sufficiently to compare allocation identities. This makes those particular values unavailable to the oracle; it does not establish that any other behavior matches. The excluded values must be omitted from the runner's compared fields, while each claimed verified observable still needs successful case-by-case comparison against retail execution.

## Revalidation trigger

Replace this scope with a live retail-versus-candidate comparison if IGI-Probe can observe and normalize the boundary values, if a compatible retail heap model becomes executable, or if the host ABI, allocator, resource backend, or callback contract changes.
