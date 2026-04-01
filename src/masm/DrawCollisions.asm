INCLUDE CommonMacros.inc
.DATA
EXTERN g_DrawCollisions_ReturnAddr:QWORD
EXTERN g_DrawCollisionsCall:QWORD

.CODE
DrawCollisionsDetour PROC
	PushAllRegsAndXmm
	sub		rsp, 20h ; Shadow space for the call
	call qword ptr [g_DrawCollisionsCall] ; draw the collisions
	add rsp, 20h
	jmp OriginalCode

OriginalCode:
	PopAllRegsAndXmm
	movss dword ptr [rbx + 140h], xmm1
	jmp [g_DrawCollisions_ReturnAddr]
DrawCollisionsDetour ENDP

END