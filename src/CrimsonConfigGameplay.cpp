#pragma once
#include "CrimsonConfigGameplay.hpp"

CrimsonConfigGameplay defaultCrimsonGameplay;
CrimsonConfigGameplay queuedCrimsonGameplay;
CrimsonConfigGameplay activeCrimsonGameplay;

bool* g_FasterTurnRateSettingAddr = nullptr;
float* g_ActiveFOVMultSettingAddr = nullptr;
bool* g_NoAirLunarPhaseLiftSettingAddr = nullptr;
