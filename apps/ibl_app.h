//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef ibl_app_hpp
#define ibl_app_hpp

#include "forward_application.h"
#include "controls/orbit_control.h"
#include "texture/sampled_texturecube.h"

namespace vox {
class IBLApp : public ForwardApplication {
public:
    bool prepare(Engine &engine) override;
    
    void loadScene(uint32_t width, uint32_t height) override;
    
private:
    SampledTextureCubePtr _cubeMap{nullptr};
    
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
    
    std::string _path;
    std::array<std::string, 6> _images;
};
}

#endif /* ibl_app_hpp */
