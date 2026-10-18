// IGI native provenance: AlarmControl_TriggerAlarm - 0x00440970 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00440970-AlarmControl_TriggerAlarm.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00440970-AlarmControl_TriggerAlarm.hpp"
char* AlarmControl_TriggerAlarm(void* self)

{
    (*((Env_UpdateCameraLens_3() << 2) + &g_a976e0))(self);

    if (Script_EvaluateCondition_4(self) >= 0)
    {
        int result_val_4 = Script_SpawnNewTask_6();
        int counter_val_1 = 0;
        counter_val_1 = *(self + 0x1c);
        int result_val_8 = *(((result_val_4 * 0x180 + counter_val_1) << 2) + &g_a96ae0);

        if (result_val_8)
            result_val_8(self);
    }

    Script_CallNativeHandler_6(self + 0x2ac);
    return Script_CallNativeHandler_6(self + 0x2fc);
}
