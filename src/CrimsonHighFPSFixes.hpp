#include "../ThirdParty/glm/glm.hpp"
#pragma once
#include <cstdint>
#include "Vars.hpp"

namespace CrimsonHighFPSFixes {

	void ClothPhysicsFixesController();
	void ClothPhysicsEnhancementFixes(bool enable);
	void LookAtBossCamFixes(bool enable);
	void ToggleAllFixes(bool enable);
}