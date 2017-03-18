#pragma once
#include <imgui.h>

struct GLFWwindow;

#define IMGUI_VK_QUEUED_FRAMES 2

struct ImGui_ImplGlfwVulkan_Init_Data
{
	VkAllocationCallbacks* allocator;
	VkPhysicalDevice       gpu;
	VkDevice               device;
	VkRenderPass           render_pass;
	VkPipelineCache        pipeline_cache;
	VkDescriptorPool       descriptor_pool;
	void(*check_vk_result)(VkResult err);
};

IMGUI_API bool        ImGui_ImplGlfwVulkan_Init(GLFWwindow* window, bool install_callbacks, ImGui_ImplGlfwVulkan_Init_Data *init_data);
IMGUI_API void        ImGui_ImplGlfwVulkan_Shutdown();
IMGUI_API void        ImGui_ImplGlfwVulkan_NewFrame();
IMGUI_API void        ImGui_ImplGlfwVulkan_Render(VkCommandBuffer command_buffer);

// Use if you want to reset your rendering device without losing ImGui state.
IMGUI_API void        ImGui_ImplGlfwVulkan_InvalidateFontUploadObjects();
IMGUI_API void        ImGui_ImplGlfwVulkan_InvalidateDeviceObjects();
IMGUI_API bool        ImGui_ImplGlfwVulkan_CreateFontsTexture(VkCommandBuffer command_buffer);
IMGUI_API bool        ImGui_ImplGlfwVulkan_CreateDeviceObjects();

// GLFW callbacks (installed by default if you enable 'install_callbacks' during initialization)
// Provided here if you want to chain callbacks.
// You can also handle inputs yourself and use those as a reference.
IMGUI_API void        ImGui_ImplGlfwVulkan_MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
IMGUI_API void        ImGui_ImplGlfwVulkan_ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);
IMGUI_API void        ImGui_ImplGlfwVulkan_KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
IMGUI_API void        ImGui_ImplGlfwVulkan_CharCallback(GLFWwindow* window, unsigned int c);