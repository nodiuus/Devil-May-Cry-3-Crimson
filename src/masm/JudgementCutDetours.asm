INCLUDE CommonMacros.inc
.DATA
EXTERN g_JudgementCutSpeed_ReturnAddr:QWORD
EXTERN g_JudgementCutStartDelayCall:QWORD
EXTERN g_JudgementCutSpawnCollisionCall:QWORD
EXTERN g_JudgementCutCheckJustFrameCall:QWORD

.CODE
JudgementCutSpeedDetour PROC
	PushAllRegs
	sub		rsp, 20h ; Shadow space for the call
	mov rcx, qword ptr [rcx+530h] ; playerAddr in rcx (shlActorAddr) + 530h
	call qword ptr [g_JudgementCutCheckJustFrameCall] ; Check if in JustFrameJDC
	add rsp, 20h
	cmp al, 1
	je ChangeDelay
	PopAllRegs
	jmp OriginalCode
	
ChangeDelay:
	PopAllRegs
	xor r11, r11
	mov r11, 42a00000h ; 80.0f in hex
	mov qword ptr [rcx+528h], r11 ; Set jdcDelay to 80.0f
	jmp OriginalCode

OriginalCode:
	jmp [g_JudgementCutStartDelayCall]

JudgementCutSpeedDetour ENDP


END