// Paired probe for the MEF bone rig natives.
//
// The same probe runs against igi.exe and open-igi.exe. On the retail side it
// hooks the retail virtual addresses; on the candidate side it hooks the
// candidate symbols from the newly built x86 binary. Both sides report the
// ordered call sequence with the chunk tag that was searched for and whether
// the search found a chunk.
//
// The probe never dereferences a returned pointer across the two processes:
// pointer values differ between address spaces, so the comparison uses the
// searched tag and the found flag.

'use strict';

const SCHEME = JSON.parse(Process.argv[2] || '{}');
const TARGETS = SCHEME.targets || [];
const IMAGE_BASE = SCHEME.imageBase || 0x400000;
let escape = { requested: SCHEME.mode === 'retail', posted: false, attempts: 0 };

function hex(value) {
    return '0x' + (value >>> 0).toString(16).padStart(8, '0');
}

const observed = [];
const hooks = [];

function processLaunchContext() {
    const kernel = Process.findModuleByName('kernel32.dll');
    if (kernel === null) {
        return { commandLine: null, currentDirectory: null };
    }
    const getCommandLineAddress = kernel.findExportByName('GetCommandLineW');
    const getCurrentDirectoryAddress = kernel.findExportByName('GetCurrentDirectoryW');
    let commandLine = null;
    let currentDirectory = null;
    if (getCommandLineAddress !== null) {
        const getCommandLine = new NativeFunction(getCommandLineAddress, 'pointer', [], 'stdcall');
        const value = getCommandLine();
        if (!value.isNull()) commandLine = value.readUtf16String();
    }
    if (getCurrentDirectoryAddress !== null) {
        const getCurrentDirectory = new NativeFunction(
            getCurrentDirectoryAddress, 'uint32', ['uint32', 'pointer'], 'stdcall');
        const buffer = Memory.alloc(32768);
        const length = getCurrentDirectory(32768, buffer);
        if (length > 0 && length < 32768) currentDirectory = buffer.readUtf16String(length);
    }
    return { commandLine: commandLine, currentDirectory: currentDirectory };
}

const launchContext = processLaunchContext();

function findTargetModule(name) {
    const lowerName = name.toLowerCase();
    const exact = Process.findModuleByName(name);
    if (exact !== null) {
        return exact;
    }
    return Process.enumerateModules().find(function (module) {
        return module.name.toLowerCase() === lowerName ||
            module.path.toLowerCase().endsWith('\\' + lowerName) ||
            module.path.toLowerCase().endsWith('/' + lowerName);
    }) || null;
}

for (const target of TARGETS) {
    const module = findTargetModule(target.module);
    if (module === null) {
        hooks.push({ target: target.name, installed: false, reason: 'module not found', requestedModule: target.module });
        continue;
    }
    const runtime = module.base.add(target.vaddr - IMAGE_BASE);
    let address = runtime;
    if (target.symbol) {
        const exported = module.findExportByName(target.symbol);
        if (exported !== null) {
            address = exported;
        }
    }
    try {
        Interceptor.attach(address, {
            onEnter(args) {
                // Frida's args is array-like and has no length property.
                const tag = args[1];
                this.record = {
                    target: target.name,
                    tag: tag !== undefined ? hex(tag.toUInt32()) : null,
                    found: false,
                };
            },
            onLeave(retval) {
                this.record.found = !retval.isNull();
                observed.push(this.record);
            },
        });
        hooks.push({ target: target.name, installed: true, module: module.name, path: module.path, address: address.toString() });
    } catch (error) {
        hooks.push({ target: target.name, installed: false, module: module.name, path: module.path, address: address.toString(), reason: String(error) });
        send({ error: 'attach failed for ' + target.name + ': ' + error });
    }
}
send({ type: 'hook-installation', hooks: hooks });

function postEscapeToGameWindow() {
    const user32 = Process.findModuleByName('user32.dll');
    if (user32 === null) {
        return { posted: false, reason: 'user32.dll is not loaded' };
    }
    const enumWindowsAddress = user32.findExportByName('EnumWindows');
    const processIdAddress = user32.findExportByName('GetWindowThreadProcessId');
    const isVisibleAddress = user32.findExportByName('IsWindowVisible');
    const postMessageAddress = user32.findExportByName('PostMessageA');
    if ([enumWindowsAddress, processIdAddress, isVisibleAddress, postMessageAddress].some(x => x === null)) {
        return { posted: false, reason: 'required User32 export is unavailable' };
    }

    const enumWindows = new NativeFunction(enumWindowsAddress, 'int', ['pointer', 'pointer'], 'stdcall');
    const getWindowProcessId = new NativeFunction(processIdAddress, 'uint32', ['pointer', 'pointer'], 'stdcall');
    const isWindowVisible = new NativeFunction(isVisibleAddress, 'int', ['pointer'], 'stdcall');
    const postMessage = new NativeFunction(postMessageAddress, 'int', ['pointer', 'uint32', 'uint32', 'uint32'], 'stdcall');
    let window = ptr(0);
    const processId = Process.id;
    const callback = new NativeCallback(function (candidate, _parameter) {
        const owner = Memory.alloc(4);
        getWindowProcessId(candidate, owner);
        if (owner.readU32() === processId && isWindowVisible(candidate) !== 0) {
            window = candidate;
            return 0;
        }
        return 1;
    }, 'int', ['pointer', 'pointer'], 'stdcall');
    enumWindows(callback, ptr(0));
    if (window.isNull()) {
        return { posted: false, processId: processId, reason: 'no visible game window found' };
    }

    const keyDown = postMessage(window, 0x0100, 0x1b, 0x00010001);
    const keyUp = postMessage(window, 0x0101, 0x1b, 0xc0010001);
    return {
        posted: keyDown !== 0 && keyUp !== 0,
        processId: processId,
        window: window.toString(),
        keyDown: keyDown !== 0,
        keyUp: keyUp !== 0,
        key: 'Escape',
    };
}

function attemptRetailEscape() {
    if (!escape.requested || escape.posted || escape.attempts >= 10) {
        return;
    }
    const attempts = escape.attempts + 1;
    const result = postEscapeToGameWindow();
    // Replace the last attempt's result instead of merging it: after an
    // earlier window-enumeration miss, a successful post must not retain that
    // stale `reason` alongside `posted: true`.
    escape = Object.assign({ requested: true, attempts: attempts }, result);
    if (!escape.posted) {
        setTimeout(attemptRetailEscape, 1000);
    }
}

if (escape.requested) {
    setTimeout(attemptRetailEscape, 2000);
}

rpc.exports = {
    stop() {
        return { observed: observed, escape: escape, hooks: hooks, launchContext: launchContext };
    },
};
