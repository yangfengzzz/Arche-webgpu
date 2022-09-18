//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/mesh/buffer_pool.h"

#include "vox.base/logging.h"

namespace vox {
BufferAllocation::BufferAllocation(Buffer &buffer, uint32_t size, uint32_t offset)
    : buffer_{&buffer}, size_{size}, base_offset_{offset} {}

void BufferAllocation::update(wgpu::Device &device, const std::vector<uint8_t> &data, uint32_t offset) {
    assert(buffer_ && "Invalid buffer pointer");

    if (offset + data.size() <= size_) {
        buffer_->uploadData(device, data.data(), data.size(), utility::ToU32(base_offset_) + offset);
    } else {
        LOGE("Ignore buffer allocation update")
    }
}

bool BufferAllocation::empty() const { return size_ == 0 || buffer_ == nullptr; }

uint32_t BufferAllocation::getSize() const { return size_; }

uint32_t BufferAllocation::getOffset() const { return base_offset_; }

const Buffer &BufferAllocation::getBuffer() const {
    assert(buffer_ && "Invalid buffer pointer");
    return *buffer_;
}

//----------------------------------------------------------------------------------------------------------------------
BufferBlock::BufferBlock(wgpu::Device &device, uint32_t size, wgpu::BufferUsage usage) : buffer_{device, size, usage} {}

BufferAllocation BufferBlock::allocate(const uint32_t allocate_size) {
    assert(allocate_size > 0 && "Allocation size must be greater than zero");

    auto aligned_offset = (offset_ + alignment_ - 1) & ~(alignment_ - 1);

    if (aligned_offset + allocate_size > buffer_.size()) {
        // No more space available from the underlying buffer, return empty allocation
        return BufferAllocation{};
    }

    // Move the current offset and return an allocation
    offset_ = aligned_offset + allocate_size;
    return BufferAllocation{buffer_, allocate_size, aligned_offset};
}

uint32_t BufferBlock::getSize() const { return buffer_.size(); }

void BufferBlock::reset() { offset_ = 0; }

//----------------------------------------------------------------------------------------------------------------------
BufferPool::BufferPool(wgpu::Device &device, uint32_t block_size, wgpu::BufferUsage usage)
    : device_{device}, block_size_{block_size}, usage_{usage} {}

BufferBlock &BufferPool::requestBufferBlock(uint32_t minimum_size) {
    // Find the first block in the range of the inactive blocks
    // which can fit the minimum size
    auto it = std::upper_bound(
            buffer_blocks_.begin() + active_buffer_block_count_, buffer_blocks_.end(), minimum_size,
            [](const uint32_t &a, const std::unique_ptr<BufferBlock> &b) -> bool { return a <= b->getSize(); });

    if (it != buffer_blocks_.end()) {
        // Recycle inactive block
        active_buffer_block_count_++;
        return *it->get();
    }

    LOGD("Building #{} buffer block ({})", buffer_blocks_.size(), uint32_t(usage_))

    // Create a new block, store and return it
    buffer_blocks_.emplace_back(std::make_unique<BufferBlock>(device_, std::max(block_size_, minimum_size), usage_));

    auto &block = buffer_blocks_[active_buffer_block_count_++];

    return *block;
}

void BufferPool::reset() {
    for (auto &buffer_block : buffer_blocks_) {
        buffer_block->reset();
    }

    active_buffer_block_count_ = 0;
}

}  // namespace vox