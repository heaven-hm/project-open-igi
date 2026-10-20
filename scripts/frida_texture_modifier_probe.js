'use strict';
// 0x00505E40 ABI/layout: BN + RE-Agent radare2 ev_0041b4b3f7b953ab4744f3a74c261506c6a81a78b59bfd384961ac0184b2d01e.
let naturalCalls = 0;
function hex(address, size) {
  return Array.from(new Uint8Array(address.readByteArray(size))).map(x=>x.toString(16).padStart(2,'0')).join('');
}
rpc.exports = {
  run(target) {
    const module = Process.getModuleByName(target.module);
    function checked(value, size) {
      const address = ptr(value);
      if (address.compare(module.base)<0 || address.add(size).compare(module.base.add(module.size))>0)
        throw new Error('Address outside expected executable');
      return address;
    }
    const entry=checked(target.entry,1),count=checked(target.count,4),stack=checked(target.stack,80),mask=checked(target.mask,4);
    const regions=[[count,4],[stack,80],[mask,4]], saved=regions.map(([p,n])=>p.readByteArray(n));
    const context=Memory.alloc(512),table=Memory.alloc(0xe00),row=Memory.alloc(64);
    const invoke=new NativeFunction(entry,'pointer',['pointer'],'mscdecl'),cases=[];
    let restored=false;
    try {
      for(const fixture of target.fixtures) {
        if(![-2147483648,-1,0,1,2].includes(fixture.index) || ![0,3,4,19,20].includes(fixture.count) ||
           (fixture.index>=0 && fixture.enabled!==0 && fixture.count>=20))
          throw new Error('Fixture outside nonfatal reviewed scope');
        context.writeByteArray(new Uint8Array(512).fill(0xa5));
        table.writeByteArray(new Uint8Array(0xe00));row.writeByteArray(new Uint8Array(64));
        context.add(0x68).writePointer(fixture.index>=0?table:ptr(0));
        context.add(0xbc).writeS32(fixture.index);context.add(0xe4).writeU32(fixture.value);
        table.add(0xdcc).writePointer(row);
        if(fixture.index>=0)row.add(fixture.index*12).writeU32(fixture.enabled);
        count.writeS32(fixture.count);stack.writeByteArray(new Uint8Array(80).fill(0x5a));mask.writeU32(fixture.mask);
        const result=invoke(context);
        // Normalize the one pointer written by the fixture, retaining all native context writes.
        const originalTable=fixture.index>=0?table:ptr(0);
        if(!context.add(0x68).readPointer().equals(originalTable))
          throw new Error('Native corrupted its source-table pointer');
        context.add(0x68).writeU32(fixture.index>=0?1:0);
        cases.push({input:fixture,output:{return_value:{kind:result.equals(context)?'context':'integer',value:result.equals(context)?0:result.toUInt32()},
          context_hex:hex(context,512),count:count.readS32(),mask:mask.readU32(),stack_hex:hex(stack,80)}});
      }
    } finally {
      regions.forEach(([p,n],i)=>p.writeByteArray(saved[i]));
      restored=regions.every(([p,n],i)=>hex(p,n)===Array.from(new Uint8Array(saved[i])).map(x=>x.toString(16).padStart(2,'0')).join(''));
      if(!restored)throw new Error('Native globals were not restored');
    }
    Interceptor.attach(entry,{onEnter(){naturalCalls++;}});Interceptor.flush();
    return {cases,globals_restored:restored,scope:'Explicit valid-context agent-thread fixtures; no natural integration claim'};
  },
  snapshot(){return {natural_calls:naturalCalls};}
};
