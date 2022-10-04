//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/animation_ik_floor_app.h"

#include "vox.geometry/matrix_utils.h"
#include "vox.render/animation/animation_states/animation_clip.h"
#include "vox.render/animation/animator.h"
#include "vox.render/animation/skinned_mesh_renderer.h"
#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.render/material/blinn_phong_material.h"
#include "vox.render/material/pbr_material.h"
#include "vox.render/mesh/primitive_mesh.h"
#include "vox.toolkit/controls/orbit_control.h"
#include "vox.toolkit/skeleton_view/skeleton_view.h"

namespace vox {
namespace {
// Moller–Trumbore intersection algorithm
// https://en.wikipedia.org/wiki/M%C3%B6ller%E2%80%93Trumbore_intersection_algorithm
bool rayIntersectsTriangle(const Vector3F& _ray_origin,
                           const Vector3F& _ray_direction,
                           const Vector3F& _p0,
                           const Vector3F& _p1,
                           const Vector3F& _p2,
                           Vector3F* _intersect,
                           Vector3F* _normal) {
    const float kEpsilon = 0.0000001f;

    const Vector3F edge1 = _p1 - _p0;
    const Vector3F edge2 = _p2 - _p0;
    const Vector3F h = _ray_direction.cross(edge2);

    const float a = edge1.dot(h);
    if (a > -kEpsilon && a < kEpsilon) {
        return false;  // This ray is parallel to this triangle.
    }

    const float inv_a = 1.f / a;
    const Vector3F s = _ray_origin - _p0;
    const float u = s.dot(h) * inv_a;
    if (u < 0.f || u > 1.f) {
        return false;
    }

    const Vector3F q = s.cross(edge1);
    const float v = _ray_direction.dot(q) * inv_a;
    if (v < 0.f || u + v > 1.f) {
        return false;
    }

    // At this stage we can compute t to find out where the intersection point is
    // on the line.
    const float t = edge2.dot(q) * inv_a;

    if (t > kEpsilon) {  // Ray intersection
        *_intersect = _ray_origin + _ray_direction * t;
        *_normal = edge1.cross(edge2).normalized();
        return true;
    } else {  // This means that there is a line intersection but not a ray
              // intersection.
        return false;
    }
}

bool rayIntersectsMesh(const Vector3F& _ray_origin,
                       const Vector3F& _ray_direction,
                       const std::shared_ptr<Skin>& _mesh,
                       Vector3F* _intersect,
                       Vector3F* _normal) {
    assert(_mesh->parts.size() == 1 && !_mesh->skinned());

    bool intersected = false;
    Vector3F intersect, normal;
    const float* vertices = array_begin(_mesh->parts[0].positions);
    const uint16_t* indices = array_begin(_mesh->triangle_indices);
    for (int i = 0; i < _mesh->triangle_index_count(); i += 3) {
        const float* pf0 = vertices + indices[i + 0] * 3;
        const float* pf1 = vertices + indices[i + 1] * 3;
        const float* pf2 = vertices + indices[i + 2] * 3;
        Vector3F lcl_intersect, lcl_normal;
        if (rayIntersectsTriangle(_ray_origin, _ray_direction, Vector3F(pf0[0], pf0[1], pf0[2]),
                                  Vector3F(pf1[0], pf1[1], pf1[2]), Vector3F(pf2[0], pf2[1], pf2[2]), &lcl_intersect,
                                  &lcl_normal)) {
            // Is it closer to start point than the previous intersection.
            if (!intersected ||
                lcl_intersect.distanceSquaredTo(_ray_origin) < intersect.distanceSquaredTo(_ray_origin)) {
                intersect = lcl_intersect;
                normal = lcl_normal;
            }
            intersected = true;
        }
    }

    // Copy output
    if (intersected) {
        if (_intersect) {
            *_intersect = intersect;
        }
        if (_normal) {
            *_normal = normal;
        }
    }
    return intersected;
}

bool rayIntersectsMeshes(const Vector3F& _ray_origin,
                         const Vector3F& _ray_direction,
                         const span<const std::shared_ptr<Skin>>& _meshes,
                         Vector3F* _intersect,
                         Vector3F* _normal) {
    bool intersected = false;
    Vector3F intersect, normal;
    for (const auto& mesh : _meshes) {
        Vector3F lcl_intersect, lcl_normal;
        if (rayIntersectsMesh(_ray_origin, _ray_direction, mesh, &lcl_intersect, &lcl_normal)) {
            // Is it closer to start point than the previous intersection.
            if (!intersected ||
                lcl_intersect.distanceSquaredTo(_ray_origin) < intersect.distanceSquaredTo(_ray_origin)) {
                intersect = lcl_intersect;
                normal = lcl_normal;
            }
            intersected = true;
        }
    }

    // Copy output
    if (intersected) {
        if (_intersect) {
            *_intersect = intersect;
        }
        if (_normal) {
            *_normal = normal;
        }
    }
    return intersected;
}

class FloorTargetScript : public Script {
public:
    Animator* animator{nullptr};
    std::vector<std::shared_ptr<Skin>> floor;
    Animator::FloorIKData data;
    const char* kLeftJointNames[3] = {"LeftUpLeg", "LeftLeg", "LeftFoot"};
    const char* kRightJointNames[3] = {"RightUpLeg", "RightLeg", "RightFoot"};
    enum { kLeft, kRight };

