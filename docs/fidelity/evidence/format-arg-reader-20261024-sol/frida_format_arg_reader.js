'use strict';
// Retail 0x4A7EFF is a 13-byte cdecl cursor update/DWORD load, not a camera method.
// Replay invokes the freshly compiled native on Frida's RPC thread. DLL-host
// replay resumes retail before loading the DLL, avoiding suspended-loader waits.
// It does NOT establish natural candidate formatter reachability.
let listener = null;
let closed = false;
let count = 0;
let returned = 0;
let failures = [];
let pending = new Set();
let mode;
let replayCount = 0;
let replayCases = null;
let replayTotal = 0;
let invoke = null;
let replayAddress = null;
const MAX_CALLS = 65536;
function readable(address, bytes) {
    const range = Process.findRangeByAddress(address);
    if (!range || !range.protection.includes('r') ||
        address.add(bytes).compare(range.base.add(range.size)) > 0)
        throw new Error('unreadable typed cursor region');
}
function finish() {
    if (!closed) {
        if (pending.size) throw new Error('cannot close while native calls are pending');
        if (listener) listener.detach();
        listener = null;
        closed = true;
        send({type: 'arg_reader_complete', count, returned, pending: pending.size,
              failures, mode, replayCount, naturalCandidateFlow: false});
    }
    return {count, returned, pending: pending.size, failures, closed, mode, replayCount,
            verified: false, naturalCandidateFlow: false};
}
rpc.exports = {
    hook(address, layout) {
        send({type: 'arg_reader_setup', stage: 'entered'});
        const evidence = layout.evidenceInput;
        const retail = !evidence;
        if (retail && layout.moduleName.toLowerCase() !== 'igi.exe')
            throw new Error('natural capture requires retail image');
        if (retail && ptr(address).toUInt32() !== 0x4a7eff)
            throw new Error('retail target binding mismatch');
        const image = Process.getModuleByName(layout.moduleName);
        let target = image.base.add(ptr(address).sub(ptr(layout.preferredBase)));
        let artifact = null;
        mode = retail ? 'natural-retail' : 'retail-input-replay-before-startup';
        if (!retail && (!evidence || evidence.kind !== 'igi-retail-arg-reader-inputs' ||
            evidence.retailVa !== '0x004A7EFF' || !Number.isInteger(evidence.caseCount) ||
            evidence.casesStreamed !== true || evidence.caseCount < 1 || evidence.caseCount > MAX_CALLS))
            throw new Error('candidate replay requires complete retained retail inputs');
        if (!retail && evidence.compiledDllPath) {
            send({type: 'arg_reader_setup', stage: 'loading-compiled-dll'});
            const dll = Module.load(evidence.compiledDllPath);
            send({type: 'arg_reader_setup', stage: 'compiled-dll-loaded'});
            target = dll.getExportByName('Native_004A7EFF');
            const codeHex = Array.from(new Uint8Array(target.readByteArray(13)),
                                      byte => byte.toString(16).padStart(2, '0')).join('');
            if (codeHex !== evidence.expectedCodeHex) throw new Error('loaded candidate code mismatch');
            artifact = {name: dll.name, path: dll.path, base: dll.base.toString(), codeHex,
                        export: 'Native_004A7EFF', target: target.toString()};
            mode = 'compiled-dll-replay-in-retail-host';
        }
        if (retail) listener = Interceptor.attach(target, {
            onEnter(args) {
                // Gum may dispatch an already queued entry after detach while
                // leaving the RPC callback. Closure is an explicit admission
                // boundary, with zero pending admitted calls; retain later
                // dispatches as outside-window metadata rather than inventing
                // a truncated native entry or treating them as probe failures.
                if (closed) {
                    send({type: 'arg_reader_outside_window',
                          callerRva: this.returnAddress.sub(image.base).toString()});
                    return;
                }
                if (count >= MAX_CALLS) {
                    if (!failures.length) {
                        failures.push('argument-reader capacity exceeded; entire capture rejected');
                        send({type: 'overflow', capacity: MAX_CALLS});
                    }
                    return;
                }
                this.index = ++count;
                pending.add(this.index);
                this.cell = args[0];
                readable(this.cell, 4);
                this.cursor = this.cell.readPointer();
                readable(this.cursor, 4);
                this.before = this.cursor.readU32();
                const overlap = this.cell.compare(this.cursor.add(4)) < 0 &&
                    this.cursor.compare(this.cell.add(4)) < 0;
                send({type: 'observed', phase: 'entry', address, callIndex: this.index,
                    inputWord: this.before, cursorCellOverlapsValue: overlap,
                    cellAddress: this.cell.toString(), cursorAddress: this.cursor.toString(),
                    callerRva: this.returnAddress.sub(image.base).toString(), mode});
            },
            onLeave(value) {
                if (!this.index) return;
                try {
                    const after = this.cell.readPointer();
                    send({type: 'observed', phase: 'return', address, callIndex: this.index,
                        resultWord: value.toUInt32(),
                        cursorAdvance: after.sub(this.cursor).toInt32(),
                        originalWordAfter: this.cursor.readU32(),
                        cursorAddressAfter: after.toString(), mode});
                    returned++;
                } catch (error) { failures.push(String(error)); throw error; }
                finally { pending.delete(this.index); }
            }
        });
        if (!retail) {
            // No return value is replaced. The actual compiled native executes
            // once for each captured input; only owned test memory is supplied.
            invoke = new NativeFunction(target, 'uint32', ['pointer'],
                                        {abi: 'mscdecl', traps: 'none'});
            replayTotal = evidence.caseCount;
            replayAddress = address;
            send({type: 'arg_reader_setup', stage: 'native-call-interface-ready'});
        }
        return {installed: true, target: target.toString(), mode,
                naturalCandidateFlow: false, pendingReplay: !retail,
                replayTotal: retail ? 0 : replayTotal, artifact};
    },
    replaynext(cases) {
        if (!invoke || closed || !Array.isArray(cases) || cases.length < 1 || cases.length > 256 ||
            replayCount + cases.length > replayTotal) throw new Error('invalid pending replay batch');
        // Bounded RPC batches let the driver observe progress while the main
        // thread stays suspended. Candidate calls execute directly, without
        // recursively intercepting the same function from its JS caller.
        for (const input of cases) {
            if (!Number.isInteger(input.inputWord) || input.inputWord < 0 ||
                input.inputWord > 0xffffffff || input.cursorCellOverlapsValue !== false)
                throw new Error('unrepresentable or overlapping recorded input');
            const word = Memory.alloc(4);
            const cell = Memory.alloc(4);
            word.writeU32(input.inputWord);
            cell.writePointer(word);
            const index = ++count;
            pending.add(index);
            send({type: 'observed', phase: 'entry', address: replayAddress, callIndex: index,
                inputWord: word.readU32(), cursorCellOverlapsValue: false,
                cellAddress: cell.toString(), cursorAddress: word.toString(), mode});
            const result = invoke(cell);
            const after = cell.readPointer();
            send({type: 'observed', phase: 'return', address: replayAddress, callIndex: index,
                resultWord: result >>> 0, cursorAdvance: after.sub(word).toInt32(),
                originalWordAfter: word.readU32(), cursorAddressAfter: after.toString(), mode});
            returned++;
            pending.delete(index);
            replayCount++;
        }
        const done = replayCount === replayTotal;
        if (done) finish();
        return {done, completed: replayCount, total: replayTotal};
    },
    summary() { return finish(); },
    detach() { if (listener) { listener.detach(); listener = null; } }
};
