'use strict';

/*
 * Alias-table probe: 0x004B12F0 (registrar), 0x004B11A0 (device index),
 * 0x004B1020 (retail reader), and the fixed buffer at 0x00943A18.
 *
 * Retail only. Every pointer is range-checked before dereference; strings are
 * read one byte at a time with bounded limits. Table snapshots are raw reads of
 * the known retail table (32 * 0x94 at 0x00942330).
 */

const MAX_STRING = 0x100;
const MAX_READER_CALLS = 16;
const MAX_DEVICE_EVENTS = 24;
const MAX_TABLE_RECORDS = 32;
const TABLE_BASE = '0x942330';
const TABLE_STRIDE = 0x94;
const TABLE_END = '0x9435b0';
const FIXED_BUFFER = '0x943a18';

const installed = [];
const failures = [];
const counts = {};
const populatorEntries = [];
const populatorExits = [];
const deviceCalls = [];
let lastDeviceCall = null;
const readerEntries = [];
const readerExits = [];
let readerCallCount = 0;
let fixedBufferMaxLength = 0;
let fixedBufferHistory = [];

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

function boundedString(pointer) {
    if (pointer === null) return null;
    return safeCString(pointer, MAX_STRING);
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
        if (frames.length) return frames.slice(0, 8).map(frameInfo);
    } catch (error) { /* fall through */ }
    try {
        return Thread.backtrace(context, Backtracer.FUZZY).slice(0, 8).map(frameInfo);
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
            pointer: hex(pointer),
            text: boundedString(pointer)
        };
    } catch (error) {
        return { pointer: null, text: null, error: String(error) };
    }
}

function readCdeclU32(index) {
    const address = cdeclArg.call(this, index);
    if (address === null) return null;
    try { return address.readU32(); } catch (error) { return null; }
}

function snapshotTable() {
    const occupied = [];
    for (let index = 0; index < MAX_TABLE_RECORDS; index++) {
        const record = ptr(TABLE_BASE).add(index * TABLE_STRIDE);
        if (!safeRange(record)) continue;
        const flag = safeU32(record.add(0x90));
        const key = safeCString(record, 0x80);
        const target = safeCString(record.add(0x10), 0x80);
        if (flag === 0 && (key === null || key.length === 0) && (target === null || target.length === 0)) continue;
        occupied.push({
            index: index,
            pointer: hex(record),
            key: key,
            target: target,
            handle90: flag,
            keyHex: (function () {
                try {
                    const buffer = record.readByteArray(0x10);
                    return buffer === null ? null : Array.from(new Uint8Array(buffer)).map(function (b) { return ('0' + b.toString(16)).slice(-2); }).join('');
                } catch (error) { return null; }
            })(),
            targetHex: (function () {
                try {
                    const buffer = record.add(0x10).readByteArray(0x20);
                    return buffer === null ? null : Array.from(new Uint8Array(buffer)).map(function (b) { return ('0' + b.toString(16)).slice(-2); }).join('');
                } catch (error) { return null; }
            })()
        });
    }
    return {
        occupied: occupied,
        occupiedCount: occupied.length,
        zeroCount: MAX_TABLE_RECORDS - occupied.length,
        tableBase: TABLE_BASE,
        tableEnd: TABLE_END,
        stride: TABLE_STRIDE
    };
}

function readFixedBuffer() {
    const pointer = ptr(FIXED_BUFFER);
    const text = safeCString(pointer, MAX_STRING);
    const length = text === null ? null : text.length;
    if (length !== null && length > fixedBufferMaxLength) fixedBufferMaxLength = length;
    return {
        pointer: FIXED_BUFFER,
        text: text,
        length: length,
        maxObservedLength: fixedBufferMaxLength,
        hitReadLimit: length !== null && length >= MAX_STRING
    };
}

function install(address, label, handlers) {
    try {
        Interceptor.attach(ptr(address), handlers);
        installed.push({ address: address, label: label });
    } catch (error) {
        failures.push({ address: address, label: label, error: String(error) });
    }
}

// 0x004B11A0: device index lookup. Signature cdecl (const char* resolved_path).
install('0x4b11a0', 'QFile_DeviceIndex', {
    onEnter: function () {
        this.arg = readCdeclString.call(this, 0);
        this.caller = null;
        try { this.caller = hex(this.returnAddress); } catch (error) { this.caller = null; }
    },
    onLeave: function (retval) {
        let value = null;
        try { value = retval.toInt32(); } catch (error) { value = null; }
        const event = {
            type: 'device-index',
            callIndex: counts.device || 0,
            arg: this.arg,
            ret: value,
            caller: this.caller
        };
        counts.device = (counts.device || 0) + 1;
        deviceCalls.push(event);
        lastDeviceCall = event;
        if (deviceCalls.length <= MAX_DEVICE_EVENTS) send(event);
    }
});

