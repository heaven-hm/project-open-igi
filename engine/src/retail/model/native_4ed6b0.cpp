// Native: LODSettings_Init; retail VA 0x4ED6B0.
// Artifact: retail igi.exe SHA-256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01; image base 0x00400000.
// Source: body and signature adopted from IGI-RE 022dc7bedf5e469743ffdc66e1b87ecf89c6b559 src/animtrig/lodsettings.c; converted C -> C++.
//         IGI-RE is a functional decompilation - equivalent behaviour, NOT byte-identical code.
// Behavior/CFG: see the converted body below; retail instruction-level CFG not measured for this file.
// Status/limits: TRANSCRIBED; BUILD_VERIFIED; STRUCTURALLY_VERIFIED against the IGI-RE source body only;
//         runtime UNOBSERVED. SECOND-HAND reconstruction: correctness rests on IGI-RE's differential
//         tests, not on our own instruction-level reading of igi.exe. Not verified against retail.
// Conversion: domain 'model' from IGI-RE directory 'animtrig'; signature source: igi-re-ORIG.
// Signature sources per callee: LODSettings_Changed=file-local prototype (no ORIG entry); LODSettings_Create=file-local prototype (no ORIG entry); LODSettings_GetParams=file-local prototype (no ORIG entry); ModelLODSettingsContainer_Create=file-local prototype (no ORIG entry); ModelLODSettingsContainer_PreDelete=file-local prototype (no ORIG entry); ModelLODSettings_Changed=file-local prototype (no ORIG entry) ...
// ARGUMENT-COUNT DIVERGENCE: 19 of 49 call site(s) pass a different number of arguments in IGI-RE than retail pushes; the IGI-RE call is generated faithfully and the divergence is recorded in docs/fidelity/evidence/igire-conversion-ledger.md.
// Conversion warning: `ModelLODSettings_Changed` is 0x4EDB60, where this tree has `LODSettings_LoadParameters` with 2 parameter(s) but IGI-RE's `ModelLODSettings_Changed` takes 1; NOT substituting - the IGI-RE name is kept and this stays an open link edge (the two are different functions)

#include <cstdint>
#include <cstddef>

#if defined(_MSC_VER) && defined(_M_IX86)
#define NATIVE_DECL __cdecl
#elif defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
#define NATIVE_DECL __attribute__((cdecl))
#else
#define NATIVE_DECL
#endif

// ---- IGI-RE type definitions, verbatim (layouts must match retail) ----

// /tmp/IGI-RE/src/types.h: verbatim IGI-RE type definition (layout must match retail)
typedef struct Node {
    struct Node *next;
    struct Node *prev;
} Node;

// /tmp/IGI-RE/src/types.h: verbatim IGI-RE type definition (layout must match retail)
typedef struct List {
    Node *head;
    Node *tail;
    Node *tailpred;
} List;

// /tmp/IGI-RE/src/task/tasktypes.h: verbatim IGI-RE type definition (layout must match retail)
typedef struct Task {
    union {
        Node node;
        struct {
            struct Task *next;
            struct Task *prev;
        };
    };
    union {
        List children;
        struct {
            struct Task *head;
            struct Task *tail;
            struct Task *tailpred;
        };
    };
    struct Task *parent;
    int refs;
    uint16_t type;
    char pooled;
    char unk1F;
} Task;

// /tmp/IGI-RE/src/animtrig/lodsettings.c: verbatim IGI-RE type definition (layout must match retail)
typedef struct LODSettings {
    Task task;
    float splineDetail;
    float rigidDetail;
    float boneDetail;
    float splineCull;
    float rigidCull;
    float boneCull;
    float blend;
    float range;
} LODSettings;

// /tmp/IGI-RE/src/animtrig/lodsettings.c: verbatim IGI-RE type definition (layout must match retail)
typedef struct ModelLODSettings {
    Task task;
    char name[16];
    float dist[5];
} ModelLODSettings;

// /tmp/IGI-RE/src/param/paramtypes.h: verbatim IGI-RE type definition (layout must match retail)
typedef struct RangeI {
    int min, max;
} RangeI;

