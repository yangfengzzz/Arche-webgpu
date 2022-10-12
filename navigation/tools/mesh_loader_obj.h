//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <string>

namespace vox {

class rcMeshLoaderObj {
public:
    rcMeshLoaderObj();
    ~rcMeshLoaderObj();

    bool load(const std::string& fileName);

    [[nodiscard]] const float* getVerts() const { return m_verts; }
    [[nodiscard]] const float* getNormals() const { return m_normals; }
    [[nodiscard]] const int* getTris() const { return m_tris; }
    [[nodiscard]] int getVertCount() const { return m_vertCount; }
    [[nodiscard]] int getTriCount() const { return m_triCount; }
    [[nodiscard]] const std::string& getFileName() const { return m_filename; }

private:
    // Explicitly disabled copy constructor and copy assignment operator.
    rcMeshLoaderObj(const rcMeshLoaderObj&);
    rcMeshLoaderObj& operator=(const rcMeshLoaderObj&);

    void addVertex(float x, float y, float z, int& cap);
    void addTriangle(int a, int b, int c, int& cap);

    std::string m_filename;
    float m_scale;
    float* m_verts;
    int* m_tris;
    float* m_normals;
    int m_vertCount;
    int m_triCount;
};
}  // namespace vox