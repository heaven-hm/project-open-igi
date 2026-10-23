'use strict';
// Synthetic keyboard driver for paired runtime captures.
//
// Several MVP natives sit behind interactive paths (task/objective view, pause
// and status changes, menus) that a passive gameplay capture never enters. The
// retail input layer is DirectInput, which reads device state rather than posted
// WM_KEYDOWN messages, so this driver injects through SendInput from inside the
// game process - the same path a physical key press takes.
//
// The driver attaches as a second Frida session so the observation probe stays
// byte-identical and its recorded hash keeps meaning what it says.

const sendInput = new NativeFunction(
  Module.getExportByName('user32.dll', 'SendInput'), 'uint', ['uint', 'pointer', 'int']);
const SIZE = 28;          // sizeof(INPUT) on 32-bit Windows
const KEYUP = 0x0002;

function press(virtualKey) {
  const buffer = Memory.alloc(SIZE);
  for (let offset = 0; offset < SIZE; offset += 1) {
    buffer.add(offset).writeU8(0);
  }
  buffer.writeU32(1);                  // INPUT_KEYBOARD
  buffer.add(4).writeU16(virtualKey);  // KEYBDINPUT.wVk
  buffer.add(6).writeU16(0);           // KEYBDINPUT.wScan
  buffer.add(8).writeU32(0);           // key down
  sendInput(1, buffer, SIZE);
  setTimeout(() => {
    const release = Memory.alloc(SIZE);
    for (let offset = 0; offset < SIZE; offset += 1) {
      release.add(offset).writeU8(0);
    }
    release.writeU32(1);
    release.add(4).writeU16(virtualKey);
    release.add(8).writeU32(KEYUP);
    sendInput(1, release, SIZE);
  }, 80);
}

// Tab / M / T / O / N are the candidate task and map views, F1 help, Esc the
// pause menu, the arrows and WASD movement, Enter and E interaction, C crouch.
const KEYS = [0x09, 0x4D, 0x54, 0x4F, 0x4E, 0x70, 0x49, 0x1B, 0x1B,
              0x25, 0x27, 0x26, 0x28, 0x57, 0x41, 0x53, 0x44, 0x0D, 0x45, 0x43];
let index = 0;

function step() {
  press(KEYS[index % KEYS.length]);
  index += 1;
  setTimeout(step, 1100);
}

// Let the level finish loading before touching the keyboard.
setTimeout(step, 10000);
