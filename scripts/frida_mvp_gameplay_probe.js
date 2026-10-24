// IGI retail MVP gameplay probe.
//
// Small, focused probe over the natives that drive the MVP demo loop:
// player movement, jumping and physics, collision, input keys, frame loading
// and the camera. The ABI is intentionally unknown here: record raw argument
// words and EAX only; do not dereference.
'use strict';
const targets = [
  { "name": "Human_RunUpdate",              "va": "0x004109A0" },
  { "name": "Human_Update",                "va": "0x0045F800" },
  { "name": "Human_MovementIntegrator",    "va": "0x004614B0" },
  { "name": "Human_AttachedMovement",      "va": "0x00462CF0" },
  { "name": "Human_CameraEyePosition",     "va": "0x00465820" },
  { "name": "Human_IsCrouched",            "va": "0x00463180" },
  { "name": "Human_TaskViewReset",         "va": "0x004659E0" },
  { "name": "Input_Run",                   "va": "0x00507EC0" },
  { "name": "Input_InitKeyboard",          "va": "0x0048FED0" },
  { "name": "Collision_Reset",             "va": "0x00489F00" },
  { "name": "Camera_FirstPerson_Update",   "va": "0x00482A40" },
  { "name": "Camera_ThirdPerson_Update",   "va": "0x00482C30" },
  { "name": "QTask_OpenEvent",             "va": "0x00401050" },
  { "name": "Game_GetContext",             "va": "0x00402880" },
  { "name": "Flow_SetState",               "va": "0x004028A0" },
  { "name": "Terrain_Load",                "va": "0x004C6240" },
  { "name": "Terrain_LoadTexturesAndLightmaps", "va": "0x004C5F00" },
  { "name": "Terrain_Draw",                "va": "0x00512C30" },
  { "name": "Texture_UploadMipmapPacket",  "va": "0x004B23B0" },
  { "name": "DynObj_LoadModelMesh",        "va": "0x004CF390" },
  { "name": "Model_GetAttachment",         "va": "0x004D0CF0" },
  { "name": "Model_GetAttachmentCount",    "va": "0x004D0D10" }
];
const imageBase = ptr('0x400000');
let targetModule = null;
for (const m of Process.enumerateModules()) {
  const wanted = 'igi.exe';
  if (m.name.toLowerCase() === wanted) { targetModule = m; break; }
}
if (targetModule === null) {
  send({type: 'probe_error', error: 'target game module not found',
        modules: Process.enumerateModules().map(m => ({name:m.name, base:m.base.toString(), size:m.size}))});
  throw new Error('target game module not found');
}
send({type:'probe_start', module:targetModule.name, base:targetModule.base.toString(), size:targetModule.size,
      retailAddressMode: targetModule.base.equals(imageBase), targetCount:targets.length});
const installed = [];
const failures = [];
const counts = {};
for (const target of targets) {
  const va = ptr(target.va);
  if (targetModule.name.toLowerCase() !== 'igi.exe' || !targetModule.base.equals(imageBase)) {
    continue;
  }
  const address = va;
  const offset = address.sub(targetModule.base);
  if (offset.compare(ptr(0)) < 0 || offset.compare(ptr(targetModule.size)) >= 0) {
    failures.push({name:target.name, va:target.va, error:'VA outside module'});
    continue;
  }
  counts[target.name] = {va:target.va, hits:0, first:[]};
  try {
    Interceptor.attach(address, {
      onEnter(args) {
        const rec = counts[target.name];
        rec.hits++;
        this.callRecord = null;
        if (rec.first.length < 4) {
          const words = [];
          for (let i = 0; i < 6; i++) words.push(args[i].toString());
          this.callRecord = {args: words};
          rec.first.push(this.callRecord);
        }
      },
      onLeave(retval) {
        if (this.callRecord !== null) this.callRecord.returnValue = retval.toString();
      }
    });
    installed.push({name:target.name, va:target.va});
  } catch (e) {
    failures.push({name:target.name, va:target.va, error:String(e)});
  }
}
send({type:'probe_hooks', installed:installed.length, failed:failures.length, failures});
setInterval(() => {
  const called = {};
  for (const name of Object.keys(counts)) if (counts[name].hits) called[name] = counts[name];
  send({type:'probe_snapshot', called});
}, 5000);
