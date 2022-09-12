//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/fog/fog.h"

namespace vox {
class LinearFog : public Fog {
public:
    /**
     * Start of fog.
     */
    [[nodiscard]] float getNear() const;

    void setNear(float value);

    /**
     * End of fog.
     */
    [[nodiscard]] float getFar() const;

    void setFar(float value);

    explicit LinearFog(Entity* entity);

private:
    float _near = 1;
    float _far = 1000;
    static const std::string _nearProperty;
    static const std::string _farProperty;
};
}  // namespace vox