#version 450

layout(set = 0, binding = 5) uniform sampler2D baseTexture;
layout(set = 0, binding = 7) uniform faceIndex {
    int value;
} face_index;

layout (location = 0) in vec2 v_uv;
layout(location = 0) out vec4 o_color;

vec4 RGBMToLinear( in vec4 value, in float maxRange ) {
    return vec4( value.rgb * value.a * maxRange, 1.0 );
}

void main(){
    vec2 uv = v_uv;
    if(face_index.value == 2){
        uv.x = v_uv.y;
        uv.y= 1.0 - v_uv.x;
    }else if(face_index.value == 3){
        uv.x = 1.0 - v_uv.y;
        uv.y=  v_uv.x;
    }

    o_color = texture(baseTexture, uv);
}