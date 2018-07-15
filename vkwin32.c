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

#include <stdlib.h>

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

#include "vkdemo.h"

int DrawFrame(const VulkanDevice *device, float dt);
int InitResources(const VulkanDevice *device);
void UninitResources(const VulkanDevice *device);

static Vulkan s_vulkan;
static VulkanDevice s_device;

static HMODULE s_vulkanLib;

#ifndef NDEBUG
static VkBool32 VKAPI_CALL vkDebugCallback(
   VkDebugUtilsMessageSeverityFlagBitsEXT           messageSeverity,
   VkDebugUtilsMessageTypeFlagsEXT                  messageType,
   const VkDebugUtilsMessengerCallbackDataEXT*      pCallbackData,
   void*                                            pUserData)
{
   OutputDebugStringA(pCallbackData->pMessage);
   OutputDebugStringA("\r\n");
   return VK_FALSE;
}
#endif

static int initSwapchain(VulkanDevice *device, HWND hwnd)
{
   ASSERT(device && device->vk);
   const Vulkan *vk = device->vk;

   RECT rect;
   GetClientRect(hwnd, &rect);
   device->surfaceWidth = rect.right - rect.left;
   device->surfaceHeight = rect.bottom - rect.top;

   VkPhysicalDevice physicalDevice = vk->physicalDevices[device->physicalDeviceIdx];
   uint32_t frameCount = ARRAY_COUNT(device->frames);

   VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,
      .pNext = NULL,
      .flags = 0,
      .hinstance = GetWindowInstance(hwnd),
      .hwnd = hwnd
   };

   VK_VERIFY(vk->vkCreateWin32SurfaceKHR(vk->instance, &surfaceCreateInfo, NULL, &device->surface));

   VkBool32 supported = VK_FALSE;
   VK_VERIFY(vk->vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, device->queueFamilyIndex,
      device->surface, &supported));
   ASSERT(supported == VK_TRUE);

   VkSurfaceCapabilitiesKHR surfaceCapabilities;
   VK_VERIFY(vk->vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, device->surface, &surfaceCapabilities));
   ASSERT(surfaceCapabilities.minImageCount <= frameCount && frameCount <= surfaceCapabilities.maxImageCount);

   uint32_t surfaceFormatCount = 0;
   VkSurfaceFormatKHR *surfaceFormats;

   VK_VERIFY(vk->vkGetPhysicalDeviceSurfaceFormatsKHR(vk->physicalDevices[device->physicalDeviceIdx],
      device->surface, &surfaceFormatCount, NULL));
   surfaceFormats = malloc(sizeof(*surfaceFormats) * surfaceFormatCount);
   VK_VERIFY(vk->vkGetPhysicalDeviceSurfaceFormatsKHR(vk->physicalDevices[device->physicalDeviceIdx],
      device->surface, &surfaceFormatCount, surfaceFormats));

   static const VkFormat desiredFormats[] = {
      VK_FORMAT_B8G8R8A8_SRGB,
      VK_FORMAT_R8G8B8A8_SRGB,
   };

   VkSurfaceFormatKHR *surfaceFormat = NULL;
   for (uint32_t i = 0; i < ARRAY_COUNT(desiredFormats); ++i) {
      for (uint32_t j = 0; j < surfaceFormatCount; ++j) {
         if (surfaceFormats[j].format == desiredFormats[i]) {
            surfaceFormat = &surfaceFormats[j];
            goto foundFormat;
         }
      }
   }
   ASSERT(surfaceFormat);

foundFormat:
   device->surfaceFormat = surfaceFormat->format;

   VkSwapchainCreateInfoKHR swapchainCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
      .pNext = NULL,
      .flags = 0,
      .surface = device->surface,
      .minImageCount = frameCount,
      .imageFormat = surfaceFormat->format,
      .imageColorSpace = surfaceFormat->colorSpace,
      .imageExtent = {
         .width = device->surfaceWidth,
         .height = device->surfaceHeight,
      },
      .imageArrayLayers = 1,
      .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
      .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
      .queueFamilyIndexCount = 1,
      .pQueueFamilyIndices = &device->queueFamilyIndex,
      .preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR,
      .compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
      .presentMode = VK_PRESENT_MODE_FIFO_KHR,
      .clipped = VK_TRUE,
      .oldSwapchain = VK_NULL_HANDLE,
   };

   free(surfaceFormats);

   VK_VERIFY(device->vkCreateSwapchainKHR(device->device, &swapchainCreateInfo, NULL, &device->swapchain));

   VK_VERIFY(device->vkGetSwapchainImagesKHR(device->device, device->swapchain, &device->swapchainImageCount, NULL));
   device->swapchainImages = malloc(sizeof(*device->swapchainImages) * device->swapchainImageCount);
   VK_VERIFY(device->vkGetSwapchainImagesKHR(device->device, device->swapchain, &device->swapchainImageCount, device->swapchainImages));

   return InitResources(device);
}

