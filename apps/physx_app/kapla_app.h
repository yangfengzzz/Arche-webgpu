//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef kapla_app_hpp
#define kapla_app_hpp

#include "forward_application.h"

namespace vox {
class KaplaApp : public ForwardApplication {
public:
    void loadScene() override;
    
    void createCylindricalTower(Entity* entity, uint32_t nbRadialPoints, float maxRadius, float minRadius, uint32_t height,
                                const Vector3F& dims, const Vector3F& centerPos, physics::PxMaterial *material,
                                physics::PxFilterData &simFilterData, physics::PxFilterData &queryFilterData,
                                float density = 1.f, bool bUseSweeps = false, bool bStartAsleep = false);
};

}
#endif /* kapla_app_hpp */
