// Retail skinning probe for a sequential `igi.exe window levelN` run.
// The addresses are retail VAs for SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// It records raw cdecl words and the proven source/palette fields without changing game memory.
'use strict';

const retailBase = ptr('0x00400000');
const captured = { meshSkinCalls: [], paletteSkinCalls: [] };

function word(pointer) {
  try { return pointer.readU32(); } catch (_) { return null; }
}

function short(pointer) {
  try { return pointer.readU16(); } catch (_) { return null; }
}

function scalar(pointer) {
  try { return pointer.readFloat(); } catch (_) { return null; }
}

function hex(pointer) {
  return pointer ? pointer.toString() : null;
}

function address(va) {
  const module = Process.getModuleByName('igi.exe');
  return module.base.add(ptr(va).sub(retailBase));
}

function capturePalette(args) {
  if (captured.paletteSkinCalls.length >= 12) return;
  const source = args[0];
  const paletteHolder = args[2];
  const records = word(source.add(0x18));
  const sample = records === null ? null : ptr(records);
  const paletteAddress = word(paletteHolder.add(8));
  const palette = paletteAddress === null ? null : ptr(paletteAddress);
  captured.paletteSkinCalls.push({
    source: hex(source),
    baseVertexCount: word(source.add(0x10)),
    extraInfluenceCount: word(source.add(0x0c)),
    records: hex(sample),
    recordBone: sample === null ? null : short(sample.add(0x26)),
    recordWeightBits: sample === null ? null : word(sample.add(0x20)),
    paletteHolder: hex(paletteHolder),
    paletteEntries: hex(palette),
    firstPaletteEntry: palette === null ? null : {
      translation: [scalar(palette.add(0x18)), scalar(palette.add(0x1c)), scalar(palette.add(0x20))],
      rotationRow0: [scalar(palette.add(0x24)), scalar(palette.add(0x28)), scalar(palette.add(0x2c))],
      rotationRow1: [scalar(palette.add(0x30)), scalar(palette.add(0x34)), scalar(palette.add(0x38))],
      rotationRow2: [scalar(palette.add(0x3c)), scalar(palette.add(0x40)), scalar(palette.add(0x44))]
    },
    alternatePath: args[3].toInt32()
  });
}

function captureMesh(args) {
  if (captured.meshSkinCalls.length >= 12) return;
  captured.meshSkinCalls.push({ context: hex(args[0]) });
}

Interceptor.attach(address('0x0049B700'), { onEnter(args) { capturePalette(args); } });
Interceptor.attach(address('0x0049F700'), { onEnter(args) { captureMesh(args); } });

setTimeout(() => {
  send({ event: 'retail_skin_probe', module: 'igi.exe', captured });
}, 10000);
