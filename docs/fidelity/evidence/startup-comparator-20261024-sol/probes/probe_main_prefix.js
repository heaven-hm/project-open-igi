'use strict';
// Main-prefix differential probe (Project IGI retail vs candidate).
//
// Generated per capture from scripts/tester/build_main_prefix_gate.py: a JSON
// config is substituted for __MAIN_PREFIX_CONFIG_JSON__. The driver calls
// rpc.exports.hook(address, layout) once. Only calls whose return address lies
// inside the configured main-prefix range are recorded, so nested callee calls
// and everything after the hard stop are excluded rather than guessed at.
// Only packet/static-proven C-string addresses are ever dereferenced; struct
// snapshots are bounded and range-checked.

const CONFIG = {
  "target": {
    "va": "0x0048F530",
    "name": "main"
  },
  "moduleName": "igi.exe",
  "preferredBase": "0x00400000",
  "start": "0x0048F530",
  "hardStop": "0x0048F7AA",
  "argWords": 4,
  "maxEvents": 8192,
  "provenStrings": [
    {
      "va": "0x00541ECC",
      "text": "IGIMUTEX"
    },
    {
      "va": "0x00541E60",
      "text": "Error"
    },
    {
      "va": "0x00541DBC",
      "text": "IGI is already running."
    },
    {
      "va": "0x00541E48",
      "text": "Couldn't open window"
    },
    {
      "va": "0x00541E68",
      "text": "This application requires DirectX version %d.%d or greater to run."
    },
    {
      "va": "0x0053B534",
      "text": "Innerloop"
    },
    {
      "va": "0x00541EC0",
      "text": "filesys.cfg"
    },
    {
      "va": "0x00541EB8",
      "text": "Window"
    },
    {
      "va": "0x00541EAC",
      "text": "WinInput"
    },
    {
      "va": "0x00541D60",
      "text": "Germany"
    },
    {
      "va": "0x005382C8",
      "text": "USA"
    },
    {
      "va": "0x00541E38",
      "text": "HardcoreGerman"
    },
    {
      "va": "0x00541DD4",
      "text": "Invalid FPU rounding mode. May be caused by running a debug session with ftol optimize turned on."
    },
    {
      "va": "0x00567C74",
      "text": ""
    }
  ],
  "globals": [
    {
      "label": "clear_range_5c8d28",
      "va": "0x005C8D28",
      "length": 192
    },
    {
      "label": "ptr_chain_5c8df0",
      "va": "0x005C8DF0",
      "length": 12
    },
    {
      "label": "window_handle_5c8bc4",
      "va": "0x005C8BC4",
      "length": 4
    },
    {
      "label": "task_id_byte_5c8de8",
      "va": "0x005C8DE8",
      "length": 1
    }
  ],
  "calls": [
    {
      "name": "CreateMutexA",
      "kind": "import",
      "iat": "0x00533124",
      "argWords": 3
    },
    {
      "name": "GetLastError",
      "kind": "import",
      "iat": "0x00533128",
      "argWords": 0
    },
    {
      "name": "CoInitialize",
      "kind": "import",
      "iat": "0x0053321C",
      "argWords": 1
    },
    {
      "name": "GetForegroundWindow",
      "kind": "import",
      "iat": "0x005331D8",
      "argWords": 0
    },
    {
      "name": "LoadIconA",
      "kind": "import",
      "iat": "0x005331A4",
      "argWords": 2
    },
    {
      "name": "LoadCursorA",
      "kind": "import",
      "iat": "0x005331A8",
      "argWords": 2
    },
    {
      "name": "RegisterClassA",
      "kind": "import",
      "iat": "0x005331AC",
      "argWords": 1,
      "struct": {
        "slot": 0,
        "offset": 0,
        "length": 40,
        "label": "wndclass"
      }
    },
    {
      "name": "MessageBoxA",
      "kind": "import",
      "iat": "0x005331B0",
      "argWords": 4
    },
    {
      "name": "GetSystemMetrics",
      "kind": "import",
      "iat": "0x005331E8",
      "argWords": 1
    },
    {
      "name": "CreateWindowExA",
      "kind": "import",
      "iat": "0x005331B4",
      "argWords": 12
    },
    {
      "name": "ShowCursor",
      "kind": "import",
      "iat": "0x005331B8",
      "argWords": 1
    },
    {
      "name": "ShowWindow",
      "kind": "import",
      "iat": "0x005331BC",
      "argWords": 2
    },
    {
      "name": "StoreCommandLine",
      "kind": "native",
      "va": "0x0048F0C0",
      "argWords": 1
    },
    {
      "name": "SetAppPath(?)",
      "kind": "native",
      "va": "0x0048F0D0",
      "argWords": 1
    },
    {
      "name": "StartupSetter_48F0F0",
      "kind": "native",
      "va": "0x0048F0F0",
      "argWords": 1
    },
    {
      "name": "StartupSetter_48F100",
      "kind": "native",
      "va": "0x0048F100",
      "argWords": 1
    },
    {
      "name": "StartupSetter_48F140",
      "kind": "native",
      "va": "0x0048F140",
      "argWords": 1
    },
    {
      "name": "StartupSetter_48F120",
      "kind": "native",
      "va": "0x0048F120",
      "argWords": 1
    },
    {
      "name": "StartupSetter_48F1A0",
      "kind": "native",
      "va": "0x0048F1A0",
      "argWords": 1
    },
    {
      "name": "StartupSetter_48F1C0",
      "kind": "native",
      "va": "0x0048F1C0",
      "argWords": 1
    },
    {
      "name": "StartupSetter_48F1E0",
      "kind": "native",
      "va": "0x0048F1E0",
      "argWords": 1
    },
    {
      "name": "StartupSetter_48F240",
      "kind": "native",
      "va": "0x0048F240",
      "argWords": 1
    },
    {
      "name": "StartupSetter_48F260",
      "kind": "native",
      "va": "0x0048F260",
      "argWords": 1
    },
    {
      "name": "StartupSetter_48F280",
      "kind": "native",
      "va": "0x0048F280",
      "argWords": 1
    },
    {
      "name": "StartupSetter_48F290",
      "kind": "native",
      "va": "0x0048F290",
      "argWords": 1
    },
    {
      "name": "StartupSetter_48F2A0",
      "kind": "native",
      "va": "0x0048F2A0",
      "argWords": 1
    },
    {
      "name": "StartupSetter_48F210",
      "kind": "native",
      "va": "0x0048F210",
      "argWords": 1
    },
    {
      "name": "StartupSetter_48F2B0",
      "kind": "native",
      "va": "0x0048F2B0",
      "argWords": 1
    },
    {
      "name": "StartupSetter_48F300",
      "kind": "native",
      "va": "0x0048F300",
      "argWords": 1
    },
    {
      "name": "StartupSetter_48F200",
      "kind": "native",
      "va": "0x0048F200",
      "argWords": 1
    },
    {
      "name": "AppMain_ParseCmdLineArgs",
      "kind": "native",
      "va": "0x0048F360",
      "argWords": 2
    },
    {
      "name": "Display_DinputDll",
      "kind": "native",
      "va": "0x00498480",
      "argWords": 2
    },
    {
      "name": "sprintf",
      "kind": "native",
      "va": "0x004A53B3",
      "argWords": 3
    },
    {
      "name": "Camera_ApplyBandFilter_6",
      "kind": "native",
      "va": "0x004A5AD5",
      "argWords": 1
    },
    {
      "name": "Display_GetScreenMode(?)",
      "kind": "native",
      "va": "0x0048F130",
      "argWords": 0
    },
    {
      "name": "StoreWindowHandle",
      "kind": "native",
      "va": "0x0048F0E0",
      "argWords": 1
    },
    {
      "name": "Display_ClearSurfaces_16",
      "kind": "native",
      "va": "0x00498460",
      "argWords": 0
    },
    {
      "name": "QTask_CreateTask",
      "kind": "native",
      "va": "0x00401000",
      "argWords": 0,
      "snapshotTag": "qtask"
    }
  ]
};
const UNAVAILABLE = 'unavailable';

