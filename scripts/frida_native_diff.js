// Tiny headless native probe for the retail game and the reconstruction.
//
// Observe natural calls only. Raw stack words/EAX are diagnostic observations;
// unknown calling conventions, pointer contents, float returns and side effects
// require a typed per-native probe. Never invoke unknown ABIs with raw words.
'use strict';

function probeHook(address) {
  try {
    const module = Process.mainModule;
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
    send({type: 'hooked', address: address.toString()});
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
