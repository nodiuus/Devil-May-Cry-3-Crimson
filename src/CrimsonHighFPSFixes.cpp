#include "../ThirdParty/glm/glm.hpp"
#include <algorithm>
#include "Core/Core.hpp"
#include <stdio.h>
#include "Utility/Detour.hpp"
#include <intrin.h>
#include <string>
#include "CrimsonUtil.hpp"
#include "DMC3Input.hpp"
#include "Global.hpp"

#include "Core/Macros.h"
#include "Config.hpp"
#include <iostream>
#include "CrimsonPatches.hpp"
#include "CrimsonLDK.hpp"
#include "Actor.hpp"
#include "CrimsonHighFPSFixes.hpp"

namespace CrimsonHighFPSFixes {

extern "C" {
	// FixSecretMissionTimerFPS
// 	std::uint64_t g_FixSecretMissionTimerFPS_ReturnAddr;
// 	void FixSecretMissionTimerFPSDetour();
// 	std::uint64_t g_FixSecretMissionTimerFPS_ReturnAddr2;
// 	void FixSecretMissionTimerFPSDetour2();

	// FixBlendingEffects
	std::uint64_t g_FixBlendingEffects_Mist_ReturnAddr1;
	std::uint64_t g_FixBlendingEffects_Mist_ReturnAddr2;
	std::uint64_t g_FixBlendingEffects_Mist_ReturnAddr3;
	std::uint64_t g_MistBlendingEffect_SpdAddr1;
	std::uint64_t g_MistBlendingEffect_SpdAddr2;
	void FixBlendingEffect_Mist_Detour1();
	void FixBlendingEffect_Mist_Detour2();
	void FixBlendingEffect_Mist_Detour3();

	std::uint64_t g_FixBlendingEffects_Warp_ReturnAddr1;
	std::uint64_t g_FixBlendingEffects_Warp_ReturnAddr2;
	void FixBlendingEffect_Warp_Detour1();
	void FixBlendingEffect_Warp_Detour2();


	// FixClothPhysics
	void FixClothPhysicsDetour1();
	void FixClothPhysicsDetour2();
	void FixClothPhysicsDetour3();
	void FixClothPhysicsDetour4();
	void FixClothPhysicsDetour5();
	void FixClothPhysicsDetour6();
	void FixClothPhysicsDetour7();
	void FixClothPhysicsDetour8();
	void FixClothPhysicsDetour9();
	void FixClothPhysicsDetour10();
	void FixClothPhysicsDetour11();
	void FixClothPhysicsDetour12();
	void FixClothPhysicsDetour13();
	void FixClothPhysicsDetour14();
	void FixClothPhysicsDetour15();
	
	std::uint64_t g_FixClothPhysics_ReturnAddr1;
	std::uint64_t g_FixClothPhysics_ReturnAddr2;
	std::uint64_t g_FixClothPhysics_ReturnAddr3;
	std::uint64_t g_FixClothPhysics_ReturnAddr4;
	std::uint64_t g_FixClothPhysics_ReturnAddr5;
	std::uint64_t g_FixClothPhysics_ReturnAddr6;
	std::uint64_t g_FixClothPhysics_ReturnAddr7;
	std::uint64_t g_FixClothPhysics_ReturnAddr8;
	std::uint64_t g_FixClothPhysics_ReturnAddr9;
	std::uint64_t g_FixClothPhysics_ReturnAddr10;
	std::uint64_t g_FixClothPhysics_ReturnAddr11;
	std::uint64_t g_FixClothPhysics_ReturnAddr12;
	std::uint64_t g_FixClothPhysics_ReturnAddr13;
	std::uint64_t g_FixClothPhysics_ReturnAddr14;
	std::uint64_t g_FixClothPhysics_ReturnAddr15;

	std::uint64_t g_FixClothPhysics_Detour11_FloatConstAddr;
	std::uint64_t g_FixClothPhysics_RigidnessDanteCoatCall;

	// Flag read by Detour11 to conditionally skip velocity computation
	uint32_t g_ClothPhysicsEnhancementEnabled = 1;

	// FixBossCam
	void FixBossCamLookAtDetour(); // Fixes BossCam FollowUp Speed
	std::uint64_t g_FixBossCamLookAt_ReturnAddr;

	void FixBossCamRefocusDetour1(); 
	std::uint64_t g_FixBossCamRefocus_ReturnAddr1;

	void BossCamFOVDetour();
	std::uint64_t g_BossCamFOV_ReturnAddr;
	std::uint64_t g_BossCamFOV_ConstAddr;

	void BossCamCustomPositioningDetour();
	std::uint64_t g_BossCamCustomPositioning_ReturnAddr;
	bool* g_isMPCamActiveAddr = nullptr;

	// FixStaggerGravityInertia
	void FixStaggerGravityDetour();
	std::uint64_t g_FixStaggerGravity_ReturnAddr;

}

void BlendingEffectsSpeedFixes(bool enable) {
	using namespace Utility;

	static bool run = false;
	if (run == enable) {
		return;
	}

	// From UpdateBlendEffect_Mist_sub_14031AE10:
	// dmc3.exe + 31AF16 - F3 0F 10 0D 8A CD 1E 00 - movss xmm1, [dmc3.exe + 507CA8] { (0.00) }
	static std::unique_ptr<Utility::Detour_t> FixMistHook1 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x31AF16, &FixBlendingEffect_Mist_Detour1, 8);
	g_FixBlendingEffects_Mist_ReturnAddr1 = FixMistHook1->GetReturnAddress();
	g_MistBlendingEffect_SpdAddr1 = (uintptr_t)appBaseAddr + 0x507CA8;
	FixMistHook1->Toggle(enable);

