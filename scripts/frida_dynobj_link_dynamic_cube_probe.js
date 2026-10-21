'use strict';

rpc.exports = {
  pressEscape() {
    const user32 = Process.getModuleByName('user32.dll');
    const findWindow = new NativeFunction(user32.getExportByName('FindWindowW'),
                                          'pointer', ['pointer', 'pointer'], 'stdcall');
    const enumWindows = new NativeFunction(user32.getExportByName('EnumWindows'),
                                            'int', ['pointer', 'pointer'], 'stdcall');
    const getWindowText = new NativeFunction(user32.getExportByName('GetWindowTextW'),
                                             'int', ['pointer', 'pointer', 'int'], 'stdcall');
    const getWindowProcessId = new NativeFunction(user32.getExportByName('GetWindowThreadProcessId'),
                                                   'uint32', ['pointer', 'pointer'], 'stdcall');
    const postMessage = new NativeFunction(user32.getExportByName('PostMessageW'),
                                           'int', ['pointer', 'uint32', 'uint32', 'pointer'], 'stdcall');
    const title = Memory.allocUtf16String('IGI');
    const pidBuffer = Memory.alloc(4);
    const textBuffer = Memory.alloc(512);
    let window = ptr(0);
    const deadline = Date.now() + 20000;
    while (Date.now() < deadline && window.isNull()) {
      window = findWindow(ptr(0), title);
      if (window.isNull()) {
        const callback = new NativeCallback((candidate, _data) => {
          pidBuffer.writeU32(0);
          getWindowProcessId(candidate, pidBuffer);
          if (pidBuffer.readU32() !== Process.id) return 1;
          textBuffer.writeU16(0);
          getWindowText(candidate, textBuffer, 256);
          if (textBuffer.readUtf16String() === 'IGI') {
            window = candidate;
            return 0;
          }
          return 1;
        }, 'int', ['pointer', 'pointer'], 'stdcall');
        enumWindows(callback, ptr(0));
      }
      if (window.isNull()) Thread.sleep(0.2);
    }
    if (window.isNull()) return {sent: false, reason: 'IGI window not found after 20 seconds'};
    const down = postMessage(window, 0x0100, 0x1B, ptr(0));
    const up = postMessage(window, 0x0101, 0x1B, ptr(0));
    return {sent: down !== 0 && up !== 0, window: window.toString(),
            key: 'Escape', title: 'IGI', processId: Process.id};
  },

  probe(moduleName, functionVa, testCases) {
    const module = Process.findModuleByName(moduleName);
    if (module === null) throw new Error('game module not loaded: ' + moduleName);
    const preferredBase = ptr('0x00400000');
    const offset = ptr(functionVa).sub(preferredBase);
    if (offset.compare(ptr(module.size)) >= 0) {
      throw new Error('function RVA is outside game image: base=' + module.base +
                      ' size=0x' + module.size.toString(16) + ' RVA=' + offset);
    }
    const entry = module.base.add(offset);
    const native = new NativeFunction(entry, 'uint32',
      ['uint32', 'uint32', 'uint32', 'uint32', 'uint32', 'uint32'], 'mscdecl');
    const stackEffects = [];
    const listener = Interceptor.attach(entry, {
      onEnter() {
        this.argumentAddress = this.context.esp.add(4);
        this.argumentBefore = this.argumentAddress.readU32();
      },
      onLeave() {
        stackEffects.push({before: this.argumentBefore,
                           after: this.argumentAddress.readU32()});
      }
    });
    const results = [];
    for (const args of testCases) {
      if (!Array.isArray(args) || args.length !== 6) throw new Error('expected six scalar arguments');
      results.push({args, result: native(...args).toString()});
    }
    listener.detach();
    if (stackEffects.length !== results.length) throw new Error('incomplete argument-stack observations');
    for (let i = 0; i < results.length; i++) results[i].stackEffect = stackEffects[i];
    return {module: module.name, moduleBase: module.base.toString(), moduleSize: module.size,
            functionVa,
            entry: entry.toString(), calls: results};
  }
};
