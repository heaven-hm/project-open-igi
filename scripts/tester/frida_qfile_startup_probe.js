'use strict';

/*
 * Tester probe: retail startup/level-load observation of five landed qfile/sys natives.
 *
 * These are retail VAs, cdecl. This script only hooks and reads; it never writes game memory.
 * Unknown pointers are checked with Process.findRangeByAddress before any dereference.
 *
 * Targets:
 *   0x004B1620 QFile_LookupDeviceByName_4B1620   const char* name -> uint32
 *   0x004B13A0 QFile_ClearEntryByName_4B13A0      const char* name -> void
 *   0x004B1A90 QFile_FindRecordAtAddress_4B1A90   uint32 address -> void
 *   0x004B0E00 Sys_AddRenderMode_4B0E00           const char* name -> void
 *   0x004B0E70 Sys_AddSlot_4B0E70                 const char* name -> void
 */

const MAX_CALLS_PER_TARGET = 8;
const MAX_STRING = 160;
const MAX_LIST = 512;
const MAX_SLOT_RECORDS = 128;
const MAX_RENDERMODE_RECORDS = 8;
const MAX_CLEAR_RECORDS = 32;

const TARGETS = [
    { id: 'lookup', va: '0x004b1620', label: 'QFile_LookupDeviceByName_4B1620', arg: 'string', ret: 'u32' },
    { id: 'clear', va: '0x004b13a0', label: 'QFile_ClearEntryByName_4B13A0', arg: 'string', ret: 'void' },
    { id: 'walker', va: '0x004b1a90', label: 'QFile_FindRecordAtAddress_4B1A90', arg: 'u32', ret: 'void' },
    { id: 'rendermode', va: '0x004b0e00', label: 'Sys_AddRenderMode_4B0E00', arg: 'string', ret: 'void' },
    { id: 'slot', va: '0x004b0e70', label: 'Sys_AddSlot_4B0E70', arg: 'string', ret: 'void' }
];

const installed = [];
const failures = [];
const counts = {};
const first = {};
const last = {};

function hex(value) {
    try {
        return value.toString();
    } catch (error) {
        return null;
    }
}

function safeRange(address) {
    if (address === null || address === undefined) return null;
    try {
        if (address.isNull()) return null;
        const range = Process.findRangeByAddress(address);
        if (!range) return null;
        if (range.protection && range.protection.indexOf('r') < 0) return null;
        return range;
    } catch (error) {
        return null;
    }
}

function safeU8(address) {
    if (!safeRange(address)) return null;
    try { return address.readU8(); } catch (error) { return null; }
}

function safeU16(address) {
    if (!safeRange(address)) return null;
    try { return address.readU16(); } catch (error) { return null; }
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
        try {
            byte = address.add(i).readU8();
        } catch (error) {
            return text.length ? text : null;
        }
        if (byte === 0) return text;
        text += String.fromCharCode(byte);
    }
    return text;
}

function readCdeclArgWord(index) {
    try {
        const esp = this.context.esp;
        if (!esp) return null;
        return esp.add(4 + index * 4);
    } catch (error) {
        return null;
    }
}

function snapLookup() {
    const names = [];
    const values = [];
    try {
        for (let i = 0; i < 16; i++) {
            const slot = ptr('0x00546f04').add(i * 4);
            const namePtr = safePtr(slot);
            if (namePtr === null || namePtr.isNull()) {
                return { names: names, values: values, terminatorIndex: i, nonNullNames: names.length };
            }
            const text = safeCString(namePtr);
            names.push({ index: i, pointer: hex(namePtr), text: text });
            values.push(safeU32(ptr('0x00534a70').add(i * 4)));
        }
    } catch (error) {
        return { error: String(error) };
    }
    return { names: names, values: values, terminatorIndex: null, nonNullNames: names.length };
}

function snapClear() {
    const records = [];
    try {
        for (let i = 0; i < MAX_CLEAR_RECORDS; i++) {
            const record = ptr('0x00942330').add(i * 0x94);
            const parallel = ptr('0x009423c0').add(i * 0x94);
            records.push({
                index: i,
                name: safeCString(record, 0x94),
                flag90: safeU32(record.add(0x90)),
                parallel: safeU32(parallel)
            });
        }
    } catch (error) {
        return { error: String(error) };
    }
    return { records: records };
}

