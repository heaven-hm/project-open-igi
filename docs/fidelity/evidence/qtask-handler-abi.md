# QTask handler ABI: the physics-object class callbacks at 0x0042DAC0-0x0042FEFB

Artifact: retail `igi.exe`, SHA-256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`, image base `0x00400000`. Every `0x005...`/`0x004...`
address below is a retail VA unless it is a source path. Disassembly: `build/retail-dis.txt` (GNU objdump, i386 PE); instruction streams were
re-read against that listing, and data (the phase bytes at `0x0054DC30`, the `DefinePhysicsObjType` name at `0x0054DC34`) was read from the PE image.

Scope: the handler table at `0x00A96AE0` (`QTask_HandlerTable`, indexed `phase * 0x180 + class`, see
`engine/include/igi/retail/core/task_types.hpp` and `engine/src/retail/core/task_types.cpp`), the second-argument question, and the six routines
transcribed with this document:

| file | symbol (placeholder) | retail VA |
| --- | --- | --- |
| `engine/src/retail/core/native_42dac0.cpp` | `QTask_LinkClassHandlersA_42DAC0` | `0x0042DAC0` (links the "Car" class callbacks) |
| `engine/src/retail/core/native_42db00.cpp` | `QTask_HandlerA_42DB00` | `0x0042DB00` (Car parse callback) |
| `engine/src/retail/core/native_42dce0.cpp` | `QTask_HandlerB_42DCE0` | `0x0042DCE0` (Car size callback) |
| `engine/src/retail/core/native_42fe40.cpp` | `QTask_LinkClassHandlersB_42FE40` | `0x0042FE40` (links the "Wheel" class callbacks) |
| `engine/src/retail/core/native_42fe80.cpp` | `QTask_HandlerC_42FE80` | `0x0042FE80` (Wheel parse callback) |
| `engine/src/retail/core/native_42fef0.cpp` | `QTask_HandlerD_42FEF0` | `0x0042FEF0` (Wheel size callback) |

The class names "Car" and "Wheel" are read off the strings at `0x0053BE28` (`Car` class name passed to `0x00401900` by `0x0042DCF0`) and `0x0053BE4C`
(`Wheel`, passed by `0x0042FF00`). The class *ids* are dynamic: `0x00401900` returns them into `0x0053BDA8` / `0x0057BD30`, so every site reads the id
through `0x0042FDC0` / `0x00430950` instead of using a constant.

## 1. Answer

**The second handler argument is a caller-supplied pointer to caller-owned storage. It is neither a field of the definition record nor the QVM
argument frame, and it has no single meaning for the whole table: the callback's role decides what the pointer addresses.**

| callback role | arg1 | arg2 | decisive evidence |
| --- | --- | --- | --- |
| size callback (`0x0042DCE0`, `0x0042FEF0`; also `0x00427F80` and `0x004F43D0`) | never read | `std::uint32_t*`, an out-parameter for the class block size. The handler stores the size through it and leaves the same pointer in EAX. | body is `mov 0x8(%esp),%eax; movl $size,(%eax); ret`; the loader passes its own third argument slot as arg2 and then feeds that slot to `Mem_Alloc` (`0x004EDF09`, `0x004EDF0B`, `0x004EDF12`) |
| parse callback (`0x0042DB00`, `0x0042FE80`; base implementation `0x004F43B0`) | only forwarded to the base class (Car) or never read (Wheel) | `void*` to a three-word context record `{block, QVM argument frame, next argument index}` that the loader `0x004EDD20` builds at `E-0x18` in its own frame | loader `0x004EDF1D`/`0x004EDF40`/`0x004EDF44` fill the record, `0x004EDF38`-`0x004EDF48` pass its address; Car forwards the same pointer; the base parse handler passes `block` and `&context[2]` to `0x005031F0` |

Consequences for the port:

- The second argument is **not** "a pointer to the definition record shifted by the class size" and **not** the QVM argument frame: the frame is
  a separate word *inside* the record that arg2 points at (`context+0x4`), and the block pointer is `context+0x0`. The loader holds the definition
  record in `EBX` while it calls the size callback and still passes `&context`, not a field of that record.
- The block pointer is *also* stored in the definition record the loader is building (`record+0x2D4` at `0x004EDF17`), but the callback receives
  it through the context record, not through arg1 and not by computing anything from arg2.
- arg1 is not established as anything more specific than "one 32-bit word". At the two loader call sites it is the literal `0`; `0x0042DB00`
  forwards it unchanged to the base class callback; the base class callback and all four size callbacks never read it.

## 2. The loader `0x004EDD20` and its two dispatch sites

`0x004EDD20` is the QVM native registered as `DefinePhysicsObjType`: `0x004EDCE0` pushes the inline name string `"DefinePhysicsObjType"` at
`0x0054DC34` and calls `Script_QHashNodeRegisterEx` (`0x004B8890`, transcribed at `engine/src/retail/script/native_4b8890.cpp`) with
`(name, 0x004EDD20, 0, -1)`. The same routine allocates the three private
phase ids first (`QTask_AllocateEvent` `0x004017C0` three times, results stored to `0x0054DC30`, `0x0054DC31`, `0x0054DC32`), which is why those
three bytes are runtime values and every site reads them through the getters `0x004EE0E0` (parse phase), `0x004EE0F0` (size phase) and
`0x004EE100` (third phase) instead of hard-coding a phase number. `0x0054DC30`/`31`/`32` are initialised `0x00`/`0xFF`/`0xFF` in the image.

Prologue: `sub $0x1c,%esp`, four saved registers, then `mov 0x38(%esp),%edi` (`0x004EDD2F`) - with `esp = E-0x2C` at that point this reads
`E+0x0C`, the loader's own third argument. `EDI` therefore holds the QVM argument frame for the whole routine and is what the context record calls
"frame".

Size callback for the class named by QVM argument 0 of `DefinePhysicsObjType`. `EBX` is the definition record being built,
`0x00A5EFE0 + index * 0x2D8` with `index = [0x00A5EFD8]` (computed at `0x004EDD23`-`0x004EDD3C`), and its field 0 is the class id read from QVM
argument 0 at `0x004EDD4D`-`0x004EDD55` (`mov %ax,(%ebx)`):

```
0x004EDED6  xor    %eax,%eax
0x004EDED8  movl   $0x0,0x2d4(%ebx)        ; the block field starts cleared
0x004EDEE2  mov    0x0054DC31,%al          ; private size phase
0x004EDEE7  xor    %edx,%edx
0x004EDEE9  mov    (%ebx),%dx              ; class word of the object being defined
0x004EDEEC  add    $0x10,%esp
0x004EDEEF  lea    (%eax,%eax,2),%ecx
0x004EDEF2  shl    $0x7,%ecx
0x004EDEF5  add    %edx,%ecx               ; index = phase * 0x180 + class
0x004EDEF7  mov    0xa96ae0(,%ecx,4),%eax ; handler entry
0x004EDEFE  test   %eax,%eax
0x004EDF00  je     0x4EDF52                ; no size callback: skip the allocation and the parse dispatch too
0x004EDF02  lea    0x38(%esp),%ecx         ; &size_slot, a loader stack slot (== the loader's third argument slot at E+0x0C)
0x004EDF06  push   %ecx                    ; arg2 = &size_slot
0x004EDF07  push   $0x0                    ; arg1 = 0
0x004EDF09  call   *%eax
0x004EDF0B  mov    0x40(%esp),%edx         ; same slot, read back after the call
0x004EDF0F  push   $0x4                    ; arg2 = 4 (alignment)
0x004EDF11  push   %edx                    ; arg1 = size written by the callback
0x004EDF12  call   0x4B0C60                ; Mem_Alloc(size, 4)
0x004EDF17  mov    %eax,0x2d4(%ebx)        ; definition record +0x2D4 = the block
0x004EDF1D  mov    %eax,0x24(%esp)         ; context+0x0 = the block (context == E-0x18)
```

The stack accounting is the interesting part: this site does **not** clean its two argument words after `0x004EDF09`; `0x004EDF0B` reads the size
slot at `0x40(%esp)` while the two words are still pushed, and the arguments of the size call, the `Mem_Alloc` call and the parse call below are all
released by the single `add $0x18,%esp` at `0x004EDF4F`. That is ordinary shared-cleanup code generation, not a different calling convention.

Parse callback for the same class at the private parse phase, after the block exists:

```
0x004EDF21  mov    0x0054DC30,%eax         ; private parse phase
0x004EDF26  xor    %ecx,%ecx
0x004EDF28  mov    (%ebx),%cx              ; class word
0x004EDF2B  and    $0xff,%eax
0x004EDF30  lea    0x24(%esp),%edx         ; &context (E-0x18)
0x004EDF34  inc    %esi                    ; argument index is pre-incremented
0x004EDF35  lea    (%eax,%eax,2),%eax
0x004EDF38  push   %edx                    ; arg2 = &context
0x004EDF39  shl    $0x7,%eax
0x004EDF3C  add    %ecx,%eax               ; index = phase * 0x180 + class
0x004EDF3E  push   $0x0                    ; arg1 = 0
0x004EDF40  mov    %edi,0x30(%esp)         ; context+0x4 = QVM argument frame
0x004EDF44  mov    %esi,0x34(%esp)         ; context+0x8 = argument index
0x004EDF48  call   *0xa96ae0(,%eax,4)      ; re-dispatches from the table with arg1 = 0
0x004EDF4F  add    $0x18,%esp              ; releases all three calls' arguments
```

So the context record is exactly 12 bytes, `E-0x18`..`E-0x0D`, inside the loader's `sub $0x1c` area:

| offset | value | written at |
| --- | --- | --- |
| `+0x0` | the `Mem_Alloc(size, 4)` block (same pointer as definition record `+0x2D4`, `0x00A5EFE0 + index * 0x2D8`) | `0x004EDF1D` |
| `+0x4` | the QVM argument frame (`EDI`, loader argument 3, saved at `0x004EDD2F`) | `0x004EDF40` |
| `+0x8` | index of the next QVM argument to read (`ESI`, incremented at `0x004EDF34`) | `0x004EDF44` |

The base-class parse callback takes the same record: `0x004F43B0` (Vehicle class id `0x0054E200`, linked at the parse phase by `0x004F4230`) is

```
0x004F43B0  mov    0x8(%esp),%eax   ; arg2 = context
0x004F43B4  mov    0x4(%eax),%ecx   ; frame
0x004F43B7  lea    0x8(%eax),%edx   ; &context->next_argument
0x004F43BA  mov    (%eax),%eax      ; block
0x004F43BC  push   %ecx
0x004F43BD  push   %edx
0x004F43BE  push   %eax
0x004F43BF  call   0x5031F0         ; fills the base part of the block and advances the shared cursor
0x004F43C4  add    $0xc,%esp
0x004F43C7  ret
```

which is the same record, read the same way, with the shared cursor passed by address - and arg1 (`0x4(%esp)`) is never read. That is why
`0x0042DB00` can hand its own two arguments straight to that callback and why the derived handler must read `context+0x8` itself afterwards.

## 3. The four callbacks in this batch

### 3.1 `0x0042DCE0` (Car size) and `0x0042FEF0` (Wheel size)

Both are the same 11-byte routine shape as the already transcribed `0x00427F80`
(`engine/src/retail/script/native_427f80.cpp`, which stores `0x8`):

```
0x0042DCE0  mov    0x8(%esp),%eax   ; arg2
0x0042DCE4  movl   $0x2c0,(%eax)    ; Car block size
0x0042DCEA  ret                     ; EAX still holds arg2
0x0042FEF0  mov    0x8(%esp),%eax
0x0042FEF4  movl   $0x2f0,(%eax)    ; Wheel block size
0x0042FEFA  ret
```

No branch, no global, no callee, arg1 unread, return value is arg2. The two sizes match the blocks the parse callbacks fill: Car writes its last
field at `0x2BF` (`mov 0x10` block at `0x2B0`), Wheel at `0x2EF` (`0x2EC` float + 4). This is the third independent size-callback instance after
`0x00427F80` and Vehicle `0x004F43D0` (`movl $0x118,(%eax)`, `0x004F43D0`), and the loader proves the out-parameter role by allocating the value
the callback wrote (`0x004EDF0B`-`0x004EDF12`).

### 3.2 `0x0042DB00` (Car parse)

`(std::uint32_t arg1, void* context)`; the retail reads `0x14(%esp)` after three pushes (`0x0042DB03`, arg2) and `0x14(%esp)` again after the
fourth push (`0x0042DB22`, arg1):

- `0x0042DB08`/`0x0042DB0A`: `ebp = context[0]` (block), `edi = context[1]` (argument frame).
- `0x0042DB0D`-`0x0042DB36`: index = `(byte 0x0054DC30 & 0xFF) * 0x180 + (word 0x0054E200 & 0xFFFF)`; push `context`, push arg1; `call *table[index]`.
  The retail does **not** test the entry for null here. The class word in `0x0054E200` is the parent that Car's own class init `0x0042DCF0` hands to
  `0x00401900` (`push $0x0; push $0x0; call 0x004F45F0; push %eax; push $0x2ED8; push $0x53BE28; call 0x00401900`), and `0x004F45F0` is the getter for
  `0x0054E200` (`mov 0x54e200,%ax; ret`), so this is the base-class (Vehicle) parse call.
- `0x0042DB38`-`0x0042DB51`: read a signed count from the frame at `context[2]`, store it at `block+0x118`.
- `0x0042DB61`-`0x0042DBF5`: `count` times a 0x14-byte record at `block+0x11C`: byte at `+0x11C`, byte at `+0x11D`, float at `+0x120`,
  dword at `+0x124`, `fmuls 0x005333B8` then float at `+0x128`, float at `+0x12C`. The loop counter lives in the dead `arg2` stack slot
  (`movl $0x0,0x18(%esp)` at `0x0042DB53`), the count is re-read from `block+0x118` every iteration (`0x0042DBE5`).
- `0x0042DC0B`: `fmuls 0x005333B8` then float at `block+0x25C`.
- `0x0042DC27`/`0x0042DC3D`: bytes at `block+0x260` and `block+0x261`.
- `0x0042DC43`-`0x0042DC57`: `0x4B8A80(frame, index, block+0x262, 0x20)` (the `0x20` is pushed first at `0x0042DC4C`).
- `0x0042DC67`: signed count at `block+0x2AC`; `0x0042DC85`-`0x0042DCAE`: that many dwords at `block+0x284`, cursor advanced by 4 through a
  stack slot (`0x0042DC81`, `0x0042DCA5`); count re-read from `block+0x2AC` (`0x0042DCA5`).
- `0x0042DCC4`: `0x4B8A80(frame, index, block+0x2B0, 0x10)`.

Every reader call increments `context[2]` in memory **before** the call (`mov 0x8(%esi),%eax; lea 0x1(%eax),%edx; mov %edx,0x8(%esi); call`), so the
base callback and the derived handler share one cursor. There are no bounds checks on the two counts: the retail stores whatever it reads into a
fixed-size block, and the transcription reproduces that (a class script that supplies too many records overwrites the block and beyond in the retail
too).

### 3.3 `0x0042FE80` (Wheel parse)

`(std::uint32_t arg1, void* context)`; arg1 is never read in the body (`0x0042FE80`-`0x0042FEE3`), arg2 is read at `0x0042FE82`. The body contains
**no table access at all**: unlike Car it never calls an inherited parse handler. (Wheel's class row does inherit one: `0x0042FF00` passes the getter
`0x4D9FA0` as the third argument of `0x00401900`, and that word is the class id of **MagicObj** - registered by `0x4D9A00` with the string at
`0x00548660` (`MagicObj`), record size `0x138`, its id stored to `0x00548648` at `0x004D9A1B`; `0x00401900` copies the parent's 0xFE-entry handler
row into the new class row (section 5.5). Wheel simply does not call through it.)

- `0x0042FEA0`: `0x4B8A80(frame, index, block+0x2D8, 0x10)`.
- `0x0042FEB0`/`0x0042FEBB`: `fmuls 0x005333B8`, float at `block+0x2E8`.
- `0x0042FECC`/`0x0042FED1`: `fmuls 0x005333B8`, float at `block+0x2EC`.

The three calls' arguments are released once at `0x0042FED7` (`add $0x20,%esp`).

## 4. The registrars `0x0042DAC0` and `0x0042FE40`

Both are the same 51-byte shape, two links through `QTask_LinkQTaskEventHandler` (`0x00401400`), each preceded by the phase getter and the class
getter, and the second link's EAX becomes the registrar's return value (no caller reads it):

```
0x0042DAC0  push   $0x42db00     ; handler  = 0x0042DB00
0x0042DAC5  call   0x4ee0e0      ; phase    = byte 0x0054DC30
0x0042DACA  push   %eax
0x0042DACB  call   0x42fdc0      ; class id = word 0x0053BDA8 (Car)
0x0042DAD0  push   %eax
0x0042DAD1  call   0x401400      ; link(class, phase, handler)
0x0042DAD6  add    $0xc,%esp
0x0042DAD9  push   $0x42dce0     ; handler  = 0x0042DCE0
0x0042DADE  call   0x4ee0f0      ; phase    = byte 0x0054DC31
0x0042DAE3  push   %eax
0x0042DAE4  call   0x42fdc0      ; same class id
0x0042DAE9  push   %eax
0x0042DAEA  call   0x401400
0x0042DAEF  add    $0xc,%esp
0x0042DAF2  ret

0x0042FE40  push   $0x42fe80     ; handler  = 0x0042FE80
0x0042FE45  call   0x4ee0e0      ; phase    = byte 0x0054DC30
0x0042FE4A  push   %eax
0x0042FE4B  call   0x430950      ; class id = word 0x0057BD30 (Wheel)
0x0042FE50  push   %eax
0x0042FE51  call   0x401400
0x0042FE56  add    $0xc,%esp
0x0042FE59  push   $0x42fef0
0x0042FE5E  call   0x4ee0f0      ; phase    = byte 0x0054DC31
0x0042FE63  push   %eax
0x0042FE64  call   0x430950
0x0042FE69  push   %eax
0x0042FE6A  call   0x401400
0x0042FE6F  add    $0xc,%esp
0x0042FE72  ret
```

This is the same argument order the Vehicle registrar `0x004F4230` uses (`push $0x4f43b0; call 0x4ee0e0; push %eax; mov 0x54e200,%dx; push %edx;
call 0x401400`, then `push $0x4f43d0; call 0x4ee0f0; ...`), which is why `QTask_LinkQTaskEventHandler(class_id, phase, handler)` in
`task_types.hpp` - index `class_id + phase * 384` - is the right call shape.

Callers: start-up at `0x00402B79` calls `0x0042FF00` (Wheel class init), `0x00402B7E` calls `0x0042FE40`, `0x00402B83` calls `0x0042DCF0` (Car class
init), `0x00402B88` calls `0x0042DAC0`, all with no arguments. Both registrars are therefore reached once, after their class init has written the id
word, and they only read the id back.

## 5. Cross-checks outside this batch

1. **A size callback already in the tree**: `0x00427F80` is transcribed at `engine/src/retail/script/native_427f80.cpp` as
   `(void* arg0, std::uint32_t* value)` storing `0x8` through `value` and returning it; `arg0` is unread. Same shape, same role, independently
   transcribed before this investigation.
2. **The whole physics-object family**: every class of the family links a parse callback at the private parse phase (`0x0054DC30`, read through
   `0x004EE0E0`) and a size callback at the private size phase (`0x0054DC31`, read through `0x004EE0F0`), through `0x00401400`, and every size
   callback is the same 11-byte `mov 0x8(%esp),%eax; movl $imm,(%eax); ret`:

   | class id word VA | class name | size | parse callback | size callback | parse linked at | size linked at |
   | --- | --- | --- | --- | --- | --- | --- |
   | `0x0053BB8C` | (hash entry) | `0x250` | `0x00429730` | `0x00429720` | `0x00428081` | `0x0042809C` |
   | `0x0057BD10` | (hash entry) | `0x20` | `0x0042D9F0` | `0x0042DA60` | `0x0042D374` | `0x0042D38E` |
   | `0x0053BE58` | (hash entry) | `0x154` | `0x00432F60` | `0x00432F80` | `0x00431275` | `0x00431290` |
   | `0x0054066C` | (hash entry) | `0x8` | `0x00476F70` | `0x00427F80` | `0x00476803` | `0x0047681E` |
   | `0x0054E200` | `0x0054E298` ("Vehicle") | `0x118` | `0x004F43B0` | `0x004F43D0` | `0x004F422B` | `0x004F4246` |
   | `0x0053BDA8` | `0x0053BE28` ("Car") | `0x2C0` | `0x0042DB00` | `0x0042DCE0` | `0x0042DAC0` | `0x0042DAD9` |
   | `0x0057BD30` | `0x0053BE4C` ("Wheel") | `0x2F0` | `0x0042FE80` | `0x0042FEF0` | `0x0042FE40` | `0x0042FE59` |

   All seven parse callbacks read the same 12-byte context record (`mov 0x8(%esp),%eax` then `(%eax)` block, `0x4(%eax)` frame, `0x8(%eax)` cursor),
   all seven size callbacks write through arg2 and return it, and none of them reads arg1. `0x00429730` is the closest corroboration of Car's body:
   it dispatches `(arg1, context)` to the base class at `(parse phase, 0x0054E200)` exactly like `0x0042DB00` (`0x0042975D`-`0x00429766`), then
   advances `context+0x8` before each read and fills `block+0x118`, `+0x11C`, `+0x120`, `+0x124`, ... through `0x004B8A20`/`0x004B8A50`.
   `0x0042D9F0` reads `context+2` and hands it to `0x004B8A20`, `0x00432F60` hands `(block, &context[2], frame)` to `0x00433030` exactly as Vehicle
   `0x004F43B0` does to `0x005031F0`, and `0x00476F70` reads `context[2]` into an x87 argument for `0x004B8A50` and stores `fstps (%esi)` with
   `esi = context[0]`.
3. **Existing transcriptions in the tree** already record the same split. `QTask_RegisterInputPortHandler` (`0x004012A0`) in
   `engine/src/retail/input/input_port_process.cpp` dispatches two words at `0x004012DC`-`0x004012E0` with the local cast
   `void (NATIVE_DECL*)(void*, std::uint32_t)`; its caller builds a 12-byte record `{0x4, 0x005393E8 or 0x005393F8, byte}` at `0(%esp)`
   (`0x004154F7`-`0x00415510`) and passes its address as the wrapper's third argument, which the wrapper forwards as the handler's second word. The
   Flow walks at `0x00402365`, `0x004023CC` and `0x00402481` push one word and clean 4. The same file's status line already states that retail
   handlers are bare function pointers with no single signature.
4. **`0x00407C77`/`0x00407CBC`**: `lea 0x8(%esp),%edx; push %edx; push $0x0; call *%eax; add $0x8,%esp` - a two-word dispatch whose second word is
   the address of a caller local, with literal `0` as the first word, twice in a row inside one function.

5. **`0x00401900` is the class registration that builds every table row**, and its body independently confirms the layout the ports assume:
   `entry(class, phase) = 0xA96AE0 + class*4 + phase*0x600`, with class ids handed out in registration order and `0x181` as the "no parent" sentinel.

   ```
   0x00401927  mov    0xc(%esp),%ecx          ; arg2 = record size
   0x0040192b  mov    0x8(%esp),%eax          ; arg1 = name string pointer
   0x00401938  mov    0x18(%esp),%edi         ; arg3 = parent class id (after the two saves)
   0x00401943  mov    %ecx,0xaf5ee4(%esi)     ; record+4 = size
   0x00401949  mov    %eax,0xaf5ee0(%esi)     ; record+0 = name
   0x00401954  mov    %di,0xaf5ee8(%esi)      ; record+8 = parent class id
   0x0040196a  mov    0xaf5f0c(%edx),%eax     ; parent record+0x2c
   0x00401970  mov    %eax,0xaf5f0c(%esi)     ; inherited into record+0x2c
   0x004019ad  cmp    $0x181,%di
   0x004019ba  lea    0xa96ae0(,%ebp,4),%ecx  ; dest = table + new class * 4
   0x004019c1  mov    $0xfe,%edx
   0x004019c6  lea    0xa96ae0(,%edi,4),%eax  ; src  = table + parent class * 4
   0x004019cd  mov    (%eax),%esi
   0x004019cf  add    $0x600,%eax             ; next phase of the parent's row
   0x004019d4  mov    %esi,(%ecx)
   0x004019d6  add    $0x600,%ecx             ; next phase of the new row
   0x004019dc  dec    %edx
   0x004019dd  jne    0x004019cd             ; 0xFE phases copied
   0x004019df  call   0x004014b0             ; not transcribed
   0x004019e5  mov    %bp,%ax                 ; returns the new class id
   ```

   With `arg3 == 0x181` the body takes the other arm (`0x004019EB`) and stores `0` to the same 0xFE entries instead of copying (`0x004019F7`-
   `0x00401A03`), i.e. a class registered without a parent starts with an empty row. That is why Car's parse callback can dispatch at
   `(0x0054DC30, 0x0054E200)` without a null test: `0x0042DCF0` registered Car with `arg3 = 0x54E200` (Vehicle), so Car's row began as a copy of
   Vehicle's row and Vehicle linked its parse handler there through `0x004F4230`. The four ids in this batch are assigned by this routine:
   `0x0054E200` (Vehicle), `0x0053BDA8` (Car), `0x0057BD30` (Wheel), `0x00548648` (MagicObj). `0x00401900` itself is outside this batch and is
   not transcribed here.

## 6. Survey of every handler-table dispatch site in `build/retail-dis.txt`

The table is addressed in two ways that both have to be counted: with the base `0x00A96AE0` and with the phase folded into the displacement. The
loader and `0x0042DB00` compute `phase * 0x180 + class` at run time and index `0xA96AE0`, but where the phase is a compile-time constant the compiler
folds it and the instruction carries `0x00A96AE0 + phase * 0x600` directly. The image therefore references **13 distinct bases**:

| base | phase | refs |
| --- | --- | --- |
| `0x00A96AE0` | 0 | 588 |
| `0x00A970E0` | 1 | 119 |
| `0x00A976E0` | 2 | 15 |
| `0x00A97CE0` | 3 | 1 |
| `0x00A982E0` | 4 | 21 |
| `0x00A988E0` | 5 | 2 |
| `0x00A994E0` | 7 | 13 |
| `0x00A99AE0` | 8 | 5 |
| `0x00A9A0E0` | 9 | 7 |
| `0x00A9A6E0` | 10 | 5 |
| `0x00A9ACE0` | 11 | 4 |
| `0x00A9C4E0` | 15 | 7 |
| `0x00A9CAE0` | 16 | 4 |

The phases that never appear folded only occur as run-time values (the three private phases `0x54DC30`/`31`/`32` are 0/1/2 here, supplied
dynamically), which is consistent with phases 6, 12, 13, 14 and 17+ having no static dispatcher.

**791 references in total**: 701 loads into a register, 78 `call` operands indexed in place, 9 `cmp`/`lea`/`test`, 3 other. The folded bases are
themselves evidence for the index formula in `task_types.hpp`: every observed base is exactly `0x00A96AE0 + phase * 0x600`, i.e. `phase * 0x180`
entries, which confirms `task_phase_stride = 0x180` from a second, independent direction (the loader's `(phase * 3) << 7` is the same arithmetic).

There are also *other* dispatch tables in the same address range (`0x00A94E84` carries 88 indexed references, plus small ones at `0x00A94A80`,
`0x00A950A0`, `0x00A952A4`, `0x00A96108`, `0x00A965C0`). They are not `QTask_HandlerTable` (their offsets are not multiples of the 0x600 phase
stride) and are outside this survey; the four callbacks in this batch are only ever *referenced* by their registrars' `push` instructions
(`0x0042DAC0`, `0x0042DAD9`, `0x0042FE40`, `0x0042FE59`) and are reachable only through `QTask_HandlerTable`.

Method for the dispatch sites: select every indirect `call`; a site targets the table when the `call` operand itself is a table base indexed by
`4 * register` (78 sites), or when its target register was loaded by such an indexed table access and nothing between that load and the call
overwrites the register (647 sites; the backward scan stops at a call/ret/jmp or at a write to the register). For each site the *argument window* is
the run after the last control-flow transfer (`call`/`ret`/`jmp`/`Jcc`/`loop`), capped at 20 instructions, and every `push` in it is counted -
pushes interleaved with `mov`/`lea` are the norm (the loader writes the context fields between its two argument pushes), so counting only
*consecutive* pushes would undercount. The window can overcount saved-register pushes at a function prologue; nine such sites were read
instruction by instruction and resolved as described below. Cleanup is the first `add $imm,%esp` within five instructions after the call, stopping
at a call/ret/jmp.

Result: **725 dispatch sites**.

- **457 sites push two argument words, 268 push one word.** After resolving the nine window outliers (below) no site passes zero or more than two
  argument words.
- Cleanup after the 457 two-word sites: `add $0x8` 356, none within the window 67, `add $0x18` 10, `add $0x10` 8, `add $0x1c` 6, `add $0x14` 3,
  `add $0xc` 3, `add $0x20` 2, `add $0x4` 1, `add $0x30` 1.
- Cleanup after the 268 one-word sites: `add $0x4` 198, none within the window 48, `add $0xc` 7, `add $0x8` 5, `add $0x14` 4, `add $0x20` 3,
  `add $0x10` 2, `add $0x18` 1.
- The non-4/non-8 values are shared cleanups: e.g. the loader's two sites (`0x004EDF09`, `0x004EDF48`) are both released by the single
  `add $0x18,%esp` at `0x004EDF4F`, and `0x0042DB48` releases the base-class dispatch and the first reader call together (`add $0x10`). 67+48 sites
  have no `add` in the five-instruction window at all because the block cleans its stack further away or once at its epilogue.

The nine window outliers, each read instruction by instruction (the extra pushes belong to saved registers or to an earlier setup, not to the
dispatch):

| site | window pushes | actual argument words | why |
| --- | --- | --- | --- |
| `0x00413D9F` | 3 | 2 (`&local`, `0`) | the first push belongs to the preceding statement; the dispatch's own two pushes are `lea 0x4(%esp),%eax; push %eax; push %edx` and `add $0x8` follows |
| `0x0041EC91` | 5 | 1 (`%edi`) | `push %ebx/%ebp/%esi/%edi` are the prologue saves of the function that starts just above |
| `0x0042B941` | 4 | 1 (`%ebx`) | `push %esi; push %edi` are saves (the function loads those registers as scratch afterwards); the single argument push `%ebx` is cleaned by `add $0x4,%esp` at `0x0042B95D` |
| `0x0043E2A1` | 4 | 1 (`%ebx`) | same code shape as `0x0042B941` |
| `0x0047C452` | 5 | 2 (`&local`, `%ecx`) | saves plus `lea 0xc(%esp),%eax; push %eax; push %ecx` |
| `0x004DE256` | 4 | 2 (`%edi`, `%esi`) | two saves plus the two argument pushes, `add $0x8` follows |
| `0x004DE2E6` | 4 | 2 (`%edi`, `%esi`) | same shape |
| `0x004DE376` | 4 | 2 (`%edi`, `%esi`) | same shape |
| `0x004DE414` | 6 | 2 (`%ebx`, `%esi`) | four saves plus two argument pushes, `add $0x8` follows |

What the second word *is* at the 457 two-word sites, by the instruction that produced the value pushed first (arg2):

| producer | sites | reading |
| --- | --- | --- |
| `lea` of a stack slot into the pushed register | 178 | address of caller-owned storage (the out-parameter and context-record cases) |
| `mov` from memory/register | 137 | a pointer or word loaded from an object/global |
| literal `$0x0` | 50 | an unused second word |
| written outside the 20-instruction window | 91 | value computed further up; not classified here |
| `xor` with itself (zero) | 1 | zero, like the literal case |

At the 268 one-word sites the single pushed word comes from `%esi` at 153 sites (the task/object pointer of the per-frame walk), `%ecx` at 63, `%eax`
at 13, `%edi` at 13, `%edx` at 12, `%ebx` at 9 and `%ebp` at 5.

What the survey proves: the table stores raw code addresses that are reached with **one or two 32-bit words**, nothing in the image passes a
by-value record or a third argument word to a single dispatch, and the two-word form dominates for construction/event/loader dispatch (457 of 725).
What it does not prove: the prototype of any individual handler, or the meaning of either word at a specific site - that has to be read out of the
handler body, as done for the four callbacks in section 3.

## 7. Verdict on `FrameHandler`

`engine/include/igi/retail/core/task_types.hpp` currently declares:

```cpp
using FrameHandler = void (*)(void*);
```

**As a storage type for the table this is fine and should stay**: the table is an array of raw code addresses (`QTask_HandlerTable[]`), and this
alias is what the linker-facing declarations and the existing per-frame walk use. **As a call signature it is proven incomplete**, by the bodies and
by the survey:

1. 457 of 725 dispatch sites pass two 32-bit words and clean 8 bytes; calling such an entry through `void (*)(void*)` would pass one word and leave
   the second argument undefined.
2. The two word positions are not typed: the loader passes literal `0` as the first word, `QTask_RegisterInputPortHandler` (`0x004012A0`) passes
   an integer context as the second word (its transcription at `engine/src/retail/input/input_port_process.cpp` uses the local call type
   `void (NATIVE_DECL*)(void*, std::uint32_t)`), 50 two-word sites push the literal `$0x0` as the second word and one more pushes an `xor`ed zero
   there. Neither parameter is always a pointer.
3. The table is heterogeneous by construction: different phases hold callbacks of different roles (per-frame walk, construction/event, class
   size, class parse), and the retail reaches each entry through a cast-free indirect call, i.e. the image itself has no single prototype.

Exact replacement for `task_types.hpp` (Lead-owned; not edited by this batch): the alias itself stays, and the header comment must stop implying
that it is a call signature for every entry.

```cpp
// Storage type of one handler-table entry (retail 0x00A96AE0). The table is a heterogeneous array
// of raw code addresses: retail dispatches entries with one word (268 of 725 sites, e.g. the
// per-frame walks at 0x00402365/0x004023CC/0x00402481) or two words (457 of 725 sites, e.g. the
// loader 0x004EDF48 and 0x004012A0), and the word positions are sometimes integers and sometimes
// pointers, so this alias is only the element type of the array. A call site must cast to the
// shape it actually passes (for two words with a pointer second argument: void (*)(std::uint32_t,
// void*)). The retail image has no prototype for any entry.
using FrameHandler = void (*)(void*);
```

No new global type alias is needed or recommended: the faithful form is a *local* call type at each dispatch site, exactly as
`engine/src/retail/input/input_port_process.cpp` already does (`using PhaseZeroHandler = void (NATIVE_DECL*)(void*, std::uint32_t);` followed by
`reinterpret_cast<PhaseZeroHandler>(handler)(task, context)`), and the two new parse callbacks do when they call the table entry. The array
declaration and `QTask_LinkQTaskEventHandler` need no change; the registrars keep casting the callback address to `FrameHandler` only to store it.

## 8. Unknowns (not established by this batch)

1. **arg1 of the two-word dispatch**: `0` at the loader's two call sites, forwarded unchanged by `0x0042DB00`, never read by `0x0042FEF0`,
   `0x0042FE80`, `0x004F43B0` or any size callback. The construction/event paths pass an object pointer in some sites and `0` in others; what the
   handler is supposed to do with it is not established by the code in this batch.
2. **`0x004B8A20` and `0x004B8A80`** are not transcribed in `engine/` yet. Their return conventions are taken from the project's existing candidate
   notes (`Script_GetArg` / `Sys_ResolveSymbol_23`, integer and argument-block readers). Both are declared in the two parse-callback files under
   address-suffixed placeholders and must be unified when their own natives land. `0x004B8A80`'s return value is unobserved at these sites.
3. **Field meanings** of the Car block (`0x118`..`0x2BF`) and the Wheel block (`0x2D8`..`0x2EF`) are not named by the image; only their offsets,
   widths, argument order and the `4096.0` scale (`0x005333B8`) are established.
4. **The rest of the loader `0x004EDD20`**: after the parse dispatch it continues at `0x004EDF52` (increments the counter at `0x00A5EFD8`, copies
   six dwords from the loader's frame and increments a global), and the size-handler-null branch jumps into that same tail with a different stack
   offset. The loader is not transcribed here; only its two dispatch sites and the context record are documented.
5. **The third private phase** `0x0054DC32` and the handler family registered against it (`0x0042DCF0`/`0x0042FF00` also link callbacks at phases
   `0`, `1`, `2` through `0x00401400`/`0x00401530`) are outside this batch.
6. **`0x004018C0`** (called by `0x004EDFB0`, which unlinks all three private phases) is the removal counterpart; not transcribed here.
7. **`0x00401900`, `0x004014B0`, `0x00401A20` and `0x004B0B00`** (class registration, its post-registration call, the class-row reset seen at
   `0x0053150B`/`0x00531521`, and the container initialiser used for `record+0xC`) are read only far enough to establish the table layout and the
   class-id provenance; none is transcribed, and the class record fields beyond `+0`/`+4`/`+8`/`+0xC`/`+0x24`/`+0x25`/`+0x2C` are not mapped.

## Appendix A: the six routines

| VA | bytes | blocks/edges | instructions | calls | caller |
| --- | --- | --- | --- | --- | --- |
| `0x0042DAC0` | 51 | 1 / 0 | 15 | `0x004EE0E0`, `0x0042FDC0`, `0x00401400`, `0x004EE0F0`, `0x0042FDC0`, `0x00401400` | `0x00402B88` (start-up) |
| `0x0042DB00` | 465 | 5 / 8 | 156 | `0x004EE0E0`, `0x004F45F0`, table dispatch, `0x004B8A20` x8, `0x004B8A50` x4, `0x004B8A80` x2 | table only |
| `0x0042DCE0` | 11 | 1 / 0 | 3 | - | table only |
| `0x0042FE40` | 51 | 1 / 0 | 15 | `0x004EE0E0`, `0x00430950`, `0x00401400`, `0x004EE0F0`, `0x00430950`, `0x00401400` | `0x00402B7E` (start-up) |
| `0x0042FE80` | 100 | 1 / 0 | 36 | `0x004B8A80`, `0x004B8A50` x2 | table only |
| `0x0042FEF0` | 11 | 1 / 0 | 3 | - | table only |

Notes on the counts: the listing's byte column truncates one 8-byte instruction to 7 bytes (the `movl $0x0,0x18(%esp)` at `0x0042DB53`), so the
instruction-byte sum is 464 while the span `0x0042DB00`-`0x0042DCD0` is 465 bytes; the block/edge counts are from branch instructions in the body
(`0x0042DB0D`-`0x0042DB36` fall-through, `0x0042DB5B jle`, `0x0042DBEF jl`, `0x0042DC79 jle`, `0x0042DCA8 jl`), and the tail is shared between both
count tests (`jle` targets `0x0042DBFB` and `0x0042DCB0`).

## Appendix B: every handler-table dispatch site in `build/retail-dis.txt` (725 sites)

Grouped by the number of argument words pushed in the argument window and by the immediate `add $imm,%esp` cleanup found within five instructions
after the call ("no immediate cleanup" means the site shares a cleanup with neighbouring calls or cleans later). Sites are the addresses of the
`call` instructions.

### one argument word, no immediate cleanup (48 sites)

0x40eab6 0x419c3f 0x419ca6 0x419d10 0x41a69b 0x41a9bb 0x41be1d 0x41cdc6
0x41ce7e 0x41ec91 0x41edea 0x41ee34 0x41fa69 0x41fde6 0x421c48 0x422734
0x422f2b 0x423913 0x42b941 0x42e562 0x43b203 0x43b28c 0x43b667 0x43b727
0x43b7cc 0x43e2a1 0x440987 0x440a87 0x440b60 0x440df7 0x440ee3 0x440f6c
0x4439b3 0x443aa7 0x443bc0 0x4481a0 0x448317 0x448430 0x449347 0x4494ea
0x4496a7 0x44974c 0x454ef6 0x46e6ff 0x474f47 0x4754b6 0x4e6c23 0x4f4675

### one argument word, cleanup `add $0x4,%esp` (198 sites)

0x40164f 0x401682 0x402365 0x4023cc 0x402481 0x402511 0x402578 0x4025f5
0x416399 0x417556 0x4175fb 0x41764a 0x417684 0x4178bf 0x417ae1 0x4180bf
0x4180d6 0x4184a5 0x4184c3 0x4187bf 0x4188ed 0x4189c5 0x4189fb 0x418a4f
0x41a176 0x41a3ff 0x41a468 0x41aa49 0x41ae89 0x41b238 0x41b658 0x41b80f
0x41c046 0x41c2cc 0x41c4ce 0x41c576 0x41c9c5 0x41cb1c 0x41d621 0x41d78e
0x41da6a 0x41db06 0x41e117 0x41e1b9 0x41e4d7 0x41e574 0x41ed57 0x41f110
0x41f1b6 0x41fcc9 0x41fd78 0x41fffb 0x420096 0x4208d6 0x42096e 0x420c6d
0x421125 0x4211a0 0x421666 0x421a65 0x42216a 0x4222d6 0x422376 0x422aec
0x4233bd 0x423755 0x423e86 0x423f64 0x4242d6 0x424356 0x4243d6 0x424738
0x425197 0x425377 0x4284b1 0x42b88e 0x42b901 0x42c091 0x42d659 0x42e41f
0x42e488 0x42e9b5 0x430482 0x431a56 0x43373c 0x436a20 0x436d7f 0x437d81
0x437e5d 0x439e9d 0x43a37b 0x43a3aa 0x43a409 0x43afb3 0x43b127 0x43b523
0x43c169 0x43ca5d 0x43d31f 0x43dd40 0x43e5d2 0x43ebf2 0x43f39d 0x43fe29
0x4408bb 0x4409b5 0x440d8b 0x440e25 0x441ab2 0x44265d 0x442707 0x4438fb
0x4439e1 0x4445da 0x444613 0x44469d 0x44513d 0x4452ad 0x446b13 0x446d6b
0x4474cc 0x4480d4 0x4481ce 0x448e7d 0x449aca 0x44c6da 0x450071 0x459ef9
0x45cff8 0x45ef1b 0x45efc0 0x468d70 0x4693f8 0x46a122 0x46adf3 0x46aed8
0x46c472 0x46d7b2 0x46debf 0x46e6cd 0x46f926 0x4739c3 0x474337 0x475cd7
0x476c8d 0x4786db 0x4805da 0x4819bd 0x481af7 0x481bfd 0x485701 0x486d42
0x486fc4 0x4876b4 0x487761 0x487fd6 0x488127 0x488749 0x488a67 0x489240
0x4892f3 0x4d99b9 0x4de528 0x4de591 0x4de618 0x4de67a 0x4e748a 0x4e74d5
0x4e7ce0 0x4e8025 0x4e9b3a 0x4eb41a 0x4ebd7e 0x4f05c9 0x4f06b2 0x4f09b5
0x4f09d9 0x4f20d6 0x4f2ad6 0x4f2b24 0x4f3af1 0x4f5114 0x4f521f 0x503b32
0x504942 0x50499d 0x504fc4 0x508381 0x509644 0x51bde1 0x51be5d 0x51f311
0x51f38d 0x51fa61 0x51faea 0x5203d1 0x520452 0x5316c2

### one argument word, cleanup `add $0x8,%esp` (5 sites)

0x41cddf 0x41ec9f 0x41fa77 0x41fe09 0x470883

### one argument word, cleanup `add $0xc,%esp` (7 sites)

0x410895 0x418777 0x41ee57 0x448871 0x4707f9 0x475d71 0x4f2127

### one argument word, cleanup `add $0x10,%esp` (2 sites)

0x41d391 0x422e2e

### one argument word, cleanup `add $0x14,%esp` (4 sites)

0x418929 0x41af9a 0x41df26 0x422fa2

### one argument word, cleanup `add $0x18,%esp` (1 sites)

0x41bc8e

### one argument word, cleanup `add $0x20,%esp` (3 sites)

0x41cea1 0x41ea5b 0x41f734

### two argument words, no immediate cleanup (67 sites)

0x408155 0x408211 0x4103c2 0x41467c 0x414748 0x41a38c 0x41acbc 0x41b4ac
0x41bb2d 0x41bd9c 0x41c8ec 0x41ccf0 0x41d2ac 0x41d416 0x41e8dd 0x41ebd6
0x41f8a8 0x42056d 0x420eac 0x4219cc 0x422ce0 0x422ea8 0x429766 0x42db36
0x43af3d 0x43b19e 0x43b4ad 0x43b6be 0x44085c 0x440a1e 0x440d4c 0x440e7e
0x443869 0x443a3e 0x448019 0x448b34 0x4492bd 0x44963e 0x454b2c 0x456c8c
0x459a3c 0x45cc7f 0x45f0b1 0x45f0f7 0x45f303 0x45f344 0x463024 0x464505
0x46454b 0x469cdb 0x47051c 0x47077d 0x472e2c 0x4734ec 0x473acc 0x47547c
0x475c56 0x47c452 0x49e607 0x4c45ba 0x4ebf1d 0x4edf09 0x4f20fe 0x508515
0x508527 0x50898e 0x528a31

### two argument words, cleanup `add $0x4,%esp` (1 sites)

0x42fc0c

### two argument words, cleanup `add $0x8,%esp` (356 sites)

0x4012de 0x4013ee 0x401dc4 0x401ec2 0x407270 0x407c77 0x407cbc 0x4108ee
0x41094e 0x41319c 0x4131d7 0x413cb4 0x413d9f 0x419bfc 0x419c7e 0x41a5e4
0x41ae0e 0x41dded 0x41ec54 0x41f5be 0x41f9f4 0x420539 0x4207d2 0x421066
0x42152c 0x4215c4 0x42260d 0x4226b9 0x4232d4 0x4234d1 0x424518 0x4246c5
0x426581 0x429516 0x4296f2 0x42a281 0x42a6e6 0x42a775 0x42ab2e 0x42abc4
0x42ac93 0x42bb8e 0x42bbce 0x42be89 0x42d287 0x42d988 0x42d9e4 0x42e525
0x42ebe9 0x42eedd 0x42f765 0x42fcb9 0x431df0 0x432461 0x432529 0x432b56
0x432fbe 0x433b07 0x434218 0x4342df 0x4349ea 0x438555 0x4389bc 0x439f01
0x43a5ad 0x43ac85 0x43c6ac 0x43d936 0x43e26d 0x43efe4 0x43fde0 0x4400ac
0x442870 0x442c4e 0x443423 0x4434b2 0x4435eb 0x44483a 0x445d9c 0x446421
0x4473ce 0x447a31 0x44823b 0x4487b3 0x448a64 0x448af2 0x4491fb 0x44f560
0x44fa89 0x44fb7d 0x450d79 0x454dfd 0x454eb7 0x454fe6 0x456f26 0x45877c
0x459dad 0x459ea6 0x45b67c 0x45de0c 0x45f53b 0x45f83b 0x45fbec 0x45fe0b
0x46047c 0x460990 0x4609e0 0x460b10 0x460ddd 0x461094 0x462936 0x463d1b
0x464217 0x465b6e 0x467946 0x467b83 0x468849 0x46aa5c 0x46c875 0x46c92a
0x46ca19 0x46d385 0x46dadb 0x46e1d7 0x46e77c 0x46e931 0x46f3f5 0x46f9c5
0x47001c 0x470912 0x470c5c 0x470cae 0x4728ed 0x472923 0x472b37 0x472cb9
0x472e9e 0x473094 0x47354f 0x473666 0x473697 0x473b3e 0x4743f1 0x475288
0x475dc1 0x476fd6 0x4779a0 0x4779b9 0x477a44 0x477b7d 0x477bb3 0x477cf0
0x478534 0x478941 0x4789a2 0x4797b5 0x479923 0x479a70 0x479af8 0x47a2ba
0x47a3d3 0x47a5a5 0x47a7b1 0x47a9c8 0x47aa2d 0x47ab11 0x47ba54 0x47d09a
0x47eabe 0x480ec9 0x48121c 0x48236c 0x482489 0x482522 0x482740 0x487029
0x4875a1 0x48764c 0x48783b 0x4878eb 0x487a41 0x487aee 0x487b90 0x487df1
0x487e81 0x488284 0x4885c8 0x4897d9 0x49ac27 0x4a0275 0x4a0317 0x4a2395
0x4c18f5 0x4c3242 0x4c36b1 0x4c4752 0x4c568f 0x4c5a72 0x4c7aed 0x4c7bd0
0x4c87bb 0x4c889e 0x4c9c01 0x4cb267 0x4cb31d 0x4cb63b 0x4cb6b8 0x4ccc58
0x4ccca0 0x4d1324 0x4d13f2 0x4d1441 0x4d1ae3 0x4d1cab 0x4d2766 0x4d28ed
0x4d293f 0x4d298f 0x4d76d8 0x4d8968 0x4d9572 0x4d99f8 0x4d9f36 0x4dbe0a
0x4dc419 0x4dd25b 0x4dd2a4 0x4dd2f1 0x4de256 0x4de2a4 0x4de2e6 0x4de334
0x4de376 0x4de3c4 0x4de414 0x4de483 0x4de7db 0x4de874 0x4de8db 0x4de92a
0x4de9dc 0x4dea58 0x4deaf5 0x4deb71 0x4dec87 0x4ded03 0x4deda2 0x4dee1e
0x4df436 0x4e07dc 0x4e08f1 0x4e17a4 0x4e400b 0x4e4389 0x4e5fd1 0x4e6b73
0x4e6c4f 0x4e6cca 0x4e6d07 0x4e7273 0x4e8e09 0x4ea08e 0x4eaef9 0x4eb5a1
0x4eb653 0x4eb71d 0x4eb7e1 0x4eb871 0x4eb901 0x4eb991 0x4eba21 0x4ec14c
0x4f08e4 0x4f12c1 0x4f20a6 0x4f21a3 0x4f2237 0x4f22d0 0x4f2377 0x4f23b2
0x4f23f2 0x4f271c 0x4f2d1c 0x4f319a 0x4f3216 0x4f32a0 0x4f331c 0x4f39ab
0x4f3d08 0x4f3da0 0x4f3e29 0x4f40b8 0x4f4416 0x4f447c 0x4f4515 0x4f4557
0x4f6470 0x4f90a6 0x4f95ff 0x4f96c8 0x4fa1fa 0x4fa30a 0x4fa39f 0x4fbc8f
0x4fde92 0x4fea92 0x502e4f 0x503116 0x503146 0x503176 0x5031a6 0x504226
0x504f53 0x50503e 0x50512a 0x5051e5 0x5052e5 0x505380 0x505451 0x5054e1
0x505571 0x5055f9 0x505787 0x5058b1 0x5059a2 0x505bb6 0x505be4 0x506dd4
0x507e64 0x5085b4 0x508667 0x512576 0x51b09f 0x51b384 0x51bf4b 0x51c0f1
0x51c1a2 0x51c1fe 0x51f51b 0x51fe8f 0x52069b 0x523758 0x5239f8 0x523ab8
0x523c2f 0x527425 0x528365 0x529117 0x5291cd 0x529c95 0x52c4d7 0x52c69c
0x52c978 0x52c9c7 0x53177f 0x531c73

### two argument words, cleanup `add $0xc,%esp` (3 sites)

0x45f574 0x45f872 0x4ee3e6

### two argument words, cleanup `add $0x10,%esp` (8 sites)

0x40f1ec 0x41ebff 0x41f8d5 0x47b0e5 0x4e0c96 0x4e6c92 0x4f2342 0x51b840

### two argument words, cleanup `add $0x14,%esp` (3 sites)

0x406f48 0x41cd17 0x4dd638

### two argument words, cleanup `add $0x18,%esp` (10 sites)

0x4081a8 0x408265 0x4146d2 0x4147a2 0x41b579 0x41e00f 0x47d305 0x4c460c
0x4ebf6c 0x4edf48

### two argument words, cleanup `add $0x1c,%esp` (6 sites)

0x448a06 0x455039 0x456f79 0x45f12e 0x45f369 0x464571

### two argument words, cleanup `add $0x20,%esp` (2 sites)

0x408413 0x421c12

### two argument words, cleanup `add $0x30,%esp` (1 sites)

0x41bdc3


## 9. 0x00401900 transcription and link status

This section records the 0x00401900 work: the byte-level transcription in `engine/src/retail/core/native_401900.cpp`,
the arguments and return width read by its 279 callers, the CFG numbers, the link status of every symbol it needs, and
the two callees that were reported missing. It supersedes the §8 item 7 "untranscribed" list for 0x00401900, 0x004014B0
and 0x004B0B00: 0x00401900 has a committed implementation in `engine/src/retail/core/task_types.cpp` (line 362,
`TaskType_Register`), and 0x004014B0/0x004014E0/0x004B0B00 have committed implementation files.

**Status of the new file.** `native_401900.cpp` is a *move target*, not a second native: it carries the same
`TaskType_Register` definition that `task_types.cpp` already holds, so the tree must not link both. The new file is not
in `CMakeLists.txt`, so the `igi` target is unchanged today; the owner's atomic change (delete `task_types.cpp`
lines 346-427, add this file to the `igi` source list, repoint the manifest record for 0x00401900) makes it the single
implementation. Section 9.8 shows the link test for that end state.

### 9.1 Retail body

77 instructions, 273 bytes, `0x00401900-0x00401A10`; the byte column below is the GNU objdump text from
`build/retail-dis.txt`. The one truncated byte column in that file is 0x00401978, whose full encoding is
`c7 86 0c 5f af 00 00 00 00 20` (10 bytes, shown in the listing).

```
00401900  55                          push   %ebp
00401901  33 ed                       xor    %ebp,%ebp
00401903  b8 05 5f af 00              mov    $0xaf5f05,%eax
00401908  80 38 00                    cmpb   $0x0,(%eax)
0040190b  74 1a                       je     0x401927
0040190d  83 c0 30                    add    $0x30,%eax
00401910  45                          inc    %ebp
00401911  3d 05 a7 af 00              cmp    $0xafa705,%eax
00401916  7c f0                       jl     0x401908
00401918  68 c4 61 53 00              push   $0x5361c4
0040191d  e8 8e de 0a 00              call   0x4af7b0
00401922  83 c4 04                    add    $0x4,%esp
00401925  eb fe                       jmp    0x401925
00401927  8b 4c 24 0c                 mov    0xc(%esp),%ecx
0040192b  8b 44 24 08                 mov    0x8(%esp),%eax
0040192f  56                          push   %esi
00401930  8d 74 6d 00                 lea    0x0(%ebp,%ebp,2),%esi
00401934  c1 e6 04                    shl    $0x4,%esi
00401937  57                          push   %edi
00401938  8b 7c 24 18                 mov    0x18(%esp),%edi
0040193c  c6 86 05 5f af 00 01        movb   $0x1,0xaf5f05(%esi)
00401943  89 8e e4 5e af 00           mov    %ecx,0xaf5ee4(%esi)
00401949  89 86 e0 5e af 00           mov    %eax,0xaf5ee0(%esi)
0040194f  66 81 ff 81 01              cmp    $0x181,%di
00401954  66 89 be e8 5e af 00        mov    %di,0xaf5ee8(%esi)
0040195b  74 1b                       je     0x401978
0040195d  8b c7                       mov    %edi,%eax
0040195f  25 ff ff 00 00              and    $0xffff,%eax
00401964  8d 14 40                    lea    (%eax,%eax,2),%edx
00401967  c1 e2 04                    shl    $0x4,%edx
0040196a  8b 82 0c 5f af 00           mov    0xaf5f0c(%edx),%eax
00401970  89 86 0c 5f af 00           mov    %eax,0xaf5f0c(%esi)
00401976  eb 0a                       jmp    0x401982
00401978  c7 86 0c 5f af 00 00        movl   $0x20000000,0xaf5f0c(%esi)   ; 10 bytes: c7 86 0c 5f af 00 00 00 00 20
00401982  8b 44 24 1c                 mov    0x1c(%esp),%eax
00401986  85 c0                       test   %eax,%eax
00401988  74 1c                       je     0x4019a6
0040198a  6a 00                       push   $0x0
0040198c  50                          push   %eax
0040198d  51                          push   %ecx
0040198e  8d 8e ec 5e af 00           lea    0xaf5eec(%esi),%ecx
00401994  51                          push   %ecx
00401995  e8 66 f1 0a 00              call   0x4b0b00
0040199a  83 c4 10                    add    $0x10,%esp
0040199d  c6 86 04 5f af 00 01        movb   $0x1,0xaf5f04(%esi)
004019a4  eb 07                       jmp    0x4019ad
004019a6  c6 86 04 5f af 00 00        movb   $0x0,0xaf5f04(%esi)
004019ad  66 81 ff 81 01              cmp    $0x181,%di
004019b2  74 37                       je     0x4019eb
004019b4  81 e7 ff ff 00 00           and    $0xffff,%edi
004019ba  8d 0c ad e0 6a a9 00        lea    0xa96ae0(,%ebp,4),%ecx
004019c1  ba fe 00 00 00              mov    $0xfe,%edx
004019c6  8d 04 bd e0 6a a9 00        lea    0xa96ae0(,%edi,4),%eax
004019cd  8b 30                       mov    (%eax),%esi
004019cf  05 00 06 00 00              add    $0x600,%eax
004019d4  89 31                       mov    %esi,(%ecx)
004019d6  81 c1 00 06 00 00           add    $0x600,%ecx
004019dc  4a                          dec    %edx
004019dd  75 ee                       jne    0x4019cd
004019df  e8 cc fa ff ff              call   0x4014b0
004019e4  5f                          pop    %edi
004019e5  66 8b c5                    mov    %bp,%ax
004019e8  5e                          pop    %esi
004019e9  5d                          pop    %ebp
004019ea  c3                          ret
004019eb  8d 04 ad e0 6a a9 00        lea    0xa96ae0(,%ebp,4),%eax
004019f2  b9 fe 00 00 00              mov    $0xfe,%ecx
004019f7  c7 00 00 00 00 00           movl   $0x0,(%eax)
004019fd  05 00 06 00 00              add    $0x600,%eax
00401a02  49                          dec    %ecx
00401a03  75 f2                       jne    0x4019f7
00401a05  e8 a6 fa ff ff              call   0x4014b0
00401a0a  5f                          pop    %edi
00401a0b  66 8b c5                    mov    %bp,%ax
00401a0e  5e                          pop    %esi
00401a0f  5d                          pop    %ebp
00401a10  c3                          ret
```

### 9.2 Arguments, identifier assignment and return width

At entry (`E` = caller's `esp` at the `call`, return address at `[E]`):

| slot | meaning | first read |
| --- | --- | --- |
| `[E+4]` | class-name pointer (dword) | `0x0040192B mov 0x8(%esp),%eax` |
| `[E+8]` | object size (dword) | `0x00401927 mov 0xc(%esp),%ecx` |
| `[E+0xC]` | parent class id (dword slot, only the low 16 bits are used) | `0x00401938 mov 0x18(%esp),%edi`, then `cmp $0x181,%di` and `mov %di,0xaf5ee8(%esi)` |
| `[E+0x10]` | task-list argument (dword) | `0x00401982 mov 0x1c(%esp),%eax` |

- **Argument widths.** The parent id is passed in a dword slot but the callee reads only `%di`, and for the inherited
  flags it masks the dword with `and $0xffff,%eax` before indexing the parent slot. The runtime capture shows the
  upper half is genuinely undefined: 125 of the 279 recorded parent words have non-zero high bits
  (for example id 44 has `parentRaw 0x00540002` for parent 2, id 147 has `0x03E50008` for parent 8), which is exactly
  why the retail masks. The window is `0x0040194F-0x00401976`; `0x00401954` stores the parent word before the jump.
- **The 0x0181 sentinel.** `0x181` is one past the last allocation id (`0x180` = 384 slots, ids 0-383), used as "no
  parent". The capture has exactly two such registrations: id 0 `QTask` and id 121 `GFXCapsViewer`.
- **Identifier assignment.** `ebp` starts at 0 (`0x00401901 xor %ebp,%ebp`), is incremented once per occupied slot
  (`0x00401910 inc %ebp`), and stops at the first slot whose `+0x25` in-use byte is zero (`0x00401908 cmpb $0x0,(%eax)`).
  The id is therefore the lowest free slot index, handed out in registration order. **The callee stores the id
  nowhere**: it returns it in `AX` (`mov %bp,%ax` at `0x004019E5` and `0x00401A0B`), and each caller stores it in its
  own per-class id word immediately after the call (for example `0x0042DD1B mov %ax,0x0053BDA8` for the Car class and
  `0x004D9A1B mov %ax,0x00548648` for the MagicObj class). The capture confirms the contiguous result: ids 0..278,
  279 registrations, no gap.
- **Slot bound and error path.** The scan compares the slot address against `0x00AFA705` (`0x00401911`), i.e.
  `(0x00AFA705 - 0x00AF5F05) / 0x30` = 384 slots. On exhaustion `0x00401918-0x00401925` pushes the literal at
  `0x005361C4` ("Unable to allocate task type"), calls `0x004AF7B0` and spins in `jmp $`; it never returns.
- **Return width actually read by callers.** The retail return is 16-bit: only `AX` is written. Census of all 279 call
  sites: 279/279 read the result as a 16-bit word (`mov %ax,<class-word>`), 0 sites store `%eax`. 214 of those sites
  also forward the just-returned `EAX` as a dword stack slot to the next call (205 push-then-store, 9 store-then-push):
  `0x00401400` (172 sites), `0x00401530` (34), `0x00401FA0` (6), `0x00401810` (1), `0x004017C0` (1). Those consumers
  read only the low half: `0x00401400` reads its class id at `0x00401429` and masks it with `and $0xffff,%eax` at
  `0x00401430` (and its phase word with `and $0xff` at `0x00401417`), and `0x00401530` has the same shape. The
  candidate's `std::uint16_t` return (zero-extended `EAX`) therefore differs from the retail only in `EAX[31:16]`, a
  half no caller reads.

### 9.3 The two row branches

Both branches write the handler table `QTask_HandlerTable` at `0x00A96AE0` with row stride `0x180` entries (`0x600`
bytes) and `0xFE` (254) iterations, and both end with their own `call 0x004014B0`, `pop %edi`, `mov %bp,%ax`,
`pop %esi`, `pop %ebp`, `ret`:

- **copy branch**, `0x004019B4-0x004019DD`, taken when the parent id is not `0x0181`: destination
  `lea 0xa96ae0(,%ebp,4)` (new class), source `lea 0xa96ae0(,%edi,4)` (parent), `mov (%eax),%esi` /
  `mov %esi,(%ecx)`, `add $0x600` on both pointers, `dec %edx` / `jne`. The branch's own refresh call is
  `0x004019DF`.
- **zero branch**, `0x004019EB-0x00401A03`, taken for the `0x0181` parent: destination `lea 0xa96ae0(,%ebp,4)`,
  `movl $0x0,(%eax)`, `add $0x600`, `dec %ecx` / `jne`. Its refresh call is `0x00401A05`.

The 0x0181 test is re-executed at `0x004019AD`, after the task-list branch, exactly as the retail does; the candidate
keeps the two loops separate and keeps one refresh call in each arm, so the two retail call sites exist in the source
(and in the compiled object: two `call` relocations at offsets `0xbf` and `0x11e` of the candidate). The row layout
(`entry(class,phase) = 0x00A96AE0 + class*4 + phase*0x600`) is proven independently in §5 item 5 from `0x00401900`
itself and the `0x00401900`-adjacent copy loop; this section only records that the transcription preserves it.

### 9.4 `movb $0x0,0x00AF5F04(%esi)` at 0x004019A6: what the byte is, and whether the path is real

`%esi` is the selected slot (`0xAF5EE0 + id*0x30`), so the store writes **slot + 0x24**, the retail "class task list
initialized" byte (`has_task_list` in `TaskTypeSlot`). `0x0040199D` writes 1 to the same byte on the task-list path,
and it has two retail readers. `0x00401A20` (`0x00401A30 mov 0xaf5f04(%esi),%al; test %al,%al; je`) frees the
descriptor with `0x004B0B70(&slot+0x0C)` and then clears the in-use byte at `+0x25` (`0x00401A49`). `0x004012F0`
(`0x00401314 mov 0xaf5f04(%edi),%al; test %al,%al; je 0x00401362`; 189 bytes, 6 blocks, 7 edges, 61 instructions)
walks the descriptor at `+0x0C` (`+0x14` stride/`+0x18` count/`+0x10` element size) only when the byte is set. So the
byte answers "this slot owns a `+0x0C` task-list descriptor".

**The path is reachable and is the common one.** `0x00401988 je 0x004019A6` takes it whenever the fourth stack slot
is zero. The committed level-1 registration capture
(`docs/fidelity/evidence/0x00401900-retail-registration-order.json`, probe SHA-256
`d00697c36343f7c70c5ff2f03241529ca6ce12d639a6a44016fccee1a13b73a8`) records 279 registrations: 266 pass
`stackSlot4 = 0` and take `0x004019A6`; 13 pass a non-zero value and take the `0x004B0B00` path at `0x00401995`:
ids 24 `SoundDefGroup` 512, 25 `SoundDefSound` 512, 26 `SoundDefGraph` 512, 27 `SoundDefTriggerOnce` 512,
39 `FlashTask` 2, 44 `Particle2DDynCubeObjTask` 25, 114 `SimpointObjContainer` 32, 137 `Explosion0` 32,
138 `FireBall` 96, 139 `Fragment` 160, 140 `ExplosionSmoke` 64, 141 `FlashBang` 4, 147 `GunCasing` 50.

Neither the reader `0x00401A20` nor the free routine `0x004B0B70` has an engine transcription today, so the flag's
consumer side is still an open edge (not needed by 0x00401900 itself, which only produces the flag).

### 9.5 0x004014B0 and 0x004014E0 are already transcribed

`0x004014B0` is 43 bytes (`0x004014B0-0x004014DA`), 18 instructions, 5 basic blocks, 6 edges, one callee
(`0x004014E0`): it walks the 384 slots and, for every slot whose `+0x25` byte is set, stores
`0x004014E0(slot_index)` as a word at `+0x28`. `0x004014E0` is 75 bytes (`0x004014E0-0x0040152A`), 27 instructions,
5 blocks, 6 edges: it builds a 16-bit mask from the `0x0054FF24` phase map (bit *i* set when the class has a handler
for phase `0x0054FF24[i]`; `0xFF` in that map = absent) and sets bit 15. Both are committed:
`engine/src/retail/core/native_4014b0.cpp` (`TaskType_RefreshHandlerMask`) and `native_4014e0.cpp`
(`TaskType_HandlerPhaseMask`), listed at `CMakeLists.txt` lines 34-35. `0x004014B0` has four retail callers:
`0x0040148F`, `0x004015B9`, `0x004019DF`, `0x00401A05`. It is therefore not a pending unblock; the smallest
remaining piece of this path is not 0x4014B0.

### 9.6 Callers of 0x00401900: 279 sites

All `call 0x401900` sites in `build/retail-dis.txt`:

```
0x401130 0x401fd1 0x40646a 0x40648c 0x4072b9 0x40731d 0x40fe44 0x415081 0x415556 0x4170d0 0x4190fa 0x419771
0x419a18 0x41a1fa 0x41ab0e 0x41b32a 0x41b8fe 0x41c71a 0x41d09a 0x41dc6a 0x41e6aa 0x41f35a 0x42031a 0x420cda
0x42144a 0x42176a 0x4223fe 0x422b5a 0x4230b4 0x423fd1 0x424ba1 0x424e9a 0x425c9a 0x426644 0x426da4 0x4272b4
0x427844 0x427da4 0x42800a 0x42acda 0x42bec4 0x42d2c4 0x42dd0e 0x42ff14 0x43110e 0x43310a 0x4351f4 0x435884
0x435da4 0x4361e4 0x4367c2 0x436b24 0x43764f 0x4383a1 0x4388ca 0x438e0a 0x4397dc 0x4399c5 0x43a85a 0x43ad9d
0x43b30a 0x43b86a 0x43bfe4 0x43c6ea 0x43cc8d 0x43e504 0x43e8ea 0x43f00a 0x43f91a 0x4403f4 0x44072a 0x440c3a
0x440fea 0x44134c 0x4414a7 0x44369a 0x443eea 0x4440ef 0x444c0a 0x4464dc 0x447dde 0x4485fa 0x448d44 0x44937a
0x449984 0x449e0f 0x44c15c 0x44c8f1 0x4533c4 0x45357f 0x45498a 0x456aea 0x45989a 0x45caca 0x45db8a 0x45e478
0x464274 0x465a24 0x465bd4 0x4663e4 0x466e54 0x466f8a 0x467711 0x468323 0x4688c1 0x468e31 0x4696c4 0x46ab71
0x46b524 0x46b804 0x46ba74 0x46c0da 0x46cb8a 0x46d43a 0x46db54 0x46dd14 0x46df21 0x46e38e 0x46f2c4 0x470424
0x470624 0x470fb7 0x471069 0x472684 0x47272a 0x472a84 0x472d14 0x473404 0x4739e4 0x473eea 0x475394 0x475ada
0x476124 0x476744 0x477234 0x47738c 0x4774ae 0x477e4a 0x4781fe 0x47b4c4 0x47cfc0 0x47e720 0x4801c4 0x48021a
0x480305 0x4803a4 0x4813fa 0x4825d4 0x4851c1 0x48534a 0x485a54 0x486916 0x4872da 0x487bfa 0x48863a 0x48881a
0x488bdc 0x498acf 0x49a535 0x4a2a50 0x4a2a6f 0x4a2a8e 0x4a2aad 0x4c3465 0x4d7565 0x4d9774 0x4d9a14 0x4da1e5
0x4dcf34 0x4dcfbe 0x4dd172 0x4e0851 0x4e0964 0x4e41c4 0x4e5382 0x4e60a8 0x4e60ca 0x4e60ec 0x4e7391 0x4e7895
0x4e7c61 0x4e7da5 0x4e82d1 0x4e83a7 0x4e92d1 0x4e92f0 0x4e9491 0x4e9537 0x4ea3e7 0x4eb1e4 0x4ebc71 0x4ed522
0x4ed717 0x4ed736 0x4ed755 0x4ee424 0x4ee991 0x4ee9ae 0x4ee9cb 0x4ee9e8 0x4eea05 0x4f275a 0x4f3454 0x4f41a4
0x4f4b40 0x4f7380 0x4f8374 0x4faa7c 0x4fb1c4 0x4fcf65 0x4fe7e3 0x4fe87b 0x4fe8f6 0x4fe970 0x4fe9eb 0x502267
0x502289 0x503401 0x5034f1 0x503547 0x503d74 0x5043ca 0x504c6a 0x504e21 0x505668 0x505ca1 0x505f94 0x506fd4
0x507f55 0x508d84 0x508fba 0x51877a 0x518f44 0x519720 0x519774 0x51986a 0x51a1e5 0x51b154 0x51b411 0x51b974
0x51bada 0x51bfc4 0x51c2ca 0x51c2ec 0x51de0a 0x51de2c 0x51ecc4 0x51eefa 0x51f59e 0x51ff0a 0x5207ba 0x520bcb
0x520bea 0x52634b 0x526ef7 0x526f16 0x527cac 0x527cce 0x5284ba 0x52ab04 0x52ad2e 0x52b05a 0x52b3d7 0x52b6d4
0x52c0da 0x52c6c4 0x52c834 0x52ca44 0x52ca81 0x530234 0x530a94 0x530c95 0x5310f4 0x5311a4 0x531315 0x531336
0x5317e4 0x531f05 0x531fb5
```

They fan into 239 distinct enclosing functions (function starts derived from the set of call targets; a call target
inside a function would shift this, so treat 239 as an upper bound on distinct callers). Only 19 sites sit in 15
functions that already have an engine transcription (a VA mention in `engine/`):

0x401130 (in 0x4010d0 QTask_Init), 0x401fd1 (0x401fc0), 0x415556 (0x415530), 0x424ba1 (0x424b90),
0x4d9774 (0x4d9730), 0x4da1e5 (0x4da1d0), 0x4e5382 (0x4e5370), 0x4e7c61 (0x4e7c50), 0x4ebc71 (0x4ebc60),
0x4ed522 (0x4ed510), 0x4ee991/0x4ee9ae/0x4ee9cb/0x4ee9e8/0x4eea05 (0x4ee970), 0x51a1e5 (0x51a1d0),
0x530234 (0x530220), 0x530a94 (0x530a80), 0x5310f4 (0x5310e0)

The remaining 260 sites are in class-init functions that are not in the tree yet. The port does not depend on them:
start-up register the captured order through `TaskTypes_RegisterRetailClasses` (`task_types.cpp`), which replays the
279 captured records through `TaskType_Register` and fails on an id mismatch. The transcribed `QTask_Init` site is
`0x00401130` (the `"QTask"` registration with parent `0x0181`).

### 9.7 CFG numbers (radare2 6.2.0)

Measured with `r2 -q -e scr.color=0 -c 'aaa; aflj' <retail>` and cross-checked with `af @ <va>; afij @ <va>` for each
entry (the binary has no symbols, so `aa` alone does not create these functions; `aaa` does):

| VA | function | bytes | blocks | edges | instructions |
| --- | --- | --- | --- | --- | --- |
| 0x00401900 | `fcn.00401900` | 273 | 18 | 23 | 77 |
| 0x004014B0 | `fcn.004014b0` | 43 | 5 | 6 | 18 |
| 0x004014E0 | `fcn.004014e0` | 75 | 5 | 6 | 27 |
| 0x004AF7B0 | `fcn.004af7b0` | 89 | 5 | 5 | 29 |
| 0x004B0B00 | `fcn.004b0b00` | 101 | 5 | 7 | 41 |
| 0x004B0C60 (`Mem_Alloc`) | `fcn.004b0c60` | 172 | 8 | 11 | 63 |

An independent block walk of the listing agrees on 18 blocks for 0x00401900 and on 23 edges. The edge count is 23, not
24, because `0x00401925 jmp 0x401925` is a single self-edge; a hand count that treats the `jmp` instruction and its
target as two blocks double-counts one edge.

### 9.8 Link status: every symbol the file needs, and where it lives

| symbol | retail VA | kind | defined at | in the `igi` source list |
| --- | --- | --- | --- | --- |
| `TaskType_Register` | 0x00401900 | the native itself | `engine/src/retail/core/native_401900.cpp` (new, not in the build) **and** `engine/src/retail/core/task_types.cpp:362` (committed) | `task_types.cpp` at line 308; the new file is absent |
| `TaskType_RefreshHandlerMask` | 0x004014B0 | callee | `engine/src/retail/core/native_4014b0.cpp` | line 34 |
| `TaskType_HandlerPhaseMask` | 0x004014E0 | callee of the callee | `engine/src/retail/core/native_4014e0.cpp` | line 35 |
| `Debug_Error` | 0x004AF7B0 | callee (error path) | `engine/src/retail/core/retail_error.cpp:29` | line 291 |
| `Mem_AllocIndexedBlock` | 0x004B0B00 | callee (task-list path) | `engine/src/retail/mem/task_list.cpp:13` | line 323 |
| `Mem_Alloc` | 0x004B0C60 | callee of `Mem_AllocIndexedBlock` | `engine/src/retail/mem/mem_alloc.cpp` | line 360 |
| `String_VFormat`, `Error_GetGlobals` | 0x004A5702 etc. | callees of `Debug_Error` | `engine/src/retail/error/error_show.cpp` | line 315 |
| `QTask_TypeTable`, `QTask_HandlerTable`, `g_54ff24` | 0x00AF5EE0, 0x00A96AE0, 0x0054FF24 | data | `engine/src/retail/core/task_types.cpp` | line 308 |
| `"Unable to allocate task type"` | 0x005361C4 | string literal | in the file | - |

Evidence that the file needs nothing new:

1. Compiled objects: `native_401900.o` has exactly five undefined symbols: `_Debug_Error`,
   `_Mem_AllocIndexedBlock`, `_QTask_HandlerTable`, `_QTask_TypeTable`, `_TaskType_RefreshHandlerMask`.
2. Partial link of the five TUs that own them (`i686-w64-mingw32-ld -r` of `native_401900.o`,
   `task_types.o`, `retail_error.o`, `task_list.o`, `native_4014b0.o`, `native_4014e0.o`) fails with exactly one
   error, the expected duplicate: `multiple definition of 'TaskType_Register'`.
3. Same merge with a `/tmp` copy of `task_types.cpp` whose lines 346-427 were removed (the owner's deletion, done on
   a copy only): `ld -r` returns 0; the only remaining undefined symbols are the C++/CRT runtime plus
   `Mem_Alloc`, `String_VFormat`, `Error_GetGlobals`, `TaskEventTable_MarkSlot`, `TaskEventTable_MarkSlotNoMask` -
   all of which are own engine TUs in the `igi` source list.

So 0x00401900 links as soon as the move lands; there is no third-party or missing-native blocker. What remains for
0x00401900 is only the owner's atomic step and the manifest repoint.

**Syntax record.** The first version of the file used `NATIVE_DECL` after `#include "igi/retail/core/task_types.hpp"`,
whose final line is `#undef NATIVE_DECL`, so `clang++ -std=c++17 -fsyntax-only -Iengine/include` failed with
`expected ';' after top level declarator` at the `Debug_Error` declaration. Fixed by the local
MSVC/GCC `NATIVE_DECL` block after the includes plus `#undef NATIVE_DECL` at the end of the file. Current results:
`clang++ -std=c++17 -fsyntax-only -Iengine/include engine/src/retail/core/native_401900.cpp` rc=0 and
`i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra -Iengine/include -c ...` rc=0 with no diagnostics. The host-clang
command is 64-bit-host biased: the committed `task_types.cpp:386` fails the same command with
`cast from pointer to smaller type 'std::uint32_t' loses information`, so the file stores the name through
`std::uintptr_t` (identical 32-bit store on the x86 target, host-check clean).

### 9.9 The two callees reported missing are implemented: 0x004AF7B0 and 0x004B0B00

No new file was created for either: both addresses already have exactly one implementation in the `igi` source list,
and adding a second one would break the link (`multiple definition`) and violate the one-implementation-per-VA rule.
The records the task asked for are kept here instead.

**0x004AF7B0 `Debug_Error` (`engine/src/retail/core/retail_error.cpp:29`; 89 bytes, 5 blocks, 5 edges,
29 instructions, cdecl, 274 retail call sites).** Body: read the display callback `0x00936268`; when non-null,
`sprintf(0x00A84640, "Fatal error:\n\n")` (the literal at `0x00546DB8` is exactly `46 61 74 61 6c 20 65 72 72 6f
72 3a 0a 0a 00`), scan the buffer for its NUL with `repne scasb`, append the caller's `format` and varargs with
`String_VFormat` (`0x004A5702`) at the NUL, call the display callback with `0x00A84640`; then read the exit callback
`0x0093626C`: null -> return 0, otherwise `jmp eax` (the callback's result becomes the function's result). It is not
a leaf: callees `sprintf` (0x004A53B3, CRT), `String_VFormat` (0x004A5702) and two indirect calls through the two
globals. Its string handling and stack cleanup are transcribed in `retail_error.cpp` (the file's own provenance block
records the same bytes), and the literal/callback globals are byte-verified above.

**0x004B0B00 `Mem_AllocIndexedBlock` (`engine/src/retail/mem/task_list.cpp:13`; 101 bytes, 5 blocks, 7 edges,
41 instructions, cdecl, 4 arguments, 32 retail call sites).** Body: `alignment = max(arg4, 4)`
(0x004B0B00-0x004B0B0E); `stride = (element_size + alignment + 3) & ~(alignment - 1)`
(0x004B0B19-0x004B0B2A); `descriptor[5] = element_size` stored before the allocation (0x004B0B1D); `base =
Mem_Alloc((stride + 4) * count, alignment)` (0x004B0B2C-0x004B0B38, `imul` at 0x004B0B2F proves arg3 is the count);
`descriptor[0] = base`, `[1] = base + stride*count`, `[3] = count`, `[4] = stride`, `[2] = 0`
(0x004B0B3A-0x004B0B51, in exactly that store order) and the index table loop `index_table[i] = i` for `i < count`
(0x004B0B56-0x004B0B5F); returns 0. Not a leaf: its only callee is `Mem_Alloc` (0x004B0C60, 172 bytes, 8 blocks,
11 edges, 63 instructions), implemented at `engine/src/retail/mem/mem_alloc.cpp` (line 360). At 0x00401995 the
0x00401900 call site pushes `&slot+0x0C`, `object_size`, `task_list_count`, `0` and cleans `0x10`, which matches
`Mem_AllocIndexedBlock(descriptor, element_size, count, alignment)` argument for argument.

### 9.10 Candidate-vs-retail differences

- **Source order.** The candidate keeps the retail statement order (scan, error spin, `+0x25`/`+0x04`/`+0x00` stores,
  `+0x08` word, `+0x2C` branch, fourth-slot branch, 0x0181 re-test, two separate row loops, two refresh calls,
  `return` id). The compiled object reorders the three independent slot stores (in-use, size, name) - a compiler
  scheduling decision with no observable difference in a single-threaded retail flow.
- **Return upper half.** Retail writes `AX` only; the candidate's 16-bit return zero-extends `EAX`. No caller reads
  above `AX` (§9.2).
- **Name store form.** `static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(name))` is the same 32-bit store
  on the x86 target and keeps the file valid for a 64-bit-host syntax check.
- Everything else - constants `0x30`, `0x180`, `0xFE`, `0x600`, `0x181`, `0x20000000`, `0x54FF24`, `0x00A96AE0`,
  `0x00AF5EE0`, the two calls and the `jmp $` error spin - is the retail value at the retail instruction.

### 9.11 Remaining blockers and stale notes

1. **Owner's atomic move** for 0x00401900: delete `task_types.cpp` lines 346-427, add `native_401900.cpp` to
   `CMakeLists.txt` (next to `task_types.cpp`, line 308), repoint the manifest record's `implementation.path`, and
   update the new file's status. Until then the new file is not part of the `igi` target and the committed definition
   remains authoritative.
2. **Stale limit texts** (owner-owned, not edited): `task_types.hpp:132-135` and `native_00530a80.cpp:5` still say the
   0x004B0B00 task-list path and the parent handler-phase copy are not transcribed; both are in `task_types.cpp`
   now, and `@004019A6`/`@004019DF` are implemented in the candidate too.
3. **Consumer edge not in the tree**: `0x00401A20` (reads `+0x24`, frees through `0x004B0B70`), `0x004012F0`
   (reads `+0x24`, walks the task-list descriptor) and `0x004B0B70` have no engine transcription. That is downstream
   of 0x00401900, not needed to link it.
4. Unknowns from §8 still stand for this native: the meaning of `arg1` beyond "class-name pointer", and the
   `inherited_flags` bit semantics.

## 10. The teardown chain: 0x004B0B70, 0x00401A20, 0x004012F0, and reachability from 0x00402930

Registration now has its producer side (`TaskType_Register`, 0x00401900, committed in `native_401900.cpp` since
7ebdea29). This section records the teardown side. Two of the three VAs named for this lane were already implemented
in the tree, so only 0x00401A20 needed a new file; the measured facts and the link status are recorded here rather
than duplicated into new files.

### 10.1 0x004B0B70, the indexed-block release (already implemented)

`engine/src/retail/qfile/native_4b0b70.cpp`, `QFile_ReleaseBlock_4B0B70`, CMakeLists line 93. Retail: 24 bytes
(0x004B0B70-0x004B0B87), 9 instructions, 1 block, 0 edges; 33 direct call sites.

```
004b0b70 56                  push   %esi
004b0b71 8b 74 24 08         mov    0x8(%esp),%esi      ; argument = pointer to the descriptor
004b0b75 8b 06               mov    (%esi),%eax         ; descriptor + 0x00 = the indexed-block base
004b0b77 50                  push   %eax
004b0b78 e8 93 01 00 00      call   0x4b0d10            ; Mem_DeAllocFn, one argument
004b0b7d 83 c4 04            add    $0x4,%esp
004b0b80 c7 06 00 00 00 00   movl   $0x0,(%esi)         ; descriptor + 0x00 = 0
004b0b86 5e                  pop    %esi
004b0b87 c3                  ret
```

**ABI against the 0x004B0B00 descriptor.** One cdecl pointer argument, the address of descriptor[0] (retail
`&slot+0x0C`, exactly what 0x004B0B00 receives). It reads and clears **only descriptor[0]** (the pool base):

| descriptor word | slot offset | written by 0x4B0B00 | read by 0x4B0B70 | after the call |
| --- | --- | --- | --- | --- |
| [0] | +0x0C | base from Mem_Alloc | yes | 0 |
| [1] | +0x10 | base + stride*count (index table) | no | unchanged |
| [2] | +0x14 | 0 (used count, advanced by 0x4012F0) | no | unchanged |
| [3] | +0x18 | count | no | unchanged |
| [4] | +0x1C | stride | no | unchanged |
| [5] | +0x20 | element size | no | unchanged |

It does **not** clear the slot's +0x24 `has_task_list` flag and it does not clear +0x25; the caller (0x00401A20)
clears the in-use byte, and the flag stays set in retail. The release itself goes to `Mem_DeAllocFn` (0x004B0D10),
which is implemented (`engine/src/retail/mem/native_4b0d10.cpp`, CMakeLists 245) and whose own callees are implemented
(`HeapList_Remove` 0x004AF960 at `mem/heap_list.cpp`, CMakeLists 323; 0x004A6A50 at `mem/native_4a6a50.cpp`,
CMakeLists 244) - so the whole free path is in the build today.

The tree file matches the retail instruction for instruction (load, call with the loaded word, `add $0x4,%esp`,
store null, return). No engine file calls it yet; 0x00401A20 is its first transcribed caller.

### 10.2 0x00401A20, the slot teardown (transcribed in this lane)

`engine/src/retail/core/native_401a20.cpp`, `TaskType_Clear_401A20` (placeholder name; retail symbol unknown).
Retail: 50 bytes (0x00401A20-0x00401A51), 15 instructions, 3 blocks, 3 edges; 279 direct call sites, and that site
set is disjoint from 0x00401900's 279 sites (no shared address).

```
00401a20 8b 44 24 04         mov    0x4(%esp),%eax      ; dword slot; only the low word is used
00401a24 56                  push   %esi
00401a25 25 ff ff 00 00      and    $0xffff,%eax
00401a2a 8d 34 40            lea    (%eax,%eax,2),%esi
00401a2d c1 e6 04            shl    $0x4,%esi           ; esi = 0xAF5EE0 + id*0x30
00401a30 8a 86 04 5f af 00   mov    0xaf5f04(%esi),%al  ; slot + 0x24
00401a36 84 c0               test   %al,%al
00401a38 74 0f               je     0x401a49
00401a3a 8d 86 ec 5e af 00   lea    0xaf5eec(%esi),%eax ; &slot + 0x0C
00401a40 50                  push   %eax
00401a41 e8 2a f1 0a 00      call   0x4b0b70
00401a46 83 c4 04            add    $0x4,%esp
00401a49 c6 86 05 5f af 00 00 movb   $0x0,0xaf5f05(%esi) ; clear slot + 0x25 in-use
00401a50 5e                  pop    %esi
00401a51 c3                  ret
```

Order is the point: the free at 0x00401A41 runs **before** the in-use clear at 0x00401A49, and the clear is on both
paths (the `je` targets 0x00401A49, after the call). The +0x24 flag is tested but never written here; a teardown
leaves the flag set and the in-use byte clear, and the next 0x00401900 registration on that slot rewrites both.
The 279 callers are the class teardown sites (the first eight enclosing functions: 0x004011F0, 0x004027E0,
0x00406D90 (two sites each of 0x00406DAF/0x00406DBB), 0x00407D00 (0x00407D33/0x00407D3F), ...), i.e. the paired
counterpart of the 279 registrations.

Candidate checks: host `clang++ -std=c++17 -fsyntax-only` rc=0; `i686-w64-mingw32-g++ -std=c++20 -O2 -Wall -Wextra -c`
rc=0. The object needs only `QFile_ReleaseBlock_4B0B70` and `QTask_TypeTable`, both defined in the igi source list,
so it links as soon as it is listed. It is not in CMakeLists (report-only lane).

### 10.3 0x004012F0, the class-object allocator (already implemented)

`engine/src/retail/input/input_port_process.cpp:223`, `QTask_AllocateInputPort`, CMakeLists line 322. Retail: 189 bytes
(0x004012F0-0x004013AC), 61 instructions, 6 blocks, 7 edges; 2 call sites (0x004012AC inside
`QTask_RegisterInputPortHandler` 0x004012A0, and 0x004013BC). Callees: 0x004B0D60 `Mem_FlagStackPop`
(`mem/native_4b0d60.cpp`, CMakeLists 363), `Mem_Alloc` 0x004B0C60 (`mem/mem_alloc.cpp`, CMakeLists 360), 0x004B0D90
`Mem_FlagStackPush` (`mem/native_4b0d90.cpp`, CMakeLists 364). Every callee exists in the build, so the file links
today.

Its two branches are the ones this lane's 0x401900 record named: with slot `+0x24` set and `+0x14 < +0x18` it takes an
index from the slot's `+0x10` index table, multiplies by the `+0x1C` stride, adds the `+0x0C` base and stores the used
index in the element trailer at `element + element_size` (0x0040134E) - element_size is the descriptor[5] word that
0x004B0B00 stored, used as an offset, not a base pointer; the earlier "unsettled" note describes this trailer store.
Otherwise it pops the flag stack with 1 (0x004B0D60), allocates `object_size` with alignment 4, pushes the flag stack
back (0x004B0D90), and writes the object header in retail order +4, +0, +0x14, +0x1E, +0x1C, +0x18, +8, +0xC, +0x10.
Transcribed, not runtime-observed.

### 10.4 Can any of the 279 captured registrations be reached statically from 0x00402930?

**Yes - 157 of the 279, at depth 2, and the chain is byte-verified for 0x0042DCF0.**

Facts about the sequence itself: 0x00402930 (`QTask_RegisterClasses`) is called only from 0x00401011; its body is
0x00402930-0x00402C32, **155 consecutive `call` instructions with no `ret` in between** (the only non-call
instruction in the range is the tail `jmp 0x004CEF30` at 0x00402C36). So every one of those 155 callees runs during
that one function body, in that order.

Fully byte-verified chain for the `Car` registration:

```
00402b83  e8 68 b1 02 00      call   0x42dcf0                 ; inside 0x00402930's straight-line body
0042dcf0  b8 d8 2e 00 00      mov    $0x2ed8,%eax
0042dcf5  e8 a6 77 07 00      call   0x4a54a0
0042dcfa  6a 00               push   $0x0
0042dcfc  6a 00               push   $0x0
0042dcfe  e8 ed 68 0c 00      call   0x4f45f0                 ; parent id getter, returns the raw dword
0042dd03  50                  push   %eax
0042dd04  68 d8 2e 00 00      push   $0x2ed8                 ; object size
0042dd09  68 28 be 53 00      push   $0x53be28               ; "Car"
0042dd0e  e8 ed 3b fd ff      call   0x401900                 ; TaskType_Register
0042dd13  68 60 e1 42 00      push   $0x42e160
0042dd18  6a 00               push   $0x0
0042dd1a  50                  push   %eax
0042dd1b  66 a3 a8 bd 53 00   mov    %ax,0x53bda8            ; the class id word
```

The capture row this produces is `{classId 245, name "Car", namePointer 0x53BE28, objectSize 11992 (0x2ED8),
parentRaw 0x420058}`: same name pointer and same size as the instructions, and the parentRaw low word 0x0058 is the
parent id (the high half is the garbage the retail masks away). So registration id 245 is statically reachable from
0x00402930 by direct calls only.

For the other sites, the same two-hop test was applied mechanically: for each of the 279 call sites, its enclosing
function start was taken from radare2's function list (entries with an implausible >0x4000 size excluded, then the
greatest start <= site) and checked for membership in 0x00402930's 155 direct callees. **157 of 279 sites** pass, so
157 registrations are reachable at depth 2. The remaining 122 are not covered by this test; they are either deeper
(each direct callee has its own call tree) or called from the later startup segment 0x00402C40-0x00402F50, and this
report does not claim them. Indirect calls are not followed by the test, and the enclosing-function attribution for
sites in functions that radare2 split or merged is a stated limitation. What the byte evidence does establish is the
Car chain above and 154 other call sites of 0x00402930 whose enclosing functions each contain at least one 0x00401900
call site.

