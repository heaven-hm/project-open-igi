'use strict';
// Registrar-chain differential probe (Project IGI retail vs candidate).
//
// One probe file is generated per target from scripts/tester/build_registrar_gate.py:
// a JSON config block is substituted for __PROBE_CONFIG_JSON__. The driver calls
// rpc.exports.hook(address, layout) once; the probe resolves the address against
// the main module base so the same retail VA works in the candidate image.
//
// Observed evidence:
//   - target entry/return, bounded argument words, caller return address, EAX;
//   - packet-proven side effects (id stores, AL stores, the material-clear range)
//     snapshotted at entry and return; every read is range-checked first;
//   - ordered registration-API calls inside the target (address + argument words +
//     C-string arguments only for packet-proven literal addresses + return value);
//   - ordered chain calls for a list of expected (target, callsite) pairs, filtered
//     by the observed caller return address (order proof without BN names).
//
// Safety: no unknown pointer is dereferenced. Strings are read only when the
// argument word equals a literal VA proven by the static packet. Every observation
// records how the value was obtained, or why it was not.
//
// Messages:
//   {type:'observed', phase:'entry'|'return', ...}
//   {type:'registration', ...}
//   {type:'chain_call', ...}
//   {type:'overflow', ...}
//   {type:'hook_failed', ...}

const CONFIG = __PROBE_CONFIG_JSON__;
const MAX_SNAPSHOT_LENGTH = 4096;
const MAX_STRING_LENGTH = 96;
const UNAVAILABLE = 'unavailable';

let installed = null;
let sequence = 0;
let targetDepth = 0;
let entryCount = 0;
let returnCount = 0;
let overflowed = false;
const counters = { registrations: 0, chainCalls: 0, chainMismatches: 0,
                   chainInstallFailures: [], errors: [], hooks: 0 };

function mainModule(preferredName) {
  if (preferredName) {
    const direct = Process.findModuleByName(preferredName);
    if (direct !== null) return direct;
    const wanted = String(preferredName).toLowerCase();
    const matches = Process.enumerateModules().filter(m => m.name.toLowerCase() === wanted);
    if (matches.length === 1) return matches[0];
  }
  for (const module of Process.enumerateModules()) {
    if (module.name.toLowerCase().endsWith('.exe')) return module;
  }
  throw new Error('executable module not found; modules=' +
                  Process.enumerateModules().map(m => m.name).join(','));
}

function hex(value) {
  try { return '0x' + value.toString(16); } catch (error) { return UNAVAILABLE; }
}

function resolveVA(va) {
  if (installed === null) throw new Error('probe not installed');
  const absolute = ptr(va);
  return installed.base.add(absolute.sub(installed.preferredBase));
}

function sameResolved(vaString) {
  try { return resolveVA(vaString); } catch (error) { return null; }
}

function readRangeChecked(address, length) {
  const record = { address: hex(address), length: length, status: 'unavailable' };
  let range = null;
  try { range = Process.findRangeByAddress(address); } catch (error) { range = null; }
  if (range === null) { record.status = 'unmapped'; return record; }
  const available = range.base.add(range.size).sub(address).toInt32();
  if (available < length) { record.status = 'short-range'; record.available = available; return record; }
  try {
    const bytes = new Uint8Array(address.readByteArray(length));
    let text = '';
    for (let i = 0; i < bytes.length; i += 1) text += (bytes[i] < 16 ? '0' : '') + bytes[i].toString(16);
    record.status = 'read';
    record.hex = text;
    if (length >= 4) record.word = address.readU32() >>> 0;
  } catch (error) { record.status = 'read-error'; record.error = String(error); }
  return record;
}

function readSideEffects() {
  if (!CONFIG.sideEffects || CONFIG.sideEffects.length === 0) return null;
  const results = [];
  for (const effect of CONFIG.sideEffects) {
    const resolved = resolveVA(effect.va);
    const record = readRangeChecked(resolved, Math.max(1, Math.min(MAX_SNAPSHOT_LENGTH, effect.length | 0)));
    record.label = effect.label;
    record.va = effect.va;
    if (record.status === 'read' && effect.length >= 4) record.word = resolved.readU32() >>> 0;
    results.push(record);
  }
  return results;
}

function readArgumentWords(context, count) {
  const words = [];
  let esp;
  try { esp = context.esp || context.sp; } catch (error) { esp = null; }
  if (!esp) return words;
  for (let index = 0; index < count; index += 1) {
    const address = esp.add(4 + 4 * index);
    try { words.push(ptr(address).readU32() >>> 0); }
    catch (error) { words.push(null); }
  }
  return words;
}

