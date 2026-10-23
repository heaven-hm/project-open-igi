'use strict';
// Typed observation of two natives as one paired unit:
//   retail 0x004B5B30 Resource_CalculatePathHash (candidate __ZN3igi6retail3sys26Resource_CalculatePathHashEPKc)
//   retail 0x004659E0 Human_TaskViewReset      (candidate __ZN3igi6retail5human19Human_TaskViewResetEPv)
//
// Retail bytes (igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01):
//
// 0x004B5B30 Resource_CalculatePathHash
//   004b5b30  8b 54 24 04            mov edx, dword [esp+4]        ; cdecl argument: char* path (NUL terminated)
//   004b5b34  b8 01 00 00 00         mov eax, 1
//   004b5b39  8a 0a                  mov cl, byte [edx]
//   004b5b3b  84 c9                  test cl, cl
//   004b5b3d  74 0f                  je 0x004b5b4e               ; empty string leaves eax = 1
//   004b5b3f  0f be c9               movsbl ecx, cl              ; SIGNED char
//   004b5b42  40                     inc eax                     ; eax = eax + 1
//   004b5b43  0f af c1               imul eax, ecx               ; 32-bit wrapping multiply
//   004b5b46  8a 4a 01               mov cl, byte [edx+1]
//   004b5b49  42                     inc edx
//   004b5b4a  84 c9                  test cl, cl
//   004b5b4c  75 f1                  jne 0x004b5b3f
//   004b5b4e  33 d2                  xor edx, edx
//   004b5b50  b9 37 30 00 00         mov ecx, 0x3037
//   004b5b55  f7 f1                  div ecx                     ; unsigned edx:eax / 0x3037
//   004b5b57  8b c2                  mov eax, edx                ; EAX = remainder in [0, 0x3036]
//   004b5b59  c3                     ret
//   No writes, no calls: the only memory access is the NUL-terminated input string.
//
// 0x004659E0 Human_TaskViewReset
//   004659e0  dd 05 e8 35 53 00      fld qword [0x005335E8]      ; double 0x3FE921FB60000000 (float pi/4)
//   004659e6  d9 f2                  fptan
//   004659e8  8b 44 24 04            mov eax, dword [esp+4]      ; cdecl argument: void* human
//   004659ec  c7 80 ec 01 00 00 00   mov dword [eax+0x1EC], 0    ; aux field = 0
//   004659f3  00 00 00
//   004659f6  dd d8                  fstp st(0)                  ; discard the 1.0 pushed by fptan
//   004659f8  d9 98 e4 01 00 00      fstp dword [eax+0x1E4]      ; store tan() as f32
//   004659fe  dd 05 c0 39 53 00      fld qword [0x005339C0]      ; double 0x3FE0C15240000000 (float pi/6)
//   00465a04  d9 f2                  fptan
//   00465a06  dd d8                  fstp st(0)
//   00465a08  d9 98 e8 01 00 00      fstp dword [eax+0x1E8]      ; store tan() as f32
//   00465a0e  c3                     ret                          ; EAX is still the argument pointer
//   One basic block, no calls, exactly three 4-byte stores at +0x1E4/+0x1E8/+0x1EC.
//
// Reviewed contract asserted here:
//   hash: for every observed call EAX == retail_hash(string bytes) in [0, 0x3036]; the
//         input string is unchanged after the call; there is no other memory effect.
//   view: for every observed call the three reviewed dwords become
//         0x3F800000 / 0x3F13CD3A / 0x00000000, no other dword in the reviewed window
//         changes, and EAX == the argument pointer.
//
// Nothing here compares a raw address across processes. Addresses are used only inside
// one process: to follow the argument pointer and to classify the caller. The typed
// state carries the input string bytes, scalar field values, bit patterns, delta
// relations, changed-offset lists and per-process pointer relations (return == argument).
//
// Driver contract (scripts/frida_scoped_capture.py):
//   exports_sync.hook(address, layout) -> {installed: true, address, ...}
//   send({type: 'observed', address, typedState: {scope, inputs, outputs, sideEffects}})
//   send({type: 'call_census', address, scope, ...})
//   send({type: 'hook_failed', address, error})
//
// Argument reads are explicit and defensive: on this Frida ia32 endpoint the Interceptor
// `args` object itself can throw when measured or coerced (args.length and
// Object.prototype.toString.call(args) both throw), while explicit indices still resolve.
// Each cdecl word is read on its own, in its own try/catch, with an independent x86 stack
// read ([esp + 4 + 4*index], cdecl) as fallback, and the record says how each word was
// obtained or that it was unavailable. No args.length, no array methods, no iteration.

