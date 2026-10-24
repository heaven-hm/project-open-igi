'use strict';
// Analysis-only retail probe of 0x00498480. The entry, 0x44A-byte extent,
// two writable DWORD outputs and import argument layouts come from the retained
// Binary Ninja/Radare2 listing. No API return or target state is changed.
// COM slots and caller sites are established by the same listing. Pointer
// identities are local ordinals, not addresses compared across processes.
let listeners = [];
let events = [];
let owner = null;
let count = 0;
let completed = 0;
let target;
let image;
let objectIds = new Map();
let hookedMethods = new Set();
let hookedCreators = new Set();
const callMethods = new Map([
    [0x98652, 'QueryInterface'], [0x987c1, 'QueryInterface'], [0x987f6, 'QueryInterface'],
    [0x98742, 'SetCooperativeLevel'], [0x98781, 'CreateSurface'],
    ...[0x98660, 0x98687, 0x986af, 0x986e7, 0x98750, 0x9878f,
        0x987cf, 0x98804, 0x98822, 0x9882c, 0x98882, 0x988bc].map(site => [site, 'Release'])
]);
function inNative(caller) {
    return owner === Process.getCurrentThreadId() && caller.compare(target) >= 0 &&
        caller.compare(target.add(0x44a)) < 0;
}
function identify(object) {
    const key = object.toString();
    if (!objectIds.has(key)) objectIds.set(key, objectIds.size + 1);
    return objectIds.get(key);
}
function observeInterface(object, primary) {
    if (object.isNull()) throw new Error('successful COM call returned NULL');
    identify(object);
    const table = object.readPointer();
    for (const slot of (primary ? [0, 2, 6, 20] : [0, 2])) {
        const methodAddress = table.add(4 * slot).readPointer();
        const key = methodAddress.toString();
        if (hookedMethods.has(key)) continue;
        hookedMethods.add(key);
        listeners.push(Interceptor.attach(methodAddress, {
            onEnter(args) {
                this.observed = inNative(this.returnAddress);
                if (!this.observed) return;
                const callerRva = this.returnAddress.sub(image.base).toUInt32();
                this.method = callMethods.get(callerRva);
                if (!this.method) throw new Error('unresolved COM caller site ' + callerRva.toString(16));
                const record = {type: 'directx_com', phase: 'entry', method: this.method,
                    objectId: identify(args[0]), callerRva};
                if (this.method === 'QueryInterface') {
                    record.interfaceGuid = hexBytes(args[1], 16);
                    this.output = args[2];
                } else if (this.method === 'CreateSurface') {
                    record.descriptor = hexBytes(args[1], 0x6c);
                    record.outerIsNull = args[3].isNull();
                    this.output = args[2];
                } else if (this.method === 'SetCooperativeLevel') {
                    record.windowIsNull = args[1].isNull();
                    record.flags = args[2].toUInt32();
                }
                emit(record);
            },
            onLeave(value) {
                if (!this.observed) return;
                const record = {type: 'directx_com', phase: 'return', method: this.method,
                    eax: value.toUInt32()};
                if (this.output && value.toInt32() >= 0) {
                    const object = this.output.readPointer();
                    record.objectId = identify(object);
                    observeInterface(object, false);
                }
                emit(record);
            }
        }));
    }
}
function observeCreator(address, name) {
    if (hookedCreators.has(address.toString())) return;
    hookedCreators.add(address.toString());
    listeners.push(Interceptor.attach(address, {
        onEnter(args) {
            this.observed = inNative(this.returnAddress);
            if (!this.observed) return;
            this.output = args[1];
            const extended = name === 'DirectDrawCreateEx';
            const record = {type: 'directx_com', phase: 'entry', method: name,
                guidIsNull: args[0].isNull(), outerIsNull: args[extended ? 3 : 2].isNull()};
            if (extended) record.interfaceGuid = hexBytes(args[2], 16);
            emit(record);
        },
        onLeave(value) {
            if (!this.observed) return;
            const record = {type: 'directx_com', phase: 'return', method: name, eax: value.toUInt32()};
            if (value.toInt32() >= 0) {
                const object = this.output.readPointer();
                record.objectId = identify(object);
                observeInterface(object, name === 'DirectDrawCreate');
            }
            emit(record);
        }
    }));
}
function emit(event) {
    if (events.length >= 256) throw new Error('DirectX observation capacity exceeded');
    events.push(event);
    send(event);
}
function stringAt(address) {
    for (let length = 0; length < 256; ++length) {
        if (address.add(length).readU8() === 0) return address.readUtf8String(length);
    }
    throw new Error('unterminated API string');
}
function hexBytes(address, size) {
    return Array.from(new Uint8Array(address.readByteArray(size)),
        byte => byte.toString(16).padStart(2, '0')).join('');
}
rpc.exports = {
    hook(address, layout) {
        if (layout.moduleName.toLowerCase() !== 'igi.exe' || ptr(address).toUInt32() !== 0x498480)
            throw new Error('retail-only address binding');
        image = Process.getModuleByName(layout.moduleName);
        target = image.base.add(0x98480);
        const modules = {kernel32: Process.getModuleByName('kernel32.dll'),
                         ole32: Process.getModuleByName('ole32.dll')};
        const specs = [['GetVersionExA', 1], ['LoadLibraryA', 1], ['GetProcAddress', 2],
                       ['FreeLibrary', 1], ['OutputDebugStringA', 1], ['CoInitialize', 1],
                       ['CoCreateInstance', 5], ['CoUninitialize', 0]];
        for (const [name, words] of specs) {
            const module = name.startsWith('Co') ? modules.ole32 : modules.kernel32;
            listeners.push(Interceptor.attach(module.getExportByName(name), {
                onEnter(args) {
                    this.observed = inNative(this.returnAddress);
                    if (!this.observed) return;
                    const record = {type: 'directx_api', phase: 'entry', name,
                        callerRva: this.returnAddress.sub(image.base).toString()};
                    if (name === 'LoadLibraryA' || name === 'OutputDebugStringA')
                        record.text = stringAt(args[0]);
                    if (name === 'GetProcAddress') {
                        record.procedure = stringAt(args[1]);
                        this.procedure = record.procedure;
                    }
                    if (name === 'CoInitialize') record.reservedIsNull = args[0].isNull();
                    if (name === 'GetVersionExA') {
                        this.version = args[0];
                        record.structureSize = this.version.readU32();
                    }
                    if (name === 'CoCreateInstance') {
                        record.classGuid = hexBytes(args[0], 16);
                        record.outerIsNull = args[1].isNull();
                        record.context = args[2].toUInt32();
                        record.interfaceGuid = hexBytes(args[3], 16);
                        this.output = args[4];
                    }
                    emit(record);
                },
                onLeave(value) {
                    if (!this.observed) return;
                    const record = {type: 'directx_api', phase: 'return', name};
                    if (name === 'LoadLibraryA' || name === 'GetProcAddress')
                        record.nonNull = !value.isNull();
                    else if (name !== 'OutputDebugStringA' && name !== 'CoUninitialize')
                        record.eax = value.toUInt32();
                    if (this.version) record.version = {
                        major: this.version.add(4).readU32(), minor: this.version.add(8).readU32(),
                        build: this.version.add(12).readU32(), platform: this.version.add(16).readU32()};
                    if (this.output) {
                        const object = this.output.readPointer();
                        record.objectIsNonNull = !object.isNull();
                        if (value.toInt32() >= 0) {
                            record.objectId = identify(object);
                            observeInterface(object, false);
                        }
                    }
                    if (!value.isNull() && ['DirectDrawCreate', 'DirectDrawCreateEx'].includes(this.procedure))
                        observeCreator(value, this.procedure);
                    emit(record);
                }
            }));
        }
        listeners.push(Interceptor.attach(target, {
            onEnter(args) {
                if (owner !== null) throw new Error('unexpected recursive DirectX probe');
                owner = Process.getCurrentThreadId();
                this.output0 = args[0];
                this.output1 = args[1];
                this.callIndex = ++count;
                emit({type: 'observed', phase: 'entry', address, callIndex: count,
                    outputsAlias: args[0].equals(args[1]),
                    callerRva: this.returnAddress.sub(image.base).toString(),
                    scope: 'retail startup DirectX capability output and imported call observations'});
            },
            onLeave(value) {
                emit({type: 'observed', phase: 'return', address, callIndex: this.callIndex,
                    capability: this.output0.readU32(), platform: this.output1.readU32(),
                    eax: value.toUInt32()});
                completed++;
                owner = null;
            }
        }));
        return {installed: true, target: target.toString(), extent: 0x44a,
            scope: 'retail-only analysis; imports, ordered COM calls and typed descriptor observations'};
    },
    summary() { return {count, completed, events: events.length, verified: false,
        objectIdentities: objectIds.size, comMethodHooks: hookedMethods.size,
        missing: ['candidate capture/comparison', 'error branch coverage']}; },
    detach() { for (const listener of listeners) listener.detach(); listeners = []; }
};
