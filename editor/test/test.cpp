//
//  test.cpp
//  editor
//
//  Created by 杨丰 on 2022/3/16.
//

#include <rttr/registration>
#include "vector3.h"

using namespace vox;
using namespace rttr;

RTTR_REGISTRATION {
    rttr::registration::class_<Vector3F>("vec")
         .constructor<>()
         .property("data", &Vector3F::x)
         .method("set", &Vector3F::normalize)
         .method("add", select_overload<Vector3F(float v) const>(&Vector3F::add));

}