var HASH = '0x004B5B30';
var VIEW = '0x004659E0';
var SCOPE = 'path-hash-and-view-reset';
var UNAVAILABLE = 'unavailable';
var HASH_MODULUS = 0x3037;
var HASH_MAX_STRING_BYTES = 256;
var HASH_MAX_DISTINCT_INPUTS = 96;
// Retail resolves most resources through 'LOCAL:' aliases while a candidate may
// pass a fully qualified path. Retain both shapes so the shared-input comparison
// is not defeated by the alias-vs-absolute difference.
var HASH_MAX_OTHER_INPUTS = 24;
var HASH_MAX_DISTINCT_RETURNS = 64;
var VIEW_MAX_CALLS = 65536;
var HASH_MAX_CALLS = 262144;
var CENSUS_PERIOD_MS = 3000;

// Reviewed store constants for Human_TaskViewReset.
var VIEW_TANGENT_X_BITS = 0x3F800000;   // tan(0x3FE921FB60000000), narrowed to f32
var VIEW_TANGENT_VM_BITS = 0x3F13CD3A;  // tan(0x3FE0C15240000000), narrowed to f32
var VIEW_AUX_BITS = 0x00000000;
var VIEW_DEFAULT_WINDOW_START = 0x1C0;
var VIEW_DEFAULT_WINDOW_SIZE = 0x50;
var VIEW_DEFAULT_TANGENT_X = 0x1E4;
var VIEW_DEFAULT_TANGENT_VM = 0x1E8;
var VIEW_DEFAULT_AUX = 0x1EC;

function moduleFor(name) {
  var direct = Process.findModuleByName(name);
  if (direct !== null) return direct;
  var wanted = String(name).toLowerCase();
  var matches = Process.enumerateModules().filter(function (module) {
    return module.name.toLowerCase() === wanted;
  });
  if (matches.length !== 1) throw new Error('module not found: ' + name);
  return matches[0];
}

// Map a reviewed VA to this process. Never blind-read: the target range must be inside
// the resolved image.
function resolveImageVa(module, preferredBase, va, size) {
  var relative = ptr(va).sub(ptr(preferredBase));
  if (relative.compare(ptr(0)) < 0 || relative.compare(ptr(module.size - size)) > 0) {
    throw new Error('reviewed VA ' + va + ' is outside ' + module.name);
  }
  return module.base.add(relative);
}

function readArgument(args, context, index) {
  var value = null;
  var source = UNAVAILABLE;
  var errors = [];
  try {
    var candidate = args[index];
    if (candidate !== null && candidate !== undefined) {
      value = ptr(candidate);
      source = 'args[' + index + ']';
    }
  } catch (error) {
    errors.push(String(error));
  }
  if (value === null) {
    try {
      var stack = ptr(context.esp);
      if (stack.isNull()) throw new Error('stack pointer unavailable');
      value = stack.add(4 + index * 4).readPointer();
      source = 'stack[esp+' + (4 + index * 4) + ']';
    } catch (error) {
      errors.push(String(error));
    }
  }
  var marker = {index: index, state: value === null ? UNAVAILABLE : 'value', source: source};
  if (value === null && errors.length > 0) marker.error = errors.join('; ');
  return {marker: marker, value: value};
}

function signedWord(value) {
  try {
    if (value === null || value === undefined) return {state: UNAVAILABLE};
    var pointer = ptr(value);
    return {state: 'known', value: pointer.toInt32(), unsigned: pointer.toUInt32()};
  } catch (error) {
    return {state: UNAVAILABLE, error: String(error)};
  }
}