function provenString(pointerValue) {
  if (pointerValue === null || typeof pointerValue !== 'number' || pointerValue === 0) return null;
  if (!CONFIG.provenStrings || CONFIG.provenStrings.length === 0) return null;
  const matched = CONFIG.provenStrings.some(va => parseInt(va, 16) === pointerValue);
  if (!matched) return null;
  try {
    const address = ptr(pointerValue);
    const range = Process.findRangeByAddress(address);
    if (range === null) return { va: '0x' + pointerValue.toString(16), status: 'unmapped' };
    let length = MAX_STRING_LENGTH;
    const available = range.base.add(range.size).sub(address).toInt32();
    if (available < 1) return { va: '0x' + pointerValue.toString(16), status: 'short-range' };
    if (available < length) length = available;
    const bytes = new Uint8Array(address.readByteArray(length));
    let text = '';
    for (let i = 0; i < bytes.length; i += 1) {
      if (bytes[i] === 0) break;
      text += String.fromCharCode(bytes[i]);
    }
    return { va: '0x' + pointerValue.toString(16), status: 'read', text: text };
  } catch (error) {
    return { va: '0x' + pointerValue.toString(16), status: 'read-error', error: String(error) };
  }
}

function emit(payload) {
  if (overflowed) return;
  if (sequence >= installed.maxEvents) {
    overflowed = true;
    send({ type: 'overflow', address: CONFIG.target.va, maxEvents: installed.maxEvents });
    return;
  }
  sequence += 1;
  payload.order = sequence;
  send(payload);
}

function installTargetHook(target, layout) {
  Interceptor.attach(target, {
    onEnter: function (args) {
      if (overflowed) return;
      entryCount += 1;
      targetDepth += 1;
      this.__probeCallIndex = entryCount;
      this.__probeWords = readArgumentWords(this.context, installed.argWords);
      this.__probeCaller = null;
      try { this.__probeCaller = this.returnAddress ? this.returnAddress.toString() : null; } catch (error) { this.__probeCaller = null; }
      installed.entryCaller = this.__probeCaller;
      const message = {
        type: 'observed', phase: 'entry', address: CONFIG.target.va, name: CONFIG.target.name,
        callIndex: this.__probeCallIndex, argWords: this.__probeWords,
        callerReturnAddress: this.__probeCaller,
        sideEffects: readSideEffects(),
      };
      emit(message);
    },
    onLeave: function (retval) {
      if (overflowed) return;
      returnCount += 1;
      targetDepth = Math.max(0, targetDepth - 1);
      const message = {
        type: 'observed', phase: 'return', address: CONFIG.target.va, name: CONFIG.target.name,
        callIndex: this.__probeCallIndex, argWords: this.__probeWords,
        callerReturnAddress: this.__probeCaller,
        returnValue: null, eax: null,
        sideEffects: readSideEffects(),
      };
      try {
        const eax = this.context.eax >>> 0;
        message.returnValue = eax;
        message.eax = '0x' + eax.toString(16);
      } catch (error) { /* recorded unavailable */ }
      emit(message);
    },
  });
  counters.hooks += 1;
}

function installApiHook(api) {
  const target = resolveVA(api.va);
  Interceptor.attach(target, {
    onEnter: function (args) {
      if (overflowed || targetDepth <= 0) return;
      let caller = null;
      try { caller = this.returnAddress ? this.returnAddress.toString() : null; } catch (error) { caller = null; }
      // Only the packet-proven registration callsites of THIS target count as
      // registrations. The same QTask API is also called by deeply nested setup
      // routines while the target runs; those are not part of its ordered stream.
      // The allowlist also fixes the name argument slots, so stale stack words
      // from a previous registration can never be reported as a second name.
      const callerNorm = String(caller).toLowerCase();
      const allow = (CONFIG.registrationAllowlist || []).filter(entry => {
        // The runtime caller is the return address, i.e. callsite + instruction
        // length (5 for the `call rel32` form used by these registrars). Accept
        // both the instruction address and that documented next address.
        const callsite = String(entry.callsite).toLowerCase();
        const next = '0x' + (parseInt(callsite, 16) + 5).toString(16);
        if (callerNorm !== callsite && callerNorm !== next) return false;
        return !entry.api || String(entry.api).toLowerCase() === String(api.va).toLowerCase();
      });
      if (allow.length === 0) return;
      const allowedNames = new Set();
      for (const entry of allow) {
        for (const name of entry.names || []) allowedNames.add(parseInt(String(name), 16));
      }
      const words = readArgumentWords(this.context, Math.max(api.argWords | 0, 4));
      this.__probeApi = {
        address: api.va, name: api.name, argWords: words, callerReturnAddress: caller,
        nameStrings: words.map((word, slot) => {
          const string = provenString(word);
          if (!string || !allowedNames.has(parseInt(String(string.va), 16))) return null;
          return { slot: slot, ...string };
        }).filter(Boolean),
      };
    },
    onLeave: function (retval) {
      if (overflowed || this.__probeApi === undefined) return;
      counters.registrations += 1;
      const message = { type: 'registration', ...this.__probeApi, returnValue: null, eax: null };
      try {
        const eax = this.context.eax >>> 0;
        message.returnValue = eax;
        message.eax = '0x' + eax.toString(16);
      } catch (error) { /* recorded unavailable */ }
      emit(message);
    },
  });
  counters.hooks += 1;
}

