# Default heap allocator verification scope

This scope applies separately to `Mem_AllocChunk` (`0x004AA670`),
`Mem_AllocWithRetry` (`0x004AA682`), and `Mem_AllocateRawBlock` (`0x004AA6AE`).
The exact native entries from retail `igi.exe` and freshly compiled PE32 x86
candidate functions were executed with angr's VEX engine. Heap APIs were
replaced by deterministic boundary procedures that record request sizes and
return the same synthetic pointer or null on both sides.

The application-backed scope is the default heap configuration: allocator
mode 0, null CRT new-handler, and the retail default heap handle. The source
tree initializes these globals to those defaults and has no production writes
that select custom-pool modes or install a new-handler. `Mem_AllocateRawBlock`
also received fallback-path cases for modes 1, 2, and 3; mode 2 cases set the
bucket limit to zero and mode 3 cases use sizes above the small-pool threshold.
Those cases do not exercise custom-pool allocation itself.

The differential compared the returned null/non-null class and every requested
allocation size. Absolute allocation addresses and CRT/OS allocator metadata
are process-specific and are excluded. The test does not treat an allocator
name or return-address equality as proof of matching behavior.

## Verified observables

- `return_class`: `null` or `allocated` for each identical x86 input.
- `requested_allocation_sizes`: ordered byte-size requests sent to the modeled
  retail `HeapAlloc` boundary and candidate `malloc` boundary.
- At least eight case-specific comparisons were made per native, including
  zero, alignment boundaries, normal sizes, allocation failure, and the
  `Mem_AllocWithRetry` upper-size guard where applicable.

## Excluded observables

- Absolute pointer identity, CRT heap metadata, and the host allocator's
  internal state.
- `Mem_AllocateRawBlock` custom-pool allocations in modes 2 and 3.
- Retry behavior after a non-null new-handler callback; the production scope
  has a null handler.
- End-to-end resource registration, intrusive heap-list layout, and the
  renderer callback results owned by other natives.

## Why a live retail differential is unavailable

This host cannot launch the retail Windows x86 game process. The comparison
executes the actual retail function bytes and the x86 candidate under angr,
with only the platform heap APIs replaced at their ABI boundary. This is a
binary-emulation differential, not a live game session or an emulator proof
of CRT internals.

## Revalidation trigger

Re-run this comparison and review the scope if allocator mode or heap globals
gain production writes, a non-null new-handler is introduced, either
implementation changes, the target ABI changes, or IGI-Probe/live retail
execution becomes available.
