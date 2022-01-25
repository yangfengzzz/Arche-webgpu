//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef background_hpp
#define background_hpp

#include "color.h"

namespace vox {
/**
 * The Background mode enumeration.
 */
struct BackgroundMode {
    enum Enum {
        /* Solid color. */
        SolidColor,
        /** Texture */
        Texture
    };
};

/**
 * Background of scene.
 */
class Background {
public:
    /**
     * Background mode.
     * @defaultValue `BackgroundMode.SolidColor`
     * @remarks If using `BackgroundMode.Sky` mode and material or mesh of the `sky` is not defined, it will downgrade to `BackgroundMode.SolidColor`.
     */
    BackgroundMode::Enum mode = BackgroundMode::Enum::SolidColor;
    
    /**
     * Background solid color.
     * @defaultValue `new Color(0.25, 0.25, 0.25, 1.0)`
     * @remarks When `mode` is `BackgroundMode.SolidColor`, the property will take effects.
     */
    Color solidColor = Color(0.25, 0.25, 0.25, 1.0);
    
    Background() = default;
};

}

#endif /* background_hpp */
