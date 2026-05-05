INCLUDE CommonMacros.inc
.DATA
EXTERN g_FrameRateTimeMultiplier:DWORD
EXTERN g_FrameRateTimeMultiplierRounded:DWORD

EXTERN g_FixStaggerGravity_ReturnAddr:QWORD

.CODE
FixStaggerGravityDetour PROC

OriginalCode:
    movss xmm0, dword ptr [rdi+0A4h] ; player in rdi, 0xA4 is vertical inertia multiplier

ApplyFPSMultiplier:
    mulss xmm0, dword ptr [g_FrameRateTimeMultiplier]

    jmp qword ptr [g_FixStaggerGravity_ReturnAddr]

FixStaggerGravityDetour ENDP

END