static void uninitSwapchain(VulkanDevice *device)
{
   ASSERT(device && device->vk);
   const Vulkan *vk = device->vk;

   VK_VERIFY(device->vkDeviceWaitIdle(device->device));

   UninitResources(device);

   free(device->swapchainImages);
   device->swapchainImages = NULL;
   device->swapchainImageCount = 0;

   device->vkDestroySwapchainKHR(device->device, device->swapchain, NULL);
   device->swapchain = VK_NULL_HANDLE;

   vk->vkDestroySurfaceKHR(vk->instance, device->surface, NULL);
   device->surface = VK_NULL_HANDLE;
}

static int createDevice(const Vulkan *vk, uint32_t physicalDeviceIdx, VulkanDevice *device)
{
   ASSERT(vk && device);
   ASSERT(physicalDeviceIdx < vk->physicalDeviceCount);

   memset(device, 0, sizeof(*device));
   device->vk = vk;

   uint32_t queueFamilyPropertyCount = 0;
   vk->vkGetPhysicalDeviceQueueFamilyProperties(vk->physicalDevices[physicalDeviceIdx], &queueFamilyPropertyCount, NULL);
   device->queueFamilyProperties = malloc(sizeof(*device->queueFamilyProperties) * queueFamilyPropertyCount);
   vk->vkGetPhysicalDeviceQueueFamilyProperties(vk->physicalDevices[physicalDeviceIdx], &queueFamilyPropertyCount, device->queueFamilyProperties);

   uint32_t desiredQueueIndex = queueFamilyPropertyCount;
   for (uint32_t i = 0; i < queueFamilyPropertyCount; ++i) {
      if (device->queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
         desiredQueueIndex = i;
         break;
      }
   }
   if (desiredQueueIndex >= queueFamilyPropertyCount) {
      return 0;
   }
   device->queueFamilyPropertyCount = queueFamilyPropertyCount;
   device->queueFamilyIndex = desiredQueueIndex;

   float queuePriorities[] = { 1.0f };
   VkDeviceQueueCreateInfo queueInfo = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
      .pNext = NULL,
      .flags = 0,
      .queueFamilyIndex = desiredQueueIndex,
      .queueCount = ARRAY_COUNT(queuePriorities),
      .pQueuePriorities = queuePriorities,
   };

   const char *enabledExtensionNames[] = {
      "VK_KHR_swapchain",
   };

   VkDeviceCreateInfo createInfo = {
      .sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,
      .pNext = NULL,
      .flags = 0,
      .queueCreateInfoCount = 1,
      .pQueueCreateInfos = &queueInfo,
      .enabledLayerCount = 0,
      .ppEnabledLayerNames = NULL,
      .enabledExtensionCount = ARRAY_COUNT(enabledExtensionNames),
      .ppEnabledExtensionNames = enabledExtensionNames,
      .pEnabledFeatures = NULL,
   };

   VK_VERIFY(vk->vkCreateDevice(vk->physicalDevices[physicalDeviceIdx], &createInfo, NULL, &device->device));

#define VK_DEVICE_FUNC(ret, name, args) device->name = (ret (VKAPI_PTR *)args) vk->vkGetDeviceProcAddr(device->device, #name); ASSERT(device->name)
#include "vkfuncs.h"
#undef VK_DEVICE_FUNC

   device->vkGetDeviceQueue(device->device, device->queueFamilyIndex, 0, &device->queue);

   VkCommandPoolCreateInfo cpCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
      .pNext = NULL,
      .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
      .queueFamilyIndex = desiredQueueIndex,
   };

   VK_VERIFY(device->vkCreateCommandPool(device->device, &cpCreateInfo, NULL, &device->commandPool));

   // per-frame data
   {
      VkSemaphoreCreateInfo semaphoreCreateInfo = {
         .sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO,
         .pNext = NULL,
         .flags = 0,
      };
      VkFenceCreateInfo fenceCreateInfo = {
         .sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
         .pNext = NULL,
         .flags = VK_FENCE_CREATE_SIGNALED_BIT
      };
      VkCommandBufferAllocateInfo cbAllocInfo = {
         .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
         .pNext = NULL,
         .commandPool = device->commandPool,
         .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
         .commandBufferCount = 1,
      };

      for (uint32_t i = 0; i < ARRAY_COUNT(device->frames); ++i) {
         VK_VERIFY(device->vkCreateSemaphore(device->device, &semaphoreCreateInfo, NULL, &device->frames[i].imageAcquired));
         VK_VERIFY(device->vkCreateSemaphore(device->device, &semaphoreCreateInfo, NULL, &device->frames[i].drawComplete));
         VK_VERIFY(device->vkCreateFence(device->device, &fenceCreateInfo, NULL, &device->frames[i].frameComplete));
         VK_VERIFY(device->vkAllocateCommandBuffers(device->device, &cbAllocInfo, &device->frames[i].commandBuffer));
      }
   }

   device->physicalDeviceIdx = physicalDeviceIdx;
   return 1;
}

