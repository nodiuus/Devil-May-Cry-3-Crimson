#include "CrimsonEfk.hpp"
#include "../Core/Core.hpp"
#include "Global.hpp"
#include "Utility/Detour.hpp"

#include <cassert>
#include <chrono>
#include <unordered_set>

#include "EffekseerRendererDX11.h"
#include <d3d11.h>
#include <wrl/client.h>

#include "CrimsonDetours.hpp"

// ============================================================================
// GLOBALS
// ============================================================================

static constexpr auto EFK_INSTANCES_MAX = 1000;

static const float EFK_Z_NEAR = 9000.0f;
static const float EFK_Z_FAR  = 1.0f;

static const float EFK_Z_NEAR_RENDER = 1.0f;
static const float EFK_Z_FAR_RENDER = 0.0f;

static int g_frame_counter_current = 0;
static int g_frame_counter_last = 0;

static std::vector<Effekseer::EffectRef> g_effects;

static std::unique_ptr<Utility::Detour_t> s_renderProcHook;
static float g_windowDimensions[2];

static int g_rt_index;

// Frame timing for consistent playback speed
static auto g_lastFrameTime = std::chrono::high_resolution_clock::now();
static float g_deltaTime = 0.0f;

// like 4th set of vector math types in the project yay
static ::Effekseer::ManagerRef g_efkManager = nullptr;
static ::EffekseerRendererDX11::RendererRef g_efkRenderer = nullptr;

static ::Effekseer::Matrix44 g_projectionMatrix;
static ::Effekseer::Matrix44 g_cameraMatrix;
static float* g_followMatrixPtrs[EFK_INSTANCES_MAX] = {};
static float* g_followPosPtrs[EFK_INSTANCES_MAX] = {};
static std::unordered_set<EffekseerRefHandle> g_warmedEffects;

// ============================================================================
// RECLASS
// ============================================================================
namespace Devil3 {

    struct Vector3 {
        float x, y, z;
        float unused;
    };
    struct Vector4 {
        float x, y, z, w;
    };

    struct Matrix4x4 {
        Vector4 col0;
        Vector4 col1;
        Vector4 col2;
        Vector4 col3;
    };

    // Created with ReClass.NET 1.2 by KN4CK3R
#pragma pack(push, r1, 1)
    class cCameraControl {
    public:
        char pad_0008[8];  // 0x0008
        Vector3 up;        // 0x0010
        float FOV;         // 0x0020
        float roll;        // 0x0024
        float unused0;     // 0x0028
        uint32_t unused1;  // 0x002C
        Matrix4x4 transform; // 0x0030
        Vector4 eye;       // 0x0070
        Vector4 lookat;    // 0x0080
        char pad_0090[32]; // 0x0090

        virtual void Function0();
        virtual void Function1();
        virtual void Function2();
        virtual void Function3();
        virtual void Function4();
        virtual void Function5();
        virtual void Function6();
        virtual void Function7();
        virtual void Function8();
        virtual void Function9();
    }; // Size: 0x00B0
    static_assert(offsetof(cCameraControl, up) == 0x0010);
    static_assert(offsetof(cCameraControl, FOV) == 0x0020);
    static_assert(offsetof(cCameraControl, roll) == 0x0024);
    static_assert(offsetof(cCameraControl, transform) == 0x0030);
    static_assert(offsetof(cCameraControl, eye) == 0x0070);
    static_assert(offsetof(cCameraControl, lookat) == 0x0080);

    class sCameraCtrlPtr {
    public:
        class cCameraControl* pCameraControl; // 0x0000
        char pad_0008[56];                    // 0x0008
    };                                        // Size: 0x0040

