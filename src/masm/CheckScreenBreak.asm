; From CScrnBreakUpdate_sub_1402440B0:
; dmc3.exe+24419E - F3 0F11 8E 50240000 - movss [rsi+00002450],xmm1 { Screen Break }

INCLUDE CommonMacros.inc

.DATA
EXTERN g_CheckScreenBreak_ReturnAddr:QWORD
EXTERN g_screenBreakTriggered:WORD
compareValue dd 3.0

.CODE
CheckScreenBreakDetour PROC
    pushfq
    comiss xmm1, dword ptr [compareValue] 
    jb ScreenBreakTrigger
    popfq
    jmp OriginalCode

ScreenBreakTrigger:
    mov word ptr [g_screenBreakTriggered], 1 ; Set the screen break triggered flag
    popfq

OriginalCode:
    movss dword ptr [rsi+02450h], xmm1
    jmp qword ptr [g_CheckScreenBreak_ReturnAddr]

CheckScreenBreakDetour ENDP
END