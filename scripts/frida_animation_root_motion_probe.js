/*
 * Small, read-only retail animation/root-motion probe.
 *
 * Target: igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01
 *
 * This probe only records function entries/returns and typed reads at documented
 * human/animation offsets.  It never writes game memory and never dereferences an
 * unchecked pointer.  Observational input is delivered through user32!keybd_event
 * because retail polls DirectInput; window messages alone do not set key state.
 */
'use strict';

const MODULE_NAME = 'igicap.exe';
const RETAIL_BASE = 0x00400000;
const module = Process.findModuleByAddress(ptr(RETAIL_BASE));
if (module === null) throw new Error('retail image is not mapped at 0x00400000');
const imageBase = module.base;
const targets = [
  ['Human_StateAnimation', 0x00489E70],
  ['Human_SetAnimation', 0x00489EA0],
  ['AnimClock_SetFps', 0x004D2D30],
  ['Anim_RootSample', 0x004D2FB0],
  ['Anim_RootEval', 0x004D3020],
  ['Anim_TrackEval', 0x004D31C0],
  ['Anim_ResolveClip', 0x004D47D0],
  ['Anim_Play', 0x004D61D0],
  ['Human_RunUpdate', 0x004109A0],
  ['Human_UpdateVariantA', 0x004550B0],
  ['Human_UpdateVariantB', 0x00456FF0],
  ['Human_UpdateVariantC', 0x00459F50],
  ['Human_UpdateVariantD', 0x0045D080],
  ['Human_ScaleFactor', 0x00413A80],
  ['Keyboard_Update', 0x00490230],
  ['Human_Integrate', 0x004614B0],
];
const counts = Object.create(null);
const failures = [];
const installed = [];
const observations = [];
const MAX_OBSERVATIONS = 40000;
let ready = false;

function address(va) {
  return imageBase.add(va - RETAIL_BASE);
}

function safe(fn, fallback) {
  try {
    return fn();
  } catch (error) {
    return fallback;
  }
}

function readable(pointer) {
  if (pointer === null || pointer === undefined) return false;
  try {
    if (pointer.isNull()) return false;
    return Process.findRangeByAddress(pointer) !== null;
  } catch (error) {
    return false;
  }
}

function f32(value) {
  return new Float32Array(new Uint32Array([value >>> 0]).buffer)[0];
}

function floatArg(arg) {
  return f32(arg.toUInt32());
}

function u32At(base, offset) {
  return safe(() => base.add(offset).readU32(), null);
}

function f32At(base, offset) {
  return safe(() => base.add(offset).readFloat(), null);
}

function f64At(base, offset) {
  return safe(() => base.add(offset).readDouble(), null);
}

function vec3f(base, offset) {
  return safe(() => ({
    x: base.add(offset).readFloat(),
    y: base.add(offset + 4).readFloat(),
    z: base.add(offset + 8).readFloat(),
  }), null);
}

function vec3d(base, offset) {
  return safe(() => ({
    x: base.add(offset).readDouble(),
    y: base.add(offset + 8).readDouble(),
    z: base.add(offset + 16).readDouble(),
  }), null);
}

function record(name, data) {
  counts[name] = (counts[name] || 0) + 1;
  if (observations.length >= MAX_OBSERVATIONS) return;
  const item = { kind: 'observation', target: name, ordinal: counts[name], ...data };
  observations.push(item);
  send(item);
}

function attach(name, va, handlers) {
  const at = address(va);
  try {
    Interceptor.attach(at, handlers);
    installed.push({ name, va: '0x' + va.toString(16).toUpperCase(), address: at.toString() });
  } catch (error) {
    failures.push({ name, va: '0x' + va.toString(16).toUpperCase(), error: String(error) });
  }
}

// 0x00489E70: Human_StateAnimation(human, state, blend).
// Retail indexes an 8-byte {clip, loopFlag} pair from [human+0x338].
attach('Human_StateAnimation', 0x00489E70, {
  onEnter(args) {
    const human = args[0];
    const state = args[1].toInt32();
    const blendBits = args[2].toUInt32();
    const table = readable(human) ? safe(() => ptr(human.add(0x338).readU32()), null) : null;
    let clip = null;
    let loopFlag = null;
    if (table !== null && readable(ptr(table)) && state >= 0 && state < 32) {
      clip = u32At(ptr(table), state * 8);
      loopFlag = u32At(ptr(table), state * 8 + 4);
    }
    record('Human_StateAnimation', {
      human: human.toString(), state, blendBits, blend: f32(blendBits),
      table: table === null ? null : '0x' + table.toString(16),
      clip, loopFlag,
    });
  },
  onLeave(retval) {
    record('Human_StateAnimation.return', { eax: retval.toString() });
  },
});

