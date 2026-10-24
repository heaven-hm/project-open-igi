// Native: Native_004A5000; retail VA 0x004A5000.
// Artifact: retail igi.exe SHA256 dea34892d417715873ea61d7b2ba5556655f1ad65385104348146f9679915a01.
// Tools/evidence: Radare2 6.2.0 and angr CFGFast; Binary Ninja MCP unavailable in this run, so no second decompiler was used.
// CFG/behavior: 123 bytes, 10 basic blocks, 14 edges; saves the x86 register/return state into arg1,
// writes the 0x56433230 "02CV" cookie, captures fs:[0], processes the _setjmp3 variadic unwind words,
// and returns zero. Caller 0x00401050 passes 0x0054F8E0 as arg1 and zero as the count.
// Status/limits: BUILD_VERIFIED only; paired retail/candidate runtime evidence is unavailable because the
// candidate does not currently execute this address and the Frida provider is blocked by tool health.

#include "004A5000-Native_004A5000.hpp"

#if defined(__i386__) && (defined(__GNUC__) || defined(__clang__))
extern "C" __attribute__((naked)) int __cdecl Native_004A5000(
    void*, unsigned, ...)
{
    __asm__ volatile(
        "movl 4(%esp), %edx\n"
        "movl %ebp, (%edx)\n"
        "movl %ebx, 4(%edx)\n"
        "movl %edi, 8(%edx)\n"
        "movl %esi, 12(%edx)\n"
        "movl %esp, 16(%edx)\n"
        "movl (%esp), %eax\n"
        "movl %eax, 20(%edx)\n"
        "movl $0x56433230, 32(%edx)\n"
        "movl $0, 36(%edx)\n"
        "movl %fs:0, %eax\n"
        "movl %eax, 24(%edx)\n"
        "cmpl $-1, %eax\n"
        "jne 1f\n"
        "movl $-1, 28(%edx)\n"
        "jmp 8f\n"
        "1:\n"
        "movl 8(%esp), %ecx\n"
        ".byte 0x0b, 0xc9\n"
        "je 3f\n"
        "movl 12(%esp), %eax\n"
        "movl %eax, 36(%edx)\n"
        "decl %ecx\n"
        "jne 4f\n"
        "3:\n"
        "movl 12(%eax), %eax\n"
        "movl %eax, 28(%edx)\n"
        "jmp 8f\n"
        "4:\n"
        "movl 16(%esp), %eax\n"
        "movl %eax, 28(%edx)\n"
        "decl %ecx\n"
        "je 8f\n"
        "pushl %esi\n"
        "pushl %edi\n"
        "leal 28(%esp), %esi\n"
        "leal 40(%edx), %edi\n"
        "cmpl $6, %ecx\n"
        "jbe 7f\n"
        "movl $6, %ecx\n"
        "7:\n"
        "rep movsl\n"
        "popl %edi\n"
        "popl %esi\n"
        "8:\n"
        ".byte 0x2b, 0xc0\n"
        "ret\n");
}
#else
#error "Setjmp3Frame_004A5000 requires the x86 compiler used for the retail candidate"
#endif
