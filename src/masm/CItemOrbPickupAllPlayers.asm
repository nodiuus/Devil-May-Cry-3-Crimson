INCLUDE CommonMacros.inc

.DATA
EXTERN g_CItemOrbPickupAllPlayers_ReturnAddr:QWORD
EXTERN g_CItemOrbPickupAllPlayersCheckCall:QWORD           ; Uncomment if calling C++ functions from ASM

.CODE
; From CItemOrbBehavior_sub_1401B61C0:
; dmc3.exe+1B6344 - 80 B9 99 3E 00 00 01 - cmp byte ptr [rcx+00003E99],01 { compare player->isDead }
CItemOrbPickupAllPlayersDetour PROC

LoadNewPlayer:
    PushAllRegsExcept rcx
    lea rdx, [rdi+0E0h] ; CItemOrb->positionVec4
    add rsp, 28h  
    call qword ptr [g_CItemOrbPickupAllPlayersCheckCall]  
    sub rsp, 28h
    mov rcx, rax
    PopAllRegsExcept rcx
    
OriginalCode:    
    cmp byte ptr [rcx+03E99h], 01  ; compare player->isDead 
    jmp qword ptr [g_CItemOrbPickupAllPlayers_ReturnAddr]

CItemOrbPickupAllPlayersDetour ENDP
END