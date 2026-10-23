'use strict';
// Typed observation of the two app-context accessors as one paired unit:
//   retail 0x0048F140 AppContext_SetIsActive  (candidate _AppContext_SetIsActive)
//   retail 0x0048F170 AppContext_GetIsActive  (candidate _AppContext_GetIsActive)
//
// Retail bytes (igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01):
//   0x0048F140  8b 44 24 04      mov eax, dword [esp+4]        ; cdecl argument (int32)
//   0x0048F144  0f be c8         movsx ecx, al                 ; sign-extend the low byte
//   0x0048F147  89 0d fc 8b 5c 00 mov dword [0x005C8BFC], ecx ; the isActive dword
//   0x0048F14D  8b 0d 18 8c 5c 00 mov ecx, dword [0x005C8C18]   ; optional handler
//   0x0048F153  85 c9            test ecx, ecx
//   0x0048F155  74 0d            je 0x0048F164
//   0x0048F157  8b 15 1c 8c 5c 00 mov edx, dword [0x005C8C1C]   ; handler context
//   0x0048F15D  52               push edx
//   0x0048F15E  50               push eax
//   0x0048F15F  ff d1            call ecx                     ; handler(value, context), cdecl
//   0x0048F161  83 c4 08         add esp, 8
//   0x0048F164  c3               ret                          ; EAX = handler result, else the argument bits
//   0x0048F170  a1 fc 8b 5c 00   mov eax, dword [0x005C8BFC]
//   0x0048F175  c3               ret
//
// Contract observed here: the setter's only state delta is the dword store of
// sign_extend_8(argument[7:0]); when the optional handler word is null the
// return is the argument's 32-bit value, otherwise the handler's return; the
// getter is a pure load of that same dword. The 0x005C8BFC/0x005C8C18/
// 0x005C8C1C dwords are zero-filled .data (no file-backed initialiser) and the
// only code that reads or writes them is 0x0048F140..0x0048F1A0; the installer
// 0x0048F180 has no callers, so the handler is expected null.
//
// Nothing here compares a raw address across processes. The typed state carries
// scalar flag values, the sign-extension relation, the store delta on the
// reviewed state VA, pointer classes (null / non-null), the caller class
// (this side's reviewed startup callsite versus any other caller), and the
// getter's load relation. Caller classification uses the per-side
// `primaryCallerReturnVA` from the reviewed layout; it never compares those
// addresses across processes.
//
// One `observed` record per native is sent for the first reviewed-callsite
// (primary) invocation: that is the startup setter call and the first
// frame-loop getter call. Every other invocation is still observed and is
// reported through `setter_aux` / aggregate `getter_sequence` records, so no
// call is silently dropped; the aggregate is emitted on a timer and carries
// the cumulative sequence relations.
//
// Driver contract (scripts/frida_scoped_capture.py):
//   exports_sync.hook(address, layout) -> {installed: true, address, ...}
//   send({type: 'observed', address, typedState: {scope, inputs, outputs, sideEffects}})
//   send({type: 'setter_aux' | 'getter_sequence', address, scope, ...})
//   send({type: 'hook_failed', address, error})
//
// Argument reads are explicit and defensive: on this Frida ia32 endpoint the
// Interceptor `args` object itself can throw when measured or coerced
// (args.length and Object.prototype.toString.call(args) both throw), while
// explicit indices still resolve. So each cdecl word is read on its own, in
// its own try/catch, with an independent x86 stack read
// ([esp + 4 + 4*index], cdecl) as fallback, and the record says how each word
// was obtained or that it was unavailable. No args.length, no array methods,
// no iteration of args.

const SETTER = '0x0048F140';
const GETTER = '0x0048F170';
const SCOPE = 'startup-app-context-active-flag';
const ARG_WORDS = 4;              // cdecl words inspected per setter call, probe-side bound
const MAX_SETTER_CALLS = 64;      // per-side bound; overflow is reported, never truncated silently
const MAX_GETTER_CALLS = 8192;    // per-side bound; overflow is reported, never truncated silently
const SEQUENCE_PERIOD_MS = 250;   // cumulative getter aggregate refresh
const UNAVAILABLE = 'unavailable';

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
  const errors = [];
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
  const marker = {index: index, state: value === null ? UNAVAILABLE : 'value', source: source};
  if (value === null && errors.length > 0) marker.error = errors.join('; ');
  return {marker: marker, value: value};
}

