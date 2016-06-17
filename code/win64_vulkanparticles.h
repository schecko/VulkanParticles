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
#define ArrayCount(Array) (sizeof(Array) / sizeof((Array)[0]))

static const uint32_t VERTEX_BUFFER_BIND_ID = 0;
static const float CAMERA_SPEED = 0.001f;
#define VALIDATION_LAYERS false
#define VALIDATION_MESSAGES false
#define INTERNAL_DEBUG true //developer only debugging tools etc
static const uint32_t EXTERNAL_DEBUG = true; //debugging for release for logs etc

static const uint32_t SMALLPOOLPAGESIZE = 256;
static const uint32_t MEDIUMPOOLPAGESIZE = Kibibytes(1);
static const uint32_t BIGPOOLPAGESIZE = Mebibytes(1);

#define PreventWriteInstructionsFromCrossingThisLine _WriteBarrier(); _mm_sfence(); 
#define PreventReadInstructionsFromCrossingThisLine _ReadBarrier();

enum AssetType
{
	nullAsset,
	shaderAsset,
	textureAsset,
	vertexAsset
};

//vertex data stored on ram
struct Vertex
{
	float pos[3];
	float col[3];
};


//for assets from files such as textures or models
struct AssetNode
{
	//if not allowing files of multiple page sizes
	//next and prev might not be needed
	uint32_t* next;
	uint32_t* prev;
	
	AssetType assetType;
	char assetName[20];
	uint32_t dataSize;
	uint32_t numPages;
};

//for vectors, arrays, maps, etc
struct DataNode
{
	//next MUST BE FIRST VALUE
	uint32_t* next;
	//****
	uint32_t* prev;
	uint32_t dataSize; //todo datasize of numpages one or the other?
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

struct SubPoolInfo
{
	uint32_t* start;
	uint32_t totalSize;
	uint32_t* nextEmpty;
	uint32_t pagesUsed;
	uint32_t actualSpaceUsed;
	uint32_t pageSize;
};

struct PoolInfo
{
	uint32_t totalPoolSize;
	SubPoolInfo subPool[3];
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