let mainModule = null;
let preferredBase = null;
let listeners = [];
let targetDepth = 0;
let sequence = 0;
let entryCount = 0;
let returnCount = 0;
let overflowed = false;
let failures = [];
let installations = [];

function hexNumber(value) {
  try { return '0x' + (value >>> 0).toString(16); } catch (error) { return UNAVAILABLE; }
}

function toNumber(value) {
  try { return value >>> 0; } catch (error) { return null; }
}

function resolveVA(va) {
  const numeric = typeof va === 'number' ? va : parseInt(String(va), 16);
  return mainModule.base.add(numeric - preferredBase);
}

function readWordAt(address, label) {
  try {
    if (!Process.findRangeByAddress(address)) {
      return { label: label, status: 'unmapped', value: null, hex: null };
    }
    return { label: label, status: 'read', value: address.readU32(), hex: hexNumber(address.readU32()) };
  } catch (error) {
    return { label: label, status: 'unreadable', value: null, hex: null };
  }
}

function readArgumentWords(context, count) {
  const words = [];
  for (let index = 0; index < count; index += 1) {
    let address = null;
    try { address = context.esp.add(4 + 4 * index); } catch (error) { address = null; }
    words.push(address === null
      ? { label: 'stack[esp+4+' + 4 * index + ']', status: 'no-context', value: null, hex: null }
      : readWordAt(address, 'stack[esp+4+' + 4 * index + ']'));
  }
  return words;
}