// Read one dword of reviewed state. Never throws: unavailable is an explicit
// marker in the typed record, never a zero value.
function readDword(address) {
  try {
    return {state: 'known', value: address.readS32(), unsigned: address.readU32()};
  } catch (error) {
    return {state: UNAVAILABLE, error: String(error)};
  }
}

function readPointer(address) {
  try {
    return {state: 'known', value: address.readPointer()};
  } catch (error) {
    return {state: UNAVAILABLE, error: String(error)};
  }
}

// Signed 32-bit view of an Interceptor return value or a read pointer. Never
// throws; returns an explicit unavailable marker instead.
function signedWord(value) {
  try {
    if (value === null || value === undefined) return {state: UNAVAILABLE};
    const pointer = ptr(value);
    return {state: 'known', value: pointer.toInt32(), unsigned: pointer.toUInt32()};
  } catch (error) {
    return {state: UNAVAILABLE, error: String(error)};
  }
}

function signExtendedLowByte(unsigned) {
  const low = unsigned & 0xff;
  return low >= 0x80 ? low - 0x100 : low;
}

function pointerClass(read) {
  if (read.state !== 'known') return UNAVAILABLE;
  return read.value.isNull() ? 'null' : 'non-null-pointer';
}

// The caller word on the x86 stack at entry is the return address. Reading it
// needs only the Interceptor context; no argument object is touched.
function readReturnAddress(context) {
  try {
    const stack = ptr(context.esp);
    if (stack.isNull()) return {state: UNAVAILABLE};
    return {state: 'known', value: stack.readPointer()};
  } catch (error) {
    return {state: UNAVAILABLE, error: String(error)};
  }
}

// Module-level per-process state shared by both hooks: exactly one observed
// record per native, the ordering relation between the pair, and the
// cumulative sequence counters for the always-called getter.
const installedAddresses = {};

const shared = {
  primarySetterSeen: false,
  primarySetterStored: null,
  primarySetterSent: false,
  primaryGetterSent: false,
  setterCalls: 0,
  setterOverflow: false,
  getterCalls: 0,
  getterPrimaryCalls: 0,
  getterOtherCalls: 0,
  getterOverflow: false,
  getterFirstPrimaryAfterSetter: null,
  getterPrimaryReturns: [],
  getterOtherReturns: [],
  getterAllReturns: [],
  getterAllReturnEqualsFlagBefore: true,
  getterAllFlagUnchanged: true,
  startedAt: Date.now(),
  sequenceTimer: null
};

function addDistinct(list, value) {
  if (list.indexOf(value) < 0) list.push(value);
}

function sortedValues(list) {
  return list.slice().sort(function (a, b) { return a - b; });
}

function getterSequenceRecord(module, finalFlag) {
  return {
    type: 'getter_sequence',
    address: GETTER,
    scope: SCOPE,
    module: module.name,
    observed_ms: Date.now() - shared.startedAt,
    call_count: shared.getterCalls,
    overflow: shared.getterOverflow,
    primary_call_count: shared.getterPrimaryCalls,
    other_call_count: shared.getterOtherCalls,
    distinct_primary_returns: sortedValues(shared.getterPrimaryReturns),
    distinct_other_returns: sortedValues(shared.getterOtherReturns),
    distinct_all_returns: sortedValues(shared.getterAllReturns),
    all_returns_equal_flag_before: shared.getterAllReturnEqualsFlagBefore,
    all_calls_left_flag_unchanged: shared.getterAllFlagUnchanged,
    first_primary_call_after_primary_setter: shared.getterFirstPrimaryAfterSetter,
    final_flag_state: finalFlag.state,
    final_flag_value: finalFlag.state === 'known' ? finalFlag.value : null
  };
}

