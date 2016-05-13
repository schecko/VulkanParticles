#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <windows.h>
#include <vulkan/vulkan.h>
#include "commonvulkan.h"
#include "commonwindows.h"

struct WindowInfo;
//todo remove cyclic dependance
struct DeviceInfo;

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

VkSurfaceKHR NewSurface(const WindowInfo* window, VkInstance vkInstance);

uint32_t FindGraphicsQueueFamilyIndex(VkPhysicalDevice vkPhysicalDevice, VkSurfaceKHR surface);

void GetSurfaceColorSpaceAndFormat(VkPhysicalDevice physicalDevice,
	SurfaceInfo* surfaceInfo);

SwapchainInfo NewSwapchainInfo(
	const DeviceInfo* deviceInfo,
	VkPhysicalDevice physDevice,
	SurfaceInfo* surfaceInfo,
	uint32_t* width,
	uint32_t* height);

VkResult AcquireNextImage(const DeviceInfo* deviceInfo, SurfaceInfo* surfaceInfo);

VkResult QueuePresent(const DeviceInfo* deviceInfo, const SurfaceInfo* surfaceInfo);

void DestroySurfaceInfo(VkInstance vkInstance, VkDevice device, SurfaceInfo* surfaceInfo);