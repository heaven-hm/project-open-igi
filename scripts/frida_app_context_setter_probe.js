'use strict';
// 0x0048F1A0: MOVSX EAX, byte [ESP+4]; MOV [0x005C8BF8], EAX; RET.
// Ghidra snapshot and independent retail disassembly establish the signed-byte ABI.
rpc.exports.probe = function(moduleName, encodedLayout, fixtures) {
  if (Process.pointerSize !== 4) throw new Error('x86 required');
  const layout = JSON.parse(encodedLayout), module = Process.getModuleByName(moduleName);
  const resolve = (value, size) => {
    const rva = ptr(value).sub(ptr('0x00400000'));
    if (rva.compare(ptr(module.size - size)) > 0) throw new Error('VA outside image');
    return module.base.add(rva);
  };
  const entry = resolve(layout.entry, 11);
  const fields = layout.state ? (() => {
    const accessor = new NativeFunction(resolve(layout.state, 1), 'pointer', [], 'mscdecl');
    const state = accessor();
    if (state.compare(module.base) < 0 || state.add(24).compare(module.base.add(module.size)) > 0)
      throw new Error('candidate state outside image');
    return [0, 4, 8, 12, 16, 20].map(offset => state.add(offset));
  })() : layout.fields.map(value => resolve(value, 4));
  const native = new NativeFunction(entry, 'int32', ['int8'], 'mscdecl');
  const saved = fields.map(p => p.readS32()), calls = [];
  try {
    for (const fixture of fixtures) {
      if (!Number.isInteger(fixture.signed_arg) || fixture.signed_arg < -128 || fixture.signed_arg > 127 ||
          fixture.initial_state.length !== fields.length) throw new Error('invalid signed-byte fixture');
      fields.forEach((p, i) => p.writeS32(fixture.initial_state[i]));
      const before = fields.map(p => p.readS32());
      const returned = native(fixture.signed_arg);
      calls.push({input: fixture, output: {return_value: returned,
        state_delta: {before, after: fields.map(p => p.readS32())},
        side_effects: {fields: fields.map(p => p.readS32())}}});
    }
  } finally {
    fields.forEach((p, i) => p.writeS32(saved[i]));
    if (fields.some((p, i) => p.readS32() !== saved[i])) throw new Error('global restore failed');
  }
  return {module: module.name, entry: entry.toString(), globalStateRestored: true, calls};
};
