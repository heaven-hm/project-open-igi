'use strict';
// Diagnostic observation of WinMain entry through CoInitialize entry.
// Retail 0x48F544/560/56D/579 establishes the expected direct API order.
// The candidate main address is resolved from its own PE symbols by the caller.
// This probe never changes arguments, returns, globals, mutex names or branches.
let handles = [];
let main = null;
let active = false;
let events = [];
let module = null;
function record(event) {
    events.push(event);
    send(Object.assign({type: 'mutex_prefix'}, event));
}
rpc.exports = {
    hook(address, layout) {
        module = Process.getModuleByName(layout.moduleName);
        main = module.base.add(ptr(address).sub(ptr(layout.preferredBase)));
        function direct(returnAddress) {
            return active && returnAddress.compare(main) >= 0 &&
                returnAddress.compare(main.add(0x1000)) < 0;
        }
        for (const name of ['CreateMutexA', 'GetLastError', 'CoInitialize']) {
            const owner = Process.getModuleByName(name === 'CoInitialize' ? 'ole32.dll' : 'kernel32.dll');
            handles.push(Interceptor.attach(owner.getExportByName(name), {
                onEnter(args) {
                    this.inScope = direct(this.returnAddress);
                    if (!this.inScope) return;
                    this.event = {name, phase: 'entry', callerRva: this.returnAddress.sub(module.base).toString()};
                    if (name === 'CreateMutexA') {
                        this.event.securityIsNull = args[0].isNull();
                        this.event.initialOwner = args[1].toInt32();
                        let length = 0;
                        while (length < 64 && args[2].add(length).readU8() !== 0) length++;
                        if (length === 64) throw new Error('mutex name lacks a bounded terminator');
                        this.event.mutexName = args[2].readUtf8String(length);
                    }
                    if (name === 'CoInitialize') {
                        this.event.reservedIsNull = args[0].isNull();
                        active = false;
                    }
                    record(this.event);
                },
                onLeave(result) {
                    if (!this.inScope) return;
                    record({name, phase: 'return', value: result.toUInt32()});
                }
            }));
        }
        handles.push(Interceptor.attach(main, {
            onEnter() {
                active = true;
                send({type: 'observed', phase: 'entry', address, scope: 'WinMain mutex prefix only'});
                record({name: 'WinMain', phase: 'entry'});
            }
        }));
        return {installed: true, target: main.toString(), module: module.name,
            scope: 'WinMain entry through CoInitialize; natural successful mutex path only'};
    },
    summary() { return {events, boundaryReached: events.some(e => e.name === 'CoInitialize'), completeMain: false}; },
    detach() { for (const handle of handles) handle.detach(); handles = []; }
};