function readReturnAddress(context) {
  try {
    var stack = ptr(context.esp);
    if (stack.isNull()) return {state: UNAVAILABLE};
    return {state: 'known', value: stack.readPointer()};
  } catch (error) {
    return {state: UNAVAILABLE, error: String(error)};
  }
}

function hex32(value) {
  var text = (value >>> 0).toString(16);
  while (text.length < 8) text = '0' + text;
  return text;
}

function hexBytes(bytes) {
  var out = '';
  for (var i = 0; i < bytes.length; i += 1) {
    var part = (bytes[i] & 0xff).toString(16);
    if (part.length < 2) part = '0' + part;
    out += part;
  }
  return out;
}

function dwordAt(bytes, offset) {
  return (bytes[offset] & 0xff)
    | ((bytes[offset + 1] & 0xff) << 8)
    | ((bytes[offset + 2] & 0xff) << 16)
    | ((bytes[offset + 3] & 0xff) << 24);
}

function floatView(bits) {
  try {
    var scratch = Memory.alloc(4);
    scratch.writeU32(bits >>> 0);
    return scratch.readFloat();
  } catch (error) {
    return null;
  }
}

// Number of bytes readable from `address` without leaving the enclosing mapped range.
function readableBytes(address, wanted) {
  try {
    var range = Process.findRangeByAddress(address);
    if (range === null || range.protection.indexOf('r') < 0) return 0;
    var available = range.base.add(range.size).sub(address).toUInt32();
    return available < wanted ? available : wanted;
  } catch (error) {
    return 0;
  }
}

// Read a NUL-terminated byte string without ever crossing an unmapped boundary and
// without throwing. Returns an explicit unavailable marker instead of a partial guess.
function readCString(address, maxBytes) {
  var readable = readableBytes(address, maxBytes);
  if (readable <= 0) {
    return {state: UNAVAILABLE, readable_bytes: readable, error: 'argument pointer is not readable'};
  }
  try {
    var buffer = address.readByteArray(readable);
    var bytes = new Uint8Array(buffer);
    var length = 0;
    while (length < bytes.length && bytes[length] !== 0) length += 1;
    var text = '';
    for (var i = 0; i < length; i += 1) {
      var code = bytes[i];
      text += code >= 0x20 && code < 0x7f ? String.fromCharCode(code) : '.';
    }
    return {
      state: 'known',
      length: length,
      hex: hexBytes(bytes.subarray(0, length)),
      text: text,
      truncated: length === bytes.length,
      readable_bytes: readable
    };
  } catch (error) {
    return {state: UNAVAILABLE, readable_bytes: readable, error: String(error)};
  }
}

// Retail hash over the captured string bytes: acc = 1; per SIGNED byte
// c: acc = (acc + 1) * c (32-bit wrap); return acc % 0x3037 (unsigned).
function retailHash(bytes) {
  var acc = 1 >>> 0;
  for (var i = 0; i < bytes.length; i += 1) {
    var signed = bytes[i] >= 0x80 ? bytes[i] - 0x100 : bytes[i];
    acc = Math.imul((acc + 1) >>> 0, signed) >>> 0;
  }
  return (acc % HASH_MODULUS) >>> 0;
}

function hexToBytes(hex) {
  var bytes = new Uint8Array(hex.length / 2);
  for (var i = 0; i < bytes.length; i += 1) {
    bytes[i] = parseInt(hex.substr(i * 2, 2), 16);
  }
  return bytes;
}

function callerLabel(caller, callers) {
  if (caller.state !== 'known') return UNAVAILABLE;
  try {
    var address = caller.value;
    for (var i = 0; i < callers.length; i += 1) {
      if (address.equals(callers[i].address)) return callers[i].label;
    }
    return 'other';
  } catch (error) {
    return UNAVAILABLE;
  }
}

var installed = {};
var shared = {
  hash: {
    calls: 0, overflow: false, firstSent: false,
    distinct: [], distinctOverflow: false,
    other: [], otherOverflow: false,
    distinctReturns: [], allReturnsEqualExpected: true, allInputsUnchanged: true,
    firstCallIndex: null
  },
  view: {
    calls: 0, overflow: false, firstSent: false,
    distinctAfter: [], distinctChanged: [], allOnlyReviewedFields: true,
    allReturnEqualsArgument: true, allWindowsReadable: true
  },
  timer: null,
  moduleName: null,
  startedAt: Date.now()
};