static void destroyDevice(VulkanDevice *device)
{
   if (device && device->vk) {
      const Vulkan *vk = device->vk;

      uninitSwapchain(device);

      for (uint32_t i = 0; i < ARRAY_COUNT(device->frames); ++i) {
         device->vkDestroySemaphore(device->device, device->frames[i].imageAcquired, NULL);
         device->frames[i].imageAcquired = VK_NULL_HANDLE;
         device->vkDestroySemaphore(device->device, device->frames[i].drawComplete, NULL);
         device->frames[i].drawComplete = VK_NULL_HANDLE;
         device->vkDestroyFence(device->device, device->frames[i].frameComplete, NULL);
         device->frames[i].frameComplete = VK_NULL_HANDLE;
         device->vkFreeCommandBuffers(device->device, device->commandPool, 1, &device->frames[i].commandBuffer);
         device->frames[i].commandBuffer = VK_NULL_HANDLE;
      }

      device->vkDestroyCommandPool(device->device, device->commandPool, NULL);
      device->commandPool = VK_NULL_HANDLE;

      free(device->queueFamilyProperties);
      device->queueFamilyProperties = NULL;
      device->queueFamilyPropertyCount = 0;

      vk->vkDestroyDevice(device->device, NULL);
      device->device = VK_NULL_HANDLE;
      device->vk = NULL;
   }
}

static int initVulkan(Vulkan *vk)
{
   ASSERT(vk);

   // bootstrap the API
   s_vulkanLib = LoadLibrary(L"vulkan-1");
   if (!s_vulkanLib) {
      return 0;
   }

#define VK_BOOT_FUNC(ret, name, args) vk->name = (ret (VKAPI_PTR *)args) GetProcAddress(s_vulkanLib, #name); ASSERT(vk->name)
#include "vkfuncs.h"
#undef VK_BOOT_FUNC

   uint32_t instanceVersion;
   if (vk->vkEnumerateInstanceVersion(&instanceVersion) != VK_SUCCESS || instanceVersion < VK_API_VERSION_1_1) {
      FreeLibrary(s_vulkanLib);
      return 0;
   }

   // create instance
#ifndef NDEBUG
   const char *enabledLayerNames[] = {
      "VK_LAYER_LUNARG_standard_validation"
   };
#endif
   const char *enabledExtensionNames[] = {
      "VK_KHR_surface",
      "VK_KHR_win32_surface",
#ifndef NDEBUG
      "VK_EXT_debug_report",
      "VK_EXT_debug_utils",
#endif
   };

   VkApplicationInfo appInfo = {
      .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
      .pNext = NULL,
      .pApplicationName = "vkdemo",
      .applicationVersion = 1,
      .pEngineName = "vkdemo engine",
      .engineVersion = 1,
      .apiVersion = VK_API_VERSION_1_0,
   };

   VkInstanceCreateInfo instanceCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
      .pNext = NULL,
      .flags = 0,
      .pApplicationInfo = &appInfo,
#ifndef NDEBUG
      .enabledLayerCount = 1,
      .ppEnabledLayerNames = enabledLayerNames,
#else
      .enabledLayerCount = 0,
      .ppEnabledLayerNames = NULL,
#endif
      .enabledExtensionCount = ARRAY_COUNT(enabledExtensionNames),
      .ppEnabledExtensionNames = enabledExtensionNames,
   };

#ifndef NDEBUG
   VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo = {
      .sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
      .pNext = NULL,
      .flags = 0,
      .messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
      .messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
      .pfnUserCallback = vkDebugCallback,
      .pUserData = NULL,
   };
   instanceCreateInfo.pNext = &debugMessengerCreateInfo;
#endif

   if (vk->vkCreateInstance(&instanceCreateInfo, NULL, &vk->instance) != VK_SUCCESS) {
      FreeLibrary(s_vulkanLib);
      return 0;
   }

   // load the rest of the API
#define VK_INSTANCE_FUNC(ret, name, args) vk->name = (ret (VKAPI_PTR *)args) vk->vkGetInstanceProcAddr(vk->instance, #name); ASSERT(vk->name)
#include "vkfuncs.h"
#undef VK_INSTANCE_FUNC

#ifndef NDEBUG
   VK_VERIFY(vk->vkCreateDebugUtilsMessengerEXT(vk->instance, &debugMessengerCreateInfo, NULL, &vk->debugCallback));
