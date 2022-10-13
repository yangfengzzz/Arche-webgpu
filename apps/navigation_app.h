//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.navigation/input_geom.h"
#include "vox.navigation/sample.h"
#include "vox.navigation/test_case.h"
#include "vox.render/forward_application.h"

namespace vox {
class NavigationApp : public ForwardApplication {
public:
    void loadScene() override;

    void inputEvent(const vox::InputEvent& inputEvent) override;

private:
    nav::InputGeom* geom{nullptr};
    nav::Sample* sample{nullptr};
    bool done = false;
    bool showMenu = true;
    bool showLog = false;
    bool showTools = true;
    bool showLevels = false;
    bool showSample = false;
    bool showTestCases = false;

    float rayStart[3];
    float rayEnd[3];
    bool mouseOverMenu = false;
    const std::string testCasesFolder = "TestCases";
    nav::TestCase* test{nullptr};

    std::string sampleName = "Choose Sample...";

    std::vector<std::string> files;
    const std::string meshesFolder = "Meshes";
    std::string meshName = "Choose Mesh...";
};
}  // namespace vox