function snapWalker() {
    const records = [];
    let head = null;
    try {
        head = safePtr(ptr('0x00943970'));
        let current = head;
        for (let i = 0; i < MAX_LIST; i++) {
            if (current === null || current.isNull()) break;
            if (!safeRange(current)) break;
            let next = null;
            try { next = current.readPointer(); } catch (error) { next = null; }
            records.push({
                index: i,
                address: hex(current),
                next: next === null ? null : hex(next),
                address18: safeU32(current.add(0x18))
            });
            if (next === null || next.isNull()) break;
            if (hex(next) === hex(current)) break;
            current = next;
        }
    } catch (error) {
        return { head: head === null ? null : hex(head), records: records, error: String(error) };
    }
    return { head: head === null ? null : hex(head), records: records };
}

function snapRenderModes() {
    const records = [];
    let used = 0;
    try {
        for (let i = 0; i < MAX_RENDERMODE_RECORDS; i++) {
            const record = ptr('0x00a950a0').add(i * 0x208);
            const namePtr = safePtr(record);
            const flag = safeU8(record.add(0x204));
            if (flag !== 0 && flag !== null) used += 1;
            records.push({
                index: i,
                namePointer: namePtr === null ? null : hex(namePtr),
                name: namePtr === null ? null : safeCString(namePtr),
                flag204: flag
            });
        }
    } catch (error) {
        return { error: String(error) };
    }
    return { records: records, used: used };
}

function snapSlots() {
    const records = [];
    let used = 0;
    try {
        for (let i = 0; i < MAX_SLOT_RECORDS; i++) {
            const record = ptr('0x00a94a80').add(i * 8);
            const namePtr = safePtr(record);
            const flag = safeU8(record.add(4));
            if (flag !== 0 && flag !== null) used += 1;
            records.push({
                index: i,
                namePointer: namePtr === null ? null : hex(namePtr),
                name: namePtr === null ? null : safeCString(namePtr),
                flag4: flag
            });
        }
    } catch (error) {
        return { error: String(error) };
    }
    return { records: records, used: used };
}

function snapshot(id) {
    try {
        if (id === 'lookup') return snapLookup();
        if (id === 'clear') return snapClear();
        if (id === 'walker') return snapWalker();
        if (id === 'rendermode') return snapRenderModes();
        if (id === 'slot') return snapSlots();
    } catch (error) {
        return { error: String(error) };
    }
    return { error: 'unknown target' };
}

function lookupMatch(argText) {
    if (argText === null) return { matchIndex: null };
    const table = snapLookup();
    if (!table.names) return { matchIndex: null, error: table.error || null };
    const index = table.names.findIndex(function (entry) { return entry.text === argText; });
    return {
        matchIndex: index,
        matchPointer: index >= 0 ? table.names[index].pointer : null,
        matchValue: index >= 0 ? table.values[index] : null
    };
}

function clearMatch(argText) {
    if (argText === null) return { matchIndex: null };
    for (let i = 0; i < MAX_CLEAR_RECORDS; i++) {
        const record = ptr('0x00942330').add(i * 0x94);
        const flag = safeU32(record.add(0x90));
        if (flag === null || flag === 0) continue;
        const name = safeCString(record, 0x94);
        if (name === argText) {
            return {
                matchIndex: i,
                matchName: name,
                flag90Before: flag,
                parallelNow: safeU32(ptr('0x009423c0').add(i * 0x94))
            };
        }
    }
    return { matchIndex: -1 };
}

function walkerMatch(argU32) {
    if (argU32 === null) return { argMatchesRecord: null };
    const table = snapWalker();
    if (!table.records) return { argMatchesRecord: null, error: table.error || null };
    return {
        argMatchesRecord: table.records.some(function (entry) { return entry.address18 === argU32; }),
        listLength: table.records.length
    };
}

function firstFreeSlot() {
    for (let i = 0; i < MAX_SLOT_RECORDS; i++) {
        const flag = safeU8(ptr('0x00a94a80').add(i * 8 + 4));
        if (flag === 0) return i;
    }
    return null;
}

function firstFreeRenderMode() {
    for (let i = 0; i < MAX_RENDERMODE_RECORDS; i++) {
        const flag = safeU8(ptr('0x00a950a0').add(i * 0x208 + 0x204));
        if (flag === 0) return i;
    }
    return null;
}

function firstDifference(before, after) {
    try {
        if (!before || !after) return null;
        if (JSON.stringify(before) !== JSON.stringify(after)) {
            return { changed: true };
        }
        return { changed: false };
    } catch (error) {
        return { error: String(error) };
    }
}