function addDistinctString(state, hex, text, item) {
  var isLocalAlias = typeof text === 'string' && text.slice(0, 6).toUpperCase() === 'LOCAL:';
  var list = isLocalAlias ? state.distinct : state.other;
  var cap = isLocalAlias ? HASH_MAX_DISTINCT_INPUTS : HASH_MAX_OTHER_INPUTS;
  var overflowKey = isLocalAlias ? 'distinctOverflow' : 'otherOverflow';
  if (state[overflowKey] === true) return;
  for (var i = 0; i < list.length; i += 1) {
    if (list[i].hex === hex) return;
  }
  if (list.length >= cap) {
    state[overflowKey] = true;
    return;
  }
  list.push(item);
}

function addDistinctValue(list, value, cap) {
  if (list.length >= cap) return;
  if (list.indexOf(value) < 0) list.push(value);
}

function hashCensus(module) {
  var state = shared.hash;
  return {
    type: 'call_census',
    address: HASH,
    scope: SCOPE,
    module: module.name,
    observed_ms: Date.now() - shared.startedAt,
    call_count: state.calls,
    overflow: state.overflow,
    distinct_input_count: state.distinct.length,
    distinct_input_overflow: state.distinctOverflow,
    distinct_other_count: state.other.length,
    distinct_other_overflow: state.otherOverflow,
    distinct_return_count: state.distinctReturns.length,
    distinct_returns: state.distinctReturns.slice(),
    all_returns_equal_expected: state.allReturnsEqualExpected,
    all_inputs_unchanged: state.allInputsUnchanged,
    first_call_index: state.firstCallIndex,
    distinct_inputs: state.distinct,
    distinct_other_inputs: state.other
  };
}

function viewCensus(module) {
  var state = shared.view;
  return {
    type: 'call_census',
    address: VIEW,
    scope: SCOPE,
    module: module.name,
    observed_ms: Date.now() - shared.startedAt,
    call_count: state.calls,
    overflow: state.overflow,
    distinct_after_count: state.distinctAfter.length,
    distinct_after: state.distinctAfter.slice(),
    distinct_changed_offsets_count: state.distinctChanged.length,
    distinct_changed_offsets: state.distinctChanged.slice(),
    all_only_reviewed_fields_changed: state.allOnlyReviewedFields,
    all_return_equals_argument: state.allReturnEqualsArgument,
    all_windows_readable: state.allWindowsReadable
  };
}

