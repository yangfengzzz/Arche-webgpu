//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "apps/animation_create_app.h"

#include "vox.animation/offline/animation_builder.h"
#include "vox.animation/offline/skeleton_builder.h"
#include "vox.render/animation/animation_states/animation_clip.h"
#include "vox.render/animation/animator.h"
#include "vox.render/camera.h"
#include "vox.render/entity.h"
#include "vox.render/material/unlit_material.h"
#include "vox.render/mesh/mesh_renderer.h"
#include "vox.render/mesh/primitive_mesh.h"
#include "vox.toolkit/controls/orbit_control.h"
#include "vox.toolkit/skeleton_view/skeleton_view.h"

namespace vox {
namespace {
// A millipede slice is 2 legs and a spine.
// Each slice is made of 7 joints, organized as follows.
//          * root
//             |
//           spine                                   spine
//         |       |                                   |
//     left_up    right_up        left_down - left_u - . - right_u - right_down
//       |           |                  |                                    |
//   left_down     right_down     left_foot         * root            right_foot
//     |               |
// left_foot        right_foot

// The following constants are used to define the millipede skeleton and
// animation.
// Skeleton constants.
const Vector3F kTransUp = Vector3F(0.f, 0.f, 0.f);
const Vector3F kTransDown = Vector3F(0.f, 0.f, 1.f);
const Vector3F kTransFoot = Vector3F(1.f, 0.f, 0.f);

const QuaternionF kRotLeftUp = QuaternionF(Vector3F::y_axis(), -kHalfPiF);
const QuaternionF kRotLeftDown = QuaternionF(Vector3F::x_axis(), kHalfPiF) * QuaternionF(Vector3F::y_axis(), -kHalfPiF);
const QuaternionF kRotRightUp = QuaternionF(Vector3F::y_axis(), kHalfPiF);
const QuaternionF kRotRightDown =
        QuaternionF(Vector3F::x_axis(), kHalfPiF) * QuaternionF(Vector3F::y_axis(), -kHalfPiF);

// Animation constants.
const float kDuration = 6.f;
const float kSpinLength = .5f;
const float kWalkCycleLength = 2.f;
const int kWalkCycleCount = 4;
const float kSpinLoop = 2 * kWalkCycleCount * kWalkCycleLength / kSpinLength;

const animation::offline::RawAnimation::TranslationKey kPrecomputedKeys[] = {
        {0.f * kDuration, Vector3F(.25f * kWalkCycleLength, 0.f, 0.f)},
        {.125f * kDuration, Vector3F(-.25f * kWalkCycleLength, 0.f, 0.f)},
        {.145f * kDuration, Vector3F(-.17f * kWalkCycleLength, .3f, 0.f)},
        {.23f * kDuration, Vector3F(.17f * kWalkCycleLength, .3f, 0.f)},
        {.25f * kDuration, Vector3F(.25f * kWalkCycleLength, 0.f, 0.f)},
        {.375f * kDuration, Vector3F(-.25f * kWalkCycleLength, 0.f, 0.f)},
        {.395f * kDuration, Vector3F(-.17f * kWalkCycleLength, .3f, 0.f)},
        {.48f * kDuration, Vector3F(.17f * kWalkCycleLength, .3f, 0.f)},
        {.5f * kDuration, Vector3F(.25f * kWalkCycleLength, 0.f, 0.f)},
        {.625f * kDuration, Vector3F(-.25f * kWalkCycleLength, 0.f, 0.f)},
        {.645f * kDuration, Vector3F(-.17f * kWalkCycleLength, .3f, 0.f)},
        {.73f * kDuration, Vector3F(.17f * kWalkCycleLength, .3f, 0.f)},
        {.75f * kDuration, Vector3F(.25f * kWalkCycleLength, 0.f, 0.f)},
        {.875f * kDuration, Vector3F(-.25f * kWalkCycleLength, 0.f, 0.f)},
        {.895f * kDuration, Vector3F(-.17f * kWalkCycleLength, .3f, 0.f)},
        {.98f * kDuration, Vector3F(.17f * kWalkCycleLength, .3f, 0.f)}};
const int kPrecomputedKeyCount = VOX_ARRAY_SIZE(kPrecomputedKeys);
}  // namespace

void AnimationCreateApp::loadScene() {
    auto scene = _sceneManager->currentScene();
    scene->ambientLight()->setDiffuseSolidColor(Color(1, 1, 1));
    auto rootEntity = scene->createRootEntity();

    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    cameraEntity->addComponent<control::OrbitControl>();

    // init point light
    auto light = rootEntity->createChild("light");
    light->transform->setPosition(0, 3, 0);
    auto pointLight = light->addComponent<PointLight>();
    pointLight->intensity = 0.3;

    auto characterEntity = rootEntity->createChild();
    auto animator = characterEntity->addComponent<Animator>();
    auto animationClip = std::make_shared<AnimationClip>("");
    animator->setRootState(animationClip);
    characterEntity->addComponent<skeleton_view::SkeletonView>();

    _build(animator, animationClip.get());

    auto planeEntity = rootEntity->createChild();
    auto planeRenderer = planeEntity->addComponent<MeshRenderer>();
    planeRenderer->setMesh(PrimitiveMesh::createPlane(_device, 10, 10));
    auto texturedMaterial = std::make_shared<UnlitMaterial>(_device);
    texturedMaterial->setRenderFace(RenderFace::Double);
    planeRenderer->setMaterial(texturedMaterial);

    texturedMaterial->setBaseTexture(ImageManager::GetSingleton().loadTexture("Textures/wood.png"));

    scene->play();
}

void AnimationCreateApp::_build(Animator* animator, AnimationClip* animationClip) {
    // Initializes the root. The root pointer will change from a spine to the
    // next for each slice.
    animation::offline::RawSkeleton raw_skeleton;
    _createSkeleton(&raw_skeleton);

    // Build the run time skeleton.
    animation::offline::SkeletonBuilder skeleton_builder;
    auto skeleton = skeleton_builder(raw_skeleton);

    // Build a walk animation.
    animation::offline::RawAnimation raw_animation;
    _createAnimation(*skeleton, &raw_animation);

    // Build the run time animation from the raw animation.
    animation::offline::AnimationBuilder animation_builder;
    animationClip->animation() = std::move(*animation_builder(raw_animation));
    animator->loadSkeleton(skeleton);

}

void AnimationCreateApp::_createSkeleton(animation::offline::RawSkeleton* _skeleton) const {
    _skeleton->roots.resize(1);
    animation::offline::RawSkeleton::Joint* root = &_skeleton->roots[0];
    root->name = "root";
    root->transform.translation = Vector3F(0.f, 1.f, float(-slice_count_) * kSpinLength);
    root->transform.rotation = QuaternionF::makeIdentity();
    root->transform.scale = Vector3F::one();

    char buf[16];
    for (int i = 0; i < slice_count_; ++i) {
        // Format joint number.
        std::sprintf(buf, "%d", i);

        root->children.resize(3);

        // Left leg.
        animation::offline::RawSkeleton::Joint& lu = root->children[0];
        lu.name = "lu";
        lu.name += buf;
        lu.transform.translation = kTransUp;
        lu.transform.rotation = kRotLeftUp;
        lu.transform.scale = Vector3F::one();

        lu.children.resize(1);
        animation::offline::RawSkeleton::Joint& ld = lu.children[0];
        ld.name = "ld";
        ld.name += buf;
        ld.transform.translation = kTransDown;
        ld.transform.rotation = kRotLeftDown;
        ld.transform.scale = Vector3F::one();

        ld.children.resize(1);
        animation::offline::RawSkeleton::Joint& lf = ld.children[0];
        lf.name = "lf";
        lf.name += buf;
        lf.transform.translation = Vector3F::x_axis();
        lf.transform.rotation = QuaternionF::makeIdentity();
        lf.transform.scale = Vector3F::one();

        // Right leg.
        animation::offline::RawSkeleton::Joint& ru = root->children[1];
        ru.name = "ru";
        ru.name += buf;
        ru.transform.translation = kTransUp;
        ru.transform.rotation = kRotRightUp;
        ru.transform.scale = Vector3F::one();

        ru.children.resize(1);
        animation::offline::RawSkeleton::Joint& rd = ru.children[0];
        rd.name = "rd";
        rd.name += buf;
        rd.transform.translation = kTransDown;
        rd.transform.rotation = kRotRightDown;
        rd.transform.scale = Vector3F::one();

        rd.children.resize(1);
        animation::offline::RawSkeleton::Joint& rf = rd.children[0];
        rf.name = "rf";
        rf.name += buf;
        rf.transform.translation = Vector3F::x_axis();
        rf.transform.rotation = QuaternionF::makeIdentity();
        rf.transform.scale = Vector3F::one();

        // Spine.
        animation::offline::RawSkeleton::Joint& sp = root->children[2];
        sp.name = "sp";
        sp.name += buf;
        sp.transform.translation = Vector3F(0.f, 0.f, kSpinLength);
        sp.transform.rotation = QuaternionF::makeIdentity();
        sp.transform.scale = Vector3F::one();

        root = &sp;
    }
}

void AnimationCreateApp::_createAnimation(animation::Skeleton& skeleton,
                                          animation::offline::RawAnimation* _animation) const {
    _animation->duration = kDuration;
    _animation->tracks.resize(skeleton.num_joints());

    for (int i = 0; i < _animation->num_tracks(); ++i) {
        animation::offline::RawAnimation::JointTrack& track = _animation->tracks[i];
        const char* joint_name = skeleton.joint_names()[i];

        if (strstr(joint_name, "ld") || strstr(joint_name, "rd")) {
            bool left = joint_name[0] == 'l';  // First letter of "ld".

            // Copy original keys while taking into consideration the spine number
            // as a phase.
            const int spine_number = std::atoi(joint_name + 2);
            const float offset = kDuration * float(slice_count_ - spine_number) / kSpinLoop;
            const float phase = std::fmod(offset, kDuration);

            // Loop to find animation start.
            int i_offset = 0;
            while (i_offset < kPrecomputedKeyCount && kPrecomputedKeys[i_offset].time < phase) {
                i_offset++;
            }

            // Push key with their corrected time.
            track.translations.reserve(kPrecomputedKeyCount);
            for (int j = i_offset; j < i_offset + kPrecomputedKeyCount; ++j) {
                const animation::offline::RawAnimation::TranslationKey& rkey =
                        kPrecomputedKeys[j % kPrecomputedKeyCount];
                float new_time = rkey.time - phase;
                if (new_time < 0.f) {
                    new_time = kDuration - phase + rkey.time;
                }

                if (left) {
                    const animation::offline::RawAnimation::TranslationKey tkey = {new_time, kTransDown + rkey.value};
                    track.translations.push_back(tkey);
                } else {
                    const animation::offline::RawAnimation::TranslationKey tkey = {
                            new_time, Vector3F(kTransDown.x - rkey.value.x, kTransDown.y + rkey.value.y,
                                               kTransDown.z + rkey.value.z)};
                    track.translations.push_back(tkey);
                }
            }

            // Pushes rotation key-frame.
            if (left) {
                const animation::offline::RawAnimation::RotationKey rkey = {0.f, kRotLeftDown};
                track.rotations.push_back(rkey);
            } else {
                const animation::offline::RawAnimation::RotationKey rkey = {0.f, kRotRightDown};
                track.rotations.push_back(rkey);
            }
        } else if (strstr(joint_name, "lu")) {
            const animation::offline::RawAnimation::TranslationKey tkey = {0.f, kTransUp};
            track.translations.push_back(tkey);

            const animation::offline::RawAnimation::RotationKey rkey = {0.f, kRotLeftUp};
            track.rotations.push_back(rkey);

        } else if (strstr(joint_name, "ru")) {
            const animation::offline::RawAnimation::TranslationKey tkey0 = {0.f, kTransUp};
            track.translations.push_back(tkey0);

            const animation::offline::RawAnimation::RotationKey rkey0 = {0.f, kRotRightUp};
            track.rotations.push_back(rkey0);
        } else if (strstr(joint_name, "lf")) {
            const animation::offline::RawAnimation::TranslationKey tkey = {0.f, kTransFoot};
            track.translations.push_back(tkey);
        } else if (strstr(joint_name, "rf")) {
            const animation::offline::RawAnimation::TranslationKey tkey0 = {0.f, kTransFoot};
            track.translations.push_back(tkey0);
        } else if (strstr(joint_name, "sp")) {
            const animation::offline::RawAnimation::TranslationKey skey = {0.f, Vector3F(0.f, 0.f, kSpinLength)};
            track.translations.push_back(skey);

            const animation::offline::RawAnimation::RotationKey rkey = {0.f, QuaternionF::makeIdentity()};
            track.rotations.push_back(rkey);
        } else if (strstr(joint_name, "root")) {
            const animation::offline::RawAnimation::TranslationKey tkey0 = {
                    0.f, Vector3F(0.f, 1.f, float(-slice_count_) * kSpinLength)};
            track.translations.push_back(tkey0);
            const animation::offline::RawAnimation::TranslationKey tkey1 = {
                    kDuration, Vector3F(0.f, 1.f, kWalkCycleCount * kWalkCycleLength + tkey0.value.z)};
            track.translations.push_back(tkey1);
        }

        // Make sure begin and end keys are looping.
        if (track.translations.front().time != 0.f) {
            const animation::offline::RawAnimation::TranslationKey& front = track.translations.front();
            const animation::offline::RawAnimation::TranslationKey& back = track.translations.back();
            const float lerp_time = front.time / (front.time + kDuration - back.time);
            const animation::offline::RawAnimation::TranslationKey tkey = {0.f,
                                                                           lerp(front.value, back.value, lerp_time)};
            track.translations.insert(track.translations.begin(), tkey);
        }
        if (track.translations.back().time != kDuration) {
            const animation::offline::RawAnimation::TranslationKey& front = track.translations.front();
            const animation::offline::RawAnimation::TranslationKey& back = track.translations.back();
            const float lerp_time = (kDuration - back.time) / (front.time + kDuration - back.time);
            const animation::offline::RawAnimation::TranslationKey tkey = {kDuration,
                                                                           lerp(back.value, front.value, lerp_time)};
            track.translations.push_back(tkey);
        }
    }
}

}  // namespace vox
