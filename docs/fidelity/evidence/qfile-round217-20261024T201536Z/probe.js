'use strict';

/*
 * Round 217 retail probe.
 *
 * Targets:
 *   0x004B19C0  loaded-file record creator
 *   0x004B1A90  loaded-file record walker
 *   0x004B12F0  populator of 0x00942330 after 0x004B13A0 clears
 *   0x004B1510  caller of 0x004B1620 (lookup), plus the post-call sites
 *
 * Retail only, cdecl, read-only instrumentation. All unknown pointers are
 * checked with Process.findRangeByAddress before dereference. Reads after a
 * record are bounded by the allocation size captured from Mem_Alloc.
 */

const MAX_LIST = 512;
const MAX_STRING = 192;
const MAX_CREATOR_CALLS = 32;
const MAX_WALKER_EVENTS = 20;
const MAX_LOOKUP_CALLS = 12;
const MAX_ALLOC_EVENTS = 2000;

const installed = [];
const failures = [];
const counts = {};
const allocations = {};
const creatorRecords = [];
const creatorSeq = [];
const populatorEntries = [];
const populatorExits = [];
const clearCalls = [];
const writerSites = [];
const lookupCalls = [];
const lookupReturns = [];
const walkerEvents = [];

function hex(value) {
    try { return value.toString(); } catch (error) { return null; }
}

function safeRange(address) {
    if (address === null || address === undefined) return null;
    try {
        if (address.isNull()) return null;
        const range = Process.findRangeByAddress(address);
        if (!range) return null;
        if (range.protection && range.protection.indexOf('r') < 0) return null;
        return range;
    } catch (error) { return null; }
}

function safeU8(address) {
    if (!safeRange(address)) return null;
    try { return address.readU8(); } catch (error) { return null; }
}

function safeU32(address) {
    if (!safeRange(address)) return null;
    try { return address.readU32(); } catch (error) { return null; }
}

function safePtr(address) {
    if (!safeRange(address)) return null;
    try { return address.readPointer(); } catch (error) { return null; }
}

function safeCString(address, max) {
    if (!safeRange(address)) return null;
    const limit = max || MAX_STRING;
    let text = "";
    for (let i = 0; i < limit; i++) {
        let byte = null;
        try { byte = address.add(i).readU8(); } catch (error) { return text.length ? text : null; }
        if (byte === 0) return text;
        text += String.fromCharCode(byte);
    }
    return text;
}

function safeBytes(address, length) {
    if (length <= 0 || !safeRange(address)) return null;
    try {
        const buffer = address.readByteArray(length);
        if (buffer === null) return null;
        const view = new Uint8Array(buffer);
        let out = "";
        for (let i = 0; i < view.length; i++) {
            out += ("0" + view[i].toString(16)).slice(-2);
        }
        return out;
    } catch (error) { return null; }
}

function frameInfo(address) {
    try {
        const module = Process.findModuleByAddress(address);
        return {
            address: hex(address),
            module: module ? module.name : null,
            offset: module ? hex(address.sub(module.base)) : null
        };
    } catch (error) {
        return { address: hex(address), module: null, offset: null };
    }
}

function backtrace(context) {
    try {
        const frames = Thread.backtrace(context, Backtracer.ACCURATE);
        if (frames.length) return frames.slice(0, 12).map(frameInfo);
    } catch (error) { /* fall through */ }
    try {
        return Thread.backtrace(context, Backtracer.FUZZY).slice(0, 12).map(frameInfo);
    } catch (inner) {
        return { error: String(inner) };
    }
}

function cdeclArg(index) {
    try {
        const esp = this.context.esp;
        if (!esp) return null;
        return esp.add(4 + index * 4);
    } catch (error) { return null; }
}

function readCdeclString(index) {
    const address = cdeclArg.call(this, index);
    if (address === null) return { pointer: null, text: null };
    try {
        const pointer = address.readPointer();
        return {
            pointer: pointer === null ? null : hex(pointer),
            text: pointer === null ? null : safeCString(pointer)
        };
    } catch (error) {
        return { pointer: null, text: null, error: String(error) };
    }
}

function currentAllocSize(pointer) {
    if (pointer === null) return null;
    return allocations[pointer.toString()] || null;
}

