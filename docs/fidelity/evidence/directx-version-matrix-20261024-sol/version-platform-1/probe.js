globalThis.IGI_DIRECTX_FAILURE_MODE = "version-platform-1";
'use strict';
// Single-native diagnostic comparison probe of retail 0x00498480 and its candidate.
// The original entry, 0x44A-byte retail extent,
// two writable DWORD outputs and import argument layouts come from the retained
// Binary Ninja/Radare2 listing. Natural mode changes no API return or state.
// version-api-zero mode explicitly overrides GetVersionExA's return after its
// real call, solely to exercise the retail 0x4984B9 conditional failure path.
// version-major-3/4 and version-platform-1 explicitly override the two version
// fields tested by retail, after recording the actual API output. They do not
// represent actual runs on other Windows versions.
// COM slots and caller sites are established by the same listing. Pointer
// identities are local ordinals, not addresses compared across processes.
let listeners = [];
let events = [];
let owner = null;
let count = 0;
let completed = 0;
let target;
let image;
let nativeSize;
let isRetail;
let capabilityOutput = null;
let platformOutput = null;
let capabilityInitialized = false;
let versionObserved = false;
let nt4Observed = false;
let objectIds = new Map();
let issuedObjectIds = 0;
let hookedMethods = new Set();
let methodKinds = new Map();
let hookedCreators = new Set();
let moduleNames = new Map();
let ownedModuleReferences = {};
const failureMode = globalThis.IGI_DIRECTX_FAILURE_MODE || 'none';
if (!['none', 'version-api-zero', 'version-major-3', 'version-major-4',
      'version-platform-1'].includes(failureMode)) throw new Error('unknown failure mode');
