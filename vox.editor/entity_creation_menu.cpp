//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "entity_creation_menu.h"

#include <utility>

#include "editor_actions.h"

namespace vox::editor {
std::function<void()> combine(std::function<void()> p_a, const std::optional<std::function<void()>>& p_b) {
    if (p_b.has_value()) {
        return [=]() {
            p_a();
            p_b.value()();
        };
    }

    return p_a;
}

template <class T>
std::function<void()> entityWithComponentCreationHandler(Entity* parent,
                                                         std::optional<std::function<void()>> onItemClicked) {
    return combine(
            std::bind(&EditorActions::createMonoComponentEntity<T>, EditorActions::getSingletonPtr(), true, parent),
            onItemClicked);
}

std::function<void()> entityWithModelComponentCreationHandler(
        Entity* parent, const std::string& modelName, const std::optional<std::function<void()>>& onItemClicked) {
    return combine(std::bind(&EditorActions::createEntityWithModel, EditorActions::getSingletonPtr(),
                             ":Models/" + modelName + ".fbx", true, parent, modelName),
                   onItemClicked);
}

void EntityCreationMenu::generateEntityCreationMenu(::vox::ui::MenuList& menuList,
                                                    Entity* parent,
                                                    const std::optional<std::function<void()>>& onItemClicked) {
    menuList.CreateWidget<MenuItem>("Create Empty").clicked_event_ +=
            combine(std::bind(&::vox::editor::EditorActions::createEmptyEntity, EditorActions::getSingletonPtr(), true,
                              parent, ""),
                    onItemClicked);

    auto& primitives = menuList.CreateWidget<MenuList>("Primitives");
    auto& physicals = menuList.CreateWidget<MenuList>("Physicals");
    auto& lights = menuList.CreateWidget<MenuList>("Lights");
    auto& audio = menuList.CreateWidget<MenuList>("Audio");
    auto& others = menuList.CreateWidget<MenuList>("Others");

    primitives.CreateWidget<MenuItem>("Cube").clicked_event_ +=
            entityWithModelComponentCreationHandler(parent, "Cube", onItemClicked);
    primitives.CreateWidget<MenuItem>("Sphere").clicked_event_ +=
            entityWithModelComponentCreationHandler(parent, "Sphere", onItemClicked);
    primitives.CreateWidget<MenuItem>("Cone").clicked_event_ +=
            entityWithModelComponentCreationHandler(parent, "Cone", onItemClicked);
    primitives.CreateWidget<MenuItem>("Cylinder").clicked_event_ +=
            entityWithModelComponentCreationHandler(parent, "Cylinder", onItemClicked);
    primitives.CreateWidget<MenuItem>("Plane").clicked_event_ +=
            entityWithModelComponentCreationHandler(parent, "Plane", onItemClicked);
    primitives.CreateWidget<MenuItem>("Gear").clicked_event_ +=
            entityWithModelComponentCreationHandler(parent, "Gear", onItemClicked);
    primitives.CreateWidget<MenuItem>("Helix").clicked_event_ +=
            entityWithModelComponentCreationHandler(parent, "Helix", onItemClicked);
    primitives.CreateWidget<MenuItem>("Pipe").clicked_event_ +=
            entityWithModelComponentCreationHandler(parent, "Pipe", onItemClicked);
    primitives.CreateWidget<MenuItem>("Pyramid").clicked_event_ +=
            entityWithModelComponentCreationHandler(parent, "Pyramid", onItemClicked);
    primitives.CreateWidget<MenuItem>("Torus").clicked_event_ +=
            entityWithModelComponentCreationHandler(parent, "Torus", onItemClicked);

    (void)physicals;
    (void)lights;
    (void)audio;
    (void)others;
}

}  // namespace vox::editor
