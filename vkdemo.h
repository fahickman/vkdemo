/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "vulkan.h"

#ifdef NDEBUG
#  define ASSERT(x) __assume(x)
#else
#  include <assert.h>
#  define ASSERT(x) assert(x)
#endif

#define VK_VERIFY(x) do { VkResult res = (x); ASSERT(res == VK_SUCCESS); } while(0)
#define ARRAY_COUNT(a)  (uint32_t)(sizeof(a)/sizeof((a)[0]))

typedef struct Vulkan {
   VkInstance instance;

#define VK_BOOT_FUNC(ret, name, args) ret (VKAPI_PTR *name) args
#define VK_INSTANCE_FUNC VK_BOOT_FUNC
#include "vkfuncs.h"
#undef VK_INSTANCE_FUNC
#undef VK_BOOT_FUNC

#ifndef NDEBUG
   VkDebugUtilsMessengerEXT debugCallback;
#endif

   uint32_t physicalDeviceCount;
   VkPhysicalDevice *physicalDevices;
   VkPhysicalDeviceProperties *physicalDeviceProperties;
} Vulkan;

typedef struct VulkanFrame {
   VkSemaphore       imageAcquired;
   VkSemaphore       drawComplete;
   VkFence           frameComplete;
   VkCommandBuffer   commandBuffer;
} VulkanFrame;

typedef struct VulkanDevice {
   const Vulkan *vk;

#define VK_DEVICE_FUNC(ret, name, args) ret (VKAPI_PTR *name) args
#include "vkfuncs.h"
#undef VK_DEVICE_FUNC

   uint32_t physicalDeviceIdx; // index into vk->physicalDevices/physicalDeviceProperties

   uint32_t queueFamilyPropertyCount;
   uint32_t queueFamilyIndex;
   VkQueueFamilyProperties *queueFamilyProperties;

   VkDevice device;
   VkQueue queue;
   VulkanFrame frames[2];

   VkSurfaceKHR surface;
   VkFormat surfaceFormat;
   uint32_t surfaceWidth;
   uint32_t surfaceHeight;

   uint32_t swapchainImageCount;
   VkImage *swapchainImages;
   VkSwapchainKHR swapchain;

   VkCommandPool commandPool;
} VulkanDevice;
