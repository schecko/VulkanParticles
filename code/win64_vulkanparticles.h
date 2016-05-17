#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include <windows.h>
#include <vector>

#include "commonvulkan.h"
#include "surface.h"
#include "commonwindows.h"
#include "camera.h"

#define Kibibytes(Value) ((Value)*1024LL)
#define Mebibytes(Value) (Kibibytes(Value)*1024LL)
#define Gibibytes(Value) (Mebibytes(Value)*1024LL)
#define Terabytes(Value) (Gibibytes(Value)*1024LL)

static const uint32_t VERTEX_BUFFER_BIND_ID = 0;
static const float CAMERA_SPEED = 0.001f;
#define VALIDATION_LAYERS false
#define VALIDATION_MESSAGES false
#define DEBUGGING true

static const uint32_t POOLPAGESIZE = Kibibytes(1);

enum AssetType
{
	nullAsset,
	shaderAsset,
	textureAsset
};

//vertex data stored on ram
struct Vertex
{
	float pos[3];
	float col[3];
};

struct AssetNode
{
	uint32_t* prev;
	uint32_t* next;
	uint32_t* data;
	AssetType assetType;
	uint32_t dataSize;
	uint32_t numPages;
};


//vertex data stored on the gpu ram
struct VertexBuffer
{
	//vertex placement data
	std::vector<Vertex> vPos;
	VkBuffer vBuffer;
	VkDeviceMemory vMemory;
	VkPipelineVertexInputStateCreateInfo viInfo;
	std::vector<VkVertexInputBindingDescription> vBindingDesc;
	std::vector<VkVertexInputAttributeDescription> vBindingAttr;

	//vertex index data
	std::vector<uint32_t> iPos;
	int iCount;
	VkBuffer iBuffer;
	VkDeviceMemory iMemory;

};

struct PoolInfo
{
	uint32_t* poolStart;
	uint32_t poolSize;

};

//main struct, pretty much holds everything
struct MainMemory
{
	HWND consoleHandle;
	WindowInfo windowInfo;
	TimerInfo timerInfo;

	InputInfo input;

	InstanceInfo instanceInfo;
	SurfaceInfo surfaceInfo;
	PhysDeviceInfo physDeviceInfo;
	DeviceInfo deviceInfo;
	SwapchainInfo swapchainInfo;

	VkCommandBuffer textureCmdBuffer;
	VertexBuffer vertexBuffer;

	Camera camera;
	PipelineInfo pipelineInfo;
	PoolInfo poolInfo;


};