// 0x00489EA0: Human_SetAnimation(human, clip, loopFlag, blend).
attach('Human_SetAnimation', 0x00489EA0, {
  onEnter(args) {
    record('Human_SetAnimation', {
      human: args[0].toString(), clip: args[1].toInt32(),
      loopFlag: args[2].toUInt32(), blendBits: args[3].toUInt32(),
      blend: floatArg(args[3]),
    });
  },
});

// 0x004D2D30: AnimClock_SetFps(fps) => [0x00A54658] = 4800.0 / fps.
attach('AnimClock_SetFps', 0x004D2D30, {
  onEnter(args) {
    this.fps = floatArg(args[0]);
  },
  onLeave() {
    record('AnimClock_SetFps', {
      fps: this.fps,
      millisecondsPerTick: safe(() => ptr(0x00A54658).readFloat(), null),
    });
  },
});

// 0x004D2FB0: Anim_RootSample(outA, outB, animationObject).
// Caller Human_RunUpdate passes [human+0x514] and [human+0x520].
attach('Anim_RootSample', 0x004D2FB0, {
  onEnter(args) {
    this.outA = args[0];
    this.outB = args[1];
    const animation = args[2];
    let active = null;
    if (readable(animation)) {
      const track = safe(() => ptr(animation.add(8).readU32()), null);
      if (track !== null && readable(ptr(track))) active = u32At(ptr(track), 0x80);
    }
    this.animation = animation.toString();
    this.active = active;
  },
  onLeave() {
    record('Anim_RootSample', {
      outA: this.outA.toString(), outB: this.outB.toString(),
      a: readable(this.outA) ? vec3f(this.outA, 0) : null,
      b: readable(this.outB) ? vec3f(this.outB, 0) : null,
      animation: this.animation, activeAnimation: this.active,
    });
  },
});

// 0x004D3020: Anim_RootEval(outA, outB, clip).
attach('Anim_RootEval', 0x004D3020, {
  onEnter(args) {
    this.outA = args[0];
    this.outB = args[1];
    this.clip = args[2];
  },
  onLeave(retval) {
    record('Anim_RootEval', {
      outA: this.outA.toString(), outB: this.outB.toString(),
      a: readable(this.outA) ? vec3f(this.outA, 0) : null,
      b: readable(this.outB) ? vec3f(this.outB, 0) : null,
      clip: this.clip.toString(), eax: retval.toInt32(),
    });
  },
});

// 0x004D31C0: Anim_TrackEval(outA, outB, node).  Raw ABI observation only.
attach('Anim_TrackEval', 0x004D31C0, {
  onEnter(args) {
    this.outA = args[0];
    this.outB = args[1];
    this.node = args[2];
  },
  onLeave(retval) {
    record('Anim_TrackEval', {
      outA: this.outA.toString(), outB: this.outB.toString(), node: this.node.toString(),
      a: readable(this.outA) ? vec3f(this.outA, 0) : null,
      b: readable(this.outB) ? vec3f(this.outB, 0) : null,
      eax: retval.toInt32(),
    });
  },
});

// 0x004D47D0: Anim_ResolveClip(animationClass, slot, clipIndex, flags).
attach('Anim_ResolveClip', 0x004D47D0, {
  onEnter(args) {
    record('Anim_ResolveClip', {
      animationClass: args[0].toInt32(), slot: args[1].toInt32(),
      clipIndex: args[2].toInt32(), flags: args[3].toUInt32(),
    });
  },
  onLeave(retval) {
    record('Anim_ResolveClip.return', { handle: retval.toString() });
  },
});

// 0x004D61D0: Anim_Play(animationObject, slot, clip, loopFlag, seconds).
attach('Anim_Play', 0x004D61D0, {
  onEnter(args) {
    record('Anim_Play', {
      animationObject: args[0].toString(), slot: args[1].toInt32(),
      clip: args[2].toInt32(), loopFlag: args[3].toUInt32(),
      secondsBits: args[4].toUInt32(), seconds: floatArg(args[4]),
    });
  },
});

