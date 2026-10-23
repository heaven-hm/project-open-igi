'use strict';
// Windows input-injection tooling: send the owner's requested Escape to this process only.
rpc.exports = {
  pressEscape() {
    const user32 = Process.getModuleByName('user32.dll');
    const enumerate = new NativeFunction(user32.getExportByName('EnumWindows'), 'int', ['pointer', 'pointer'], 'stdcall');
    const owner = new NativeFunction(user32.getExportByName('GetWindowThreadProcessId'), 'uint32', ['pointer', 'pointer'], 'stdcall');
    const visible = new NativeFunction(user32.getExportByName('IsWindowVisible'), 'int', ['pointer'], 'stdcall');
    const post = new NativeFunction(user32.getExportByName('PostMessageW'), 'int', ['pointer', 'uint32', 'uint32', 'pointer'], 'stdcall');
    const pid = Memory.alloc(4);
    let window = ptr(0);
    const callback = new NativeCallback(function(handle, _) {
      owner(handle, pid);
      if (pid.readU32() === Process.id && visible(handle)) { window = handle; return 0; }
      return 1;
    }, 'int', ['pointer', 'pointer'], 'stdcall');
    enumerate(callback, ptr(0));
    if (window.isNull()) return {sent: false, reason: 'owned visible window not ready'};
    return {sent: post(window, 0x100, 0x1b, ptr(0)) !== 0 && post(window, 0x101, 0x1b, ptr(0)) !== 0,
            processId: Process.id, window: window.toString()};
  }
};
