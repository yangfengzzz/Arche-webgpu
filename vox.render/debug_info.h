//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>
#include <utility>
#include <vector>

#include "vox.render/helper.h"
#include "vox.math/vector3.h"
#include "vox.render/error.h"

namespace vox {
namespace field {
/**
 * @brief Base Field Interface class
 */
struct Base {
    std::string label;

    explicit Base(std::string label) : label{std::move(label)} {}

    virtual ~Base() = default;

    virtual std::string to_string() = 0;
};

/**
 * @brief Static Field Implementation
 *
 * To be used for values that do not change often.
 */
template <typename T>
struct Static : public Base {
    T value;

    Static(const std::string &label, const T &value) : Base(label), value{value} {}

    ~Static() override = default;

    std::string to_string() override { return vox::ToString(value); }
};

/**
 * @brief Dynamic Field Implementation
 *
 * To be used for values that change frequently.
 */
template <typename T>
struct Dynamic : public Base {
    T &value;

    Dynamic(const std::string &label, T &value) : Base(label), value{value} {}

    ~Dynamic() override = default;

    std::string to_string() override { return vox::ToString(value); }
};

/**
 * @brief Vector Field Implementation
 *
 * To be used for values that have an X, Y and Z value.
 */
template <typename T>
struct Vector final : public Static<T> {
    T x, y, z;

    Vector(const std::string &label, const Vector3F &vec) : Vector(label, vec.x, vec.y, vec.z) {}

    Vector(const std::string &label, T x, T y, T z) : Static<T>(label, x), x{x}, y{y}, z{z} {}

    virtual ~Vector() = default;

    std::string to_string() override {
        return "x: " + vox::ToString(x) + " " + "y: " + vox::ToString(y) + " " + "z: " + vox::ToString(z);
    }
};

/**
 * @brief MinMax Field Implementation
 *
 * To be used for numbers that change a lot, keeping track of the high/low values.
 */
template <typename T>
struct MinMax final : public Dynamic<T> {
    T min, max;

    MinMax(const std::string &label, T &value) : Dynamic<T>(label, value), min{value}, max{value} {
        static_assert(std::is_arithmetic<T>::value, "MinMax must be templated to a numeric type.");
    }

    virtual ~MinMax() = default;

    std::string to_string() override {
        if (Dynamic<T>::value > max) {
            max = Dynamic<T>::value;
        }
        if (Dynamic<T>::value < min) {
            min = Dynamic<T>::value;
        }
        if (min == 0) {
            min = Dynamic<T>::value;
        }

        return "current: " + vox::ToString(Dynamic<T>::value) + " min: " + vox::ToString(min) +
               " max: " + vox::ToString(max);
    }
};
}  // namespace field

/**
 * @brief Manages the debug information
 */
class DebugInfo {
public:
    std::vector<std::unique_ptr<field::Base>> &get_fields();

    /**
     * @brief   Calculates the field label with the most amount of characters
     * @returns The length of the longest label
     */
    [[nodiscard]] float get_longest_label() const;

    /**
     * @brief Constructs and inserts a new field of type C<T>
     *
     * Replaces the field if it is of type static.
     */
    template <template <typename> class C, typename T, typename... A>
    void insert(const std::string &label, A &&...args) {
        static_assert(std::is_base_of<field::Base, C<T>>::value, "C is not a type of field::Base.");

        for (auto &field : fields_) {
            if (field->label == label) {
                if (dynamic_cast<typename field::Static<T> *>(field.get())) {
                    field = std::make_unique<C<T>>(label, args...);
                }
                return;
            }
        }

        auto field = std::make_unique<C<T>>(label, std::forward<A>(args)...);
        fields_.push_back(std::move(field));
    }

private:
    std::vector<std::unique_ptr<field::Base>> fields_;
};

}  // namespace vox
