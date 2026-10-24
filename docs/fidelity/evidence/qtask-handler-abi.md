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

