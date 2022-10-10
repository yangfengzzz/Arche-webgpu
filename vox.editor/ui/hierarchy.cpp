//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.editor/ui/hierarchy.h"

#include "vox.editor/editor_actions.h"
#include "vox.editor/entity_creation_menu.h"
#include "vox.render/ui/plugins/contextual_menu.h"
#include "vox.render/ui/plugins/drag_drop_source.h"
#include "vox.render/ui/plugins/drag_drop_target.h"
#include "vox.render/ui/widgets/buttons/button_simple.h"
#include "vox.render/ui/widgets/input_fields/input_text.h"

namespace vox::editor::ui {
namespace {
class HierarchyContextualMenu : public ContextualMenu {
public:
    HierarchyContextualMenu(Entity *p_target, TreeNode &p_treeNode, bool p_panelMenu = false)
        : _target(p_target), _treeNode(p_treeNode) {
        if (_target) {
            auto &focusButton = CreateWidget<MenuItem>("Focus");
            focusButton.clicked_event_ += [this] { EditorActions::getSingleton().moveToTarget(_target); };

            auto &duplicateButton = CreateWidget<MenuItem>("Duplicate");
            duplicateButton.clicked_event_ += [this] {
                EditorActions::getSingleton().delayAction(
                        std::bind(&EditorActions::duplicateEntity, EditorActions::getSingletonPtr(), _target, nullptr,
                                  true),
                        0);
            };

            auto &deleteButton = CreateWidget<MenuItem>("Delete");
            deleteButton.clicked_event_ += [this] { EditorActions::getSingleton().destroyEntity(_target); };
        }

        auto &createEntity = CreateWidget<MenuList>("Create...");
        EntityCreationMenu::generateEntityCreationMenu(createEntity, _target, std::bind(&TreeNode::Open, &_treeNode));
    }

