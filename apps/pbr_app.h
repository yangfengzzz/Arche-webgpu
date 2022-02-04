//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef pbr_app_hpp
#define pbr_app_hpp

#include "forward_application.h"
#include "controls/orbit_control.h"

namespace vox {
class PBRApp : public ForwardApplication {
public:
    void loadScene(uint32_t width, uint32_t height) override;
    
private:
    struct Material {
        std::string name;
        Color baseColor;
        float roughness;
        float metallic;
        
        Material() {
        };
        
        Material(std::string n, Color c, float r, float m) : name(n) {
            roughness = r;
            metallic = m;
            baseColor = c;
        };
    };
    std::vector<Material> _materials;
};

}

#endif /* pbr_app_hpp */
