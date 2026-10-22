// Tiny headless native probe for the retail game and the reconstruction.
//
// Observe natural calls only. Raw stack words/EAX are diagnostic observations;
// unknown calling conventions, pointer contents, float returns and side effects
// require a typed per-native probe. Never invoke unknown ABIs with raw words.
'use strict';

// Recognised game modules, in precedence order (first exact name match wins):
//   1. igi.exe               - retail Project IGI 1.
//   2. open-igi.exe          - legacy reconstruction executable name.
//   3. igi1.exe              - current reconstruction executable name.
//   4. open-igi-probe-*.exe  - strict driver's staged copy of the candidate.
// The staged pattern is matched last; if several staged copies exist they are
// ordered by name then base, so resolution is deterministic. There is
// deliberately no Process.mainModule / getModuleByName fallback: under
// CrossOver Process.mainModule can be ucrtbase.dll or advapi32.dll, and hooking
// a system DLL would be silent wrong evidence.
const moduleNames = ['igi.exe', 'open-igi.exe', 'igi1.exe'];

function resolveGameModule() {
  const modules = Process.enumerateModules();
  const lowered = m => m.name.toLowerCase();
  for (const wanted of moduleNames) {
    const match = modules.find(m => lowered(m) === wanted);
    if (match !== undefined) return match;
  }
  const staged = modules.filter(m => lowered(m).startsWith('open-igi-probe-') &&
                                     lowered(m).endsWith('.exe'));
  staged.sort((a, b) => {
    const left = lowered(a);
    const right = lowered(b);
    if (left < right) return -1;
    if (left > right) return 1;
    return a.base.compare(b.base);
  });
  return staged.length > 0 ? staged[0] : null;
}

let resolvedModule = null;

function requireGameModule() {
  if (resolvedModule !== null) return resolvedModule;
  const module = resolveGameModule();
  if (module === null) {
    send({type: 'probe_error',
          error: 'target game module not found; refusing to hook a system module',
          modules: Process.enumerateModules().map(m => ({name: m.name,
                                                         base: m.base.toString(),
                                                         size: m.size}))});
    throw new Error('target game module not found');
  }
  resolvedModule = module;
  return module;
}

function probeHook(address) {
  // A missing game module is a hard refusal, not a hook_failed per address.
  const module = requireGameModule();
  try {
    const peOffset = module.base.add(0x3c).readU32();
    const preferredBase = module.base.add(peOffset + 0x34).readU32();
    const actual = module.base.add(address.sub(ptr(preferredBase)));
    Interceptor.attach(actual, {
      onEnter(args) {
        const seen = [];
        for (let i = 0; i < 6; i++) seen.push(args[i].toString());
        this.seen = seen;
        this.record = true;
      },
      onLeave(retval) {
        if (this.record) {
          this.record = false;
          send({type: 'observed', address: address.toString(),
                args: this.seen, retval: retval.toString()});
        }
      }
    });
    send({type: 'hooked', address: address.toString(),
          module: module.name, base: module.base.toString()});
  } catch (e) {
    send({type: 'hook_failed', address: address.toString(), error: String(e)});
  }
}

globalThis.probeHook = probeHook;
rpc.exports = {
  hook(address) {
    probeHook(ptr(address));
    return true;
  }
};
