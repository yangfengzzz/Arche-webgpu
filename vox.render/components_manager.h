//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef component_manager_hpp
#define component_manager_hpp

#include <unordered_map>
#include <vector>

#include "rendering/render_element.h"
#include "scene_forward.h"
#include "singleton.h"
#include "vox.math/bounding_frustum.h"
#include "vox.math/matrix4x4.h"
#include "vox.render/platform/input_events.h"

namespace vox {
/**
 * The manager of the components.
 */
class ComponentsManager : public Singleton<ComponentsManager> {
public:
    static ComponentsManager &getSingleton(void);

    static ComponentsManager *getSingletonPtr(void);

    void addOnStartScript(Script *script);

    void removeOnStartScript(Script *script);

    void addOnUpdateScript(Script *script);

    void removeOnUpdateScript(Script *script);

    void addDestroyComponent(Script *component);

    void callScriptOnStart();

    void callScriptOnUpdate(float deltaTime);

    void callScriptOnLateUpdate(float deltaTime);

    void callScriptInputEvent(const InputEvent &inputEvent);

    void callScriptResize(uint32_t win_width, uint32_t win_height, uint32_t fb_width, uint32_t fb_height);

    void callComponentDestroy();

public:
    void addRenderer(Renderer *renderer);

    void removeRenderer(Renderer *renderer);

    void callRendererOnUpdate(float deltaTime);

    void callRender(Camera *camera,
                    std::vector<RenderElement> &opaqueQueue,
                    std::vector<RenderElement> &alphaTestQueue,
                    std::vector<RenderElement> &transparentQueue);

    void callRender(const BoundingFrustum &frustrum,
                    std::vector<RenderElement> &opaqueQueue,
                    std::vector<RenderElement> &alphaTestQueue,
                    std::vector<RenderElement> &transparentQueue);

public:
    void addOnUpdateSceneAnimators(SceneAnimator *animator);

    void removeOnUpdateSceneAnimators(SceneAnimator *animator);

    void callSceneAnimatorUpdate(float deltaTime);

public:
    void callCameraOnBeginRender(Camera *camera);

    void callCameraOnEndRender(Camera *camera);

    std::vector<Component *> getActiveChangedTempList();

    void putActiveChangedTempList(std::vector<Component *> &componentContainer);

private:
    // Script
    std::vector<Script *> _onStartScripts;
    std::vector<Script *> _onUpdateScripts;
    std::vector<Script *> _destroyComponents;

    // Render
    std::vector<Renderer *> _renderers;

    // Delay dispose active/inActive Pool
    std::vector<std::vector<Component *>> _componentsContainerPool;

    // Animatior
    std::vector<SceneAnimator *> _onUpdateSceneAnimators;
};

template <>
inline ComponentsManager *Singleton<ComponentsManager>::ms_singleton{nullptr};

}  // namespace vox
#endif /* component_manager_hpp */
