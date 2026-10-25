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
      const neighborsBefore = snapshot(state.sub(4), 12);
      const returned = native(fixture.value);
      const after = state.readS32();
      const neighborsAfter = snapshot(state.sub(4), 12);
      const adjacentUnchanged = neighborsBefore.slice(0, 4).concat(neighborsBefore.slice(8)).every(
        (value, index) => value === neighborsAfter.slice(0, 4).concat(neighborsAfter.slice(8))[index]);
      if (returned !== fixture.value || after !== fixture.value || !adjacentUnchanged)
        throw new Error('controlled store/return/adjacent state differs');
      calls.push({input: fixture, output: {return_value: returned,
        state_delta: {global_0x005C8BC0_before: before, global_0x005C8BC0_after: after},
        side_effects: {global_0x005C8BC0_after: after,
          return_aliases_argument: returned === fixture.value,
          stored_aliases_argument: after === fixture.value,
          adjacent_bytes_unchanged: adjacentUnchanged}}});
    }
  } finally {
    state.writeS32(saved);
    if (state.readS32() !== saved) throw new Error('global restore failed');
  }
  return {module: module.name, entry: entry.toString(), globalStateRestored: true, calls};
};

// Natural WinMain call. Normalize only the proven module-base handle; arbitrary
// handles/words are never dereferenced or treated as equivalent across processes.
let listener;
let entries = 0;
let returns = 0;
function snapshot(address, length) {
  const range = Process.findRangeByAddress(address);
  if (!range || !range.protection.includes('r') ||
      address.add(length).compare(range.base.add(range.size)) > 0)
    throw new Error('reviewed snapshot is not readable');
  return Array.from(new Uint8Array(address.readByteArray(length)));
}
rpc.exports.hook = function(address, layout) {
  if (Process.pointerSize !== 4) throw new Error('x86 required');
  const image = Process.getModuleByName(layout.moduleName);
  const relocate = value => image.base.add(ptr(value).sub(ptr(layout.preferredBase)));
  const entry = relocate(layout.entryVA), state = relocate(layout.stateVA);
  const code = snapshot(entry, 10);
  if (code[0] !== 0x8b || code[1] !== 0x44 || code[2] !== 0x24 || code[3] !== 4 ||
      code[4] !== 0xa3 || code[9] !== 0xc3 || !entry.add(5).readPointer().equals(state))
    throw new Error('compiled/retail argument-store-return body or global mapping differs');
  listener = Interceptor.attach(entry, {
    onEnter(args) {
      this.index = ++entries;
      this.argument = args[0];
      if (!this.argument.equals(image.base)) throw new Error('natural instance argument is not module base');
      this.before = state.readU32();
      if (this.before !== 0) throw new Error('natural instance global is not initially null');
      this.neighbors = snapshot(state.sub(4), 12);
    },
    onLeave(retval) {
      returns++;
      const neighbors = snapshot(state.sub(4), 12);
      const returnedInput = retval.equals(this.argument);
      const storedInput = state.readPointer().equals(this.argument);
      const adjacentUnchanged = this.neighbors.slice(0, 4).concat(this.neighbors.slice(8)).every(
        (value, index) => value === neighbors.slice(0, 4).concat(neighbors.slice(8))[index]);
      if (!returnedInput || !storedInput || !adjacentUnchanged)
        throw new Error('instance store, EAX alias or adjacent bytes differ');
      const value = {kind: 'process-instance-handle', module_rva: 0};
      send({type: 'observed', address, callIndex: this.index,
        typedState: {scope: 'startup-scalar-store',
          inputs: {value, initial_state: this.before},
          outputs: {return_value: value,
            state_delta: {global_0x005C8BC0_before: this.before, global_0x005C8BC0_after: value}},
          sideEffects: {global_0x005C8BC0_after: value,
            return_aliases_argument: returnedInput, stored_aliases_argument: storedInput,
            adjacent_bytes_unchanged: adjacentUnchanged}}});
      send({type: 'capture_complete', address, entries, returns});
    }
  });
  return {installed: true, address, mode: 'natural-startup-call'};
};
rpc.exports.summary = function() { return {entries, returns}; };
rpc.exports.detach = function() { if (listener) listener.detach(); };
