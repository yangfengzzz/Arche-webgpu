//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef fbx_mesh_hpp
#define fbx_mesh_hpp

#include <ozz/base/containers/vector.h>
#include <ozz/base/io/archive_traits.h>
#include <ozz/base/maths/simd_math.h>
#include <ozz/base/maths/vec_float.h>
#include <ozz/base/platform.h>

namespace ozz {
namespace loader {
// Defines a mesh with skinning information (joint indices and weights).
// The mesh is subdivided into parts that group vertices according to their
// number of influencing joints. Triangle indices are shared across mesh parts.
struct Mesh {
    // Number of triangle indices for the mesh.
    int triangle_index_count() const {
        return static_cast<int>(triangle_indices.size());
    }
    
    // Number of vertices for all mesh parts.
    int vertex_count() const {
        int vertex_count = 0;
        for (size_t i = 0; i < parts.size(); ++i) {
            vertex_count += parts[i].vertex_count();
        }
        return vertex_count;
    }
    
    // Maximum number of joints influences for all mesh parts.
    int max_influences_count() const {
        int max_influences_count = 0;
        for (size_t i = 0; i < parts.size(); ++i) {
            const int influences_count = parts[i].influences_count();
            max_influences_count = influences_count > max_influences_count
            ? influences_count
            : max_influences_count;
        }
        return max_influences_count;
    }
    
    // Test if the mesh has skinning informations.
    bool skinned() const {
        for (size_t i = 0; i < parts.size(); ++i) {
            if (parts[i].influences_count() != 0) {
                return true;
            }
        }
        return false;
    }
    
    // Returns the number of joints used to skin the mesh.
    int num_joints() const { return static_cast<int>(inverse_bind_poses.size()); }
    
    // Returns the highest joint number used in the skeleton.
    int highest_joint_index() const {
        // Takes advantage that joint_remaps is sorted.
        return joint_remaps.size() != 0 ? static_cast<int>(joint_remaps.back()) : 0;
    }
    
    // Defines a portion of the mesh. A mesh is subdivided in sets of vertices
    // with the same number of joint influences.
    struct Part {
        int vertex_count() const { return static_cast<int>(positions.size()) / 3; }
        
        int influences_count() const {
            const int _vertex_count = vertex_count();
            if (_vertex_count == 0) {
                return 0;
            }
            return static_cast<int>(joint_indices.size()) / _vertex_count;
        }
        
        typedef vector<float> Positions;
        Positions positions;
        enum { kPositionsCpnts = 3 };  // x, y, z components
        
        typedef vector<float> Normals;
        Normals normals;
        enum { kNormalsCpnts = 3 };  // x, y, z components
        
        typedef vector<float> Tangents;
        Tangents tangents;
        enum { kTangentsCpnts = 4 };  // x, y, z, right or left handed.
        
        typedef vector<float> UVs;
        UVs uvs;  // u, v components
        enum { kUVsCpnts = 2 };
        
        typedef vector<uint8_t> Colors;
        Colors colors;
        enum { kColorsCpnts = 4 };  // r, g, b, a components
        
        typedef vector<uint16_t> JointIndices;
        JointIndices joint_indices;  // Stride equals influences_count
        
        typedef vector<float> JointWeights;
        JointWeights joint_weights;  // Stride equals influences_count - 1
    };
    typedef vector<Part> Parts;
    Parts parts;
    
    // Triangles indices. Indices are shared across all parts.
    typedef vector<uint16_t> TriangleIndices;
    TriangleIndices triangle_indices;
    
    // Joints remapping indices. As a skin might be influenced by a part of the
    // skeleton only, joint indices and inverse bind pose matrices are reordered
    // to contain only used ones. Note that this array is sorted.
    typedef vector<uint16_t> JointRemaps;
    JointRemaps joint_remaps;
    
    // Inverse bind-pose matrices. These are only available for skinned meshes.
    typedef vector<math::Float4x4> InversBindPoses;
    InversBindPoses inverse_bind_poses;
};
}  // namespace loader

namespace io {
OZZ_IO_TYPE_TAG("ozz-sample-Mesh-Part", loader::Mesh::Part)
OZZ_IO_TYPE_VERSION(1, loader::Mesh::Part)

template <>
struct Extern<loader::Mesh::Part> {
    static void Save(OArchive& _archive, const loader::Mesh::Part* _parts,
                     size_t _count);
    static void Load(IArchive& _archive, loader::Mesh::Part* _parts,
                     size_t _count, uint32_t _version);
};

OZZ_IO_TYPE_TAG("ozz-sample-Mesh", loader::Mesh)
OZZ_IO_TYPE_VERSION(1, loader::Mesh)

template <>
struct Extern<loader::Mesh> {
    static void Save(OArchive& _archive, const loader::Mesh* _meshes,
                     size_t _count);
    static void Load(IArchive& _archive, loader::Mesh* _meshes, size_t _count,
                     uint32_t _version);
};

}  // namespace io

} // ozz

#endif /* fbx_mesh_hpp */
