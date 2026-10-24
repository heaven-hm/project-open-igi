// Candidate: DynObj_IsNumericToken_4C0170; retail VA 0x004C0170. Catalog candidate for engine/src/retail/dynobj/native_4c0170.cpp.
// Retail artifact: igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Behavior: classifies the string at object+0x14 by its first one or two characters (digit, $hex, 0x, X) and returns 0 or 1.
#include <cstdint>
extern "C" {
int DynObj_IsNumericToken_4C0170(void* object);
}