### 10.5 Owner actions and status

1. `native_401a20.cpp` needs to be listed in the `igi` source list (`CMakeLists.txt`, with the other core natives) by
   the owner; it links today once listed (only `QFile_ReleaseBlock_4B0B70` and `QTask_TypeTable` are needed, both
   defined by names already in the list). No manifest record exists for 0x00401A20 yet.
2. `QFile_ReleaseBlock_4B0B70` lives under `qfile/` although its 33 call sites are generic block releases and its
   companion is `mem/task_list.cpp`; that is a naming/domain review item for the owner, not changed here.
3. Remaining untranscribed nodes of this chain after 0x00401A20: none for the release path itself. The 279 teardown
   *callers* are class teardown functions, most of them untranscribed, which is why the chain is static-only.

## 11. The 0x00402C40 pass, the shutdown gap, and the per-class teardown wave

### 11.1 0x00402C40: the global task-class teardown pass reached by tail call from 0x004010B0

`radare2` `axt 0x00402C40` finds no xref, and the committed call-sequence table lists the routine as a second,
unrecorded 154-call sequence with no caller. That is a **tail-jump** blind spot, not a missing reference. The
instruction exists in the linear listing:

```
004010c7  e9 74 1b 00 00      jmp    0x00402c40        ; target = 0x004010CC + 0x1B74
```