// Bounded snapshot; returns unavailable instead of dereferencing unknown memory.
function snapshotPointer(pointerValue, offset, length) {
  const result = { offset: offset, requestedLength: length, status: UNAVAILABLE };
  if (pointerValue === null || pointerValue === undefined || pointerValue === 0) {
    result.status = 'null-pointer';
    return result;
  }
  let base;
  try { base = ptr(pointerValue >>> 0); } catch (error) { result.status = 'invalid-pointer'; return result; }
  let target;
  try { target = base.add(offset); } catch (error) { result.status = 'invalid-offset'; return result; }
  try {
    if (!Process.findRangeByAddress(target)) { result.status = 'unmapped'; return result; }
    const bytes = target.readByteArray(length);
    if (bytes === null) { result.status = 'short-read'; return result; }
    result.status = 'read';
    result.hex = Array.from(new Uint8Array(bytes), b => ('0' + b.toString(16)).slice(-2)).join('');
    return result;
  } catch (error) {
    result.status = 'unreadable';
    return result;
  }
}

function provenStringAt(value) {
  if (value === null || value === undefined) return null;
  const numeric = toNumber(value);
  if (numeric === null) return null;
  for (const entry of CONFIG.provenStrings) {
    if (parseInt(entry.va, 16) !== numeric) continue;
    let text = null;
    try {
      if (Process.findRangeByAddress(ptr(numeric))) text = ptr(numeric).readCString();
    } catch (error) { text = null; }
    return { va: entry.va, text: text === null ? UNAVAILABLE : text };
  }
  return null;
}

function collectStrings(words) {
  const strings = [];
  words.forEach((word, index) => {
    const found = word && word.status === 'read' ? provenStringAt(word.value) : null;
    if (found !== null) strings.push({ slot: index, va: found.va, text: found.text });
  });
  return strings;
}

function collectStructStrings(snapshot) {
  const strings = [];
  if (!snapshot || snapshot.status !== 'read' || !snapshot.hex) return strings;
  for (let offset = 0; offset < snapshot.hex.length; offset += 8) {
    const byte0 = parseInt(snapshot.hex.slice(offset, offset + 2), 16);
    const byte1 = parseInt(snapshot.hex.slice(offset + 2, offset + 4), 16);
    const byte2 = parseInt(snapshot.hex.slice(offset + 4, offset + 6), 16);
    const byte3 = parseInt(snapshot.hex.slice(offset + 6, offset + 8), 16);
    const pointer = (byte0 | (byte1 << 8) | (byte2 << 16) | (byte3 << 24)) >>> 0;
    const found = provenStringAt(pointer);
    if (found !== null) strings.push({ slot: 'struct+0x' + (offset / 2).toString(16), va: found.va, text: found.text });
  }
  return strings;
}

function readGlobalSnapshots() {
  return (CONFIG.globals || []).map(global => {
    const snapshot = snapshotPointer(parseInt(global.va, 16), 0, global.length);
    return { label: global.label, va: global.va, length: global.length,
             status: snapshot.status, hex: snapshot.hex || null };
  });
}

function emit(message) {
  if (overflowed) return;
  if (sequence >= CONFIG.maxEvents) {
    overflowed = true;
    send({ type: 'overflow', maxEvents: CONFIG.maxEvents });
    return;
  }
  sequence += 1;
  message.sequence = sequence;
  send(message);
}

function inPrefix(caller) {
  const numeric = toNumber(caller);
  if (numeric === null) return false;
  // The return address of a call is callsite + instruction length. The hard-stop
  // call at 0x0048F7AA must be included (its return address is 0x0048F7AF); a
  // six-byte `call [mem]` form is the widest accepted instruction length here.
  return numeric >= parseInt(CONFIG.start, 16) &&
         numeric <= parseInt(CONFIG.hardStop, 16) + 6;
}

