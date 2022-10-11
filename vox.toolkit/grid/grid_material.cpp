//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.toolkit/grid/grid_material.h"

#include "vox.render/shader/shader_manager.h"

namespace vox::grid {
const std::string GridMaterial::_gridProperty = "u_grid";

GridMaterial::GridMaterial(wgpu::Device& device, const std::string& name) : BaseMaterial(device, name) {
    vertex_source_ = ShaderManager::GetSingleton().LoadShader("toolkit/grid/grid.vert");
    fragment_source_ = ShaderManager::GetSingleton().LoadShader("toolkit/grid/grid.frag");

    setIsTransparent(true);
    shaderData.setData(_gridProperty, _grid);
}

float GridMaterial::getNearClipPlane() const { return _grid.nearClip; }

void GridMaterial::setNearClipPlane(float value) {
    _grid.nearClip = value;
    shaderData.setData(_gridProperty, _grid);
}

float GridMaterial::getFarClipPlane() const { return _grid.farClip; }

void GridMaterial::setFarClipPlane(float value) {
    _grid.farClip = value;
    shaderData.setData(_gridProperty, _grid);
}

float GridMaterial::getPrimaryScale() const { return _grid.primaryScale; }

void GridMaterial::setPrimaryScale(float value) {
    _grid.primaryScale = value;
    shaderData.setData(_gridProperty, _grid);
}

float GridMaterial::getSecondaryScale() const { return _grid.secondaryScale; }

void GridMaterial::setSecondaryScale(float value) {
    _grid.secondaryScale = value;
    shaderData.setData(_gridProperty, _grid);
}

float GridMaterial::getGridIntensity() const { return _grid.gridIntensity; }

void GridMaterial::setGridIntensity(float value) {
    _grid.gridIntensity = value;
    shaderData.setData(_gridProperty, _grid);
}

float GridMaterial::getAxisIntensity() const { return _grid.axisIntensity; }

void GridMaterial::setAxisIntensity(float value) {
    _grid.axisIntensity = value;
    shaderData.setData(_gridProperty, _grid);
}

float GridMaterial::getFlipProgress() const { return _grid.flipProgress; }

void GridMaterial::setFlipProgress(float value) {
    _grid.flipProgress = value;
    shaderData.setData(_gridProperty, _grid);
}

}  // namespace vox::grid