function installChainHook(entry) {
  const target = resolveVA(entry.target);
  let listener = null;
  try {
    listener = Interceptor.attach(target, {
    onEnter: function (args) {
      if (overflowed || targetDepth <= 0) return;
      let caller = null;
      try { caller = this.returnAddress ? this.returnAddress.toString() : null; } catch (error) { caller = null; }
      // The caller return address of a normal call is the next instruction
      // (callsite + instruction length), not the callsite itself. A tail jump
      // returns to the target's own entry caller, because the jmp pushes nothing.
      let expected = null;
      if (entry.tailJump) {
        expected = installed.entryCaller ? ptr(installed.entryCaller) : null;
      } else if (entry.returnAddress) {
        expected = resolveVA(entry.returnAddress);
      } else if (entry.callsite) {
        expected = resolveVA('0x' + (parseInt(entry.callsite, 16) + 5).toString(16));
      }
      const matches = expected === null ? null : (caller !== null && ptr(caller).equals(expected));
      if (matches === false) {
        counters.chainMismatches += 1;
        emit({ type: 'chain_mismatch', target: entry.target, expectedCallsite: entry.callsite || null,
               expectedReturnAddress: expected === null ? null : expected.toString(),
               callerReturnAddress: caller });
        return;
      }
      counters.chainCalls += 1;
      emit({
        type: 'chain_call', target: entry.target, label: entry.label || null,
        expectedCallsite: entry.callsite || null, callerReturnAddress: caller, matches: matches,
      });
    },
    });
  } catch (error) {
    // Some retail chain targets are one-byte `ret` stubs that Frida cannot
    // intercept. Record the failure and keep the remaining chain hooks alive
    // instead of aborting the whole capture; the comparator treats any install
    // failure as incomplete order evidence (never a pass).
    counters.chainInstallFailures.push({ target: entry.target, callsite: entry.callsite || null,
                                         label: entry.label || null, error: String(error) });
    send({ type: 'chain_install_failure', target: entry.target, callsite: entry.callsite || null,
           label: entry.label || null, error: String(error) });
    return;
  }
  counters.hooks += 1;
}

function install(address, layout) {
  if (installed !== null) throw new Error('probe already installed');
  const main = mainModule(layout.moduleName);
  installed = {
    base: main.base,
    preferredBase: layout.preferredBase,
    argWords: Math.max(0, Math.min(16, layout.argWords | 0)),
    maxEvents: Math.max(16, Math.min(65536, (layout.maxEvents | 0) || (CONFIG.maxEvents | 0) || 4096)),
    address: address,
  };
  try {
    // The driver passes the side (retail or candidate) target VA; the probe hooks
    // that exact module-relative address. Side-effect/API/chain VAs are retail
    // VAs and assume the candidate transcribes the same native/data addresses.
    installTargetHook(resolveVA(address), layout);
    const apiList = CONFIG.apiHooks || [];
    for (const api of apiList) installApiHook(api);
    const chainList = CONFIG.chainCalls || [];
    for (const entry of chainList) installChainHook(entry);
  } catch (error) {
    send({ type: 'hook_failed', address: CONFIG.target.va, error: String(error) });
    throw error;
  }
  return {
    address: CONFIG.target.va,
    name: CONFIG.target.name,
    hooksInstalled: counters.hooks,
    apiHooks: (CONFIG.apiHooks || []).length,
    chainHooks: (CONFIG.chainCalls || []).length,
    sideEffects: (CONFIG.sideEffects || []).length,
  };
}

function summary() {
  return {
    target: CONFIG.target, hooks: counters.hooks, entries: entryCount, returns: returnCount,
    registrations: counters.registrations, chainCalls: counters.chainCalls,
    chainMismatches: counters.chainMismatches,
    chainInstallFailures: counters.chainInstallFailures,
    overflow: overflowed, errors: counters.errors,
  };
}

function detach() {
  try { Interceptor.detachAll(); } catch (error) { /* best effort */ }
}

rpc.exports = { hook: install, summary: summary, detach: detach };
