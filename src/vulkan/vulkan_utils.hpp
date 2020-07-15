#ifndef ELEMD_VULKAN_UTILS_HPP
#define ELEMD_VULKAN_UTILS_HPP

#include <iostream>
#include <glad/vulkan.h>

#include "vulkan_context.hpp"
#include "physical_device_composite.hpp"

namespace vku 
{
	inline void err(std::string message)
	{
		std::cerr << "VK ERROR: " << message << std::endl;
		// exit(EXIT_FAILURE);
	}

	inline std::string device_type(const VkPhysicalDeviceType& deviceType)
	{
		std::string dtype = "";

		switch (deviceType) {
		case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
			dtype = "Integrate GPU";
			break;
		case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
			dtype = "Discrete GPU";
			break;
		case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
			dtype = "Virtual GPU";
			break;
		case VK_PHYSICAL_DEVICE_TYPE_CPU:
			dtype = "CPU";
			break;
		default:
			dtype = "Unknown";
			break;
		}

		return dtype;
	}

	inline elemd::PhysicalDeviceComposite select_physical_device(
        VkPhysicalDevice* physicalDevices, uint32_t count)
	{	
		int bestDeviceIndex = 0;
		uint32_t bestQueuFamily = 0;
		uint32_t maxQueueCount = 0;

		for (uint32_t d = 0; d < count; ++d) {
			uint32_t queueFamilyPropertyCount;
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[d],
                                                     &queueFamilyPropertyCount, NULL);

			VkQueueFamilyProperties* queueFamilyProperties = new VkQueueFamilyProperties[queueFamilyPropertyCount];
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[d], &queueFamilyPropertyCount,
                                                     queueFamilyProperties);

			for (uint32_t f = 0; f < queueFamilyPropertyCount; ++f)
			{
				if (maxQueueCount < queueFamilyProperties[f].queueCount)
				{
					maxQueueCount = queueFamilyProperties[f].queueCount;
					bestQueuFamily = f;
					bestDeviceIndex = (int)d;
				}
			}

