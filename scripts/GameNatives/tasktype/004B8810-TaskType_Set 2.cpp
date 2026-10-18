// IGI native provenance: TaskType_Set - 0x004B8810 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004B8810-TaskType_Set.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "TaskType_Set.h"
void* TaskType_Set(int* context, int arg2)

// Sig: void TaskType_Set(int** taskDef, int taskTypeId)
// Note: Stores the TYPE ID onto a task definition structure (simple field
// write, verified 2-arg store). Params: taskDef = task definition object,
// taskTypeId = TASKTYPE_* enum value (TASKTYPE_GUN, TASKTYPE_BUILDING, etc. -
// full enum embedded in igi.exe .data).
{
    return Q_TaskHashTableSet(context, Sys_AllocateMemory_23, arg2, 4, 2, 1, 0);
}
