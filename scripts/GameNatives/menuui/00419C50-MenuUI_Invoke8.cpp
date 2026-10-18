// IGI native provenance: MenuUI_Invoke8 - 0x00419C50 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 00419C50-MenuUI_Invoke8.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "00419C50-MenuUI_Invoke8.hpp"
int MenuUI_Invoke8(int arg1, int arg2)

{
    return (*(((Script_ParseEnumReal32_8() * 0x180 + Menu_PlayMusic_13()) << 2) + &g_a96ae0))(arg1,
        arg2);
}