let injectedFailures = 0;
let injectedVersions = 0;
const callMethods = new Map([
    [0x98652, 'QueryInterface'], [0x987c1, 'QueryInterface'], [0x987f6, 'QueryInterface'],
    [0x98742, 'SetCooperativeLevel'], [0x98781, 'CreateSurface'],
    ...[0x98660, 0x98687, 0x986af, 0x986e7, 0x98750, 0x9878f,
        0x987cf, 0x98804, 0x98822, 0x9882c, 0x98882, 0x988bc].map(site => [site, 'Release'])
]);
function inNative(caller) {
    return owner === Process.getCurrentThreadId() && caller.compare(target) >= 0 &&
        caller.compare(target.add(nativeSize)) < 0;
}
function identify(object) {
    const key = object.toString();
    if (!objectIds.has(key)) objectIds.set(key, ++issuedObjectIds);
    return objectIds.get(key);
}
function observeInterface(object, primary) {
    if (object.isNull()) throw new Error('successful COM call returned NULL');
    identify(object);
    const table = object.readPointer();
    for (const slot of (primary ? [0, 2, 6, 20] : [0, 2])) {
        const methodAddress = table.add(4 * slot).readPointer();
        const key = methodAddress.toString();
        const kind = ({0: 'QueryInterface', 2: 'Release', 6: 'CreateSurface', 20: 'SetCooperativeLevel'})[slot];
        if (!methodKinds.has(key)) methodKinds.set(key, new Set());
        methodKinds.get(key).add(kind);
        if (hookedMethods.has(key)) continue;
        hookedMethods.add(key);
        listeners.push(Interceptor.attach(methodAddress, {
            onEnter(args) {
                this.observed = inNative(this.returnAddress);
                if (!this.observed) return;
                const callerRva = this.returnAddress.sub(image.base).toUInt32();
                const kinds = methodKinds.get(key);
                this.method = isRetail ? callMethods.get(callerRva) :
                    (kinds.size === 1 ? Array.from(kinds)[0] : null);
                if (!this.method || !kinds.has(this.method))
                    throw new Error('unresolved or ambiguous COM caller ' + callerRva.toString(16));
                this.output = null;
                this.object = args[0];
                // The first QueryInterface follows the proven 0x498641 store of 0x100.
                if (this.method === 'QueryInterface') capabilityInitialized = true;
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
                if (this.method === 'Release' && value.toUInt32() === 0)
                    objectIds.delete(this.object.toString());
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
    if (event.type === 'directx_api' || event.type === 'directx_com') {
        event.outputState = {
            // Null means not observed as initialized yet, never a substituted value.
            capability: capabilityInitialized ? capabilityOutput.readU32() : null,
            platform: versionObserved ? platformOutput.readU32() : null
        };
    }
    events.push(event);
    event.sequence = events.length;
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
        isRetail = layout.moduleName.toLowerCase() === 'igi.exe';
        if (isRetail && ptr(address).toUInt32() !== 0x498480)
            throw new Error('retail address binding mismatch');
        image = Process.getModuleByName(layout.moduleName);
        nativeSize = layout.nativeSize;
        if (!Number.isInteger(nativeSize) || nativeSize < 1 || nativeSize > 65536 ||
            (isRetail && nativeSize !== 0x44a)) throw new Error('missing reviewed native extent');
        target = image.base.add(ptr(address).sub(ptr(layout.preferredBase)));
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
                    if (name === 'LoadLibraryA') this.libraryName = record.text;
                    if (name === 'GetProcAddress' || name === 'FreeLibrary') {
                        this.libraryName = args[0].isNull() ? 'NULL' : moduleNames.get(args[0].toString());
                        if (!this.libraryName) throw new Error('unresolved DLL handle identity');
                        record.library = this.libraryName;
                    }
                    // At this call site the NT4 branch has already stored 0x200.
                    if (name === 'LoadLibraryA' && nt4Observed) capabilityInitialized = true;
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
                    if (name === 'GetVersionExA' && failureMode === 'version-api-zero') {
                        record.injectedFailure = {api: name, originalEax: value.toUInt32(), effectiveEax: 0,
                            scope: 'real API executes; only its return value is overridden'};
                        value.replace(0);
                        injectedFailures++;
                    }
                    if (name === 'GetVersionExA' && !value.isNull() &&
                        ['version-major-3', 'version-major-4', 'version-platform-1'].includes(failureMode)) {
                        record.originalVersion = {
                            major: this.version.add(4).readU32(), minor: this.version.add(8).readU32(),
                            build: this.version.add(12).readU32(), platform: this.version.add(16).readU32()};
                        if (failureMode === 'version-platform-1') this.version.add(16).writeU32(1);
                        else {
                            this.version.add(16).writeU32(2);
                            this.version.add(4).writeU32(failureMode === 'version-major-3' ? 3 : 4);
                        }
                        record.injectedVersion = failureMode;
                        injectedVersions++;
                    }
                    if (name === 'LoadLibraryA' || name === 'GetProcAddress')
                        record.nonNull = !value.isNull();
                    else if (name !== 'OutputDebugStringA' && name !== 'CoUninitialize')
                        record.eax = value.toUInt32();
                    if (name === 'LoadLibraryA' && !value.isNull()) {
                        moduleNames.set(value.toString(), this.libraryName);
                        ownedModuleReferences[this.libraryName] = (ownedModuleReferences[this.libraryName] || 0) + 1;
                    }
                    if (name === 'FreeLibrary' && value.toUInt32() !== 0 && this.libraryName !== 'NULL')
                        ownedModuleReferences[this.libraryName] -= 1;
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
                    // The platform store occurs after GetVersionExA returns; observe it
                    // at the next native call, rather than treating the pre-store word as initialized.
                    if (this.version && !value.isNull()) {
                        versionObserved = true;
                        nt4Observed = record.version.platform === 2 && record.version.major === 4;
                    }
                }
            }));
        }
        listeners.push(Interceptor.attach(target, {
            onEnter(args) {
                if (owner !== null) throw new Error('unexpected recursive DirectX probe');
                owner = Process.getCurrentThreadId();
                this.output0 = args[0];
                this.output1 = args[1];
                capabilityOutput = args[0];
                platformOutput = args[1];
                capabilityInitialized = false;
                versionObserved = false;
                nt4Observed = false;
                this.callIndex = ++count;
                emit({type: 'observed', phase: 'entry', address, callIndex: count,
                    outputsAlias: args[0].equals(args[1]),
                    callerRva: this.returnAddress.sub(image.base).toString(),
                    scope: 'DirectX capability native: ordered imports, COM operations and output stores'});
            },
            onLeave(value) {
                emit({type: 'observed', phase: 'return', address, callIndex: this.callIndex,
                    capability: this.output0.readU32(), platform: this.output1.readU32(),
                    returnEqualsCapabilityPointer: value.equals(this.output0),
                    eax: value.toUInt32()});
                completed++;
                owner = null;
                send({type: 'directx_summary', nativeComplete: true, count, completed,
                    sequenceLength: events.length, issuedObjectIds, ownedModuleReferences,
                    failureMode, injectedFailures, injectedVersions,
                    scope: 'completed native only; later game startup may still fail'});
            }
        }));
        return {installed: true, target: target.toString(), extent: nativeSize,
            scope: 'single-native diagnostic comparison; full WinMain/game acceptance remains separate'};
    },
    summary() { return {count, completed, events: events.length, verified: false,
        objectIdentities: issuedObjectIds, liveObjectIdentities: objectIds.size,
        comMethodHooks: hookedMethods.size,
        missing: ['independent semantic approval', 'error branch coverage', 'production intake']}; },
    detach() { for (const listener of listeners) listener.detach(); listeners = []; }
};
