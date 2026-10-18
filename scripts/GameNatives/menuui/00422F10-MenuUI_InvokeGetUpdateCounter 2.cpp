// IGI native provenance: MenuUI_InvokeGetUpdateCounter - 0x00422F10 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00422F10-MenuUI_InvokeGetUpdateCounter.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "MenuUI_InvokeGetUpdateCounter.h"

int MenuUI_InvokeGetUpdateCounter(int arg1)

{
    Frames_GetUpdateCounter();
    return (*((Menu_PlayMusic_13() << 2) + &g_a970e0))(arg1);
}
