#include "../ThirdParty/glm/glm.hpp"
#pragma once
#include <cstdint>
#include "Vars.hpp"

namespace CrimsonHighFPSFixes {

void EnemyAIMultiplayerTargettingDetours(bool enable);
uintptr_t EnemyTargetPlayerSelection(const glm::vec3& enemyPosition, bool hasValidEnemyPosition, uintptr_t defaultPlayerAddr);

}