#endif

   uint32_t physicalDeviceCount = 0;
   VK_VERIFY(vk->vkEnumeratePhysicalDevices(vk->instance, &physicalDeviceCount, NULL));
   if (!physicalDeviceCount) {
      vk->vkDestroyInstance(vk->instance, NULL);
      FreeLibrary(s_vulkanLib);
      return 0;
   }

   vk->physicalDevices = malloc(sizeof(*vk->physicalDevices) * physicalDeviceCount);
   vk->physicalDeviceProperties = malloc(sizeof(*vk->physicalDeviceProperties) * physicalDeviceCount);
   VK_VERIFY(vk->vkEnumeratePhysicalDevices(vk->instance, &physicalDeviceCount, vk->physicalDevices));

   for (uint32_t i = 0; i < physicalDeviceCount; ++i) {
      vk->vkGetPhysicalDeviceProperties(vk->physicalDevices[i], &vk->physicalDeviceProperties[i]);
   }
   vk->physicalDeviceCount = physicalDeviceCount;

   return 1;
}

static LRESULT CALLBACK wndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
   static LARGE_INTEGER s_lastTime, s_freq;

   switch (msg) {
   case WM_CREATE:
      QueryPerformanceFrequency(&s_freq);
      QueryPerformanceCounter(&s_lastTime);
      return 0;
   case WM_CLOSE:
      PostQuitMessage(0);
      return 0;
   case WM_PAINT:
   {
      PAINTSTRUCT ps;
      LARGE_INTEGER curTime;

      QueryPerformanceCounter(&curTime);
      double dt = (curTime.QuadPart - s_lastTime.QuadPart) / (double)s_freq.QuadPart;
      s_lastTime = curTime;

      BeginPaint(hwnd, &ps);
      if (!DrawFrame(&s_device, (float)dt)) {
         // handle device lost
         uninitSwapchain(&s_device);
         initSwapchain(&s_device, hwnd);
      }
      EndPaint(hwnd, &ps);
      return 0;
   }
   case WM_SIZE:
      if (wParam != SIZE_MINIMIZED) {
         uninitSwapchain(&s_device);
         initSwapchain(&s_device, hwnd);
      }
      return 0;
   default:
      return DefWindowProc(hwnd, msg, wParam, lParam);
   }
}

static void uninitVulkan(Vulkan *vk)
{
   if (vk) {
      free(vk->physicalDeviceProperties);
      vk->physicalDeviceProperties = NULL;
      free(vk->physicalDevices);
      vk->physicalDevices = NULL;
      vk->physicalDeviceCount = 0;

#ifndef NDEBUG
      vk->vkDestroyDebugUtilsMessengerEXT(vk->instance, vk->debugCallback, NULL);
#endif
      vk->vkDestroyInstance(vk->instance, NULL);
      vk->instance = NULL;
      FreeLibrary(s_vulkanLib);
      s_vulkanLib = NULL;
   }
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nShowCmd)
{
   if (!initVulkan(&s_vulkan)) {
      //FIXME: better error message.
      MessageBox(NULL, L"Could not initialize Vulkan library.", L"Error", MB_ICONERROR | MB_OK);
      return -1;
   }

   uint32_t physicalDeviceIdx = 0;   // just use first device found
   if (!createDevice(&s_vulkan, physicalDeviceIdx, &s_device)) {
      MessageBox(NULL, L"Could not find suitable Vulkan device.", L"Error", MB_ICONERROR | MB_OK);
      uninitVulkan(&s_vulkan);
      return -1;
   }

   WNDCLASSEX wcex = {
      .cbSize = sizeof(wcex),
      .style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
      .lpfnWndProc = wndProc,
      .cbClsExtra = 0,
      .cbWndExtra = 0,
      .hInstance = hInstance,
      .hIcon = NULL,
      .hCursor = LoadCursor(NULL, IDC_ARROW),
      .hbrBackground = NULL,
      .lpszMenuName = NULL,
      .lpszClassName = L"vkdemo.mainWnd",
      .hIconSm = NULL,
   };

   ATOM atom = RegisterClassEx(&wcex);
   if (!atom) {
      destroyDevice(&s_device);
      uninitVulkan(&s_vulkan);
      return -1;
   }

   HWND hwnd = CreateWindowEx(0, MAKEINTATOM(atom), L"Vulkan", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);
   if (!hwnd) {
      return -1;
   }

   ShowWindow(hwnd, nShowCmd);

   MSG msg = { 0 };
   while (msg.message != WM_QUIT) {
      while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
         TranslateMessage(&msg);
         DispatchMessage(&msg);
      }
      RedrawWindow(hwnd, NULL, NULL, RDW_INTERNALPAINT);
   }

   destroyDevice(&s_device);
   uninitVulkan(&s_vulkan);
   return (int)msg.wParam;
}