#pragma once

#include "Effekseer.h"
typedef int EffekseerHandle;
typedef unsigned long long EffekseerRefHandle;

struct ID3D11Device;
struct ID3D11DeviceContext;

bool EffekseerInit(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, int windowWidth, int windowHeight);
void EffekseerTerminate();

void EffekseerInitRenderHook();
void EffekseerDrawImGui();

EffekseerRefHandle EffekseerLoadEffect(const wchar_t* path, float scale = 1.0f);
EffekseerRefHandle EffekseerLoadEffectFromMemory(const void* data, int size, float scale = 1.0f);

EffekseerHandle EffekseerPlayEffect(EffekseerRefHandle pEffect, float* vec3, void* player = nullptr);
EffekseerHandle EffekseerPlayEffect(EffekseerRefHandle pEffect, float x, float y, float z, void* player = nullptr);

void EffeseekerSetDynamicPos(EffekseerHandle handle, float x, float y, float z);
void EffeseekerSetDynamicPos(EffekseerHandle handle, float* vec3);
void EffekseerStopEffect(EffekseerHandle handle);
void EffekseerStopAllEffects();
bool EffekseerIsPlaying(EffekseerHandle handle);
void EffekseerMoveEffect(EffekseerHandle handle, float x, float y, float z);
void EffeseekerMoveEffect(EffekseerHandle handle, float* vec3);
void EffeseekerSetMatrix(EffekseerHandle handle, Effekseer::Matrix43 mat43);
void EffeseekerSetMatrix(EffekseerHandle handle, float* mat43);

void EffekseerRender(ID3D11DeviceContext* pContext, float deltaTime = 1.0f / 60.0f);
void EffekseerIncFrames();

//void EffekseerDrawImGui();

