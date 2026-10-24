# Tail-path note: `0x00402C37` jump and the remaining registrar calls

## Answer

The jump at `0x00402C37` is an unconditional tail call from `Flow_RunHandler`
(`0x00402930`) to `DynObj_LinkDynamicCube_13` (`0x004CEF30`). It is not
conditional on startup arguments, level number, or a flag. The remaining
registrar calls in the linear disassembly belong to a **different function**,
`Flow_DeleteHandler` (`0x00402C40`), reached only through a QTask event-dispatch
path (`QTask_DispatchEvent` `0x004010B0`).

Therefore the remaining positions are **dead for the level-1 startup
QTask_RegisterClasses sequence**, but they are not dead in the binary: they run
when a QTask event is dispatched through `0x004010B0`. Do not transcribe them as
startup positions; transcribe them only when implementing that event path.

## Bytes and function boundaries (Binary Ninja)

| Address | Evidence |
| --- | --- |
| `0x00402930` | BN: `Flow_RunHandler`, range `0x402930-0x402C3B`, one basic block. |
| `0x00402C32` | `e8 19 03 00 00` — `call 0x00402F50` (`Config_YmbeAfp`). |
| `0x00402C37` | `e9 f4 c2 0c 00` — `jmp 0x004CEF30`, unconditional; the function's only outgoing edge. |
| `0x004CEF30` | BN: `DynObj_LinkDynamicCube_13`, 11 bytes: `a1 ec ed a4 00` (`mov eax,[0xA4EDEC]`), `a3 f0 ed a4 00` (`mov [0xA4EDF0],eax`), `c3` (`ret`). Its only callsite is `0x00402C37`. |
| `0x00402C40` | BN: `Flow_DeleteHandler`, range `0x402C40-0x402F46`, auto-discovered false; one incoming edge: `jmp 0x00402C40` at `0x004010C7`. |
| `0x004010B0` | BN: `QTask_DispatchEvent`; ends with the `jmp 0x00402C40`. Callers: `QTask_CloseEvent` (`0x004010A0`, fallthrough at `0x004010AF`) and `Input_GetDIErrStr` (`0x0048F970`, the retail window procedure). |

`Flow_RunHandler` has a single basic block, so no branch selects between the
tail jump and the `0x00402C40` code. `Config_YmbeAfp` is called immediately
before the jump; its failure path loops at `0x00402FE4`, but if it returns, the
jump is still taken. No startup argument can make `Flow_RunHandler` fall into
`Flow_DeleteHandler`.

## Runtime observation

The spawn-suspended level-1 retail trace (`build/tester/20261024T030541Z/registration-baseline/runtime/retail-registration.json`) contains
15,025 registrar entries over the 20 s observation. **Zero** of them have a
return address in the `Flow_DeleteHandler` range `0x00402C40-0x00402F46`; the
executed QTask_RegisterClasses order ends at position 153 (`0x00402F50`) and its
last registration is class id 278. The three `0x004C1780` static occurrences are
single-byte-ret positions and register nothing.

## Practical rule

- Level-1 startup oracle: compare only the 153 executed positions in
  `docs/fidelity/evidence/retail-registration-per-registrar.json`.
- `Flow_DeleteHandler` (`0x00402C40`) is a separate event-path function and is
  outside the startup oracle. Its calls are not unreachable garbage; they are
  simply not part of `Flow_RunHandler`'s execution on any argument combination.
- If the event path is later in scope, capture it with its own event/message
  scenario; do not append those registrations to the startup class-id range
  without observing the ids that path actually returns.
