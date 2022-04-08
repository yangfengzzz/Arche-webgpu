//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "hierarchy.h"

#include "ui/widgets/buttons/button_simple.h"
#include "ui/widgets/selection/check_box.h"
#include "ui/widgets/visual/separator.h"
#include "ui/widgets/input_fields/input_text.h"
#include "ui/plugins/drag_drop_target.h"
#include "ui/plugins/drag_drop_source.h"
#include "ui/plugins/contextual_menu.h"
#include "editor_actions.h"
#include "entity_creation_menu.h"

namespace vox {
namespace editor {
namespace ui {
namespace {
class HierarchyContextualMenu : public ContextualMenu {
public:
    HierarchyContextualMenu(Entity* p_target, TreeNode &p_treeNode, bool p_panelMenu = false) :
    _target(p_target),
    _treeNode(p_treeNode) {
        if (_target) {
            auto &focusButton = createWidget<MenuItem>("Focus");
            focusButton.clickedEvent += [this] {
                EditorActions::getSingleton().moveToTarget(_target);
            };
            
            auto &duplicateButton = createWidget<MenuItem>("Duplicate");
            duplicateButton.clickedEvent += [this] {
                EditorActions::getSingleton().delayAction(std::bind(&EditorActions::duplicateEntity,
                                                                    EditorActions::getSingletonPtr(), _target, nullptr, true), 0);
            };
            
            auto &deleteButton = createWidget<MenuItem>("Delete");
            deleteButton.clickedEvent += [this] {
                EditorActions::getSingleton().destroyEntity(_target);
            };
        }
        
        auto &createEntity = createWidget<MenuList>("Create...");
        EntityCreationMenu::generateEntityCreationMenu(createEntity, _target, std::bind(&TreeNode::open, &_treeNode));
    }
    