	// dmc3.exe+31AF0E - F3 0F 59 05 96 CD 1E 00   - mulss xmm0,[dmc3.exe+507CAC] { (0.00) }
	static std::unique_ptr<Utility::Detour_t> FixMistHook2 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x31AF0E, &FixBlendingEffect_Mist_Detour2, 8);
	g_FixBlendingEffects_Mist_ReturnAddr2 = FixMistHook2->GetReturnAddress();
	g_MistBlendingEffect_SpdAddr2 = (uintptr_t)appBaseAddr + 0x507CAC;
	FixMistHook2->Toggle(enable);

	// dmc3.exe+31AF1E - F3 0F 59 35 86 CD 1E 00   - mulss xmm6,[dmc3.exe+507CAC] { (0.00) }
	static std::unique_ptr<Utility::Detour_t> FixMistHook3 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x31AF1E, &FixBlendingEffect_Mist_Detour3, 8);
	g_FixBlendingEffects_Mist_ReturnAddr3 = FixMistHook3->GetReturnAddress();
	FixMistHook3->Toggle(enable);

	// From UpdateBlendEffect_Warp_sub_14031A830:
	// dmc3.exe+31A85A - 8B 46 20              - mov eax,[rsi+20]
	// dmc3.exe+31A85D - 8B D3                 - mov edx,ebx
	static std::unique_ptr<Utility::Detour_t> FixWarpHook1 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x31A85A, &FixBlendingEffect_Warp_Detour1, 5);
	g_FixBlendingEffects_Warp_ReturnAddr1 = FixWarpHook1->GetReturnAddress();
	FixWarpHook1->Toggle(enable);

	// From UpdateBlendEffect_Warp_sub_14031A830:
	// dmc3.exe+31A862 - 8B 46 24              - mov eax,[rsi+24]
	// dmc3.exe+31A865 - 01 46 74              - add [rsi+74],eax
	static std::unique_ptr<Utility::Detour_t> FixWarpHook2 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x31A862, &FixBlendingEffect_Warp_Detour2, 6);
	g_FixBlendingEffects_Warp_ReturnAddr2 = FixWarpHook2->GetReturnAddress();
	FixWarpHook2->Toggle(enable);

	run = enable;
}

