# StoreInstanceHandle — unified single-native observation scope

Retail `0x0048F0D0` is WinMain's second game-native call, at `0x0048F5FA`.
Image SHA-256: `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`.
Its ten bytes, `8b442404 a3c08b5c00 c3`, load one cdecl argument word,
store it at `0x005C8BC0`, and return the same bits. No branch, callee,
argument dereference, arithmetic or floating-point operation occurs.

`scripts/probes/store_instance_handle.js` supplies both controlled replay and
pre-resume natural observation. Controlled replay retains the existing eleven
opaque-word inputs, checks each ordered return and seeded destination transition,
checks the sampled neighboring bytes, and restores the original destination.
No input word is dereferenced or assumed to be a valid handle.

Natural observation requires exactly one actual WinMain invocation per image.
The argument must equal that process's image base and the destination must
initially be null. Return and stored word must each alias that argument exactly;
the four bytes on either side of the destination must remain unchanged. Only
after these checks is the handle normalized to the same module-relative role.
Raw pointer addresses are never compared across processes. The probe checks the
actual ten-byte body and its destination operand against the resolved candidate
symbol mapping before installing the natural hook.

The capture completes at this native's return, after emitting its typed state.
No action is scripted for this narrow observation. Both programs still use
the original game-data working directory and `window level1`. Retail runs first,
then the freshly built candidate, under the shared sequential runtime lease.

The selected BN decompiler, independent r2 CFG, exact-entry angr, source-semantic
review, validated x86 build receipt, controlled replay and typed natural-call
comparison must all pass. This scope does not grant acceptance or transfer old
runtime evidence to the new image. It does not test later window creation,
the enclosing startup sequence, rendering, movement or playable Level 1.
Compiler CRT and Windows SDK internals remain outside game-native transcription.
