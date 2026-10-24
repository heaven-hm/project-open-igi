# Retail getter xref note: 0x4F0F80 / 0x4F1000 / 0x4F1A60 / 0x4F1A70

Asked: confirm from the retail image, via xref callers only, that these four
getters are never called before QTask returns, so they can be recorded as
"not startup-path".

Result: **cannot be confirmed; the xref evidence says the opposite.**

Each getter has direct callers that are themselves QTask_RegisterClasses
registrar positions, and those positions executed in the retail registration
trace (`build/tester/20261024T030541Z/registration-baseline`):

| getter | example direct caller (BN) | QTask position in trace | callsite |
| --- | --- | --- | --- |
| `0x4F0F80` | `AmbientArea_Init` `0x44C130` | 77 | `0x44C249`, unconditional |
| `0x4F1000` | `WeaponConfig_Init` `0x407290` | 26 | `0x407B26`, unconditional |
| `0x4F1A60` | `WeaponConfig_Init` `0x407290` | 26 | `0x4072F3`, unconditional |
| `0x4F1A70` | `WeaponConfig_Init` `0x407290` | 26 | `0x4072D9`, unconditional |

These registrar bodies are invoked by `Flow_RunHandler` (`0x00402930`) during
level-1 class registration; the trace recorded all 153 executed positions and
279 class ids. Static callsites inside the bodies are straightforward
`call` instructions (e.g. `0x44C240: ... 0x44C249: call 0x4F0F80`), not in an
alternate branch after a return. Therefore the getters are **reachable on the
retail startup registration path**, even though the prefix boundary does not
observe them (it stops at the QTask_CreateTask call and the candidate throws
before executing any registrar body).

Honest record: "unobserved at the main-prefix boundary; reachable from
QTask_RegisterClasses registrar Init positions per direct xrefs and the retail
registration trace." Do not record "never called before QTask returns" or
"not startup-path" without a targeted retail getter hook proving each callsite
is not taken.

A targeted retail probe hooking the four getter entries (spawn-suspended) would
settle actual call counts/values; it was not run here because the requested
proof was xref-only and the current run was candidate-boundary work.
