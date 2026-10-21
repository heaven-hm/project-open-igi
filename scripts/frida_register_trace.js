'use strict';
// Record every TaskType_Register (0x00401900) call in the retail process:
// name, object size, parent class and the returned class id, in call order.
// ABI verified in igi.exe: stdcall(char* name, uint objectSize, uint parent),
// three stack arguments; the fourth stack slot is caller residue, not an
// argument. The parent value comes from 0x4018f0 (reads word [0x54F928], the
// last-registered class id), and the register function stores the parent's
// low 16 bits at table slot+8. This is the exact registration sequence the
// port has to replay so class ids and handler-table slots match the retail.
const modules = Process.enumerateModules();
const image = modules.find(m => /^igi\.exe$/i.test(m.name)) ||
              modules.find(m => /open-igi.*\.exe$/i.test(m.name)) || Process.mainModule;
const base = image.base;
const peOffset = base.add(0x3c).readU32();
const preferredBase = base.add(peOffset + 0x34).readU32();
function actual(va) { return base.add(ptr(va).sub(ptr(preferredBase))); }

let seq = 0;

function readName(pointer) {
  if (pointer.isNull()) return { raw: 'null' };
  try {
    const bytes = new Uint8Array(pointer.readByteArray(48));
    let len = 0;
    while (len < bytes.length && bytes[len] !== 0) len++;
    let text = '';
    for (let i = 0; i < len; i++) text += String.fromCharCode(bytes[i]);
    return { raw: pointer.toString(), text: text };
  } catch (e) { return { raw: pointer.toString(), error: String(e) }; }
}

rpc.exports = {
  hook(address) {
    Interceptor.attach(actual(address), {
      onEnter(args) {
        this.entry = {
          name: readName(args[0]),
          objectSize: args[1].toInt32() >>> 0,
          parent: args[2].toInt32() >>> 0,
          flags: args[3] ? args[3].toInt32() >>> 0 : null
        };
      },
      onLeave(retval) {
        const entry = this.entry;
        entry.seq = ++seq;
        entry.classId = retval.toInt32() & 0xffff;
        send({ type: 'register', entry: entry });
      }
    });
    send({ type: 'hooked', image: image.name, base: base.toString() });
    return true;
  }
};
