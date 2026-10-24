'use strict';
// Retail WinMain's first game-native call, 0x48F5F4 -> 0x48F0C0.
// Observe the real call; no replacement, NativeFunction replay or early resume.
let listener;
let entries = 0;
let returns = 0;
let state;
let image;
function bytes(address, length) {
    const range = Process.findRangeByAddress(address);
    if (!range || !range.protection.includes('r') ||
        address.add(length).compare(range.base.add(range.size)) > 0)
        throw new Error('typed snapshot range is not readable');
    return Array.from(new Uint8Array(address.readByteArray(length)),
                      b => b.toString(16).padStart(2,'0')).join('');
}
function text(pointer) {
    if (pointer.isNull()) return null;
    let result = '';
    for (let i=0; i<4096; ++i) {
        const byte = bytes(pointer.add(i),1);
        result += byte;
        if (byte === '00') return result;
    }
    throw new Error('command-line string exceeds explicit capture capacity');
}
rpc.exports = {
    hook(address, layout) {
        if (layout.evidenceInput || layout.watch.length !== 1)
            throw new Error('natural call capture needs exactly one reviewed global mapping');
        image = Process.getModuleByName(layout.moduleName);
        const relocate = value => image.base.add(ptr(value).sub(ptr(layout.preferredBase)));
        const target = relocate(address);
        state = relocate(layout.watch[0].address);
        const code = bytes(target,10);
        if (!code.startsWith('8b442404a3') || !code.endsWith('c3') ||
            !target.add(5).readPointer().equals(state))
            throw new Error('compiled/retail load-store-return body or global mapping differs');
        listener = Interceptor.attach(target, {
            onEnter(args) {
                this.index = ++entries;
                this.input = args[0];
                send({type:'observed',phase:'entry',address,callIndex:this.index,
                    argumentWord:args[0].toUInt32(),inputHex:text(args[0]),
                    globalBefore:state.readU32(),neighborsBefore:bytes(state.sub(4),12),
                    callerRva:this.returnAddress.sub(image.base).toUInt32()});
            },
            onLeave(value) {
                send({type:'observed',phase:'return',address,callIndex:this.index,
                    returnWord:value.toUInt32(),globalAfter:state.readU32(),
                    inputHexAfter:text(this.input),neighborsAfter:bytes(state.sub(4),12)});
                returns++;
                send({type:'store_command_line_complete',entries,returns,
                      scope:'natural startup calls to 0x0048F0C0 only',codeHex:code,
                      stateAddress:state.toString(),targetAddress:target.toString()});
            }
        });
        return {installed:true,target:target.toString(),state:state.toString(),codeHex:code,
                mode:'natural-pre-resume-hook'};
    },
    summary() { return {entries,returns,verified:false,scope:'single native; full WinMain remains separate'}; },
    detach() { if (listener) listener.detach(); }
};