function linkKind(record, link) {
    if (link === null) return 'null';
    if (hex(link) === '0x943974') return 'sentinel_0x943974';
    for (let i = 0; i < creatorRecords.length; i++) {
        const previous = creatorRecords[i];
        if (previous.pointer === null) continue;
        const start = previous.pointer;
        const size = previous.allocSize || 0x30;
        if (link.compare(start) >= 0 && link.compare(start.add(size)) < 0) {
            return 'inside_previous_record_' + previous.callIndex;
        }
    }
    return safeRange(link) ? 'readable_other' : 'unreadable';
}

function recordSnapshot(pointer, allocSize) {
    if (pointer === null || pointer.isNull()) {
        return { pointer: null, error: 'null record pointer' };
    }
    if (!safeRange(pointer)) {
        return { pointer: hex(pointer), error: 'record pointer not readable' };
    }
    const knownSize = (allocSize === null || allocSize === undefined) ? null : allocSize;
    // Never read past the allocation. If the size is unknown, 0x1C is the header
    // the creator unconditionally allocates (strlen(name) + 0x1C).
    const want = knownSize === null ? 0x1C : Math.min(0x30, knownSize);
    const bytes = safeBytes(pointer, want);
    const namePointer = safePtr(pointer.add(8));
    const directNameMax = knownSize === null ? 0x1C : Math.max(0, Math.min(128, knownSize - 0x1C));
    return {
        pointer: hex(pointer),
        allocSize: knownSize,
        capturedBytes: want,
        bytesHex: bytes,
        truncatedBefore0x30: knownSize !== null && knownSize < 0x30,
        fields: {
            u32_00: safeU32(pointer),
            ptr_04_raw: hex(safePtr(pointer.add(4))),
            ptr_08_name: hex(namePointer),
            u32_0c: safeU32(pointer.add(0x0c)),
            u32_10: safeU32(pointer.add(0x10)),
            u32_14: safeU32(pointer.add(0x14)),
            u32_18: safeU32(pointer.add(0x18)),
            u32_18_valid_readable: safeRange(safePtr(pointer.add(0x18))) !== null
        },
        namePointerValue: namePointer === null ? null : safeCString(namePointer, directNameMax > 0 ? directNameMax : 0),
        directName: safeCString(pointer.add(0x1c), directNameMax > 0 ? directNameMax : 0),
        link04: hex(safePtr(pointer.add(4))),
        link04Kind: linkKind(pointer, safePtr(pointer.add(4))),
        link04SamePage: (function () {
            const link = safePtr(pointer.add(4));
            if (link === null) return null;
            try { return Math.floor(link.toString() / 0x1000) === Math.floor(pointer.toString() / 0x1000); } catch (error) { return null; }
        })()
    };
}

function scanTableRecordByName(name) {
    if (name === null) return null;
    for (let i = 0; i < 32; i++) {
        const record = ptr('0x00942330').add(i * 0x94);
        const flag = safeU32(record.add(0x90));
        if (flag === null || flag === 0) continue;
        const text = safeCString(record, 0x94);
        if (text !== name) continue;
        return {
            index: i,
            pointer: hex(record),
            name: text,
            flag90: flag,
            u32_10: safeU32(record.add(0x10)),
            u32_14: safeU32(record.add(0x14)),
            parallel: safeU32(ptr('0x009423c0').add(i * 0x94))
        };
    }
    return null;
}

function findWalkerMatch(addressValue) {
    let current = safePtr(ptr('0x00943970'));
    const seen = {};
    for (let i = 0; i < MAX_LIST; i++) {
        if (current === null || current.isNull()) break;
        if (!safeRange(current)) break;
        const key = current.toString();
        if (seen[key]) break;
        seen[key] = true;
        const address18 = safeU32(current.add(0x18));
        if (address18 === addressValue) {
            const record = recordSnapshot(current, currentAllocSize(current));
            record.listIndex = i;
            return record;
        }
        current = safePtr(current);
    }
    return null;
}

function install(address, label, handlers) {
    try {
        Interceptor.attach(ptr(address), handlers);
        installed.push({ address: address, label: label });
    } catch (error) {
        failures.push({ address: address, label: label, error: String(error) });
    }
}

