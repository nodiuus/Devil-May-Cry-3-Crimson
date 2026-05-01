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
	ClothPhysicsHook11->Toggle(enable);

	// dmc3.exe+2C9ADA - F3 41 0F 58 F2         - addss xmm6,xmm10 { Cloth relaxation 1 } -- try7 - 35
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook12 =
		std::make_unique<Detour_t>((uintptr_t)appBaseAddr + 0x2C9ADA, &FixClothPhysicsDetour12, 5);
	g_FixClothPhysics_ReturnAddr12 = ClothPhysicsHook12->GetReturnAddress();
	ClothPhysicsHook12->Toggle(enable);

	// dmc3.exe+2C9AE5 - F3 41 0F 58 FB         - addss xmm7,xmm11 { cloth relaxation 2 } -- try7 - 36
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook13 =
		std::make_unique<Utility::Detour_t>((uintptr_t)appBaseAddr + 0x2C9AE5, &FixClothPhysicsDetour13, 5);
	g_FixClothPhysics_ReturnAddr13 = ClothPhysicsHook13->GetReturnAddress();
	ClothPhysicsHook13->Toggle(enable);

	// dmc3.exe+2C9AF9 - F3 45 0F 58 C4         - addss xmm8,xmm12 { cloth relaxation 3 } -- try7 - 37
	static std::unique_ptr<Utility::Detour_t> ClothPhysicsHook14 =
		std::make_unique<Utility::Detour_t>((uintptr_t)appBaseAddr + 0x2C9AF9, &FixClothPhysicsDetour14, 5);
	g_FixClothPhysics_ReturnAddr14 = ClothPhysicsHook14->GetReturnAddress();
	ClothPhysicsHook14->Toggle(enable);

	run = enable;
}

void ClothPhysicsFixesController() {
	if (g_FrameRate >= 58.0f && g_FrameRate <= 62.0f) {
		ClothPhysicsFixes(false);
	}
	else {
		ClothPhysicsFixes(true);
	}
}


void ToggleAllFixes(bool enable) {
	BlendingEffectsSpeedFixes(enable);
}


}