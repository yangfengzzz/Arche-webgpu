//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef plugin_h
#define plugin_h

namespace vox {
namespace ui {
/**
 * Interface to any plugin of UI.
 * A plugin is basically a behaviour that you can plug to a widget
 */
class Plugin {
public:
    virtual ~Plugin() {
    }
    
    /**
     * Execute the plugin behaviour
     */
    virtual void execute() = 0;
    
    /* Feel free to store any data you want here */
    void *userData = nullptr;
};

}
}
#endif /* plugin_h */
