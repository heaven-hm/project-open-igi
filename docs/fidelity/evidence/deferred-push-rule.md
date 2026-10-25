# The deferred-push rule

**Independently discovered twice, on two different bodies, by two different agents. It is now a
project rule, not a per-file observation.**

## The rule

**A push that no adjacent call consumes is not necessarily dead. Check the next callee's argument
reads before discarding it.** When a routine defers all cleanup to a single `add esp,N` at the
end, arguments pushed for earlier calls are still live on the stack and can be read as later
calls' arguments.

Two failure directions, and both were hit:

- **Counting pushes at one call site misses an argument.** At `0x0044995C` only `push eax`
  precedes `call 0x4c17c0`, yet the callee reads two arguments: arg2 is the `push esi` from
  `0x00449955`, still live because the single `add esp,0x10` at `0x0044996B` defers all cleanup.
- **Calling a push dead when it is deferred.** In `0x52CA70`, pushes at `0x52CA8F`/`0x52CAA9` are
  the handler arguments `0x52CAD0`/`0x52CAF0`, pushed *before* an intermediate getter call purely
  by instruction scheduling. `0x401900` reads four argument slots, so its fourth is the first of
  two earlier pushes, still holding 0.

## How to apply it

Derive the slot **from the callee prologues**, not from the push count. Measured reads:

| callee | argument slots read (after its own pushes) |
|---|---|
| `0x00401530` | `[esp+0xC]`, `[esp+0x14]`, `[esp+0x18]` |
| `0x00401900` | `[esp+0xC]`, `[esp+8]`, `[esp+0x18]` |
| `0x004C17C0` | `[esp+8]` = arg1, `[esp+0x10]` = arg2 |

## The arithmetic error to avoid

The running `esp` delta is what to trust, **and it must be restored after each `call` returns.**
Carrying a callee's `esp` forward past its return is what made `0x00449950` look like a
one-argument call, and that mistake was then written up as an "undecidable gap" - see
`0x00449950-arg-count-gap.md`, which is now resolved.

## Consequence for the port's signatures

The port's signature arity is the **callee's** arity, and ignored pushes are omitted. But when an
ignored push is still on the stack and **is** the next callee's argument, it must be passed
explicitly. That is the difference between "dead" and "deferred".
