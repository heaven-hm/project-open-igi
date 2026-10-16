# Retail random generator

Reference executable SHA-256:
`dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`.

The implementation copies the initializer and shift/add recurrence exported in
`project-igi-natives/assets/BinaryNinja/natives/sys/004B4720-Sys_ResolveSymbol_13.c`
and `004B4740-Sys_CheckHeap_13.c`, using descriptive names and unsigned 32-bit
words. These exported semantic names are incorrect: the routines initialize
and draw from the random stream. The C# `IgiRandom` agrees on the recurrence;
its convenience single-seed initializer is not used.

REA/Ghidra 12.1.2 and live Binary Ninja MCP confirm initializer `0x4B4720`:
`C8FB1A07`, `39EA7756`, `7FF13CCD`. REA evidence:
`ev_eadd1ef1a3913eaa136b999fd383c8e013efebd3e0959dbd915e0809f5817813`.
For the float draw at `0x4B4770`, evidence:
`ev_f94a84130da1b07463e7a6116544e30f3e8ed5287d374531783eb167691aa72f`.
Independent radare2 instructions confirm logical SHR, wrapping ADD, and
`AND EAX, 0xFFFF` before multiplying by exactly `2^-16`. Binary Ninja's
current pseudo-C omits that mask, demonstrating why instruction checks are
required before using generated C directly.

`retail_random_generator_probe.cpp` executes private copies of the original
25-byte initializer, 38-byte word draw and 71-byte real draw under CrossOver.
It compares the C++ result and all three mutated words after each call.
Six states, including the original defaults, zero, all ones and high-bit cases,
each pass 65,536 paired draws: **393,216 word and float comparisons**. Float
results are compared by exact IEEE-754 bits. Only the float wrapper's absolute
constant-address operand is relocated to an identical double; every other
instruction byte is checked against the external retail executable. The first
attempt to reserve its original constant address failed; this is retained as
an address-layout limitation, not a successful run.

The native checkpoint test records the first eight original word/float results
and verifies restoration of captured stream state. To repeat original execution:

```sh
i686-w64-mingw32-g++ -std=c++20 -Wall -Wextra -Wpedantic -Werror -static \
  -I engine/include -Wl,--image-base,0x10000000,--disable-dynamicbase \
  engine/tools/retail_random_generator_probe.cpp -o /tmp/igi-retail-rng.exe
```

Run the tool in the existing CrossOver `project-igi` bottle with `igi.exe` as
the sole argument. Original code and original assets remain external.

This establishes the tested RNG primitive, not a full-game runtime trace.
Mission initialization, other consumers, draw ordering, and the stream state
at the first player shot still require naturally reached retail captures.
No catalog entry is promoted to full runtime-verified status from this test.
