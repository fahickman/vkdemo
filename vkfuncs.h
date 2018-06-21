
// boot functions are acquired with GetProcAddress/dlsym, rather than vkGetInstanceProcAddr
#ifdef VK_BOOT_FUNC
VK_BOOT_FUNC(VkResult, vkEnumerateInstanceVersion, (
   uint32_t*                                   pApiVersion));
VK_BOOT_FUNC(VkResult, vkCreateInstance, (
   const VkInstanceCreateInfo*                 pCreateInfo,
   const VkAllocationCallbacks*                pAllocator,
   VkInstance*                                 pInstance));
VK_BOOT_FUNC(void, vkDestroyInstance, (
   VkInstance                                  instance,
   const VkAllocationCallbacks*                pAllocator));
VK_BOOT_FUNC(PFN_vkVoidFunction, vkGetInstanceProcAddr, (
   VkInstance                                  instance,
   const char*                                 pName));
#endif //VK_BOOT_FUNC

#ifdef VK_FUNC
VK_FUNC(VkResult, vkEnumeratePhysicalDevices, (
   VkInstance                                  instance,
   uint32_t*                                   pPhysicalDeviceCount,
   VkPhysicalDevice*                           pPhysicalDevices));
VK_FUNC(void, vkGetPhysicalDeviceProperties, (
   VkPhysicalDevice                            physicalDevice,
   VkPhysicalDeviceProperties*                 pProperties));
VK_FUNC(void, vkGetPhysicalDeviceQueueFamilyProperties, (
   VkPhysicalDevice                            physicalDevice,
   uint32_t*                                   pQueueFamilyPropertyCount,
   VkQueueFamilyProperties*                    pQueueFamilyProperties));
VK_FUNC(VkResult, vkCreateDevice, (
   VkPhysicalDevice                            physicalDevice,
   const VkDeviceCreateInfo*                   pCreateInfo,
   const VkAllocationCallbacks*                pAllocator,
   VkDevice*                                   pDevice));
VK_FUNC(void, vkDestroyDevice, (
   VkDevice                                    device,
   const VkAllocationCallbacks*                pAllocator));
VK_FUNC(VkResult, vkDeviceWaitIdle, (
   VkDevice                                    device));
VK_FUNC(void, vkGetDeviceQueue, (
   VkDevice                                    device,
   uint32_t                                    queueFamilyIndex,
   uint32_t                                    queueIndex,
   VkQueue*                                    pQueue));
VK_FUNC(VkResult, vkCreateCommandPool, (
   VkDevice                                    device,
   const VkCommandPoolCreateInfo*              pCreateInfo,
   const VkAllocationCallbacks*                pAllocator,
   VkCommandPool*                              pCommandPool));
VK_FUNC(void, vkDestroyCommandPool, (
   VkDevice                                    device,
   VkCommandPool                               commandPool,
   const VkAllocationCallbacks*                pAllocator));

VK_FUNC(VkResult, vkAllocateCommandBuffers, (
   VkDevice                                    device,
   const VkCommandBufferAllocateInfo*          pAllocateInfo,
   VkCommandBuffer*                            pCommandBuffers));
VK_FUNC(void, vkFreeCommandBuffers, (
   VkDevice                                    device,
   VkCommandPool                               commandPool,
   uint32_t                                    commandBufferCount,
   const VkCommandBuffer*                      pCommandBuffers));
VK_FUNC(VkResult, vkBeginCommandBuffer, (
   VkCommandBuffer                             commandBuffer,
   const VkCommandBufferBeginInfo*             pBeginInfo));
VK_FUNC(VkResult, vkEndCommandBuffer, (
   VkCommandBuffer                             commandBuffer));
VK_FUNC(VkResult, vkQueueSubmit, (
   VkQueue                                     queue,
   uint32_t                                    submitCount,
   const VkSubmitInfo*                         pSubmits,
   VkFence                                     fence));

VK_FUNC(VkResult, vkCreateFence, (
   VkDevice                                    device,
   const VkFenceCreateInfo*                    pCreateInfo,
   const VkAllocationCallbacks*                pAllocator,
   VkFence*                                    pFence));
