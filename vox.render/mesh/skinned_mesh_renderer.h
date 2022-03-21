//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef skinned_mesh_renderer_hpp
#define skinned_mesh_renderer_hpp

#include "renderer.h"
#include "loader/fbx_mesh.h"
#include <ozz/animation/runtime/skeleton.h>
#include <ozz/base/containers/vector.h>
#include <ozz/base/maths/soa_transform.h>
#include "graphics/buffer.h"

namespace vox {
class SkinnedMeshRenderer : public Renderer {
public:
    SkinnedMeshRenderer(Entity *entity);
    
    void _render(std::vector<RenderElement> &opaqueQueue,
                 std::vector<RenderElement> &alphaTestQueue,
                 std::vector<RenderElement> &transparentQueue) override;
    
    void _updateBounds(BoundingBox3F &worldBounds) override;
    
    void update(float deltaTime) override;
    
    bool loadSkeleton(const std::string &filename);
    
    bool addSkinnedMesh(const std::string &skin_filename,
                        const std::string &skel_filename);
    
public:
    int numJoints();
    
    int numSoaJoints();
    
public:
    /**
     * Serialize the component
     */
    void onSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node) override;
    
    /**
     * Deserialize the component
     */
    void onDeserialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node) override;
    
    /**
     * Defines how the component should be drawn in the inspector
     */
    void onInspector(ui::WidgetContainer& p_root) override;
    
private:
    // Computes the bounding box of _skeleton. This is the box that encloses all
    // skeleton's joints in model space.
    // _bound must be a valid math::Box instance.
    static void computeSkeletonBounds(const ozz::animation::Skeleton &_skeleton,
                                      BoundingBox3F *_bound);
    
    // Computes the bounding box of posture defines be _matrices range.
    // _bound must be a valid math::Box instance.
    static void computePostureBounds(ozz::span<const ozz::math::Float4x4> _matrices,
                                     BoundingBox3F *_bound);
    
    // Renders a skinned mesh at a specified location.
    std::shared_ptr<Mesh> drawSkinnedMesh(size_t index,
                                          const ozz::loader::Mesh &_mesh,
                                          const ozz::span<ozz::math::Float4x4> _skinning_matrices,
                                          const ozz::math::Float4x4 &_transform);
    
private:
    Animator *_animator{nullptr};
    
    // Runtime skeleton.
    ozz::animation::Skeleton _skeleton;
    
    // Blending job bind pose threshold.
    float _threshold;
    
    // Buffer of local transforms which stores the blending result.
    ozz::vector<ozz::math::SoaTransform> _blendedLocals;
    
    // Buffer of model space matrices. These are computed by the local-to-model
    // job after the blending stage.
    ozz::vector<ozz::math::Float4x4> _models;
    
    // Buffer of skinning matrices, result of the joint multiplication of the
    // inverse bind pose with the model space matrix.
    ozz::vector<ozz::math::Float4x4> _skinningMatrices;
    
    // The mesh used by the sample.
    ozz::vector<ozz::loader::Mesh> _meshes;
    ozz::vector<std::optional<Buffer>> _vertexBuffers;
    ozz::vector<std::optional<Buffer>> _uvBuffers;
    ozz::vector<std::optional<Buffer>> _indexBuffers;
    
    std::vector<wgpu::VertexAttribute> _vertexAttribute{};
    std::vector<wgpu::VertexAttribute> _uvAttribute{};
    std::vector<wgpu::VertexBufferLayout> _vertexBufferLayouts{};
};

}

#endif /* skinned_mesh_renderer_hpp */
