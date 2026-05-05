INCLUDE CommonMacros.inc

; Let's use FLD/x87, easier than doing the SSE implementation xD
; --------------------------------------------------------------------------
; POWF_XMM macro:  dst = powf(dst, g_FrameRateTimeMultiplier)
; Implements via X87:  2^(mult * log2(dst))
; Clobbers xmm0, st(0).  Leaves x87 stack clean.
; Requires rsp scratch space to be pre-allocated (caller's sub rsp, 16).
; --------------------------------------------------------------------------
POWF_XMM MACRO dst
    movss   xmm0, dword ptr [g_FrameRateTimeMultiplier]
    cvtss2sd xmm0, xmm0                             ; double: multiplier
    cvtss2sd dst, dst                               ; double: value
    movsd   qword ptr [rsp],     dst                ; [rsp]   = value
    movsd   qword ptr [rsp+8],  xmm0               ; [rsp+8] = multiplier

    fld     qword ptr [rsp+8]                       ; st(0) = multiplier
    fld     qword ptr [rsp]                         ; st(0) = value, st(1) = mult
    fyl2x                                            ; st(0) = mult * log2(value)
    fld     st(0)                                    ; st(0)=st(1)= exponent
    frndint                                          ; st(0) = floor(exponent)
    fsub    st(1), st(0)                             ; st(1) = fractional part
    fxch    st(1)                                    ; st(0) = frac, st(1)=int
    f2xm1                                            ; st(0) = 2^frac - 1
    fld1                                             ; st(0) = 1
    faddp   st(1), st(0)                             ; st(0) = 2^frac
    fscale                                           ; st(0) = 2^exponent
    fstp    st(1)                                    ; pop integer part
    fstp    qword ptr [rsp]                          ; [rsp] = result

    movsd   dst, qword ptr [rsp]
    cvtsd2ss dst, dst                               ; dst = powf(value, mult)
ENDM

.DATA
EXTERN g_FrameRateTimeMultiplier:DWORD
EXTERN g_FrameRateTimeMultiplierRounded:DWORD

EXTERN g_FixBossCamLookAt_ReturnAddr:QWORD

.CODE
FixBossCamLookAtDetour PROC

OriginalCode:
    mov [rsp+040h], rax

ApplyFPSMultiplier:
    ; Compute:
    ;   xmm2 = powf(xmm2, g_FrameRateTimeMultiplier)  — X/Z smoothing
    ;   xmm3 = powf(xmm3, g_FrameRateTimeMultiplier)  — Y   smoothing
    ;
    ; At this point xmm2/xmm3 hold the raw blend weights.
    ; xmm6-xmm10 are dead — the function loads them from parameters
    ; right after the replaced instruction.
    sub     rsp, 16                                 ; scratch space
    POWF_XMM xmm2                                    ; xmm2 = powf(xmm2, mult)
    POWF_XMM xmm3                                    ; xmm3 = powf(xmm3, mult)
    add     rsp, 16

    jmp qword ptr [g_FixBossCamLookAt_ReturnAddr]

FixBossCamLookAtDetour ENDP


END

