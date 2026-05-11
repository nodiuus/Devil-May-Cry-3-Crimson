#include <random>

#include "CrimsonFastcallDetours.hpp"
#include "Internal.hpp"
#include "File.hpp"
#include "Utility/Detour.hpp"
#include "Global.hpp"
#include "../Core/Core.hpp"
#include <memory>
#include <array>
#include "imgui.h"
#include <cassert>
#include "Vars.hpp"
#include "CrimsonConfig.hpp"
using namespace Utility;
namespace CrimsonFastcallDetours{

 static constexpr auto DANTE_TAUNT_OFFSET() { return 0x1FE860; }
 static constexpr auto VERGIL_TAUNT_OFFSET() { return 0x21A220; }
 static std::unique_ptr<Utility::Detour_t> s_DanteTauntHook;
 static std::unique_ptr<Utility::Detour_t> s_VergilTauntHook;
 std::random_device randomDevice;
 std::mt19937 rng(randomDevice());

 /// <summary>
 /// Returns a uniformly distributed random integer in [0, range) using Mersenne Twister.
 /// Please let`s not use rand() for this.
 /// </summary>
 unsigned bounded_rand(unsigned range)
 {
     std::uniform_int_distribution<unsigned> dist(0, range - 1);
     return dist(rng);
 }

 static uintptr_t __fastcall CPlDanteTauntController_sub_1401FE860(byte8* actorBaseAddr) {
 	typedef uintptr_t (__fastcall *DanteTauntTrampoline)(byte8*);
    auto& extramovemode = activeCrimsonGameplay.Gameplay.General.extramoves;

 	uintptr_t trampoline_raw = s_DanteTauntHook->GetTrampoline();



    uintptr_t res = NULL;
        
    DanteTauntTrampoline trampoline = (DanteTauntTrampoline)trampoline_raw;


    if (!actorBaseAddr) {
        return res;
    }

    auto& actorData = *reinterpret_cast<PlayerActorData*>(actorBaseAddr);

//  	if (!extramovemode) {
//         res = trampoline(actorBaseAddr);
//  		return res;
//  	}
    int stylerank = actorData.styleData.rank;
    int motionbank = bounded_rand(2);
    if (motionbank < 0 || motionbank > 2) {
        Log("Error in taunt rand function");
        res = trampoline(actorBaseAddr);
        return res;
    }
    if (actorData.recoverState[0] == 0) {
        //if (motionbank == 0) {
        //    actorData.motionArchives[MOTION_GROUP_DANTE::TAUNTS] = File_staticFiles[pl000_00_2]; 
        //    res = trampoline(actorBaseAddr);
        //    return res;
        //}
        if (motionbank == 0) {
            actorData.motionArchives[MOTION_GROUP_DANTE::TAUNTS] = newTauntDanteAnims1_pl000_00_2; // Taunt Bank 1
            actorData.recoverState[0] = 1;
        }
        if (motionbank == 1) {
            actorData.motionArchives[MOTION_GROUP_DANTE::TAUNTS] = newTauntDanteAnims2_pl000_00_2; // Taunt Bank 2
        }
        actorData.recoverState[0] = 1;
        switch (stylerank) {
        case STYLE_RANK::BLAST:
        case STYLE_RANK::ALRIGHT:
            PlayAnimation_1EFB90(actorBaseAddr, MOTION_GROUP_DANTE::TAUNTS, 1, -1.0f, -1, 2, 5);
            break;
        case STYLE_RANK::SWEET:
        case STYLE_RANK::SHOWTIME:
            PlayAnimation_1EFB90(actorBaseAddr, MOTION_GROUP_DANTE::TAUNTS, 2, -1.0f, -1, 2, 5);
            break;
        case STYLE_RANK::STYLISH:
            PlayAnimation_1EFB90(actorBaseAddr, MOTION_GROUP_DANTE::TAUNTS, 3, -1.0f, -1, 2, 0);
            break;
        default:
            PlayAnimation_1EFB90(actorBaseAddr, MOTION_GROUP_DANTE::TAUNTS, 0, -1.0f, -1, 2, 5);
            break;
        }
        return res;
    }


    res = trampoline(actorBaseAddr);
 	return res;
 }