rpc.exports = {
  hook(address, layout) {
    if (address !== SETTER && address !== GETTER) {
      throw new Error('unsupported native: ' + address);
    }
    if (Process.pointerSize !== 4) throw new Error('x86 pointer ABI required');
    if (typeof layout !== 'object' || layout === null) throw new Error('missing layout');
    if (typeof layout.moduleName !== 'string' || !layout.moduleName) {
      throw new Error('missing moduleName');
    }
    if (typeof layout.preferredBase !== 'string' || typeof layout.entryVA !== 'string'
        || typeof layout.stateVA !== 'string') {
      throw new Error('layout needs preferredBase, entryVA and stateVA');
    }
    // The reviewed callsite return address is strongly preferred: it proves the
    // invocation is the reviewed call and not some other caller of the same
    // native. When a caller path (for example the strict gate driver's manifest
    // runtime_layout, whose candidate values must all be symbols) cannot supply
    // it, fall back to classifying the first invocation as the primary one and
    // say so in the hook result; the compared caller_class stays the same.
    const classifyByReturnAddress = typeof layout.primaryCallerReturnVA === 'string'
        && layout.primaryCallerReturnVA.length > 0;

    if (installedAddresses[address] === true) {
      throw new Error('native already hooked: ' + address);
    }
    const module = moduleFor(layout.moduleName);
    const entry = resolveImageVa(module, layout.preferredBase, layout.entryVA, 1);
    const state = resolveImageVa(module, layout.preferredBase, layout.stateVA, 4);
    const primaryCaller = classifyByReturnAddress
      ? resolveImageVa(module, layout.preferredBase, layout.primaryCallerReturnVA, 1)
      : null;

    // The optional handler and its context belong to the setter's contract. They
    // are resolved when the layout supplies them and are unavailable otherwise.
    let handler = null;
    let handlerContext = null;
    if (typeof layout.handlerVA === 'string' && layout.handlerVA) {
      handler = resolveImageVa(module, layout.preferredBase, layout.handlerVA, 4);
    }
    if (typeof layout.handlerContextVA === 'string' && layout.handlerContextVA) {
      handlerContext = resolveImageVa(module, layout.preferredBase, layout.handlerContextVA, 4);
    }

    function classify(caller, index) {
      if (primaryCaller === null) {
        // No reviewed callsite word: the first invocation is the primary one.
        return index === 0 ? 'primary' : 'other';
      }
      if (caller.state !== 'known') return UNAVAILABLE;
      try {
        return caller.value.equals(primaryCaller) ? 'primary' : 'other';
      } catch (error) {
        return UNAVAILABLE;
      }
    }

    installedAddresses[address] = true;

    if (address === SETTER) {
      Interceptor.attach(entry, {
        onEnter(args) {
          try {
            const caller = readReturnAddress(this.context);
            const reads = [];
            let argument = null;
            for (let index = 0; index < ARG_WORDS; ++index) {
              const read = readArgument(args, this.context, index);
              reads.push(read.marker);
              if (index === 0) argument = read.value;
            }
            const index = shared.setterCalls;
            shared.setterCalls += 1;
            if (index >= MAX_SETTER_CALLS) {
              shared.setterOverflow = true;
              this.skip = true;
              return;
            }
            this.record = {
              index: index,
              caller: classify(caller, index),
              callerState: caller.state,
              argument: argument,
              argumentReads: reads,
              previous: readDword(state),
              handler: handler === null ? {state: UNAVAILABLE} : readPointer(handler),
              handlerContext: handlerContext === null
                ? {state: UNAVAILABLE} : readPointer(handlerContext)
            };
            this.failed = null;
            if (this.record.caller === 'primary') shared.primarySetterSeen = true;
          } catch (error) {
            this.failed = String(error);
          }
        },
        onLeave(returned) {
          if (this.skip === true) return;
          if (this.failed !== null && this.failed !== undefined) {
            send({type: 'hook_failed', address: address, error: this.failed});
            return;
          }
          try {
            const record = this.record;
            const stored = readDword(state);
            const returnedWord = signedWord(returned);
            const previousKnown = record.previous.state === 'known';
            const storedKnown = stored.state === 'known';
            const argumentKnown = record.argument !== null;
            const argumentUnsigned = argumentKnown ? record.argument.toUInt32() : null;
            const extended = argumentKnown ? signExtendedLowByte(argumentUnsigned) : null;
            const handlerNull = record.handler.state === 'known'
              ? record.handler.value.isNull() : null;
            const handlerContextNull = record.handlerContext.state === 'known'
              ? record.handlerContext.value.isNull() : null;
            const returnAliasesArgument = argumentKnown && returnedWord.state === 'known'
              ? returnedWord.value === (argumentUnsigned | 0) : null;
            const returnAliasesStored = storedKnown && returnedWord.state === 'known'
              ? returnedWord.value === stored.value : null;
            const storedEqualsExtended = storedKnown && extended !== null
              ? stored.value === extended : null;
            const storeChanged = previousKnown && storedKnown
              ? record.previous.value !== stored.value : null;

            const typedState = {
              scope: SCOPE,
              inputs: {
                caller_class: record.caller,
                caller_state: record.callerState,
                is_primary_startup_call: record.caller === 'primary',
                argument_state: argumentKnown ? 'known' : UNAVAILABLE,
                argument_bits: argumentKnown ? (argumentUnsigned | 0) : null,
                argument_low_byte: argumentKnown ? (argumentUnsigned & 0xff) : null,
                argument_sign_extended_low_byte: extended,
                argument_is_zero: argumentKnown ? argumentUnsigned === 0 : null,
                argument_reads: record.argumentReads
              },
              outputs: {
                return_state: returnedWord.state,
                return_bits: returnedWord.state === 'known' ? returnedWord.value : null,
                return_equals_argument: argumentKnown ? returnAliasesArgument : null,
                return_is_zero: returnedWord.state === 'known' ? returnedWord.value === 0 : null,
                return_equals_stored: storedKnown ? returnAliasesStored : null
              },
              sideEffects: {
                previous_flag_state: record.previous.state,
                previous_flag_value: previousKnown ? record.previous.value : null,
                previous_flag_is_zero: previousKnown ? record.previous.value === 0 : null,
                stored_flag_state: stored.state,
                stored_flag_value: storedKnown ? stored.value : null,
                stored_equals_sign_extended_low_byte: storedEqualsExtended,
                store_changed: storeChanged,
                handler_state: record.handler.state,
                handler_is_null: handlerNull,
                handler_context_state: record.handlerContext.state,
                handler_context_is_null: handlerContextNull,
                flag_readback_after_state: stored.state,
                flag_readback_after_value: storedKnown ? stored.value : null,
                store_visible_after_return: storedKnown
              }
            };

            if (record.caller === 'primary' && !shared.primarySetterSent) {
              shared.primarySetterSent = true;
              shared.primarySetterStored = storedKnown ? stored.value : null;
              send({type: 'observed', address: address, typedState: typedState});
            } else {
              send({
                type: 'setter_aux',
                address: address,
                scope: SCOPE,
                call: {
                  index: record.index,
                  caller_class: record.caller,
                  argument_bits: argumentKnown ? (argumentUnsigned | 0) : null,
                  previous_flag_value: previousKnown ? record.previous.value : null,
                  stored_flag_value: storedKnown ? stored.value : null,
                  stored_equals_sign_extended_low_byte: storedEqualsExtended,
                  store_changed: storeChanged,
                  handler_is_null: handlerNull,
                  return_bits: returnedWord.state === 'known' ? returnedWord.value : null
                }
              });
            }
            if (record.caller === 'primary') {
              shared.primarySetterStored = storedKnown ? stored.value : null;
            }
          } catch (error) {
            send({type: 'hook_failed', address: address, error: String(error)});
          }
        }
      });
      Interceptor.flush();
      return {
        installed: true,
        address: address,
        module: module.name,
        resolvedEntry: entry.toString(),
        resolvedState: state.toString(),
        resolvedHandler: handler === null ? null : handler.toString(),
        resolvedPrimaryCaller: primaryCaller === null ? null : primaryCaller.toString(),
        callerClassification: classifyByReturnAddress
          ? 'reviewed-return-address' : 'first-invocation-fallback'
      };
    }

    // Getter: a pure load of the same dword. The first reviewed-callsite
    // invocation is the one paired `observed` record; every other call feeds
    // the cumulative aggregate so the whole sequence is accounted for.
    Interceptor.attach(entry, {
      onEnter() {
        try {
          const caller = readReturnAddress(this.context);
          const index = shared.getterCalls;
          shared.getterCalls += 1;
          if (index >= MAX_GETTER_CALLS) {
            shared.getterOverflow = true;
            this.skip = true;
            return;
          }
          const classified = classify(caller, index);
          if (classified === 'primary') shared.getterPrimaryCalls += 1;
          else shared.getterOtherCalls += 1;
          this.record = {
            index: index,
            caller: classified,
            before: readDword(state)
          };
          this.failed = null;
        } catch (error) {
          this.failed = String(error);
        }
      },
      onLeave(returned) {
        if (this.skip === true) return;
        if (this.failed !== null && this.failed !== undefined) {
          send({type: 'hook_failed', address: address, error: this.failed});
          return;
        }
        try {
          const record = this.record;
          const after = readDword(state);
          const returnedWord = signedWord(returned);
          const beforeKnown = record.before.state === 'known';
          const afterKnown = after.state === 'known';
          const returnedKnown = returnedWord.state === 'known';
          const equalsBefore = beforeKnown && returnedKnown
            ? returnedWord.value === record.before.value : null;
          const unchanged = beforeKnown && afterKnown
            ? record.before.value === after.value : null;

          if (record.caller === 'primary' && shared.getterFirstPrimaryAfterSetter === null) {
            shared.getterFirstPrimaryAfterSetter =
              shared.primarySetterSeen && shared.primarySetterStored !== null;
          }
          if (returnedKnown) {
            addDistinct(shared.getterAllReturns, returnedWord.value);
            if (record.caller === 'primary') {
              addDistinct(shared.getterPrimaryReturns, returnedWord.value);
            } else {
              addDistinct(shared.getterOtherReturns, returnedWord.value);
            }
          } else {
            shared.getterAllReturnEqualsFlagBefore = false;
          }
          if (equalsBefore !== true) shared.getterAllReturnEqualsFlagBefore = false;
          if (unchanged !== true) shared.getterAllFlagUnchanged = false;

          if (record.caller === 'primary' && !shared.primaryGetterSent) {
            shared.primaryGetterSent = true;
            const storedFromSetter = shared.primarySetterStored;
            const equalsSetterStore = returnedKnown && storedFromSetter !== null
              ? returnedWord.value === storedFromSetter : null;
            send({
              type: 'observed',
              address: address,
              typedState: {
                scope: SCOPE,
                inputs: {
                  caller_class: record.caller,
                  is_primary_frame_loop_call: true,
                  flag_before_state: record.before.state,
                  flag_before_value: beforeKnown ? record.before.value : null,
                  flag_before_is_zero: beforeKnown ? record.before.value === 0 : null,
                  primary_setter_observed_before: shared.primarySetterSeen,
                  setter_stored_state: shared.primarySetterStored === null
                    ? UNAVAILABLE : 'known',
                  setter_stored_value: shared.primarySetterStored,
                  flag_before_equals_setter_stored: beforeKnown && shared.primarySetterStored !== null
                    ? record.before.value === shared.primarySetterStored : null
                },
                outputs: {
                  return_state: returnedWord.state,
                  return_bits: returnedKnown ? returnedWord.value : null,
                  return_equals_flag_before: equalsBefore,
                  return_is_zero: returnedKnown ? returnedWord.value === 0 : null,
                  return_equals_setter_stored: equalsSetterStore
                },
                sideEffects: {
                  flag_after_state: after.state,
                  flag_after_value: afterKnown ? after.value : null,
                  flag_unchanged_during_call: unchanged,
                  getter_wrote_nothing: unchanged
                }
              }
            });
          }
          // Every other call is accounted for in the cumulative aggregate
          // emitted by the timer: per-call messages are not needed, and no
          // call is dropped from the sequence counters.
        } catch (error) {
          send({type: 'hook_failed', address: address, error: String(error)});
        }
      }
    });
    Interceptor.flush();

    if (shared.sequenceTimer === null) {
      shared.sequenceTimer = setInterval(function () {
        try {
          send(getterSequenceRecord(module, readDword(state)));
        } catch (error) {
          // Never throw from a timer callback.
        }
      }, SEQUENCE_PERIOD_MS);
    }

    return {
      installed: true,
      address: address,
      module: module.name,
      resolvedEntry: entry.toString(),
      resolvedState: state.toString(),
      resolvedPrimaryCaller: primaryCaller === null ? null : primaryCaller.toString(),
      callerClassification: classifyByReturnAddress
        ? 'reviewed-return-address' : 'first-invocation-fallback'
    };
  }
};
