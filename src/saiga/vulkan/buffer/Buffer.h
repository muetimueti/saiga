﻿/**
 * Copyright (c) 2017 Darius Rückert
 * Licensed under the MIT License.
 * See LICENSE file for more information.
 */


#pragma once

#include "saiga/vulkan/buffer/DeviceMemory.h"
#include "saiga/vulkan/svulkan.h"

namespace Saiga
{
namespace Vulkan
{
class SAIGA_GLOBAL Buffer
{
   protected:
    VulkanBase* base;
    vk::BufferUsageFlags usageFlags = vk::BufferUsageFlagBits();
    MemoryLocation m_memoryLocation;

   public:
    ~Buffer() { destroy(); }


    void createBuffer(Saiga::Vulkan::VulkanBase& base, size_t size, vk::BufferUsageFlags usage,
                      const vk::MemoryPropertyFlags& flags, vk::SharingMode sharingMode = vk::SharingMode::eExclusive);

    /**
     * Perform a staged upload to the buffer. A StagingBuffer is created and used for this.
     * \see Saiga::Vulkan::StagingBuffer
     * @param base A reference to a VulkanBase
     * @param size Size of the data
     * @param data Pointer to the data.
     */
    void stagedUpload(VulkanBase& base, size_t size, const void* data);

    vk::DescriptorBufferInfo createInfo();

    void flush(VulkanBase& base, vk::DeviceSize size = VK_WHOLE_SIZE, vk::DeviceSize offset = 0);

    void destroy();

    inline vk::DeviceSize size() const { return m_memoryLocation.size; }

    /**
     * Copy this buffer to another with vk::CommandBuffer::copyBuffer
     * @param cmd vk::commandBuffer to use.
     * @param target Target buffer
     * @param srcOffset Offset in bytes at the source.
     * @param dstOffset Offset in bytes at the target.
     * @param size Number of bytes to copy.
     */
    void copyTo(vk::CommandBuffer cmd, Buffer& target, vk::DeviceSize srcOffset = 0, vk::DeviceSize dstOffset = 0,
                vk::DeviceSize size = VK_WHOLE_SIZE);

    /**
     * Copy the buffer to an image with vk::CommandBuffer::copyBufferToImage
     * @param cmd command buffer to use
     * @param dstImage Destination image
     * @param dstImageLayout Destination image layout
     * @param regions Regions to copy
     */
    void copyTo(vk::CommandBuffer cmd, vk::Image dstImage, vk::ImageLayout dstImageLayout,
                vk::ArrayProxy<const vk::BufferImageCopy> regions);

    /**
     * Get a buffer image copy with the correct offset set for this buffer.
     * @param offset Additional offset in bytes from the beginning of the buffer.
     * @return An instance of vk::BufferImageCopy.
     */
    vk::BufferImageCopy getBufferImageCopy(vk::DeviceSize offset) const;

    void update(vk::CommandBuffer cmd, size_t size, void* data, vk::DeviceSize offset = 0);

    void download(void* data) {
        m_memoryLocation.download(base->device, data);
    }

    void upload(size_t size, void* data) {
        m_memoryLocation.upload(base->device,data, size);
    }

    inline bool isMapped() const { return m_memoryLocation.mappedPointer != nullptr; }

    inline void* getMappedPointer() const
    {
        return static_cast<char*>(m_memoryLocation.mappedPointer) + m_memoryLocation.offset;
    }
};

}  // namespace Vulkan
}  // namespace Saiga
