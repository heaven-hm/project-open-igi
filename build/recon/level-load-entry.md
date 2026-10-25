# The game-flow entry: `0x00401050`, and the dispatch table at `0xA970E0`

Measured 2026-10-25. This is the first correct target for the level-load closure.

## Why `0x00401050` is the entry

**WinMain `0x0048F530` calls it LAST, at `0x0048F81B`.** WinMain has 32 call sites, all of
them the `0x0048F0xx`-`0x0048F3xx` startup helpers plus four `0x0048F360` calls and the
window block (`0x0048F770` CreateWindowExA, `0x0048F799` ShowWindow, `0x0048F7A0`
StoreWindowHandle). **After the window exists, WinMain's final act is
`call 0x00401050`** - so this is the game flow, entered once, with the window already up.

**And `0x00401000` - the function containing the guard call - is NOT the flow.** It is a
short init function that `ret`s at `0x00401042` after `call 0x00402930` (the guard),
`call 0x00402800`, and a `call 0x0048F220` whose result picks `push 4` or `push 2` into
`0x004028A0`. **It returns to its caller; it does not fall into anything.** The main flow
is a sibling at `0x00401050`.

## `0x00401050`'s body, all instructions read

```
0x00401050  push ebp / mov ebp,esp / push esi
0x00401054  push 0 / push 0x54F8E0 / call 0x004A5000      <- SEH prologue, see below
0x00401060  add esp,8
0x00401063  test eax,eax
0x00401065  jne 0x00401087                                 <- non-zero: clean exit
0x00401067  call 0x00402880                                <- the state getter
0x0040106c  xor ecx,ecx / mov cx,word [eax+0x1c]           <- state index is a 16-bit word
0x00401072  mov esi,dword [ecx*4 + 0xA970E0]               <- INDIRECT DISPATCH through a table
0x00401079  call 0x00402880                                <- the getter again, fresh value
0x0040107e  push eax
0x0040107f  call esi                                       <- the handler
0x00401081  add esp,4 / pop esi / pop ebp / ret
0x00401087  push 0 / call 0x0048F480 / add esp,4 / pop esi / pop ebp / ret
```

**So the game is a table-driven state dispatch:** get the state object, take the 16-bit
word at `+0x1C` as an index, load a code pointer from `0xA970E0[index]`, get the state
object **again**, and call the handler with it.

**The double `call 0x00402880` is not redundant and must be preserved.** The index is read
from the first result and the argument passed is the second result's. Whether they differ
is a runtime question; the retail instruction sequence re-reads, and a transcription that
reuses `eax` would be a different program.

## `0xA970E0` is populated at RUNTIME - the table is empty in the image

`pxw 64 @ 0xA970E0` returns **64 bytes of zero.** And the arithmetic confirms why:

    .data  VA 0x00536000, raw size 0x1A000  ->  file-backed end VA 0x00550000
    0xA970E0 > 0x00550000  ->  past the file-backed end

**So the table has no image value at all, and every handler in it is installed by code
that runs before `0x00401050` executes.** That makes "which handler loads level 1" a
question about **who writes `0xA970E0`**, and that writer is the next thing to find. **It
also means no static read of the table can answer the question** - the entries do not
exist until runtime.

## `0x004A5000` is SEH scaffolding, NOT a game function

Its first instructions store `ebp`, `ebx`, `edi`, `esi`, `esp`, the return address,
`0x56433230` and `fs:[0]` into a structure at `[esp+4]`, then `cmp eax,0xFFFFFFFF`. That
is a compiler-generated `__try`/`__except` frame prologue (`0x56433230` is the MSVC SEH
scope marker). **`0x00401050` is therefore wrapped in structured exception handling**, and
`0x004A5000` falls under the goal's compiler/CRT exclusion - it is not a game native and
must not be transcribed as one.

## Next step

**Find the writer of `0xA970E0`.** Search for the address as an immediate in `.text`; the
routine that stores code pointers into it is the registrar of the game states, and the
level-load handler is one of the entries it installs. **That routine is the real entry to
the level-load closure, and it is not in the guard** - consistent with flow-analyst's
result that the guard contains no terrain, model, texture or render loader.

---

## `0xA970E0` is the QTask handler dispatch table - and it has NO static writer

Measured: **23 references, and all 23 are `CALL` through the table.** Examples:
`call dword [ecx*4 + 0xa970e0]` at `0x4180BF`, `0x4180D6`, `0x4184A5`, `0x4184C3`,
`0x418777`, `0x418929`, `0x41BE1D`, `0x41CDC6`, `0x41EC91`, `0x41EC9F`, `0x41FA69`,
`0x41FA77`.

**The callers are concentrated in the `0x0041xxxx` band** - the QTask handler family - with
single sites at `0x004F46xx`, `0x004F2Axx` and `0x00474Fxx`. **Zero references are writes.**

**So the table is populated indirectly.** A `pxw` read gives 64 zero bytes, the address is
past the file-backed end of `.data`, and no `.text` instruction stores into it. **That
means the population happens through a computed address - a table copy, a registration
loop, or a `memset`/`memcpy` from another structure - and finding it needs a different
instrument than `/r`, not a different guess.**

**This is the structural picture, and it is coherent with everything measured today:**

    0x00401050  (WinMain's last call - the game flow)
      -> call 0x00402880, take word [eax+0x1c] as an index
      -> call dword [index*4 + 0xA970E0]      <- the QTask handler table
      -> call 0x00402880 again, pass that as the handler's argument

**The `0x0042xxxx` registrars measured by analyst and flow-analyst register QTask classes
and their handlers; `0xA970E0` is where those handlers are dispatched from; and the game
flow is a loop over it.** The level-load path therefore runs *through a task handler*, not
through the guard - which is exactly why the guard contains no terrain, model, texture or
render loader.

**Next instrument, not next guess:** the writer must be found by watching the table at
runtime or by finding the structure it is copied from. **A static search has now been run
and has returned zero writes; recording that as the result rather than concluding the
table is never populated.**
