//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef editor_actions_inl_h
#define editor_actions_inl_h

#include "editor_actions.h"
#include "entity.h"

namespace vox {
namespace editor {
template<typename T>
inline Entity* EditorActions::createMonoComponentEntity(bool p_focusOnCreation, Entity* p_parent) {
    auto instance = createEmptyEntity(false, p_parent);

    T* component = instance->addComponent<T>();

    instance->name = component->name();

    if (p_focusOnCreation)
        selectEntity(instance);

    return instance;
}


}
}
#endif /* editor_actions_inl_h */