VK_FUNC(void, vkDestroyFence, (
   VkDevice                                    device,
   VkFence                                     fence,
   const VkAllocationCallbacks*                pAllocator));
VK_FUNC(VkResult, vkWaitForFences, (
   VkDevice                                    device,
   uint32_t                                    fenceCount,
   const VkFence*                              pFences,
   VkBool32                                    waitAll,
   uint64_t                                    timeout));
VK_FUNC(VkResult, vkResetFences, (
   VkDevice                                    device,
   uint32_t                                    fenceCount,
   const VkFence*                              pFences));
VK_FUNC(VkResult, vkCreateSemaphore, (
   VkDevice                                    device,
   const VkSemaphoreCreateInfo*                pCreateInfo,
   const VkAllocationCallbacks*                pAllocator,
   VkSemaphore*                                pSemaphore));
VK_FUNC(void, vkDestroySemaphore, (
   VkDevice                                    device,
   VkSemaphore                                 semaphore,
   const VkAllocationCallbacks*                pAllocator));

VK_FUNC(VkResult, vkCreateGraphicsPipelines, (
   VkDevice                                    device,
   VkPipelineCache                             pipelineCache,
   uint32_t                                    createInfoCount,
   const VkGraphicsPipelineCreateInfo*         pCreateInfos,
   const VkAllocationCallbacks*                pAllocator,
   VkPipeline*                                 pPipelines));
VK_FUNC(void, vkDestroyPipeline, (
   VkDevice                                    device,
   VkPipeline                                  pipeline,
   const VkAllocationCallbacks*                pAllocator));

VK_FUNC(VkResult, vkCreateImageView, (
   VkDevice                                    device,
   const VkImageViewCreateInfo*                pCreateInfo,
   const VkAllocationCallbacks*                pAllocator,
   VkImageView*                                pView));
VK_FUNC(void, vkDestroyImageView, (
   VkDevice                                    device,
   VkImageView                                 imageView,
   const VkAllocationCallbacks*                pAllocator));

VK_FUNC(VkResult, vkCreatePipelineLayout, (
   VkDevice                                    device,
   const VkPipelineLayoutCreateInfo*           pCreateInfo,
   const VkAllocationCallbacks*                pAllocator,
   VkPipelineLayout*                           pPipelineLayout));
VK_FUNC(void, vkDestroyPipelineLayout, (
   VkDevice                                    device,
   VkPipelineLayout                            pipelineLayout,
   const VkAllocationCallbacks*                pAllocator));
VK_FUNC(VkResult, vkCreateShaderModule, (
   VkDevice                                    device,
   const VkShaderModuleCreateInfo*             pCreateInfo,
   const VkAllocationCallbacks*                pAllocator,
   VkShaderModule*                             pShaderModule));
VK_FUNC(void, vkDestroyShaderModule, (
   VkDevice                                    device,
   VkShaderModule                              shaderModule,
   const VkAllocationCallbacks*                pAllocator));
VK_FUNC(VkResult, vkCreateRenderPass, (
   VkDevice                                    device,
   const VkRenderPassCreateInfo*               pCreateInfo,
   const VkAllocationCallbacks*                pAllocator,
   VkRenderPass*                               pRenderPass));
VK_FUNC(void, vkDestroyRenderPass, (
   VkDevice                                    device,
   VkRenderPass                                renderPass,
   const VkAllocationCallbacks*                pAllocator));
VK_FUNC(VkResult, vkCreateFramebuffer, (
   VkDevice                                    device,
   const VkFramebufferCreateInfo*              pCreateInfo,
   const VkAllocationCallbacks*                pAllocator,
   VkFramebuffer*                              pFramebuffer));
VK_FUNC(void, vkDestroyFramebuffer, (
   VkDevice                                    device,
   VkFramebuffer                               framebuffer,
   const VkAllocationCallbacks*                pAllocator));
VK_FUNC(void, vkCmdBeginRenderPass, (
   VkCommandBuffer                             commandBuffer,
   const VkRenderPassBeginInfo*                pRenderPassBegin,
   VkSubpassContents                           contents));
VK_FUNC(void, vkCmdEndRenderPass, (
   VkCommandBuffer                             commandBuffer));