    class PCRenderTarget
    {
    public:
        char pad_0000[16]; //0x0000
        void* notSure1; //0x0010
        char pad_0018[16]; //0x0018
        void* unkD3DSomething; //0x0028
        void* unkD3DSomething0; //0x0030
        char pad_0038[56]; //0x0038
        class gfxTexture* gfxTexturePtr; //0x0070
        ID3D11RenderTargetView* renderTargetView; //0x0078
        ID3D11DepthStencilView* depthStencilView; //0x0080
        char pad_0088[8]; //0x0088
        uint32_t N0000008E; //0x0090
        float width; //0x0094
        float height; //0x0098
        float idk; //0x009C
        float someFloat; //0x00A0
        char pad_00A4[4]; //0x00A4
    }; //Size: 0x00A8

    class sRender
    {
    public:
        char pad_0000[8]; //0x0000
        ID3D11Device* device; //0x0008
        ID3D11DeviceContext* deviceContext; //0x0010
        IDXGISwapChain* swapChain; //0x0018
        class PCRenderTarget renderTargets[28]; //0x0020
        char pad_1280[1048]; //0x1280
    }; //Size: 0x1698

    class gfxTexture
    {
    public:
        char pad_0000[8]; //0x0000
    }; //Size: 0x0008

#pragma pack(pop, r1)
} // namespace Devil3

// ============================================================================
// INITIALIZATION
// ============================================================================

namespace CrimsonEfk {

    static inline Effekseer::Matrix43 Matrix43FromFloat44(float* mat44) {
        Effekseer::Matrix43 mat;

        mat.Value[0][0] = mat44[0];  mat.Value[0][1] = mat44[1];  mat.Value[0][2] = mat44[2];
        mat.Value[1][0] = mat44[4];  mat.Value[1][1] = mat44[5];  mat.Value[1][2] = mat44[6];
        mat.Value[2][0] = mat44[8];  mat.Value[2][1] = mat44[9];  mat.Value[2][2] = mat44[10];

        mat.Value[3][0] = mat44[12]; mat.Value[3][1] = mat44[13]; mat.Value[3][2] = mat44[14];

        return mat;
    }

    void CaptureDepthStencilForPresent(ID3D11DeviceContext* pContext) {
        (void)pContext;
    }

    static inline void ClearTrackedMatrixPtr(EffekseerHandle handle) {
        if (handle >= 0 && handle < EFK_INSTANCES_MAX) {
            g_followMatrixPtrs[handle] = nullptr;
        }
    }

    static inline void ClearTrackedPosPtr(EffekseerHandle handle) {
        if (handle >= 0 && handle < EFK_INSTANCES_MAX) {
            g_followPosPtrs[handle] = nullptr;
        }
    }

    static inline void ApplyTrackedMatricesAtPresent() {
        if (g_efkManager == nullptr) {
            return;
        }

        for (int handle = 0; handle < EFK_INSTANCES_MAX; ++handle) {
            auto mat44 = g_followMatrixPtrs[handle];
            if (!mat44) {
                continue;
            }

            if (!g_efkManager->Exists(handle)) {
                g_followMatrixPtrs[handle] = nullptr;
                continue;
            }

            Effekseer::Matrix43 mat;
            mat.Value[0][0] = mat44[0];  mat.Value[0][1] = mat44[1];  mat.Value[0][2] = mat44[2];
            mat.Value[1][0] = mat44[4];  mat.Value[1][1] = mat44[5];  mat.Value[1][2] = mat44[6];
            mat.Value[2][0] = mat44[8];  mat.Value[2][1] = mat44[9];  mat.Value[2][2] = mat44[10];
            mat.Value[3][0] = mat44[12]; mat.Value[3][1] = mat44[13]; mat.Value[3][2] = mat44[14];

            g_efkManager->SetMatrix(handle, mat);
            g_efkManager->UpdateHandle(handle, 0.0f);
        }

        for (int handle = 0; handle < EFK_INSTANCES_MAX; ++handle) {
            if (g_followMatrixPtrs[handle]) {
                continue;
            }

            auto vec3 = g_followPosPtrs[handle];
            if (!vec3) {
                continue;
            }

            if (!g_efkManager->Exists(handle)) {
                g_followPosPtrs[handle] = nullptr;
                continue;
            }

            g_efkManager->SetLocation(handle, Effekseer::Vector3D(vec3[0], vec3[1], vec3[2]));
            g_efkManager->UpdateHandle(handle, 0.0f);
        }
    }