rpc.exports = {
  hook: function (address, layout) {
    if (address !== HASH && address !== VIEW) {
      throw new Error('unsupported native: ' + address);
    }
    if (Process.pointerSize !== 4) throw new Error('x86 pointer ABI required');
    if (typeof layout !== 'object' || layout === null) throw new Error('missing layout');
    if (typeof layout.moduleName !== 'string' || !layout.moduleName) {
      throw new Error('missing moduleName');
    }
    if (typeof layout.preferredBase !== 'string' || typeof layout.entryVA !== 'string') {
      throw new Error('layout needs preferredBase and entryVA');
    }
    if (installed[address] === true) throw new Error('native already hooked: ' + address);

    var module = moduleFor(layout.moduleName);
    shared.moduleName = module.name;
    var entry = resolveImageVa(module, layout.preferredBase, layout.entryVA, 1);

    var callers = [];
    if (Object.prototype.toString.call(layout.callers) === '[object Array]') {
      for (var index = 0; index < layout.callers.length; index += 1) {
        var row = layout.callers[index];
        if (row && typeof row.returnVA === 'string' && typeof row.label === 'string') {
          callers.push({
            address: resolveImageVa(module, layout.preferredBase, row.returnVA, 1),
            label: row.label
          });
        }
      }
    }

    var windowStart = typeof layout.windowStartOffset === 'number'
      ? layout.windowStartOffset : VIEW_DEFAULT_WINDOW_START;
    var windowSize = typeof layout.windowSize === 'number'
      ? layout.windowSize : VIEW_DEFAULT_WINDOW_SIZE;
    var tangentX = typeof layout.tangentXOffset === 'number'
      ? layout.tangentXOffset : VIEW_DEFAULT_TANGENT_X;
    var tangentVm = typeof layout.tangentVmOffset === 'number'
      ? layout.tangentVmOffset : VIEW_DEFAULT_TANGENT_VM;
    var aux = typeof layout.auxOffset === 'number' ? layout.auxOffset : VIEW_DEFAULT_AUX;
    var maxStringBytes = typeof layout.maxStringBytes === 'number'
      ? layout.maxStringBytes : HASH_MAX_STRING_BYTES;

    installed[address] = true;

    if (address === HASH) {
      Interceptor.attach(entry, {
        onEnter: function (args) {
          try {
            var caller = readReturnAddress(this.context);
            var argument = readArgument(args, this.context, 0);
            var state = shared.hash;
            var index = state.calls;
            if (index >= HASH_MAX_CALLS) {
              state.overflow = true;
              this.skip = true;
              return;
            }
            this.record = {
              index: index,
              caller: caller,
              callerClass: callerLabel(caller, callers),
              argument: argument,
              string: argument.value === null
                ? {state: UNAVAILABLE, error: 'argument unavailable'}
                : readCString(argument.value, maxStringBytes)
            };
            this.failed = null;
          } catch (error) {
            this.failed = String(error);
          }
        },
        onLeave: function (returned) {
          if (this.skip === true) return;
          if (this.failed !== null && this.failed !== undefined) {
            send({type: 'hook_failed', address: address, error: this.failed});
            return;
          }
          try {
            var record = this.record;
            var state = shared.hash;
            state.calls += 1;
            var returnedWord = signedWord(returned);
            var stringKnown = record.string.state === 'known';
            var expected = stringKnown ? retailHash(hexToBytes(record.string.hex)) : null;
            var returnInRange = returnedWord.state === 'known'
              ? returnedWord.unsigned >= 0 && returnedWord.unsigned < HASH_MODULUS : null;
            var returnEqualsExpected = stringKnown && returnedWord.state === 'known'
              ? returnedWord.unsigned === expected : null;

            // The only memory this native touches is the input string: re-read it and
            // prove it was not mutated.
            var unchanged = null;
            if (stringKnown && record.argument.value !== null) {
              var again = readCString(record.argument.value, record.string.readable_bytes);
              unchanged = again.state === 'known' ? again.hex === record.string.hex : null;
            }

            if (state.firstCallIndex === null) state.firstCallIndex = record.index;
            if (returnEqualsExpected !== true) state.allReturnsEqualExpected = false;
            if (unchanged !== true) state.allInputsUnchanged = false;
            if (returnedWord.state === 'known') {
              addDistinctValue(state.distinctReturns, returnedWord.unsigned, HASH_MAX_DISTINCT_RETURNS);
            }
            if (stringKnown) {
              addDistinctString(state, record.string.hex, record.string.text, {
                hex: record.string.hex,
                text: record.string.text,
                length: record.string.length,
                truncated: record.string.truncated,
                return_value: returnedWord.state === 'known' ? returnedWord.unsigned : null,
                expected_hash: expected,
                return_equals_expected: returnEqualsExpected
              });
            }

            if (!state.firstSent) {
              state.firstSent = true;
              var typedState = {
                scope: SCOPE,
                inputs: {
                  caller_class: record.callerClass,
                  caller_state: record.caller.state,
                  argument_state: record.argument.marker.state,
                  argument_read_source: record.argument.marker.source,
                  argument_reads: [record.argument.marker],
                  string_state: record.string.state,
                  string_hex: stringKnown ? record.string.hex : null,
                  string_text: stringKnown ? record.string.text : null,
                  string_length: stringKnown ? record.string.length : null,
                  string_truncated: stringKnown ? record.string.truncated : null
                },
                outputs: {
                  return_state: returnedWord.state,
                  return_value: returnedWord.state === 'known' ? returnedWord.unsigned : null,
                  return_in_range: returnInRange,
                  return_equals_retail_formula: returnEqualsExpected,
                  expected_hash_state: expected === null ? UNAVAILABLE : 'known',
                  expected_hash: expected
                },
                sideEffects: {
                  input_string_unchanged_after_call: unchanged,
                  reviewed_no_calls: true,
                  reviewed_no_stores: true,
                  write_target_count: 0
                }
              };
              send({type: 'observed', address: address, typedState: typedState});
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
        callerCount: callers.length,
        window: null
      };
    }

    // Human_TaskViewReset: snapshot a window that contains the three reviewed fields,
    // call, then snapshot again. Exactly three dwords may change.
    if ((windowStart & 3) !== 0 || (windowSize & 3) !== 0 || windowSize < 16) {
      throw new Error('view window must be 4-byte aligned and at least 16 bytes');
    }
    if (tangentX < windowStart || tangentX + 4 > windowStart + windowSize
        || tangentVm < windowStart || tangentVm + 4 > windowStart + windowSize
        || aux < windowStart || aux + 4 > windowStart + windowSize) {
      throw new Error('reviewed fields must lie inside the snapshot window');
    }

    Interceptor.attach(entry, {
      onEnter: function (args) {
        try {
          var caller = readReturnAddress(this.context);
          var argument = readArgument(args, this.context, 0);
          var state = shared.view;
          var index = state.calls;
          if (index >= VIEW_MAX_CALLS) {
            state.overflow = true;
            this.skip = true;
            return;
          }
          var before = null;
          var readable = 0;
          if (argument.value !== null) {
            readable = readableBytes(argument.value.add(windowStart), windowSize);
            if (readable >= windowSize) {
              try {
                before = new Uint8Array(argument.value.add(windowStart).readByteArray(windowSize));
              } catch (error) {
                before = null;
              }
            }
          }
          this.record = {
            index: index,
            caller: caller,
            callerClass: callerLabel(caller, callers),
            argument: argument,
            before: before,
            readable: readable
          };
          this.failed = null;
        } catch (error) {
          this.failed = String(error);
        }
      },
      onLeave: function (returned) {
        if (this.skip === true) return;
        if (this.failed !== null && this.failed !== undefined) {
          send({type: 'hook_failed', address: address, error: this.failed});
          return;
        }
        try {
          var record = this.record;
          var state = shared.view;
          state.calls += 1;
          var returnedWord = signedWord(returned);
          var after = null;
          if (record.before !== null) {
            try {
              after = new Uint8Array(record.argument.value.add(windowStart).readByteArray(windowSize));
            } catch (error) {
              after = null;
            }
          }

          var returnEqualsArgument = null;
          if (record.argument.value !== null && returnedWord.state === 'known') {
            try {
              returnEqualsArgument = returnedWord.unsigned === record.argument.value.toUInt32();
            } catch (error) {
              returnEqualsArgument = null;
            }
          }
          if (returnEqualsArgument !== true) state.allReturnEqualsArgument = false;
          if (record.before === null) state.allWindowsReadable = false;

          var changedOffsets = [];
          var storedX = null;
          var storedVm = null;
          var storedAux = null;
          var onlyReviewed = null;
          if (record.before !== null && after !== null) {
            for (var offset = 0; offset < windowSize; offset += 4) {
              var beforeWord = dwordAt(record.before, offset);
              var afterWord = dwordAt(after, offset);
              if (beforeWord !== afterWord) changedOffsets.push(windowStart + offset);
              if (windowStart + offset === tangentX) storedX = afterWord;
              if (windowStart + offset === tangentVm) storedVm = afterWord;
              if (windowStart + offset === aux) storedAux = afterWord;
            }
            // A field that already held its target value is not a change: the contract
            // is "every changed dword is one of the three reviewed fields, and every
            // reviewed field ends at its reviewed value", not "all three differ".
            var reviewedSet = [tangentX, tangentVm, aux];
            var onlyReviewedOffsets = true;
            for (var c = 0; c < changedOffsets.length; c += 1) {
              if (reviewedSet.indexOf(changedOffsets[c]) < 0) onlyReviewedOffsets = false;
            }
            onlyReviewed = onlyReviewedOffsets
              && storedX !== null && (storedX >>> 0) === VIEW_TANGENT_X_BITS
              && storedVm !== null && (storedVm >>> 0) === VIEW_TANGENT_VM_BITS
              && storedAux !== null && (storedAux >>> 0) === VIEW_AUX_BITS;
          }

          if (storedX !== null) {
            addDistinctValue(state.distinctAfter,
              hex32(storedX) + ':' + hex32(storedVm) + ':' + hex32(storedAux), 16);
          }
          addDistinctValue(state.distinctChanged, changedOffsets.join(','), 16);
          if (onlyReviewed !== true) state.allOnlyReviewedFields = false;

          if (!state.firstSent) {
            state.firstSent = true;
            var beforeKnown = record.before !== null;
            var previousX = beforeKnown ? dwordAt(record.before, tangentX - windowStart) : null;
            var previousVm = beforeKnown ? dwordAt(record.before, tangentVm - windowStart) : null;
            var previousAux = beforeKnown ? dwordAt(record.before, aux - windowStart) : null;
            send({
              type: 'observed',
              address: address,
              typedState: {
                scope: SCOPE,
                inputs: {
                  caller_class: record.callerClass,
                  caller_state: record.caller.state,
                  argument_state: record.argument.marker.state,
                  argument_read_source: record.argument.marker.source,
                  argument_reads: [record.argument.marker],
                  previous_tangent_x_state: beforeKnown ? 'known' : UNAVAILABLE,
                  previous_tangent_x_bits: previousX,
                  previous_tangent_x_float: previousX === null ? null : floatView(previousX),
                  previous_tangent_vm_state: beforeKnown ? 'known' : UNAVAILABLE,
                  previous_tangent_vm_bits: previousVm,
                  previous_tangent_vm_float: previousVm === null ? null : floatView(previousVm),
                  previous_aux_state: beforeKnown ? 'known' : UNAVAILABLE,
                  previous_aux_bits: previousAux,
                  window_offset: windowStart,
                  window_size: windowSize
                },
                outputs: {
                  return_state: returnedWord.state,
                  return_equals_argument: returnEqualsArgument,
                  return_is_null: returnedWord.state === 'known' ? returnedWord.value === 0 : null
                },
                sideEffects: {
                  stored_tangent_x_bits: storedX,
                  stored_tangent_vm_bits: storedVm,
                  stored_aux_bits: storedAux,
                  stored_tangent_x_equals_reviewed: storedX === null ? null : (storedX >>> 0) === VIEW_TANGENT_X_BITS,
                  stored_tangent_vm_equals_reviewed: storedVm === null ? null : (storedVm >>> 0) === VIEW_TANGENT_VM_BITS,
                  stored_aux_is_zero: storedAux === null ? null : (storedAux >>> 0) === VIEW_AUX_BITS,
                  tangent_x_changed: previousX === null ? null : previousX !== storedX,
                  tangent_vm_changed: previousVm === null ? null : previousVm !== storedVm,
                  aux_changed: previousAux === null ? null : previousAux !== storedAux,
                  changed_dword_offsets: changedOffsets,
                  changed_dword_count: changedOffsets.length,
                  only_reviewed_fields_changed: onlyReviewed,
                  window_unchanged_outside_reviewed_fields: onlyReviewed,
                  reviewed_no_calls: true
                }
              }
            });
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
      callerCount: callers.length,
      window: {start: windowStart, size: windowSize,
               tangentX: tangentX, tangentVm: tangentVm, aux: aux}
    };
  }
};

// Cumulative census for both natives, refreshed on a timer so no call is silently
// dropped even when the process is torn down at the end of the observation window.
shared.timer = setInterval(function () {
  try {
    // The module name is per side (igi.exe / the staged candidate name); resolve it by
    // enumeration so the census echoes the real image name and never guesses.
    var names = Process.enumerateModules();
    var chosen = null;
    for (var i = 0; i < names.length; i += 1) {
      if (shared.moduleName !== null && names[i].name === shared.moduleName) {
        chosen = names[i];
        break;
      }
    }
    if (chosen === null) return;
    if (installed[HASH] === true) send(hashCensus(chosen));
    if (installed[VIEW] === true) send(viewCensus(chosen));
  } catch (error) {
    // Never throw from a timer callback.
  }
}, CENSUS_PERIOD_MS);
