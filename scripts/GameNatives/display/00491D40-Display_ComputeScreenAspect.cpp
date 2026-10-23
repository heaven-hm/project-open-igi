// Native: Display_ComputeScreenAspect; retail VA 0x00491D40; signature x86 cdecl double(void).
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Binary Ninja + Radare2 CFG (docs/fidelity/evidence/0x00491D40-radare2.json,
// -binaryninja.json, -semantic-review.json); angr differential run by scripts/view_block_diff_runner.py.
// Behavior/CFG: one basic block, no branches. Calls Display_GetActiveMode (0x00491CF0) twice for the
// display-mode object; computes (fild dword [mode+8] * 4.0) / (fild dword [mode+4] * 3.0) on the x87
// stack - the screen aspect factor - and leaves it in st0 as the double return value.
// Status/limits: canonical reference transcription for the record's engine implementation
// engine/src/retail/render/view_block.cpp. Not compiled into the candidate build; the engine body is the
// translation unit the candidate links, and this file is the address-qualified retail reference.
extern "C" int Display_GetActiveMode_retail(); // 0x00491CF0

double Display_ComputeScreenAspect()
{
    const unsigned char* mode = reinterpret_cast<const unsigned char*>(Display_GetActiveMode_retail());
    const int height = *reinterpret_cast<const int*>(mode + 8);
    const int width = *reinterpret_cast<const int*>(mode + 4);
    return (static_cast<double>(height) * 4.0) / (static_cast<double>(width) * 3.0);
}
