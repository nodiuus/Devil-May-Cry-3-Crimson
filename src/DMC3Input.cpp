// UNSTUPIFY(Disclaimer: by 5%)... POOOF
#include "DMC3Input.hpp"
#include "Config.hpp"
#include "Global.hpp"
#include "Vars.hpp"

#include <array>
#include <vector>
#include <memory>
#include <cstring>
#include "Core/DataTypes.hpp"
#include "Utility/Detour.hpp"
#include "Xinput.h"
#include "Core/GUI.hpp"
#include "ImGuiExtra.hpp"
#include "CrimsonGUI.hpp"
#include "CrimsonConfig.hpp"

// NOTE(): writing bind table at that particular place seems to work fine for me... idk, needs testing?
static std::unique_ptr<Utility::Detour_t> s_ButtonToActionHook;
// NOTE(): to show custom imgoo menu when opening settings, idk if there is a better way tbqh
static std::unique_ptr<Utility::Detour_t> s_CUIDControlConsHook; // Constructor
static std::unique_ptr<Utility::Detour_t> s_CUIDControlDestHook; // Destructor

// Per-player defaults (historical behavior). These are copied into per-character slots at runtime.
static BindTable s_PlayerDefaultBinds[PLAYER_COUNT] = {
    {
        .up = 0x1000,
        .down = 0x4000,
        .right = 0x2000,
        .left = 0x8000,
        .melee_atk = 0x10,
        .jump = 0x40,
        .style = 0x20,
        .shoot = 0x80,
        .dt = 0x4,
        .change_gun = 0x1,
        .change_target = 0x200,
        .lock_on = 0x8,
        .change_sword = 0x2,
        .default_camera = 0x400,
        .taunt = 0x100,
        .start = 0x800
    },
#ifdef MY_BINDS_TEST
    {
        .up = 0x1000,
        .down = 0x4000,
        .right = 0x2000,
        .left = 0x8000,
        .melee_atk = 0x40,
        .jump = 0x20,
        .style = 0x10,
        .shoot = 0x80,
        .dt = 0x4,
        .change_gun = 0x1,
        .change_target = 0x200,
        .lock_on = 0x8,
        .change_sword = 0x2,
        .default_camera = 0x400,
        .taunt = 0x100,
        .start = 0x800
    },
#else
    {
        .up = 0x1000,
        .down = 0x4000,
        .right = 0x2000,
        .left = 0x8000,
        .melee_atk = 0x10,
        .jump = 0x40,
        .style = 0x20,
        .shoot = 0x80,
        .dt = 0x4,
        .change_gun = 0x1,
        .change_target = 0x200,
        .lock_on = 0x8,
        .change_sword = 0x2,
        .default_camera = 0x400,
        .taunt = 0x100,
        .start = 0x800
    },
#endif
    {
        .up = 0x1000,
        .down = 0x4000,
        .right = 0x2000,
        .left = 0x8000,
        .melee_atk = 0x10,
        .jump = 0x40,
        .style = 0x20,
        .shoot = 0x80,
        .dt = 0x4,
        .change_gun = 0x1,
        .change_target = 0x200,
        .lock_on = 0x8,
        .change_sword = 0x2,
        .default_camera = 0x400,
        .taunt = 0x100,
        .start = 0x800
    },
    {
        .up = 0x1000,
        .down = 0x4000,
        .right = 0x2000,
        .left = 0x8000,
        .melee_atk = 0x10,
        .jump = 0x40,
        .style = 0x20,
        .shoot = 0x80,
        .dt = 0x4,
        .change_gun = 0x1,
        .change_target = 0x200,
        .lock_on = 0x8,
        .change_sword = 0x2, 
        .default_camera = 0x400,
        .taunt = 0x100,
        .start = 0x800
    },
};

// Actual runtime bind storage: per player, per character.
static BindTable s_CoopPlayerBinds[PLAYER_COUNT][2] = {};

uint16_t(*activeConfigInputs[PLAYER_COUNT][2])[NUM_BINDS] = {
	{ &activeCrimsonConfig.System.ButtonConfig.dante1P,
	  &activeCrimsonConfig.System.ButtonConfig.vergil1P },
	{ &activeCrimsonConfig.System.ButtonConfig.dante2P,
	  &activeCrimsonConfig.System.ButtonConfig.vergil2P },
	{ &activeCrimsonConfig.System.ButtonConfig.dante3P,
	  &activeCrimsonConfig.System.ButtonConfig.vergil3P },
	{ &activeCrimsonConfig.System.ButtonConfig.dante4P,
	  &activeCrimsonConfig.System.ButtonConfig.vergil4P }
};

uint16_t(*queuedConfigInputs[PLAYER_COUNT][2])[NUM_BINDS] = {
	{ &queuedCrimsonConfig.System.ButtonConfig.dante1P,
	  &queuedCrimsonConfig.System.ButtonConfig.vergil1P },
	{ &queuedCrimsonConfig.System.ButtonConfig.dante2P,
	  &queuedCrimsonConfig.System.ButtonConfig.vergil2P },
	{ &queuedCrimsonConfig.System.ButtonConfig.dante3P,
	  &queuedCrimsonConfig.System.ButtonConfig.vergil3P },
	{ &queuedCrimsonConfig.System.ButtonConfig.dante4P,
	  &queuedCrimsonConfig.System.ButtonConfig.vergil4P }
};

