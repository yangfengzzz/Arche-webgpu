//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef scene_forward_h
#define scene_forward_h

#include <memory>

namespace vox {
class Scene;

using ScenePtr = std::shared_ptr<Scene>;

class Entity;

class Component;

class ComponentsManager;

class Transform;

class SubMesh;

class Mesh;

using MeshPtr = std::shared_ptr<Mesh>;

class ModelMesh;

using ModelMeshPtr = std::shared_ptr<ModelMesh>;

class Material;

using MaterialPtr = std::shared_ptr<Material>;

class UnlitMaterial;

using UnlitMaterialPtr = std::shared_ptr<UnlitMaterial>;

class BlinnPhongMaterial;

using BlinnPhongMaterialPtr = std::shared_ptr<BlinnPhongMaterial>;

class PBRMaterial;

using PBRMaterialPtr = std::shared_ptr<PBRMaterial>;

class Camera;

class Renderer;

class MeshRenderer;

class GPUSkinnedMeshRenderer;

class Script;

class Behaviour;

class Animator;

class SceneAnimator;

class Light;

class PointLight;

class SpotLight;

class DirectLight;

}

#endif /* scene_forward_h */
