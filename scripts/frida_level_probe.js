// IGI retail entry/return probe. Run generate_frida_level_probe.py to refresh
// its address list from the manifest's implemented native sources.
// ABI is intentionally unknown here: record raw argument words and EAX only; do not dereference.
'use strict';
// These are runtime comparison anchors.  They remain in the probe even before
// their OpenIGI counterparts have an address-bound implementation: their job is
// to observe the retail sequence needed to reconstruct player input and terrain
// behavior.  The generated list below separately follows implemented sources.
const runtimeAnchors = [
  { name: "DefineHumanPlayerGeneral", va: "0x00410070" },
  { name: "HumanPlayer_LoadParameters", va: "0x004137A0" },
  { name: "GOInRemap", va: "0x004037C0" },
  { name: "InputPort_Process", va: "0x004ED5C0" }
];
const targets = [
  {
    "name": "QTask_OpenEvent",
    "va": "0x00401050"
  },
  {
    "name": "Game_GetContext",
    "va": "0x00402880"
  },
  {
    "name": "Flow_SetState",
    "va": "0x004028A0"
  },
  {
    "name": "Config_GetActiveGraphicOptions",
    "va": "0x00404590"
  },
  {
    "name": "Config_GraphicOptionsSetPerformanceFlags",
    "va": "0x00404F00"
  },
  {
    "name": "Config_GraphicOptionsGetPerformanceFlags",
    "va": "0x00404F60"
  },
  {
    "name": "Config_GraphicOptionsGetPerfLevelFromFlags",
    "va": "0x00405530"
  },
  {
    "name": "Config_GraphicOptionsGetPerfFlagsFromLevel",
    "va": "0x004055C0"
  },
  {
    "name": "Config_GraphicOptionsNumPerfLevels",
    "va": "0x00405620"
  },
  {
    "name": "Human_IsCrouched",
    "va": "0x00463180"
  },
  {
    "name": "Human_CameraEyePosition",
    "va": "0x00465820"
  },
  {
    "name": "Human_TaskViewReset",
    "va": "0x004659E0"
  },
  {
    "name": "Script_GetDebugCompileState",
    "va": "0x0048F1D0"
  },
  {
    "name": "Script_GetForcePrecompiledState",
    "va": "0x0048F340"
  },
  {
    "name": "Loop_SetStatusByte",
    "va": "0x0048F480"
  },
  {
    "name": "Display_ComputeScreenAspect",
    "va": "0x00491D40"
  },
  {
    "name": "Transcontext_BuildViewBlock",
    "va": "0x00497D70"
  },
  {
    "name": "Camera_ResetMatrix",
    "va": "0x004A2600"
  },
  {
    "name": "Camera_UpdateFogPalette",
    "va": "0x004A26C0"
  },
  {
    "name": "sprintf",
    "va": "0x004A53B3"
  },
  {
    "name": "String_VFormat",
    "va": "0x004A5702"
  },
  {
    "name": "Mem_AllocChunk",
    "va": "0x004AA670"
  },
  {
    "name": "Mem_AllocWithRetry",
    "va": "0x004AA682"
  },
  {
    "name": "Mem_AllocateRawBlock",
    "va": "0x004AA6AE"
  },
  {
    "name": "Resource_MatchPrefix",
    "va": "0x004AAD10"
  },
  {
    "name": "Mem_InvokeNewHandler",
    "va": "0x004AD722"
  },
  {
    "name": "String_ToUpperChar",
    "va": "0x004AD73D"
  },
  {
    "name": "Error_Show",
    "va": "0x004AF7B0"
  },
  {
    "name": "HeapList_Insert",
    "va": "0x004AF8F0"
  },
  {
    "name": "HeapList_Remove",
    "va": "0x004AF960"
  },
  {
    "name": "Mem_InitHeapState",
    "va": "0x004B0BA0"
  },
  {
    "name": "Mem_Alloc",
    "va": "0x004B0C60"
  },
  {
    "name": "QHash_ValueSet",
    "va": "0x004B0D40"
  },
  {
    "name": "Texture_GetActiveBackendId",
    "va": "0x004B0F80"
  },
  {
    "name": "Device_CloseStream",
    "va": "0x004B1690"
  },
  {
    "name": "Device_GetStreamPosition",
    "va": "0x004B1780"
  },
  {
    "name": "Device_SeekStream",
    "va": "0x004B17F0"
  },
  {
    "name": "String_FindExtension",
    "va": "0x004B1E90"
  },
  {
    "name": "Texture_UploadMipmapPacket",
    "va": "0x004B23B0"
  },
  {
    "name": "Texture_LoadLoopTextureFile",
    "va": "0x004B24C0"
  },
  {
    "name": "Texture_EnableUpload",
    "va": "0x004B26A0"
  },
  {
    "name": "Texture_DisableUpload",
    "va": "0x004B26B0"
  },
  {
    "name": "Resource_CreateNode",
    "va": "0x004B59F0"
  },
  {
    "name": "Resource_CanonicalizePath",
    "va": "0x004B5AF0"
  },
  {
    "name": "Resource_CalculatePathHash",
    "va": "0x004B5B30"
  },
  {
    "name": "Resource_FileExists",
    "va": "0x004B5B60"
  },
  {
    "name": "Resource_FreeNode",
    "va": "0x004B5C90"
  },
  {
    "name": "Resource_Load",
    "va": "0x004B5F00"
  },
  {
    "name": "Resource_RegisterBuffer",
    "va": "0x004B5F80"
  },
  {
    "name": "Resource_RegisterIlffContainer",
    "va": "0x004B60F0"
  },
  {
    "name": "Resource_Unload",
    "va": "0x004B6380"
  },
  {
    "name": "Matrix_InvalidateSlot",
    "va": "0x004B7450"
  },
  {
    "name": "Ilff_GetNextChunk",
    "va": "0x004B7470"
  },
  {
    "name": "Ilff_GetFirstChunk",
    "va": "0x004B7490"
  },
  {
    "name": "Ilff_GetChunkPayload",
    "va": "0x004B74D0"
  },
  {
    "name": "Ilff_ReadContainerHeader",
    "va": "0x004B7540"
  },
  {
    "name": "Ilff_GetFormType",
    "va": "0x004B7570"
  },
  {
    "name": "Ilff_GetChunkTag",
    "va": "0x004B7580"
  },
  {
    "name": "Ilff_FindChunk",
    "va": "0x004B7590"
  },
  {
    "name": "Texture_UploadPalettePacket",
    "va": "0x004B7CF0"
  },
  {
    "name": "Qvm_ReportError",
    "va": "0x004B7E10"
  },
  {
    "name": "Qvm_Load",
    "va": "0x004B80B0"
  },
  {
    "name": "Qvm_Cleanup",
    "va": "0x004B83D0"
  },
  {
    "name": "Qvm_SetGlobalState",
    "va": "0x004B8A10"
  },
  {
    "name": "Qvm_ExecuteInstructions",
    "va": "0x004BCFD0"
  },
  {
    "name": "Qvm_SetOpcodeHandler",
    "va": "0x004BD3D0"
  },
  {
    "name": "Qvm_PushInteger",
    "va": "0x004BDA00"
  },
  {
    "name": "Terrain_LoadTexturesAndLightmaps",
    "va": "0x004C5F00"
  },
  {
    "name": "Terrain_Load",
    "va": "0x004C6240"
  },
  {
    "name": "Terrain_CreateMaterial",
    "va": "0x004C6670"
  },
  {
    "name": "Terrain_CreateTileMap",
    "va": "0x004C6760"
  },
  {
    "name": "Render_GetActiveBackendId",
    "va": "0x004CEA80"
  },
  {
    "name": "Ilff_FindFormChunk",
    "va": "0x004CEAA0"
  },
  {
    "name": "DynObj_LoadModelTexturePackage",
    "va": "0x004CF080"
  },
  {
    "name": "DynObj_LookupHashItem",
    "va": "0x004CF300"
  },
  {
    "name": "DynObj_LoadPackageMeshes",
    "va": "0x004CF330"
  },
  {
    "name": "DynObj_LoadModelMesh",
    "va": "0x004CF390"
  },
  {
    "name": "DynObj_LoadPackageLovs",
    "va": "0x004CF630"
  },
  {
    "name": "DynObj_LoadModelTextures",
    "va": "0x004CF790"
  },
  {
    "name": "DynObj_LoadPackagePalettes",
    "va": "0x004CF870"
  },
  {
    "name": "DynObj_LoadPackageAnimations",
    "va": "0x004CF910"
  },
  {
    "name": "Model_GetAttachment",
    "va": "0x004D0CF0"
  },
  {
    "name": "Model_GetAttachmentCount",
    "va": "0x004D0D10"
  },
  {
    "name": "Terrain_TraverseContourQuadtree",
    "va": "0x004D14E0"
  },
  {
    "name": "LODSettings_LoadParameters",
    "va": "0x004EDB60"
  },
  {
    "name": "TerrainLightMap_Init_2",
    "va": "0x004F82B0"
  },
  {
    "name": "TextureModifier_Push",
    "va": "0x00505E40"
  },
  {
    "name": "HeightMap_Enter",
    "va": "0x00506F00"
  },
  {
    "name": "Terrain_DecodeChunkCommands",
    "va": "0x00510010"
  },
  {
    "name": "Terrain_Draw",
    "va": "0x00512C30"
  },
  {
    "name": "Terrain_LoadTexturePackage",
    "va": "0x005175A0"
  }
];
const probeTargets = [...runtimeAnchors, ...targets].filter((target, index, items) =>
  items.findIndex(other => other.va === target.va) === index);
