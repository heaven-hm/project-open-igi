'use strict';
// Typed observation for retail 0x0048F0C0 (StoreCommandLine) and the staged
// candidate's equivalent entry. Retail bytes 0x0048F0C0..0x0048F0CA:
//   8b442404    mov eax, dword [esp+4]      ; cdecl argument: WinMain's lpCmdLine
//   a3c88b5c00  mov dword [0x005C8BC8], eax ; the command-line global
//   c3          ret                         ; EAX still holds the argument bits
// One basic block, no calls, no other memory access; the only side effect is
// the 4-byte store to the reviewed state VA.
//
// A pointer value means different things in different processes, so nothing
// here reports a raw address as data: inputs/outputs/sideEffects carry the
// text the pointer designates plus typed relations between the retail-scope
// bits (argument / stored word / return value). Unreadable or null data is
// reported as such, never guessed.
//
// Driver contract (scripts/frida_scoped_capture.py):
//   exports_sync.hook(address, layout) -> {installed: true, address}
//   send({type: 'observed', address, typedState: {scope, inputs, outputs, sideEffects}})
//
// Argument reads are explicit and defensive. On this Frida 17.24.0 ia32
// endpoint the Interceptor `args` object itself throws
// "RangeError: invalid array index" when it is measured or coerced
// (args.length and Object.prototype.toString.call(args) both throw), while
// explicit indices still resolve. So the four cdecl words are read one at a
// time, each in its own try/catch, with an independent x86 stack read from the
// Interceptor context as fallback. No args.length, no array methods, no
// iteration of args. Every word reports how it was obtained (or that it was
// unavailable); nothing is guessed.

const NATIVE = '0x0048F0C0';
const SCOPE = 'startup-command-line-store';
const ENTRY_SIZE = 10;   // 0x0048F0C0..0x0048F0CA
const STATE_SIZE = 4;    // one dword at the retail state VA
const MAX_TEXT = 4096;   // probe safety bound, not a game constant
const ARG_WORDS = 4;     // cdecl words inspected per call, probe-side bound
const ARG_UNAVAILABLE = '<unavailable>';

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

function decodeLatin1(bytes) {
  return bytes.map(byte => String.fromCharCode(byte)).join('');
}

// Bounded, range-checked C-string read. Refuses rather than faults on a null,
// unreadable or unmapped pointer, and says which case occurred.
function readText(value) {
  const address = ptr(value);
  if (address.isNull()) return {state: 'null', text: '<null>'};
  const range = Process.findRangeByAddress(address);
  if (range === null) return {state: 'unreadable', text: '<unreadable>'};
  if (range.protection.indexOf('r') < 0) return {state: 'unreadable', text: '<unreadable>'};
  const end = range.base.add(range.size);
  if (address.compare(end) >= 0) return {state: 'unreadable', text: '<unreadable>'};
  const available = end.sub(address).toInt32();
  const bound = Math.min(MAX_TEXT, available > 0 ? available : 0);
  const bytes = [];
  try {
    for (let i = 0; i < bound; ++i) {
      const byte = address.add(i).readU8();
      if (byte === 0) return {state: 'text', text: decodeLatin1(bytes)};
      bytes.push(byte);
    }
  } catch (error) {
    return {state: 'unreadable', text: '<unreadable>'};
  }
  return {state: 'truncated', text: decodeLatin1(bytes)};
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

    const module = moduleFor(layout.moduleName);
    const entry = resolveImageVa(module, layout.preferredBase, layout.entryVA, ENTRY_SIZE);
    const state = resolveImageVa(module, layout.preferredBase, layout.stateVA, STATE_SIZE);

    Interceptor.attach(entry, {
      onEnter(args) {
        try {
          // cdecl x86: the command line is the dword at [esp+4]. Read the four
          // inspected words individually and keep an explicit marker for each;
          // readArgument never throws, so one broken surface cannot suppress
          // the observation.
          const reads = [];
          let argument = null;
          for (let index = 0; index < ARG_WORDS; ++index) {
            const read = readArgument(args, this.context, index);
            reads.push(read.marker);
            if (index === 0) argument = read.value;
          }
          this.argument = argument;   // null means "could not be read", not 0
          this.argumentReads = reads;
          const before = state.readPointer();
          this.input = argument === null
            ? {state: 'unavailable', text: ARG_UNAVAILABLE}
            : readText(argument);
          this.previous = readText(before);
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
          const argumentKnown = this.argument !== null;
          const stored = state.readPointer();
          const after = readText(stored);
          const returnedPtr = ptr(returned);
          const aliasesArgument = argumentKnown && returnedPtr.equals(this.argument);
          const returnedText = aliasesArgument ? this.input : readText(returned);
          send({
            type: 'observed',
            address,
            typedState: {
              scope: SCOPE,
              inputs: {
                command_line: this.input.text,
                command_line_state: this.input.state,
                // One explicit marker per inspected cdecl word: state is
                // 'value' or 'unavailable', source names the surface used.
                argument_reads: this.argumentReads
              },
              outputs: {
                return_aliases_argument: argumentKnown ? aliasesArgument : null,
                return_aliases_argument_state: argumentKnown ? 'known' : 'unknown',
                return_is_null: returnedPtr.isNull(),
                return_matches_stored: returnedPtr.equals(stored),
                return_text: returnedText.text
              },
              sideEffects: {
                previous_command_line: this.previous.text,
                previous_command_line_state: this.previous.state,
                stored_command_line: after.text,
                stored_command_line_state: after.state,
                stored_aliases_argument: argumentKnown ? stored.equals(this.argument) : null,
                stored_aliases_argument_state: argumentKnown ? 'known' : 'unknown'
              }
            }
          });
        } catch (error) {
          send({type: 'hook_failed', address, error: String(error)});
        }
      }
    });
    Interceptor.flush();
    return {
      installed: true,
      address,
      module: module.name,
      resolvedEntry: entry.toString(),
      resolvedState: state.toString()
    };
  }
};
