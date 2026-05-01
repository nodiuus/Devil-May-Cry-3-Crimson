INCLUDE CommonMacros.inc
.DATA
EXTERN g_FrameRateTimeMultiplier:DWORD
EXTERN g_FrameRateTimeMultiplierRounded:DWORD

EXTERN g_FixClothPhysics_ReturnAddr1:QWORD
EXTERN g_FixClothPhysics_ReturnAddr2:QWORD
EXTERN g_FixClothPhysics_ReturnAddr3:QWORD
EXTERN g_FixClothPhysics_ReturnAddr4:QWORD
EXTERN g_FixClothPhysics_ReturnAddr5:QWORD
EXTERN g_FixClothPhysics_ReturnAddr6:QWORD
EXTERN g_FixClothPhysics_ReturnAddr7:QWORD
EXTERN g_FixClothPhysics_ReturnAddr8:QWORD
EXTERN g_FixClothPhysics_ReturnAddr9:QWORD
EXTERN g_FixClothPhysics_ReturnAddr10:QWORD
EXTERN g_FixClothPhysics_ReturnAddr11:QWORD

.CODE
FixClothPhysicsDetour1 PROC

OriginalCode:
    mulss xmm8, xmm9 

ApplyFPSMultiplier:
    mulss xmm6, dword ptr [g_FrameRateTimeMultiplier]
    mulss xmm7, dword ptr [g_FrameRateTimeMultiplier]
    mulss xmm8, dword ptr [g_FrameRateTimeMultiplier]

    jmp qword ptr [g_FixClothPhysics_ReturnAddr1]

FixClothPhysicsDetour1 ENDP


.CODE
FixClothPhysicsDetour2 PROC

OriginalCode:
    mulss xmm2, dword ptr [rsi+0258h]

ApplyFPSMultiplier:
    mulss xmm2, dword ptr [g_FrameRateTimeMultiplier]
    mulss xmm0, dword ptr [g_FrameRateTimeMultiplier]

    jmp qword ptr [g_FixClothPhysics_ReturnAddr2]

FixClothPhysicsDetour2 ENDP


.CODE
FixClothPhysicsDetour3 PROC

ApplyFPSMultiplier:
    mulss xmm3, dword ptr [g_FrameRateTimeMultiplier]

OriginalCode:
    subss xmm0, xmm3
    mov [rsp+00000250h],r12

    jmp qword ptr [g_FixClothPhysics_ReturnAddr3]

FixClothPhysicsDetour3 ENDP


.CODE
FixClothPhysicsDetour4 PROC

OriginalCode:
    movss xmm1, dword ptr [r14+000000A4h]

ApplyFPSMultiplier:
    mulss xmm0, dword ptr [g_FrameRateTimeMultiplier]
    mulss xmm1, dword ptr [g_FrameRateTimeMultiplier]

    jmp qword ptr [g_FixClothPhysics_ReturnAddr4]

FixClothPhysicsDetour4 ENDP


.CODE
FixClothPhysicsDetour5 PROC

OriginalCode:
    movss xmm0, dword ptr [r14+000000A8h]

ApplyFPSMultiplier:
    mulss xmm0, dword ptr [g_FrameRateTimeMultiplier]

    jmp qword ptr [g_FixClothPhysics_ReturnAddr5]

FixClothPhysicsDetour5 ENDP


.CODE
FixClothPhysicsDetour6 PROC

ApplyFPSMultiplier:
    mulss xmm1, dword ptr [g_FrameRateTimeMultiplier]

OriginalCode:
    movss xmm1, dword ptr [rbp-080h]

    jmp qword ptr [g_FixClothPhysics_ReturnAddr6]

FixClothPhysicsDetour6 ENDP


.CODE
FixClothPhysicsDetour7 PROC

ApplyFPSMultiplier:
    mulss xmm0, dword ptr [g_FrameRateTimeMultiplier]

OriginalCode:
    movss xmm0, dword ptr [rbp-078h]

    jmp qword ptr [g_FixClothPhysics_ReturnAddr7]

FixClothPhysicsDetour7 ENDP


.CODE
FixClothPhysicsDetour8 PROC

ApplyFPSMultiplier:
    mulss xmm2, dword ptr [g_FrameRateTimeMultiplier]

OriginalCode:
    movss xmm2, dword ptr [rbp-07Ch]

    jmp qword ptr [g_FixClothPhysics_ReturnAddr8]

FixClothPhysicsDetour8 ENDP


.CODE
FixClothPhysicsDetour9 PROC

OriginalCode:
    movss dword ptr [rbp+00h], xmm1

ApplyFPSMultiplier:
    mulss xmm1, dword ptr [g_FrameRateTimeMultiplier]

    jmp qword ptr [g_FixClothPhysics_ReturnAddr9]

FixClothPhysicsDetour9 ENDP


.CODE
FixClothPhysicsDetour10 PROC

OriginalCode:
    movss dword ptr [rbp-010h], xmm1

ApplyFPSMultiplier:
    mulss xmm1, dword ptr [g_FrameRateTimeMultiplier]

    jmp qword ptr [g_FixClothPhysics_ReturnAddr10]

FixClothPhysicsDetour10 ENDP


.CODE
FixClothPhysicsDetour11 PROC
    push rax
    sub rsp, 28h
    movdqu [rsp+10h], xmm0 

OriginalCode:
    mov rax, 1404C5F40h
    movss xmm0, dword ptr [rax]
    mulss xmm9, xmm0

    movdqu xmm0, [rsp+10h]
    add rsp, 28h
    pop rax
    
ApplyFPSMultiplier:
    mulss xmm9, dword ptr [g_FrameRateTimeMultiplier]

    jmp qword ptr [g_FixClothPhysics_ReturnAddr11]

FixClothPhysicsDetour11 ENDP


END
