'use strict';
// Retail-only observation of 0x401050 -> 0x4A5000, count=0.
// Hook before the two argument pushes and after CALL returns. Hooking the CALL
// itself would relocate its return address and contaminate the saved-EIP evidence.
// No onLeave hook is installed on the frame-builder itself.
let handles = [];
let pending = new Map();
let calls = 0;
let returns = 0;
let failures = 0;
let overflow = false;
rpc.exports = {
    hook(address, layout) {
        if (ptr(address).toUInt32() !== 0x4a5000 || layout.moduleName.toLowerCase() !== 'igi.exe')
            throw new Error('retail-only probe; candidate address mapping is unresolved');
        const module = Process.getModuleByName(layout.moduleName);
        const va = value => module.base.add(value - 0x400000);
        const buffer = va(0x54f8e0);
        const maximum = layout.maxCalls;
        handles.push(Interceptor.attach(va(0x401054), {
            onEnter() {
                if (calls >= maximum) {
                    if (!overflow) send({type: 'overflow', address, maxCalls: maximum});
                    overflow = true;
                    return;
                }
                const tid = Process.getCurrentThreadId();
                if (pending.has(tid)) throw new Error('nested frame-builder call');
                const c = this.context;
                const expected = [c.ebp, c.ebx, c.edi, c.esi, c.esp.sub(12), va(0x401060)].map(p => p.toUInt32());
                const tail = Array.from(new Uint8Array(buffer.add(40).readByteArray(24)));
                const callIndex = ++calls;
                pending.set(tid, {expected, tail, callIndex});
                send({type: 'observed', address, phase: 'entry', callIndex, threadId: tid,
                    input: {count: 0, bufferIsGlobal54F8E0: true, expectedSavedWords: expected},
                    scope: 'retail QTask_OpenEvent direct frame save, count=0'});
            }
        }));
        handles.push(Interceptor.attach(va(0x401060), {
            onEnter() {
                const tid = Process.getCurrentThreadId();
                const before = pending.get(tid);
                if (!before) return;
                pending.delete(tid);
                const words = Array.from({length: 10}, (_, i) => buffer.add(4 * i).readU32());
                const tail = Array.from(new Uint8Array(buffer.add(40).readByteArray(24)));
                let scopeValue = null;
                if (words[6] !== 0xffffffff) {
                    const scopeAddress = ptr(words[6]).add(12);
                    const range = Process.findRangeByAddress(scopeAddress);
                    if (!range || !range.protection.includes('r') ||
                        scopeAddress.add(4).compare(range.base.add(range.size)) > 0)
                        throw new Error('SEH scope field is unreadable');
                    scopeValue = scopeAddress.readU32();
                }
                const checks = {
                    savedRegistersStackAndReturn: before.expected.every((value, i) => words[i] === value),
                    cookie: words[8] === 0x56433230,
                    noUnwindCallback: words[9] === 0,
                    sehScope: words[7] === (words[6] === 0xffffffff ? 0xffffffff : scopeValue),
                    unusedTailUnchanged: before.tail.every((value, i) => tail[i] === value),
                    zeroReturn: this.context.eax.toUInt32() === 0
                };
                if (!Object.values(checks).every(Boolean)) failures++;
                returns++;
                send({type: 'observed', address, phase: 'return', callIndex: before.callIndex,
                    threadId: tid, savedBufferWords: words, observedSehScope: scopeValue,
                    returnValue: this.context.eax.toUInt32(), checks});
            }
        }));
        return {installed: true, scope: 'retail count=0 only',
            sites: ['0x00401054 before argument pushes', '0x00401060 after return'],
            untouchedNative: '0x004A5000', buffer: buffer.toString()};
    },
    summary() { return {calls, returns, failures, pending: pending.size, overflow,
        verified: false, candidateObserved: false}; },
    detach() { for (const h of handles) h.detach(); handles = []; }
};