    void onAwake() override {
        animator = entity()->getComponent<Animator>();
        data.raycast = [&](const Vector3F& ray_origin, const Vector3F& ray_direction, Vector3F* intersect,
                           Vector3F* normal) -> bool {
            return rayIntersectsMeshes(ray_origin, ray_direction, make_span(floor), intersect, normal);
        };
        data.legs.resize(2);
        // Finds left and right joints.
        setupLeg(animator->skeleton(), kLeftJointNames, &data.legs[kLeft]);
        setupLeg(animator->skeleton(), kRightJointNames, &data.legs[kRight]);
    }

    static bool setupLeg(const animation::Skeleton& _skeleton,
                         const char* _joint_names[3],
                         Animator::FloorIKData::LegSetup* _leg) {
        int found = 0;
        int joints[3] = {0};
        for (int i = 0; i < _skeleton.num_joints() && found != 3; i++) {
            const char* joint_name = _skeleton.joint_names()[i];
            if (std::strcmp(joint_name, _joint_names[found]) == 0) {
                joints[found] = i;
                ++found;
            }
        }
        _leg->hip = joints[0];
        _leg->knee = joints[1];
        _leg->ankle = joints[2];
        return found == 3;
    }

    explicit FloorTargetScript(Entity* entity) : Script(entity) {}

    void onUpdate(float deltaTime) override { animator->encodeFloorIK(data); }
};

class ControllerScript : public Script {
private:
    Entity* camera_{nullptr};
    Vector3F displacement_ = Vector3F();

public:
    explicit ControllerScript(Entity* entity) : Script(entity) {}

    void targetCamera(Entity* camera) { camera_ = camera; }

    void inputEvent(const vox::InputEvent& input_event) override {
        if (input_event.GetSource() == EventSource::KEYBOARD) {
            const auto& key_event = static_cast<const KeyInputEvent&>(input_event);

            Vector3F forward = camera_->transform->worldForward();
            forward.y = 0;
            forward.normalize();
            forward *= -1;
            Vector3F cross = Vector3F(forward.z, 0, -forward.x);

            float animationSpeed = 0.05;
            switch (key_event.GetCode()) {
                case KeyCode::W:
                    displacement_ = forward * animationSpeed;
                    break;
                case KeyCode::S:
                    displacement_ = -forward * animationSpeed;
                    break;
                case KeyCode::A:
                    displacement_ = cross * animationSpeed;
                    break;
                case KeyCode::D:
                    displacement_ = -cross * animationSpeed;
                    break;
                default:
                    break;
            }
        }
    }