void ClothPhysicsFixes(bool enable) {
	using namespace Utility;

	static bool run = false;
	if (run == enable) {
		return;
	}

	// All detours from ClothPhysicsUpdate_sub_1402C9450:

	// dmc3.exe+2C9CDB - F3 45 0F 59 C1         - mulss xmm8,xmm9 -- try7 - 4
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook1 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2C9CDB, &FixClothPhysicsDetour1, 5);
	g_FixClothPhysics_ReturnAddr1 = ClothPhysicsHook1->GetReturnAddress();
	ClothPhysicsHook1->Toggle(enable);

	// dmc3.exe+2C9572 - F3 0F 59 96 58 02 00 00   - mulss xmm2,[rsi+00000258] { wind simulation } -- try7 - 8
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook2 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2C9572, &FixClothPhysicsDetour2, 8);
	g_FixClothPhysics_ReturnAddr2 = ClothPhysicsHook2->GetReturnAddress();
	ClothPhysicsHook2->Toggle(enable);

	// dmc3.exe+2C96FA - F3 0F 5C C3              - subss xmm0,xmm3 { very important, cloth deformation update } -- try7 - 14
	// dmc3.exe+2C96FE - 4C 89 A4 24 50 02 00 00  - mov [rsp+00000250],r12
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook3 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2C96FA, &FixClothPhysicsDetour3, 12);
	g_FixClothPhysics_ReturnAddr3 = ClothPhysicsHook3->GetReturnAddress();
	ClothPhysicsHook3->Toggle(enable);

	// dmc3.exe+2C9985 - F3 41 0F10 8E A4000000  - movss xmm1,[r14+000000A4] -- try7 - 19
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook4 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2C98A2, &FixClothPhysicsDetour4, 8);
	g_FixClothPhysics_ReturnAddr4 = ClothPhysicsHook4->GetReturnAddress();
	ClothPhysicsHook4->Toggle(enable);

	// dmc3.exe+2C9993 - F3 41 0F 10 86 A8 00 00 00  - movss xmm0,[r14+000000A8] -- try7 - 20
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook5 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2C9993, &FixClothPhysicsDetour5, 9);
	g_FixClothPhysics_ReturnAddr5 = ClothPhysicsHook5->GetReturnAddress();
	ClothPhysicsHook5->Toggle(enable);

	// dmc3.exe+2C99D2 - F3 0F 10 4D 80         - movss xmm1,[rbp-80] { another wind simulation block 2 } -- try7 - 23
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook6 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2C99D2, &FixClothPhysicsDetour6, 5);
	g_FixClothPhysics_ReturnAddr6 = ClothPhysicsHook6->GetReturnAddress();
	ClothPhysicsHook6->Toggle(enable);

	// dmc3.exe+2C99DB - F3 0F 10 45 88         - movss xmm0,[rbp-78] -- try7 - 24
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook7 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2C99DB, &FixClothPhysicsDetour7, 5);
	g_FixClothPhysics_ReturnAddr7 = ClothPhysicsHook7->GetReturnAddress();
	ClothPhysicsHook7->Toggle(enable);

	// dmc3.exe+2C99E4 - F3 0F 10 55 84         - movss xmm2,[rbp-7C] -- try7 - 25
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook8 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2C99E4, &FixClothPhysicsDetour8, 5);
	g_FixClothPhysics_ReturnAddr8 = ClothPhysicsHook8->GetReturnAddress();
	ClothPhysicsHook8->Toggle(enable);

	// dmc3.exe+2C99F1 - F3 0F 11 4D 00         - movss [rbp+00],xmm1 -- try7 - 26
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook9 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2C99F1, &FixClothPhysicsDetour9, 5);
	g_FixClothPhysics_ReturnAddr9 = ClothPhysicsHook9->GetReturnAddress();
	ClothPhysicsHook9->Toggle(enable);

	// dmc3.exe+2C9A09 - F3 0F 11 4D F0         - movss [rbp-10],xmm1 -- try7 - 28
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook10 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2C9A09, &FixClothPhysicsDetour10, 5);
	g_FixClothPhysics_ReturnAddr10 = ClothPhysicsHook10->GetReturnAddress();
	ClothPhysicsHook10->Toggle(enable);

	// dmc3.exe+2C9A52 - F3 44 0F 59 0D E5 C4 1F 00  - mulss xmm9,[dmc3.exe+4C5F40] { (10.00) } -- try7 - 32
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook11 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2C9A52, &FixClothPhysicsDetour11, 9);
	g_FixClothPhysics_ReturnAddr11 = ClothPhysicsHook11->GetReturnAddress();
	g_FixClothPhysics_Detour11_FloatConstAddr = (uintptr_t)appBaseAddr + 0x4C5F40;
	ClothPhysicsHook11->Toggle(enable);

	run = enable;
}

void ClothPhysicsEnhancementFixes(bool enable) {
	using namespace Utility;

	static bool run = false;
	if (run == enable) {
		return;
	}

	// dmc3.exe+2C9ADA - F3 41 0F 58 F2         - addss xmm6,xmm10 { Cloth relaxation 1 } -- try7 - 35
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook12 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2C9ADA, &FixClothPhysicsDetour12, 5);
	g_FixClothPhysics_ReturnAddr12 = ClothPhysicsHook12->GetReturnAddress();
	ClothPhysicsHook12->Toggle(enable);

	// dmc3.exe+2C9AE5 - F3 41 0F 58 FB         - addss xmm7,xmm11 { cloth relaxation 2 } -- try7 - 36
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook13 =
		std::make_unique<Utility::Detour_t>((uintptr_t)appBaseAddr + 0x2C9AE5, &FixClothPhysicsDetour13, 5);
	g_FixClothPhysics_ReturnAddr13 = ClothPhysicsHook13->GetReturnAddress();
	ClothPhysicsHook13->Toggle(false);

	// dmc3.exe+2C9AF9 - F3 45 0F 58 C4         - addss xmm8,xmm12 { cloth relaxation 3 } -- try7 - 37
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook14 =
		std::make_unique<Utility::Detour_t>((uintptr_t)appBaseAddr + 0x2C9AF9, &FixClothPhysicsDetour14, 5);
	g_FixClothPhysics_ReturnAddr14 = ClothPhysicsHook14->GetReturnAddress();
	ClothPhysicsHook14->Toggle(enable);

	// From UpdateCPlDanteCoat_sub_140212070:
	// dmc3.exe+2120BF - E8 FC 7C 0B 00           - call dmc3.RigidnessCPlPlayerCoat_sub_1402C9DC0
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook15 =
		std::make_unique<Utility::Detour_t>((uintptr_t)appBaseAddr + 0x2120BF, &FixClothPhysicsDetour15, 5);
	g_FixClothPhysics_ReturnAddr15 = ClothPhysicsHook15->GetReturnAddress();
	g_FixClothPhysics_RigidnessDanteCoatCall = (uintptr_t)appBaseAddr + 0x2C9DC0;
	ClothPhysicsHook15->Toggle(enable);

	run = enable;
}

