# Verification methods that have actually caught errors (2026-10-25)

Every rule below exists because the naive version of the check passed while the underlying claim
was wrong. They are cheap and they belong in every increment.

## 1. Counts need their pattern and corpus attached

A VA index built from provenance headers gave 629, then 672, then 676 for the same tree, depending
on the regular expression and the file set. The exact figure is not a fact about the tree; it is a
fact about the query. So when a count is used to declare something missing, record the pattern and
the corpus with the number.

The specific bug worth remembering: the pattern
`^// Native:\s*[^;\n]+;\s*retail VA\s*0x([0-9A-Fa-f]{6,8})` cannot cross a semicolon inside the
name segment, so every file whose name carries a parenthetical containing a semicolon was silently
dropped from the index. Reproduced independently at 629 against a corrected 672-676. An index that
under-counts is indistinguishable from a set of genuine gaps.

**Rule:** before an index is used to declare anything missing, validate it against an independent
count of the same corpus, and resolve an address by **content** - the provenance header - never by
a filename glob. A filename test misses zero-padded names (`native_00460bf0.cpp`; there are 17
`native_0*.cpp` files in `core/` alone) and files not named after a VA at all (`qtask_list.cpp`
holds `0x004C1800`). The filename test found 4 phantom gaps where the content test found 10.

## 2. Definition and declaration must agree, and only `nm` proves it

Twice this session a native was implemented while its callers still referenced a mangled or
differently-named symbol. Compiling a file proves nothing about the other side of the call.
`nm -u` on the caller and `nm --defined-only` on the definition, compared by name, is the check.
The `0x004AF960` case also shows the trap is latent: the mismatch existed for hours and would only
have failed at link time, once a file that was not yet in `CMakeLists.txt` was added.

## 3. A dropped call is invisible to a distinct-callee count

A candidate emitted 76 calls against retail's 77 because the callee never reads the value passed,
so the call looked unnecessary. Retail executes it, so the call is behaviour. The dropped call was
the only call to that target, so a distinct-callee count was unchanged; only the **total** moved.
Per-target call counts against the retail disassembly are the standard, and the total is the
backstop.

## 4. Private copies of shared retail storage look correct

Three accepted natives each declared their own byte at `0x005391AE`/`AF`/`B0` initialised to the
image value `0xFF`, while the registrar `0x00413DF0` writes the real values into
`0x005391AC..0x005391B2`. Each file was individually correct-looking; together they returned the
image constant forever. **Rule:** a retail address has one owner, code or data, and a reader
references the owner's object. See `mem:data-slot-ownership-20261025`.

## 5. A rule derived from N bodies is not a rule

Three generalisations died in one session, all to a single new body: registrars never have the
`0x4B8810` script-name tail (they do); the tail zero-extends its id with `xor`+`mov` (three
different encodings inside one body, a fourth in another); sibling registrations share a link set
(they do not - 13 links versus 11, different handlers). State findings as "in the N bodies seen so
far", never as universals, and treat a body that confirms a derived rule as no evidence at all.
