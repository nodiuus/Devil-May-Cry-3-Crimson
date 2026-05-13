INCLUDE CommonMacros.inc

.DATA
EXTERN g_FixEnemyAttackCooldowns_ReturnAddr1:QWORD
EXTERN g_FixEnemyAttackCooldowns_ReturnAddr2:QWORD
EXTERN g_FrameRateTimeMultiplier:DWORD
constOne dd 1.0

.CODE
; From EnemySlottingControl_sub_1401C9A30:
; dmc3.exe+1C9A56 - F3 0F 10 05 42 93 B2 00 - movss xmm0,[dmc3.exe+CF2DA0] { Enemy Slotting Cooldown }
FixEnemyAttackCooldownsDetour1 PROC
    
    movss xmm0, dword ptr [constOne] 
    mulss xmm0, dword ptr [g_FrameRateTimeMultiplier] ; Multiply the cooldown by the frame rate multiplier to keep it consistent across different frame rates
    jmp qword ptr [g_FixEnemyAttackCooldowns_ReturnAddr1]

FixEnemyAttackCooldownsDetour1 ENDP
; From CEm013UpdateEnigmaAttackBehavior_sub_1400D8CB0:
; dmc3.exe+D90C6 - F3 41 0F5C C0         - subss xmm0,xmm8 { Enigma Time Between Charges for BlueandRedAttacks }
FixEnemyAttackCooldownsDetour2 PROC

    mulss xmm8, dword ptr [g_FrameRateTimeMultiplier] 
    subss xmm0, xmm8
    jmp qword ptr [g_FixEnemyAttackCooldowns_ReturnAddr2]

FixEnemyAttackCooldownsDetour2 ENDP
END