function humanState(human) {
  if (!readable(human)) return null;
  return {
    taskState: u32At(human, 0x2fc),
    animationTable: u32At(human, 0x338),
    animationObject: u32At(human, 0x68),
    rootA: vec3f(human, 0x514),
    rootB: vec3f(human, 0x520),
    rootScaleFlag: safe(() => human.add(0x52c).readU8(), null),
    velocityBase: vec3f(human, 0x664),
    velocityExtra: vec3f(human, 0x670),
    velocityAccum: vec3f(human, 0x67c),
    speed: vec3f(human, 0x688),
    position: vec3d(human, 0x20),
    deltaMove: vec3f(human, 0x728),
    deltaAngle: vec3f(human, 0x75c),
  };
}

// 0x004109A0: Human_RunUpdate(human).  The selected root delta is written to
// [human+0x520] by Anim_RootSample, scaled by 1.75 when [human+0x52c]==0,
// rotated by the [human+0x78..0x98] matrix, then integrated by 0x004614B0.
attach('Human_RunUpdate', 0x004109A0, {
  onEnter(args) {
    this.human = args[0];
    this.before = humanState(this.human);
  },
  onLeave() {
    record('Human_RunUpdate', {
      human: this.human.toString(), before: this.before, after: humanState(this.human),
    });
  },
});

// 0x004109A0 is one of five class-specific human update variants.  Only one of
// them handles the local player; identify it by the human pointer it receives.
// The other variants have the same sample -> rotate -> integrate shape but
// different (or absent) scale blocks, so do not assume 0x004109A0 is the player.
const variantHits = Object.create(null);
function sampleVariant(name, data, limit = 5, every = 60) {
  variantHits[name] = (variantHits[name] || 0) + 1;
  if (variantHits[name] <= limit || variantHits[name] % every === 0) {
    record(name, { hit: variantHits[name], ...data });
  }
}

function attachUpdateVariant(name, va) {
  attach(name, va, {
    onEnter(args) {
      this.human = args[0];
      const before = humanState(this.human);
      const weaponObject = u32At(this.human, 0x800);
      let weaponCategory = null;
      if (weaponObject !== null && readable(ptr(weaponObject))) {
        weaponCategory = u32At(ptr(weaponObject), 0x3d38);
      }
      this.before = { rootB: before && before.rootB, rootScaleFlag: before && before.rootScaleFlag,
                      weaponObject, weaponCategory };
    },
    onLeave() {
      const after = humanState(this.human);
      sampleVariant(name, {
        human: this.human.toString(), before: this.before,
        after: after && { rootB: after.rootB, speed: after.speed, position: after.position },
      });
    },
  });
}

attachUpdateVariant('Human_UpdateVariantA', 0x004550B0);
attachUpdateVariant('Human_UpdateVariantB', 0x00456FF0);
attachUpdateVariant('Human_UpdateVariantC', 0x00459F50);
attachUpdateVariant('Human_UpdateVariantD', 0x0045D080);

// 0x00413A80 is the conditional scale source used by variant A when the
// weapon category at [weapon+0x3d38] is 4.  It returns float in ST0; only the
// call is recorded here (the scaled root is visible in variant A's leave data).
attach('Human_ScaleFactor', 0x00413A80, {
  onEnter(args) { this.arg = args[0].toString(); },
  onLeave() { record('Human_ScaleFactor', { arg: this.arg }); },
});

// 0x00490230: Keyboard_Update copies the DirectInput keyboard state into
// 0x00C28C62 (current, 256 bytes) and 0x00C28D62 (previous).  This observes
// whether synthetic input actually reaches the game's DirectInput path.
let lastKeyboardState = '';
attach('Keyboard_Update', 0x00490230, {
  onLeave() {
    const pressed = [];
    const state = ptr(0x00C28C62);
    for (let vk = 0; vk < 256; vk += 1) {
      const value = safe(() => state.add(vk).readU8(), 0);
      if (value !== 0) pressed.push([vk, value]);
    }
    const key = pressed.map(pair => pair.join(':')).join(',');
    if (key !== lastKeyboardState) {
      lastKeyboardState = key;
      record('Keyboard_Update', { pressed });
    }
  },
});

// 0x004614B0: Human_Integrate(human).
// local_5c = [human+0x664] + [human+0x520]; position += delta at [human+0x20].
attach('Human_Integrate', 0x004614B0, {
  onEnter(args) {
    this.human = args[0];
    this.before = humanState(this.human);
  },
  onLeave() {
    record('Human_Integrate', {
      human: this.human.toString(), before: this.before, after: humanState(this.human),
    });
  },
});

function user32() {
  return Process.getModuleByName('user32.dll');
}

