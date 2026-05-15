INCLUDE CommonMacros.inc

.DATA
EXTERN g_FixSummonedSwordsInitialTravel_ReturnAddr:QWORD
EXTERN g_FrameRateTimeMultiplier:DWORD

align 4
g_SummonedSwordInitialTravelExponent dd 1.7f
g_SummonedSwordInitialTravelSqrtValue dd 0.1872983

.CODE
; From CPl021Shl01ForwardStepTowardTargetYaw_sub_1401DA020:
; dmc3.exe+1DA078 - F3 0F 11 4C 24 68 - movss [rsp+68],xmm1 { store initial summoned sword easing step distance }
FixSummonedSwordsInitialTravelDetour PROC

    OriginalCode:
    ; xmm1 = incoming travel step (a2)

ApplyFPSCorrection:

    ; ------------------------------------------------------------------------
    ; Compute:
    ;
    ;   Level 1/2: xmm1 *= powf(FrameRateMultiplier, 1.7f)
    ;   Level 3:   xmm1 *= sqrtf(FrameRateMultiplier)
    ;
    ; Using x87 for powf because SSE implementation is annoying and this
    ; path isn't performance-critical.
    ;
    ; ------------------------------------------------------------------------

    sub rsp, 20h

    ; [rsp+00] = FrameRateMultiplier (pow path only)
    ; [rsp+08] = exponent 1.7 (pow path only)
    ; [rsp+10] = pow result (pow path only)
    ; [rsp+18] = preserved xmm0

    movss dword ptr [rsp+18h], xmm0

    ; Choose path based on summoned sword level (1, 2, or 3)
    movzx eax, byte ptr [rcx+0DF2h]
    cmp eax, 52d
    jne Use17

    ; Level 3: multiply by sqrt(FrameRateMultiplier)
    movss xmm0, dword ptr [g_SummonedSwordInitialTravelSqrtValue]
    sqrtss xmm0, xmm0
    jmp ApplyCorrection

Use17:
    ; Level 1/2: multiply by powf(FrameRateMultiplier, 1.7f)
    movss xmm0, dword ptr [g_SummonedSwordInitialTravelExponent]
    cvtss2sd xmm0, xmm0
    movsd qword ptr [rsp+8], xmm0

LoadFrameRateMultiplier:
    movss xmm0, dword ptr [g_FrameRateTimeMultiplier]
    cvtss2sd xmm0, xmm0
    movsd qword ptr [rsp], xmm0

    ; ------------------------------------------------------------------------
    ; pow(mult, exponent)
    ; result = 2^( exponent * log2(mult) )
    ; ------------------------------------------------------------------------

    fld qword ptr [rsp+8]      ; st0 = exponent
    fld qword ptr [rsp]        ; st0 = mult, st1 = exponent

    fyl2x                      ; st0 = exponent * log2(mult)

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

    ; xmm0 = pow(mult, 1.7)

    movsd xmm0, qword ptr [rsp+10]
    cvtsd2ss xmm0, xmm0

    ; xmm1 *= correction

ApplyCorrection:
    mulss xmm1, xmm0

    movss xmm0, dword ptr [rsp+18h]
    add rsp, 20h

    ; ------------------------------------------------------------------------
    ; Original instruction
    ; ------------------------------------------------------------------------

OriginalInstruction:
    movss dword ptr [rsp+68h], xmm1
    jmp qword ptr [g_FixSummonedSwordsInitialTravel_ReturnAddr]

FixSummonedSwordsInitialTravelDetour ENDP
END