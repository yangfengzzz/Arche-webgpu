//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/event.h"
#include "vox.render/ui/widgets/data_widget.h"
#include "vox.render/ui/widgets/widget_container.h"

namespace vox::ui {
/**
 * Widget that allow columnification
 */
class TreeNode : public DataWidget<std::string>, public WidgetContainer {
public:
    explicit TreeNode(std::string name = "", bool arrow_click_to_open = false);

    /**
     * Open the tree node
     */
    void Open();

    /**
     * Close the tree node
     */
    void Close();

    /**
     * Returns true if the TreeNode is currently opened
     */
    [[nodiscard]] bool IsOpened() const;

protected:
    void DrawImpl() override;

public:
    std::string name_;
    bool selected_ = false;
    bool leaf_ = false;

    Event<> clicked_event_;
    Event<> double_clicked_event_;
    Event<> opened_event_;
    Event<> closed_event_;

private:
    bool arrow_click_to_open_ = false;
    bool should_open_ = false;
    bool should_close_ = false;
    bool opened_ = false;
};

}  // namespace vox::ui