It sits at the end of the routine at `0x004010B0`, whose whole body is:

```
004010b0  e8 ab 09 00 00      call   0x00401a60       ; 0x00401A60 = `mov 0x567C58,%eax; ret`
004010b5  85 c0               test   %eax,%eax        ; 0x00567C58 = the live task counter (0x004012F0 increments it)
004010b7  74 0e               je     0x004010c7
004010b9  e8 c2 17 00 00      call   0x00402880       ; current-task getter (the getter 0x00401050 also uses)
004010be  50                  push   %eax
004010bf  e8 2c 05 00 00      call   0x004015f0       ; 120 call sites
004010c4  83 c4 04            add    $0x4,%esp
004010c7  e9 74 1b 00 00      jmp    0x00402c40       ; both branches reach the tail call
```

`0x004010B0` has exactly one caller, in WinMain: `0x0048F9D4  e8 d7 16 f7 ff  call 0x004010B0`. The full chain is
therefore `WinMain 0x0048F9D4 -> 0x004010B0 -> tail jmp 0x00402C40`, and the first callees of the 154-call body
(`0x004192B0`, `0x004199B0`, ...) each call `0x00401A20`, the type-slot teardown transcribed in section 10.2.

**Verdict (recorded without a name, as instructed):** `0x00402C40` is the **global task-class teardown pass reached
by tail call from 0x004010B0**. It is not a companion register pass - the register pass is `0x00402930`, which ends
at `0x00402C3B`. The routine in front of it runs from WinMain's shutdown path: when the live task counter is
non-zero it destroys the current task first, then falls into the teardown pass unconditionally. Because the pass
runs on shutdown and its entries call `0x00401A20` (which frees the indexed block and clears the in-use byte), its
identity as a teardown pass is established by the callee bodies, not by its position after `0x00402930`.

