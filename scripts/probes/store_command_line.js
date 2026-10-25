'use strict';
// 0x0048F0C0: MOV EAX,[ESP+4]; MOV [0x005C8BC8],EAX; RET.
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
        state_delta: {global_0x005C8BC8_before: before, global_0x005C8BC8_after: after},
        side_effects: {global_0x005C8BC8_after: after}}});
    }
  } finally {
    state.writeS32(saved);
    if (state.readS32() !== saved) throw new Error('global restore failed');
  }
  return {module: module.name, entry: entry.toString(), globalStateRestored: true, calls};
};

// Natural WinMain observation. The same program also exports the existing
// controlled ABI replay above; the two modes share the evidenced word copy.
let naturalListener;
let naturalEntries = 0;
let naturalReturns = 0;
function snapshot(address, length) {
  const range = Process.findRangeByAddress(address);
  if (!range || !range.protection.includes('r') ||
      address.add(length).compare(range.base.add(range.size)) > 0)
    throw new Error('reviewed snapshot is not readable');
  return Array.from(new Uint8Array(address.readByteArray(length)));
}
function commandBytes(address) {
  const result = [];
  for (let i = 0; i < 256; i++) {
    const value = snapshot(address.add(i), 1)[0];
    result.push(value);
    if (value === 0) return result;
  }
  throw new Error('command line exceeds reviewed observation bound');
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
  naturalListener = Interceptor.attach(entry, {
    onEnter(args) {
      this.index = ++naturalEntries;
      this.argument = args[0];
      this.value = {kind: 'command-line-bytes', bytes: commandBytes(args[0])};
      this.before = state.readU32();
      this.neighbors = snapshot(state.sub(4), 12);
    },
    onLeave(retval) {
      naturalReturns++;
      const neighbors = snapshot(state.sub(4), 12);
      const returnedInput = retval.equals(this.argument);
      const storedInput = state.readPointer().equals(this.argument);
      const inputUnchanged = JSON.stringify(commandBytes(this.argument)) === JSON.stringify(this.value.bytes);
      const adjacentUnchanged = this.neighbors.slice(0, 4).concat(this.neighbors.slice(8)).every(
        (value, index) => value === neighbors.slice(0, 4).concat(neighbors.slice(8))[index]);
      if (!returnedInput || !storedInput || !inputUnchanged || !adjacentUnchanged)
        throw new Error('word copy, returned argument, input or adjacent memory differs');
      send({type: 'observed', address, callIndex: this.index,
        typedState: {scope: 'startup-scalar-store',
          inputs: {value: this.value, initial_state: this.before},
          outputs: {return_value: this.value,
            state_delta: {global_0x005C8BC8_before: this.before, global_0x005C8BC8_after: this.value}},
          sideEffects: {global_0x005C8BC8_after: this.value,
            argument_bytes_unchanged: inputUnchanged, adjacent_bytes_unchanged: adjacentUnchanged}}});
      send({type: 'capture_complete', address, entries: naturalEntries, returns: naturalReturns});
    }
  });
  return {installed: true, address, mode: 'natural-startup-call'};
};
rpc.exports.summary = function() { return {entries: naturalEntries, returns: naturalReturns}; };
rpc.exports.detach = function() { if (naturalListener) naturalListener.detach(); };
