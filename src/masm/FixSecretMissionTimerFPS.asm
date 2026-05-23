.DATA
EXTERN g_FixSecretMissionTimerFPS_ReturnAddr:QWORD
EXTERN g_FrameRateTimeMultiplier:DWORD
EXTERN g_FrameRateTimeMultiplierRounded:DWORD

; From CUIDCockpit00_SetSecretMissionTimer_sub_14027C0A0:
; dmc3.exe+27C0DD - F3 0F 11 81 48 69 00 00   - movss [rcx+00006948],xmm0 { Setting secret mission timer }
.CODE
FixSecretMissionTimerFPSDetour PROC
    ; xmm0 = original timer ticks (ex: 3600)
    ;mulss xmm0, dword ptr [g_SecretMissionTimerScale]

OriginalCode:
    movss dword ptr [rcx+6948h], xmm0
    jmp qword ptr [g_FixSecretMissionTimerFPS_ReturnAddr]

FixSecretMissionTimerFPSDetour ENDP

.DATA
EXTERN g_FixSecretMissionTimerFPS_ReturnAddr2:QWORD
EXTERN g_FrameRateTimeMultiplier:DWORD

; From CUIDCockpit00_ControlSecretMissionTimer_sub_14027EEC0:
; dmc3.exe+27EF97 - 44 8B 15 62104B05 - mov r10d,[145730000] { (holding FrameRate value) }
.CODE
FixSecretMissionTimerFPSDetour2 PROC 
    mov r10d, 60d ; Lock this to 60 FPS, which is the original intended frame rate for the timer;
    jmp    qword ptr [g_FixSecretMissionTimerFPS_ReturnAddr2]

FixSecretMissionTimerFPSDetour2 ENDP
END