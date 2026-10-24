'use strict';
// Generic per-native call probe for Project IGI retail/candidate differential work.
//
// One probe file is used UNCHANGED against both executables. The driver resolves
// the target from the main image base, so a 0x00400000-based retail VA and the
// same native's candidate address are never compared as raw addresses.
//
// Driver contract (scripts/tester/frida_probe_driver.py):
//   rpc.exports.hook(address, layout) -> installed hook description
//   layout = {
//     preferredBase: 0x00400000,       // PE preferred image base of THIS executable
//     argWords: 4,                     // bounded number of cdecl stack words to record
//     memArg: null | {index, offset, length},  // one pointer argument to snapshot
//     maxCalls: 256,                   // hard bound; overflow is reported, not truncated
//   }
// Messages:
//   {type:'observed', callIndex, address, phase:'entry'|'return', ...}
//   {type:'overflow', address, maxCalls}
//   {type:'hook_failed', address, error}
//
// Safety: no unknown pointer is dereferenced. A memory snapshot is only read
// after Process.findRangeByAddress proves the whole requested range readable.
// Every observation reports how each value was obtained (or why it was not).

const TARGET = { address: '0x004F4850', name: 'Sys_GetGlobal8_4F4850' };
const MAX_SNAPSHOT_LENGTH = 4096;
const UNAVAILABLE = 'unavailable';

let installed = null;
let callCount = 0;
let overflowReported = false;
const summary = {
  address: TARGET.address,
  name: TARGET.name,
  hooksInstalled: 0,
  callsObserved: 0,
  entryMessages: 0,
  returnMessages: 0,
  overflow: false,
  errors: [],
};

function mainModule(preferredName) {
  // Never assume enumerateModules()[0]: under Wine the first module is a DLL.
  if (preferredName) {
    const direct = Process.findModuleByName(preferredName);
    if (direct !== null) return direct;
    const wanted = String(preferredName).toLowerCase();
    const matches = Process.enumerateModules().filter(
      module => module.name.toLowerCase() === wanted);
    if (matches.length === 1) return matches[0];
  }
  for (const module of Process.enumerateModules()) {
    const low = module.name.toLowerCase();
    if (low.endsWith('.exe')) return module;
  }
  throw new Error('executable module not found; modules=' +
                  Process.enumerateModules().map(m => m.name).join(','));
}

function hex(value) {
  try { return '0x' + value.toString(16); } catch (error) { return UNAVAILABLE; }
}

function readWordAt(address, description) {
  try {
    const value = address.readU32();
    return { value: value >>> 0, hex: hex(value), source: description };
  } catch (error) {
    return { value: null, source: description, error: String(error) };
  }
}

function readArgumentWords(esp, count) {
  const words = [];
  for (let index = 0; index < count; index += 1) {
    const address = esp.add(4 + 4 * index);
    words.push(readWordAt(address, 'stack[esp+4+' + 4 * index + ']'));
  }
  return words;
}

// Read one pointer argument's target bytes only after proving the range readable.
function snapshotPointer(pointerValue, offset, length) {
  const result = { requestedLength: length, offset: offset, status: 'unavailable' };
  if (pointerValue === null || pointerValue === 0) {
    result.status = 'null-pointer';
    return result;
  }
  let base;
  try { base = ptr(pointerValue >>> 0); } catch (error) {
    result.status = 'invalid-pointer';
    result.error = String(error);
    return result;
  }
  const start = base.add(offset);
  let range = null;
  try { range = Process.findRangeByAddress(start); } catch (error) { range = null; }
  if (range === null) {
    result.status = 'unmapped';
    return result;
  }
  const available = range.base.add(range.size).sub(start).toInt32();
  if (available < length) {
    result.status = 'short-range';
    result.available = available;
    return result;
  }
  try {
    const bytes = start.readByteArray(length);
    const view = new Uint8Array(bytes);
    let text = '';
    for (let i = 0; i < view.length; i += 1) {
      text += (view[i] < 16 ? '0' : '') + view[i].toString(16);
    }
    result.status = 'read';
    result.address = hex(pointerValue);
    result.hex = text;
  } catch (error) {
    result.status = 'read-error';
    result.error = String(error);
  }
  return result;
}

function argWordValue(words, index) {
  if (index < 0 || index >= words.length) return null;
  const word = words[index];
  return word && typeof word.value === 'number' ? word.value : null;
}

function readWatch(watch) {
  if (!watch || watch.length === 0) return null;
  const results = [];
  for (const item of watch) {
    const resolved = mainModule().base.add(item.address.sub(installed.preferredBase));
    const target = resolved.add(item.offset);
    const record = { requestedAddress: hex(item.address), resolvedAddress: hex(target),
                     offset: item.offset, length: item.length, status: 'unavailable' };
    let range = null;
    try { range = Process.findRangeByAddress(target); } catch (error) { range = null; }
    if (range === null) {
      record.status = 'unmapped';
    } else {
      const available = range.base.add(range.size).sub(target).toInt32();
      if (available < item.length) {
        record.status = 'short-range';
        record.available = available;
      } else {
        try {
          const view = new Uint8Array(target.readByteArray(item.length));
          let text = '';
          for (let i = 0; i < view.length; i += 1) text += (view[i] < 16 ? '0' : '') + view[i].toString(16);
          record.status = 'read';
          record.hex = text;
          record.word = item.length >= 4 ? target.readU32() >>> 0 : null;
        } catch (error) {
          record.status = 'read-error';
          record.error = String(error);
        }
      }
    }
    results.push(record);
  }
  return results;
}


