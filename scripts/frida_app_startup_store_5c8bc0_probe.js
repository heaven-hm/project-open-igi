'use strict';
// 0x0048F0D0: MOV EAX,[ESP+4]; MOV [0x005C8BC0],EAX; RET.
rpc.exports.probe = function(moduleName, encodedLayout, fixtures) {
  if (Process.pointerSize !== 4) throw new Error('x86 required');
  const layout = JSON.parse(encodedLayout), module = Process.getModuleByName(moduleName);
  const resolve = (value, size) => {
    const rva = ptr(value).sub(ptr('0x00400000'));
    if (rva.compare(ptr(module.size - size)) > 0) throw new Error('VA outside image');
    return module.base.add(rva);
  };
  const entry = resolve(layout.entry, 10), state = resolve(layout.state, 4);
  const native = new NativeFunction(entry, 'int32', ['int32'], 'mscdecl');
  const saved = state.readS32(), calls = [];
  try {
    for (const fixture of fixtures) {
      state.writeS32(fixture.initial_state);
      const before = state.readS32();
      const returned = native(fixture.value);
      const after = state.readS32();
      calls.push({input: fixture, output: {return_value: returned,
        state_delta: {global_0x005C8BC0_before: before, global_0x005C8BC0_after: after},
        side_effects: {global_0x005C8BC0_after: after}}});
    }
  } finally {
    state.writeS32(saved);
    if (state.readS32() !== saved) throw new Error('global restore failed');
  }
  return {module: module.name, entry: entry.toString(), globalStateRestored: true, calls};
};
