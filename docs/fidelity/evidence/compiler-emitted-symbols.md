# Compiler-emitted symbols: a call the source does not contain

Recorded 2026-10-25 after two independent sightings. Both are the same failure mode in opposite directions: a `call` that exists in an object file
while nothing in the retail body corresponds to it.

## Witness 1, retail side: `0x004A54A0` is `__chkstk`

`0x004A54A0..0x004A54CE` is MSVC's 4 KB stack-probe helper (`push ecx; cmp eax,0x1000; lea ecx,[esp+8]; jb done;` page-touch loop; `sub ecx,eax;
mov eax,esp; test [ecx],eax; mov esp,ecx; ... push eax; ret`). It sits between the known CRT routines `sprintf` (`0x004A53B3`) and `strncpy`
(`0x004A5500`). Call sites set EAX to the frame size and push no arguments (e.g. `0x004170A0`: `mov eax,0x2898; call 0x004A54A0`), which is the
compiler idiom, not a hand call. It must be classified CRT and never hand-transcribed; a transcription would be an invented native that is also wrong
to call from a C++ frame. `scripts/native_coverage.py` classifies it accordingly.

An image-wide byte-exact sweep found exactly one `__chkstk` fingerprint, zero `__ftol2`, zero `__security_check_cookie`, zero `__EH_prolog3`, and
four SEH-prolog matches (`0x004A50F5`, `0x004A5DEE`, `0x004AD225`, `0x004AD4D3`), of which three are referenced only from inside the CRT cluster and
one has no references at all. So no game body reaches a compiler-emitted routine other than the stack probe.

## Witness 2, candidate side: `___stack_chk_*` appears in our own object

A transcription of `0x004978F0` first used a `std::uint32_t pair[2]` local. Taking that array's address made clang emit a stack-protector call:
the object carried undefined `___stack_chk_fail` and `___stack_chk_guard`, symbols retail has no call to. Replacing the array with a two-scalar
anonymous struct (same 8 bytes, same layout, no array type) removed them and left exactly the one transcribed callee.

## Rule

Every deliverable states its object's undefined-symbol set, and every symbol in it must be **either a transcribed native or a declared CRT routine**.
A symbol that is neither is a defect in the source, not a build detail: the compiler has added a call that retail does not make. Do not silence it by
linking the host runtime; change the source until the call set is retail's.

Two consequences worth keeping:
- `build/retail-dis.txt` renders no calls to `0x004A54A0` at all while `r2 /r` lists eight callers, so callee counting from that listing is blind to
  at least this one routine. Check the image, not only the listing.
- Status claims are about the object as built. A transcription whose undefined set has never been read has not been checked for this class of defect.
