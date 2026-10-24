/*
 * Read-only retail HumanPlayer motion probe.
 *
 * Usage (after retail has been launched from the IGI data directory):
 *   frida -H 127.0.0.1:27042 -n igi.exe -l scripts/frida_player_motion_probe.js
 *
 * This intentionally records only function entries, returns, and raw ABI values.  It never
 * dereferences unknown pointers or changes game memory.  The addresses are retail VAs for
 * igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
 */

'use strict';

const retailImageBase = ptr('0x00400000');
const targets = [
    { name: 'Human_MovementIntegrator', va: ptr('0x004614b0') },
    { name: 'Human_GroundQuery', va: ptr('0x00462390') },
    { name: 'Human_WallSweep', va: ptr('0x00461a70') },
];
const maxEventsPerTarget = 128;
const counts = Object.create(null);
const module = Process.findModuleByName('igi.exe');

if (module === null) {
    send({ kind: 'error', error: 'igi.exe is not loaded; attach to the retail process.' });
} else {
    for (const target of targets) {
        const address = module.base.add(target.va.sub(retailImageBase));
        counts[target.name] = 0;
        Interceptor.attach(address, {
            onEnter(args) {
                const ordinal = ++counts[target.name];
                if (ordinal > maxEventsPerTarget) {
                    return;
                }
                this.record = { target: target.name, ordinal: ordinal };
                send({
                    kind: 'enter',
                    target: target.name,
                    ordinal: ordinal,
                    address: address.toString(),
                    context: args[0].toString(),
                    arg1: args[1].toString(),
                    arg2: args[2].toString(),
                    arg3: args[3].toString(),
                    arg4: args[4].toString(),
                });
            },
            onLeave(retval) {
                if (this.record !== undefined) {
                    send({ kind: 'leave', target: this.record.target, ordinal: this.record.ordinal,
                           returnValue: retval.toString() });
                }
            },
        });
    }
    send({ kind: 'ready', moduleBase: module.base.toString(), targets: targets.map(t => t.name) });
}