    static inline void TryImmediateHandleTransformUpdate(EffekseerHandle handle) {
        if (g_efkManager == nullptr || handle < 0) {
            return;
        }

        g_efkManager->UpdateHandle(handle, 0.0f);
    }

    bool EffekInit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, int windowWidth, int windowHeight)
    {
        if (pDevice == nullptr || pContext == nullptr)
            return false;

        g_efkManager = ::Effekseer::Manager::Create(EFK_INSTANCES_MAX);
        if (g_efkManager == nullptr)
            return false;

        auto graphicsDevice = ::EffekseerRendererDX11::CreateGraphicsDevice(pDevice, pContext);
        if (graphicsDevice == nullptr)
            return false;

        g_efkRenderer = ::EffekseerRendererDX11::Renderer::Create(graphicsDevice, EFK_INSTANCES_MAX, D3D11_COMPARISON_GREATER_EQUAL);
        if (g_efkRenderer == nullptr)
            return false;

        g_efkManager->SetSpriteRenderer(g_efkRenderer->CreateSpriteRenderer());
        g_efkManager->SetRibbonRenderer(g_efkRenderer->CreateRibbonRenderer());
        g_efkManager->SetRingRenderer(g_efkRenderer->CreateRingRenderer());
        g_efkManager->SetTrackRenderer(g_efkRenderer->CreateTrackRenderer());
        g_efkManager->SetModelRenderer(g_efkRenderer->CreateModelRenderer());

        g_efkManager->SetTextureLoader(g_efkRenderer->CreateTextureLoader());
        g_efkManager->SetModelLoader(g_efkRenderer->CreateModelLoader());
        g_efkManager->SetMaterialLoader(g_efkRenderer->CreateMaterialLoader());
        g_efkManager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());

        g_windowDimensions[0] = windowWidth;
        g_windowDimensions[1] = windowHeight;

        auto viewerPosition = ::Effekseer::Vector3D(10.0f, 5.0f, 20.0f);
        float aspectRatio = (float)windowWidth / (float)windowHeight;

        g_projectionMatrix.PerspectiveFovRH(90.0f / 180.0f * 3.14159f, aspectRatio, 1.0f, 1000.0f);

        g_cameraMatrix.LookAtRH(viewerPosition, ::Effekseer::Vector3D(0.0f, 0.0f, 0.0f), ::Effekseer::Vector3D(0.0f, 1.0f, 0.0f));

        return true;
    }

    void EffekTerminate()
    {
        CrimsonEfk::StopAllEffects();

        if (g_efkManager != nullptr) g_efkManager.Reset();
        if (g_efkRenderer != nullptr) g_efkRenderer.Reset();

        g_efkManager = nullptr;
        g_efkRenderer = nullptr;

        g_effects.clear();

        for (int i = 0; i < EFK_INSTANCES_MAX; ++i) {
            g_followMatrixPtrs[i] = nullptr;
            g_followPosPtrs[i] = nullptr;
        }

        g_warmedEffects.clear();
    }

    // ============================================================================
    // EFFECT LOADING
    // ============================================================================

    // Effekseer uses UTF-16 :sob:
    EffekseerRefHandle LoadEffect(const wchar_t* path, float scale)
    {
        if (g_efkManager == nullptr || path == nullptr)
            return -1;

        g_effects.emplace_back(Effekseer::Effect::Create(g_efkManager, (const char16_t*)(path), scale));
        return g_effects.size() - 1;
    }

    EffekseerRefHandle LoadEffectFromMemory(const void* data, int size, float scale)
    {
        if (g_efkManager == nullptr || data == nullptr || size == 0)
            return -1;

        g_effects.emplace_back(Effekseer::Effect::Create(g_efkManager, data, size, scale));
        return g_effects.size() - 1;
    }

    // ============================================================================
    // EFFECT PLAYBACK
    // ============================================================================

    EffekseerHandle PlayEffect(EffekseerRefHandle hEffect, float x, float y, float z, void* player) {

        if (g_efkManager == nullptr || hEffect < 0) {
            return -1;
        }

        // NOTE(): hack to force the game to issue some particle rendering drawcalls,
        // plays an invisible particle effect - otherwise particles wont show up unless
        // there's something in the camera frustrum already - i tried to disable particle
        // culling, but then drawcalls dont get sorted and effekseer renders behind players.
        // cant stand starring at their rendering spaghetti anymore so this will have to do
        if (!player) {
            // TODO(): idk what's the best way to get main player
            auto pool_10222 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
            if (!pool_10222 || !pool_10222[3]) {
                return -1;
            }
            if (pool_10222) {
                auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_10222[3]);
                player = mainActorData;
            }
        }

        static constexpr int fakeParticleBank = 3;
        static constexpr int fakeParticleId = 4;
        static constexpr float fakeParticleTime = 0.1f;

        assert(player);

        CrimsonDetours::CreateEffectDetour(player, fakeParticleBank, fakeParticleId, 1, 1, 0, fakeParticleTime);

        return g_efkManager->Play(g_effects[hEffect], x, y, z);
    }

    EffekseerHandle PlayEffect(EffekseerRefHandle hEffect, float* vec3, void* player) {
        return PlayEffect(hEffect, vec3[0], vec3[1], vec3[2], player);
    }

    EffekseerHandle PlayEffectAtMatrix(EffekseerRefHandle hEffect, float* mat44, void* player) {
        if (g_efkManager == nullptr || hEffect < 0 || mat44 == nullptr) {
            return -1;
        }

        auto mat = Matrix43FromFloat44(mat44);

        // One-time warmup to avoid first-use transform oddities.
        if (g_warmedEffects.find(hEffect) == g_warmedEffects.end()) {
            auto warmHandle = PlayEffect(hEffect, mat44[12], mat44[13], mat44[14], player);
            if (warmHandle >= 0) {
                g_efkManager->SetMatrix(warmHandle, mat);
                g_efkManager->UpdateHandle(warmHandle, 0.0f);
                g_efkManager->StopEffect(warmHandle);
            }
            g_warmedEffects.insert(hEffect);
        }

        auto handle = PlayEffect(hEffect, mat44[12], mat44[13], mat44[14], player);
        if (handle < 0) {
            return handle;
        }

        g_efkManager->SetMatrix(handle, mat);

        if (handle < EFK_INSTANCES_MAX) {
            g_followMatrixPtrs[handle] = mat44;
            g_followPosPtrs[handle] = nullptr;
        }

        // Apply immediately so first visible frame has the intended transform.
        g_efkManager->UpdateHandle(handle, 0.0f);

        return handle;
    }

    EffekseerHandle PlayEffectAtPos(EffekseerRefHandle hEffect, float* vec3, void* player) {
        if (g_efkManager == nullptr || hEffect < 0 || vec3 == nullptr) {
            return -1;
        }

        auto handle = PlayEffect(hEffect, vec3[0], vec3[1], vec3[2], player);
        if (handle < 0) {
            return handle;
        }

        g_efkManager->SetLocation(handle, Effekseer::Vector3D(vec3[0], vec3[1], vec3[2]));

        if (handle < EFK_INSTANCES_MAX) {
            g_followMatrixPtrs[handle] = nullptr;
            g_followPosPtrs[handle] = vec3;
        }

        g_efkManager->UpdateHandle(handle, 0.0f);

        return handle;
    }

    void SetDynamicPos(EffekseerHandle handle, float x, float y, float z) {
        if (g_efkManager != nullptr && handle >= 0) {
            g_efkManager->SetDynamicInput(handle, 0, x);
            g_efkManager->SetDynamicInput(handle, 1, y);
            g_efkManager->SetDynamicInput(handle, 2, z);
            TryImmediateHandleTransformUpdate(handle);
        }
    }

    void SetDynamicPos(EffekseerHandle handle, float* vec3) {
        SetDynamicPos(handle, vec3[0], vec3[1], vec3[2]);
    }

    void StopEffect(EffekseerHandle handle) {
        if (g_efkManager != nullptr && handle >= 0) {
            g_efkManager->StopEffect(handle);
            ClearTrackedMatrixPtr(handle);
            ClearTrackedPosPtr(handle);
        }
    }

    void StopAllEffects() {
        if (g_efkManager != nullptr) {
            g_efkManager->StopAllEffects();
        }

        for (int i = 0; i < EFK_INSTANCES_MAX; ++i) {
            g_followMatrixPtrs[i] = nullptr;
            g_followPosPtrs[i] = nullptr;
        }
    }

    bool IsPlaying(EffekseerHandle handle) {
        if (g_efkManager == nullptr || handle < 0) {
            return false;
        }

        return g_efkManager->Exists(handle);
    }

    void MoveEffect(EffekseerHandle handle, float x, float y, float z) {
        if (g_efkManager != nullptr && handle >= 0) {
            g_efkManager->SetLocation(handle, Effekseer::Vector3D(x, y, z));
            ClearTrackedMatrixPtr(handle);
            ClearTrackedPosPtr(handle);
            TryImmediateHandleTransformUpdate(handle);
        }
    }

    void MoveEffect(EffekseerHandle handle, float* vec3) {
        if (g_efkManager != nullptr && handle >= 0) {
            g_efkManager->SetLocation(handle, Effekseer::Vector3D(vec3[0], vec3[1], vec3[2]));
            ClearTrackedMatrixPtr(handle);
            if (handle < EFK_INSTANCES_MAX) {
                g_followPosPtrs[handle] = vec3;
            }
            TryImmediateHandleTransformUpdate(handle);
        }
    }

    void SetMatrix(EffekseerHandle handle, Effekseer::Matrix43 mat43) {
        if (g_efkManager != nullptr && handle >= 0) {
            g_efkManager->SetMatrix(handle, mat43);
            ClearTrackedMatrixPtr(handle);
            ClearTrackedPosPtr(handle);
            TryImmediateHandleTransformUpdate(handle);
        }
    }

    void SetMatrix(EffekseerHandle handle, float* mat44) {
        if (g_efkManager != nullptr && handle >= 0) {
            Effekseer::Matrix43 mat = Matrix43FromFloat44(mat44);

            g_efkManager->SetMatrix(handle, mat);
            if (handle < EFK_INSTANCES_MAX) {
                g_followMatrixPtrs[handle] = mat44;
                g_followPosPtrs[handle] = nullptr;
            }
            TryImmediateHandleTransformUpdate(handle);
        }
    }

    void SetAllColor(EffekseerHandle handle, uint32_t color) {
        if (g_efkManager != nullptr && handle >= 0) {
            uint8_t r = color & 0xFF;
            uint8_t g = (color >> 8) & 0xFF;
            uint8_t b = (color >> 16) & 0xFF;
            uint8_t a = (color >> 24) & 0xFF;
            g_efkManager->SetAllColor(handle, Effekseer::Color(r, g, b, a));
        }
    }

    // ============================================================================
    // CAMERA SETUP
    // ============================================================================

    void EffekseerSetDevil3Camera(Devil3::cCameraControl* devil3cam) {
        const float fovY = devil3cam->FOV;

        float aspectRatio = (float)g_windowDimensions[0] / (float)g_windowDimensions[1];
        g_projectionMatrix.PerspectiveFovRH(fovY, aspectRatio, EFK_Z_NEAR, EFK_Z_FAR);

        auto viewerPosition = ::Effekseer::Vector3D(devil3cam->eye.x, devil3cam->eye.y, devil3cam->eye.z);
        auto targetPoisiont = ::Effekseer::Vector3D(devil3cam->lookat.x, devil3cam->lookat.y, devil3cam->lookat.z);
        auto cameraUp = ::Effekseer::Vector3D(devil3cam->up.x, devil3cam->up.y, devil3cam->up.z);
        g_cameraMatrix.LookAtRH(viewerPosition, targetPoisiont, cameraUp);
    }

    // ============================================================================
    // RENDERING
    // ============================================================================

    void EffekRender(ID3D11DeviceContext* pContext, float deltaTime)
    {
        if (g_efkManager == nullptr || g_efkRenderer == nullptr) {
            return;
        }

        static Devil3::sCameraCtrlPtr* staticCameraCtrlPtr = (Devil3::sCameraCtrlPtr*)(appBaseAddr + 0xC8FBD0);
        if (staticCameraCtrlPtr->pCameraControl) {
            EffekseerSetDevil3Camera(staticCameraCtrlPtr->pCameraControl);
        }

#if 0
        Devil3::sRender* render = (Devil3::sRender*)0x140C0B410;
        ID3D11RenderTargetView* pRTV = nullptr;
        ID3D11DepthStencilView* pDSV = nullptr;
        pDSV = render->renderTargets[g_rt_index].depthStencilView;
        pRTV = render->renderTargets[g_rt_index].renderTargetView;

        if (pRTV == nullptr || pDSV == nullptr)
        {
            if (pRTV) pRTV->Release();
            if (pDSV) pDSV->Release();
            return;
        }
        pDSV->AddRef();
        pRTV->AddRef();

        pContext->OMSetRenderTargets(1, &pRTV, NULL);

        ID3D11DepthStencilView* pGameDSV = pDSV;
        ID3D11DepthStencilState* pDepthState = nullptr;
        D3D11_DEPTH_STENCIL_DESC dsDesc = {};
        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        dsDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;
        dsDesc.StencilEnable = FALSE;
        render->device->CreateDepthStencilState(&dsDesc, &pDepthState);
        render->deviceContext->OMSetDepthStencilState(pDepthState, 0);
#endif

        Effekseer::Manager::UpdateParameter updateParam;
        updateParam.DeltaFrame = deltaTime * 60.0f;
        updateParam.SyncUpdate = true;
        g_efkManager->Update(updateParam);

        static float g_time = 0.0f;
        g_time += deltaTime;
        g_efkRenderer->SetTime(g_time);

        g_efkRenderer->SetProjectionMatrix(g_projectionMatrix);
        g_efkRenderer->SetCameraMatrix(g_cameraMatrix);

        g_efkRenderer->BeginRendering();

        Effekseer::Manager::DrawParameter drawParam;
        drawParam.ZNear = EFK_Z_NEAR_RENDER;
        drawParam.ZFar = EFK_Z_FAR_RENDER;
        drawParam.ViewProjectionMatrix = g_efkRenderer->GetCameraProjectionMatrix();

        g_efkManager->Draw(drawParam);

        g_efkRenderer->EndRendering();

#if 0
        pContext->OMSetRenderTargets(1, &pRTV, pGameDSV);

        if (pRTV) pRTV->Release();
        if (pDSV) pDSV->Release();
#endif
    }


    void EffekseerTestSpawn(EffekseerRefHandle hEffect, float x, float y, float z) {
        Effekseer::Handle h = CrimsonEfk::PlayEffect(hEffect, x, y, z);
    }


    class cDraw
    {
    public:
        char pad_0000[16]; //0x0000
        bool b0; //0x0010
        char pad_0011[7]; //0x0011
        bool b1; //0x0018
        bool b2; //0x0019
        char pad_001A[14]; //0x001A
        uint32_t ass; //0x0028
        char pad_002C[2068]; //0x002C
    }; //Size: 0x0840

    std::vector<cDraw*> g_rtities;
    char imgoo_buffer[9999];
    static int effect_id = 4;
    void EffekDrawImgui()
    {
        static bool pressed = false;
        //static EffekseerEffect* ass = EffekseerLoadEffect(L"Sample\\01_Pierre01\\Lightning.efkefc", 6.0f);
        static EffekseerRefHandle ass = LoadEffect(L"Sample\\00_Version16\\Aura01.efkefc", 100.0f);
        //static EffekseerEffect* ass = EffekseerLoadEffect(L"Sample\\00_Basic\\Laser01.efkefc");
        if (ImGui::Button("Load test effect")) {

            pressed = true;
        }
        if (pressed && (g_frame_counter_current % 120 == 0)) {
            vec3 position{};
            auto pool_10222 = *reinterpret_cast<byte8***>(appBaseAddr + 0xC90E28);
            if (!pool_10222 || !pool_10222[3]) {
                position.x = 0.0f; position.y = 0.1f, position.z = 0.0f;
            }
            if (pool_10222) {
                auto& mainActorData = *reinterpret_cast<PlayerActorData*>(pool_10222[3]);
                if (mainActorData) {
                    position.x = mainActorData.position.x;
                    position.y = mainActorData.position.y;
                    position.z = mainActorData.position.z;
                    CrimsonDetours::CreateEffectDetour(mainActorData, 3, 4, 1, 1, 0, 0.00001f);
                }
            }

            EffekseerTestSpawn(ass, position.x, position.y, position.z);

        }
        ImGui::SliderInt("RtIndex:", &g_rt_index, 0, 28);
        ImGui::SliderInt("effect_id", &effect_id, 0, 255);
        // Capture current render target and depth buffer from game
        Devil3::sRender* render = (Devil3::sRender*)0x140C0B410;
        ID3D11RenderTargetView* pRTV = nullptr;
        ID3D11DepthStencilView* pDSV = nullptr;
        pDSV = render->renderTargets[g_rt_index].depthStencilView;
        pRTV = render->renderTargets[g_rt_index].renderTargetView;
        ImGui::Text("pDSV: %p", pDSV);
        ImGui::Text("pRTV: %p", pRTV);
        //     ImGui::SliderFloat("znear:", &EFK_Z_NEAR, -1000.0f, 1000.0f, "%.3f");
        //     ImGui::SliderFloat("zfar:", &EFK_Z_FAR, -1000.0f, 1000.0f, "%.3f");
        //     ImGui::SliderFloat("znear_render:", &EFK_Z_NEAR_RENDER, -1000.0f, 1000.0f, "%.3f");
        //     ImGui::SliderFloat("zfar_render:", &EFK_Z_FAR_RENDER, -1000.0f, 1000.0f, "%.3f");

        ImGui::InputTextMultiline("ass", imgoo_buffer, sizeof(imgoo_buffer));
        if (ImGui::Button("parse rttit")) {
            char* line = strtok((char*)imgoo_buffer, "\n");

            while (line != NULL)
            {
                // Skip "0x" prefix if present
                char* hexStr = line;
                if (strncmp(line, "0x", 2) == 0)
                    hexStr = line + 2;

                // Parse hex value
                unsigned long long value = strtoull(hexStr, NULL, 16);

                //printf("Parsed: 0x%lX = %lu\n", value, value);

                g_rtities.push_back((cDraw*)value);

                line = strtok(NULL, "\n");
            }
        }
        for (auto& rtit : g_rtities) {
            ImGui::PushID(&rtit->ass);
            ImGui::Text("%p", rtit);
            ImGui::Checkbox("b0", &rtit->b0); ImGui::SameLine();
            ImGui::Checkbox("b1", &rtit->b1); ImGui::SameLine();
            ImGui::Checkbox("b2", &rtit->b2); ImGui::SameLine();
            ImGui::PopID();
        }
    }


    void EffekIncFrames() {
        g_frame_counter_current += 1;
    }

    void EffekRenderOnPresent(ID3D11DeviceContext* pContext) {
        (void)pContext;
    }



    __int64 __fastcall D3D11_Draw_sub_140043F90(Devil3::sRender* render, unsigned int a2, unsigned int a3, unsigned __int64 a4, __int64 a5) {
        typedef __int64(__fastcall* D3D11DrawSub140043F90)(Devil3::sRender*, unsigned int, unsigned int, unsigned __int64, __int64);
        static D3D11DrawSub140043F90 original = (D3D11DrawSub140043F90)(appBaseAddr + 0x43F90); // dmc3.exe+43F90
        auto result = original(render, a2, a3, a4, a5);

        if (g_frame_counter_current != g_frame_counter_last) {
            ApplyTrackedMatricesAtPresent();

            auto currentTime = std::chrono::high_resolution_clock::now();
            std::chrono::duration<float> elapsed = currentTime - g_lastFrameTime;
            g_deltaTime = elapsed.count();
            g_lastFrameTime = currentTime;

            CrimsonEfk::EffekRender(render->deviceContext, g_deltaTime);
            g_frame_counter_last = g_frame_counter_current;
        }

        return result;
    }

