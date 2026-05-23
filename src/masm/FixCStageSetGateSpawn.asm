INCLUDE CommonMacros.inc
.DATA
EXTERN g_FixCStageSetGateSpawn_ReturnAddr1:QWORD
EXTERN g_FixCStageSetGateSpawn_ReturnAddr2:QWORD
EXTERN g_FixCStageSetGateSpawn_ReturnAddr3:QWORD
EXTERN g_FixCStageSetGateSpawn_ReturnAddr4:QWORD

newSpeed dd 1.0f

.CODE
FixCStageSetGateSpawnDetour1 PROC

MakeSpeedConsistent:
    movss xmm1, dword ptr [newSpeed]
    jmp qword ptr [g_FixCStageSetGateSpawn_ReturnAddr1]

;OriginalCode:
    ;movss xmm1, dword ptr [rdi+14] ; gateaddr in rdi, speed in [rdi+14]

FixCStageSetGateSpawnDetour1 ENDP

.CODE
FixCStageSetGateSpawnDetour2 PROC

MakeSpeedConsistent:
    movss xmm1, dword ptr [newSpeed]
    jmp qword ptr [g_FixCStageSetGateSpawn_ReturnAddr2]

;OriginalCode:
    ;movss xmm1, dword ptr [rdi+14] ; gateaddr in rdi, speed in [rdi+14]

FixCStageSetGateSpawnDetour2 ENDP


.CODE
FixCStageSetGateSpawnDetour3 PROC

MakeSpeedConsistent:
    movss xmm1, dword ptr [newSpeed]
    jmp qword ptr [g_FixCStageSetGateSpawn_ReturnAddr3]

;OriginalCode:
    ;movss xmm1, dword ptr [rdi+14] ; gateaddr in rdi, speed in [rdi+14]

FixCStageSetGateSpawnDetour3 ENDP


.CODE
FixCStageSetGateSpawnDetour4 PROC

MakeSpeedConsistent:
    movss xmm1, dword ptr [newSpeed]
    jmp qword ptr [g_FixCStageSetGateSpawn_ReturnAddr4]

;OriginalCode:
    ;movss xmm1, dword ptr [rdi+14] ; gateaddr in rdi, speed in [rdi+14]

FixCStageSetGateSpawnDetour4 ENDP

END