 static uintptr_t __fastcall CPlNewVergilTauntController_sub_14021A220(byte8* actorBaseAddr) {
     typedef uintptr_t(__fastcall* VergilTauntTrampoline)(byte8*);
     auto& extramovemode = activeCrimsonGameplay.Gameplay.General.extramoves;

     uintptr_t trampoline_raw = s_VergilTauntHook->GetTrampoline();



     uintptr_t res = NULL;

     VergilTauntTrampoline trampoline = (VergilTauntTrampoline)trampoline_raw;


     if (!actorBaseAddr) {
         return res;
     }

     auto& actorData = *reinterpret_cast<PlayerActorDataVergil*>(actorBaseAddr);
     auto meleeWeaponEquipped = actorData.newWeapons[actorData.activeMeleeWeaponIndex];


     
     if (!extramovemode) {
         actorData.motionArchives[MOTION_GROUP_VERGIL::TAUNTS] = File_staticFiles[pl021_00_2];
         res = trampoline(actorBaseAddr);
         return res;
     }
     if (actorData.recoverState[0] == 0){
            if (meleeWeaponEquipped != WEAPON::YAMATO_VERGIL) {
            int fileIndex = bounded_rand(2);
            if (meleeWeaponEquipped == WEAPON::BEOWULF_VERGIL) {
                actorData.motionArchives[MOTION_GROUP_VERGIL::TAUNTS] = newTauntVergilAnims2_pl021_00_2;
            }
            //force edge
            else {
                actorData.motionArchives[MOTION_GROUP_VERGIL::TAUNTS] = newTauntVergilAnims1_pl021_00_2;
            }
            PlayAnimation_1EFB90(actorBaseAddr, MOTION_GROUP_VERGIL::TAUNTS, fileIndex, -1.0f, -1, 2, 5);
            actorData.recoverState[0] = 1;
            return res;
            }
            //yamato vanilla functionality
            else {
                actorData.motionArchives[MOTION_GROUP_VERGIL::TAUNTS] = File_staticFiles[pl021_00_2];
                res = trampoline(actorBaseAddr);
                return res;
            }
     }
     res = trampoline(actorBaseAddr);
     return res;
 }

 void ModdedTauntDetour() {
 	s_DanteTauntHook =
 		std::make_unique<Utility::Detour_t>(
 			(uintptr_t)appBaseAddr + DANTE_TAUNT_OFFSET(),
 			(uintptr_t)&CPlDanteTauntController_sub_1401FE860,
 			NULL, "dante_taunt_detour");
 	bool res = s_DanteTauntHook->Toggle();
 	assert(res);
 }


 void ModdedTauntVergilDetour() {
     s_VergilTauntHook =
         std::make_unique<Utility::Detour_t>(
             (uintptr_t)appBaseAddr + VERGIL_TAUNT_OFFSET(),
             (uintptr_t)&CPlNewVergilTauntController_sub_14021A220,
             NULL, "vergil_taunt_detour");
     bool res = s_VergilTauntHook->Toggle();
     assert(res);
 }


 void InitDetours() {
     ModdedTauntDetour();
     ModdedTauntVergilDetour();
 }
}
// void LdkDrawImGuiWidget() {
// 	ImGui::Checkbox("Enable LDK", &s_ldkEnable);
// 	ImGui::Checkbox("Bosses LDK", &s_ldkBosses);
// 	ImGui::InputInt("Enemy multiplier", &s_dudeMultiplier);
// 	if(s_ldkBosses) {
// 		ImGui::InputInt("Bosses multipler", &s_bossMultiplier);
// 	}
// }