#if 0
    static __int64 __fastcall D3D11_DrawIndexed_sub_1400425B0(
        Devil3::sRender* render,
        unsigned int a2,
        unsigned int a3,
        __int64 a4,
        void* Src,
        unsigned int a6) {

        typedef __int64(__fastcall* D3D11_DrawIndexed_sub_1400425B0)(Devil3::sRender* render,
            unsigned int a2,
            unsigned int a3,
            __int64 a4,
            void* Src,
            unsigned int a6);
        uintptr_t trampoline_raw = s_renderProcHook->GetTrampoline();
        D3D11_DrawIndexed_sub_1400425B0 trampoline = (D3D11_DrawIndexed_sub_1400425B0)trampoline_raw;
        //render->deviceContext->OMSetRenderTargets(1, nullptr, nullptr);
        ID3D11DepthStencilState* dsv;
        UINT dsvStencil;
        render->deviceContext->OMGetDepthStencilState(&dsv, &dsvStencil);
        D3D11_DEPTH_STENCIL_DESC dsvDesc;
        dsv->GetDesc(&dsvDesc);
        printf("Frame: %d", g_frame_counter_current);
        printf("DepthStencilState: %p", dsv);
        char buffer[256];
        int offset = 0;
        print_uint8_array(buffer, offset, "src:", (uint8_t*)Src, 64);
        printf("%s", buffer);
        if ((dsvDesc.DepthWriteMask == D3D11_DEPTH_WRITE_MASK_ZERO) &&
            (dsvDesc.DepthFunc == D3D11_COMPARISON_GREATER_EQUAL)) {
            printf("D3D11_DEPTH_WRITE_MASK_ZERO && D3D11_COMPARISON_GREATER_EQUAL\n");
            if (g_frame_counter_current != g_frame_counter_last) {
                EffekseerRender(render->deviceContext);
                g_frame_counter_last = g_frame_counter_current;
            }
        }
        else {
            printf("\n");
        }

        return trampoline(render, a2, a3, a4, Src, a6);
    }
