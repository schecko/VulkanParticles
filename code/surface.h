#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan/vulkan.h>
#include "commonvulkan.h"
#include "commonwindows.h"

struct DeviceInfo;
struct PhysDeviceInfo;
struct WindowInfo;

struct SurfaceInfo
{
	VkSurfaceKHR surface;
	uint32_t renderingQueueFamilyIndex;
	VkColorSpaceKHR colorSpace;
	VkFormat colorFormat;
};

struct SwapchainInfo
{
	uint32_t imageCount;
	std::vector<VkImage> images;
	std::vector<VkImageView> views;
	VkSwapchainKHR swapChain;
	uint32_t currentBuffer;
};

VkSurfaceKHR NewSurface(VkInstance vkInstance, const WindowInfo* window);

uint32_t FindGraphicsQueueFamilyIndex(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR surface);

void GetSurfaceColorSpaceAndFormat(VkPhysicalDevice physicalDevice,
	SurfaceInfo* surfaceInfo);

void NewSwapchainInfo(WindowInfo* windowInfo,
	const PhysDeviceInfo* physDeviceInfo,
	const SurfaceInfo* surfaceInfo,
	const DeviceInfo* deviceInfo,
	SwapchainInfo* swapchainInfo);

VkResult AcquireNextImage(const DeviceInfo* deviceInfo, SwapchainInfo* swapchainInfo);

VkResult QueuePresent(const DeviceInfo* deviceInfo, const SwapchainInfo* swapchainInfo);

void DestroySurfaceInfo(VkInstance vkInstance, SurfaceInfo* surfaceInfo);

void NewSurfaceInfo(VkInstance vkInstance, 
	const WindowInfo* windowInfo, 
	const PhysDeviceInfo* physDeviceInfo, 
	SurfaceInfo* surfaceInfo);

void DestroySwapchainInfo(const DeviceInfo* deviceInfo, SwapchainInfo* swapchainInfo);
