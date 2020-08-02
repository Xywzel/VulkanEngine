#pragma once

#include "Types.h"

// Enum types
enum VkFormat;

// Pure structs
struct VkExtent2D;
struct VkVertexInputBindingDescription;
struct VkVertexInputAttributeDescription;

typedef uint32_t VkFlags;
typedef VkFlags VkBufferUsageFlags;
typedef VkFlags VkMemoryPropertyFlags;

// Structs with pointer wrappers
struct VkCommandPool_T;
typedef VkCommandPool_T* VkCommandPool;

struct VkDevice_T;
typedef VkDevice_T* VkDevice;

struct VkQueue_T;
typedef VkQueue_T* VkQueue;

struct VkPhysicalDevice_T;
typedef VkPhysicalDevice_T* VkPhysicalDevice;

struct VkFramebuffer_T;
typedef VkFramebuffer_T* VkFramebuffer;

struct VkRenderPass_T;
typedef VkRenderPass_T* VkRenderPass;

struct VkImageView_T;
typedef VkImageView_T* VkImageView;

struct VkCommandBuffer_T;
typedef VkCommandBuffer_T* VkCommandBuffer;

struct VkBuffer_T;
typedef VkBuffer_T* VkBuffer;

struct VkDeviceMemory_T;
typedef VkDeviceMemory_T* VkDeviceMemory;
