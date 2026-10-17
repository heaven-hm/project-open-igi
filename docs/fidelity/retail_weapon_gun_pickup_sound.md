# Retail `Weapon_GunPickup` Sound Side Effect

## Retail evidence

- Image: `/Users/haseeb-mir/Downloads/IGI/igi.exe`
- SHA-256: `dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01`
- Native: `Weapon_GunPickup` at `0x0045FFC0`
- REA/Ghidra evidence: `ev_969c64534bfb476404db23fcf72a6689a539cfbcd9cc0d902adb437647577249`
- Ghidra body: `0x0045FFC0..0x00460067`, 168 bytes, 14 basic blocks, five calls
- Independent radare2 body: 168 bytes, 14 basic blocks, 65 instructions, five calls
- CFG score: `0.970` using the documented block/conditional/call score; Ghidra and radare2 differ by one conditional branch count

The retail handler first checks the byte at human offset `+0xCF5`. When it is zero, it updates the weapon/ammunition state and then calls `Sound_EventTrigger` at `0x004E7200` with `weaponpickup_1`. When the byte is nonzero, the handler returns without the sound call.

## C++ change

The Level 1 pickup path now queues one `weaponpickup_1` event for each pickup that passes the already-portable `GunPickup_Action` distance gate. The main loop dispatches each queued event through `SoundSystem::SoundSystem_PlayDefinition`, which uses the existing retail sound-definition registry and mixer path. The inventory probe verifies that one accepted pickup queues one event and that an already-collected pickup queues none. The Level 1 sound-system test also resolves `weaponpickup_1` from the retail definitions/bank and verifies that it creates an active mixer voice.

## Remaining fidelity work

This change ports only the sound side effect. The native's `+0xCF5` suppression field is not yet mapped to a C++ player-state field. The inventory helper calls (`0x0047CE00` and `0x00477C50`) also need their ammo record bounds/caps and weapon-task initialization mapped before the inventory branch can be considered a complete `Weapon_GunPickup` port. The test verifies definition lookup and PCM voice creation; audible SDL device playback during an actual pickup remains unverified.
