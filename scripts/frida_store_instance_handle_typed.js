'use strict';
// Typed observation for retail 0x0048F0D0 (StoreInstanceHandle) and the staged
// candidate's equivalent entry. Retail bytes 0x0048F0D0..0x0048F0DA:
//   8b442404    mov eax, dword [esp+4]      ; cdecl argument: WinMain's HINSTANCE
//   a3c08b5c00  mov dword [0x005C8BC0], eax ; the process instance-handle global
//   c3          ret                         ; EAX still holds the argument bits
// One basic block, no calls, no other memory access; the only side effect is
// the 4-byte store to the reviewed state VA.
//
// An HINSTANCE value is process-local. Nothing here reports a raw address as
// comparable data: the typed state carries the argument's class (null,
// non-null pointer, or the process instance handle), the within-process
// relations to the main module base, to the stored word and to the return
// value, and the store delta. A separate observation, sent after the return,
// reads the first visible top-level "Innerloop" window owned by this process
// and checks that the HINSTANCE the window was created with is the same
// handle. That is the only way this probe treats the argument as an instance
// handle rather than an arbitrary pointer.
//
// Driver contract (scripts/frida_scoped_capture.py):
//   exports_sync.hook(address, layout) -> {installed: true, address}
//   send({type: 'observed', address, typedState: {scope, inputs, outputs, sideEffects}})
//   send({type: 'window_instance_observation', address, scope, window: {...}})
//
// Argument reads are explicit and defensive. On this Frida 17.24.0 ia32
// endpoint the Interceptor `args` object itself throws
// "RangeError: invalid array index" when measured or coerced (args.length and
// Object.prototype.toString.call(args) both throw), while explicit indices
// still resolve. So the four cdecl words are read one at a time, each in its
// own try/catch, with an independent x86 stack read from the Interceptor
// context as fallback. No args.length, no array methods, no iteration of args.
// Every word reports how it was obtained (or that it was unavailable).

const NATIVE = '0x0048F0D0';
const SCOPE = 'startup-instance-handle-store';
const ENTRY_SIZE = 10;      // 0x0048F0D0..0x0048F0DA
const STATE_SIZE = 4;       // one dword at the retail state VA
const ARG_WORDS = 4;        // cdecl words inspected per call, probe-side bound
const ARG_UNAVAILABLE = '<unavailable>';
const WINDOW_POLL_MS = 25;
const WINDOW_POLL_LIMIT_MS = 20000;
const GWL_HINSTANCE = -6;

// Resolve the main module by name for both igi.exe and the staged candidate.
// The image may not yet be exposed by the plain lookup during early spawn, so
// fall back to a case-insensitive enumeration and require one match.
function moduleFor(name) {
  const direct = Process.findModuleByName(name);
  if (direct !== null) return direct;
  const wanted = String(name).toLowerCase();
  const matches = Process.enumerateModules().filter(
    module => module.name.toLowerCase() === wanted);
  if (matches.length !== 1) throw new Error('module not found: ' + name);
  return matches[0];
}

// Map a reviewed VA to this process. Never blind-read: the target range must be
// inside the resolved image.
function resolveImageVa(module, preferredBase, va, size) {
  const relative = ptr(va).sub(ptr(preferredBase));
  if (relative.compare(ptr(0)) < 0 || relative.compare(ptr(module.size - size)) > 0) {
    throw new Error('reviewed VA ' + va + ' is outside ' + module.name);
  }
  return module.base.add(relative);
}

// Read one stack word without ever letting a bad `args` surface escape. The
// Interceptor argument object is attempted only by explicit index; if that
// throws or yields no pointer, the same dword is read from the x86 stack via
// the Interceptor context ([esp + 4 + 4*index], cdecl). Returns a marker that
// records the source, or an explicit unavailable marker; never throws.
function readArgument(args, context, index) {
  let value = null;
  let source = 'unavailable';
  let errors = [];
  try {
    const candidate = args[index];
    if (candidate !== null && candidate !== undefined) {
      value = ptr(candidate);
      source = 'args[' + index + ']';
    }
  } catch (error) {
    errors.push(String(error));
  }
  if (value === null) {
    try {
      const stack = ptr(context.esp);
      if (stack.isNull()) throw new Error('stack pointer unavailable');
      value = stack.add(4 + index * 4).readPointer();
      source = 'stack[esp+' + (4 + index * 4) + ']';
    } catch (error) {
      errors.push(String(error));
    }
  }
  const marker = {
    index: index,
    state: value === null ? 'unavailable' : 'value',
    source: source
  };
  if (value === null && errors.length > 0) marker.error = errors.join('; ');
  return {marker: marker, value: value};
}