const imageBase = ptr('0x00400000');
// Recognised game modules, in precedence order (first exact name match wins):
//   1. igi.exe               - retail Project IGI 1; its code path below is unchanged.
//   2. open-igi.exe          - legacy reconstruction executable name.
//   3. igi1.exe              - current reconstruction executable name.
//   4. open-igi-probe-*.exe  - strict driver's staged copy of the candidate.
// The staged pattern is matched last; if several staged copies exist they are
// ordered by name then base, so resolution is deterministic. There is
// deliberately no Process.mainModule / getModuleByName fallback: an absent game
// module is an explicit probe error, never a hook on an unrelated system DLL.
const moduleNames = ['igi.exe', 'open-igi.exe', 'igi1.exe'];
const modules = Process.enumerateModules();
const lowered = m => m.name.toLowerCase();
let targetModule = null;
for (const wanted of moduleNames) {
  targetModule = modules.find(m => lowered(m) === wanted) || null;
  if (targetModule !== null) break;
}
if (targetModule === null) {
  const staged = modules.filter(m => lowered(m).startsWith('open-igi-probe-') &&
                                     lowered(m).endsWith('.exe'));
  staged.sort((a, b) => {
    const left = lowered(a);
    const right = lowered(b);
    if (left < right) return -1;
    if (left > right) return 1;
    return a.base.compare(b.base);
  });
  if (staged.length > 0) targetModule = staged[0];
}
if (!targetModule) {
  send({type: 'probe_error', error: 'target game module not found', modules: modules.map(m => ({name:m.name, base:m.base.toString(), size:m.size}))});
  throw new Error('target game module not found');
}
send({type:'probe_start', module:targetModule.name, base:targetModule.base.toString(), size:targetModule.size,
      retailAddressMode: targetModule.name.toLowerCase() === 'igi.exe' && targetModule.base.equals(imageBase),
      targetCount:probeTargets.length});
