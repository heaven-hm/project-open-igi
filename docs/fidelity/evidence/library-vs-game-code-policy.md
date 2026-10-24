# What to include and what to exclude: library code in a matching decompilation

Written after the 0x00401400 question, with external references, because the address-range heuristic this work had been using is
not sufficient and produced at least one wrong classification.

## Consensus from existing decompilation projects

- **Matching decompilations exclude the statically linked runtime and middleware, and record that exclusion.** meteor-decomp's plan
  states its non-goal as decompiling "MSSMIXER / Miles Sound System / DirectX wrapper glue / VC++ runtime / STL", and prefers
  functional re-derivation for the long tail rather than matching every byte.
  <https://www.garlemaldsoftware.com/projects/meteor-decomp/docs/plan/>
- **Library code is identified by signature, not by address.** RetDec ships YARA signature files for statically linked compiler
  runtimes precisely so that this code can be *removed* before decompiling, and documents that you must write your own signatures for
  other runtimes. <https://github.com/avast/retdec/wiki/Removing-Statically-Linked-Code>
- **IDA's FLIRT is the same idea**: library functions are recognised by byte/CRC signatures, can be named automatically, and can be
  hidden from the analyst. <https://docs.hex-rays.com/core/flirt/concepts/ida-f.l.i.r.t.-technology-in-depth>
- **HARD signals beat heuristics.** A production classifier for this problem triggers on canonical library symbols (mangled operators
  like `??2@YAPAXI@Z`, decorated CRT imports like `_strtoi64@4`, `std::_*`, SEH machinery) and treats region or caller-count as
  supporting evidence only. <https://github.com/bethington/ghidra-mcp/commit/4b866560>
- **Everything that is the game's own code is in scope**, including utilities that merely look generic; a N64 matching decompilation
  decomposes all code except handwritten assembly, which is kept as assembly.
  <https://github.com/zeldaret/tmc/blob/master/CONTRIBUTING.md>

## Policy for this project

1. **Exclude only what is provably the toolchain's**: the CRT/startup code the linker equivalent supplies, C++ runtime and STL
   machinery, SEH scaffolding, and third-party middleware. Evidence of exclusion must name the library function (or the runtime
   contract it implements), not merely its address.
2. **Include everything else**, including small utilities with hundreds of callers, and including code that sits at low addresses next
   to the CRT.
3. **A retail call site into excluded code is expressed as the candidate's equivalent runtime call**, with the same arguments and return
   value, and the substitution is recorded in the record's boundary note.
4. **No stubbing and no guessing**: if the callee's identity is not established, the caller stays blocked.
5. **Address ranges are a hint, never the evidence.** The 0x00401000-0x00402000 range was treated as CRT in earlier rounds; that was
   an inference, and the case below shows it was wrong for at least one function.

## Case study: 0x00401400 is game code, not CRT

Earlier rounds parked 0x00401400 as a "printf-family CRT internal" because it lives at 0x00401400 and writes through a table. The
evidence says otherwise:

- 1312 call sites across the image - more than any CRT internal would need, and consistent with a project-wide trace/log dispatcher.
- Frida observation at level-1 startup, caller 0x004C59A4: arguments `(2, 36, 0x004C5A10)` - a small category, a small id, and a
  pointer, not a printf format-and-values pattern.
- Its body increments counters 0x00567C6C and 0x00536148, writes the third argument into `0x00A96AE0 + (id*384 + (arg1 & 0xFFFF))*4`,
  sets a byte in 0x0054FF34, then walks 2048 records of 0x30 bytes from 0x00AF5EE8 to 0x00AFA6E8 and **recursively calls itself**
  for records whose category matches and whose byte is clear, before calling 0x004014B0.
- The table at 0x00AF5EE8 is game data, not CRT data; the recursion distributes one trace value over a category tree.

So 0x00401400 must be transcribed as its own native. The same re-examination is owed to 0x00401530 (623 callers), 0x00401900 (279),
0x004017C0 (150) and 0x004018F0 (118): the caller counts alone rule out CRT internals, and each needs the same evidence check before it
is either transcribed or excluded.

## Consequence for the boundary work

The 0x00402930 chain is gated on these game utilities, not on CRT. That is worse news than the earlier reading, and it is the honest
one: the boundary needs the utilities transcribed too, which is why it is 133 callees each with closures rather than 133 wrappers over
the C runtime.

## Next increment prepared: 0x00401400's shape (measured, not yet transcribed)

Extent 0x00401400-0x004014A4, 53 instructions, 0xA4 bytes. Everything below is read from the retail instructions; the transcription is
deliberately not attempted in this round because its storage extents are not yet proven.

- Arguments: `arg1` at [esp+4] (category/id word, used as `(arg1 & 0xFFFF) + (arg1 & 0xFF) * 384`), `arg2` at [esp+8] (a 16-bit
  selector compared against the word at each record), `arg3` at [esp+0xC] (a value stored into the table).
- Counters: 0x00567C6C incremented on entry and decremented at 0x0040149A; 0x00536148 incremented once per call.
- Stores: `arg3` into `0x00A96AE0 + index*4`; `(counter == 1)` byte into `0x0054FF34 + index`.
- Recursion: walks records of 0x30 bytes from 0x00AF5EE8 while the pointer is below 0x00AFA6E8 (2048 records). For a record whose
  byte at +0x1D is set, whose word at +0x00 equals `arg2`, and whose byte at `0x0054FF34 + index + i` is zero, it calls itself with
  `(i, arg1, arg3)`. Then it calls 0x004014B0 once.
- 0x004014B0 (0x2B bytes, 18 instructions) walks the same 2048 records from 0x00AF5F08 in 0x30 strides up to 0x00AFA708, and for each
  record whose byte at -0x03 is set calls 0x004014E0(record index) and stores the returned word at record+0x30. 0x004014E0 is already
  transcribed (this tree owns native_4014e0.cpp), so 0x004014B0 is gated only on 0x00401400 and its storage.
- Section arithmetic confirms the data is real, not unmapped: image base 0x00400000, .data covers RVA 0x00136000..0x00832558, so
  0x00A96AE0, 0x00AF5EE8 and 0x00AFA6E8 all fall inside .data at RVAs 0x00696AE0, 0x006F5EE8 and 0x006FA6E8.

Open questions that block a faithful transcription and must be answered from the image, not guessed:

1. the extent of the 0x00A96AE0 table (the index formula can reach 163359 dwords, which fits inside .data, but "it fits" is not proof of
   the declared size);
2. the record type at 0x00AF5EE8: which fields at +0x00, +0x1D and -0x03 are, and how a record's word relates to `arg2`;
3. whether 0x0054FF34 is one byte per index or a wider per-index record.

Only after those are established can 0x00401400 be transcribed without inventing structure. That is the next increment, and it is what
unblocks 0x0042DAC0, 0x0042FE40 and every other registration routine that reports through it.
