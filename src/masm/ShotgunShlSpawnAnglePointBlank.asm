INCLUDE CommonMacros.inc
.DATA
EXTERN g_ShotgunShlSpawnAnglePointBlank_ReturnAddr1:QWORD
EXTERN g_ShotgunShlSpawnAnglePointBlank_ReturnAddr2:QWORD
EXTERN g_ShotgunShlSpawnAnglePointBlankCheckCall:QWORD

.CODE
ShotgunShlSpawnAnglePointBlankDetour PROC
	PushAllRegs
	sub		rsp, 20h ; Shadow space for the call
	mov rcx, rbx ; playerAddr in rbx
	call qword ptr [g_ShotgunShlSpawnAnglePointBlankCheckCall] ; Check if in Backslide
	add rsp, 20h
	cmp al, 1
	je AlterAngle
	PopAllRegs
	jmp OriginalCode
	
AlterAngle:
	PopAllRegs
	add cx,[rbx+0C0h]
	add cx, 8000h ; Add 180 degrees to the angle
	jmp [g_ShotgunShlSpawnAnglePointBlank_ReturnAddr1]

OriginalCode:
	add cx,[rbx+0C0h]
	jmp [g_ShotgunShlSpawnAnglePointBlank_ReturnAddr1]
ShotgunShlSpawnAnglePointBlankDetour ENDP


.CODE
ShotgunShlSpawnAnglePointBlankDetour2 PROC
	PushAllRegs
	sub		rsp, 20h ; Shadow space for the call
	mov rcx, rbx ; playerAddr in rbx
	call qword ptr [g_ShotgunShlSpawnAnglePointBlankCheckCall] ; Check if in Backslide
	add rsp, 20h
	cmp al, 1
	je AlterAngle
	PopAllRegs
	jmp OriginalCode
	
AlterAngle:
	PopAllRegs
	add r14w,[rbx+0C0h]
	add r14w, 8000h ; Add 180 degrees to the angle
	jmp [g_ShotgunShlSpawnAnglePointBlank_ReturnAddr2]

OriginalCode:
	add r14w,[rbx+0C0h]
	jmp [g_ShotgunShlSpawnAnglePointBlank_ReturnAddr2]
ShotgunShlSpawnAnglePointBlankDetour2 ENDP

END