// Mem_Alloc(0x4B0C60): allocate request size [esp+8], return payload pointer.
install('0x4b0c60', 'Mem_Alloc', {
    onEnter: function () {
        this.requestSize = cdeclArg.call(this, 1) === null ? null : cdeclArg.call(this, 1).readU32();
        this.allocCaller = null;
        try { this.allocCaller = hex(this.returnAddress); } catch (error) { this.allocCaller = null; }
    },
    onLeave: function (retval) {
        try {
            const pointer = retval;
            if (pointer === null || pointer.isNull()) return;
            const key = pointer.toString();
            allocations[key] = this.requestSize === null ? null : this.requestSize;
            if (Object.keys(allocations).length <= MAX_ALLOC_EVENTS) {
                // Allocation events are supporting evidence; keep the first finite batch only.
                send({
                    type: 'alloc',
                    pointer: key,
                    requestSize: this.requestSize,
                    caller: this.allocCaller
                });
            }
        } catch (error) { /* allocation bookkeeping must not disturb the target */ }
    }
});

// 0x004B19C0 record creator.
install('0x4b19c0', 'QFile_CreateLoadedFileRecord', {
    onEnter: function () {
        this.nameArg = readCdeclString.call(this, 0);
        this.creatorCaller = null;
        try { this.creatorCaller = hex(this.returnAddress); } catch (error) { this.creatorCaller = null; }
        this.stack = null;
        try { this.stack = backtrace(this.context); } catch (error) { this.stack = { error: String(error) }; }
        counts.creator = (counts.creator || 0) + 1;
    },
    onLeave: function (retval) {
        const recordPointer = retval;
        const allocSize = currentAllocSize(recordPointer);
        const snapshot = recordSnapshot(recordPointer, allocSize);
        snapshot.callIndex = creatorSeq.length;
        snapshot.creatorCaller = this.creatorCaller;
        snapshot.nameArg = this.nameArg;
        snapshot.stack = this.stack;
        creatorSeq.push(snapshot.pointer);
        creatorRecords.push({
            callIndex: snapshot.callIndex,
            pointer: recordPointer === null ? null : recordPointer,
            allocSize: allocSize,
            snapshot: snapshot
        });
        if (creatorSeq.length <= MAX_CREATOR_CALLS) send({ type: 'creator', data: snapshot });
    }
});

// 0x004B1A90 record walker: record the matching record's fields at each call.
install('0x4b1a90', 'QFile_FindRecordAtAddress', {
    onEnter: function () {
        const argAddress = cdeclArg.call(this, 0);
        const value = argAddress === null ? null : safeU32(argAddress);
        counts.walker = (counts.walker || 0) + 1;
        const match = value === null ? null : findWalkerMatch(value);
        const event = {
            type: 'walker',
            callIndex: counts.walker - 1,
            arg: value,
            match: match
        };
        if (walkerEvents.length < MAX_WALKER_EVENTS) {
            walkerEvents.push(event);
            send(event);
        }
    }
});

// 0x004B12F0 populator of 0x00942330.
install('0x4b12f0', 'QFile_TablePopulator', {
    onEnter: function () {
        const arg1 = readCdeclString.call(this, 0);
        const arg2 = readCdeclString.call(this, 1);
        const entry = {
            type: 'populate-entry',
            callIndex: populatorEntries.length,
            arg1: arg1,
            arg2: arg2,
            caller: null,
            stack: null
        };
        try { entry.caller = hex(this.returnAddress); } catch (error) { entry.caller = null; }
        try { entry.stack = backtrace(this.context); } catch (error) { entry.stack = { error: String(error) }; }
        populatorEntries.push(entry);
        send(entry);
        this.popCallIndex = entry.callIndex;
        this.popArg1 = arg1.text;
        this.popArg2 = arg2.text;
        counts.populator = (counts.populator || 0) + 1;
    },
    onLeave: function (retval) {
        const name = this.popArg1 || this.popArg2;
        const record = scanTableRecordByName(name);
        const exit = {
            type: 'populate-exit',
            callIndex: this.popCallIndex,
            name: name,
            record: record,
            ret: null
        };
        try { exit.ret = hex(retval); } catch (error) { exit.ret = null; }
        populatorExits.push(exit);
        send(exit);
    }
});