function api(name, result, args) {
  return new NativeFunction(user32().getExportByName(name), result, args, 'stdcall');
}

function findIgiWindow() {
  const getOwner = api('GetWindowThreadProcessId', 'uint32', ['pointer', 'pointer']);
  const getTitle = api('GetWindowTextW', 'int', ['pointer', 'pointer', 'int']);
  const isVisible = api('IsWindowVisible', 'int', ['pointer']);
  const owner = Memory.alloc(4);
  const title = Memory.alloc(512);
  const windows = [];
  const visit = new NativeCallback((window, unused) => {
    getOwner(window, owner);
    if (owner.readU32() !== Process.id) return 1;
    getTitle(window, title, 256);
    const caption = title.readUtf16String();
    windows.push({
      window: window.toString(),
      caption,
      visible: isVisible(window) !== 0,
    });
    return 1;
  }, 'int', ['pointer', 'pointer'], 'stdcall');
  api('EnumWindows', 'int', ['pointer', 'pointer'])(visit, ptr(0));
  lastWindows = windows;
  const visible = windows.filter(item => item.visible);
  const named = visible.filter(item => /igi|project/i.test(item.caption));
  if (named.length === 1) return ptr(named[0].window);
  if (visible.length === 1) return ptr(visible[0].window);
  return null;
}

let demoStarted = false;
let demoSteps = [];
let keyState = [];
let lastWindows = [];

function allWindows() {
  const getOwner = api('GetWindowThreadProcessId', 'uint32', ['pointer', 'pointer']);
  const getTitle = api('GetWindowTextW', 'int', ['pointer', 'pointer', 'int']);
  const isVisible = api('IsWindowVisible', 'int', ['pointer']);
  const owner = Memory.alloc(4);
  const title = Memory.alloc(512);
  const rows = [];
  const visit = new NativeCallback((window, unused) => {
    getOwner(window, owner);
    getTitle(window, title, 256);
    rows.push({
      window: window.toString(), pid: owner.readU32(),
      caption: title.readUtf16String(), visible: isVisible(window) !== 0,
    });
    return 1;
  }, 'int', ['pointer', 'pointer'], 'stdcall');
  api('EnumWindows', 'int', ['pointer', 'pointer'])(visit, ptr(0));
  const getOwner2 = api('GetWindowThreadProcessId', 'uint32', ['pointer', 'pointer']);
  const foreground = api('GetForegroundWindow', 'pointer', [])();
  getOwner2(foreground, owner);
  getTitle(foreground, title, 256);
  const active = api('GetActiveWindow', 'pointer', [])();
  return {
    processId: Process.id,
    count: rows.length,
    rows,
    foreground: { window: foreground.toString(), pid: owner.readU32(),
                  caption: title.readUtf16String() },
    active: active.toString(),
  };
}

function beginDemo(window) {
  const setForeground = api('SetForegroundWindow', 'int', ['pointer']);
  const getForeground = api('GetForegroundWindow', 'pointer', []);
  const getOwner = api('GetWindowThreadProcessId', 'uint32', ['pointer', 'pointer']);
  setForeground(window);
  const foreground = getForeground();
  const owner = Memory.alloc(4);
  getOwner(foreground, owner);
  const focused = owner.readU32() === Process.id;
  demoSteps.push({ step: 'foreground', ok: focused, window: window.toString(),
                   foreground: foreground.toString(), foregroundPid: owner.readU32() });
  const keyEvent = api('keybd_event', 'void', ['uint8', 'uint8', 'uint32', 'pointer']);
  const mapVirtualKey = api('MapVirtualKeyW', 'uint32', ['uint32', 'uint32']);
  const extendedKeys = new Set([0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0xa3]);
  const down = (vk, label) => {
    keyEvent(vk, mapVirtualKey(vk, 0), extendedKeys.has(vk) ? 1 : 0, ptr(0));
    keyState.push({ time: Date.now(), vk, label, action: 'down' });
  };
  const up = (vk, label) => {
    keyEvent(vk, mapVirtualKey(vk, 0), (extendedKeys.has(vk) ? 1 : 0) | 2, ptr(0));
    keyState.push({ time: Date.now(), vk, label, action: 'up' });
  };
  const schedule = (ms, fn) => setTimeout(fn, ms);
  // Confirm the main menu / briefing and enter the loaded level.
  schedule(1000, () => { down(0x0d, 'enter'); up(0x0d, 'enter'); });
  schedule(2500, () => { down(0x0d, 'enter'); up(0x0d, 'enter'); });
  // Forward (KEY_UP / VK_UP).
  schedule(5000, () => down(0x26, 'forward'));
  schedule(7500, () => up(0x26, 'forward'));
  // Crouch-forward (KEY_RIGHT_CTRL / VK_RCONTROL + VK_UP).
  schedule(8000, () => { down(0xa3, 'crouch'); down(0x26, 'forward-crouch'); });
  schedule(10500, () => { up(0x26, 'forward-crouch'); up(0xa3, 'crouch'); });
  // Strafe left, then right.
  schedule(11000, () => down(0x25, 'strafe-left'));
  schedule(12500, () => up(0x25, 'strafe-left'));
  schedule(13000, () => down(0x27, 'strafe-right'));
  schedule(14500, () => up(0x27, 'strafe-right'));
  // Backward.
  schedule(15000, () => down(0x28, 'backward'));
  schedule(16500, () => up(0x28, 'backward'));
  // Jump.
  schedule(17000, () => { down(0x4a, 'jump'); up(0x4a, 'jump'); });
}

