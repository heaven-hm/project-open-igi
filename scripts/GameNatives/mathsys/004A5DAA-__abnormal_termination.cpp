// IGI native provenance: __abnormal_termination - 0x004A5DAA - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A5DAA-MathSys_ProcessStep_4A5DAA.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A5DAA-__abnormal_termination 2.hpp"
🌐  int __abnormal_termination()

{
    TEB* fsbase;
    struct _EXCEPTION_REGISTRATION_RECORD* ExceptionList = fsbase->NtTib.ExceptionList;

    if (ExceptionList->Handler == MathSys_Invoke7
            && *(ExceptionList + 8) == *(*(ExceptionList + 0xc) + 0xc))
        return 1;

    return 0;
}