    virtual void execute() override {
        if (_widgets.size() > 0)
            ContextualMenu::execute();
    }
    
private:
    Entity* _target;
    TreeNode &_treeNode;
};

void expandTreeNode(TreeNode &p_toExpand, const TreeNode *p_root) {
    p_toExpand.open();
    
    if (&p_toExpand != p_root && p_toExpand.hasParent()) {
        expandTreeNode(*static_cast<TreeNode *>(p_toExpand.parent()), p_root);
    }
}

std::vector<TreeNode *> nodesToCollapse;
std::vector<TreeNode *> founds;

void expandTreeNodeAndEnable(TreeNode &p_toExpand, const TreeNode *p_root) {
    if (!p_toExpand.isOpened()) {
        p_toExpand.open();
        nodesToCollapse.push_back(&p_toExpand);
    }
    
    p_toExpand.enabled = true;
    
    if (&p_toExpand != p_root && p_toExpand.hasParent()) {
        expandTreeNodeAndEnable(*static_cast<TreeNode *>(p_toExpand.parent()), p_root);
    }
}

} // namespace

//MARK: - Hierarchy
Hierarchy::Hierarchy(const std::string &p_title,
                     bool p_opened,
                     const PanelWindowSettings &p_windowSettings) :
PanelWindow(p_title, p_opened, p_windowSettings) {
    auto &searchBar = createWidget<InputText>();
    searchBar.contentChangedEvent += [this](const std::string &p_content) {
        founds.clear();
        auto content = p_content;
        std::transform(content.begin(), content.end(), content.begin(), ::tolower);
        
        for (auto&[entity, item]: _widgetEntityLink) {
            if (!p_content.empty()) {
                auto itemName = item->name;
                std::transform(itemName.begin(), itemName.end(), itemName.begin(), ::tolower);
                
                if (itemName.find(content) != std::string::npos) {
                    founds.push_back(item);
                }
                
                item->enabled = false;
            } else {
                item->enabled = true;
            }
        }
        
        for (auto node: founds) {
            node->enabled = true;
            
            if (node->hasParent()) {
                expandTreeNodeAndEnable(*static_cast<TreeNode *>(node->parent()), _sceneRoot);
            }
        }
        
        if (p_content.empty()) {
            for (auto node: nodesToCollapse) {
                node->close();
            }
            
            nodesToCollapse.clear();
        }
    };
    
    _sceneRoot = &createWidget<TreeNode>("Root", true);
    static_cast<TreeNode *>(_sceneRoot)->open();
    _sceneRoot->addPlugin<DDTarget<std::pair<Entity*, TreeNode *>>>("Entity").dataReceivedEvent +=
    [this](std::pair<Entity*, TreeNode *> p_element) {
        if (p_element.second->hasParent())
            p_element.second->parent()->unconsiderWidget(*p_element.second);
        
        _sceneRoot->considerWidget(*p_element.second);
        
        auto parent = p_element.first->parent();
        if (parent) {
            parent->removeChild(p_element.first);
        }
    };
    _sceneRoot->addPlugin<HierarchyContextualMenu>(nullptr, *_sceneRoot);
    
    EditorActions::getSingleton().entityUnselectedEvent += std::bind(&Hierarchy::unselectEntitiesWidgets, this);
    //    EDITOR_CONTEXT(sceneManager).SceneUnloadEvent += std::bind(&Hierarchy::clear, this);
    Entity::createdEvent += std::bind(&Hierarchy::addEntityByInstance, this, std::placeholders::_1);
    Entity::destroyedEvent += std::bind(&Hierarchy::deleteEntityByInstance, this, std::placeholders::_1);
    EditorActions::getSingleton().entitySelectedEvent += std::bind(&Hierarchy::selectEntityByInstance, this, std::placeholders::_1);
    Entity::attachEvent += std::bind(&Hierarchy::attachEntityToParent, this, std::placeholders::_1);
    Entity::dettachEvent += std::bind(&Hierarchy::detachFromParent, this, std::placeholders::_1);
}

void Hierarchy::clear() {
    EditorActions::getSingleton().unselectEntity();
    
    _sceneRoot->removeAllWidgets();
    _widgetEntityLink.clear();
}

void Hierarchy::unselectEntitiesWidgets() {
    for (auto &widget: _widgetEntityLink)
        widget.second->selected = false;
}

void Hierarchy::selectEntityByInstance(Entity* p_entity) {
    auto result = std::find_if(_widgetEntityLink.begin(), _widgetEntityLink.end(),
                               [p_entity](const std::pair<Entity*, TreeNode *>& link){
        return link.first == p_entity;
    });
    
    if (result != _widgetEntityLink.end())
        if (result->second)
            selectEntityByWidget(*result->second);
}

void Hierarchy::selectEntityByWidget(TreeNode &p_widget) {
    unselectEntitiesWidgets();
    
    p_widget.selected = true;
    
    if (p_widget.hasParent()) {
        expandTreeNode(*static_cast<TreeNode *>(p_widget.parent()), _sceneRoot);
    }
}

void Hierarchy::attachEntityToParent(Entity* p_entity) {
    auto entityWidget = _widgetEntityLink.find(p_entity);
    
    if (entityWidget != _widgetEntityLink.end()) {
        auto widget = entityWidget->second;
        
        if (widget->parent())
            widget->parent()->unconsiderWidget(*widget);
        
        if (p_entity->parent()) {
            auto parentWidget = std::find_if(_widgetEntityLink.begin(), _widgetEntityLink.end(), [&](std::pair<Entity*, TreeNode *> entity) {
                return entity.first == p_entity->parent();
            });
            parentWidget->second->leaf = false;
            parentWidget->second->considerWidget(*widget);
        }
    }
}

void Hierarchy::detachFromParent(Entity* p_entity) {
    if (auto entityWidget = _widgetEntityLink.find(p_entity); entityWidget != _widgetEntityLink.end()) {
        if (p_entity->parent() && p_entity->parent()->children().size() == 1) {
            auto parentWidget = std::find_if(_widgetEntityLink.begin(), _widgetEntityLink.end(), [&](std::pair<Entity*, TreeNode *> entity) {
                return entity.first == p_entity->parent();
            });
            
            if (parentWidget != _widgetEntityLink.end()) {
                parentWidget->second->leaf = true;
            }
        }
        
        auto widget = entityWidget->second;
        
        if (widget->parent())
            widget->parent()->unconsiderWidget(*widget);
        
        _sceneRoot->considerWidget(*widget);
    }
}

void Hierarchy::deleteEntityByInstance(Entity* p_entity) {
    if (auto result = _widgetEntityLink.find(p_entity); result != _widgetEntityLink.end()) {
        if (result->second) {
            result->second->destroy();
        }
        
        _widgetEntityLink.erase(result);
    }
}

void Hierarchy::addEntityByInstance(Entity* p_entity) {
    auto &textSelectable = _sceneRoot->createWidget<TreeNode>(p_entity->name, true);
    textSelectable.leaf = true;
    textSelectable.addPlugin<HierarchyContextualMenu>(p_entity, textSelectable);
    textSelectable.addPlugin<DDSource<std::pair<Entity*, TreeNode *>>>("Entity", "Attach to...", std::make_pair(p_entity, &textSelectable));
    textSelectable.addPlugin<DDTarget<std::pair<Entity*, TreeNode *>>>("Entity").dataReceivedEvent +=
    [p_entity, &textSelectable](std::pair<Entity*, TreeNode *> p_element) {
        if (p_element.second->parent())
            p_element.second->parent()->unconsiderWidget(*p_element.second);
        
        textSelectable.considerWidget(*p_element.second);
        
        p_entity->addChild(p_element.first);
    };
    auto &dispatcher = textSelectable.addPlugin<DataDispatcher<std::string>>();
    
    Entity* targetPtr = p_entity;
    dispatcher.registerGatherer([targetPtr] {
        return targetPtr->name;
    });
    
    _widgetEntityLink[targetPtr] = &textSelectable;
    
    textSelectable.clickedEvent += std::bind(&EditorActions::selectEntity, EditorActions::getSingletonPtr(), p_entity);
    textSelectable.doubleClickedEvent += std::bind(&EditorActions::moveToTarget, EditorActions::getSingletonPtr(), p_entity);
}


}
}
}