// 0x004B13A0 clear-by-name: order witness, not re-observed in detail.
install('0x4b13a0', 'QFile_ClearEntryByName', {
    onEnter: function () {
        const arg = readCdeclString.call(this, 0);
        const event = {
            type: 'clear-call',
            callIndex: clearCalls.length,
            name: arg,
            caller: null
        };
        try { event.caller = hex(this.returnAddress); } catch (error) { event.caller = null; }
        clearCalls.push(event);
        send(event);
    }
});

// Exact populator copy sites: rep movsd / rep movsb into 0x00942330 records.
install('0x4b1360', 'populator-rep-movsd', {
    onEnter: function () {
        const destination = this.context.edi;
        const source = this.context.esi;
        const count = this.context.ecx.toUInt32();
        writerSites.push({
            type: 'write-site',
            site: '0x4b1360',
            destination: hex(destination),
            source: hex(source),
            count: count,
            allocationSize: currentAllocSize(destination)
        });
        send(writerSites[writerSites.length - 1]);
    }
});

install('0x4b1369', 'populator-rep-movsb', {
    onEnter: function () {
        const destination = this.context.edi;
        const source = this.context.esi;
        const count = this.context.ecx.toUInt32();
        writerSites.push({
            type: 'write-site',
            site: '0x4b1369',
            destination: hex(destination),
            source: hex(source),
            count: count
        });
        send(writerSites[writerSites.length - 1]);
    }
});

// 0x004B1510 caller of the 0x004B1620 lookup.
install('0x4b1510', 'QFile_OpenPath_Caller', {
    onEnter: function () {
        if (lookupCalls.length >= MAX_LOOKUP_CALLS) return;
        const arg1 = readCdeclString.call(this, 0);
        const arg2 = readCdeclString.call(this, 1);
        const event = {
            type: 'lookup-caller-entry',
            callIndex: lookupCalls.length,
            arg1: arg1,
            arg2: arg2,
            caller: null,
            stack: null
        };
        try { event.caller = hex(this.returnAddress); } catch (error) { event.caller = null; }
        try { event.stack = backtrace(this.context); } catch (error) { event.stack = { error: String(error) }; }
        lookupCalls.push(event);
        send(event);
    },
    onLeave: function (retval) {
        if (lookupReturns.length >= MAX_LOOKUP_CALLS) return;
        const event = { type: 'lookup-caller-return', ret: null };
        try { event.ret = hex(retval); } catch (error) { event.ret = null; }
        lookupReturns.push(event);
        send(event);
    }
});

send({ type: 'ready', installed: installed, failures: failures });
send({
    type: 'module',
    name: (function () { try { return Process.getModuleByName('igi.exe').name; } catch (error) { return null; } })(),
    base: (function () { try { return Process.getModuleByName('igi.exe').base.toString(); } catch (error) { return null; } })(),
    size: (function () { try { return Process.getModuleByName('igi.exe').size; } catch (error) { return null; } })()
});

rpc.exports = {
    snapshot: function () {
        const records = creatorRecords.map(function (entry) {
            return {
                callIndex: entry.callIndex,
                allocSize: entry.allocSize,
                initial: entry.snapshot,
                final: recordSnapshot(entry.pointer, entry.allocSize)
            };
        });
        const deltas = [];
        for (let i = 1; i < records.length; i++) {
            const previous = records[i - 1].initial.pointer;
            const current = records[i].initial.pointer;
            if (previous !== null && current !== null) {
                try {
                    deltas.push({
                        fromCall: i - 1,
                        toCall: i,
                        delta: ptr(current).sub(ptr(previous)).toString()
                    });
                } catch (error) { /* ignore */ }
            }
        }
        return {
            installed: installed,
            failures: failures,
            counts: counts,
            records: records,
            recordPointerDeltas: deltas,
            populatorEntries: populatorEntries,
            populatorExits: populatorExits,
            clearCalls: clearCalls,
            writerSites: writerSites,
            lookupCalls: lookupCalls,
            lookupReturns: lookupReturns,
            walkerEvents: walkerEvents,
            walkerTotal: counts.walker || 0,
            allocationsCount: Object.keys(allocations).length
        };
    }
};
