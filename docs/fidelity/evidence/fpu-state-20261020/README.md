# Scoped x87 reconstruction evidence, 2026-10-20

The six functions retain existing catalog labels, which are misleading camera/
display names. Their observed instructions manipulate x87 control state. Do not
infer camera field-of-view behavior from those names.

Retail startup reaches 0x004A7157 through entry 0x004A50F5, 0x004A5AA8,
global 0x00543E8C and 0x004A5363. It selects 53-bit precision. Function
0x00498460 preserves the other control bits and selects truncation. Together
the observed retail control word is 0x0E7F. Merely selecting truncation in the
candidate left 64-bit precision (0x0F7F), which was a real numerical difference.

Binary Ninja was the sole decompiler. The retained per-address reports contain
Radare2 CFGs and exact-entry angr recovery. The actual candidate C++ source was
compiled into an x86 PE and run alongside retail instructions under Unicorn:
64 controlled cases passed across the six helpers, including 16 scalar boundary
cases for each conversion helper and eight hardware-state cases for each of
four stateful helpers. Meaningful returns, preserved registers, control/status
state and the relevant global memory region were compared. This is controlled
x86 replay, distinct from a Frida game-runtime observation.

The independent critic reviewed the source, ABI adaptations, and fixture scope.
Signatures distinguish implicit x87 state from real stack arguments. Catalog
statuses remain TRANSCRIBED. Pending exceptions, unmasked traps, arbitrary live
FPU stack state and every possible scalar flag input are outside the fixtures.

The paired Frida level captures observe the initializer in both applications.
They do not directly exercise every helper as a paired Frida native-call test.
The accompanying paired-transform-matrix.json records exact rigid-object model,
position and orientation data for Levels 1, 4 and 7 after the scene adapter also
retained retail's x87 multiply/add/store order. This is a class-specific data
checkpoint; material, texture, collision, player and whole-game parity remain
open. No whole-game 100% equivalence claim follows from these reports.
