INCLUDE CommonMacros.inc
.DATA
EXTERN g_ShotgunShlSpawnAnglePointBlank_ReturnAddr1:QWORD
EXTERN g_ShotgunShlSpawnAnglePointBlank_ReturnAddr2:QWORD
EXTERN g_ShotgunShlSpawnAnglePointBlankCheckCall:QWORD
EXTERN g_PointBlankShotgunFire_ReturnAddr:QWORD
EXTERN g_PointBlankShotgunFireTailCall_ReturnAddr:QWORD
EXTERN g_PointBlankShotgunFireDelayCall:QWORD
EXTERN g_PointBlankShotgunFireOgCall:QWORD
EXTERN g_PointBlankShotgunCancelAnimTailCall:QWORD

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


.CODE
PointBlankShotgunFireDetour PROC
	PushAllRegs
	sub		rsp, 20h ; Shadow space for the call
	call qword ptr [g_ShotgunShlSpawnAnglePointBlankCheckCall] ; Check if in Backslide
	add rsp, 20h
	cmp al, 1
	je DelayCall
	PopAllRegs
	jmp OriginalCode
	
DelayCall:
	PopAllRegs
	sub		rsp, 20h ; Shadow space for the call
	call qword ptr [g_PointBlankShotgunFireDelayCall] ; Queue the delay function to prevent the shotgun from firing on his foot
	add rsp, 20h
	jmp [g_PointBlankShotgunFire_ReturnAddr]

OriginalCode:
	call [g_PointBlankShotgunFireOgCall]
	jmp [g_PointBlankShotgunFire_ReturnAddr]
PointBlankShotgunFireDetour ENDP


.CODE
PointBlankShotgunFireTailCallDetour PROC
	PushAllRegs
	sub		rsp, 20h ; Shadow space for the call
	call qword ptr [g_ShotgunShlSpawnAnglePointBlankCheckCall] ; Check if in Backslide
	add rsp, 20h
	cmp al, 1
	je DelayCall
	PopAllRegs
	jmp OriginalCode
	
DelayCall:
	PopAllRegs
	PushAllRegs
	sub		rsp, 20h ; Shadow space for the call
	call qword ptr [g_PointBlankShotgunFireDelayCall] ; Queue the delay function to prevent the shotgun from firing on his foot
	add rsp, 20h
	PopAllRegs
	jmp [g_PointBlankShotgunCancelAnimTailCall] ; Jump to the tail call that cancels the shotgun fire animation to prevent it from playing on his foot

OriginalCode:
	jmp [g_PointBlankShotgunFireOgCall]
PointBlankShotgunFireTailCallDetour ENDP

END