// IGI native provenance: sprintf - 0x004A53B3 - CFG score 100.0% (PASS; structural only); runtime behavior not verified.
// Evidence: 004A53B3-sprintf.md; REA/Ghidra static record; catalog: assets/BinaryNinja/IGINatives_VERIFIED.json.
#include "004A53B3-sprintf.hpp"
int sprintf(char *buffer, const char *format, ...)
{
    return String_VFormat(buffer, format);
}