VK_FUNC(void, vkCmdBindPipeline, (
   VkCommandBuffer                             commandBuffer,
   VkPipelineBindPoint                         pipelineBindPoint,
   VkPipeline                                  pipeline));
VK_FUNC(void, vkCmdPushConstants, (
   VkCommandBuffer                             commandBuffer,
   VkPipelineLayout                            layout,
   VkShaderStageFlags                          stageFlags,
   uint32_t                                    offset,
   uint32_t                                    size,
   const void*                                 pValues));
VK_FUNC(void, vkCmdDraw, (
   VkCommandBuffer                             commandBuffer,
   uint32_t                                    vertexCount,
   uint32_t                                    instanceCount,
   uint32_t                                    firstVertex,
   uint32_t                                    firstInstance));

VK_FUNC(VkResult, vkQueuePresentKHR, (
   VkQueue                                     queue,
   const VkPresentInfoKHR*                     pPresentInfo));
VK_FUNC(void, vkDestroySurfaceKHR, (
   VkInstance                                  instance,
   VkSurfaceKHR                                surface,
   const VkAllocationCallbacks*                pAllocator));
VK_FUNC(VkResult, vkGetPhysicalDeviceSurfaceSupportKHR, (
   VkPhysicalDevice                            physicalDevice,
   uint32_t                                    queueFamilyIndex,
   VkSurfaceKHR                                surface,
   VkBool32*                                   pSupported));
VK_FUNC(VkResult, vkGetPhysicalDeviceSurfaceCapabilitiesKHR, (
   VkPhysicalDevice                            physicalDevice,
   VkSurfaceKHR                                surface,
   VkSurfaceCapabilitiesKHR*                   pSurfaceCapabilities));
VK_FUNC(VkResult, vkGetPhysicalDeviceSurfaceFormatsKHR, (
   VkPhysicalDevice                            physicalDevice,
   VkSurfaceKHR                                surface,
   uint32_t*                                   pSurfaceFormatCount,
   VkSurfaceFormatKHR*                         pSurfaceFormats));
VK_FUNC(VkResult, vkCreateSwapchainKHR, (
   VkDevice                                    device,
   const VkSwapchainCreateInfoKHR*             pCreateInfo,
   const VkAllocationCallbacks*                pAllocator,
   VkSwapchainKHR*                             pSwapchain));
VK_FUNC(void, vkDestroySwapchainKHR, (
   VkDevice                                    device,
   VkSwapchainKHR                              swapchain,
   const VkAllocationCallbacks*                pAllocator));
VK_FUNC(VkResult, vkGetSwapchainImagesKHR, (
   VkDevice                                    device,
   VkSwapchainKHR                              swapchain,
   uint32_t*                                   pSwapchainImageCount,
   VkImage*                                    pSwapchainImages));
VK_FUNC(VkResult, vkAcquireNextImageKHR, (
   VkDevice                                    device,
   VkSwapchainKHR                              swapchain,
   uint64_t                                    timeout,
   VkSemaphore                                 semaphore,
   VkFence                                     fence,
   uint32_t*                                   pImageIndex));

#ifdef _WIN32
VK_FUNC(VkResult, vkCreateWin32SurfaceKHR, (
   VkInstance                                  instance,
   const VkWin32SurfaceCreateInfoKHR*          pCreateInfo,
   const VkAllocationCallbacks*                pAllocator,
   VkSurfaceKHR*                               pSurface));
#endif   //_WIN32

#ifndef NDEBUG
VK_FUNC(VkResult, vkCreateDebugUtilsMessengerEXT, (
   VkInstance                                  instance,
   const VkDebugUtilsMessengerCreateInfoEXT*   pCreateInfo,
   const VkAllocationCallbacks*                pAllocator,
   VkDebugUtilsMessengerEXT*                   pMessenger));
VK_FUNC(void, vkDestroyDebugUtilsMessengerEXT, (
   VkInstance                                  instance,
   VkDebugUtilsMessengerEXT                    messenger,
   const VkAllocationCallbacks*                pAllocator));
#endif   //NDEBUG

#endif   //VK_FUNC