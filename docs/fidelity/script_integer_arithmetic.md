# Script integer arithmetic correction

Date: 2026-10-16. Target: original `Downloads/IGI/igi.exe`, SHA-256
`dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`.

## Discrepancy

The reused C++ runtime used signed overflowing addition, subtraction,
multiplication and negation, and unmasked shift counts. These have undefined
behavior in C++, unlike the original x86 operations. A sanitizer regression
failed before correction on a negative shift exponent at interpreter line 395.
The C# vanilla candidate implements unchecked integer arithmetic and masked
C# shifts; its behavior was treated as a hypothesis and checked against retail.

## Original evidence

Local REA checkout: `/Users/haseeb-mir/Documents/Code/rea`.
Ghidra provider: 12.1.2. Independent disassembly: local radare2.

- Original handler registrar `0x004BD430` maps opcodes 0x19–0x29 to
  `0x004BDC40`, and 0x2B–0x2E to `0x004BE6A0`.
  REA evidence: `ev_121e447bc9d4fcaba1f8507827e8e6c2dcff51d3d5aa36dc3bfb75c99fccab6e`.
- Instructions: IMUL at `0x004BDCAD`, ADD at `0x004BDCFB`, SUB at
  `0x004BDD21`, SHL at `0x004BDD47`, SAR at `0x004BDD6D`, NEG at
  `0x004BE709`. These retain low 32 result bits; x86 shifts mask counts to five bits.
- Full-image Ghidra did not define the arithmetic/unary functions. Preserve
  that limitation: the following are **exact raw instruction window** analyses,
  not full-image function decompilations. Boundaries were checked against
  preceding returns/alignment and original registrar references.
- Binary window: original file offset `0xBDC40`, 2528 bytes, loaded at
  `0x004BDC40`, includes the adjacent jump tables. REA evidence
  `ev_81b4b4f28d3043d698f730c5e197d82fb076bf27d03900dc6dd557060a2c32d9`.
  Ghidra/radare2 blocks: 120/119; block-count score
  `1 - abs(120 - 119) / 120 = 0.991667`.
- Unary window: original file offset `0xBE6A0`, 368 bytes, loaded at
  `0x004BE6A0`, includes both adjacent jump tables. REA evidence
  `ev_07dcc2213b1e36f248b2af0aad45623eb9ae247ffbf10e89428ce1db42c3803c`.
  Ghidra/radare2 blocks: 18/18; block-count score 1.0. An earlier window
  without tables could not resolve indirect branches and was not accepted.

## Correction and checks

Unsigned 32-bit arithmetic followed by `std::bit_cast<int32_t>` preserves
wrapping results. Shift counts are masked by 31. Signed right shift has
arithmetic semantics in the required C++20 standard.

`engine/tests/scripting/integer_arithmetic_test.cpp` exercises 616 boundary
operations through the LOOP execution path, including negative and oversized
shift counts, minimum/maximum integers, multiplication overflow and negation
of the minimum integer. The right-shift oracle uses signed 64-bit division
rounded toward negative infinity, independently of the implementation's shift.

- Regression failed under UBSan before correction.
- All 616 operations pass ASan/UBSan after correction (leak detection disabled).
- Game and focused test targets build. Four focused CTests pass: integer
  arithmetic, signed operands, Level 1 original AI script dispatch, and
  original Level 1 gameplay loading.

Artifacts: `/tmp/igi-qvm-retail-evidence/` contains raw windows, REA JSON and
independent radare2 listings. The sanitizer executable is
`/tmp/igi-script-integer-sanitized`.

## Verification limits and next integration

This establishes the covered integer operations and removes demonstrated
undefined behavior. It is not a live original-game arithmetic trace comparison,
complete VM parity, or completed Level 1 gameplay. Integer division fault
semantics, floating arithmetic, native bindings and task scheduling remain open.
The scripting target is still isolated under BUILD_TESTING; production AI still
uses static patrol extraction. Replacing that extraction requires event dispatch
and actual native/action bindings, not silently enabling this incomplete VM.
