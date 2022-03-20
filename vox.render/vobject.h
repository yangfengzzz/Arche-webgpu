//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef vobject_hpp
#define vobject_hpp

#include <tinyxml2.h>

namespace vox {
class VObject {
public:
    virtual ~VObject() {}
    
    /**
     * Called when the serialization is asked
     */
    virtual void onSerialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node) = 0;
    
    /**
     * Called when the deserialization is asked
     */
    virtual void onDeserialize(tinyxml2::XMLDocument& p_doc, tinyxml2::XMLNode* p_node) = 0;
};

}

#endif /* vobject_hpp */
