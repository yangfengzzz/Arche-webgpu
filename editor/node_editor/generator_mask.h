//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef generator_mask_hpp
#define generator_mask_hpp

#include <string>
#include <vector>

namespace vox {
namespace editor {
#pragma pack(push, 1)
struct GeneratorMask {
    float type;
    
    float pos[3];
    
    float d1[4];
    float d2[4];
    float d3[4];
    float d4[4];
};
#pragma pack(pop)

enum GeneratorMaskType {
    GeneratorMask_Additive = 0,
    GeneratorMask_AverageAdditive,
    GeneratorMask_Multiplicative,
    GeneratorMask_AverageMultiplicative,
    GeneratorMask_Count
};

static const char *generator_mask_type_names[] = {"Additive", "Average Additive", "Multiplicative", "Average Multiplicative"};

#define MASK_LAYER_HILL 0.0f
#define MASK_LAYER_CRATOR 1.0f
#define MASK_LAYER_CLIFF 2.0f

class GeneratorMaskManager {
public:
    GeneratorMaskManager(std::string uid);

    float evaluateAt(float x, float y, float z, float value);
    
    void showSettings();
    
    bool enabled = false;
    std::vector<GeneratorMask> masks;
    std::string uid;
    int gmcount = 0;
    GeneratorMaskType type = GeneratorMaskType::GeneratorMask_Additive;
};

}
}
#endif /* generator_mask_hpp */
