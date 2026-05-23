INCLUDE CommonMacros.inc

.DATA
EXTERN g_NoAirLunarPhaseLift_ReturnAddr:QWORD
EXTERN g_NoAirLunarPhaseLiftCheckCall:QWORD           ; Uncomment if calling C++ functions from ASM
EXTERN g_NoAirLunarPhaseLiftSettingAddr:QWORD
zeroFloat dd 0.0

.CODE
; From ExecuteLunarPhaseMove_sub_14021DE20:
; dmc3.exe+21E34A - F3 0F 10 98 70 01 00 00 - movss xmm3,[rax+00000170] { movePart 0 yInertiaMult }
; dmc3.exe+21E352 - F3 0F 10 90 6C 01 00 00 - movss xmm2,[rax+0000016C] { movePart 0 yInertia }
NoAirLunarPhaseLiftDetour PROC

CheckIfEnabled:
    push rax
    mov rax, qword ptr [g_NoAirLunarPhaseLiftSettingAddr] ; Get pointer to NoAirLunarPhaseLift setting
    cmp byte ptr [rax], 0
    pop rax
    je OriginalCode ; If the setting is disabled, jump to original code

CheckIfInAirLunarPhase:
    PushAllRegsNoFlags
    sub rsp, 20h
    call qword ptr [g_NoAirLunarPhaseLiftCheckCall] ; Call the function to check if we're in the air lunar phase
    add rsp, 20h
    test al, al
    PopAllRegsNoFlags
    je OriginalCode
    movss xmm3, dword ptr [zeroFloat] ; Set yInertiaMult to 0
    movss xmm2, dword ptr [zeroFloat] ; Set yInertia to 0
    jmp JmpOut

OriginalCode:
    movss xmm3, dword ptr [rax + 170h] ; Load yInertiaMult into xmm3 -- -.0.35f default
    movss xmm2, dword ptr [rax + 16Ch] ; Load yInertia into xmm2 -- 6.83f default

JmpOut:
    jmp qword ptr [g_NoAirLunarPhaseLift_ReturnAddr]

NoAirLunarPhaseLiftDetour ENDP
END