    void Execute() override {
        if (!widgets_.empty()) ContextualMenu::Execute();
    }

private:
    Entity *_target;
    TreeNode &_treeNode;
};

void expandTreeNode(TreeNode &p_toExpand, const TreeNode *p_root) {
    p_toExpand.Open();

    if (&p_toExpand != p_root && p_toExpand.HasParent()) {
        expandTreeNode(*static_cast<TreeNode *>(p_toExpand.Parent()), p_root);
    }
}

std::vector<TreeNode *> nodesToCollapse;
std::vector<TreeNode *> founds;

void expandTreeNodeAndEnable(TreeNode &p_toExpand, const TreeNode *p_root) {
    if (!p_toExpand.IsOpened()) {
        p_toExpand.Open();
        nodesToCollapse.push_back(&p_toExpand);
    }

    p_toExpand.enabled_ = true;

    if (&p_toExpand != p_root && p_toExpand.HasParent()) {
        expandTreeNodeAndEnable(*static_cast<TreeNode *>(p_toExpand.Parent()), p_root);
    }
}

}  // namespace

// MARK: - Hierarchy
Hierarchy::Hierarchy(const std::string &p_title, bool p_opened, const PanelWindowSettings &p_windowSettings)
    : PanelWindow(p_title, p_opened, p_windowSettings) {
    auto &searchBar = CreateWidget<InputText>();
    searchBar.content_changed_event_ += [this](const std::string &p_content) {
        founds.clear();
        auto content = p_content;
        std::transform(content.begin(), content.end(), content.begin(), ::tolower);

        for (auto &[entity, item] : _widgetEntityLink) {
            if (!p_content.empty()) {
                auto itemName = item->name_;
                std::transform(itemName.begin(), itemName.end(), itemName.begin(), ::tolower);

                if (itemName.find(content) != std::string::npos) {
                    founds.push_back(item);
                }

                item->enabled_ = false;
            } else {
                item->enabled_ = true;
            }
        }

        for (auto node : founds) {
            node->enabled_ = true;

            if (node->HasParent()) {
                expandTreeNodeAndEnable(*static_cast<TreeNode *>(node->Parent()), _sceneRoot);
            }
        }

        if (p_content.empty()) {
            for (auto node : nodesToCollapse) {
                node->Close();
            }

            nodesToCollapse.clear();
        }
    };

    _sceneRoot = &CreateWidget<TreeNode>("Root", true);
    static_cast<TreeNode *>(_sceneRoot)->Open();
    _sceneRoot->AddPlugin<DDTarget<std::pair<Entity *, TreeNode *>>>("Entity").data_received_event_ +=
            [this](std::pair<Entity *, TreeNode *> p_element) {
                if (p_element.second->HasParent()) p_element.second->Parent()->UnconsiderWidget(*p_element.second);

                _sceneRoot->ConsiderWidget(*p_element.second);

                auto parent = p_element.first->parent();
                if (parent) {
                    parent->removeChild(p_element.first);
                }
            };
    _sceneRoot->AddPlugin<HierarchyContextualMenu>(nullptr, *_sceneRoot);

    EditorActions::getSingleton().entityUnselectedEvent += std::bind(&Hierarchy::unselectEntitiesWidgets, this);
    //    EDITOR_CONTEXT(sceneManager).SceneUnloadEvent += std::bind(&Hierarchy::clear, this);
    Entity::createdEvent += std::bind(&Hierarchy::addEntityByInstance, this, std::placeholders::_1);
    Entity::destroyedEvent += std::bind(&Hierarchy::deleteEntityByInstance, this, std::placeholders::_1);
    EditorActions::getSingleton().entitySelectedEvent +=
            std::bind(&Hierarchy::selectEntityByInstance, this, std::placeholders::_1);
    Entity::attachEvent += std::bind(&Hierarchy::attachEntityToParent, this, std::placeholders::_1);
    Entity::dettachEvent += std::bind(&Hierarchy::detachFromParent, this, std::placeholders::_1);
}

void Hierarchy::clear() {
    EditorActions::getSingleton().unselectEntity();

    _sceneRoot->RemoveAllWidgets();
    _widgetEntityLink.clear();
}

void Hierarchy::unselectEntitiesWidgets() {
    for (auto &widget : _widgetEntityLink) widget.second->selected_ = false;
}

void Hierarchy::selectEntityByInstance(Entity *p_entity) {
    auto result =
            std::find_if(_widgetEntityLink.begin(), _widgetEntityLink.end(),
                         [p_entity](const std::pair<Entity *, TreeNode *> &link) { return link.first == p_entity; });

    if (result != _widgetEntityLink.end())
        if (result->second) selectEntityByWidget(*result->second);
}

void Hierarchy::selectEntityByWidget(TreeNode &p_widget) {
    unselectEntitiesWidgets();

    p_widget.selected_ = true;

    if (p_widget.HasParent()) {
        expandTreeNode(*static_cast<TreeNode *>(p_widget.Parent()), _sceneRoot);
    }
}

void Hierarchy::attachEntityToParent(Entity *p_entity) {
    auto entityWidget = _widgetEntityLink.find(p_entity);

    if (entityWidget != _widgetEntityLink.end()) {
        auto widget = entityWidget->second;

        if (widget->Parent()) widget->Parent()->UnconsiderWidget(*widget);

        if (p_entity->parent()) {
            auto parentWidget = std::find_if(
                    _widgetEntityLink.begin(), _widgetEntityLink.end(),
                    [&](std::pair<Entity *, TreeNode *> entity) { return entity.first == p_entity->parent(); });
            parentWidget->second->leaf_ = false;
            parentWidget->second->ConsiderWidget(*widget);
        }
    }
}

void Hierarchy::detachFromParent(Entity *p_entity) {
    if (auto entityWidget = _widgetEntityLink.find(p_entity); entityWidget != _widgetEntityLink.end()) {
        if (p_entity->parent() && p_entity->parent()->children().size() == 1) {
            auto parentWidget = std::find_if(
                    _widgetEntityLink.begin(), _widgetEntityLink.end(),
                    [&](std::pair<Entity *, TreeNode *> entity) { return entity.first == p_entity->parent(); });

            if (parentWidget != _widgetEntityLink.end()) {
                parentWidget->second->leaf_ = true;
            }
        }

        auto widget = entityWidget->second;

        if (widget->Parent()) widget->Parent()->UnconsiderWidget(*widget);

        _sceneRoot->ConsiderWidget(*widget);
    }
}

void Hierarchy::deleteEntityByInstance(Entity *p_entity) {
    if (auto result = _widgetEntityLink.find(p_entity); result != _widgetEntityLink.end()) {
        if (result->second) {
            result->second->Destroy();
        }

        _widgetEntityLink.erase(result);
    }
}

void Hierarchy::addEntityByInstance(Entity *p_entity) {
    auto &textSelectable = _sceneRoot->CreateWidget<TreeNode>(p_entity->name, true);
    textSelectable.leaf_ = true;
    textSelectable.AddPlugin<HierarchyContextualMenu>(p_entity, textSelectable);
    textSelectable.AddPlugin<DDSource<std::pair<Entity *, TreeNode *>>>("Entity", "Attach to...",
                                                                        std::make_pair(p_entity, &textSelectable));
    textSelectable.AddPlugin<DDTarget<std::pair<Entity *, TreeNode *>>>("Entity").data_received_event_ +=
            [p_entity, &textSelectable](std::pair<Entity *, TreeNode *> p_element) {
                if (p_element.second->Parent()) p_element.second->Parent()->UnconsiderWidget(*p_element.second);

                textSelectable.ConsiderWidget(*p_element.second);

                p_entity->addChild(p_element.first->parent()->removeChild(p_element.first));
            };
    auto &dispatcher = textSelectable.AddPlugin<DataDispatcher<std::string>>();

    Entity *targetPtr = p_entity;
    dispatcher.RegisterGatherer([targetPtr] { return targetPtr->name; });

    _widgetEntityLink[targetPtr] = &textSelectable;

    textSelectable.clicked_event_ +=
            std::bind(&EditorActions::selectEntity, EditorActions::getSingletonPtr(), p_entity);
    textSelectable.double_clicked_event_ +=
            std::bind(&EditorActions::moveToTarget, EditorActions::getSingletonPtr(), p_entity);
}

}  // namespace vox::editor::ui
