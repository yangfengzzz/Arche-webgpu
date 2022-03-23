//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef editor_settings_h
#define editor_settings_h

#include "event.h"

namespace vox {
namespace editor {
/**
 * Accessible from anywhere editor settings
 */
class EditorSettings {
public:
    template<typename T>
    class Property {
    public:
        /**
         * Creates the property with a default value
         */
        Property(T p_value) : _value(p_value) {}
        
        /**
         * Event called when the property value changes
         */
        Event<T> onValueChanged;
        
        /**
         * Assign a new value to the property
         */
        inline T& operator=(T p_value) {
            set(p_value);
            return _value;
        }
        
        /**
         * Assign a new valeu to the property
         */
        inline void set(T p_value) {
            _value = p_value;
            onValueChanged.invoke(_value);
        }
        
        inline operator T() {
            return _value;
        }
        
        /**
         * Returns the value of the property
         */
        inline T Get() const {
            return _value;
        }
        
    private:
        T _value;
    };
    
    /**
     * No construction possible
     */
    EditorSettings() = delete;
    
    inline static Property<bool> ShowGeometryBounds = { false };
    inline static Property<bool> ShowLightBounds = { false };
    inline static Property<bool> ShowGeometryFrustumCullingInSceneView = { false };
    inline static Property<bool> ShowLightFrustumCullingInSceneView = { false };
    inline static Property<float> LightBillboardScale = { 0.5f };
    inline static Property<float> TranslationSnapUnit = { 1.0f };
    inline static Property<float> RotationSnapUnit = { 15.0f };
    inline static Property<float> ScalingSnapUnit = { 1.0f };
};


}
}

#endif /* editor_settings_h */
