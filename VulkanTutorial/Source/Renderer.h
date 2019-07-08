#pragma once

#include "vulkan/vulkan.h"

class Renderer
{
public:
	Renderer();
	~Renderer();


private:
	void _InitInstance();
	void _DeInitInstance();

	void _InitDevice();
	void _DeInitDevice();


	VkInstance					_instance				= nullptr;
	VkDevice					_device					= nullptr;
	VkPhysicalDevice			_gpu					= nullptr;
	VkPhysicalDeviceProperties	_gpu_properties			= {};

	uint32_t			_graphics_family_index	= 0;
};