#endif

    void EffekInitRenderHook() {

        auto hookAddr = (appBaseAddr + 0x2B555); // dmc3.exe+2B555
        auto jumpAddr = (appBaseAddr + 0x2B55A); // dmc3.exe+2B55A
        //byte8* hookAddr = (byte8*)0x1400444F7;
        //byte8* jumpAddr = (byte8*)hookAddr+5;

        constexpr new_size_t size = 5;
#if 0
        static constexpr ptrdiff_t RENDER_OFFSET = 0x425B0;
        s_renderProcHook = std::make_unique<Utility::Detour_t>(
            (uintptr_t)appBaseAddr + RENDER_OFFSET,
            D3D11_DrawIndexed_sub_1400425B0, NULL,
            "srender_draw_stuff_maybe");
        bool res = s_renderProcHook->Toggle();
        assert(res);
#endif
#if 1
        static Function func = CreateFunction(D3D11_Draw_sub_140043F90, jumpAddr, 0, 0);
        WriteJump(hookAddr, func.addr, size - 5);
#endif
#if 0
        static constexpr ptrdiff_t RENDER_OFFSET = 0x425B0;
        s_renderProcHook = std::make_unique<Utility::Detour_t>(
            (uintptr_t)0x14032D8C0,
            sub_14032D8C0, NULL,
            "srender_draw_stuff_maybe");
        bool res = s_renderProcHook->Toggle();
        assert(res);
#endif
    }
}