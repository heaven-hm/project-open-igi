# DirectX failure termination closure: static analysis only

Retail WinMain calls 0x004A5AD5 with status 1 after the capability error message.
The existing Camera_ApplyBandFilter_6 label is unsupported by its instructions.
The 17-byte cdecl wrapper pushes zero, zero and its incoming argument, calls
0x004A5AF7, removes 12 bytes and has a return instruction. The bytes alone do not
establish an original source symbol, return type or universal non-return behavior.

The callee checks a termination-state word at 0x009360AC; if set, it obtains the
current process and calls TerminateProcess with argument 1's value as exit code.
Otherwise it writes 1 to 0x009360A8 and the low byte of argument 3 to 0x009360A4.
When argument 2 is zero it traverses a callback table backward using 0x00C32550
and 0x00C3254C, calling non-null entries, then calls 0x004A5B90 over the range
0x00536018..0x00536020. It always calls that routine over
0x00536024..0x00536028. When argument 3 is zero it sets 0x009360AC to 1 and calls
ExitProcess(argument 1); otherwise it returns. This directly supports termination
behavior, not camera effects. Cleanup callback implementations remain unresolved.

The captures bind retail SHA-256 dea34892... to one selected decompiler, Binary
Ninja, Radare2 CFG and exact angr entry recovery. No Ghidra analysis was invoked.
Wrapper graphs agree at 1.0; callee graphs have 13 blocks each and score
0.9638888889. `edge-disagreement.json` records the actual difference: Radare2
retains a fallthrough edge after TerminateProcess that Binary Ninja omits. This
assumption is not resolved by exceeding the 0.85 threshold, and no candidate or
runtime acceptance is claimed. Indirect callback targets need further evidence.

No implementation or speculative clean name is added by this packet. The
unresolved formatting/termination dependencies still block integrating the
parent failure branch. Required next evidence includes typed exit-code/global
state/callback ordering observations from the original, then a freshly compiled
candidate preserving those dependencies and branches.
