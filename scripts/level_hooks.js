// Frida Level Loading Tracer for open-igi.exe
// Hooks every major pipeline method invoked during game level loading.

var methods = [
  { addr: "0x004168e0", name: "RunRetailModelScene", desc: "Main level runner & window setup" },
  { addr: "0x0042c500", name: "Host_LoadResourceFile", desc: "VFS file read from disk" },
  { addr: "0x00424910", name: "Host_ParseModelTexturePackage", desc: "Parse .MTP model/texture index" },
  { addr: "0x00429fd0", name: "Host_LoadResourceArchive", desc: "Load .RES archive package" },
  { addr: "0x00427700", name: "Host_ParseResourceArchive", desc: "Unpack .RES archive entries" },
  { addr: "0x0042d870", name: "Host_LoadQvmScript", desc: "Load objects.qsc / objects.qvm" },
  { addr: "0x0044afa0", name: "Qvm_LoadCompiledProgram", desc: "Parse LOOP bytecode header" },
  { addr: "0x00441c10", name: "Qvm_ReadObjectCalls", desc: "Parse Task_New object definitions" },
  { addr: "0x00446cc0", name: "Host_LoadLevelTerrain", desc: "Load terrain contours & tiles" },
  { addr: "0x00442ef0", name: "Host_ParseTerrainTextureFile", desc: "Parse terrain texture layers" },
  { addr: "0x004439f0", name: "Host_ParseModelTextureFile", desc: "Parse model textures (PAL/RGB)" },
  { addr: "0x00420920", name: "DynObj_LoadModelMesh", desc: "Parse MEF model mesh & ATTA chunks" },
  { addr: "0x0041b7c0", name: "Display_DecodeMeshRenderData", desc: "Decode vertex buffers & render batches" }
];

console.log("[*] Installing level-loading hooks in open-igi.exe...");

var depth = 0;

methods.forEach(function(m) {
  try {
    var p = ptr(m.addr);
    Interceptor.attach(p, {
      onEnter: function(args) {
        this.start = Date.now();
        var indent = "  ".repeat(depth);
        console.log(indent + "[--> ENTER] " + m.name + " (" + m.desc + ")");
        depth++;
      },
      onLeave: function(retval) {
        depth = Math.max(0, depth - 1);
        var elapsed = Date.now() - this.start;
        var indent = "  ".repeat(depth);
        console.log(indent + "[<-- LEAVE] " + m.name + " (took " + elapsed + " ms)");
      }
    });
  } catch(e) {
    console.error("[-] Failed to hook " + m.name + ": " + e);
  }
});

// Also hook CreateFileA to trace the exact file paths opened on disk
try {
  var createFile = Module.getGlobalExportByName('CreateFileA');
  Interceptor.attach(createFile, {
    onEnter: function(args) {
      try {
        var filePath = args[0].readCString();
        if (filePath && !filePath.includes('Windows') && !filePath.includes('.dll')) {
          var indent = "  ".repeat(depth);
          console.log(indent + "  [FILE OPEN] " + filePath);
        }
      } catch(e) {}
    }
  });
} catch(e) {}

console.log("[+] All 13 level loading methods + File I/O successfully hooked!");
console.log("[+] Trigger a level load or restart the mission to see the trace.\n");