struct BTImGuiCtx {
    const char* actions[NUM_BINDS] = {
        "UP",
        "DOWN",
        "RIGHT",
        "LEFT",
        "MELEE_ATK",
        "JUMP",
        "STYLE",
        "SHOOT",
        "DT",
        "CHANGE_GUN",
        "CHANGE_TARGET",
        "LOCK_ON",
        "CHANGE_SWORD",
        "DEFAULT_CAMERA",
        "TAUNT",
        "START",
    };
    const char* items[NUM_BINDS] = {
        "DPAD_UP",
        "DPAD_DOWN",
        "DPAD_RIGHT",
        "DPAD_LEFT",
        "Y",
        "A",
        "B",
        "X",
        "LEFT_SHOULDER",
        "LEFT TRIGGER",
        "LEFT_THUMB",
        "RIGHT_SHOULDER",
        "RIGHT TRIGGER",
        "RIGHT_THUMB",
        "BACK",
        "START"
    };
    uint16_t values[NUM_BINDS] = {
        0x1000,
        0x4000,
        0x2000,
        0x8000,
        0x10,
        0x40,
        0x20,
        0x80,
        0x4,
        0x1,
        0x200,
        0x8,
        0x2,
        0x400,
        0x100,
        0x800
    };
};

static BindTable s_defaultBinds = {
    .up = 0x1000,
    .down = 0x4000,
    .right = 0x2000,
    .left = 0x8000,
    .melee_atk = 0x10,
    .jump = 0x40,
    .style = 0x20,
    .shoot = 0x80,
    .dt = 0x4,
    .change_gun = 0x1,
    .change_target = 0x200,
    .lock_on = 0x8,
    .change_sword = 0x2,
    .default_camera = 0x400,
    .taunt = 0x100,
    .start = 0x800
};

static uint8_t GetCharacterBindSlot(const PlayerActorData* actor) {
    if (!actor) {
        return 0;
    }

    const auto idx = static_cast<uint8_t>(actor->newCharacterIndex);

    // Only Dante/Vergil are relevant for bind switching.
    if (idx == static_cast<uint8_t>(CHARACTER::VERGIL)) {
        return 1;
    }
    if (idx == static_cast<uint8_t>(CHARACTER::DANTE)) {
        return 0;
    }

    // Fallback: if the game uses raw 0/1 slots, honor them.
    if (idx == 1) {
        return 1;
    }

    // If CHARACTER_COUNT is a constexpr/enum (not a macro), #if won't work. Use a runtime check.
    if (CHARACTER_COUNT > 1 && idx < CHARACTER_COUNT) {
        return idx;
    }

    return 0;
}

static void __fastcall sub_1401EB170(PlayerActorData* a1) {

    BindTable* mainBinds = (BindTable*)(appBaseAddr + 0xD6CE80 + 0xA);

    const auto playerIndex = (static_cast<uint32_t>(a1->newPlayerIndex) < PLAYER_COUNT) ? a1->newPlayerIndex : 0;
    const auto characterSlot = GetCharacterBindSlot(a1);
    memcpy(mainBinds, &s_CoopPlayerBinds[playerIndex][characterSlot], sizeof(BindTable));

    s_ButtonToActionHook->GetTrampoline<decltype(&sub_1401EB170)>()(a1);
}

class CUIDControl
{
public:
	char pad_0000[8]; //0x0000
	uint32_t someFlag; //0x0008
	uint32_t N000020E2; //0x000C
	char pad_0010[8]; //0x0010
	float N00000052; //0x0018
	char pad_001C[11812]; //0x001C
	uint8_t N0000062C; //0x2E40
	uint8_t cursor; //0x2E41
	uint8_t N000020DC; //0x2E42
	uint8_t N000020DF; //0x2E43
}; //Size: 0x2E44

static CUIDControl* g_control_ui = nullptr;

static CUIDControl* __fastcall CUIDControl__CUIDControl_sub_1402817C0(__int64 a1) {
    auto result = s_CUIDControlConsHook->GetTrampoline<decltype(&CUIDControl__CUIDControl_sub_1402817C0)>()(a1);
    g_control_ui = result;
    return result;
}

static uintptr_t __fastcall CUIDControl_Destructor_sub_140281840(uintptr_t block, char a2) {
    g_control_ui = nullptr;
    return s_CUIDControlDestHook->GetTrampoline<decltype(&CUIDControl_Destructor_sub_140281840)>()(block, a2);
}

static void CUIDControl_Close() {
    assert(g_control_ui && "CUIDControl instance is null!");
    g_control_ui->cursor = 15; // CUID exit button index
    g_control_ui->someFlag = 2; // idk some cWork thing?
}