// 0x004B12F0: alias registrar. Signature cdecl (const char* key, const char* target).
install('0x4b12f0', 'QFile_RegisterAlias', {
    onEnter: function () {
        const entry = {
            type: 'populate-entry',
            callIndex: populatorEntries.length,
            arg1: readCdeclString.call(this, 0),
            arg2: readCdeclString.call(this, 1),
            caller: null,
            stack: null,
            tableBefore: snapshotTable(),
            deviceAtEntry: lastDeviceCall
        };
        try { entry.caller = hex(this.returnAddress); } catch (error) { entry.caller = null; }
        try { entry.stack = backtrace(this.context); } catch (error) { entry.stack = { error: String(error) }; }
        populatorEntries.push(entry);
        send(entry);
        this.popCallIndex = entry.callIndex;
        this.popArg1 = entry.arg1.text;
        this.popArg2 = entry.arg2.text;
        counts.populator = (counts.populator || 0) + 1;
    },
    onLeave: function (retval) {
        const tableAfter = snapshotTable();
        const deviceCall = lastDeviceCall;
        let ret = null;
        try { ret = hex(retval); } catch (error) { ret = null; }
        const matching = tableAfter.occupied.find(function (entry) {
            return (this.popArg1 !== null && entry.key === this.popArg1) ||
                   (this.popArg2 !== null && entry.key === this.popArg2);
        }, this) || null;
        const exit = {
            type: 'populate-exit',
            callIndex: this.popCallIndex,
            key: this.popArg1,
            target: this.popArg2,
            ret: ret,
            tableAfter: tableAfter,
            deviceCall: deviceCall,
            matchingRecord: matching,
            handleMatchesDevice: matching !== null && deviceCall !== null && matching.handle90 === deviceCall.ret,
            filledIndices: tableAfter.occupied.map(function (entry) { return entry.index; })
        };
        populatorExits.push(exit);
        send(exit);
    }
});

// 0x004B1020: retail alias reader. Signature cdecl (char* destination, const char* alias, int depth).
install('0x4b1020', 'QFile_AliasResolve', {
    onEnter: function () {
        if (readerCallCount >= MAX_READER_CALLS) return;
        const destination = (function () {
            try { return cdeclArg.call(this, 0).readPointer(); } catch (error) { return null; }
        }).call(this);
        const entry = {
            type: 'reader-entry',
            callIndex: readerCallCount,
            destination: hex(destination),
            alias: readCdeclString.call(this, 1),
            depth: readCdeclU32.call(this, 2),
            caller: null,
            stack: null,
            tableAtEntry: snapshotTable(),
            fixedBufferAtEntry: readFixedBuffer()
        };
        try { entry.caller = hex(this.returnAddress); } catch (error) { entry.caller = null; }
        try { entry.stack = backtrace(this.context); } catch (error) { entry.stack = { error: String(error) }; }
        readerEntries.push(entry);
        send(entry);
        this.readerIndex = readerCallCount;
        this.readerDestination = destination;
        this.readerAlias = entry.alias.text;
        this.readerDepth = entry.depth;
        readerCallCount += 1;
        counts.reader = readerCallCount;
    },
    onLeave: function (retval) {
        if (this.readerIndex === undefined) return;
        let output = null;
        if (this.readerDestination !== null) output = safeCString(this.readerDestination, MAX_STRING);
        const fixed = readFixedBuffer();
        fixedBufferHistory.push({
            callIndex: this.readerIndex,
            alias: this.readerAlias,
            output: output,
            fixedBuffer: fixed.text,
            fixedLength: fixed.length
        });
        if (fixedBufferHistory.length > 64) fixedBufferHistory.shift();
        let ret = null;
        try { ret = retval.toInt32(); } catch (error) { ret = null; }
        const event = {
            type: 'reader-exit',
            callIndex: this.readerIndex,
            destination: hex(this.readerDestination),
            alias: this.readerAlias,
            depth: this.readerDepth,
            output: output,
            outputLength: output === null ? null : output.length,
            ret: ret,
            fixedBuffer: fixed,
            outputEqualsAlias: output !== null && this.readerAlias !== null && output === this.readerAlias
        };
        readerExits.push(event);
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

send({ type: 'initial-table', table: snapshotTable(), fixedBuffer: readFixedBuffer() });

rpc.exports = {
    snapshot: function () {
        return {
            installed: installed,
            failures: failures,
            counts: counts,
            populatorEntries: populatorEntries,
            populatorExits: populatorExits,
            deviceCalls: deviceCalls,
            readerEntries: readerEntries,
            readerExits: readerExits,
            fixedBufferHistory: fixedBufferHistory,
            fixedBufferMaxLength: fixedBufferMaxLength,
            finalTable: snapshotTable(),
            finalFixedBuffer: readFixedBuffer(),
            lastDeviceCall: lastDeviceCall
        };
    }
};
