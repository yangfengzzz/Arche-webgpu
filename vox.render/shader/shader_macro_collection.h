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
    
private:
    friend class ShaderProgram;
    
    friend class ShaderData;
    
    std::map<std::string, double> _value{};
};

}

#endif /* shader_macro_collection_hpp */