function startWhenWindow(attempt) {
  const window = findIgiWindow();
  if (window !== null) {
    demoSteps.push({ step: 'window', ok: true, attempt, window: window.toString(),
                     caption: lastWindows });
    beginDemo(window);
    return;
  }
  if (attempt === 0 || attempt % 5 === 0) {
    demoSteps.push({ step: 'window-wait', attempt, windows: lastWindows,
                     all: allWindows() });
  }
  if (attempt < 30) {
    setTimeout(() => startWhenWindow(attempt + 1), 500);
  } else {
    demoSteps.push({ step: 'window', ok: false, error: 'window did not appear in 15s',
                     windows: lastWindows });
  }
}

rpc.exports = {
  snapshot() {
    return {
      ready, installed, failures, counts,
      variantHits,
      observationCount: observations.length,
      demoStarted, demoSteps, keyState, windows: lastWindows,
    };
  },
  windows() {
    return allWindows();
  },
  recent(count) {
    return observations.slice(Math.max(0, observations.length - (count || 20)));
  },
  table(humanAddress, count) {
    const human = ptr(humanAddress);
    if (!readable(human)) return { error: 'human is not readable' };
    const table = safe(() => ptr(human.add(0x338).readU32()), null);
    if (table === null || !readable(table)) return { error: 'animation table is not readable', human: human.toString() };
    const rows = [];
    const limit = Math.min(Math.max(count || 64, 1), 256);
    for (let state = 0; state < limit; state += 1) {
      rows.push({ state, clip: u32At(table, state * 8), loopFlag: u32At(table, state * 8 + 4) });
    }
    return { human: human.toString(), table: table.toString(), rows };
  },
  observedHumans() {
    const humans = Object.create(null);
    for (const item of observations) {
      if (item.human) humans[item.human] = (humans[item.human] || 0) + 1;
    }
    return humans;
  },
  focus() {
    const window = findIgiWindow();
    if (window === null) return { ok: false, windows: lastWindows };
    const setForeground = api('SetForegroundWindow', 'int', ['pointer']);
    const setFocus = api('SetFocus', 'pointer', ['pointer']);
    const bring = api('BringWindowToTop', 'int', ['pointer']);
    bring(window);
    setForeground(window);
    setFocus(window);
    return { ok: true, window: window.toString(), foreground: api('GetForegroundWindow', 'pointer', [])().toString() };
  },
  press(vk, holdMs) {
    const keyEvent = api('keybd_event', 'void', ['uint8', 'uint8', 'uint32', 'pointer']);
    const mapVirtualKey = api('MapVirtualKeyW', 'uint32', ['uint32', 'uint32']);
    const extendedKeys = new Set([0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0xa3]);
    const scan = mapVirtualKey(vk, 0);
    const flags = extendedKeys.has(vk) ? 1 : 0;
    keyEvent(vk, scan, flags, ptr(0));
    keyState.push({ time: Date.now(), vk, scan, action: 'down' });
    setTimeout(() => {
      keyEvent(vk, scan, flags | 2, ptr(0));
      keyState.push({ time: Date.now(), vk, scan, action: 'up' });
    }, Math.max(1, holdMs | 0));
    return { vk, scan, holdMs, extended: flags !== 0 };
  },
  demo() {
    if (demoStarted) throw new Error('demo already started');
    demoStarted = true;
    startWhenWindow(0);
    return { ok: true, scheduled: true, window: lastWindows };
  },
};

send({ kind: 'ready', moduleBase: imageBase.toString(), installed });