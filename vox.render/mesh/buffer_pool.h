//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/helper.h"
#include "vox.render/mesh/buffer.h"

namespace vox {
/**
 * @brief An allocation of vulkan memory; different buffer allocations,
 *        with different offset and size, may come from the same Vulkan buffer
 */
class BufferAllocation {
public:
    BufferAllocation() = default;

    BufferAllocation(Buffer &buffer, uint64_t size, uint64_t offset);

    BufferAllocation(const BufferAllocation &) = delete;

    BufferAllocation(BufferAllocation &&) = default;

    BufferAllocation &operator=(const BufferAllocation &) = delete;

    BufferAllocation &operator=(BufferAllocation &&) = default;

    void update(wgpu::Device &device, const std::vector<uint8_t> &data, uint64_t offset = 0);

    template <class T>
    void update(wgpu::Device &device, const T &value, uint64_t offset = 0) {
        update(device, utility::ToBytes(value), offset);
    }

    [[nodiscard]] bool empty() const;

    [[nodiscard]] uint64_t getSize() const;

    [[nodiscard]] uint64_t getOffset() const;

    [[nodiscard]] const Buffer &getBuffer() const;

private:
    Buffer *buffer_{nullptr};

    uint64_t base_offset_{0};

    uint64_t size_{0};
};

/**
 * @brief Helper class which handles multiple allocation from the same underlying Vulkan buffer.
 */
class BufferBlock {
public:
    BufferBlock(wgpu::Device &device, uint64_t size, wgpu::BufferUsage usage);

    /**
     * @return An usable view on a portion of the underlying buffer
     */
    BufferAllocation allocate(uint64_t size);

    [[nodiscard]] uint64_t getSize() const;

    void reset();

private:
    Buffer buffer_;

    // Memory alignment, it may change according to the usage
    uint64_t alignment_{16};

    // Current offset, it increases on every allocation
    uint64_t offset_{0};
};

/**
 * @brief A pool of buffer blocks for a specific usage.
 * It may contain inactive blocks that can be recycled.
 *
 * BufferPool is a linear allocator for buffer chunks, it gives you a view of the size you want.
 * A BufferBlock is the corresponding VkBuffer and you can get smaller offsets inside it.
 * Since a shader cannot specify dynamic UBOs, it has to be done from the code
 * (set_resource_dynamic).
 *
 * When a new frame starts, buffer blocks are returned: the offset is reset and contents are
 * overwritten. The minimum allocation size is 256 kb, if you ask for more you get a dedicated
 * buffer allocation.
 *
 * We re-use descriptor sets: we only need one for the corresponding buffer infos (and we only
 * have one VkBuffer per BufferBlock), then it is bound and we use dynamic offsets.
 */
class BufferPool {
public:
    BufferPool(wgpu::Device &device, uint64_t block_size, wgpu::BufferUsage usage);

    BufferBlock &requestBufferBlock(uint64_t minimum_size);

    void reset();

private:
    wgpu::Device &device_;

    /// List of blocks requested
    std::vector<std::unique_ptr<BufferBlock>> buffer_blocks_;

    /// Minimum size of the blocks
    uint64_t block_size_{0};

    wgpu::BufferUsage usage_{};

    /// Numbers of active blocks from the start of buffer_blocks
    uint64_t active_buffer_block_count_{0};
};

}  // namespace vox