Consequence for the work order: `0x00402C40`'s 145 absent callees are not part of the startup path the candidate
stops at; they are shutdown-path work and can be deferred behind the startup chain, except where a startup callee
shares a dependency with them (the `0x00419xxx` group in 11.3 does not).

### 11.2 Shutdown-path gap in `engine/src/retail/core/main.cpp` (tracked finding, not changed)

Retail's WinMain calls `0x004010B0` at `0x0048F9D4`, between the `push $0x1; movb $0x0,0x5C8DE8; call *0x5331B8`
sequence and the `push $0x0; call *0x5331A0` tail of the shutdown path:

```
0048f9c5  6a 01               push   $0x1
0048f9c7  c6 05 e8 8d 5c 00 00   movb   $0x0,0x5c8de8
0048f9ce  ff 15 b8 31 53 00   call   *0x5331b8
0048f9d4  e8 d7 16 f7 ff      call   0x4010b0        <-- missing from the port
0048f9d9  6a 00               push   $0x0
0048f9db  ff 15 a0 31 53 00   call   *0x5331a0
```

The port's `main.cpp` has no reference to `0x004010B0` (grep for `4010b0`/`0x4010B0` is empty) and its shutdown
goes directly `SetForegroundWindow`, `UnregisterClassA("Innerloop")`, `CoUninitialize`, `ReleaseMutex`, return.
So the candidate currently **skips the retail task-system teardown pass**. This is a fidelity gap, recorded here;
the port is not changed because the increment needs its own evidence and `0x004010B0`, `0x00401A60` and the 145
teardown entries are not all transcribed.

