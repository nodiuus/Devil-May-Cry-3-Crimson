INCLUDE CommonMacros.inc

.DATA
EXTERN g_FixSummonedSwordsInitialTravel_ReturnAddr:QWORD
EXTERN g_FixSummonedSwordsInitialTravel_ReturnAddr2:QWORD
EXTERN g_FixSummonedSwordsInitialTravel_ReturnAddr3:QWORD
EXTERN g_FrameRateTimeMultiplier:DWORD

align 4
g_SummonedSwordInitialTravelExponent dd 1.7f
constTwo dd 2.0f

.CODE
; From CPl021Shl01ForwardStepTowardTargetYaw_sub_1401DA020:
; dmc3.exe+1DA078 - F3 0F 11 4C 24 68
; movss [rsp+68],xmm1
;
; Store initial summoned sword easing step distance.
; Value is later yaw-rotated toward the target and integrated into position.
;
; Level 1/2:
;   xmm1 *= powf(FrameRateMultiplier, 1.7f)
;
; For Level 3, we use Detours 2 & 3.

FixSummonedSwordsInitialTravelDetour PROC
    ; xmm1 = incoming travel step (a2)

ApplyFPSCorrection:

    sub rsp, 30h

    ; [rsp+00] = FrameRateMultiplier
    ; [rsp+08] = exponent
    ; [rsp+10] = pow result
    ; [rsp+18] = preserved xmm0

    movss dword ptr [rsp+18h], xmm0

    ; Determine summoned sword level
    movzx eax, byte ptr [rcx+0DF2h]
    cmp eax, 52d
    jne Use17
    jmp RestoreAndExit

    ; ----------------------------------------------------------------------
    ; Level 1/2 path
    ; ----------------------------------------------------------------------

Use17:

    movss xmm0, dword ptr [g_SummonedSwordInitialTravelExponent]
    cvtss2sd xmm0, xmm0
    movsd qword ptr [rsp+8], xmm0

LoadFrameRateMultiplier:

    movss xmm0, dword ptr [g_FrameRateTimeMultiplier]
    cvtss2sd xmm0, xmm0
    movsd qword ptr [rsp], xmm0

    ; pow(mult, exponent)
    ; result = 2^( exponent * log2(mult) )

    fld qword ptr [rsp+8]
    fld qword ptr [rsp]

    fyl2x

    fld st(0)
    frndint
    fsub st(1), st(0)
    fxch st(1)

    f2xm1
    fld1
    faddp st(1), st(0)
    fscale
    fstp st(1)

    fstp qword ptr [rsp+10]

    movsd xmm0, qword ptr [rsp+10]
    cvtsd2ss xmm0, xmm0

    ; xmm1 *= correction
    mulss xmm1, xmm0

RestoreAndExit:

    movss xmm0, dword ptr [rsp+18h]

    add rsp, 30h

OriginalCode:

    movss dword ptr [rsp+68h], xmm1

    jmp qword ptr [g_FixSummonedSwordsInitialTravel_ReturnAddr]

FixSummonedSwordsInitialTravelDetour ENDP


; dmc3.exe+1DB8C2 - F3 0F 11 83 D4 0D 00 00 - movss [rbx+00000DD4],xmm0 { easing accumulation }
; For level 3, we stop the easing accumulation from occuring (detour 2),
; and clamp the displacement according to the frame rate mult / 2 (detour3) to keep everything consistent.
FixSummonedSwordsInitialTravelDetour2 PROC
    
    ; Determine summoned sword level
    push rax
    pushfq

    movzx eax, byte ptr [rbx+0DF2h]
    cmp eax, 52d
    je JmpOut

OriginalCode:
    popfq
    pop rax

    movss dword ptr [rbx+0DD4h], xmm0
    jmp qword ptr [g_FixSummonedSwordsInitialTravel_ReturnAddr2]

JmpOut:
    popfq
    pop rax

    jmp qword ptr [g_FixSummonedSwordsInitialTravel_ReturnAddr2]

FixSummonedSwordsInitialTravelDetour2 ENDP


; dmc3.exe+1DA133 - 0F 28 4C 24 20 - movaps xmm1,[rsp+20] { easing displacement vector }
FixSummonedSwordsInitialTravelDetour3 PROC

OriginalCode:
    movaps xmm1, [rsp+20h]

CheckIfLevel3:
    push rax
    pushfq
    movzx eax, byte ptr [rbx+0DF2h]
    cmp eax, 52d
    jne JmpOut

ApplyScaling:
    movdqu [rsp+30h], xmm7
    movss xmm7, dword ptr [g_FrameRateTimeMultiplier]
    divss xmm7, dword ptr [constTwo]
    mulps xmm1, xmm7
    movdqu xmm7, [rsp+30h]

JmpOut:
    popfq
    pop rax
    jmp qword ptr [g_FixSummonedSwordsInitialTravel_ReturnAddr3]

FixSummonedSwordsInitialTravelDetour3 ENDP

END