			delete[] queueFamilyProperties;
		}

		return elemd::PhysicalDeviceComposite{bestDeviceIndex, bestQueuFamily, maxQueueCount};
	}

	inline void print_layers()
    {
        uint32_t layerCount = 0;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        VkLayerProperties* layerProperties = new VkLayerProperties[layerCount];
        vkEnumerateInstanceLayerProperties(&layerCount, layerProperties);

        std::cout << "\"VkLayerProperties\": [\n";
        for (uint32_t i = 0; i < layerCount; ++i)
        {
            std::cout << "{\n"
                      << "\t\"layerName\": \"" << layerProperties[i].layerName << "\",\n"
                      << "\t\"specVersion\": " << layerProperties[i].specVersion << ",\n"
                      << "\t\"implementationVersion\": " << layerProperties[i].implementationVersion
                      << ",\n"
                      << "\t\"description\": \"" << layerProperties[i].description << "\"\n"
                      << "}" << (i + 1 == layerCount ? "\n" : ",\n");
        }
        std::cout << "],\n";

		delete[] layerProperties;
	}
	
	inline void print_extensions()
    {
        uint32_t extencionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extencionCount, nullptr);
        VkExtensionProperties* extensionProperties = new VkExtensionProperties[extencionCount];
        vkEnumerateInstanceExtensionProperties(nullptr, &extencionCount, extensionProperties);

        std::cout << "\"VkExtensionProperties\": [\n";
        for (uint32_t i = 0; i < extencionCount; ++i)
        {
            std::cout << "{\n"
                      << "\t\"extensionName\": \"" << extensionProperties[i].extensionName << "\",\n"
                      << "\t\"specVersion\": " << extensionProperties[i].specVersion << "\n"
                      << "}" << (i + 1 == extencionCount ? "\n" :",\n");
        }
        std::cout << "],\n";

		delete[] extensionProperties;
    }

	inline void print_physical_devices(VkPhysicalDevice* physicalDevices, uint32_t count)
	{
        std::cout << "\"VkPhysicalDevices\": [\n";
        for (uint32_t i = 0; i < count; ++i)
        {
            std::cout << "\t{\n";

            VkPhysicalDeviceProperties properties;
            vkGetPhysicalDeviceProperties(physicalDevices[i], &properties);

            std::cout << "\t\"VkPhysicalDeviceProperties\": {\n"
                      << "\t\t\"apiVersion\": \"" << VK_VERSION_MAJOR(properties.apiVersion) << "."
                      << VK_VERSION_MINOR(properties.apiVersion) << "."
                      << VK_VERSION_PATCH(properties.apiVersion) << "\",\n"
                      << "\t\t\"driverVersion\": " << properties.driverVersion << ",\n"
                      << "\t\t\"vendorID\": " << properties.vendorID << ",\n"
                      << "\t\t\"deviceID\": " << properties.deviceID << ",\n"
                      << "\t\t\"deviceType\": \"" << device_type(properties.deviceType) << "\",\n"
                      << "\t\t\"deviceName\": \"" << properties.deviceName << "\",\n"
                      << "\t\t\"pipelineCacheUUID\": \"" << properties.pipelineCacheUUID
                      << "\"\n"
                      //<< "\t\t\"limits\": " << properties.limits << ",\n"
                      //<< "\t\t\"sparseProperties\": \"" << properties.sparseProperties << "\",\n"
                      << "\t},\n";

            VkPhysicalDeviceFeatures features;
            vkGetPhysicalDeviceFeatures(physicalDevices[i], &features);

            std::cout << "\t\"VkPhysicalDeviceFeatures\": {\n"
                      << "\t\t\"geometryShader\": " << std::boolalpha << features.geometryShader
                      << ",\n"
                      << "\t\t\"tessellationShader\": " << std::boolalpha
                      << features.tessellationShader << "\n"
                      << "\t},\n";

            VkPhysicalDeviceMemoryProperties physicalDeviceMemoryProperties;
            vkGetPhysicalDeviceMemoryProperties(physicalDevices[i],
                                                &physicalDeviceMemoryProperties);

            uint32_t queueFamilyPropertyCount;
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &queueFamilyPropertyCount,
                                                     NULL);

            VkQueueFamilyProperties* queueFamilyProperties =
                new VkQueueFamilyProperties[queueFamilyPropertyCount];
            vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices[i], &queueFamilyPropertyCount,
                                                     queueFamilyProperties);

            std::cout << "\t\"VkQueueFamilyProperties\": [\n";
            for (uint32_t q = 0; q < queueFamilyPropertyCount; ++q)
            {
                std::cout
                    << "\t\t{\n"
                    << "\t\t\t\"VK_QUEUE_GRAPHICS_BIT\": " << std::boolalpha
                    << ((queueFamilyProperties[q].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) << ",\n"
                    << "\t\t\t\"VK_QUEUE_COMPUTE_BIT\": " << std::boolalpha
                    << ((queueFamilyProperties[q].queueFlags & VK_QUEUE_COMPUTE_BIT) != 0) << ",\n"
                    << "\t\t\t\"VK_QUEUE_TRANSFER_BIT\": " << std::boolalpha
                    << ((queueFamilyProperties[q].queueFlags & VK_QUEUE_TRANSFER_BIT) != 0) << ",\n"
                    << "\t\t\t\"VK_QUEUE_SPARSE_BINDING_BIT\": " << std::boolalpha
                    << ((queueFamilyProperties[q].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0)
                    << ",\n"
                    << "\t\t\t\"queueCount\": " << queueFamilyProperties[q].queueCount << ",\n"
                    << "\t\t\t\"timestampValidBits\": "
                    << queueFamilyProperties[q].timestampValidBits << "\n"
                    << "\t\t}" << (q + 1 == queueFamilyPropertyCount ? "\n" : ",\n");
            }
            std::cout << "\t]\n";

			std::cout << "\t}" << (i + 1 == count ? "\n" : ",\n");
		
			delete[] queueFamilyProperties;
        }

		std::cout << "],\n";
	}

	inline void print_selected_device(const elemd::PhysicalDeviceComposite& bestDevice)
    {
        std::cout << "\"selected_device\": {\n"
                  << "\t\"deviceIndex\": " << bestDevice.deviceIndex << ",\n"
                  << "\t\"queueCount\": " << bestDevice.queueCount << ",\n"
                  << "\t\"queueFamilyIndex\": " << bestDevice.queueFamilyIndex << "\n"
                  << "},\n";
    }

	inline bool err_check(const VkResult& result)
	{
		if (result != VK_SUCCESS)
		{
			switch (result) {
			case VK_NOT_READY:
				err("A fence or query has not yet completed");
				break;
			case VK_TIMEOUT:
				err("A wait operation has not completed in the specified time");
				break;
			case VK_EVENT_SET:
				err("An event is signaled");
				break;
			case VK_EVENT_RESET:
				err("An event is unsignaled");
				break;
			case VK_INCOMPLETE:
				err("A return array was too small for the result");
				break;
			case VK_SUBOPTIMAL_KHR:
				err("A swapchain no longer matches the surface properties exactly, but can still be used to present to the surface successfully.");
				break;
			case VK_ERROR_OUT_OF_HOST_MEMORY:
				err("A host memory allocation has failed.");
				break;
			case VK_ERROR_OUT_OF_DEVICE_MEMORY:
				err("A device memory allocation has failed.");
				break;
			case VK_ERROR_INITIALIZATION_FAILED:
				err("Initialization of an object could not be completed for implementation-specific reasons.");
				break;
			case VK_ERROR_DEVICE_LOST:
				err("The logical or physical device has been lost. See Lost Device");
				break;
			case VK_ERROR_MEMORY_MAP_FAILED:
				err("Mapping of a memory object has failed.");
				break;
			case VK_ERROR_LAYER_NOT_PRESENT:
				err("A requested layer is not present or could not be loaded.");
				break;
			case VK_ERROR_EXTENSION_NOT_PRESENT:
				err("A requested extension is not supported.");
				break;
			case VK_ERROR_FEATURE_NOT_PRESENT:
				err("A requested feature is not supported.");
				break;
			case VK_ERROR_INCOMPATIBLE_DRIVER:
				err("The requested version of Vulkan is not supported by the driver or is otherwise incompatible for implementation-specific reasons.");
				break;
			case VK_ERROR_TOO_MANY_OBJECTS:
				err("Too many objects of the type have already been created.");
				break;
			case VK_ERROR_FORMAT_NOT_SUPPORTED:
				err("A requested format is not supported on this device.");
				break;
			case VK_ERROR_FRAGMENTED_POOL:
				err("A pool allocation has failed due to fragmentation of the pool's memory. This must only be returned if no attempt to allocate host or device memory was made to accommodate the new allocation. This should be returned in preference to VK_ERROR_OUT_OF_POOL_MEMORY, but only if the implementation is certain that the pool allocation failure was due to fragmentation.");
				break;
			case VK_ERROR_SURFACE_LOST_KHR:
				err("A surface is no longer available.");
				break;
			case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:
				err("The requested window is already in use by Vulkan or another API in a manner which prevents it from being used again.");
				break;
			case VK_ERROR_OUT_OF_DATE_KHR:
				err("A surface has changed in such a way that it is no longer compatible with the swapchain, and further presentation requests using the swapchain will fail. Applications must query the new surface properties and recreate their swapchain if they wish to continue presenting to the surface.");
				break;
			//case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:
			//	err("The display used by a swapchain does not use the same presentable image layout, or is incompatible in a way that prevents sharing an image.");
			//	break;
			//case VK_ERROR_INVALID_SHADER_NV:
			//	err("One or more shaders failed to compile or link. More details are reported back to the application via VK_EXT_debug_report if enabled.");
			//	break;
			case VK_ERROR_OUT_OF_POOL_MEMORY:
				err("A pool memory allocation has failed. This must only be returned if no attempt to allocate host or device memory was made to accommodate the new allocation. If the failure was definitely due to fragmentation of the pool, VK_ERROR_FRAGMENTED_POOL should be returned instead.");
				break;
			case VK_ERROR_INVALID_EXTERNAL_HANDLE:
				err("An external handle is not a valid handle of the specified type.");
				break;
			case VK_ERROR_FRAGMENTATION:
				err("A descriptor pool creation has failed due to fragmentation.");
				break;
			case VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:
				err("A buffer creation or memory allocation failed because the requested address is not available. A shader group handle assignment failed because the requested shader group handle information is no longer valid.");
				break;
			//case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:
			//	err("An operation on a swapchain created with VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT failed as it did not have exlusive full-screen access. This may occur due to implementation-dependent reasons, outside of the application's control.");
			//	break;
			default:
				err("An unknown error has occurred; either the application has provided invalid input, or an implementation failure has occurred");
				break;
			}

			return false;
		}
		else {
			return true;
		}
	}
}

#endif // ELEMD_VULKAN_UTILS_HPP