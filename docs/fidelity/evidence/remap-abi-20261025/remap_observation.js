"use strict";
let listeners=[],entries=0,returns=0;
const active=new Map();
rpc.exports.hook=function(address,layout){
 if(Process.pointerSize!==4)throw new Error("x86 required");
 const image=Process.getModuleByName(layout.moduleName);
 const at=va=>image.base.add(ptr(va).sub(ptr(layout.preferredBase)));
 const word=p=>{const r=Process.findRangeByAddress(p);if(!r||!r.protection.includes("r")||p.add(4).compare(r.base.add(r.size))>0)throw new Error("unreadable reviewed word");return p.readU32();};
 const words=(p,n)=>Array.from({length:n},(_,i)=>word(p.add(i*4)));
 // Retail 4037CA reads entry ESP+12; 40382E reads entry ESP+4.
 listeners.push(Interceptor.attach(at(layout.entryVA),{
  onEnter(args){
   if(active.has(this.threadId))throw new Error("unexpected recursive remap");
   this.index=++entries;this.output=args[0];this.frame=args[2];this.reads=[];
   this.outputBefore=words(this.output,6);
   this.count=word(at("0x00BC2380"));
   if(this.count<1||this.count>8)throw new Error("profile count outside observed bounds");
   this.counter=at("0x00BC2358").add(this.count*345*4);this.ordinal=word(this.counter);
   if(this.ordinal>=115)throw new Error("remap ordinal outside reviewed bounds");
   this.entry=at("0x00BC25BC").add(((this.count-1)*115+this.ordinal)*12);
   this.before=words(this.entry,3);active.set(this.threadId,this);
  },
  onLeave(result){
   active.delete(this.threadId);returns++;
   const after=words(this.entry,3),output=words(this.output,6);
   const checks={return_aliases_arg1:result.equals(this.output),reader_indices:JSON.stringify(this.reads.map(x=>x.index))==="[0,1,2]",reader_frame_aliases_arg3:this.reads.length===3&&this.reads.every(x=>x.frame_alias),counter_increment:word(this.counter)===this.ordinal+1,defined_output_words:output[0]===0&&output[2]===0&&output[3]===0&&output[4]===at("0x00567C74").toUInt32()};
   checks.table_matches_reader_returns=this.reads.length===3&&after[0]===this.reads[0].value&&after[1]===this.reads[1].value&&after[2]===(this.reads[2].value&0x3fffffff);
   send({type:"observed",address,callIndex:this.index,profile_count_word:this.count,ordinal:this.ordinal,reader_calls:this.reads,entry_before:this.before,entry_after:after,output_before:this.outputBefore,output_after:output,checks,scope:"retail ABI observations only; output words 1 and 5 are copied stack residue, not specified constants"});
  }
 }));
 listeners.push(Interceptor.attach(at("0x004B8A20"),{
  onEnter(args){const parent=active.get(this.threadId);if(!parent)return;this.parent=parent;this.read={index:args[1].toUInt32(),frame_alias:args[0].equals(parent.frame)};},
  onLeave(result){if(this.parent){this.read.value=result.toUInt32();this.parent.reads.push(this.read);}}
 }));
 return {installed:true,address};
};
rpc.exports.summary=()=>({entries,returns,verified:false});
rpc.exports.detach=()=>{listeners.forEach(x=>x.detach());listeners=[];};
