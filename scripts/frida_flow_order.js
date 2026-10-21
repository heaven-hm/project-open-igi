'use strict';
// Order trace for the retail flow entry points. Records enter/leave order with
// a monotonic sequence so the actual call graph can be compared with the port.
// Instrumentation only: it never alters arguments, returns, or memory.
const modules = Process.enumerateModules();
const image = modules.find(m => /^igi\.exe$/i.test(m.name)) ||
              modules.find(m => /open-igi.*\.exe$/i.test(m.name)) || Process.mainModule;
const base = image.base;
const peOffset = base.add(0x3c).readU32();
const preferredBase = base.add(peOffset + 0x34).readU32();
function actual(va) { return base.add(ptr(va).sub(ptr(preferredBase))); }

let counter = 0;
const depth = { value: 0 };
let enabled = false;

function emit(address, phase, retval) {
  const event = { type: 'flow', seq: ++counter, address: address.toString(),
                  phase: phase, depth: depth.value };
  if (retval !== undefined) event.retval = retval.toString();
  send(event);
}

function hookOne(address) {
  Interceptor.attach(actual(address), {
    onEnter() {
      if (!enabled) return;
      depth.value += 1;
      this.counted = true;
      emit(address, 'enter');
    },
    onLeave(retval) {
      if (!enabled || !this.counted) return;
      emit(address, 'leave', retval);
      depth.value -= 1;
    }
  });
}

rpc.exports = {
  hook(address) {
    hookOne(ptr(address));
    return true;
  },
  start() { enabled = true; return true; },
  stop() { enabled = false; return true; }
};
