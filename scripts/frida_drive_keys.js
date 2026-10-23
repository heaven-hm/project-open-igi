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

// Timeline of (milliseconds after the level is up, keys) events. The sweep covers the
// task and map views, the in-game menu (which is where the graphic-option records are
// read and written), the option screens' own navigation, and movement/interaction.
const TIMELINE = [
  [10000, [0x09]],                                  // task view
  [12000, [0x4D]],                                  // map computer
  [14000, [0x1B]],                                  // in-game menu
  [15200, [0x28]],                                  // move down one entry
  [16400, [0x28]],
  [17600, [0x0D]],                                  // open the selected entry
  [19000, [0x27]],                                  // walk the option screens
  [20200, [0x28]],
  [21400, [0x25]],
  [22600, [0x0D]],                                  // apply the highlighted value
  [24000, [0x1B]],
  [25200, [0x1B]],                                  // leave the menu
  [26600, [0x70]],                                  // help
  [27800, [0x49]],                                  // inventory/status
  [29200, [0x57]], [29600, [0x41]], [30000, [0x53]], [30400, [0x44]],
  [31800, [0x0D]],                                  // interact
];
const CYCLE_MS = 34000;

function runCycle() {
  for (const [offset, keys] of TIMELINE) {
    setTimeout(() => {
      for (const key of keys) {
        press(key);
      }
    }, offset);
  }
  setTimeout(runCycle, CYCLE_MS);
}

// Let the level finish loading before touching the keyboard.
setTimeout(runCycle, 1000);