void ClothPhysicsFixesController() {
	// Always enable base cloth physics FPS fixes (detours 1-11) when above the 60 FPS range
	if (g_FrameRate >= 65) {
		ClothPhysicsFixes(true);
	}
	else {
		ClothPhysicsFixes(false); // We disable it below the range because it makes cloth physics stiffen at lower frame rates.
	}

	// Set the flag for Detour11's velocity computation
	g_ClothPhysicsEnhancementEnabled = activeCrimsonConfig.Visual.clothPhysicsEnhancement ? 1 : 0;

	// Toggle per-particle relaxation detours (12-14) based on config setting
	ClothPhysicsEnhancementFixes(activeCrimsonConfig.Visual.clothPhysicsEnhancement);
}

void BossCamFixes(bool enable) {
	using namespace Utility;
	static bool run = false;
	if (run == enable) {
		return;
	}

	// From CCameraBossLookAtTarget_sub_1402C8B80:
	// dmc3.exe+E7DDE - 48 89 44 24 40        - mov [rsp+40],rax
	static std::unique_ptr<Utility::Detour_t> BossCamLookAtHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0xE7DDE, &FixBossCamLookAtDetour, 5);
	g_FixBossCamLookAt_ReturnAddr = BossCamLookAtHook->GetReturnAddress();
	BossCamLookAtHook->Toggle(enable);

	// From CCameraBossVergilFightMovement_sub_140054490:
	// dmc3.exe + 544CD - F3 0F 5C 05 97 90 30 00 - subss xmm0, [dmc3.exe + 35D56C] { (1.00) }
	static std::unique_ptr<Utility::Detour_t> BossCamRefocusHook1 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x544CD, &FixBossCamRefocusDetour1, 8);
	g_FixBossCamRefocus_ReturnAddr1 = BossCamRefocusHook1->GetReturnAddress();
	BossCamRefocusHook1->Toggle(enable);

	// From sub_140050D70:
	// dmc3.exe+50D9F - F3 0F 59 05 79 0B 32 00   - mulss xmm0,[dmc3.exe + 371920] { fov Control }
	static std::unique_ptr<Utility::Detour_t> BossCamFOVHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x50D9F, &BossCamFOVDetour, 8);
	g_BossCamFOV_ReturnAddr = BossCamFOVHook->GetReturnAddress();
	g_BossCamFOV_ConstAddr = (uintptr_t)appBaseAddr + 0x371920;
	g_ActiveFOVMultSettingAddr = &activeCrimsonConfig.Camera.fovMultiplier;
	BossCamFOVHook->Toggle(enable);

	// From sub_140051A70:
	// dmc3.exe+51ADD - 0F 29 87 90 01 00 00      - movaps [rdi+00000190],xmm0 { DetermineBossCamPos }
	// g_CustomCameraPos_NewPosAddr
	static std::unique_ptr<Utility::Detour_t> BossCamCustomPositioningHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x51ADD, &BossCamCustomPositioningDetour, 7);
	g_BossCamCustomPositioning_ReturnAddr = BossCamCustomPositioningHook->GetReturnAddress();
	BossCamCustomPositioningHook->Toggle(enable);
	g_isMPCamActiveAddr = &g_isMPCamActive;

	run = enable;
}

void StaggerGravityInertiaFix(bool enable) {
	using namespace Utility;
	static bool run = false;
	if (run == enable) {
		return;
	}
	// From CPlayerStaggeGravity_sub_1401FBE20:
	// dmc3.exe+1FBEB9 - F3 0F 10 87 A4 00 00 00   - movss xmm0,[rdi+000000A4] // rdi == player, +0xA4 == verticalInertiaMultiplier
	static std::unique_ptr<Utility::Detour_t> StaggerGravityHook =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x1FBEB9, &FixStaggerGravityDetour, 8);
	g_FixStaggerGravity_ReturnAddr = StaggerGravityHook->GetReturnAddress();
	StaggerGravityHook->Toggle(enable);

	run = enable;
}


void ToggleAllFixes(bool enable) {
	BlendingEffectsSpeedFixes(enable);
	BossCamFixes(enable);
	StaggerGravityInertiaFix(enable);
}


}