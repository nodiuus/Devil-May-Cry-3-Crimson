INCLUDE CommonMacros.inc

.DATA
EXTERN g_FixCStageSetSeal_ReturnAddr1:QWORD
EXTERN g_FixCStageSetSeal_ReturnAddr2:QWORD
EXTERN g_FixCStageSetSeal_ReturnAddr3:QWORD
constOne dd 1.0

.CODE
; From CStageSetSealHandAndAnim_sub_14026E910:
; dmc3.exe+26E9BD - F3 44 0F 2C C0 - cvttss2si r8d,xmm0 { fade logic truncation (bad) }
FixCStageSetSealDetour1 PROC
    
    cvttss2si r8d, xmm0 ; trying to fix truncation of fade logic by checking if the value is between 0 and 1 (inclusive) before truncation, if not then set to 1 (fully faded out)
    test r8d, r8d
    jnz short JmpOut
    mov r8d, 1

JmpOut:
    jmp qword ptr [g_FixCStageSetSeal_ReturnAddr1]

FixCStageSetSealDetour1 ENDP
; dmc3.exe+26EB09 - F3 0F 10 73 14 - movss xmm6,[rbx+14] { setting speed for scrolling anim }
FixCStageSetSealDetour2 PROC

    movss xmm6, dword ptr [constOne] ; set to 1.0 to prevent slowdown of scrolling anim
    jmp qword ptr [g_FixCStageSetSeal_ReturnAddr2]

FixCStageSetSealDetour2 ENDP
; dmc3.exe+26EB46 - F3 44 0F 2C C8 - cvttss2si r9d,xmm0 { scrolling speed truncation (bad) }
; dmc3.exe+26EB4B - F3 44 0F 2C C1 - cvttss2si r8d,xmm1
FixCStageSetSealDetour3 PROC

    cvtss2si r9d, xmm0 ; cvttss2si to cvtss2si to combat aggressive rounding
    cvtss2si r8d, xmm1
    jmp qword ptr [g_FixCStageSetSeal_ReturnAddr3]

FixCStageSetSealDetour3 ENDP
END