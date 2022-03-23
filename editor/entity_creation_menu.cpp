//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "entity_creation_menu.h"
#include "editor_actions.h"

namespace vox {
namespace editor {
std::function<void()> combine(std::function<void()> p_a, std::optional<std::function<void()>> p_b) {
    if (p_b.has_value()) {
        return [=]() {
            p_a();
            p_b.value()();
        };
    }
    
    return p_a;
}

template<class T>
std::function<void()> entityWithComponentCreationHandler(EntityPtr parent,
                                                        std::optional<std::function<void()>> onItemClicked) {
    return combine(std::bind(&EditorActions::createMonoComponentEntity<T>, EditorActions::getSingletonPtr(),
                             true, parent), onItemClicked);
}

std::function<void()> entityWithModelComponentCreationHandler(EntityPtr parent, const std::string& modelName,
                                                             std::optional<std::function<void()>> onItemClicked) {
    return combine(std::bind(&EditorActions::createEntityWithModel, EditorActions::getSingletonPtr(),
                             ":Models/" + modelName + ".fbx", true, parent, modelName), onItemClicked);
}


void EntityCreationMenu::generateEntityCreationMenu(::vox::ui::MenuList& menuList, EntityPtr parent,
                                                    std::optional<std::function<void()>> onItemClicked) {
    menuList.createWidget<MenuItem>("Create Empty").clickedEvent
    += combine(std::bind(&::vox::editor::EditorActions::createEmptyEntity,
                         EditorActions::getSingletonPtr(), true, parent, ""), onItemClicked);
    
    auto& primitives = menuList.createWidget<MenuList>("Primitives");
    auto& physicals = menuList.createWidget<MenuList>("Physicals");
    auto& lights = menuList.createWidget<MenuList>("Lights");
    auto& audio = menuList.createWidget<MenuList>("Audio");
    auto& others = menuList.createWidget<MenuList>("Others");
    
    primitives.createWidget<MenuItem>("Cube").clickedEvent     += entityWithModelComponentCreationHandler(parent, "Cube", onItemClicked);
    primitives.createWidget<MenuItem>("Sphere").clickedEvent   += entityWithModelComponentCreationHandler(parent, "Sphere", onItemClicked);
    primitives.createWidget<MenuItem>("Cone").clickedEvent     += entityWithModelComponentCreationHandler(parent, "Cone", onItemClicked);
    primitives.createWidget<MenuItem>("Cylinder").clickedEvent += entityWithModelComponentCreationHandler(parent, "Cylinder", onItemClicked);
    primitives.createWidget<MenuItem>("Plane").clickedEvent    += entityWithModelComponentCreationHandler(parent, "Plane", onItemClicked);
    primitives.createWidget<MenuItem>("Gear").clickedEvent     += entityWithModelComponentCreationHandler(parent, "Gear", onItemClicked);
    primitives.createWidget<MenuItem>("Helix").clickedEvent    += entityWithModelComponentCreationHandler(parent, "Helix", onItemClicked);
    primitives.createWidget<MenuItem>("Pipe").clickedEvent     += entityWithModelComponentCreationHandler(parent, "Pipe", onItemClicked);
    primitives.createWidget<MenuItem>("Pyramid").clickedEvent  += entityWithModelComponentCreationHandler(parent, "Pyramid", onItemClicked);
    primitives.createWidget<MenuItem>("Torus").clickedEvent    += entityWithModelComponentCreationHandler(parent, "Torus", onItemClicked);
    
    (void)physicals;
    (void)lights;
    (void)audio;
    (void)others;
}

}
}
