//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.base/containers/vector.h"
#include "vox.base/io/archive_traits.h"
#include "vox.base/macros.h"
#include "vox.simd_math/simd_math.h"

namespace vox {

// Defines a mesh with skinning information (joint indices and weights).
// The mesh is subdivided into parts that group vertices according to their
// number of influencing joints. Triangle indices are shared across mesh parts.
struct Skin {
    // Number of triangle indices for the mesh.
    [[nodiscard]] int triangle_index_count() const { return static_cast<int>(triangle_indices.size()); }

    // Number of vertices for all mesh parts.
    [[nodiscard]] int vertex_count() const {
        int vertex_count = 0;
        for (const auto& part : parts) {
            vertex_count += part.vertex_count();
        }
        return vertex_count;
    }

    // Maximum number of joints influences for all mesh parts.
    [[nodiscard]] int max_influences_count() const {
        int max_influences_count = 0;
        for (const auto& part : parts) {
            const int influences_count = part.influences_count();
            max_influences_count = influences_count > max_influences_count ? influences_count : max_influences_count;
        }
        return max_influences_count;
    }

    // Test if the mesh has skinning information.
    [[nodiscard]] bool skinned() const { return !inverse_bind_poses.empty(); }

    // Returns the number of joints used to skin the mesh.
    [[nodiscard]] int num_joints() const { return static_cast<int>(inverse_bind_poses.size()); }

    // Returns the highest joint number used in the skeleton.
    [[nodiscard]] int highest_joint_index() const {
        // Takes advantage that joint_remaps is sorted.
        return !joint_remaps.empty() ? static_cast<int>(joint_remaps.back()) : 0;
    }

    // Defines a portion of the mesh. A mesh is subdivided in sets of vertices
    // with the same number of joint influences.
    struct Part {
        [[nodiscard]] int vertex_count() const { return static_cast<int>(positions.size()) / 3; }

        [[nodiscard]] int influences_count() const {
            const int _vertex_count = vertex_count();
            if (_vertex_count == 0) {
                return 0;
            }
            return static_cast<int>(joint_indices.size()) / _vertex_count;
        }

        typedef vox::vector<float> Positions;
        Positions positions;
        enum { kPositionsCpnts = 3 };  // x, y, z components

        typedef vox::vector<float> Normals;
        Normals normals;
        enum { kNormalsCpnts = 3 };  // x, y, z components

        typedef vox::vector<float> Tangents;
        Tangents tangents;
        enum { kTangentsCpnts = 4 };  // x, y, z, right or left handed.

        typedef vox::vector<float> UVs;
        UVs uvs;  // u, v components
        enum { kUVsCpnts = 2 };

        typedef vox::vector<uint8_t> Colors;
        Colors colors;
        enum { kColorsCpnts = 4 };  // r, g, b, a components

        typedef vox::vector<uint16_t> JointIndices;
        JointIndices joint_indices;  // Stride equals influences_count

        typedef vox::vector<float> JointWeights;
        JointWeights joint_weights;  // Stride equals influences_count - 1
    };
    typedef vox::vector<Part> Parts;
    Parts parts;

    // Triangles indices. Indices are shared across all parts.
    typedef vox::vector<uint16_t> TriangleIndices;
    TriangleIndices triangle_indices;

    // Joints remapping indices. As a skin might be influenced by a part of the
    // skeleton only, joint indices and inverse bind pose matrices are reordered
    // to contain only used ones. Note that this array is sorted.
    typedef vox::vector<uint16_t> JointRemaps;
    JointRemaps joint_remaps;

    // Inverse bind-pose matrices. These are only available for skinned meshes.
    typedef vox::vector<vox::simd_math::Float4x4> InverseBindPoses;
    InverseBindPoses inverse_bind_poses;
};

namespace io {

VOX_IO_TYPE_TAG("ozz-sample-Mesh-Part", Skin::Part)
VOX_IO_TYPE_VERSION(1, Skin::Part)

template <>
struct Extern<Skin::Part> {
    static void Save(OArchive& _archive, const Skin::Part* _parts, size_t _count);
    static void Load(IArchive& _archive, Skin::Part* _parts, size_t _count, uint32_t _version);
};

VOX_IO_TYPE_TAG("ozz-sample-Mesh", Skin)
VOX_IO_TYPE_VERSION(1, Skin)

template <>
struct Extern<Skin> {
    static void Save(OArchive& _archive, const Skin* _meshes, size_t _count);
    static void Load(IArchive& _archive, Skin* _meshes, size_t _count, uint32_t _version);
};
}  // namespace io
}  // namespace vox
