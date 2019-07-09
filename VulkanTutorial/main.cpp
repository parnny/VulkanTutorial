/*
 * Vulkan Program
 *
 * Copyright (C) 2016 Valve Corporation
 * Copyright (C) 2016 LunarG, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*
Vulkan C++ Project Template
Create and destroy a simple Vulkan instance.
*/

#include <vulkan/vulkan.hpp>

#include <iostream>
#include <vector>


#include "Source/Renderer.h"

int main()
{
	Renderer r;

	VkFence fence;
	VkFenceCreateInfo fence_create_info {};
	fence_create_info.sType				= VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	vkCreateFence(r._device, &fence_create_info, nullptr, &fence);

	VkSemaphore semaphore;
	VkSemaphoreCreateInfo semaphore_create_info{};
	semaphore_create_info.sType			= VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	vkCreateSemaphore( r._device, &semaphore_create_info, nullptr, &semaphore );

	VkCommandPool command_pool;
	VkCommandPoolCreateInfo command_pool_create_info	= {};
	command_pool_create_info.sType						= VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	command_pool_create_info.queueFamilyIndex			= r._graphics_family_index;
	command_pool_create_info.flags						= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

	vkCreateCommandPool(r._device, &command_pool_create_info, nullptr, &command_pool);
	
	VkCommandBuffer command_buffer[2];
	VkCommandBufferAllocateInfo command_buff_allocate_info{};
	command_buff_allocate_info.sType					= VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	command_buff_allocate_info.commandPool				= command_pool;
	command_buff_allocate_info.level					= VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	command_buff_allocate_info.commandBufferCount		= 2;
	vkAllocateCommandBuffers( r._device, &command_buff_allocate_info, command_buffer);

	{
		VkCommandBufferBeginInfo command_buffer_begin_info{};
		command_buffer_begin_info.sType						= VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		vkBeginCommandBuffer(command_buffer[0], &command_buffer_begin_info);

		vkCmdPipelineBarrier(command_buffer[0],
			VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
			0,
			0, nullptr,
			0, nullptr,
			0, nullptr);
			
		VkViewport viewport{};
		viewport.maxDepth		= 1.0f;
		viewport.minDepth		= 0.0f;
		viewport.width			= 512;
		viewport.height			= 512;
		viewport.x				= 0;
		viewport.y				= 0;
		vkCmdSetViewport( command_buffer[0], 0, 1, &viewport);

		vkEndCommandBuffer(command_buffer[0]);
	}
	{
		VkCommandBufferBeginInfo command_buffer_begin_info{};
		command_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		vkBeginCommandBuffer(command_buffer[1], &command_buffer_begin_info);

		VkViewport viewport{};
		viewport.maxDepth = 1.0f;
		viewport.minDepth = 0.0f;
		viewport.width = 512;
		viewport.height = 512;
		viewport.x = 0;
		viewport.y = 0;
		vkCmdSetViewport(command_buffer[1], 0, 1, &viewport);

		vkEndCommandBuffer(command_buffer[1]);
	}
	{
		VkSubmitInfo submit_info{};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.commandBufferCount			= 1;
		submit_info.pCommandBuffers				= &command_buffer[0];
		submit_info.signalSemaphoreCount		= 1;
		submit_info.pSignalSemaphores			= &semaphore;
		vkQueueSubmit(r._queue, 1, &submit_info, VK_NULL_HANDLE);
	}
	{
		VkPipelineStageFlags pipeline_stage_flags[]{VK_PIPELINE_STAGE_ALL_COMMANDS_BIT};
		VkSubmitInfo submit_info{};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info.commandBufferCount			= 1;
		submit_info.pCommandBuffers				= &command_buffer[1];
		submit_info.waitSemaphoreCount			= 1;
		submit_info.pWaitSemaphores				= &semaphore;
		submit_info.pWaitDstStageMask			= pipeline_stage_flags;
		vkQueueSubmit(r._queue, 1, &submit_info, VK_NULL_HANDLE);
	}
	//auto ret = vkWaitForFences( r._device, 1, &fence, VK_TRUE, UINT64_MAX);
	vkQueueWaitIdle( r._queue);

	vkDestroyCommandPool(r._device, command_pool, nullptr);
	vkDestroyFence( r._device, fence, nullptr );
	vkDestroySemaphore(r._device, semaphore, nullptr);

	return 0;

    // Use validation layers if this is a debug build
    std::vector<const char*> layers;
#if defined(_DEBUG)
    layers.push_back("VK_LAYER_LUNARG_standard_validation");
#endif

    // VkApplicationInfo allows the programmer to specifiy some basic information about the
    // program, which can be useful for layers and tools to provide more debug information.
    vk::ApplicationInfo appInfo = vk::ApplicationInfo()
        .setPApplicationName("Vulkan C++ Program Template")
        .setApplicationVersion(1)
        .setPEngineName("LunarG SDK")
        .setEngineVersion(1)
        .setApiVersion(VK_API_VERSION_1_0);

    // VkInstanceCreateInfo is where the programmer specifies the layers and/or extensions that
    // are needed. For now, none are enabled.
    vk::InstanceCreateInfo instInfo = vk::InstanceCreateInfo()
        .setFlags(vk::InstanceCreateFlags())
        .setPApplicationInfo(&appInfo)
        .setEnabledExtensionCount(0)
        .setPpEnabledExtensionNames(NULL)
        .setEnabledLayerCount(static_cast<uint32_t>(layers.size()))
        .setPpEnabledLayerNames(layers.data());

    // Create the Vulkan instance.
    vk::Instance instance;
    try {
        instance = vk::createInstance(instInfo);
    } catch(std::exception e) {
        std::cout << "Could not create a Vulkan instance: " << e.what() << std::endl;
        return 1;
    }

    // Normally, a program would do something with the instance here. This, however, is just a
    // simple demo program, so we just finish up right away.

    instance.destroy();


    return 0;
}
