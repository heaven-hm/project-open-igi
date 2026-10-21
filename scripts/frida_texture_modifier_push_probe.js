'use strict';
// Controlled, nonaliasing inputs for 0x00505E40; fatal paths and table contents
// other than the context's table pointer and selected row are not monitored.
rpc.exports.probe = function(moduleName, encodedLayout, fixtures) {
  if (Process.pointerSize !== 4) throw new Error('x86 required');
  const layout = JSON.parse(encodedLayout), module = Process.getModuleByName(moduleName);
  const resolve = (value, size) => {
    const rva = ptr(value).sub(ptr('0x00400000'));
    if (rva.compare(ptr(module.size - size)) > 0) throw new Error('VA outside image');
    return module.base.add(rva);
  };
  const entry = resolve(layout.entry, 1), count = resolve(layout.count, 4);
  const stack = resolve(layout.stack, 80), mask = resolve(layout.mask, 4);
  const native = new NativeFunction(entry, 'pointer', ['pointer'], 'mscdecl');
  const savedCount = count.readU32(), savedStack = stack.readByteArray(80), savedMask = mask.readU32();
  const hex = (p, n) => Array.from(new Uint8Array(p.readByteArray(n)), x => x.toString(16).padStart(2, '0')).join('');
  const calls = [];
  try {
    for (const fixture of fixtures) {
      if (fixture.index < -1 || fixture.index > 2 || fixture.count < 0 || fixture.count >= 20)
        throw new Error('fixture outside controlled nonfatal scope');
      const context = Memory.alloc(512), table = Memory.alloc(0xdd0), row = Memory.alloc(36);
      context.writeByteArray(new Uint8Array(512).fill(0xa5));
      table.writeByteArray(new Uint8Array(0xdd0).fill(0));
      row.writeByteArray(new Uint8Array(36).fill(0));
      context.add(0x68).writePointer(fixture.index < 0 ? ptr(0) : table);
      context.add(0xbc).writeS32(fixture.index);
      context.add(0xe4).writeU32(fixture.value);
      table.add(0xdcc).writePointer(row);
      if (fixture.index >= 0) row.add(fixture.index * 12).writeU32(fixture.enabled);
      count.writeU32(fixture.count); stack.writeByteArray(new Uint8Array(80).fill(0x5a));
      mask.writeU32(fixture.mask);
      const result = native(context);
      const tablePointerPreserved = context.add(0x68).readPointer().equals(fixture.index < 0 ? ptr(0) : table);
      if (!tablePointerPreserved) throw new Error('native modified context table pointer');
      // Normalize the one process-specific pointer stored in the context.
      context.add(0x68).writeU32(fixture.index < 0 ? 0 : 1);
      calls.push({input: fixture, output: {
        return_value: result.equals(context) ? {kind: 'context', value: 0} : {kind: 'integer', value: result.toUInt32()},
        state_delta: {context_hex: hex(context, 512), count: count.readU32(), mask: mask.readU32()},
        side_effects: {stack_hex: hex(stack, 80), row_hex: hex(row, 36), table_pointer_preserved: tablePointerPreserved}
      }});
    }
  } finally {
    count.writeU32(savedCount); stack.writeByteArray(savedStack); mask.writeU32(savedMask);
    if (count.readU32() !== savedCount || mask.readU32() !== savedMask ||
        hex(stack, 80) !== Array.from(new Uint8Array(savedStack), x => x.toString(16).padStart(2, '0')).join(''))
      throw new Error('global restore failed');
  }
  return {module: module.name, entry: entry.toString(), globalStateRestored: true, calls};
};
