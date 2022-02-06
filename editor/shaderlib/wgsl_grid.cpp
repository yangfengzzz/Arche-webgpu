//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "wgsl_grid.h"

namespace vox {
WGSLGridVertex::WGSLGridVertex() :
_commonVert("VertexIn") {
}

void WGSLGridVertex::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    _source.clear();
    _bindGroupInfo.clear();
    auto outputStructCounter = WGSLEncoder::startCounter(0);
    {
        auto encoder = createSourceEncoder(wgpu::ShaderStage::Vertex);
        encoder.addInoutType("VertexIn", Attributes::Position, UniformType::Vec3f32);

        encoder.addInoutType("VertexOut", BuiltInType::Position, "position", UniformType::Vec4f32);
        encoder.addInoutType("VertexOut", WGSLEncoder::getCounterNumber(outputStructCounter), "nearPoint", UniformType::Vec3f32);
        encoder.addInoutType("VertexOut", WGSLEncoder::getCounterNumber(outputStructCounter), "farPoint", UniformType::Vec3f32);
        encoder.addInoutType("VertexOut", WGSLEncoder::getCounterNumber(outputStructCounter), "fragView0", UniformType::Vec4f32);
        encoder.addInoutType("VertexOut", WGSLEncoder::getCounterNumber(outputStructCounter), "fragView1", UniformType::Vec4f32);
        encoder.addInoutType("VertexOut", WGSLEncoder::getCounterNumber(outputStructCounter), "fragView2", UniformType::Vec4f32);
        encoder.addInoutType("VertexOut", WGSLEncoder::getCounterNumber(outputStructCounter), "fragView3", UniformType::Vec4f32);
        encoder.addInoutType("VertexOut", WGSLEncoder::getCounterNumber(outputStructCounter), "fragProj0", UniformType::Vec4f32);
        encoder.addInoutType("VertexOut", WGSLEncoder::getCounterNumber(outputStructCounter), "fragProj1", UniformType::Vec4f32);
        encoder.addInoutType("VertexOut", WGSLEncoder::getCounterNumber(outputStructCounter), "fragProj2", UniformType::Vec4f32);
        encoder.addInoutType("VertexOut", WGSLEncoder::getCounterNumber(outputStructCounter), "fragProj3", UniformType::Vec4f32);
        
        std::string UnprojectPoint = "fn UnprojectPoint(x:f32, y:f32, z:f32, viewInv:mat4x4<f32>, projInv:mat4x4<f32>)->vec3<f32> {\n";
        UnprojectPoint += "    var unprojectedPoint =  viewInv * projInv * vec4<f32>(x, y, z, 1.0);\n";
        UnprojectPoint += "    return unprojectedPoint.xyz / unprojectedPoint.w;\n";
        UnprojectPoint += "}\n";
        encoder.addFunction(UnprojectPoint);
        
        _commonVert(encoder, macros);
        
        encoder.addEntry({{"in", "VertexIn"}}, {"out", "VertexOut"}, [&](std::string &source){
            source += "out.nearPoint = UnprojectPoint(in.position.x, in.position.y, 0.0, u_viewInvMat, u_projInvMat).xyz;\n"; // unprojecting on the near plane
            source += "out.farPoint = UnprojectPoint(in.position.x, in.position.y, 1.0, u_viewInvMat, u_projInvMat).xyz;\n"; // unprojecting on the far plane
            source += "out.fragView0 = u_viewMat.columns[0];\n";
            source += "out.fragView1 = u_viewMat.columns[1];\n";
            source += "out.fragView2 = u_viewMat.columns[2];\n";
            source += "out.fragView3 = u_viewMat.columns[3];\n";
            source += "out.fragProj0 = u_projMat.columns[0];\n";
            source += "out.fragProj1 = u_projMat.columns[1];\n";
            source += "out.fragProj2 = u_projMat.columns[2];\n";
            source += "out.fragProj3 = u_projMat.columns[3];\n";
            source += "out.position = float4(in.position, 1.0);\n";
        });
        encoder.flush();
    }
    WGSLEncoder::endCounter(outputStructCounter);
    _sourceCache[hash] = _source;
    _infoCache[hash] = _bindGroupInfo;
}

WGSLGridFragment::WGSLGridFragment() {
}

