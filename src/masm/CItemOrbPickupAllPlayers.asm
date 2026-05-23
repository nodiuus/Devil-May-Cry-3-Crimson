INCLUDE CommonMacros.inc

.DATA
EXTERN g_CItemOrbPickupAllPlayers_ReturnAddr:QWORD
EXTERN g_CItemOrbPickupAllPlayers_ReturnAddr2:QWORD
EXTERN g_CItemOrbPickupAllPlayersCheckCall:QWORD   
EXTERN g_CItemOrbPickupAllPlayers_BoneMatrixCall:QWORD

.CODE
; From CItemOrbBehavior_sub_1401B61C0:
; dmc3.exe+1B6344 - 80 B9 99 3E 00 00 01 - cmp byte ptr [rcx+00003E99],01 { compare player->isDead }
CItemOrbPickupAllPlayersDetour PROC

LoadNewPlayer:
    PushAllRegsExcept rcx
    lea rdx, [rdi+0E0h] ; CItemOrb->positionVec4
    sub rsp, 28h  
    call qword ptr [g_CItemOrbPickupAllPlayersCheckCall]  ; find playerAddr closest to the orb position
    add rsp, 28h
    mov rcx, rax
    PopAllRegsExcept rcx
    
OriginalCode:    
    cmp byte ptr [rcx+03E99h], 01  ; compare player->isDead 
    jmp qword ptr [g_CItemOrbPickupAllPlayers_ReturnAddr]

CItemOrbPickupAllPlayersDetour ENDP

; From CItemOrbAfterPickup_sub_1401B6910:
; dmc3.exe+1B6990 - E8 FB 40 04 00 - call dmc3.CPlFetchBoneMatrix_sub_1401FAA90
CItemOrbPickupAllPlayersDetour2 PROC

LoadNewPlayer:
    PushAllRegsExcept rcx
    lea rdx, [rsi+0E0h] ; CItemOrb->positionVec4
    sub rsp, 28h  
    call qword ptr [g_CItemOrbPickupAllPlayersCheckCall]  ; find playerAddr closest to the orb position
    add rsp, 28h
    mov rcx, rax
    PopAllRegsExcept rcx

OriginalCode:    
    mov rbx, rcx
    mov edx, 2
    call qword ptr [g_CItemOrbPickupAllPlayers_BoneMatrixCall] ; CPlFetchBoneMatrix_sub_1401FAA90
    ; rax = bone matrix ptr

    jmp qword ptr [g_CItemOrbPickupAllPlayers_ReturnAddr2]

CItemOrbPickupAllPlayersDetour2 ENDP
END