function install(target) {
    const address = ptr(target.va);
    try {
        Interceptor.attach(address, {
            onEnter: function (args) {
                this.target = target;
                this.argAddress = readCdeclArgWord.call(this, 0);
                this.caller = null;
                try { this.caller = hex(this.returnAddress); } catch (error) { this.caller = null; }
                this.argRaw = null;
                this.argText = null;
                this.argU32 = null;
                if (this.argAddress !== null) {
                    try {
                        if (target.arg === 'u32') {
                            this.argU32 = this.argAddress.readU32();
                            this.argRaw = this.argAddress.toString();
                        } else {
                            const pointer = this.argAddress.readPointer();
                            this.argRaw = pointer === null ? null : hex(pointer);
                            this.argText = pointer === null ? null : safeCString(pointer);
                        }
                    } catch (error) {
                        this.argRaw = null;
                    }
                }
                const count = counts[target.id] || 0;
                counts[target.id] = count + 1;
                this.callIndex = count;
                this.isFirst = count === 0;
                this.before = this.isFirst ? snapshot(target.id) : null;
                this.firstFreeBefore = null;
                if (target.id === 'slot') this.firstFreeBefore = firstFreeSlot();
                if (target.id === 'rendermode') this.firstFreeBefore = firstFreeRenderMode();
                if (this.isFirst) {
                    send({
                        type: 'first-entry',
                        id: target.id,
                        label: target.label,
                        address: target.va,
                        callIndex: this.callIndex,
                        caller: this.caller,
                        argRaw: this.argRaw,
                        argText: this.argText,
                        argU32: this.argU32,
                        before: this.before
                    });
                }
            },
            onLeave: function (retval) {
                const target = this.target;
                const after = this.isFirst ? snapshot(target.id) : null;
                let ret = null;
                if (target.ret === 'u32') {
                    try { ret = retval.toUInt32(); } catch (error) { ret = null; }
                }
                let match = null;
                if (target.id === 'lookup') match = lookupMatch(this.argText);
                if (target.id === 'clear') match = clearMatch(this.argText);
                if (target.id === 'walker') match = walkerMatch(this.argU32);
                if (target.id === 'slot') {
                    const afterFree = firstFreeSlot();
                    match = {
                        firstFreeBefore: this.firstFreeBefore,
                        firstFreeAfter: afterFree,
                        newIndex: this.firstFreeBefore !== null && afterFree !== this.firstFreeBefore ? this.firstFreeBefore : null
                    };
                }
                if (target.id === 'rendermode') {
                    const afterFree = firstFreeRenderMode();
                    match = {
                        firstFreeBefore: this.firstFreeBefore,
                        firstFreeAfter: afterFree,
                        newIndex: this.firstFreeBefore !== null && afterFree !== this.firstFreeBefore ? this.firstFreeBefore : null
                    };
                }
                const record = {
                    type: 'call',
                    id: target.id,
                    label: target.label,
                    address: target.va,
                    callIndex: this.callIndex,
                    caller: this.caller,
                    argRaw: this.argRaw,
                    argText: this.argText,
                    argU32: this.argU32,
                    ret: ret,
                    match: match,
                    before: this.before,
                    after: after,
                    firstDifference: firstDifference(this.before, after)
                };
                last[target.id] = record;
                if (this.isFirst) {
                    first[target.id] = record;
                    if (match) {
                        if (match.matchIndex !== undefined) first[target.id].matchIndex = match.matchIndex;
                        if (match.matchValue !== undefined) first[target.id].matchValue = match.matchValue;
                        if (match.argMatchesRecord !== undefined) first[target.id].argMatchesRecord = match.argMatchesRecord;
                        if (match.newIndex !== undefined) first[target.id].newIndex = match.newIndex;
                    }
                }
                send(record);
            }
        });
        installed.push({ id: target.id, label: target.label, address: target.va });
    } catch (error) {
        failures.push({ id: target.id, address: target.va, error: String(error) });
    }
}

function moduleInfo() {
    try {
        const module = Process.getModuleByName('igi.exe');
        send({ type: 'module', name: module.name, base: module.base.toString(), size: module.size });
    } catch (error) {
        try {
            const matches = Process.enumerateModules().filter(function (m) {
                return m.name.toLowerCase() === 'igi.exe';
            });
            if (matches.length) {
                send({ type: 'module', name: matches[0].name, base: matches[0].base.toString(), size: matches[0].size });
            } else {
                send({ type: 'module', error: String(error), modules: Process.enumerateModules().map(function (m) { return m.name; }) });
            }
        } catch (inner) {
            send({ type: 'module', error: String(inner) });
        }
    }
}

TARGETS.forEach(install);
moduleInfo();
send({ type: 'ready', installed: installed, failures: failures, targets: TARGETS.map(function (t) { return t.id; }) });

rpc.exports = {
    snapshot: function () {
        const finals = {};
        for (const target of TARGETS) {
            finals[target.id] = snapshot(target.id);
        }
        return {
            installed: installed,
            failures: failures,
            counts: counts,
            first: first,
            last: last,
            final: finals
        };
    }
};
