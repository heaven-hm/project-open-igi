// Drives the open-igi.exe window with synthetic input, so the retail input port,
// the player controller and the camera can be observed end to end.
//
// The script posts the same key and mouse events the retail game would receive,
// and the scene log reports the human state after each step.
'use strict';

const user32 = Process.getModuleByName('user32.dll');
const findWindow = new NativeFunction(user32.getExportByName('FindWindowW'),
                                      'pointer', ['pointer', 'pointer']);
const postMessage = new NativeFunction(user32.getExportByName('PostMessageW'),
                                       'int', ['pointer', 'uint', 'uint', 'pointer']);

function windowHandle() {
  // The scene window the port creates.
  const name = Memory.allocUtf16String('IGI.Retail.ModelScene');
  const title = Memory.allocUtf16String('IGI');
  return findWindow(name, title);
}

function postKey(window, virtual_key, down) {
  const message = down ? 0x0100 : 0x0101;   // WM_KEYDOWN / WM_KEYUP
  postMessage(window, message, virtual_key, ptr(0));
}

function postMouseMove(window, x, y) {
  const packed = ((y & 0xffff) << 16) | (x & 0xffff);
  postMessage(window, 0x0200, 0, ptr(packed));   // WM_MOUSEMOVE
}

function sleep(ms) {
  const end = Date.now() + ms;
  while (Date.now() < end) {
    Thread.sleep(50);
  }
}

rpc.exports = {
  drive() {
    const window = windowHandle();
    if (window.isNull()) {
      return {ok: false, error: 'Innerloop window not found'};
    }
    const steps = [];
    // 1. look right: move the mouse to the right
    for (let i = 0; i < 10; i++) {
      postMouseMove(window, 100 + i * 10, 200);
      sleep(60);
    }
    steps.push('mouse look right');
    // 2. jump: E
    postKey(window, 0x45, true);
    sleep(120);
    postKey(window, 0x45, false);
    steps.push('jump E');
    // 3. crouch: C
    postKey(window, 0x43, true);
    sleep(200);
    postKey(window, 0x43, false);
    steps.push('crouch C');
    // 4. walk forward: W
    postKey(window, 0x57, true);
    sleep(600);
    postKey(window, 0x57, false);
    steps.push('walk W');
    // 5. walk toggle: LSHIFT
    postKey(window, 0xA0, true);
    sleep(150);
    postKey(window, 0xA0, false);
    steps.push('walk toggle LSHIFT');
    return {ok: true, steps: steps};
  }
};