    void onUpdate(float deltaTime) override {
        auto position = entity()->transform->worldPosition();
        position -= displacement_;
        entity()->transform->setWorldPosition(position);

        if (displacement_.x != 0 || displacement_.z != 0) {
            auto currentPosition = entity()->transform->worldPosition();
            auto predictPosition = currentPosition - displacement_;
            auto rotMat = makeLookAtMatrix(predictPosition, currentPosition, Vector3F(0, 1, 0));
            auto rotation = getRotation(rotMat).inverse();
            auto newRotation = slerp(entity()->transform->worldRotationQuaternion(), rotation, 0.3f);
            entity()->transform->setWorldRotationQuaternion(newRotation);
        }
        displacement_.set(0);
    }
};

}  // namespace

void AnimationIKFloorApp::loadScene() {
    auto scene = _sceneManager->currentScene();
    scene->ambientLight()->setDiffuseSolidColor(Color(1, 1, 1));
    auto rootEntity = scene->createRootEntity();

    auto ibl_map = ImageManager::GetSingleton().generateIBL("Textures/uffizi_rgba16f_cube.ktx");
    auto sh = ImageManager::GetSingleton().generateSH("Textures/uffizi_rgba16f_cube.ktx");
    scene->ambientLight()->setSpecularTexture(ibl_map);
    scene->ambientLight()->setSpecularIntensity(0.2);
    scene->ambientLight()->setDiffuseMode(DiffuseMode::SphericalHarmonics);
    scene->ambientLight()->setDiffuseSphericalHarmonics(sh);

    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(-1, 2, -10);
    _mainCamera = cameraEntity->addComponent<Camera>();
    auto control = cameraEntity->addComponent<control::OrbitControl>();
    control->target.set(-1.17f, 2.f, -2.06f);

    // init point light
    auto light = rootEntity->createChild("light");
    light->transform->setPosition(2.17f, 2.f, -2.06f);
    auto pointLight = light->addComponent<PointLight>();
    pointLight->intensity = 0.3;

    auto characterEntity = rootEntity->createChild();
    characterEntity->transform->setPosition(-1.17f, 2.f, -2.5f);
    auto animator = characterEntity->addComponent<Animator>();
    animator->loadSkeleton("Animation/pab_skeleton.ozz");
    auto animationClip = std::make_shared<AnimationClip>("Animation/pab_crossarms.ozz");
    animator->setRootState(animationClip);
    auto targetScript = characterEntity->addComponent<FloorTargetScript>();
    auto controlScript = characterEntity->addComponent<ControllerScript>();
    controlScript->targetCamera(cameraEntity);

    auto skins = MeshManager::GetSingleton().LoadSkinnedMesh("Animation/arnaud_mesh.ozz");
    for (auto& skin : skins) {
        auto renderer = characterEntity->addComponent<SkinnedMeshRenderer>();
        renderer->setSkinnedMesh(skin);
        auto material = std::make_shared<BlinnPhongMaterial>(_device);
        material->setBaseColor(Color(0.4, 0.6, 0.6, 0.6));
        material->setIsTransparent(true);
        renderer->setMaterial(material);
    }

    auto floorEntity = rootEntity->createChild();
    targetScript->floor = MeshManager::GetSingleton().LoadSkinnedMesh("Animation/floor.ozz");
    for (auto& skin : targetScript->floor) {
        auto renderer = floorEntity->addComponent<SkinnedMeshRenderer>();
        renderer->setSkinnedMesh(skin);
        auto material = std::make_shared<PBRMaterial>(_device);
        material->setRoughness(1);
        material->setMetallic(0);
        renderer->setMaterial(material);
    }

    characterEntity->addComponent<skeleton_view::SkeletonView>();

    auto attachEntity = characterEntity->createChild();
    auto attachRenderer = attachEntity->addComponent<MeshRenderer>();
    attachRenderer->setMesh(PrimitiveMesh::createCuboid(_device, 0.01, 0.01, 1));
    auto attachMtl = std::make_shared<BlinnPhongMaterial>(_device);
    attachMtl->setBaseColor(Color(1, 0, 0, 1));
    attachRenderer->setMaterial(attachMtl);
    animator->bindEntity("LeftHandMiddle1", attachEntity);

    scene->play();
}

}  // namespace vox
