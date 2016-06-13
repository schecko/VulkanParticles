#pragma once
#include "win64_vulkanparticles.h"

void Init(MainMemory* m, void* poolStart, uint32_t poolSize);

void Update(MainMemory* m);

void Render(const DeviceInfo* deviceInfo, SwapchainInfo* swapchainInfo);

void Quit(MainMemory* m);


