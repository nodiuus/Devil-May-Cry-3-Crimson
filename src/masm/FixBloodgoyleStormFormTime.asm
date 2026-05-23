INCLUDE CommonMacros.inc

.DATA
EXTERN g_FixBloodgoyleStormFormTime_ReturnAddr:QWORD
EXTERN g_FrameRateTimeMultiplier:DWORD

.CODE
; From CEm014BloodgoyleUpdate_sub_1400E2D70:
; dmc3.exe+E2E2C - F3 0F 5C 84 C7 68 B5 56 00 - subss xmm0,[rdi+rax*8+0056B568] { Calculate time to turn back to blood from stone }
FixBloodgoyleStormFormTimeDetour PROC
PreserveRegisters:
    push rdx
    movdqu [rsp], xmm2 ; preserve registers

LoadAndCalculate:
    movss xmm2, dword ptr [rdi+rax*8+0056B568h]
    mulss xmm2, dword ptr [g_FrameRateTimeMultiplier] ; Multiply the time by the frame rate multiplier to keep it consistent across different frame rates

OriginalCode:
    subss xmm0, xmm2

RestoreRegisters:
    movdqu xmm2, [rsp]
    pop rdx

    jmp qword ptr [g_FixBloodgoyleStormFormTime_ReturnAddr]

FixBloodgoyleStormFormTimeDetour ENDP
END