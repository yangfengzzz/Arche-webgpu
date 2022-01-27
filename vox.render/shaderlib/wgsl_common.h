//
//  wgsl_common.h
//  DigitalArche
//
//  Created by 杨丰 on 2022/1/27.
//

#ifndef wgsl_common_h
#define wgsl_common_h

typedef enum {
    Position = 0,
    Normal = 1,
    UV_0 = 2,
    Tangent = 3,
    Bitangent = 4,
    Color_0 = 5,
    Weights_0 = 6,
    Joints_0 = 7,
    UV_1 = 8,
    UV_2 = 9,
    UV_3 = 10,
    UV_4 = 11,
    UV_5 = 12,
    UV_6 = 13,
    UV_7 = 14,
} Attributes;

#endif /* wgsl_common_h */
