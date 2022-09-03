//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef gpu_skinned_mesh_renderer_hpp
#define gpu_skinned_mesh_renderer_hpp

#include "mesh_renderer.h"

namespace vox {

class GPUSkinnedMeshRenderer : public MeshRenderer {
public:
    /**
     * Returns the name of the component
     */
    std::string name() override;

    struct Skin {
        std::string name;
        std::vector<Matrix4x4F> inverseBindMatrices;
        std::vector<Entity*> joints;
    };
    using SkinPtr = std::shared_ptr<Skin>;

public:
    GPUSkinnedMeshRenderer(Entity* entity);

    /**
     * Skin Object.
     */
    SkinPtr skin();

    void setSkin(const SkinPtr& skin);

    void update(float deltaTime) override;

public:
    /**
     * Serialize the component
     */
    void onSerialize(nlohmann::json& data) override;

    /**
     * Deserialize the component
     */
    void onDeserialize(nlohmann::json& data) override;

    /**
     * Defines how the component should be drawn in the inspector
     */
    void onInspector(ui::WidgetContainer& p_root) override;

private:
    SkinPtr _skin;

    void _initJoints();

    bool _hasInitJoints = false;

    std::vector<float> jointMatrix{};
    ShaderProperty _jointMatrixProperty;
};

}  // namespace vox

#endif /* gpu_skinned_mesh_renderer_hpp */
