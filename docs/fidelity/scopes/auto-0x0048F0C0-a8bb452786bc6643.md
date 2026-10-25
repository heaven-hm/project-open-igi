# StoreCommandLine unified probe scope

Retail native `0x0048F0C0`: one cdecl stack word copied to `0x005C8BC8` and returned in EAX.
Test one natural WinMain call per image using `window level1` and the existing eleven
controlled word-copy cases per image before primary-thread resume. The natural argument
is normalized to complete NUL-terminated bytes; store and return aliases are checked in
each process. Argument bytes and sampled adjacent data must remain unchanged.
The reviewed natural capture stops after exactly one fully observed native return, before
unrelated initialization. It does not accept WinMain, later startup, rendering or gameplay.
Candidate build/source/probe/scenario hashes and typed comparisons remain mandatory.