### 11.3 Pattern verdict: the 0x00419xxx - 0x00420xxx group (same shape, distinct natives)

Instruction-equality basis (bytes read from `build/retail-dis.txt`; every literal byte verified, not inferred from
the size):

| VA | size / instrs | resource global | class-id word |
| --- | --- | --- | --- |
| `0x004192B0` | 37 B / 9 | `0x0057BAF4` | `0x00539ADC` |
| `0x0041A1A0` | 37 B / 9 | `0x0057BB88` | `0x00539C28` |
| `0x0041AAC0` | 37 B / 9 | `0x0057BB90` | `0x00539C40` |
| `0x0041B260` | 37 B / 9 | `0x0057BB98` | `0x0053B580` |
| `0x0041B840` | 37 B / 9 | `0x0057BBA8` | `0x0053B58C` |
| `0x004199B0` | 25 B / 6 | -(no release call) | `0x00539AF0` |

Common shape of the five 37-byte entries (only the two literals differ; the instruction sequence and the
`add $0x8,%esp` / `add $0x4,%esp` cleanup differ because of the missing middle call):

```
mov     <resource global>,%eax      ; per-class resource object pointer
push    %eax
call    0x004C1830                  ; release the resource object
mov     <class-id word>,%cx         ; the 16-bit class id 0x00401900 stored
push    %ecx
call    0x00401A20                  ; free the type slot and clear its in-use byte
add     $0x8,%esp
movw    $0x181,<class-id word>      ; reset the class id word to "no parent" (0x181)
ret
```

