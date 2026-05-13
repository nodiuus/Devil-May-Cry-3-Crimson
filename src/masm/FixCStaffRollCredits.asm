INCLUDE CommonMacros.inc

.DATA
EXTERN g_CStaffRollCreditsFix_ReturnAddr1:QWORD
EXTERN g_CStaffRollCreditsFix_ReturnAddr2:QWORD
EXTERN g_FrameRateTimeMultiplier:DWORD

.CODE
; From CStaffRollUpdate_sub_1402454A0:
; dmc3.exe+2454D3 - F3 0F 58 0D 91 80 11 00 - addss xmm1,[dmc3.exe+35D56C] { (1.00) -- CStaffRollSpeed }
CStaffRollCreditsFixDetour1 PROC
    
    addss xmm1, dword ptr [g_FrameRateTimeMultiplier] ; Add the frame rate time multiplier to the staff roll speed to keep it consistent across different frame rates
    jmp qword ptr [g_CStaffRollCreditsFix_ReturnAddr1]

CStaffRollCreditsFixDetour1 ENDP
; From ManageCEventMission_sub_1401A6510:
; dmc3.exe+1A6CFF - F3 0F 5C 0D 65 68 1B 00 - subss xmm1,[dmc3.exe+35D56C] { (1.00) -- Mission20CreditsEndMissionTime }
CStaffRollCreditsFixDetour2 PROC
    
    subss xmm1, dword ptr [g_FrameRateTimeMultiplier] ; Subtract the frame rate time multiplier from the mission end time to keep it consistent across different frame rates
    jmp qword ptr [g_CStaffRollCreditsFix_ReturnAddr2]

CStaffRollCreditsFixDetour2 ENDP
END