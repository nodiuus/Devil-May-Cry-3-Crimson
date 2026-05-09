INCLUDE CommonMacros.inc

.DATA
EXTERN g_CheckMissionResultBPScreen_ReturnAddr1:QWORD
EXTERN g_CheckMissionResultBPScreen_ReturnAddr2:QWORD
EXTERN g_CheckMissionResultBPScreen_ReturnAddr3:QWORD
EXTERN g_CheckMissionResultBPScreen_ReturnAddr4:QWORD
EXTERN g_CheckMissionResultBPScreen_TargetAddr1:QWORD
EXTERN g_CheckMissionResultBPScreen_TargetAddr2:QWORD
EXTERN g_CheckMissionResultBPScreen_TargetAddr3:QWORD
EXTERN g_CheckMissionResultBPScreen_TargetAddr4:QWORD
EXTERN g_inMissionResultBPScreen:WORD

.CODE
; From CUIDResultBPUpdate_sub_1402A5350:
; dmc3.exe+2A5378 - E8 93 00 00 00 - call dmc3.sub_1402A5410 { begin results BP }
CheckMissionResultBPScreenDetour1 PROC

    mov word ptr [g_inMissionResultBPScreen], 1
    call qword ptr [g_CheckMissionResultBPScreen_TargetAddr1]
    jmp qword ptr [g_CheckMissionResultBPScreen_ReturnAddr1]

CheckMissionResultBPScreenDetour1 ENDP

; dmc3.exe+2A53AA - E8 41 06 00 00 - call dmc3.sub_1402A59F0 { process results BP }
CheckMissionResultBPScreenDetour2 PROC

    mov word ptr [g_inMissionResultBPScreen], 1
    call qword ptr [g_CheckMissionResultBPScreen_TargetAddr2]
    jmp qword ptr [g_CheckMissionResultBPScreen_ReturnAddr2]

CheckMissionResultBPScreenDetour2 ENDP

; dmc3.exe+2A53B4 - E8 77 0B 00 00 - call dmc3.sub_1402A5F30 { leaving results BP }
CheckMissionResultBPScreenDetour3 PROC

    mov word ptr [g_inMissionResultBPScreen], 0
    call qword ptr [g_CheckMissionResultBPScreen_TargetAddr3]
    jmp qword ptr [g_CheckMissionResultBPScreen_ReturnAddr3]

CheckMissionResultBPScreenDetour3 ENDP

; dmc3.exe+2A53BE - E8 ED 05 00 00 - call dmc3.sub_1402A59B0 { process save screen BP }
CheckMissionResultBPScreenDetour4 PROC

    mov word ptr [g_inMissionResultBPScreen], 0
    call qword ptr [g_CheckMissionResultBPScreen_TargetAddr4]
    jmp qword ptr [g_CheckMissionResultBPScreen_ReturnAddr4]

CheckMissionResultBPScreenDetour4 ENDP

END