# `NATIVE_DECL` hazard: `task_types.hpp` undefines the macro (verified 2026-10-25)

Verified in the tree, not reported second-hand:

```
engine/include/igi/retail/core/task_types.hpp:14   #define NATIVE_DECL __cdecl            (MSVC)
engine/include/igi/retail/core/task_types.hpp:16   #define NATIVE_DECL __attribute__((cdecl))
engine/include/igi/retail/core/task_types.hpp:18   #define NATIVE_DECL
engine/include/igi/retail/core/task_types.hpp:209  #undef NATIVE_DECL
```

`task_types.hpp` defines `NATIVE_DECL` at the top and **undefines it at line 209**. Any native
that includes that header and then uses `NATIVE_DECL` must restate the guarded block after the
include, or it compiles with the macro undefined.

## Why this is recorded

This is the likely root cause of the "expected initializer before ..." defect class that hit 21
of 22 files in the AIGraph batch earlier today: the macro block was placed below the
declarations that used it. The tree has two different conventions in play and no single owner:

- `task_types.hpp` and most natives use the three-way guarded block
  (`_MSC_VER` -> `__cdecl`, `__i386__` -> `__attribute__((cdecl))`, else empty);
- at least one new file (`sys/native_4f1cb0.cpp`) hardcodes
  `#define NATIVE_DECL __attribute__((cdecl))`, which works for the current cross-compiler but
  is not the tree's convention and will not build under MSVC.

## Rule applied from here

Every new native states the guarded block itself, above the first declaration that uses it, and
never relies on an include to have provided the macro. When a native includes `task_types.hpp`,
the block goes after the include and before the first use. The hardcoded single-branch form is
normalised to the guarded form at integration.

This is a convention note, not a code change: no engine file is edited for it here.
