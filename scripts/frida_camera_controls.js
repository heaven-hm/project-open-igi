'use strict';
let latest = null, frames = 0;
const user = Process.getModuleByName('user32.dll');
const api = (name, result, args) => new NativeFunction(user.getExportByName(name), result, args, 'stdcall');
function ownedWindow() {
  const matches = [], owner = Memory.alloc(4), title = Memory.alloc(512);
  const callback = new NativeCallback(window => {
    api('GetWindowThreadProcessId', 'uint32', ['pointer','pointer'])(window, owner);
    api('GetWindowTextW', 'int', ['pointer','pointer','int'])(window, title, 256);
    if (owner.readU32() === Process.id && title.readUtf16String() === 'IGI') matches.push(window);
    return 1;
  }, 'int', ['pointer','pointer'], 'stdcall');
  api('EnumWindows', 'int', ['pointer','pointer'])(callback, ptr(0));
  if (matches.length !== 1) throw new Error('Expected exactly one owned IGI window');
  return matches[0];
}
rpc.exports = {
  start(entry) {
    Interceptor.attach(ptr(entry), {
      onEnter(args) {
        const matrix = [], position = [];
        for (let i=0;i<9;i++) matrix.push(args[1].add(i*4).readFloat());
        for (let i=0;i<3;i++) position.push(args[2].add(i*8).readDouble());
        latest = {matrix, position}; frames++;
      }
    });
    Interceptor.flush();
  },
  snapshot() { return {frames, latest}; },
  key(code, repeated) {
    if (![0x78,0x79,0x7b].includes(code)) throw new Error('Unsupported developer test key');
    const window = ownedWindow();
    const post = api('PostMessageW', 'int', ['pointer','uint32','uint32','uint32']);
    if (!post(window, 0x100, code, repeated ? 0x40000001 : 1)) throw new Error('Key down failed');
    if (!post(window, 0x101, code, 0xc0000001)) throw new Error('Key up failed');
  },
  mouse(dx, dy) {
    const window=ownedWindow();
    api('SetForegroundWindow','int',['pointer'])(window);
    if (!api('GetForegroundWindow','pointer',[])().equals(window)) throw new Error('IGI is not foreground');
    api('mouse_event','void',['uint32','uint32','uint32','uint32','pointer'])(1, dx>>>0, dy>>>0, 0, ptr(0));
  },
  capture() {
    const window=ownedWindow(), rectangle=Memory.alloc(16);
    if (!api('GetClientRect','int',['pointer','pointer'])(window,rectangle)) throw new Error('GetClientRect failed');
    const width=rectangle.add(8).readS32(), height=rectangle.add(12).readS32();
    if (width<=0 || height<=0 || width>8192 || height>8192) throw new Error('Invalid capture dimensions');
    const gdi=Process.getModuleByName('gdi32.dll');
    const g=(name,result,args)=>new NativeFunction(gdi.getExportByName(name),result,args,'stdcall');
    const dc=api('GetDC','pointer',['pointer'])(window);
    let memory=ptr(0),bitmap=ptr(0),previous=ptr(0);
    try {
      memory=g('CreateCompatibleDC','pointer',['pointer'])(dc);
      const info=Memory.alloc(40),bits=Memory.alloc(Process.pointerSize);
      info.writeByteArray(new Uint8Array(40)); info.writeU32(40);
      info.add(4).writeS32(width); info.add(8).writeS32(-height);
      info.add(12).writeU16(1); info.add(14).writeU16(32);
      bitmap=g('CreateDIBSection','pointer',['pointer','pointer','uint32','pointer','pointer','uint32'])(dc,info,0,bits,ptr(0),0);
      if (memory.isNull() || bitmap.isNull()) throw new Error('Capture allocation failed');
      previous=g('SelectObject','pointer',['pointer','pointer'])(memory,bitmap);
      if (!g('BitBlt','int',['pointer','int','int','int','int','pointer','int','int','uint32'])(memory,0,0,width,height,dc,0,0,0x00cc0020)) throw new Error('BitBlt failed');
      send({kind:'frame',width,height},bits.readPointer().readByteArray(width*height*4));
      return {width,height};
    } finally {
      if (!previous.isNull()) g('SelectObject','pointer',['pointer','pointer'])(memory,previous);
      if (!bitmap.isNull()) g('DeleteObject','int',['pointer'])(bitmap);
      if (!memory.isNull()) g('DeleteDC','int',['pointer'])(memory);
      api('ReleaseDC','int',['pointer','pointer'])(window,dc);
    }
  }
};
