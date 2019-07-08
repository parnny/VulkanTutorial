#include "Renderer.h"
#include <cstdlib>
#include <assert.h>
#include <vector>


Renderer::Renderer()
{
	_InitInstance();
	_InitDevice();
}

Renderer::~Renderer()
{
	_DeInitDevice();
	_DeInitInstance();
}

void Renderer::_InitInstance()
{
	VkApplicationInfo app_info = {};
	app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	app_info.apiVersion = VK_API_VERSION_1_1;
	app_info.applicationVersion = VK_MAKE_VERSION(0,1,0);
	app_info.pApplicationName = "Vulkan tutorial";

	VkInstanceCreateInfo inst_create_info = {};
	inst_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	inst_create_info.pApplicationInfo = &app_info;

	auto err = vkCreateInstance(&inst_create_info, nullptr, &_instance);
	if (err != VK_SUCCESS)
	{
		assert(1 && "Vulkan ERROR: Create instance failed!");
		std::exit(-1);
	}
}

void Renderer::_DeInitInstance()
{
	vkDestroyInstance(_instance, nullptr);
	_instance = nullptr;
}

void Renderer::_InitDevice()
{
	{
		uint32_t gpu_count = 0;
		vkEnumeratePhysicalDevices(_instance, &gpu_count, nullptr);
		std::vector<VkPhysicalDevice> gpu_list(gpu_count);
		vkEnumeratePhysicalDevices(_instance, &gpu_count, gpu_list.data());
		_gpu = gpu_list[0];
		vkGetPhysicalDeviceProperties(_gpu, &_gpu_properties);
	}

	{
		uint32_t family_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(_gpu,&family_count,nullptr);
		std::vector<VkQueueFamilyProperties> family_property_list(family_count);
		vkGetPhysicalDeviceQueueFamilyProperties(_gpu, &family_count, family_property_list.data());
		
		bool found = false;
		for (uint32_t i=0;i<family_count;++i)
		{
			if (family_property_list[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				found = true;
				_graphics_family_index = i;
			}
		}

		if (!found)
		{
			assert(1 && "Vulkan ERROR: Queue family supporting graphics not found.");
			std::exit(-1);
		}
	}

	float queue_priorities[]{1.0f};
	VkDeviceQueueCreateInfo device_queue_create_info{};
	device_queue_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	device_queue_create_info.queueFamilyIndex = 1;
	device_queue_create_info.queueCount = 1;
	device_queue_create_info.pQueuePriorities = queue_priorities;


	VkDeviceCreateInfo device_create_info{};
	device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	device_create_info.queueCreateInfoCount = 1;
	device_create_info.pQueueCreateInfos = &device_queue_create_info;

	vkCreateDevice(_gpu, &device_create_info, nullptr, &_device);
}

void Renderer::_DeInitDevice()
{
	vkDestroyDevice(_device, nullptr);
	_device = nullptr;
}