void ShowCoopControllerRemapWindow() {
    if (!g_control_ui) {
        return;
    }
    ToggleCursor();
    BTImGuiCtx ctxControl{};
    const auto nplayers = activeConfig.Actor.playerCount;
    bool shouldClose = false;

    static std::array<int, PLAYER_COUNT> s_selectedCharacterSlotByPlayer = {};

	float width = g_renderSize.x / 1.40;
	float height = g_renderSize.y / 1.10;
	const float columnWidth = 0.5f * queuedConfig.globalScale;
	const float rowWidth = 40.0f * queuedConfig.globalScale;

	ImGui::SetNextWindowSize(ImVec2(width, height));
	// Center on screen
	ImGui::SetNextWindowPos(ImVec2(g_renderSize.x * 0.5f, g_renderSize.y * 0.5f), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

    ImGui::Begin("Button Configuration", &shouldClose); {
        ImGui::SetWindowFontScale(CrimsonGUI::scaleFactorY);

        if (ImGui::Button("close")) {
             CUIDControl_Close();
        }

        
        char buffer[33] = {};
        ImGui::BeginTable(buffer, 2); {
			ImGui::TableSetupColumn("b1", 0, columnWidth * 2.0f);
			ImGui::TableNextRow(0, rowWidth);
			ImGui::TableNextColumn();
			
            
            for (int i = 0; i < nplayers && i < PLAYER_COUNT; i++) {

                sprintf(buffer, "%dP", i + 1);
                ImGui::Text(buffer);

                int& selectedSlot = s_selectedCharacterSlotByPlayer[i];
                if (selectedSlot < 0 || selectedSlot >= CHARACTER_COUNT) {
                    selectedSlot = 0;
                }

                if (CHARACTER_COUNT > 1) {
                    ImGui::PushID(i);
                    if (ImGui::BeginTabBar("##CharacterTabs")) {
                        if (ImGui::BeginTabItem("Dante")) {
                            selectedSlot = 0;
                            ImGui::EndTabItem();
                        }
                        if (ImGui::BeginTabItem("Vergil")) {
                            selectedSlot = 1;
                            ImGui::EndTabItem();
                        }

                        ImGui::EndTabBar();
                    }
                    ImGui::PopID();
                }

                uint16_t* bind = &s_CoopPlayerBinds[i][selectedSlot].up;
				uint16_t* config = queuedConfigInputs[i][selectedSlot][0];
                for (int j = 0; j < NUM_BINDS; j++) {
                    ImGui::PushID(i);
                    ImGui::PushID(j);
                    UI::ComboMapValue2<uint16_t, NUM_BINDS>(ctxControl.actions[j], ctxControl.items, ctxControl.values, bind[j],config[j]);

                    ImGui::PopID();
                    ImGui::PopID();
                }

                ImGui::TableNextColumn();
                if (i == 1 || i == 2) {
                    ImGui::Text("");
                }
            }
        }
        ImGui::EndTable();
    }
    ImGui::End();
    if (shouldClose) {
        CUIDControl_Close();
    }
}

void ToggleCursor() {
    if (g_showMain || g_showShop || g_control_ui != nullptr) {
        Windows_ToggleCursor(true);
    } else {
        Windows_ToggleCursor(!activeConfig.hideMouseCursor);
    }
}

void InitBindings() {

    //if ((activeConfig.Actor.playerCount < 2) || !activeConfig.Actor.enable) { return; }

    s_ButtonToActionHook = std::make_unique<Utility::Detour_t>((uintptr_t)appBaseAddr + 0x1EB170, &sub_1401EB170);
    bool res = s_ButtonToActionHook->Toggle();
    assert(res);

    s_CUIDControlConsHook = std::make_unique<Utility::Detour_t>((uintptr_t)appBaseAddr + 0x2817C0, &CUIDControl__CUIDControl_sub_1402817C0);
    res = s_CUIDControlConsHook->Toggle();
    assert(res);

    s_CUIDControlDestHook = std::make_unique<Utility::Detour_t>((uintptr_t)appBaseAddr + 0x281840, &CUIDControl_Destructor_sub_140281840);
    res = s_CUIDControlDestHook->Toggle();
    assert(res);

    // Seed per-character bind tables from per-player defaults.
    BindTable empty{};
    for (uint32_t p = 0; p < PLAYER_COUNT; ++p) {
        for (uint32_t c = 0; c < 2; ++c) {
			BindTable configConversion = {};
			memcpy(&configConversion, *activeConfigInputs[p][c], sizeof(BindTable));
			BindTable base = configConversion;
			if (memcmp(&base, &empty, sizeof(BindTable)) == 0) {
				base = s_defaultBinds;
			}
            s_CoopPlayerBinds[p][c] = base;
        }
    }


#if 0
    BindTable* mainBinds = (BindTable*)(appBaseAddr + 0xD6CE80 + 0xA);
    if (activeConfig.Actor.playerCount > 1) {
        CopyMemory(mainBinds, &s_defaultBinds, sizeof(BindTable));
    }
#endif
}

void SwapXInputButtonsCoop(uint8 plindex, XINPUT_STATE* state) {
    // TODO(): idk dualshock support?
    if (g_control_ui) {
        SetMemory(&state->Gamepad, 0, sizeof(XINPUT_GAMEPAD));
    }
    
    return;
}