function snapshotFromLayout(layout, words) {
  const spec = layout.memArg;
  if (!spec) return null;
  const pointer = argWordValue(words, spec.index);
  return {
    argumentIndex: spec.index,
    offset: spec.offset,
    pointer: pointer === null ? null : hex(pointer),
    snapshot: pointer === null
      ? { requestedLength: spec.length, offset: spec.offset, status: 'argument-unavailable' }
      : snapshotPointer(pointer, spec.offset, spec.length),
  };
}

function install(address, layout) {
  const main = mainModule(layout.moduleName);
  const preferredBase = layout.preferredBase;
  const va = ptr(address);
  const rva = va.sub(preferredBase);
  const target = main.base.add(rva);
  const argWords = Math.max(0, Math.min(16, layout.argWords | 0));
  const maxCalls = Math.max(1, Math.min(4096, layout.maxCalls | 0));
  const memSpec = layout.memArg
    ? {
      index: Math.max(0, Math.min(15, layout.memArg.index | 0)),
      offset: Math.max(0, layout.memArg.offset | 0),
      length: Math.max(1, Math.min(MAX_SNAPSHOT_LENGTH, layout.memArg.length | 0)),
    }
    : null;
  const returnMemSpec = layout.returnMem
    ? {
      offset: Math.max(0, layout.returnMem.offset | 0),
      length: Math.max(1, Math.min(MAX_SNAPSHOT_LENGTH, layout.returnMem.length | 0)),
    }
    : null;
  const watchSpec = Array.isArray(layout.watch) ? layout.watch.slice(0, 8).map(item => ({
    address: ptr(item.address), offset: Math.max(0, item.offset | 0),
    length: Math.max(1, Math.min(MAX_SNAPSHOT_LENGTH, item.length | 0)),
  })) : [];
  const hookLayout = { preferredBase: preferredBase, argWords: argWords, memArg: memSpec,
                       returnMem: returnMemSpec, watch: watchSpec, maxCalls: maxCalls };

  let pending = null;
  const listener = Interceptor.attach(target, {
    onEnter(args) {
      callCount += 1;
      if (callCount > maxCalls) {
        if (!overflowReported) {
          overflowReported = true;
          summary.overflow = true;
          send({ type: 'overflow', address: TARGET.address, maxCalls: maxCalls });
        }
        pending = null;
        return;
      }
      const callIndex = callCount;
      let esp = null;
      try { esp = this.context.esp; } catch (error) { esp = null; }
      if (esp === null) {
        pending = null;
        summary.errors.push('entry context.esp unavailable');
        send({ type: 'observed', callIndex: callIndex, address: TARGET.address, phase: 'entry',
               args: [], esp: UNAVAILABLE, error: 'entry context.esp unavailable' });
        return;
      }
      const words = readArgumentWords(esp, argWords);
      let caller = UNAVAILABLE;
      try { caller = hex(this.returnAddress); } catch (error) { caller = UNAVAILABLE; }
      pending = { callIndex: callIndex, esp: hex(esp), words: words, caller: caller,
                  at: Date.now() };
      summary.entryMessages += 1;
      send({
        type: 'observed', callIndex: callIndex, address: TARGET.address, phase: 'entry',
        esp: hex(esp), caller: caller, at: pending.at, args: words,
        memory: snapshotFromLayout(hookLayout, words),
        watch: readWatch(hookLayout.watch),
      });
    },
    onLeave(retval) {
      if (pending === null) return;
      const record = pending;
      pending = null;
      let eax = UNAVAILABLE;
      let eaxValue = null;
      try {
        const raw = this.context.eax;
        eaxValue = raw >>> 0;
        eax = hex(raw);
      } catch (error) { /* recorded as unavailable */ }
      summary.returnMessages += 1;
      send({
        type: 'observed', callIndex: record.callIndex, address: TARGET.address, phase: 'return',
        esp: record.esp, caller: record.caller, at: record.at, args: record.words,
        eax: eax, eaxValue: eaxValue,
        memory: snapshotFromLayout(hookLayout, record.words),
        returnMemory: hookLayout.returnMem !== null && eaxValue !== null
          ? { offset: hookLayout.returnMem.offset,
              snapshot: snapshotPointer(eaxValue, hookLayout.returnMem.offset, hookLayout.returnMem.length) }
          : null,
        watch: readWatch(hookLayout.watch),
      });
    },
  });
  installed = { address: TARGET.address, name: TARGET.name, module: main.name,
                moduleBase: hex(main.base), target: hex(target), rva: hex(rva),
                preferredBase: preferredBase, argWords: argWords, memArg: memSpec,
                maxCalls: maxCalls };
  summary.hooksInstalled += 1;
  return installed;
}

rpc.exports = {
  hook(address, layout) {
    try {
      return install(address, layout);
    } catch (error) {
      const message = String(error);
      summary.errors.push(message);
      send({ type: 'hook_failed', address: address, error: message });
      throw error;
    }
  },
  summary() {
    summary.callsObserved = callCount;
    return summary;
  },
  detach() {
    try { if (installed !== null) Interceptor.detachAll(); } catch (error) { summary.errors.push(String(error)); }
    return summary;
  },
  target() { return TARGET; },
};
