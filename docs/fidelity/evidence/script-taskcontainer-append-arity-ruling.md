# Ruling: `Script_TaskContainer_Append` (0x004F1A80) takes SIX arguments

## The conflict

`script/native_4f1a80.cpp` declares it with **six** parameters. Every site inside
`0x004F8340` pushes **four**, and `add esp,0x18` accounts for 6 dwords = 2 getter pushes +
4 append pushes. Two transcriptions therefore disagree.

## The measurement

The callee's own prologue was read. After its two register saves it reads:

```
[esp+0xC]    = arg1
[esp+0x10]   = arg2
[esp+0x20]   = arg6      (0x20 - 0xC = 0x14 = 20 bytes = 5 slots past arg1)
```

**The callee genuinely reads arg6.** So the six-parameter declaration is correct and a
four-parameter declaration is not: it would drop an argument the routine actually consumes.

## The ruling

**The callee keeps six parameters.** The call site is not "wrong" either - retail really does
push only four there, and the callee's arg5/arg6 read whatever occupies those stack slots.

The resolution is the **deferred-push rule applied from the callee's side**: the values arg5 and
arg6 receive are the caller's own earlier pushes, still live because the caller defers cleanup to
a single `add esp,N`. They are therefore **derivable from the caller's push sequence**, not
unknowable - and the call site must pass those derived values explicitly rather than omitting the
arguments or inventing zeros.

Passing `0` for an argument retail reads is not faithful. Omitting it is not faithful. Passing the
value retail's stack actually holds is.

## What this is not

It is not a licence to widen signatures wherever a push count disagrees with a callee's reads.
Each case is decided by reading the callee's prologue; the two known outcomes are:

- the callee reads it, and the caller's deferred pushes supply it -> pass the derived value;
- the callee does **not** read it (e.g. `0x004F1A60`/`0x004F1A70`, two-instruction byte getters
  with no `[esp+N]` access at all) -> the pushed value is genuinely ignored, and the callee's
  zero-parameter declaration is correct.
