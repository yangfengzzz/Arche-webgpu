//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "unix_engine.h"
#include "engine.h"
#include "editor.h"

int main(int argc, char * argv[]) {
    vox::UnixEngine engine{vox::UnixType::Mac, argc, argv};
        
    auto code = engine.initialize();
    if (code == vox::ExitCode::Success) {
        engine.setApp(std::make_unique<vox::Editor>());
        code = engine.mainLoop();
    }
    
    engine.terminate(code);
    
    return EXIT_SUCCESS;
}
