'use strict';
// Synthetic input driver for paired runtime captures.
//
// Several MVP natives sit behind interactive paths (task/objective view, menus and the
// front-end option screens) that a passive gameplay capture never enters. The retail input
// layer is DirectInput, which reads device state rather than posted WM_KEYDOWN messages, so
// this driver injects through SendInput from inside the game process - the same path a
// physical key press or mouse click takes. Mouse clicks use ClientToScreen against the
// foreground window, so a click lands at a fraction of the game's own client area whatever
// size the window is.
//
// The driver attaches as a second Frida session so the observation probe stays
// byte-identical and its recorded hash keeps meaning what it says.

const user32 = {
  sendInput: new NativeFunction(Module.getGlobalExportByName('SendInput'),
    'uint', ['uint', 'pointer', 'int']),
  findWindow: new NativeFunction(Module.getGlobalExportByName('FindWindowA'),
    'pointer', ['pointer', 'pointer']),
  setForeground: new NativeFunction(Module.getGlobalExportByName('SetForegroundWindow'),
    'int', ['pointer']),
  bringToTop: new NativeFunction(Module.getGlobalExportByName('BringWindowToTop'),
    'int', ['pointer']),
  showWindow: new NativeFunction(Module.getGlobalExportByName('ShowWindow'),
    'int', ['pointer', 'int']),
  className: new NativeFunction(Module.getGlobalExportByName('GetClassNameA'),
    'int', ['pointer', 'pointer', 'int']),
  foreground: new NativeFunction(Module.getGlobalExportByName('GetForegroundWindow'),
    'pointer', []),
  clientRect: new NativeFunction(Module.getGlobalExportByName('GetClientRect'),
    'int', ['pointer', 'pointer']),
  clientToScreen: new NativeFunction(Module.getGlobalExportByName('ClientToScreen'),
    'int', ['pointer', 'pointer']),
  setCursorPos: new NativeFunction(Module.getGlobalExportByName('SetCursorPos'),
    'int', ['int', 'int'])
};

const INPUT_SIZE = 28;          // sizeof(INPUT) on 32-bit Windows
const KEYUP = 0x0002;
const MOUSE_LEFT_DOWN = 0x0002;
const MOUSE_LEFT_UP = 0x0004;

// The image is started windowed but Wine leaves its own desktop window in front, so
// SendInput would land on the desktop. Grab the game window by its retail class name.
const GAME_CLASSES = ['Innerloop', 'IGI.Retail.ModelScene'];
const SW_SHOW = 5;

function ensureFocus() {
  const window = user32.foreground();
  if (!window.isNull()) {
    const name = Memory.alloc(128);
    user32.className(window, name, 128);
    if (GAME_CLASSES.indexOf(name.readUtf8String()) !== -1) {
      return;
    }
  }
  for (let index = 0; index < GAME_CLASSES.length; index += 1) {
    const game = user32.findWindow(Memory.allocUtf8String(GAME_CLASSES[index]), NULL);
    if (game.isNull()) {
      continue;
    }
    user32.showWindow(game, SW_SHOW);
    user32.bringToTop(game);
    user32.setForeground(game);
    return;
  }
}

function blank() {
  const buffer = Memory.alloc(INPUT_SIZE);
  for (let offset = 0; offset < INPUT_SIZE; offset += 1) {
    buffer.add(offset).writeU8(0);
  }
  return buffer;
}

function press(virtualKey) {
  ensureFocus();
  const down = blank();
  down.writeU32(1);                  // INPUT_KEYBOARD
  down.add(4).writeU16(virtualKey);  // KEYBDINPUT.wVk
  user32.sendInput(1, down, INPUT_SIZE);
  setTimeout(() => {
    const up = blank();
    up.writeU32(1);
    up.add(4).writeU16(virtualKey);
    up.add(8).writeU32(KEYUP);
    user32.sendInput(1, up, INPUT_SIZE);
  }, 80);
}

function clickMouse() {
  const down = blank();
  down.writeU32(0);                  // INPUT_MOUSE
  down.add(16).writeU32(MOUSE_LEFT_DOWN);
  user32.sendInput(1, down, INPUT_SIZE);
  setTimeout(() => {
    const up = blank();
    up.writeU32(0);
    up.add(16).writeU32(MOUSE_LEFT_UP);
    user32.sendInput(1, up, INPUT_SIZE);
  }, 60);
}

// Click at a fraction of the foreground window's client area.
function clickAt(fractionX, fractionY) {
  ensureFocus();
  const window = user32.foreground();
  if (window.isNull()) {
    return;
  }
  const rect = Memory.alloc(16);
  if (user32.clientRect(window, rect) === 0) {
    return;
  }
  const width = rect.add(8).readS32();
  const height = rect.add(12).readS32();
  if (width <= 0 || height <= 0) {
    return;
  }
  const point = Memory.alloc(8);
  point.writeS32(Math.round(width * fractionX));
  point.add(4).writeS32(Math.round(height * fractionY));
  if (user32.clientToScreen(window, point) === 0) {
    return;
  }
  user32.setCursorPos(point.readS32(), point.add(4).readS32());
  clickMouse();
}

// Keyboard timeline: task and map views, the in-game menu and its option screens,
// help/status, movement and interaction.
const TIMELINE = [
  [10000, [0x09]],                                  // task view
  [12000, [0x4D]],                                  // map computer
  [14000, [0x1B]],                                  // in-game menu
  [15200, [0x28]],
  [16400, [0x28]],
  [17600, [0x0D]],
  [19000, [0x27]],
  [20200, [0x28]],
  [21400, [0x25]],
  [22600, [0x0D]],
  [24000, [0x1B]],
  [25200, [0x1B]],
  [26600, [0x70]],
  [27800, [0x49]],
  [29200, [0x57]], [29600, [0x41]], [30000, [0x53]], [30400, [0x44]],
  [31800, [0x0D]]
];

// Mouse sweep over the front-end menu: five columns by five rows of the client area,
// which covers the button column and the option-screen controls.
const CLICK_COLUMNS = [0.08, 0.25, 0.42, 0.58, 0.75];
const CLICK_ROWS = [0.18, 0.32, 0.46, 0.60, 0.74];
const CLICK_START_MS = 34000;
const CLICK_INTERVAL_MS = 1100;
const CYCLE_MS = CLICK_START_MS + CLICK_COLUMNS.length * CLICK_ROWS.length * CLICK_INTERVAL_MS + 4000;

function runCycle() {
  for (const [offset, keys] of TIMELINE) {
    setTimeout(() => {
      for (const key of keys) {
        press(key);
      }
    }, offset);
  }
  let index = 0;
  for (const row of CLICK_ROWS) {
    for (const column of CLICK_COLUMNS) {
      const offset = CLICK_START_MS + index * CLICK_INTERVAL_MS;
      index += 1;
      setTimeout(() => clickAt(column, row), offset);
    }
  }
  setTimeout(runCycle, CYCLE_MS);
}

// Let the level or front end finish loading before touching the input devices.
setTimeout(runCycle, 1000);