// /tmp/IGI-RE/src/param/paramtypes.h: verbatim IGI-RE type definition (layout must match retail)
typedef struct RangeF {
    float min, max;
} RangeF;

// /tmp/IGI-RE/src/param/paramtypes.h: verbatim IGI-RE type definition (layout must match retail)
typedef union ParamRange {
    RangeI i;
    RangeF f;
} ParamRange;

extern "C" {

// 0x54DBDC: IGI-RE `gLODSettingsType`, image initial value 385; emitted as `g_54dbdc` per this tree's global naming
uint16_t g_54dbdc = 385;

// 0x54DBDE: IGI-RE `gModelLODSettingsType`, image initial value 385; emitted as `g_54dbde` per this tree's global naming
uint16_t g_54dbde = 385;

// 0x54DBE0: IGI-RE `gModelLODSettingsContainerType`, image initial value 385; emitted as `g_54dbe0` per this tree's global naming
uint16_t g_54dbe0 = 385;

// 0xA5EFA8: IGI-RE `gModelLodDistRange`, zero fill; emitted as `g_a5efa8` per this tree's global naming
ParamRange g_a5efa8;

// 0xA5EFB0: IGI-RE `gLodBlendRange`, zero fill; emitted as `g_a5efb0` per this tree's global naming
ParamRange g_a5efb0;

// 0xA5EFB8: IGI-RE `gLodCullRange`, zero fill; emitted as `g_a5efb8` per this tree's global naming
ParamRange g_a5efb8;

// 0xA5EFC0: IGI-RE `gLodDetailRange`, zero fill; emitted as `g_a5efc0` per this tree's global naming
ParamRange g_a5efc0;

// 0xA5EFC8: IGI-RE `gLODSettingsParams`, zero fill; emitted as `g_a5efc8` per this tree's global naming
void * g_a5efc8;

// 0xA5EFCC: IGI-RE `gModelLODSettingsParams`, zero fill; emitted as `g_a5efcc` per this tree's global naming
void * g_a5efcc;

// ---- called functions: real declarations, never cast integers ----

// 0x4EDAA0: IGI-RE `LODSettings_Changed` (address-taken) (signature from file-local prototype (no ORIG entry))
void NATIVE_DECL LODSettings_Changed (LODSettings *s);

// 0x4EDA70: IGI-RE `LODSettings_Create` (address-taken) (signature from file-local prototype (no ORIG entry))
void NATIVE_DECL LODSettings_Create (LODSettings *s);

// 0x4EDB20: IGI-RE `LODSettings_GetParams` (address-taken) (signature from file-local prototype (no ORIG entry))
void NATIVE_DECL LODSettings_GetParams (LODSettings *s, void **out);

// 0x4EDC00: IGI-RE `ModelLODSettingsContainer_Create` (address-taken) (signature from file-local prototype (no ORIG entry))
void NATIVE_DECL ModelLODSettingsContainer_Create (Task *t);

// 0x4EDC50: IGI-RE `ModelLODSettingsContainer_PreDelete` (address-taken) (signature from file-local prototype (no ORIG entry))
void NATIVE_DECL ModelLODSettingsContainer_PreDelete (Task *t);

// 0x4EDB60: IGI-RE `ModelLODSettings_Changed` (address-taken) (signature from file-local prototype (no ORIG entry))
void NATIVE_DECL ModelLODSettings_Changed (ModelLODSettings *s);

// 0x4EDB30: IGI-RE `ModelLODSettings_Create` (address-taken) (signature from file-local prototype (no ORIG entry))
void NATIVE_DECL ModelLODSettings_Create (ModelLODSettings *s);

// 0x4EDBD0: IGI-RE `ModelLODSettings_Delete` (address-taken) (signature from file-local prototype (no ORIG entry))
void NATIVE_DECL ModelLODSettings_Delete (ModelLODSettings *s);

// 0x4EDBF0: IGI-RE `ModelLODSettings_GetParams` (address-taken) (signature from file-local prototype (no ORIG entry))
void NATIVE_DECL ModelLODSettings_GetParams (ModelLODSettings *s, void **out);

// 0x4F1A80: IGI-RE `ParamTable_Add` (called), emitted as this tree's `Script_TaskContainer_Append` (signature from arity 6 match, file-local ORIG (majority 1/1 of 80 entries))
void NATIVE_DECL Script_TaskContainer_Append (void *table, void *base, void *field, void *type, int flags, void *range);

// 0x4F1950: IGI-RE `ParamTable_New` (called), emitted as this tree's `Script_TaskContainer_Alloc` (signature from arity 1 match, file-local ORIG (majority 1/1 of 71 entries))
void * NATIVE_DECL Script_TaskContainer_Alloc (int max);

// 0x4F1A70: IGI-RE `Param_GetChangedEvent` (called), emitted as this tree's `Script_PhaseIdGetE5_4F1A70` (signature from arity 0 match, file-local ORIG (majority 1/1 of 90 entries))
uint8_t NATIVE_DECL Script_PhaseIdGetE5_4F1A70 ();

// 0x4F1A60: IGI-RE `Param_GetTableEvent` (called), emitted as this tree's `Script_PhaseIdGetE4_4F1A60` (signature from arity 0 match, file-local ORIG (majority 1/1 of 81 entries))
uint8_t NATIVE_DECL Script_PhaseIdGetE4_4F1A60 ();

// 0x4F0F50: IGI-RE `Param_Real32` (called), emitted as this tree's `Sys_GetGlobal32_4F0F50` (signature from arity 0 match, file-local ORIG (majority 1/1 of 35 entries))
void * NATIVE_DECL Sys_GetGlobal32_4F0F50 ();

// 0x4F0FD0: IGI-RE `Param_String16` (called), emitted as this tree's `Core_GetRenderWordB_4F0FD0` (signature from arity 0 match, file-local ORIG (majority 1/1 of 29 entries))
void * NATIVE_DECL Core_GetRenderWordB_4F0FD0 ();

// 0x401900: IGI-RE `TaskType_New` (called), emitted as this tree's `TaskType_Register` (signature from arity 4 match (majority 81/81 of 145 entries))
uint16_t NATIVE_DECL TaskType_Register (const char *name, int size, uint16_t parent, int pool_count);

// 0x401400: IGI-RE `TaskType_SetHandler` (called), emitted as this tree's `QTask_LinkQTaskEventHandler` (signature from arity 3 match (majority 142/144 of 144 entries))
void NATIVE_DECL QTask_LinkQTaskEventHandler (uint16_t type, uint8_t slot, void *fn);

// 0x401530: IGI-RE `TaskType_SetHandler2` (called), emitted as this tree's `QTask_LinkQTaskEventHandlerNoContextFn` (signature from arity 3 match (majority 132/132 of 132 entries))
void NATIVE_DECL QTask_LinkQTaskEventHandlerNoContextFn (uint16_t type, int slot, void *fn);

// 0x4018F0: IGI-RE `Task_GetRootType` (called), emitted as this tree's `QTask_GetCurrentType` (signature from arity 0 match (majority 59/59 of 60 entries))
uint16_t NATIVE_DECL QTask_GetCurrentType ();

void NATIVE_DECL LODSettings_Init ()
{
    LODSettings s;
    ModelLODSettings m;

    g_a5efa8.f.min = 0;
    g_a5efa8.f.max = 1000.0f;
    g_a5efc0.f.min = 0;
    g_a5efc0.f.max = 1500.0f;
    g_a5efb8.f.min = 0;
    g_a5efb8.f.max = 10000.0f;
    g_a5efb0.f.min = 0;
    g_a5efb0.f.max = 1.0f;
    g_54dbdc = TaskType_Register("LODSettings", 0x40, QTask_GetCurrentType(), 0);
    g_54dbde = TaskType_Register("ModelLODSettings", 0x44, QTask_GetCurrentType(), 0);
    g_54dbe0 = TaskType_Register("ModelLODSettingsContainer", 0x20, QTask_GetCurrentType(), 0);
    QTask_LinkQTaskEventHandler(g_54dbdc, 0, reinterpret_cast<void *>(&LODSettings_Create));
    QTask_LinkQTaskEventHandler(g_54dbdc, Script_PhaseIdGetE5_4F1A70(), reinterpret_cast<void *>(&LODSettings_Changed));
    QTask_LinkQTaskEventHandler(g_54dbdc, Script_PhaseIdGetE4_4F1A60(), reinterpret_cast<void *>(&LODSettings_GetParams));
    QTask_LinkQTaskEventHandler(g_54dbde, 0, reinterpret_cast<void *>(&ModelLODSettings_Create));
    QTask_LinkQTaskEventHandlerNoContextFn(g_54dbde, 2, reinterpret_cast<void *>(&ModelLODSettings_Delete));
    QTask_LinkQTaskEventHandler(g_54dbde, Script_PhaseIdGetE5_4F1A70(), reinterpret_cast<void *>(&ModelLODSettings_Changed));
    QTask_LinkQTaskEventHandler(g_54dbde, Script_PhaseIdGetE4_4F1A60(), reinterpret_cast<void *>(&ModelLODSettings_GetParams));
    QTask_LinkQTaskEventHandler(g_54dbe0, 0, reinterpret_cast<void *>(&ModelLODSettingsContainer_Create));
    QTask_LinkQTaskEventHandlerNoContextFn(g_54dbe0, 3, reinterpret_cast<void *>(&ModelLODSettingsContainer_PreDelete));
    g_a5efc8 = Script_TaskContainer_Alloc(8);
    g_a5efcc = Script_TaskContainer_Alloc(6);
    Script_TaskContainer_Append(g_a5efc8, &s, &s.splineDetail, Sys_GetGlobal32_4F0F50(), 7, &g_a5efc0);
    Script_TaskContainer_Append(g_a5efc8, &s, &s.rigidDetail, Sys_GetGlobal32_4F0F50(), 7, &g_a5efc0);
    Script_TaskContainer_Append(g_a5efc8, &s, &s.boneDetail, Sys_GetGlobal32_4F0F50(), 7, &g_a5efc0);
    Script_TaskContainer_Append(g_a5efc8, &s, &s.splineCull, Sys_GetGlobal32_4F0F50(), 7, &g_a5efb8);
    Script_TaskContainer_Append(g_a5efc8, &s, &s.rigidCull, Sys_GetGlobal32_4F0F50(), 7, &g_a5efb8);
    Script_TaskContainer_Append(g_a5efc8, &s, &s.boneCull, Sys_GetGlobal32_4F0F50(), 7, &g_a5efb8);
    Script_TaskContainer_Append(g_a5efc8, &s, &s.blend, Sys_GetGlobal32_4F0F50(), 7, &g_a5efb0);
    Script_TaskContainer_Append(g_a5efc8, &s, &s.range, Sys_GetGlobal32_4F0F50(), 7, NULL);
    Script_TaskContainer_Append(g_a5efcc, &m, m.name, Core_GetRenderWordB_4F0FD0(), 7, NULL);
    Script_TaskContainer_Append(g_a5efcc, &m, &m.dist[0], Sys_GetGlobal32_4F0F50(), 7, &g_a5efa8);
    Script_TaskContainer_Append(g_a5efcc, &m, &m.dist[1], Sys_GetGlobal32_4F0F50(), 7, &g_a5efa8);
    Script_TaskContainer_Append(g_a5efcc, &m, &m.dist[2], Sys_GetGlobal32_4F0F50(), 7, &g_a5efa8);
    Script_TaskContainer_Append(g_a5efcc, &m, &m.dist[3], Sys_GetGlobal32_4F0F50(), 7, &g_a5efa8);
    Script_TaskContainer_Append(g_a5efcc, &m, &m.dist[4], Sys_GetGlobal32_4F0F50(), 7, &g_a5efa8);
}

} // extern "C"

#undef NATIVE_DECL