`0x004199B0` is the same tail without the resource release: `mov <id>,%ax; push; call 0x00401A20; add $0x4,%esp;
movw $0x181,<id>; ret` - 25 bytes, 6 instructions.

Verdict: one wave, one shared evidence base, but **distinct natives**: the differing `.data` literals are the
per-class identity, so they cannot be collapsed into a parameterised routine. Each gets its own file and its own
retail VA. `0x00402C40` calls the five 37-byte entries and `0x004199B0` in that order (its calls #2-#7), which is
consistent with a per-class teardown sweep; the class-name strings that identify which class each word belongs to
are in the same `.data` neighbourhood and can be recovered from the registrar's `push $0x0053xxxx` argument, as
section 11.4 does for the `BackgroundFX` wave.

### 11.4 The handler frontier (next wave)

None of the 132 absent `0x00402930` callees can link today: each body pushes at least one code-address immediate
(handler/callback) that has no implementation, and several call absent natives (`0x004F1DB0`, `0x004E68D0`,
`0x005183D0`, `0x004CEBA0`, `0x004F1CB0`, `0x00413DB0`/`DC0`/`DD0`, ...). The frontier is therefore the handler
bodies, and the first clean closure found is the `BackgroundFX` class registered by `0x00419760`:

- `0x00419760` registers `"BackgroundFX"` (`0x00539C18`, object size `0x24`) and links `0x00419800` (event 1),
  `0x00419820` (event returned by `0x004E7530`), `0x004D3710` (event 2) and `0x004197F0` (phase 0).
- `0x004197F0` (12 B, 3 instructions) and `0x00419800` (17 B, 5 instructions) have no callees and no code
  immediates: they are landable today.
- `0x00419820` (62 B, 20 instructions) calls `0x00419860`; `0x00419860` (238 B, 64 instructions) calls
  `0x004192E0` (67 B, 22 instructions) and `0x00419950`; `0x004192E0` is straight-line x87 maths with no callee.
  The wave is `0x004197F0`, `0x00419800`, `0x004192E0` first, then `0x00419950`/`0x00419860`/`0x00419820`, then
  `0x00419760` itself on top.

`0x00415070` (`"IntroScreen"`, 0x005394CC) is the other named wrapper, but its three handlers are not clean:
`0x00415380` needs `0x00490D10` and `0x004903B0`, `0x00415450` needs `0x004B63D0` and `0x004903B0`, none of which
exist yet, so that wave is blocked behind the config/weapon natives.

### 11.5 Where the BackgroundFX wave bottoms out: the 0x00A94E84 backend dispatch

The three renderer entries that `0x00419950` drives are not self-contained handlers; each one ends in an indirect call
through the retail render-backend table:

| VA | size / instrs / blocks / edges | indirect call site | index source |
| --- | --- | --- | --- |
| `0x00419330` | 520 B / 144 / 7 / 11 | `0x0041948E`, `0x0041951D` (`call *0xa94e84(,%eax,4)`) | `0x00A84A54` |
| `0x00419540` | 230 B / 77 / 6 / 8 | `0x00419609` (`call *0xa94e84(,%edx,4)`) | `0x00A84A54` |
| `0x00419630` | 290 B / 90 / 7 / 9 | `0x0041973D` (`call *0xa94e84(,%eax,4)`) | `0x00A84A54` |

Data facts established from the image and the listing:

- `0x00A94E84` is the render-backend callback table. The port already models the retail table, but only as port-owned
  storage with internal linkage: `ModelBackendInitFn g_a94e84[0x10]{}` inside the anonymous namespace of
  `engine/src/retail/dynobj/local_models_mef.cpp:19`, reachable only through `Model_SetBackendInit` (there is no getter,
  and an internal-linkage object has no external symbol), so a transcription in `core/` cannot reference it today.
- `0x00A84A54` is **never written anywhere in the image**: the full listing has twenty reference sites and every one is a
  load (`0x00413359`, `0x004133E2`, `0x00413418`, `0x0041344D`, `0x00419458`, `0x004194E7`, `0x004195B8`, `0x00419712`,
  `0x00419ECA`, `0x00419F1C`, `0x0041A06B`, `0x0041D8EE`, ...). The PE `.data` section has VirtualSize `0x6FC558` at
  `0x00536000` but only `0x1A000` bytes of raw data, so `0x00A84A50`-`0x00A84A58` are zero-filled at load: the index is
  a constant 0 (`00 00 00 00`) for every execution. `0x00A84A50`/`0x00A84A58` are written (zeroed) from `0x00419832`/
  `0x00419839` and from later sites (`0x00419D8E`/`0x00419D94`, `0x0041D996`/`0x0041D99C`, `0x00421E13`/`0x00421E27`).
- `0x00419820` therefore owns the only definitions of `g_a84a50` and `g_a84a58` (lowest retail write address), and the
  wave's remaining files must declare, not redefine, them.

Consequence: `0x00419760` cannot be linked end to end until the `0x00A94E84` table has a declaration that `core/` can
include, or until the three renderer entries are deferred as a render-device increment. This is a tree-level decision
(the table's storage lives in `dynobj/`), recorded here as the wave's blocker rather than worked around with a helper.

### 11.6 BackgroundFX wave: files written and the exact unresolved set

Written so far (all `engine/src/retail/core/`, none in `CMakeLists.txt`, all compile host + i686 `-O2 -Wall -Wextra` with no
warnings after the fix noted below):

| file | native | bytes / instrs / blocks / edges |
| --- | --- | --- |
| `native_4197f0.cpp` | `BackgroundFX_ZeroField20_4197F0` | 12 / 3 / 1 / 0 |
| `native_419800.cpp` | `BackgroundFX_AdvanceField20_419800` | 17 / 5 / 1 / 0 |
| `native_4192e0.cpp` | `BackgroundFX_BuildRotation_4192E0` | 67 / 22 / 1 / 0 |
| `native_419950.cpp` | `BackgroundFX_EmitRecord_419950` | 89 / 42 / 3 / 4 |
| `native_419820.cpp` | `BackgroundFX_UpdateRecord_419820` | 62 / 20 / 1 / 0 |
| `native_419860.cpp` | `BackgroundFX_BuildAxes_419860` | 238 / 64 / 1 / 0 |
| `native_419760.cpp` | `BackgroundFX_Init_419760` (registrar, wave target) | 137 / 38 / 1 / 0 |

Written afterwards, resolving the §11.5 dispatch blocker (detail in §11.7): `native_419540.cpp` (`0x00419540`),
`native_419630.cpp` (`0x00419630`), `native_419330.cpp` (`0x00419330`). The wave is ten files; §11.7 carries the final
symbol state and supersedes the unresolved-symbol list below (which is kept as the state before those three and the data
block existed).

Unresolved symbols of the seven written files, measured with `i686-w64-mingw32-nm` over their objects (undefined minus
wave-defined; the three `cosf`/`sincosf`/`sinf` entries resolve from the target libm):

```
_BackgroundFX_BuildRecord_419540   <- 0x00419540, not written (dispatch blocker)
_BackgroundFX_EmitQuad_419330      <- 0x00419330, not written (dispatch blocker)
_BackgroundFX_TransformEntry_419630<- 0x00419630, not written (dispatch blocker)
_g_539be8, _g_539bf8, _g_539c08    <- static draw records, data increment (below)
_Core_GetEventWord_4E7530, _QTask_GetCurrentType, _QTask_LinkQTaskEventHandler,
_QTask_LinkQTaskEventHandlerNoContextFn, _QTask_NoOpHandler, _Script_PhaseIdGetE5_4F1A70, _TaskType_Register
                                   <- already transcribed in the tree (core/native_4e7530.cpp, core/task_types.cpp,
                                      script/native_4d3710.cpp, script/native_4f1a70.cpp, core/native_401900.cpp)
```

The static data the wave still needs is the block `0x00539AF0`-`0x00539C14`, all file-backed `.data` in the retail image:

- `0x00539AF0` = `0x00000181` (class id word, defined by `native_419760.cpp`; nearest later writer `0x004199B0`).
- `0x00539AF8` = a draw list `{ count = 5, entries[5] of 4 floats }` with the entries at `0x00539AFC` (5 × 0x10 bytes,
  read in order: {0, 70.0, 0, 300.0}, {5.0, 70.0, 5.0, 300.0}, {0, -80.0, 0, -300.0}, {90.0, 0, 300.0, 0},
  {-65.0, 65.0, -190.0, 190.0}); `0x00539B50` is the same shape `{ count = 5, entries at 0x00539B54 }`.
- `0x00539BA8` = two 0xC-byte entries `{0, 0, 100.0}`, `{28.0, -28.0, 60.0}`; `0x00539BC0` = one 0xC-byte entry
  `{0, -100.0, 200.0}`.
- `0x00539BD0` = `{20, 20, 0, 0, 300.0, 300.0, ...}` (`0x00419330`'s quad argument).
- the three records `{+0x00 pointer, +0x04 count, +0x08 array base, +0x0C tail}`: `0x00539BE8 {0, 0, 0, 0x00539BD0}`,
  `0x00539BF8 {0x00539AF8, 2, 0x00539BA8, 0}`, `0x00539C08 {0x00539B50, 1, 0x00539BC0, 0}`.

That data block was carried out as the independent increment it was described as: it needed no decision on the
`0x00A94E84` table (which is dispatched in retail regardless of runtime value) and it removed the three data symbols.
`native_419860.cpp` now defines all eight objects as typed objects with real pointers; the layout table is in §11.7.

One correction recorded: `native_419820.cpp` was first written with a `(void*, float)` signature copied from the two
`0x00419860` calls it makes. Retail's consumer of that second word is the body itself (it overwrites `[esp+8]` before
reading it), and `0x00419760` links the routine as a one-argument event handler, so the file now takes `void*` only and
the two `0x00419860` calls receive the computed floats directly. That removed the only `-Wcast-function-type` warning in
the wave.

### 11.7 The core three and the static data block: transcribed

The three entries §11.5 identified as the dispatch blocker are now written, and the static data block they consume is written
as typed objects. All ten wave files compile host and i686 `-O2 -Wall -Wextra` with no warnings, and the ten objects link
cleanly under `i686-w64-mingw32-ld -r` (partial link; every remaining undefined symbol is either tree-resident or a target
libm function).

| file | native (placeholder name, §10.14 #12) | bytes / instrs / blocks / edges | indirect dispatch |
| --- | --- | --- | --- |
| `native_419540.cpp` | `BackgroundFX_EmitSegments_419540` | 230 / 77 / 6 / 8 | 1 site, `0x00419609` |
| `native_419630.cpp` | `BackgroundFX_EmitFan_419630` | 290 / 90 / 7 / 9 | 1 site, `0x0041973D` |
| `native_419330.cpp` | `BackgroundFX_EmitAxisRuns_419330` | 520 / 144 / 7 / 11 | 2 sites, `0x0041948E`, `0x0041951D` |

**Common structure.** Each entry allocates one stack buffer of 0x24-byte vertex records, writes the same seven constant tail
floats into every record it emits (`record[2..8] = 1.0f, 0, 0, 0.075f, 0, 1.0f, 0`; the pattern the prologue loops store at
`0x0041934D`/`0x0041955C`/`0x004196C3`), builds a five-dword primitive descriptor `{1, 0x00080000, 0, vertex_count,
&records[0]}` in place, and calls `0x00A94E84[0x00A84A54]` with a pointer to that descriptor. The descriptor is
a function-local struct with a real pointer member, replacing retail's stored address word; the records it points at are a
0x24-byte stride (9 floats) in retail, so the coordinates at `+0x00`/`+0x04` are followed by the constant tail.

- `0x00419540` (`sub $0x5c`): two records; for `i < list->count` it transforms `list+4+i*0x10` (four floats = two vertices)
  through the matrix at `matrix[0..5]` into `records[0][0..1]` and `records[1][0..1]`, then dispatches. The count is re-read
  from `[list]` after every dispatch (`0x00419610`); the back edge compares the incremented index (`0x0041961C`).
- `0x00419630` (`sub $0x5ec`): `count = clamp(round(circle[2]*context[6]*pi/10), max 0x28)`; `radius =
  (float)(circle[2]*context[6])`; per record `record[0] = radius*cos(angle)+origin_x`, `record[1] =
  radius*sin(angle)+origin_y`, `angle` from `0.0f` stepping `2*pi/count`; then `rep movsl` copies `record[0]` into
  `record[count]` (closing the fan) and the descriptor's vertex count is `count+1`. Single dispatch.
- `0x00419330` (`sub $0x84`): two runs around centre `(record[4], record[5])`; run 1 runs `while (++i <= run[0])`
  (count+1 dispatches) with the far end `+(centre_x*m[0], centre_x*m[1])` and the step `(centre_y*m[2],
  centre_y*m[3])*0.05f`; run 2 runs `while (++i <= run[1])` with the far end `+(centre_y*m[2], centre_x*m[1]*0.05f)` and the
  step `(centre_x*m[0]*0.05f, centre_y*m[3]*0.05f)`. Both runs re-read their count after every dispatch. Single descriptor,
  reused by both runs.

**Backend dispatch form.** The three bodies use the retail form directly as instructed: they include
`igi/retail/dynobj/local_models_mef.hpp` and call
`reinterpret_cast<igi::retail::dynobj::ModelBackendInitFn>(igi::retail::dynobj::g_a94e84[backend])(
reinterpret_cast<void**>(&primitive))` with `backend` loaded from a defined dword. No second `extern` for the table exists in
this wave (the header declaration from commit `ecaa4d36` is used; the symbol lives in `igi::retail::dynobj`, so the
`::g_a94e84` qualification is not valid). `0x00A84A54` is defined once, in `native_419330.cpp` (lowest retail load
`0x00419458`), as `std::uint32_t g_a84a54 = 0U;` with the retail-VA comment: retail never writes the dword (twenty load sites,
no store) and it sits in `.data`'s zero-fill tail (virtual size 0x6FC558 at `0x00536000`, raw size 0x1A000), so zero is the
image fact; the load is kept so the dependency stays visible instead of being folded into a literal 0.

**Data increment.** `0x00539AF0`-`0x00539C17` is transcribed in `native_419860.cpp` as typed objects with real pointers.
Every byte was read from the file-backed image at `0x00539AF0`-`0x00539C14` (the three records end at `0x00539C17`;
`0x00539C18` begins the class-name string already reproduced as a byte-identical literal in `native_419760.cpp`).

| retail | symbol | type (bytes) | image contents |
| --- | --- | --- | --- |
| 0x00539AF8 | `g_539af8` | `BackgroundFX_DrawList` (0x54) | count 5; {0,70,0,300} {5,70,5,300} {0,-80,0,-300} {90,0,300,0} {-65,65,-190,190} |
| 0x00539B50 | `g_539b50` | `BackgroundFX_DrawList` (0x54) | count 5; {-300,-100,300,-100} {-300,100,300,100} {-200,100,-200,-100} {200,100,200,-100} {0,-400,0,150} |
| 0x00539BA8 | `g_539ba8[2]` | `BackgroundFX_Circle` (0xC each) | {0,0,100} {28,-28,60} |
| 0x00539BC0 | `g_539bc0[1]` | `BackgroundFX_Circle` (0xC) | {0,-100,200} |
| 0x00539BD0 | `g_539bd0` | `BackgroundFX_AxisPair` (0x18) | {20,20,0,0,300.0f,300.0f} |
| 0x00539BE8 | `g_539be8` | `BackgroundFX_DrawRecord` (0x10) | {nullptr, 0, nullptr, &g_539bd0} |
| 0x00539BF8 | `g_539bf8` | `BackgroundFX_DrawRecord` (0x10) | {&g_539af8, 2, &g_539ba8[0], nullptr} |
| 0x00539C08 | `g_539c08` | `BackgroundFX_DrawRecord` (0x10) | {&g_539b50, 1, &g_539bc0[0], nullptr} |

Every value is exactly representable as a float, so no bit-pattern literal was needed. `0x00539AF0` (bytes `81 01`) stays
owned by `native_419760.cpp`. The consumers read these objects through raw int/float offsets (`0x00419330` reads `+0x00`,
`+0x04`, `+0x08`, `+0x0C`, `+0x10`, `+0x14`; `0x00419540` reads `+0x00` and the 0x10-stride array at `+0x04`; `0x00419630`
reads three floats), which is why the layouts are named in the owning file rather than shared through a header, and the core
entries continue to address them the way retail does.

**Callee-name correction.** `native_419950.cpp` first declared its three callees under placeholder names
(`BackgroundFX_BuildRecord_419540`, `BackgroundFX_TransformEntry_419630`, `BackgroundFX_EmitQuad_419330`); those declarations
and both call sites now use the names of the actual definitions above, so there is one implementation per VA (§10.15 #13) and
the partial link no longer reports three duplicate callee holes.

**Measured symbol state.** `i686-w64-mingw32-nm` over the ten objects (`-r` partial link, rc 0):

```
defined data (10 wave symbols + 1): _g_539af0 _g_539af8 _g_539b50 _g_539ba8 _g_539bc0 _g_539bd0
                                    _g_539be8 _g_539bf8 _g_539c08 _g_a84a50 _g_a84a54 _g_a84a58
undefined: _g_a94e84                      <- tree: dynobj/local_models_mef.cpp (commit ecaa4d36)
           _Core_GetEventWord_4E7530      <- tree: core/native_4e7530.cpp
           _QTask_GetCurrentType          <- tree: core/task_types.cpp
           _QTask_LinkQTaskEventHandler   <- tree: core/task_types.cpp
           _QTask_LinkQTaskEventHandlerNoContextFn <- tree: core/task_types.cpp
           _TaskType_Register             <- tree: core/task_types.cpp / core/native_401900.cpp
           _QTask_NoOpHandler             <- tree: script/native_4d3710.cpp
           _Script_PhaseIdGetE5_4F1A70    <- tree: script/native_4f1a70.cpp
           _cosf _sinf _sincosf           <- target libm (used by 0x004192E0 / 0x00419860 trig)
           _sincosl _llrintl              <- target libm (used by 0x00419630)
```

So the wave carries no unresolved *retail-domain* symbol. Direct calls are all implemented (the three new bodies make no
direct calls at all); the four indirect sites through `0x00A94E84` are counted separately above and are the only remaining
non-direct control transfer in the wave.

**Limits recorded with these files.**

- `fistpll` to `llrintl`: retail converts `circle[2]*context[6]*pi/10` with x87 `fistpll` (round-to-nearest per the FPU
  control word), stores a 64-bit integer and reads only its low dword (`0x00419689`). The body uses `std::llrintl` and keeps
  the low 32 bits of the result; a NaN/overflow case would differ (retail stores the integer-indefinite pattern), which is
  unobservable here because the argument is built from the static records and the transform context.
- Angle and step: retail keeps `2*pi/count`, the accumulated angle, and the radius/centre products in 80-bit x87 registers
  and rounds only at the `fstps` stores; the body therefore uses `long double` for both and casts once at each store.
  `std::cos`/`std::sin` on `long double` are not bit-identical to x87 `fcos`/`fsin` in the last bit, so **last-bit equality is
  not claimed** for `0x00419630` (the other two bodies do not use trig).
- Compiler fusion: GCC folds the two long-double calls into one `sincosl` import, where retail executes `fcos` and `fsin`
  separately. The mathematical functions are the same, but the import set differs (`_sincosl` instead of `_cosl`+`_sinl`) and
  last-bit equality is still not claimed; the symbol links from the target libm (verified by a throwaway link, rc 0).
- x87 register stack: on the `count > 0` path `0x00419630` leaves one value on the x87 stack at return (`0x00419719` pops
  only `ST(0)` of the two live values: the step and the final angle). A C++ transcription cannot reproduce the x87 register
  stack residue; it is unobservable through memory, the return value or any ABI-visible state, and it is recorded here rather
  than emulated.
- Status: `TRANSCRIBED` + `BUILD_VERIFIED` (host and i686 single-file compile, partial link). `STRUCTURALLY_VERIFIED` only in
  the r2 block/edge sense quoted in the table; **runtime UNOBSERVED** - the startup pass cannot reach these yet (`0x00419760`
  is table entry #153 of `0x00402930`, and the 132 absent callees of that chain are still outstanding), so no runtime status
  is claimed for any of the ten files.
