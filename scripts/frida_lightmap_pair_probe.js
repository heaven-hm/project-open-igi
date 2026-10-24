'use strict';
// The shared launch helper installs pressEscape first. This replaces only probe.
// ABI/offsets: Binary Ninja 0x004F82B0 and 0x004F1160, plus retained lightmap-enter evidence.
rpc.exports.probe = function(moduleName, encodedLayout, fixtures) {
  const layout = JSON.parse(encodedLayout);
  const module = Process.getModuleByName(moduleName);
  const resolve = address => {
    const rva = ptr(address).sub(ptr('0x00400000'));
    if (rva.compare(ptr(module.size)) >= 0) throw new Error('VA outside module: ' + address);
    return module.base.add(rva);
  };
  const entry = resolve(layout.entry);
  const count = resolve(layout.count);
  const stack = resolve(layout.stack);
  const bd = resolve(layout.bd);
  const be = resolve(layout.be);
  const native = new NativeFunction(entry, 'pointer', ['pointer'], 'mscdecl');
  const saved = {count: count.readByteArray(4), stack: stack.readByteArray(80),
                 bd: bd.readU8(), be: be.readU8()};
  const hex = buffer => Array.from(new Uint8Array(buffer), x => x.toString(16).padStart(2, '0')).join('');
  const calls = [];
  try {
    for (const fixture of fixtures) {
      if (![0, 19].includes(fixture.count)) throw new Error('unsupported stack fixture');
      const context = Memory.alloc(384);
      context.writeByteArray(new Uint8Array(384).fill(0xa5));
      context.add(0x108).writeS32(fixture.enabled);
      context.add(0xd8).writeU32(fixture.value);
      count.writeU32(fixture.count);
      stack.writeByteArray(new Uint8Array(80).fill(0x5a));
      bd.writeU8(fixture.bd);
      be.writeU8(fixture.be);
      const returned = native(context);
      calls.push({input: fixture, output: {
        return_value: returned.equals(context) ? {kind: 'context', value: 0} :
                                                {kind: 'integer', value: returned.toUInt32()},
        state_delta: {context_hex: hex(context.readByteArray(384)), count: count.readU32()},
        side_effects: {stack_hex: hex(stack.readByteArray(80)), bd: bd.readU8(), be: be.readU8()}
      }});
    }
  } finally {
    count.writeByteArray(saved.count); stack.writeByteArray(saved.stack);
    bd.writeU8(saved.bd); be.writeU8(saved.be);
    if (hex(count.readByteArray(4)) !== hex(saved.count) ||
        hex(stack.readByteArray(80)) !== hex(saved.stack) ||
        bd.readU8() !== saved.bd || be.readU8() !== saved.be) {
      throw new Error('native global state restoration failed');
    }
  }
  return {module: module.name, moduleBase: module.base.toString(), entry: entry.toString(), globalStateRestored: true, calls};
};
