//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.toolkit/navigation/tools/off_mesh_connection_tool.h"

namespace vox::nav {

OffMeshConnectionTool::OffMeshConnectionTool() : m_sample(nullptr), m_hitPosSet(false), m_bidir(true), m_oldFlags(0) {}

OffMeshConnectionTool::~OffMeshConnectionTool() {
    if (m_sample) {
        m_sample->setNavMeshDrawFlags(m_oldFlags);
    }
}

void OffMeshConnectionTool::init(NavigationManager* sample) {
    if (m_sample != sample) {
        m_sample = sample;
        m_oldFlags = m_sample->getNavMeshDrawFlags();
        m_sample->setNavMeshDrawFlags(m_oldFlags & ~debug::DRAW_NAVMESH_OFF_MESH_CONS);
    }
}

void OffMeshConnectionTool::reset() { m_hitPosSet = false; }

void OffMeshConnectionTool::handleClick(const float* /*s*/, const float* p, bool shift) {
    if (!m_sample) return;
    InputGeom* geom = m_sample->getInputGeom();
    if (!geom) return;

    if (shift) {
        // Delete
        // Find nearest link end-point
        float nearestDist = FLT_MAX;
        int nearestIndex = -1;
        const float* verts = geom->getOffMeshConnectionVerts();
        for (int i = 0; i < geom->getOffMeshConnectionCount() * 2; ++i) {
            const float* v = &verts[i * 3];
            float d = rcVdistSqr(p, v);
            if (d < nearestDist) {
                nearestDist = d;
                nearestIndex = i / 2;  // Each link has two vertices.
            }
        }
        // If end point close enough, delete it.
        if (nearestIndex != -1 && sqrtf(nearestDist) < m_sample->getAgentRadius()) {
            geom->deleteOffMeshConnection(nearestIndex);
        }
    } else {
        // Create
        if (!m_hitPosSet) {
            rcVcopy(m_hitPos, p);
            m_hitPosSet = true;
        } else {
            const unsigned char area = POLY_AREA_JUMP;
            const unsigned short flags = POLY_FLAGS_JUMP;
            geom->addOffMeshConnection(m_hitPos, p, m_sample->getAgentRadius(), m_bidir ? 1 : 0, area, flags);
            m_hitPosSet = false;
        }
    }
}

void OffMeshConnectionTool::handleToggle() {}

void OffMeshConnectionTool::handleStep() {}

void OffMeshConnectionTool::handleUpdate(const float /*dt*/) {}

void OffMeshConnectionTool::handleRender() {
    debug::DebugDraw& dd = m_sample->getDebugDraw();
    const float s = m_sample->getAgentRadius();

    if (m_hitPosSet)
        debug::debugDrawCross(&dd, m_hitPos[0], m_hitPos[1] + 0.1f, m_hitPos[2], s, debug::int2RGBA(0, 0, 0, 128),
                              2.0f);

    InputGeom* geom = m_sample->getInputGeom();
    if (geom) geom->drawOffMeshConnections(&dd, true);
}

}  // namespace vox::nav