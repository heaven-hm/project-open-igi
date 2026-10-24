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
    "va": "0x4182e0",
    "name": "main"
  },
  "moduleName": "igi1.exe",
  "preferredBase": "0x00400000",
  "start": "0x4182e0",
  "hardStop": "0x418635",
  "argWords": 4,
  "maxEvents": 8192,
  "provenStrings": [
    {
      "va": "0x5681b0",
      "text": "IGI1MUTEX"
    },
    {
      "va": "0x5681ba",
      "text": "Error"
    },
    {
      "va": "0x5681c0",
      "text": "IGI is already running."
    },
    {
      "va": "0x56820b",
      "text": "Couldn't open window"
    },
    {
      "va": "0x5681d8",
      "text": "Innerloop"
    },
    {
      "va": "0x5681e2",
      "text": "filesys.cfg"
    },
    {
      "va": "0x5681ef",
      "text": "Window"
    },
    {
      "va": "0x5681f6",
      "text": "WinInput"
    },
    {
      "va": "0x5681ff",
      "text": "Germany"
    },
    {
      "va": "0x568207",
      "text": "USA"
    },
    {
      "va": "0x568220",
      "text": "HardcoreGerman"
    }
  ],
  "globals": [
    {
      "label": "window_handle_g_5c8bc4",
      "va": "0x5a3e40",
      "length": 4
    },
    {
      "label": "window_handler_chain_5c8df0",
      "va": "0x5a3f40",
      "length": 4
    },
    {
      "label": "window_handler_terminator_5c8df4",
      "va": "0x5a3f44",
      "length": 12
    },
    {
      "label": "window_handler_tail_5c8df8",
      "va": "0x5a3f48",
      "length": 4
    },
    {
      "label": "loop_status_byte_5c8de8",
      "va": "0x5a3dd0",
      "length": 1
    },
    {
      "label": "debug_key_flag_5c8e04",
      "va": "0x5a3e60",
      "length": 4
    }
  ],
  "calls": [
    {
      "name": "CreateMutexA",
      "kind": "import",
      "iat": "0x6be43c",
      "argWords": 3
    },
    {
      "name": "GetLastError",
      "kind": "import",
      "iat": "0x6be464",
      "argWords": 0
    },
    {
      "name": "CoInitialize",
      "kind": "import",
      "iat": "0x6be6a4",
      "argWords": 1
    },
    {
      "name": "GetForegroundWindow",
      "kind": "import",
      "iat": "0x6be6c4",
      "argWords": 0
    },
    {
      "name": "LoadIconA",
      "kind": "import",
      "iat": "0x6be6d4",
      "argWords": 2
    },
    {
      "name": "LoadCursorA",
      "kind": "import",
      "iat": "0x6be6d0",
      "argWords": 2
    },
    {
      "name": "RegisterClassA",
      "kind": "import",
      "iat": "0x6be6e4",
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
      "iat": "0x6be6d8",
      "argWords": 4
    },
    {
      "name": "GetSystemMetrics",
      "kind": "import",
      "iat": "0x6be6cc",
      "argWords": 1
    },
    {
      "name": "CreateWindowExA",
      "kind": "import",
      "iat": "0x6be6b0",
      "argWords": 12
    },
    {
      "name": "ShowCursor",
      "kind": "import",
      "iat": "0x6be6f0",
      "argWords": 1
    },
    {
      "name": "ShowWindow",
      "kind": "import",
      "iat": "0x6be6f4",
      "argWords": 2
    },
    {
      "name": "StoreCommandLine",
      "kind": "native",
      "va": "0x416f00",
      "argWords": 1
    },
    {
      "name": "SetAppPath(?)",
      "kind": "native",
      "va": "0x416f20",
      "argWords": 1
    },
    {
      "name": "StartupSetter_48F0F0",
      "kind": "native",
      "va": "0x417f90",
      "argWords": 1
    },
    {
      "name": "StartupSetter_48F100",
      "kind": "native",
      "va": "0x4163f0",
      "argWords": 1
    },
    {
      "name": "StartupSetter_48F140",
      "kind": "native",
      "va": "0x417df0",
      "argWords": 1
    },
    {
      "name": "StartupSetter_48F120",
      "kind": "native",
      "va": "0x416c80",
      "argWords": 1
    },
    {
      "name": "StartupSetter_48F1A0",
      "kind": "native",
      "va": "0x416330",
      "argWords": 1
    },
    {
      "name": "StartupSetter_48F1C0",
      "kind": "native",
      "va": "0x416420",
      "argWords": 1
    },
    {
      "name": "StartupSetter_48F1E0",
      "kind": "native",
      "va": "0x416350",
      "argWords": 1
    },
    {
      "name": "StartupSetter_48F240",
      "kind": "native",
      "va": "0x416360",
      "argWords": 1
    },
    {
      "name": "StartupSetter_48F260",
      "kind": "native",
      "va": "0x416380",
      "argWords": 1
    },
    {
      "name": "StartupSetter_48F280",
      "kind": "native",
      "va": "0x416ca0",
      "argWords": 1
    },
    {
      "name": "StartupSetter_48F290",
      "kind": "native",
      "va": "0x416cb0",
      "argWords": 1
    },
    {
      "name": "StartupSetter_48F2A0",
      "kind": "native",
      "va": "0x416cc0",
      "argWords": 1
    },
    {
      "name": "StartupSetter_48F210",
      "kind": "native",
      "va": "0x416cd0",
      "argWords": 1
    },
    {
      "name": "StartupSetter_48F2B0",
      "kind": "native",
      "va": "0x416ce0",
      "argWords": 1
    },
    {
      "name": "StartupSetter_48F300",
      "kind": "native",
      "va": "0x416cf0",
      "argWords": 1
    },
    {
      "name": "StartupSetter_48F200",
      "kind": "native",
      "va": "0x416ef0",
      "argWords": 1
    },
    {
      "name": "AppMain_ParseCmdLineArgs",
      "kind": "native",
      "va": "0x4165d0",
      "argWords": 2
    },
    {
      "name": "Display_GetScreenMode(?)",
      "kind": "native",
      "va": "0x416c90",
      "argWords": 0
    },
    {
      "name": "StoreWindowHandle",
      "kind": "native",
      "va": "0x417f80",
      "argWords": 1
    },
    {
      "name": "FP_InitExtendedPrecision(0x498460 substitute)",
      "kind": "native",
      "va": "0x417d50",
      "argWords": 0
    },
    {
      "name": "QTask_CreateTask",
      "kind": "native",
      "va": "0x4180f0",
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
