//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "animation_app.h"
#include "camera.h"
#include "mesh/mesh_renderer.h"
#include "mesh/skinned_mesh_renderer.h"
#include "mesh/primitive_mesh.h"
#include "animator.h"
#include "material/unlit_material.h"
#include "material/blinn_phong_material.h"
#include "material/pbr_material.h"
#include "controls/orbit_control.h"
#include "lighting/direct_light.h"
#include "lighting/point_light.h"
#include "lighting/spot_light.h"
#include "image/stb.h"

namespace vox {
void AnimationApp::loadScene() {
    auto scene = _sceneManager->currentScene();
    auto rootEntity = scene->createRootEntity();
    
    auto cameraEntity = rootEntity->createChild();
    cameraEntity->transform->setPosition(10, 10, 10);
    cameraEntity->transform->lookAt(Point3F(0, 0, 0));
    _mainCamera = cameraEntity->addComponent<Camera>();
    cameraEntity->addComponent<control::OrbitControl>();
    
    // init point light
    auto light = rootEntity->createChild("light");
    light->transform->setPosition(0, 10, 0);
    light->transform->lookAt(Point3F(0, 0, 0), Vector3F(1, 0, 0));
    auto spotLight = light->addComponent<SpotLight>();
    spotLight->intensity = 0.8;
    spotLight->setShadowIntensity(0.05);
    spotLight->setEnableShadow(true);
    
    auto light2 = rootEntity->createChild("light3");
    auto directLight = light2->addComponent<DirectLight>();
    directLight->intensity = 0.2;
    directLight->setShadowIntensity(0.2);
    directLight->setEnableShadow(true);
    
    auto characterMtl = std::make_shared<BlinnPhongMaterial>(_device);
    characterMtl->setBaseTexture(Image::load("Models/Doggy/T_Doggy_1_diffuse.png", true)->createSampledTexture(_device));
//    characterMtl->setBaseTexture(resourceLoader.loadTexture("../assets/models/Doggy", "T_Doggy_1_diffuse.png", false));
//    characterMtl->setOcclusionTexture(resourceLoader.loadTexture("../assets/models/Doggy", "T_Doggy_1_ao.png", false));
//    characterMtl->setNormalTexture(resourceLoader.loadTexture("../assets/models/Doggy", "T_Doggy_normal.png", false));
//    characterMtl->setMetallicRoughnessTexture(resourceLoader.createMetallicRoughnessTexture("../assets/models/Doggy", "T_Doggy_metallic.png",
//                                                                                            "T_Doggy_roughness.png", false));
    
    auto characterEntity = rootEntity->createChild("characterEntity");
    characterEntity->transform->setScale(3, 3, 3);
    auto characterRenderer = characterEntity->addComponent<SkinnedMeshRenderer>();
    characterRenderer->castShadow = true;
    characterRenderer->addSkinnedMesh("../assets/Models/Doggy/Doggy.fbx",
                                      "../assets/Models/Doggy/doggy_skeleton.ozz");
    characterRenderer->setMaterial(characterMtl);
    auto characterAnim = characterEntity->addComponent<Animator>();
    characterAnim->addAnimationClip("../assets/Models/Doggy/Run.ozz");
    
    auto planeEntity = rootEntity->createChild("PlaneEntity");
    auto planeMtl = std::make_shared<BlinnPhongMaterial>(_device);
    planeMtl->setBaseColor(Color(1.0, 0, 0, 1.0));
    planeMtl->setRenderFace(RenderFace::Double);
    
    auto planeRenderer = planeEntity->addComponent<MeshRenderer>();
    planeRenderer->setMesh(PrimitiveMesh::createPlane(_device, 10, 10));
    planeRenderer->setMaterial(planeMtl);
    planeRenderer->receiveShadow = true;
}

}
