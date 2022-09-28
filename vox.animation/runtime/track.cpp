//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.animation/runtime/track.h"

#include <cassert>

#include "vox.base/io/archive.h"
#include "vox.base/logging.h"
#include "vox.base/memory/allocator.h"
#include "vox.math/math_archive.h"

namespace vox::animation::internal {

template <typename T>
Track<T>::Track() : name_(nullptr) {}

template <typename T>
Track<T>::Track(Track<T>&& _other) noexcept {
    *this = std::move(_other);
}

template <typename T>
Track<T>& Track<T>::operator=(Track<T>&& _other) noexcept {
    std::swap(ratios_, _other.ratios_);
    std::swap(values_, _other.values_);
    std::swap(steps_, _other.steps_);
    std::swap(name_, _other.name_);
    return *this;
}

template <typename T>
Track<T>::~Track() {
    Deallocate();
}

template <typename T>
void Track<T>::Allocate(size_t _keys_count, size_t _name_len) {
    assert(ratios_.empty() && values_.empty());

    // Distributes buffer memory while ensuring proper alignment (serves larger
    // alignment values first).
    static_assert(alignof(T) >= alignof(float) && alignof(float) >= alignof(uint8_t),
                  "Must serve larger alignment values first)");

    // Compute overall size and allocate a single buffer for all the data.
    const size_t buffer_size = _keys_count * sizeof(T) +                  // values
                               _keys_count * sizeof(float) +              // ratios
                               (_keys_count + 7) * sizeof(uint8_t) / 8 +  // steps
                               (_name_len > 0 ? _name_len + 1 : 0);
    span<byte> buffer = {static_cast<byte*>(memory::default_allocator()->Allocate(buffer_size, alignof(T))),
                         buffer_size};

    // Fix up pointers. Serves larger alignment values first.
    values_ = fill_span<T>(buffer, _keys_count);
    ratios_ = fill_span<float>(buffer, _keys_count);
    steps_ = fill_span<uint8_t>(buffer, (_keys_count + 7) / 8);

    // Let name be nullptr if track has no name. Allows to avoid allocating this
    // buffer in the constructor of empty animations.
    name_ = _name_len > 0 ? fill_span<char>(buffer, _name_len + 1).data() : nullptr;

    assert(buffer.empty() && "Whole buffer should be consumed");
}

template <typename T>
void Track<T>::Deallocate() {
    // Deallocate everything at once.
    memory::default_allocator()->Deallocate(as_writable_bytes(values_).data());

    values_ = {};
    ratios_ = {};
    steps_ = {};
    name_ = nullptr;
}

template <typename T>
size_t Track<T>::size() const {
    const size_t size = sizeof(*this) + values_.size_bytes() + ratios_.size_bytes() + steps_.size_bytes();
    return size;
}

template <typename T>
void Track<T>::Save(vox::io::OArchive& _archive) const {
    auto num_keys = static_cast<uint32_t>(ratios_.size());
    _archive << num_keys;

    const size_t name_len = name_ ? std::strlen(name_) : 0;
    _archive << static_cast<int32_t>(name_len);

    _archive << vox::io::MakeArray(ratios_);
    _archive << vox::io::MakeArray(values_);
    _archive << vox::io::MakeArray(steps_);

    _archive << vox::io::MakeArray(name_, name_len);
}

template <typename T>
void Track<T>::Load(vox::io::IArchive& _archive, uint32_t _version) {
    // Destroy animation in case it was already used before.
    Deallocate();

    if (_version > 1) {
        LOGE("Unsupported Track version {}", _version)
        return;
    }

    uint32_t num_keys;
    _archive >> num_keys;

    int32_t name_len;
    _archive >> name_len;

    Allocate(num_keys, name_len);

    _archive >> vox::io::MakeArray(ratios_);
    _archive >> vox::io::MakeArray(values_);
    _archive >> vox::io::MakeArray(steps_);

    if (name_) {  // nullptr name_ is supported.
        _archive >> vox::io::MakeArray(name_, name_len);
        name_[name_len] = 0;
    }
}

// Explicitly instantiate supported tracks.
template class VOX_ANIMATION_DLL Track<float>;
template class VOX_ANIMATION_DLL Track<Vector2F>;
template class VOX_ANIMATION_DLL Track<Vector3F>;
template class VOX_ANIMATION_DLL Track<Vector4F>;
template class VOX_ANIMATION_DLL Track<QuaternionF>;

}  // namespace vox::animation::internal
