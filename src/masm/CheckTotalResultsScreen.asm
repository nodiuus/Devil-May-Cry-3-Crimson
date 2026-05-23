INCLUDE CommonMacros.inc

.DATA
EXTERN g_CheckTotalResultsScreen_ReturnAddr:QWORD
EXTERN g_inTotalResultsScreen:WORD

.CODE
; From CUIDTotalRankingUpdate_sub_1402BE980:
; dmc3.exe+2BE989 - 0F B6 49 08 - movzx ecx,byte ptr [rcx+08]
; dmc3.exe+2BE98D - 85 C9                 - test ecx,ecx
CheckTotalResultsScreenDetour PROC

    mov word ptr [g_inTotalResultsScreen], 1 ; Set the flag to indicate we're in the total results screen
    movzx ecx, byte ptr [rcx+08h]
    test ecx, ecx
    jmp qword ptr [g_CheckTotalResultsScreen_ReturnAddr]

CheckTotalResultsScreenDetour ENDP
END