function pointerClass(value) {
  if (value === null) return 'unavailable';
  if (value.isNull()) return 'null';
  return 'non-null-pointer';
}

rpc.exports = {
  hook(address, layout) {
    if (address !== NATIVE) throw new Error('unsupported native: ' + address);
    if (Process.pointerSize !== 4) throw new Error('x86 pointer ABI required');
    if (typeof layout !== 'object' || layout === null) throw new Error('missing layout');
    if (typeof layout.moduleName !== 'string' || !layout.moduleName) throw new Error('missing moduleName');
    if (typeof layout.preferredBase !== 'string' || typeof layout.entryVA !== 'string'
        || typeof layout.stateVA !== 'string') {
      throw new Error('layout needs preferredBase, entryVA and stateVA');
    }
    // The window class is per-side identification, not a compared behaviour:
    // retail registers "Innerloop"; the current candidate model-scene shell
    // registers "IGI.Retail.ModelScene". The contract compares the HINSTANCE
    // relation, never the class name across processes.
    if (typeof layout.windowClass !== 'string' || !layout.windowClass) {
      throw new Error('layout needs windowClass');
    }
    const expectedWindowClass = layout.windowClass;

    const module = moduleFor(layout.moduleName);
    const entry = resolveImageVa(module, layout.preferredBase, layout.entryVA, ENTRY_SIZE);
    const state = resolveImageVa(module, layout.preferredBase, layout.stateVA, STATE_SIZE);

    let callCount = 0;
    let firstArgument = null;
    let pending = null;
    let windowPollStarted = false;
    let windowObservationSent = false;

    function readStored() {
      try {
        return {value: state.readPointer(), state: 'known'};
      } catch (error) {
        return {value: null, state: 'unavailable'};
      }
    }

    // Read the HINSTANCE relation of one window handle. Returns null when the
    // handle is not a window of the reviewed game class.
    function windowFields(handle, phase) {
      const user32 = Process.getModuleByName('user32.dll');
      const className = new NativeFunction(
        user32.getExportByName('GetClassNameW'), 'int', ['pointer', 'pointer', 'int'], 'stdcall');
      const instanceOf = new NativeFunction(
        user32.getExportByName('GetWindowLongW'), 'uint32', ['pointer', 'int32'], 'stdcall');

      const classBuffer = Memory.alloc(2 * 128);
      const length = className(handle, classBuffer, 128);
      if (length <= 0) return null;
      const actualClass = classBuffer.readUtf16String(length);
      if (actualClass !== expectedWindowClass) return null;

      const instance = ptr(instanceOf(handle, GWL_HINSTANCE) >>> 0);
      const storedNow = readStored();
      const argument = pending === null ? null : pending.argument;
      const argumentKnown = argument !== null;
      return {
        found: true,
        class_name: actualClass,
        phase: phase,
        instance_handle_state: instance.isNull() ? 'null' : 'known',
        instance_is_null: instance.isNull(),
        instance_equals_argument: argumentKnown && !instance.isNull() && instance.equals(argument),
        instance_equals_stored: storedNow.value !== null && !instance.isNull()
          && instance.equals(storedNow.value),
        instance_equals_module_base: instance.equals(module.base),
        stored_now_equals_argument: argumentKnown && storedNow.value !== null
          && storedNow.value.equals(argument),
        stored_now_state: storedNow.state,
        observed_after_ms: pending === null ? null : (Date.now() - pending.at)
      };
    }

    // Send the window observation once, from either the creation hook (on the
    // game thread, no scheduling delay) or the visible-window poll fallback.
    function observeWindow(handle, phase) {
      if (windowObservationSent || pending === null) return;
      if (handle === null || handle.isNull()) return;
      try {
        const window = windowFields(handle, phase);
        if (window === null) return;
        windowObservationSent = true;
        send({type: 'window_instance_observation', address: NATIVE, scope: SCOPE, window: window});
      } catch (error) {
        // Never throw from an interceptor callback.
      }
    }

    // Read the HINSTANCE of the first visible top-level "Innerloop" window
    // owned by this process. This is the window the scenario actions target.
    function inspectWindow() {
      const user32 = Process.getModuleByName('user32.dll');
      const enumerate = new NativeFunction(
        user32.getExportByName('EnumWindows'), 'int', ['pointer', 'pointer'], 'stdcall');
      const owner = new NativeFunction(
        user32.getExportByName('GetWindowThreadProcessId'), 'uint32', ['pointer', 'pointer'], 'stdcall');
      const visible = new NativeFunction(
        user32.getExportByName('IsWindowVisible'), 'int', ['pointer'], 'stdcall');
      const className = new NativeFunction(
        user32.getExportByName('GetClassNameW'), 'int', ['pointer', 'pointer', 'int'], 'stdcall');

      const pidBuffer = Memory.alloc(4);
      const classBuffer = Memory.alloc(2 * 128);
      let found = null;
      const callback = new NativeCallback(function (handle, _) {
        try {
          owner(handle, pidBuffer);
          if (pidBuffer.readU32() !== Process.id) return 1;
          if (visible(handle) === 0) return 1;
          const length = className(handle, classBuffer, 128);
          if (length <= 0) return 1;
          if (classBuffer.readUtf16String(length) !== expectedWindowClass) return 1;
          found = handle;
          return 0;
        } catch (error) {
          return 1;
        }
      }, 'int', ['pointer', 'pointer'], 'stdcall');
      enumerate(callback, ptr(0));
      if (found === null) return null;
      return windowFields(found, 'after-return');
    }

    function finishWindowObservation() {
      if (windowObservationSent || pending === null) return;
      windowObservationSent = true;
      try {
        const window = inspectWindow();
        if (window !== null) {
          send({type: 'window_instance_observation', address: NATIVE, scope: SCOPE, window: window});
          return;
        }
      } catch (error) {
        // Fall through to the explicit unavailable record; never throw.
      }
      send({
        type: 'window_instance_observation',
        address: NATIVE,
        scope: SCOPE,
        window: {found: false, phase: 'after-return', state: 'unavailable',
                 reason: 'window-not-observed'}
      });
    }

    function scheduleWindowObservation() {
      if (windowPollStarted) return;
      windowPollStarted = true;
      const started = Date.now();
      const poll = function () {
        if (pending === null || windowObservationSent) return;
        try {
          const window = inspectWindow();
          if (window !== null) {
            windowObservationSent = true;
            send({type: 'window_instance_observation', address: NATIVE, scope: SCOPE, window: window});
            return;
          }
        } catch (error) {
          // The module or an export may not be present yet; retry until the bound.
        }
        if (Date.now() - started < WINDOW_POLL_LIMIT_MS) {
          setTimeout(poll, WINDOW_POLL_MS);
        } else {
          finishWindowObservation();
        }
      };
      setTimeout(poll, 0);
    }

    Interceptor.attach(entry, {
      onEnter(args) {
        try {
          // cdecl x86: the instance handle is the dword at [esp+4]. Read the
          // four inspected words individually and keep an explicit marker for
          // each; readArgument never throws.
          const reads = [];
          let argument = null;
          for (let index = 0; index < ARG_WORDS; ++index) {
            const read = readArgument(args, this.context, index);
            reads.push(read.marker);
            if (index === 0) argument = read.value;
          }
          const index = callCount;
          callCount += 1;
          const consistent = index === 0
            ? true
            : (argument !== null && firstArgument !== null && argument.equals(firstArgument));
          if (firstArgument === null && argument !== null) firstArgument = argument;

          this.index = index;
          this.argument = argument;       // null means "could not be read", not 0
          this.argumentReads = reads;
          this.consistent = consistent;
          this.previous = readStored();
          this.failed = null;
        } catch (error) {
          this.failed = String(error);
        }
      },
      onLeave(returned) {
        if (this.failed !== null && this.failed !== undefined) {
          send({type: 'hook_failed', address, error: this.failed});
          return;
        }
        try {
          const argument = this.argument;
          const argumentKnown = argument !== null;
          const stored = readStored();
          const returnedPtr = ptr(returned);
          const previous = this.previous;
          const previousKnown = previous.value !== null;
          const storedKnown = stored.value !== null;
          const argumentClass = argumentKnown
            ? (argument.isNull() ? 'null'
              : (argument.equals(module.base) ? 'process-instance-handle' : 'non-null-pointer'))
            : 'unavailable';
          const previousClass = !previousKnown
            ? 'unavailable'
            : (previous.value.isNull() ? 'null' : 'non-null');
          const storedAliasesArgument = argumentKnown && storedKnown
            && stored.value.equals(argument);
          const storedClass = !storedKnown
            ? 'unavailable'
            : (stored.value.isNull() ? 'null'
              : (storedAliasesArgument && argumentClass === 'process-instance-handle'
                ? 'process-instance-handle' : 'non-null-pointer'));
          const returnAliasesArgument = argumentKnown && returnedPtr.equals(argument);
          const returnAliasesStored = storedKnown && returnedPtr.equals(stored.value);
          const storeChanged = previousKnown && storedKnown
            ? !previous.value.equals(stored.value) : null;

          send({
            type: 'observed',
            address,
            typedState: {
              scope: SCOPE,
              inputs: {
                instance_handle: argumentClass,
                instance_handle_state: argumentKnown ? 'known' : 'unavailable',
                argument_is_null: argumentKnown ? argument.isNull() : null,
                argument_non_null: argumentKnown ? !argument.isNull() : null,
                argument_equals_module_base: argumentKnown ? argument.equals(module.base) : null,
                argument_module_base_state: 'known',
                argument_is_process_instance_handle: argumentKnown
                  ? argumentClass === 'process-instance-handle' : null,
                argument_consistent_with_first: argumentKnown ? this.consistent : null,
                argument_consistency_state: this.index === 0
                  ? 'first-observation' : (argumentKnown ? 'known' : 'unknown'),
                // One explicit marker per inspected cdecl word: state is
                // 'value' or 'unavailable'; source names the surface used.
                argument_reads: this.argumentReads
              },
              outputs: {
                return_aliases_argument: argumentKnown ? returnAliasesArgument : null,
                return_aliases_argument_state: argumentKnown ? 'known' : 'unknown',
                return_is_null: returnedPtr.isNull(),
                return_aliases_stored: storedKnown ? returnAliasesStored : null,
                return_aliases_stored_state: storedKnown ? 'known' : 'unknown'
              },
              sideEffects: {
                previous_instance_handle: previousClass,
                previous_instance_handle_state: previousKnown ? 'known' : 'unavailable',
                previous_is_null: previousKnown ? previous.value.isNull() : null,
                stored_instance_handle: storedClass,
                stored_instance_handle_state: storedKnown ? 'known' : 'unavailable',
                stored_is_null: storedKnown ? stored.value.isNull() : null,
                stored_aliases_argument: argumentKnown && storedKnown
                  ? storedAliasesArgument : null,
                stored_aliases_argument_state: argumentKnown && storedKnown
                  ? 'known' : 'unknown',
                store_changed: storeChanged,
                stored_equals_return: storedKnown ? returnAliasesStored : null
              }
            }
          });

          pending = {
            argument: argument,
            stored: stored.value,
            at: Date.now()
          };
          scheduleWindowObservation();
        } catch (error) {
          send({type: 'hook_failed', address, error: String(error)});
        }
      }
    });
    // Window creation is after the native call and runs on the game thread, so
    // hooking CreateWindowExA/W captures the created window without depending
    // on the probe's timer being scheduled. The visible-window poll above stays
    // as the fallback when these hooks cannot be installed.
    let windowHookState = 'unavailable';
    try {
      const user32 = Process.getModuleByName('user32.dll');
      ['CreateWindowExW', 'CreateWindowExA'].forEach(function (name) {
        Interceptor.attach(user32.getExportByName(name), {
          onLeave(retval) {
            try {
              observeWindow(ptr(retval), 'at-creation');
            } catch (error) {
              // Never throw from an interceptor callback.
            }
          }
        });
      });
      windowHookState = 'installed';
    } catch (error) {
      windowHookState = 'unavailable';
    }
    Interceptor.flush();
    return {
      installed: true,
      address,
      module: module.name,
      resolvedEntry: entry.toString(),
      resolvedState: state.toString(),
      windowHookState: windowHookState
    };
  }
};
