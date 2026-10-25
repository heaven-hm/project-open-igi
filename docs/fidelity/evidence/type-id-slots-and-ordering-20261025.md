# Type-id slots, their initializer, and an open ordering question (2026-10-25)

Status: partially resolved. The writer exists; whether it runs before the registrars is open.

## The open item this answers

`chain-worker` reported: `0x004F1E20` reads `0x0054E158`, whose image value is `0xFFFFFFFF`, and
the result becomes the fourth argument of a container append - the exact value `0x004F1A80`
rejects with `Debug_Error("Undefined parameter type.")`. It asked who writes the slot rather than
guessing. Verified in the image:

```
0x004F1E20   mov eax, dword [0x54E158]     ; the getter that reads it
0x004F1E26   ret
...
0x004F1E7F   call 0x4F19C0                 ; register a type, returns an id
0x004F1E84   mov dword [0x54E158], eax     ; THE WRITER
```

So the slot is not left at `0xFFFFFFFF` by construction. The writer sits in a 104-byte,
26-instruction initializer at `0x004F1E35`, which also writes `0x0054E160` and `0x0054E150` from
the same `0x004F19C0` return value and passes `0x0054E158` to an append at `0x004F1EC4`. The
shape is: `0x004F19C0` registers a named type and returns its id; the initializer caches ids in
slots; registrars pass those cached ids to `0x004F1A80`.

## What is still open

The initializer is **not** in the startup chain: `0x00402930`'s 155 bare calls contain zero calls
into `0x004F1E*` (verified by listing the whole function). A byte search for `0x004F1E35` as an
immediate finds no reference either, so it is most likely reached through a jump table or another
indirect dispatch - the same pattern already recorded for the tag writers and the table at
`0x574ED4`. The ordering question therefore stands: if a registrar runs before this initializer,
its type-id argument is still `0xFFFFFFFF` and the append takes the `Debug_Error` path. Nothing in
this note claims an order; it records that the writer exists, where it is, and that its caller is
unidentified.

## A second finding: the append's later arguments are unread

**CORRECTED 2026-10-25.** This section previously claimed that `0x004F1A80` reads only four
arguments, that the constant `7` and a sixth pushed slot were dead, and that extending the
declaration to six was unsupported. All three claims were wrong, and the error is worth recording
because of how it was made.

The original measurement counted `[esp+0x20]` **once**. The body reads that textual offset
**twice**, and the two reads address different slots because a `pop edi` sits between them:

- `0x004F1AB1  mov bl, byte [esp+0x20]` - after `push ebx; push esi; push edi` (12 bytes pushed),
  so this is entry+`0x14` = **arg5**, read as a byte.
- `0x004F1ABD  pop edi` - 4 of those 12 bytes are removed.
- `0x004F1AC9  mov esi, dword [esp+0x20]` - the same textual offset is now entry+`0x18` = **arg6**,
  read as a dword.

Both are stored: `mov dword [edx+ecx+8], esi` at `0x004F1AD1` writes arg6 at record+`0x08`, and
`mov byte [edx+ecx+0xc], bl` at `0x004F1ADE` writes the byte of arg5 at record+`0x0C`. So the record
is `{arg3-arg2, arg4, arg6, byte(arg5)}`, the callee takes **six** arguments, and the tree's
implementation - which stored arg5 in both fields - wrote a wrong value at every one of its call
sites.

The call sites confirm the arity independently. At `0x00509136` the sequence is
`push $0x0; push $0x7; call 0x004F0F80` and then the append with a deferred `add esp,0x18`: six
words cleaned, and the two words pushed for the *preceding* call are reused as the append's arg5
(`7`) and arg6 (`0`). That is why the cleanup is `0x18` and not `0x14` - the arithmetic settles the
arity without reading the callee at all.

The generalisable lesson: **two reads of the same textual stack offset are not two reads of the
same slot unless the stack pointer is unchanged between them.** An argument-count claim has to be
made against a stack-state trace, not against a set of distinct offsets. This note made exactly
that mistake, and the claim survived in the tree as a wrong store until `chain-worker` re-derived
the body from the instructions.

## A trap worth naming

The dropped call was caught because the candidate emitted 76 calls against retail's 77. The
reasoning that produced the bug was "the callee never reads this value, so the call is not
needed" - which is false. Retail executes the call, so the call is part of the behaviour to
transcribe; whether the result is used is a separate question. A distinct-callee count would have
missed this entirely, because the dropped call was the only call to that target: only the **total
call count** moved. Per-target counts are the stronger check.
