// Task-4 investigation with explicit OS test input and optional pure scalar calls.
// Layout evidence: retail 0x00506F00 and
// 0x004C6940 disassembly, Binary Ninja, and RE-Agent Radare2 (2026-10-20).
// These observations do not establish model/material/texture identity or
// complete native parity. Configure addresses separately for each executable.
'use strict';
let configured = false;
const heightmapCalls = [];
const objectPositions = new Map();
const levelTaskIds = new Map();
const rigidObjects = new Set();
let modelIdentity = null;
const fpuControlWords = new Map();
const counts = {};
const failures = [];
let overflow = false;
let escapeObservedFrames = 0;
let breakCutscenePressedReturns = 0;
let pendingEscapeRelease = null;
let executableImage = null;

function bytesHex(address, length) {
  return Array.from(new Uint8Array(address.readByteArray(length)))
    .map(value => value.toString(16).padStart(2, '0')).join('');
}

rpc.exports = {
  runLightmapCases(target) {
    if (!configured) throw new Error('Configure the executable first');
    const checked = (address, size) => {
      const value = ptr(address);
      if (value.compare(executableImage.base) < 0 ||
          value.add(size).compare(executableImage.base.add(executableImage.size)) > 0)
        throw new Error('Lightmap fixture address is outside the executable');
      return value;
    };
    const entry = checked(target.entry, 1), modeEntry = checked(target.modeEntry, 1);
    const count = checked(target.count, 4), stack = checked(target.stack, 80);
    const bd = checked(target.bd, 1), be = checked(target.be, 1);
    const regions = [[count, 4], [stack, 80], [bd, 1], [be, 1]];
    const saved = regions.map(([address, size]) => address.readByteArray(size));
    const invoke = new NativeFunction(entry, 'pointer', ['pointer'], 'mscdecl');
    const mode = new NativeFunction(modeEntry, 'uint32', [], 'mscdecl');
    const context = Memory.alloc(384);
    const cases = [];
    try {
      for (const fixture of target.fixtures) {
        if (![0, 19].includes(fixture.count) || ![0, 1, -1].includes(fixture.enabled) ||
            !Number.isInteger(fixture.bd) || fixture.bd < 0 || fixture.bd > 255 ||
            !Number.isInteger(fixture.be) || fixture.be < 0 || fixture.be > 255)
          throw new Error('Lightmap fixture is outside the reviewed scope');
        context.writeByteArray(new Uint8Array(384).fill(0xa5));
        context.add(0x108).writeS32(fixture.enabled);
        context.add(0xd8).writeU32(fixture.value);
        stack.writeByteArray(new Uint8Array(80).fill(0x5a));
        count.writeU32(fixture.count);
        bd.writeU8(fixture.bd); be.writeU8(fixture.be);
        const modeResult = mode();
        const result = invoke(context);
        cases.push({input: fixture, output: {
          mode_result: modeResult,
          return_value: {kind: result.equals(context) ? 'context' : 'integer',
                         value: result.equals(context) ? 0 : result.toUInt32()},
          context_hex: bytesHex(context, 384), count: count.readU32(),
          stack_hex: bytesHex(stack, 80), bd: bd.readU8(), be: be.readU8()}});
      }
    } finally {
      regions.forEach(([address], index) => address.writeByteArray(saved[index]));
    }
    const restored = regions.every(([address, size], index) =>
      bytesHex(address, size) === Array.from(new Uint8Array(saved[index]))
        .map(value => value.toString(16).padStart(2, '0')).join(''));
    if (!restored) throw new Error('Lightmap fixture global cleanup failed');
    return {cases, globals_restored: restored,
      scope: 'Explicit fixtures before main-thread resume; natural terrain calls and stack overflow are unverified'};
  },
  runPureScalarCases(targets) {
    if (!configured) throw new Error('Configure the executable identity first');
    // Only the two reviewed, side-effect-free x87 flag conversion ABIs.
    // These are explicit fixture calls on the agent thread, not natural game calls.
    return targets.map(target => {
      if (!['0x004AC66B', '0x004AC6FD'].includes(target.retailAddress))
        throw new Error('Scalar fixture target is outside the reviewed scope');
      const entry = ptr(target.entry);
      if (entry.compare(executableImage.base) < 0 ||
          entry.compare(executableImage.base.add(executableImage.size)) >= 0)
        throw new Error('Scalar fixture entry is outside the executable');
      const argumentType = target.retailAddress === '0x004AC66B' ? 'uint16' : 'uint32';
      const invoke = new NativeFunction(entry, 'uint32', [argumentType], 'mscdecl');
      return {retailAddress: target.retailAddress, entry: target.entry,
        cases: target.inputs.map(input => ({input, output: invoke(input)})),
        scope: 'Explicit pure scalar calls on the Frida agent thread; game call ordering and FPU state are not tested'};
    });
  },
  releaseEscape() {
    if (pendingEscapeRelease) pendingEscapeRelease();
    return {released: pendingEscapeRelease === null};
  },
  skipCutscene() {
    // Explicit user-requested test input, not a reconstructed gameplay method.
    const user32 = Process.getModuleByName('user32.dll');
    const api = (name, result, args) => new NativeFunction(user32.getExportByName(name), result, args, 'stdcall');
    const getOwner = api('GetWindowThreadProcessId', 'uint32', ['pointer', 'pointer']);
    const getTitle = api('GetWindowTextW', 'int', ['pointer', 'pointer', 'int']);
    const owner = Memory.alloc(4), title = Memory.alloc(512);
    const windows = [];
    const visit = new NativeCallback((window, unused) => {
      getOwner(window, owner);
      if (owner.readU32() !== Process.id) return 1;
      getTitle(window, title, 256);
      const caption = title.readUtf16String();
      if (caption === 'IGI') windows.push(window);
      return 1;
    }, 'int', ['pointer', 'pointer'], 'stdcall');
    api('EnumWindows', 'int', ['pointer', 'pointer'])(visit, ptr(0));
    if (windows.length !== 1) return {delivered: false, matchingWindows: windows.length};
    api('SetForegroundWindow', 'int', ['pointer'])(windows[0]);
    const foreground = api('GetForegroundWindow', 'pointer', [])();
    getOwner(foreground, owner);
    if (owner.readU32() !== Process.id) return {delivered: false, reason: 'Owned game is not foreground'};
    // Retail Keyboard_Update polls DirectInput. Window messages alone do not
    // establish a pressed key. Submit OS input only while this IGI is foreground.
    const keyEvent = api('keybd_event', 'void', ['uint8', 'uint8', 'uint32', 'pointer']);
    return new Promise(resolve => {
      pendingEscapeRelease = () => {
        keyEvent(0x1b, 1, 2, ptr(0));
        pendingEscapeRelease = null;
      };
      keyEvent(0x1b, 1, 0, ptr(0));
      setTimeout(() => {
        if (pendingEscapeRelease) pendingEscapeRelease();
        resolve({delivered: true, released: true, window: windows[0].toString(), title: 'IGI',
          method: 'OS Escape down/up; native consumption and cutscene transition require observation'});
      }, 150);
    });
  },
  start(config) {
    if (configured) throw new Error('Probe already configured');
    const image = Process.enumerateModules().find(module => module.name.toLowerCase() === config.module.toLowerCase());
    if (!image) throw new Error('Expected executable module is not mapped: ' + config.module);
    if (!image.base.equals(ptr('0x00400000'))) throw new Error('Unexpected executable base; absolute VAs are invalid');
    executableImage = image;
    configured = true;
    if (config.fpuInitialization) {
      Interceptor.attach(ptr(config.fpuInitialization.entry), {
        onLeave() {
          counts.Display_ClearSurfaces_16 = (counts.Display_ClearSurfaces_16 || 0) + 1;
          const word = ptr(config.fpuInitialization.controlWord).readU16();
          if (fpuControlWords.has(word)) fpuControlWords.set(word, fpuControlWords.get(word) + 1);
          else if (fpuControlWords.size < 64) fpuControlWords.set(word, 1);
          else overflow = true;
        }
      });
    }
    if (config.rigidModelIdentity) {
      modelIdentity = config.rigidModelIdentity;
      Interceptor.attach(ptr(modelIdentity.constructor), {
        onEnter(args) { rigidObjects.add(args[0].toString()); }
      });
    }
    if (config.taskRegistration) {
      Interceptor.attach(ptr(config.taskRegistration), {
        onEnter(args) { this.levelTaskId = args[0].toInt32(); this.levelTask = args[1].toString(); },
        onLeave() {
          counts.Script_LevelqtaskidDAlready = (counts.Script_LevelqtaskidDAlready || 0) + 1;
          levelTaskIds.set(this.levelTask, this.levelTaskId);
        }
      });
    }
    if (config.escapeObservation) {
      const key = ptr(config.escapeObservation.keyState);
      Interceptor.attach(ptr(config.escapeObservation.keyboardUpdate), {
        onLeave() {
          counts.Keyboard_Update = (counts.Keyboard_Update || 0) + 1;
          if ((key.readU8() & 0x80) !== 0) escapeObservedFrames++;
        }
      });
      Interceptor.attach(ptr(config.escapeObservation.breakCutscene), {
        onEnter(args) { this.outputContext = args[0]; },
        onLeave() {
          counts.LevelFlow_GetBreakCutSceneKey = (counts.LevelFlow_GetBreakCutSceneKey || 0) + 1;
          if (this.outputContext.readU32() === 0x80) breakCutscenePressedReturns++;
        }
      });
    }
    if (config.heightmap) {
      const count = ptr(config.heightmap.count);
      Interceptor.attach(ptr(config.heightmap.entry), {
        onEnter(args) {
          this.nativeContext = args[0];
          try {
            const index = this.nativeContext.add(0xbc).readS32();
            let sourceEnabled = null;
            if (index >= 0) {
              const table = this.nativeContext.add(0x68).readPointer();
              const row = table.add(0xdd0).readPointer();
              sourceEnabled = row.add(index * 12).readU32() !== 0;
            }
            this.inputs = {
              index, sourceEnabled,
              storedIndex: this.nativeContext.add(0xf8).readU32(),
              flagBefore: this.nativeContext.add(0xe8).readU32(),
              stackCountBefore: count.readS32()
            };
          } catch (error) { failures.push({native: 'HeightMap_Enter', phase: 'entry', error: String(error)}); }
        },
        onLeave(retval) {
          counts.HeightMap_Enter = (counts.HeightMap_Enter || 0) + 1;
          if (!this.inputs) return;
          if (heightmapCalls.length >= 4096) { overflow = true; return; }
          try {
            heightmapCalls.push({
              context: this.nativeContext.toString(),
              typedState: {
                scope: 'HeightMap_Enter entry condition and DWORD active flag; stack contents/global active index excluded',
                inputs: this.inputs,
                outputs: {returnKind: retval.equals(this.nativeContext) ? 'context' : 'integer',
                          returnValue: retval.equals(this.nativeContext) ? 0 : retval.toUInt32()},
                sideEffects: {flagAfter: this.nativeContext.add(0xe8).readU32(), stackCountAfter: count.readS32()}
              }
            });
          } catch (error) { failures.push({native: 'HeightMap_Enter', phase: 'return', error: String(error)}); }
        }
      });
    }
    if (config.worldObjectCommit) {
      Interceptor.attach(ptr(config.worldObjectCommit), {
        onEnter(args) { this.object = args[1]; },
        onLeave() {
          counts.WorldObject_CommitTransform = (counts.WorldObject_CommitTransform || 0) + 1;
          try {
            const key = this.object.toString();
            if (!objectPositions.has(key) && objectPositions.size >= 10000) { overflow = true; return; }
            let modelInstanceName = null;
            if (modelIdentity && rigidObjects.has(key)) {
              const model = this.object.add(0x6c).readPointer();
              const start = ptr(modelIdentity.virtualModels);
              const count = ptr(modelIdentity.virtualModelCount).readU32();
              const offset = model.sub(start).toUInt32();
              // Retail DynObj_MtpNlodvirmodelD allocates 0x48-byte records,
              // copies the name at +0x14 and limits the table to 0x258 records.
              if (count > 0x258) throw new Error('Virtual model count exceeds retail table');
              if (!model.isNull() && model.compare(start) >= 0 && offset % 0x48 === 0 && offset / 0x48 < count) {
                const nameBytes = new Uint8Array(model.add(0x14).readByteArray(0x48 - 0x14));
                const terminator = nameBytes.indexOf(0);
                if (terminator < 0) throw new Error('Virtual model name has no bounded terminator');
                modelInstanceName = model.add(0x14).readUtf8String(terminator);
              }
            }
            objectPositions.set(key, {
              runtimeObject: key,
              levelTaskId: levelTaskIds.has(key) ? levelTaskIds.get(key) : null,
              modelInstanceName,
              orientationFloat32Bits: rigidObjects.has(key) ? bytesHex(this.object.add(0x70), 36) : null,
              scaleFloat32Bits: rigidObjects.has(key) ? bytesHex(this.object.add(0x98), 12) : null,
              positionFloat64Bits: bytesHex(this.object.add(0x20), 24),
              radiusFloat32Bits: bytesHex(this.object.add(0x44), 4),
              identityStatus: levelTaskIds.has(key) ? 'registered level task ID; model/material/texture mapping unresolved' :
                'opaque runtime address; model/task/material/texture mapping unresolved'
            });
          } catch (error) { failures.push({native: 'WorldObject_CommitTransform', error: String(error)}); }
        }
      });
    }
    for (const target of config.entries || []) {
      Interceptor.attach(ptr(target.address), {onEnter() { counts[target.name] = (counts[target.name] || 0) + 1; }});
    }
    Interceptor.flush();
    return {module: image.name, base: image.base.toString(), configured: true,
            heightmapPrefix: config.heightmap ? bytesHex(ptr(config.heightmap.entry), 8) : null};
  },
  snapshot() {
    return {counts, heightmapCalls, objectPositions: Array.from(objectPositions.values()), failures, overflow,
      escapeObservedFrames, breakCutscenePressedReturns,
      fpuControlWords: Array.from(fpuControlWords, ([word, count]) => ({word, count})),
      acceptance: 'inconclusive; placement identity, materials, textures and complete heightmap effects remain unverified'};
  }
};
