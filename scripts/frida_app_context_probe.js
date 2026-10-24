'use strict';
// Layout: engine/include/igi/retail/core/app_context.hpp; fixtures: native_diff_runner.py.
let naturalCalls = {};
const installed = new Set();
rpc.exports = {
  run(target) {
    if (Process.pointerSize !== 4 || !Array.isArray(target.natives) || target.natives.length !== 1)
      throw new Error('Requires exactly one x86 native per fixture RPC');
    const requested = target.natives[0];
    if (installed.has(requested.address) || !Array.isArray(requested.fixtures) ||
        requested.fixtures.length < 1 || requested.fixtures.length > 256)
      throw new Error('Duplicate native or fixture batch outside 1..256 cases');
    const module = Process.getModuleByName(target.module);
    function checked(value, size) {
      const p = ptr(value);
      if (p.compare(module.base) < 0 || p.add(size).compare(module.base.add(module.size)) > 0)
        throw new Error('Target outside executable image');
      return p;
    }
    const fields = target.fields;
    const pointers = target.state_accessor ? (() => {
      const accessor = new NativeFunction(checked(target.state_accessor, 1), 'pointer', [], 'mscdecl');
      const state = accessor();
      checked(state, fields.length * 4);
      return fields.map((_, index) => state.add(index * 4));
    })() : fields.map(name => checked(target.globals[name], 4));
    function readState() {
      return Object.fromEntries(fields.map((name, i) => [name, pointers[i].readS32()]));
    }
    function writeState(state) { fields.forEach((name, i) => pointers[i].writeS32(state[name])); }
    const saved = readState(), cases = [];
    let restored = false;
    const entries = target.natives.map(native => {
      const entry = checked(native.entry, 1);
      return {native, entry, invoke: new NativeFunction(entry, 'int32', native.setter ? ['int8'] : [], 'mscdecl')};
    });
    try {
      for (const {native, invoke} of entries) {
        send({stage:'native-start', address:native.address, cases:native.fixtures.length});
        for (const fixture of native.fixtures) {
          writeState(fixture.initial_state);
          const before = readState();
          const returned = native.setter ? invoke(fixture.signed_arg) : invoke();
          cases.push({address:native.address, input:fixture,
            observed_initial_state:before, output:{return_value:returned, state:readState()}});
        }
        send({stage:'native-end', address:native.address, cases:native.fixtures.length});
      }
    } finally {
      writeState(saved);
      restored = JSON.stringify(readState()) === JSON.stringify(saved);
      send({stage:'globals-restored', restored});
      if (!restored) throw new Error('App context globals were not restored');
    }
    // Hooks begin after explicit calls, so these counts never include fixtures.
    for (const {native, entry} of entries) {
      naturalCalls[native.address] = 0;
      Interceptor.attach(entry, {onEnter() { naturalCalls[native.address]++; }});
    }
    Interceptor.flush(); installed.add(requested.address);
    return {cases, globals_restored:restored,
      scope:'Explicit agent-thread fixtures; six-field memory effects and EAX return only'};
  },
  snapshot() { return {natural_calls:naturalCalls}; },
  cwd() {
    const buffer = Memory.alloc(4096);
    const get = new NativeFunction(Process.getModuleByName('kernel32.dll').getExportByName('GetCurrentDirectoryW'),
      'uint32', ['uint32','pointer'], 'stdcall');
    const size = get(2048, buffer);
    if (!size || size >= 2048) throw new Error('GetCurrentDirectoryW failed');
    return buffer.readUtf16String(size);
  }
};
