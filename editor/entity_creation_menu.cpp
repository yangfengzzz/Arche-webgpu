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
std::function<void()> actorWithComponentCreationHandler(EntityPtr parent,
                                                        std::optional<std::function<void()>> onItemClicked) {
    return combine(std::bind(&EditorActions::createMonoComponentActor<T>, EditorActions::getSingletonPtr(),
                             true, parent), onItemClicked);
}

std::function<void()> actorWithModelComponentCreationHandler(EntityPtr parent, const std::string& modelName,
                                                             std::optional<std::function<void()>> onItemClicked) {
    return combine(std::bind(&EditorActions::createActorWithModel, EditorActions::getSingletonPtr(),
                             ":Models/" + modelName + ".fbx", true, parent, modelName), onItemClicked);
}


void ActorCreationMenu::generateActorCreationMenu(::vox::ui::MenuList& menuList, EntityPtr parent,
                                                  std::optional<std::function<void()>> onItemClicked) {
    menuList.createWidget<MenuItem>("Create Empty").clickedEvent
    += combine(std::bind(&::vox::editor::EditorActions::createEmptyActor,
                         EditorActions::getSingletonPtr(), true, parent, ""), onItemClicked);
    
    auto& primitives = menuList.createWidget<MenuList>("Primitives");
    auto& physicals = menuList.createWidget<MenuList>("Physicals");
    auto& lights = menuList.createWidget<MenuList>("Lights");
    auto& audio = menuList.createWidget<MenuList>("Audio");
    auto& others = menuList.createWidget<MenuList>("Others");
    
    primitives.createWidget<MenuItem>("Cube").clickedEvent     += actorWithModelComponentCreationHandler(parent, "Cube", onItemClicked);
    primitives.createWidget<MenuItem>("Sphere").clickedEvent   += actorWithModelComponentCreationHandler(parent, "Sphere", onItemClicked);
    primitives.createWidget<MenuItem>("Cone").clickedEvent     += actorWithModelComponentCreationHandler(parent, "Cone", onItemClicked);
    primitives.createWidget<MenuItem>("Cylinder").clickedEvent += actorWithModelComponentCreationHandler(parent, "Cylinder", onItemClicked);
    primitives.createWidget<MenuItem>("Plane").clickedEvent    += actorWithModelComponentCreationHandler(parent, "Plane", onItemClicked);
    primitives.createWidget<MenuItem>("Gear").clickedEvent     += actorWithModelComponentCreationHandler(parent, "Gear", onItemClicked);
    primitives.createWidget<MenuItem>("Helix").clickedEvent    += actorWithModelComponentCreationHandler(parent, "Helix", onItemClicked);
    primitives.createWidget<MenuItem>("Pipe").clickedEvent     += actorWithModelComponentCreationHandler(parent, "Pipe", onItemClicked);
    primitives.createWidget<MenuItem>("Pyramid").clickedEvent  += actorWithModelComponentCreationHandler(parent, "Pyramid", onItemClicked);
    primitives.createWidget<MenuItem>("Torus").clickedEvent    += actorWithModelComponentCreationHandler(parent, "Torus", onItemClicked);
    
    (void)physicals;
    (void)lights;
    (void)audio;
    (void)others;
}

}
}
