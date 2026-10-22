// Typed paired probe for the retail per-frame dispatch, 0x00401050.
//
// The retail dispatch establishes a setjmp frame, reads the current record at
// 0x00567C8C, loads the 16-bit class id at record+0x1C, indexes the phase-1
// handler table at 0x00A970E0, and calls the handler with the current record.
// This probe records those typed values instead of raw ABI words:
//   inputs      record_present, dispatch_index
//   outputs     handler_selected (the table entry is the Game phase-1 handler)
//   sideEffects loop status byte after the call (0x005C8DE8)
//
// It is side-aware: the candidate keeps the same fields at its own linked
// addresses. The probe source is identical for both runs; only the addresses
// passed to rpc.exports.hook differ, and the side constants below select the
// per-image globals.
'use strict';

const IMAGE_MODULE = (() => {
  const modules = Process.enumerateModules();
  return modules.find(m => /^igi\.exe$/i.test(m.name))
      || modules.find(m => /open-igi.*\.exe$/i.test(m.name))
      || Process.mainModule;
})();

const IMAGE = (() => {
  const name = (IMAGE_MODULE && IMAGE_MODULE.name || '').toLowerCase();
  if (name.indexOf('open-igi') >= 0) {
    return {
      side: 'candidate',
      // Addresses resolved from the freshly built candidate with
      // `i686-w64-mingw32-nm -C`: Game_FrameDispatchRecord, Loop_StatusByte and
      // _QTask_HandlerTable + task_phase_stride (0x180). The previously
      // hardcoded values were from an older link and made record_readable false.
      recordGlobal: 0x005D4BE0,   // Game_FrameDispatchRecord
      tableBase: 0x005F9380,      // _QTask_HandlerTable + 0x180
      statusGlobal: 0x005D4BF0,   // Loop_StatusByte
      phase1Handler: 0x0041F9E0,  // Game_Setup
      levelStart: 0x0041F9C0      // Level_Start
    };
  }
  return {
    side: 'retail',
    recordGlobal: 0x00567C8C,
    tableBase: 0x00A970E0,
    statusGlobal: 0x005C8DE8,
    phase1Handler: 0x00416260,    // Game phase-1 handler
    levelStart: 0x00415B30         // Level_Start
  };
})();

// Wine reports Process.mainModule as advapi32.dll, so the game image must be
// located by name. The image's own PE header supplies its preferred base; the
// probe never assumes the image is mapped at its linked VA.
const moduleBase = IMAGE_MODULE.base;
const preferredBase = (() => {
  const peOffset = moduleBase.add(0x3c).readU32();
  return moduleBase.add(peOffset + 0x34).readU32();
})();

function actual(va) {
  return moduleBase.add(ptr(va).sub(ptr(preferredBase)));
}

function readGlobalPointer(va) {
  return actual(va).readPointer();
}

function readByte(va) {
  return actual(va).readU8();
}

function roleFor(address) {
  const value = address.toInt32 ? address.toInt32() >>> 0 : address;
  if (value === IMAGE.levelStart) return 'level_start';
  return 'dispatch';
}

function hookDispatch(address) {
  Interceptor.attach(actual(address), {
    onEnter(args) {
      let record = null;
      let dispatchIndex = null;
      let handler = null;
      let recordReadable = false;
      try {
        record = readGlobalPointer(IMAGE.recordGlobal);
        if (!record.isNull()) {
          dispatchIndex = record.add(0x1C).readU16();
          handler = actual(IMAGE.tableBase).add(dispatchIndex * 4).readU32();
          recordReadable = true;
        }
      } catch (error) {
        recordReadable = false;
      }
      this.typedState = {
        record_present: record !== null && !record.isNull(),
        record_readable: recordReadable,
        dispatch_index: dispatchIndex,
        handler_selected: handler === IMAGE.phase1Handler,
        handler_nonzero: handler !== null && handler !== 0,
        // Image-relative VA, so the two images can be compared by symbol and
        // never by absolute heap/table address.
        handler_va: handler === null ? null : (handler - preferredBase) >>> 0
      };
      this.role = 'dispatch';
    },
    onLeave(retval) {
      let status = null;
      try { status = readByte(IMAGE.statusGlobal); } catch (error) { status = null; }
      send({
        type: 'observed',
        address: address.toString(),
        role: this.role,
        raw: { retval: retval.toString() },
        typedState: {
          scope: 'qtask-open-event-dispatch-v1',
          inputs: {
            record_present: this.typedState.record_present,
            record_readable: this.typedState.record_readable,
            dispatch_index: this.typedState.dispatch_index
          },
          outputs: {
            handler_selected: this.typedState.handler_selected,
            handler_nonzero: this.typedState.handler_nonzero,
            handler_va: this.typedState.handler_va
          },
          sideEffects: {
            status_byte_after: status
          }
        }
      });
    }
  });
}

function hookLevelStart(address) {
  Interceptor.attach(actual(address), {
    onEnter(args) {
      this.typedState = { context_nonzero: !args[0].isNull() };
    },
    onLeave(retval) {
      send({
        type: 'observed',
        address: address.toString(),
        role: 'level_start',
        raw: { retval: retval.toString() },
        typedState: {
          scope: 'level-start-entry-v1',
          inputs: { context_nonzero: this.typedState.context_nonzero },
          outputs: { entered: true },
          sideEffects: {}
        }
      });
    }
  });
}

rpc.exports = {
  hook(address) {
    try {
      const target = ptr(address);
      const role = roleFor(target);
      if (role === 'level_start') {
        hookLevelStart(target);
      } else {
        hookDispatch(target);
      }
      send({ type: 'hooked', address: target.toString(), role: role,
             side: IMAGE.side, image: IMAGE_MODULE.name,
             imageBase: moduleBase.toString(), preferredBase: preferredBase.toString() });
      return true;
    } catch (error) {
      send({ type: 'hook_failed', address: String(address), error: String(error) });
      return false;
    }
  }
};
