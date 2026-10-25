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

Measured on `0x004F1A80` over 140 instructions: the only argument reads are `[esp+4]`, `[esp+8]`,
`[esp+0xC]`, `[esp+0x10]` and `[esp+0x20]` (the last one after the frame pushes, so it addresses an
earlier argument, not a sixth). There is no read of `[esp+0x14]`.

That matters for two reasons:

1. The registrars push a constant `7` as the fifth slot of every append, and `chain-worker` found
   a sixth pushed slot whose value the callee never reads. Both are therefore **dead arguments in
   retail**, not load-bearing parameters. A previous note in this project described "argument 5 is
   7 in all 40 appends" as a pattern; the pattern is about what the call sites push, and it does
   not imply the callee consumes it.
2. Extending the tree's five-parameter declaration of `0x004F1A80` to six is **not** supported by
   evidence. The correct transcription is what `chain-worker` did: keep the call, discard the
   result, and say in the comment that the slot is unread. Do not change the callee's arity.

## A trap worth naming

The dropped call was caught because the candidate emitted 76 calls against retail's 77. The
reasoning that produced the bug was "the callee never reads this value, so the call is not
needed" - which is false. Retail executes the call, so the call is part of the behaviour to
transcribe; whether the result is used is a separate question. A distinct-callee count would have
missed this entirely, because the dropped call was the only call to that target: only the **total
call count** moved. Per-target counts are the stronger check.