void WGSLGridFragment::_createShaderSource(size_t hash, const ShaderMacroCollection& macros) {
    _source.clear();
    _bindGroupInfo.clear();
    auto inputStructCounter = WGSLEncoder::startCounter(0);
    {
        auto encoder = createSourceEncoder(wgpu::ShaderStage::Fragment);

        encoder.addInoutType("VertexOut", WGSLEncoder::getCounterNumber(inputStructCounter), "nearPoint", UniformType::Vec3f32);
        encoder.addInoutType("VertexOut", WGSLEncoder::getCounterNumber(inputStructCounter), "farPoint", UniformType::Vec3f32);
        encoder.addInoutType("VertexOut", WGSLEncoder::getCounterNumber(inputStructCounter), "fragView0", UniformType::Vec4f32);
        encoder.addInoutType("VertexOut", WGSLEncoder::getCounterNumber(inputStructCounter), "fragView1", UniformType::Vec4f32);
        encoder.addInoutType("VertexOut", WGSLEncoder::getCounterNumber(inputStructCounter), "fragView2", UniformType::Vec4f32);
        encoder.addInoutType("VertexOut", WGSLEncoder::getCounterNumber(inputStructCounter), "fragView3", UniformType::Vec4f32);
        encoder.addInoutType("VertexOut", WGSLEncoder::getCounterNumber(inputStructCounter), "fragProj0", UniformType::Vec4f32);
        encoder.addInoutType("VertexOut", WGSLEncoder::getCounterNumber(inputStructCounter), "fragProj1", UniformType::Vec4f32);
        encoder.addInoutType("VertexOut", WGSLEncoder::getCounterNumber(inputStructCounter), "fragProj2", UniformType::Vec4f32);
        encoder.addInoutType("VertexOut", WGSLEncoder::getCounterNumber(inputStructCounter), "fragProj3", UniformType::Vec4f32);
        
        encoder.addInoutType("Output", 0, "finalColor", UniformType::Vec4f32);
        encoder.addInoutType("Output", BuiltInType::FragDepth, "depth", UniformType::F32);

        std::string grid = "fn grid(fragPos3D:vec3<f32>, scale:f32, drawAxis:bool)->vec4<f32> {\n";
        grid += "   var coord = fragPos3D.xz * scale;\n"; // use the scale variable to set the distance between the lines
        grid += "    var derivative = fwidth(coord);\n";
        grid += "    var grid = abs(fract(coord - 0.5) - 0.5) / derivative;\n";
        grid += "    var line = min(grid.x, grid.y);\n";
        grid += "    var minimumz = min(derivative.y, 1.0);\n";
        grid += "    var minimumx = min(derivative.x, 1.0);\n";
        grid += "    var color = vec4<f32>(0.6, 0.6, 0.6, 1.0 - min(line, 1.0));\n";
            // z axis
        grid += "    if(fragPos3D.x > -1 * minimumx && fragPos3D.x < 1 * minimumx) {\n";
        grid += "        color = vec4<f32>(0.0, 0.0, 1.0, 1.0);\n";
        grid += "    }\n";
            // x axis
        grid += "    if(fragPos3D.z > -1 * minimumz && fragPos3D.z < 1 * minimumz) {\n";
        grid += "        color = vec4<f32>(1.0, 0.0, 0.0, 1.0);\n";
        grid += "    }\n";
        grid += "\n";
        grid += "    return color;\n";
        grid += "}\n";
        encoder.addFunction(grid);
        
        std::string computeDepth = "fn computeDepth(pos: vec3<f32>, fragView: mat4x4<f32>, fragProj: mat4x4<f32>)->f32 {\n";
        computeDepth += "    var clip_space_pos = fragProj * fragView * vec4<f32>(pos.xyz, 1.0);\n";
        computeDepth += "    return (clip_space_pos.z / clip_space_pos.w);\n";
        computeDepth += "}\n";
        encoder.addFunction(computeDepth);
        
        std::string computeLinearDepth = "fn computeLinearDepth(pos: vec3<f32>, fragView: mat4x4<f32>, fragProj: mat4x4<f32>)->f32 {\n";
        computeLinearDepth += "    var near: f32 = 0.01;\n";
        computeLinearDepth += "    var far: f32 = 100.0;\n";
        computeLinearDepth += "\n";
        computeLinearDepth += "    var clip_space_pos = fragProj * fragView * vec4<f32>(pos.xyz, 1.0);\n";
        computeLinearDepth += "    var clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0;\n"; // put back between -1 and 1
        computeLinearDepth += "    var linearDepth = (2.0 * near * far) / (far + near - clip_space_depth * (far - near));\n"; // get linear value between 0.01 and 100
        computeLinearDepth += "    return linearDepth / far;\n"; // normalize
        computeLinearDepth += "}\n";
        
        encoder.addEntry({{"in", "VertexOut"}}, {"out", "Output"},  [&](std::string &source){
            source += "var t = -in.nearPoint.y / (in.farPoint.y - in.nearPoint.y);\n";
            source += "var fragPos3D: vec3<f32> = in.nearPoint + t * (in.farPoint - in.nearPoint);\n";
            source += "\n";
            source += "var fragView: mat4x4<f32> = mat4x4<f32>(in.fragView0, in.fragView1, in.fragView2, in.fragView3);\n";
            source += "var fragProj: mat4x4<f32> = mat4x4<f32>(in.fragProj0, in.fragProj1, in.fragProj2, in.fragProj3);\n";
            source += "var depth = computeDepth(fragPos3D, fragView, fragProj);\n";
            source += "\n";
            source += "var linearDepth = computeLinearDepth(fragPos3D, fragView, fragProj);\n";
            source += "var fading = max(0.0, (0.5 - linearDepth));\n";
            
            source += "out.finalColor = (grid(fragPos3D, 1, true)) * float(t > 0);\n";
            source += "out.finalColor.a = out.finalColor.a * fading;\n";
            source += "out.depth = depth;\n";
        });
        encoder.flush();
    }
    WGSLEncoder::endCounter(inputStructCounter);
    _sourceCache[hash] = _source;
    _infoCache[hash] = _bindGroupInfo;
    
}

}
