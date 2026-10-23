// Generic paired probe for the MVP native set.
//
// The retail image carries no symbols, so every MVP native is identified by
// its image-relative VA; the candidate resolves the same native to its own
// linked address and the driver passes that address to rpc.exports.hook().
//
// Raw ABI words are not comparable across two processes: pointers, heap
// addresses and this-pointers differ by construction. Each call is therefore
// reduced to process-independent classes:
//   "null"        a null word
//   "small:<n>"   an exact integer below 0x10000 in magnitude (status codes,
//                 counts, flags)
//   "ptr"         anything else, i.e. an address or a wide value
//
// Calls are aggregated in the agent and sent as one periodic snapshot per
// address. Sending a message per call was measured to starve the hook
// callbacks on high-frequency natives (a 100-target run observed 27 retail
// natives with per-call sends versus 97 with an aggregated probe), so the
// per-call path must stay free of Frida messaging.
'use strict';

const IMAGE_MODULE = (() => {
  const modules = Process.enumerateModules();
  return modules.find(m => /^igi\.exe$/i.test(m.name))
      || modules.find(m => /^open-igi.*\.exe$/i.test(m.name))
      || Process.mainModule;
})();

const SIDE = (() => {
  const name = (IMAGE_MODULE && IMAGE_MODULE.name || '').toLowerCase();
  return name.indexOf('open-igi') >= 0 ? 'candidate' : 'retail';
})();

const moduleBase = IMAGE_MODULE.base;
const preferredBase = (() => {
  const peOffset = moduleBase.add(0x3c).readU32();
  return moduleBase.add(peOffset + 0x34).readU32();
})();

function actual(va) {
  return moduleBase.add(ptr(va).sub(ptr(preferredBase)));
}

const SMALL_LIMIT = 0x10000;
const MAX_PATTERNS = 12;
const MAX_CLASSES = 8;

const stats = {};     // address string -> {hits, ret: {}, args: {}}

function classify(value) {
  if (value === null || value === undefined) return 'unknown';
  if (value.isNull()) return 'null';
  let wide;
  try { wide = value.toUInt32(); } catch (error) { return 'ptr'; }
  if (wide < SMALL_LIMIT) return 'small:' + wide;
  const signed = (wide | 0);
  if (signed < 0 && signed > -SMALL_LIMIT) return 'small:' + signed;
  return 'ptr';
}

function remember(bucket, key, limit) {
  if (Object.keys(bucket).length < limit) bucket[key] = true;
}

function snapshotOf() {
  const snapshot = {};
  for (const key of Object.keys(stats)) {
    const entry = stats[key];
    if (!entry.hits) continue;
    snapshot[key] = {
      hits: entry.hits,
      ret_classes: Object.keys(entry.ret).sort(),
      arg_patterns: Object.keys(entry.args).sort()
    };
  }
  return snapshot;
}

function hookNative(address) {
  const key = address.toString();
  const entry = {hits: 0, ret: {}, args: {}};
  stats[key] = entry;
  Interceptor.attach(actual(address), {
    onEnter(args) {
      // Keep the hot path to array reads only: no send(), no allocation that
      // depends on the call count.
      this.pattern = classify(args[0]) + '|' + classify(args[1]) + '|'
                   + classify(args[2]) + '|' + classify(args[3]);
    },
    onLeave(retval) {
      entry.hits++;
      remember(entry.ret, classify(retval), MAX_CLASSES);
      remember(entry.args, this.pattern, MAX_PATTERNS);
      // Rare natives (the entry point in particular) can be followed by an
      // immediate process exit, which would lose the whole capture before the
      // periodic flush. The first few hits of every native are sent at once;
      // hot natives fall back to the 1.5 s aggregate.
      if (entry.hits <= 5) {
        emit(key, {hits: entry.hits,
                   ret_classes: Object.keys(entry.ret).sort(),
                   arg_patterns: Object.keys(entry.args).sort()}, false);
      }
    }
  });
}

// One message per address per tick, not one per call and not one message
// carrying every address: the shared observer keys payloads by address.
setInterval(() => {
  const snapshot = snapshotOf();
  for (const key of Object.keys(snapshot)) emit(key, snapshot[key], false);
}, 1500);

function emit(key, aggregate, final) {
  send({
    type: 'observed',
    address: key,
    role: 'native',
    side: SIDE,
    final: final,
    aggregate: {
      hits: aggregate.hits,
      ret_classes: aggregate.ret_classes,
      arg_patterns: aggregate.arg_patterns
    }
  });
}

rpc.exports = {
  hook(address) {
    try {
      const target = ptr(address);
      hookNative(target);
      send({ type: 'hooked', address: target.toString(), side: SIDE,
             image: IMAGE_MODULE.name, imageBase: moduleBase.toString(),
             preferredBase: preferredBase.toString() });
      return true;
    } catch (error) {
      send({ type: 'hook_failed', address: String(address), error: String(error) });
      return false;
    }
  },
  // Final aggregate, so a run that ends between snapshots still reports.
  finish() {
    const snapshot = snapshotOf();
    for (const key of Object.keys(snapshot)) emit(key, snapshot[key], true);
    return Object.keys(stats).length;
  }
};
