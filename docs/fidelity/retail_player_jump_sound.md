# Retail player jump sound

## Ground truth

Target executable: `/Users/haseeb-mir/Downloads/IGI/igi.exe`, SHA-256 `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`.

REA/Ghidra 12.1.2 finds `jump_1` at `0x00538D6C` (evidence `ev_12f7fff34e646b7fa44ad2afdd79517a21608de4cf8d9d9dba000606b081b275`). Its xref at `0x0040BAE7` is an observed `PUSH 0x00538D6C` (evidence `ev_e2864ec383ec26879e419de8f005b06e6b9f17d4174f86b556efec6ea352551a`), followed at `0x0040BAED` by a direct call to `FUN_004E6B00`. That helper resolves a named sound through registered engine handlers (REA evidence `ev_1148119312a94c77ec368da02eabf057c0b59dbe298fa2fb3f057b1ad120e75b`). Ghidra does not currently assign the string-reference block to a procedure, so its surrounding state predicate remains unresolved in this evidence set.

The vanilla C# port dispatches `jump_1` only when the player takes off: `apps/OpenIGI.Desktop/DesktopGame.cs` checks `TookOffThisTick`, and `DesktopGame.Audio.cs` plays `PlayerBody.JumpSoundName`. The comments there cite `0x0040BBF7` and string address `0x00536DC4`; those addresses differ from this retail binary's observed xref and are not used as C++ ground truth. The C++ port follows the already-verified jump rising edge for timing, and uses REA's retail `jump_1` string-to-sound-dispatch evidence for the named cue.

## C++ implementation and tests

`GameSimulation` queues `jump_1` on the grounded jump rising edge; the game loop consumes it once through the Level 1 `SoundSystem_PlayDefinition` route. `mvp_movement_weapon_ai` verifies take-off timing and no repeated cue on the following airborne tick. `retail_level_one_sound_playback` verifies the original Level 1 bank creates an active voice for `jump_1`.

Static event and asset evidence plus mixer playback are verified. The unresolved retail state predicate and lack of live DirectSound timing comparison prevent a claim of byte-for-byte runtime sound parity.
