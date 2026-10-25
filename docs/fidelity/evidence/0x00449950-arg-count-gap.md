# `0x00449950` passes TWO arguments to `0x004C17C0` - RESOLVED

**Status: resolved. This file previously recorded a gap that was not a gap.**

## The resolution

`0x004C17C0` takes two arguments, and this call site supplies both. Arg2 is the `push esi` at
`0x00449955`, which is still on the stack because the single `add esp,0x10` at `0x0044996B`
**defers all cleanup to the end of the routine**. Push-counting immediately before the call sees
only `push eax` and therefore cannot see arg2 at all.

Entering `0x004C17C0`, esp is **E-16** (E = esp on entry to `0x00449950`):

| step | esp | note |
|---|---|---|
| entry `0x00449950` | E | [E+4] = arg1 |
| `push esi` | E-4 | `[esp+8]` = E+4 = arg1 -> esi = object |
| `push esi` @`0x00449955` | E-8 | pushes object |
| `call 0x4168d0` | E-12 | retaddr; **on return esp is back to E-8** |
| `push eax` @`0x0044995B` | E-12 | pushes the result |
| `call 0x4c17c0` | **E-16** | callee entry |

Inside `0x004C17C0`: `push esi` -> E-20 so `[esp+8]` = [E-12] = **eax** = arg1 (the list);
`push edi` -> E-24 so `[esp+0x10]` = [E-8] = **object** = arg2 (the value).

So the call is `QTaskList_Remove(Sub_4168D0(object), object)` - get a list from the object, then
remove the object from it. The callee's body confirms the shape: `mov esi,[esp+8]` is the list
(`mov ecx,[esi]` = count, `lea edx,[esi+8]` = elements) and `mov edi,[esp+0x10]` is the value
compared against each element.

Fixed in `native_449950.cpp`, declared from the callee's own definition in `qtask_list.cpp`
(`_QTaskList_Remove`, unmangled, so the C linkage matches).

## How the earlier gap claim was wrong

The first pass carried esp = **E-12** forward *past the return of `call 0x4168d0`*, instead of
restoring it to E-8. That put the second argument slot at [E-12] and made it look like the return
address of that call - i.e. like stale stack, and therefore like a retail quirk that could not be
resolved without guessing.

**It was fully decidable. The arithmetic was simply wrong.** A wrong answer that is recorded as
"undecidable, deliberately left open" is worse than a plainly wrong answer, because the framing of
diligence discourages anyone from re-deriving it. flow-analyst re-derived it and was right.

## The rule

**The running esp delta is the thing to trust, and it must be restored after each `call` returns.**
Push-counting at a single site is not sufficient when a routine defers its cleanup to one
`add esp,N` at the end - the arguments of earlier calls are still live on the stack and can be
read as later arguments.
