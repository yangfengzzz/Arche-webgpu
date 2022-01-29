//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef shader_macro_collection_hpp
#define shader_macro_collection_hpp

#include <vector>
#include <string>
#include <map>

namespace vox {
/**
 * Shader macro collection.
 */
struct ShaderMacroCollection {    
    /**
     * Union of two macro collection.
     * @param left - input macro collection
     * @param right - input macro collection
     * @param result - union output macro collection
     */
    static void unionCollection(const ShaderMacroCollection &left, const ShaderMacroCollection &right,
                                ShaderMacroCollection &result);
    
    size_t hash() const;
    
    bool contains(const std::string& macro) const;
    
    std::optional<double> macroConstant(const std::string& macro) const;
    
public:
    /**
     * Enable macro.
     * @param macroName - Shader macro
     */
    void enableMacro(const std::string& macroName);

    /**
     * Enable macro.
     * @remarks Name and value will combine one macro, it's equal the macro of "name value".
     * @param macroName - Macro name
     * @param value - Macro value
     */
    void enableMacro(const std::string& macroName, double value);

    /**
     * Disable macro
     * @param macroName - Macro name
     */
    void disableMacro(const std::string& macroName);
    
private:    
    std::map<std::string, double> _value{};
};

}

#endif /* shader_macro_collection_hpp */