function installCall(spec) {
  let address = null;
  if (spec.kind === 'import') {
    try {
      const slot = resolveVA(spec.iat);
      if (!Process.findRangeByAddress(slot)) throw new Error('IAT slot unmapped');
      address = slot.readPointer();
      if (address.isNull()) throw new Error('IAT entry null');
    } catch (error) {
      failures.push({ name: spec.name, kind: spec.kind, va: spec.iat, error: String(error) });
      return;
    }
  } else {
    try { address = resolveVA(spec.va); } catch (error) {
      failures.push({ name: spec.name, kind: spec.kind, va: spec.va, error: String(error) });
      return;
    }
  }
  try {
    const listener = Interceptor.attach(address, {
      onEnter: function () {
        if (targetDepth <= 0 || overflowed) return;
        let caller = null;
        try { caller = this.returnAddress; } catch (error) { caller = null; }
        if (caller === null || !inPrefix(caller)) return;
        const words = readArgumentWords(this.context, spec.argWords || 0);
        const record = {
          type: 'api_call', name: spec.name, kind: spec.kind,
          target: spec.va || spec.iat, callsite: hexNumber(caller),
          argWords: words, strings: collectStrings(words),
          struct: null, structStrings: [],
        };
        if (spec.struct) {
          const pointer = words[spec.struct.slot || 0];
          if (pointer && pointer.status === 'read') {
            record.struct = snapshotPointer(pointer.value, spec.struct.offset || 0, spec.struct.length || 0x28);
            record.struct.label = spec.struct.label || spec.name;
            record.structStrings = collectStructStrings(record.struct);
          } else {
            record.struct = { label: spec.struct.label || spec.name, status: 'argument-unavailable' };
          }
        }
        this.__mainProbeCall = record;
        emit(record);
        if (spec.snapshotTag) {
          emit({ type: 'globals', tag: spec.snapshotTag, phase: 'call', snapshots: readGlobalSnapshots() });
        }
      },
      onLeave: function () {
        const record = this.__mainProbeCall;
        if (!record) return;
        this.__mainProbeCall = null;
        let eax = null;
        try { eax = this.context.eax >>> 0; } catch (error) { eax = null; }
        emit({ type: 'api_return', name: record.name, kind: record.kind,
               callsite: record.callsite, returnValue: eax });
        if (spec.snapshotTag) {
          emit({ type: 'globals', tag: spec.snapshotTag, phase: 'return', snapshots: readGlobalSnapshots() });
        }
      },
    });
    listeners.push(listener);
    installations.push({ name: spec.name, kind: spec.kind, va: spec.va || spec.iat,
                         resolved: hexNumber(address) });
  } catch (error) {
    failures.push({ name: spec.name, kind: spec.kind, va: spec.va || spec.iat, error: String(error) });
  }
}

rpc.exports = {
  hook: function (address, layout) {
    layout = layout || {};
    preferredBase = parseInt(CONFIG.preferredBase, 16);
    const moduleName = layout.moduleName || CONFIG.moduleName;
    mainModule = Process.findModuleByName(moduleName);
    if (mainModule === null) {
      const modules = Process.enumerateModules();
      mainModule = modules.length > 0 ? modules[0] : null;
    }
    if (mainModule === null) throw new Error('main module not found');
    let target = null;
    try { target = resolveVA(address); } catch (error) {
      failures.push({ name: CONFIG.target.name, kind: 'target', va: address, error: String(error) });
      throw error;
    }

    try {
      const targetListener = Interceptor.attach(target, {
        onEnter: function () {
          targetDepth += 1;
          entryCount += 1;
          emit({ type: 'entry', address: CONFIG.target.va, name: CONFIG.target.name,
                 argWords: readArgumentWords(this.context, CONFIG.argWords || 0),
                 callerReturnAddress: hexNumber(this.returnAddress) });
          emit({ type: 'globals', tag: 'entry', phase: 'call', snapshots: readGlobalSnapshots() });
        },
        onLeave: function () {
          targetDepth = Math.max(0, targetDepth - 1);
          returnCount += 1;
          let eax = null;
          try { eax = this.context.eax >>> 0; } catch (error) { eax = null; }
          emit({ type: 'return', address: CONFIG.target.va, returnValue: eax });
        },
      });
      listeners.push(targetListener);
    } catch (error) {
      failures.push({ name: CONFIG.target.name, kind: 'target', va: CONFIG.target.va,
                      error: String(error) });
      throw error;
    }

    for (const spec of CONFIG.calls) installCall(spec);
    return { address: CONFIG.target.va, name: CONFIG.target.name, module: mainModule.name,
             moduleBase: hexNumber(mainModule.base), target: hexNumber(target),
             preferredBase: hexNumber(preferredBase), calls: installations, failures: failures };
  },
  summary: function () {
    // Summary must stay a small plain-value object: returning Listener/Error
    // objects over RPC can stall the Frida main loop on Wine.
    return { target: CONFIG.target, entryCount: entryCount, returnCount: returnCount,
             sequence: sequence, overflowed: overflowed, listenerCount: listeners.length,
             installationCount: installations.length, failureCount: failures.length };
  },
  detach: function () {
    for (const listener of listeners) {
      try { listener.detach(); } catch (error) { /* already detached */ }
    }
    listeners = [];
    return { detached: true };
  },
};