const installed=[];
const failures=[];
const counts={};
// Retail VAs are absolute addresses only for retail igi.exe at its preferred
// base; that path (including the silent skip below) is unchanged. A candidate
// image is assumed to keep retail image RVAs (VA - imageBase), so its hooks are
// relocated into the loaded module and must land in executable memory. That is
// an address convention, not proof the candidate places that native at the
// retail RVA: an installed hook that never fires is not evidence of absence.
const retailModule = targetModule.name.toLowerCase() === 'igi.exe';
for (const target of targets) {
  const va=ptr(target.va);
  if (retailModule) {
    // Retail VAs are only meaningful for original igi.exe at its preferred base.
    if (!targetModule.base.equals(imageBase)) {
      continue;
    }
  }
  const address=retailModule ? va : targetModule.base.add(va.sub(imageBase));
  const offset=address.sub(targetModule.base);
  if (offset.compare(ptr(0)) < 0 || offset.compare(ptr(targetModule.size)) >= 0) {
    failures.push({name:target.name,va:target.va,error:'VA outside module'});
    continue;
  }
  if (!retailModule) {
    const range = Process.findRangeByAddress(address);
    if (range === null || range.protection.toLowerCase().indexOf('x') < 0) {
      failures.push({name:target.name,va:target.va,error:'candidate address is not executable'});
      continue;
    }
  }
  counts[target.name]={va:target.va,hits:0,first:[]};
  try {
    Interceptor.attach(address, {
      onEnter(args) {
        const rec=counts[target.name];
        rec.hits++;
        this.callRecord=null;
        if (rec.first.length < 4) {
          const words=[];
          for (let i=0;i<6;i++) words.push(args[i].toString());
          this.callRecord={args:words};
          rec.first.push(this.callRecord);
        }
      },
      onLeave(retval) {
        if (this.callRecord !== null) {
          this.callRecord.returnValue=retval.toString();
        }
      }
    });
    installed.push({name:target.name,va:target.va});
  } catch (e) {
    failures.push({name:target.name,va:target.va,error:String(e)});
  }
}
send({type:'probe_hooks',installed:installed.length,failed:failures.length,failures});
setInterval(() => {
  const called={};
  for (const name of Object.keys(counts)) if (counts[name].hits) called[name]=counts[name];
  send({type:'probe_snapshot',called});
}, 10000);
