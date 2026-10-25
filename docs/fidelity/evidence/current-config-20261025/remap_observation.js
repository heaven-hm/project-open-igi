"use strict";
let listener, entries=0, returns=0;
rpc.exports.hook=function(address,layout){
 if(Process.pointerSize!==4)throw new Error("x86 required");
 const image=Process.getModuleByName(layout.moduleName);
 const at=va=>image.base.add(ptr(va).sub(ptr(layout.preferredBase)));
 const word=address=>{const range=Process.findRangeByAddress(address);if(!range||!range.protection.includes("r")||address.add(4).compare(range.base.add(range.size))>0)throw new Error("unreadable reviewed word");return address.readU32();};
 listener=Interceptor.attach(at(layout.entryVA),{
  onEnter(){
   this.index=++entries;this.count=word(at("0x00BC2380"));
   if(this.count<1||this.count>8)throw new Error("profile-count observation outside reviewed bound");
   this.counter=at("0x00BC2358").add(this.count*345*4);this.ordinal=word(this.counter);
   if(this.ordinal>=115)throw new Error("remap ordinal outside reviewed bound");
   this.entry=at("0x00BC25BC").add(((this.count-1)*115+this.ordinal)*12);
   this.before=[word(this.entry),word(this.entry.add(4)),word(this.entry.add(8))];
  },
  onLeave(){
   returns++;const after=[word(this.entry),word(this.entry.add(4)),word(this.entry.add(8))];
   const counterAfter=word(this.counter);if(counterAfter!==this.ordinal+1)throw new Error("unexpected remap counter transition");
   send({type:"observed",address,callIndex:this.index,profile_count_word:this.count,ordinal:this.ordinal,counter_after:counterAfter,entry_before:this.before,entry_after:after,scope:"retail remap table observation only; arguments and return block not inspected"});
  }
 });return {installed:true,address};
};
rpc.exports.summary=()=>({entries,returns,verified:false});
rpc.exports.detach=()=>{if(listener)listener.detach();};
