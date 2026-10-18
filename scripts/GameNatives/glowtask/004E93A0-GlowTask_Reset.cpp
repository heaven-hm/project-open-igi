// IGI native provenance: GlowTask_Reset - 0x004E93A0 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004E93A0-GlowTask_Reset.md; Binary Ninja/Radare2 CFG record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004E93A0-GlowTask_Reset.hpp"
int GlowTask_Reset(void* self)

{
    return SoundSys_AttachLipsync_13(self);
}
