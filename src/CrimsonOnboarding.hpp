#pragma once

struct IDXGISwapChain;

namespace CrimsonOnboarding {
    /// Called from GUI_Render each frame. Shows the appropriate onboarding screen
    /// based on activeCrimsonConfig.Onboarding.welcomeStage.
    void Run(IDXGISwapChain* pSwapChain);
}
