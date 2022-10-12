//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "navigation/tools/convex_volume_tool.h"

#include <imgui.h>
#include <navigation/tools/input_geom.h>

#include <cfloat>
#ifdef WIN32
#define snprintf _snprintf
#endif

namespace vox {
// Quick and dirty convex hull.
namespace {
// Returns true if 'c' is left of line 'a'-'b'.
inline bool left(const float* a, const float* b, const float* c) {
    const float u1 = b[0] - a[0];
    const float v1 = b[2] - a[2];
    const float u2 = c[0] - a[0];
    const float v2 = c[2] - a[2];
    return u1 * v2 - v1 * u2 < 0;
}

// Returns true if 'a' is more lower-left than 'b'.
inline bool cmppt(const float* a, const float* b) {
    if (a[0] < b[0]) return true;
    if (a[0] > b[0]) return false;
    if (a[2] < b[2]) return true;
    if (a[2] > b[2]) return false;
    return false;
}

// Calculates convex hull on xz-plane of points on 'pts',
// stores the indices of the resulting hull in 'out' and
// returns number of points on hull.
int convexhull(const float* pts, int npts, int* out) {
    // Find lower-leftmost point.
    int hull = 0;
    for (int i = 1; i < npts; ++i)
        if (cmppt(&pts[i * 3], &pts[hull * 3])) hull = i;
    // Gift wrap hull.
    int endpt = 0;
    int i = 0;
    do {
        out[i++] = hull;
        endpt = 0;
        for (int j = 1; j < npts; ++j)
            if (hull == endpt || left(&pts[hull * 3], &pts[endpt * 3], &pts[j * 3])) endpt = j;
        hull = endpt;
    } while (endpt != out[0]);

    return i;
}

int pointInPoly(int nvert, const float* verts, const float* p) {
    int i, j, c = 0;
    for (i = 0, j = nvert - 1; i < nvert; j = i++) {
        const float* vi = &verts[i * 3];
        const float* vj = &verts[j * 3];
        if (((vi[2] > p[2]) != (vj[2] > p[2])) && (p[0] < (vj[0] - vi[0]) * (p[2] - vi[2]) / (vj[2] - vi[2]) + vi[0]))
            c = !c;
    }
    return c;
}
}  // namespace

ConvexVolumeTool::ConvexVolumeTool()
    : m_sample(nullptr),
      m_areaType(SAMPLE_POLYAREA_GRASS),
      m_polyOffset(0.0f),
      m_boxHeight(6.0f),
      m_boxDescent(1.0f),
      m_npts(0),
      m_nhull(0) {}

void ConvexVolumeTool::init(Sample* sample) { m_sample = sample; }

void ConvexVolumeTool::reset() {
    m_npts = 0;
    m_nhull = 0;
}

void ConvexVolumeTool::handleMenu() {
    ImGui::SliderFloat("Shape Height", &m_boxHeight, 0.1f, 20.0f);
    ImGui::SliderFloat("Shape Descent", &m_boxDescent, 0.1f, 20.0f);
    ImGui::SliderFloat("Poly Offset", &m_polyOffset, 0.0f, 10.0f);

    ImGui::Separator();

    ImGui::Text("Area Type");
    ImGui::Indent();
    if (ImGui::CheckboxFlags("Ground", &m_areaType, SAMPLE_POLYAREA_GROUND)) m_areaType = SAMPLE_POLYAREA_GROUND;
    if (ImGui::CheckboxFlags("Water", &m_areaType, SAMPLE_POLYAREA_WATER)) m_areaType = SAMPLE_POLYAREA_WATER;
    if (ImGui::CheckboxFlags("Road", &m_areaType, SAMPLE_POLYAREA_ROAD)) m_areaType = SAMPLE_POLYAREA_ROAD;
    if (ImGui::CheckboxFlags("Door", &m_areaType, SAMPLE_POLYAREA_DOOR)) m_areaType = SAMPLE_POLYAREA_DOOR;
    if (ImGui::CheckboxFlags("Grass", &m_areaType, SAMPLE_POLYAREA_GRASS)) m_areaType = SAMPLE_POLYAREA_GRASS;
    if (ImGui::CheckboxFlags("Jump", &m_areaType, SAMPLE_POLYAREA_JUMP)) m_areaType = SAMPLE_POLYAREA_JUMP;
    ImGui::Unindent();

    ImGui::Separator();

    if (ImGui::Button("Clear Shape")) {
        m_npts = 0;
        m_nhull = 0;
    }
}

void ConvexVolumeTool::handleClick(const float* /*s*/, const float* p, bool shift) {
    if (!m_sample) return;
    InputGeom* geom = m_sample->getInputGeom();
    if (!geom) return;

    if (shift) {
        // Delete
        int nearestIndex = -1;
        const ConvexVolume* vols = geom->getConvexVolumes();
        for (int i = 0; i < geom->getConvexVolumeCount(); ++i) {
            if (pointInPoly(vols[i].nverts, vols[i].verts, p) && p[1] >= vols[i].hmin && p[1] <= vols[i].hmax) {
                nearestIndex = i;
            }
        }
        // If end point close enough, delete it.
        if (nearestIndex != -1) {
            geom->deleteConvexVolume(nearestIndex);
        }
    } else {
        // Create

        // If clicked on that last pt, create the shape.
        if (m_npts && rcVdistSqr(p, &m_pts[(m_npts - 1) * 3]) < rcSqr(0.2f)) {
            if (m_nhull > 2) {
                // Create shape.
                float verts[MAX_PTS * 3];
                for (int i = 0; i < m_nhull; ++i) rcVcopy(&verts[i * 3], &m_pts[m_hull[i] * 3]);

                float minh = FLT_MAX, maxh = 0;
                for (int i = 0; i < m_nhull; ++i) minh = rcMin(minh, verts[i * 3 + 1]);
                minh -= m_boxDescent;
                maxh = minh + m_boxHeight;

                if (m_polyOffset > 0.01f) {
                    float offset[MAX_PTS * 2 * 3];
                    int noffset = rcOffsetPoly(verts, m_nhull, m_polyOffset, offset, MAX_PTS * 2);
                    if (noffset > 0) geom->addConvexVolume(offset, noffset, minh, maxh, (unsigned char)m_areaType);
                } else {
                    geom->addConvexVolume(verts, m_nhull, minh, maxh, (unsigned char)m_areaType);
                }
            }

            m_npts = 0;
            m_nhull = 0;
        } else {
            // Add new point
            if (m_npts < MAX_PTS) {
                rcVcopy(&m_pts[m_npts * 3], p);
                m_npts++;
                // Update hull.
                if (m_npts > 1)
                    m_nhull = convexhull(m_pts, m_npts, m_hull);
                else
                    m_nhull = 0;
            }
        }
    }
}

void ConvexVolumeTool::handleToggle() {}

void ConvexVolumeTool::handleStep() {}

void ConvexVolumeTool::handleUpdate(const float /*dt*/) {}

void ConvexVolumeTool::handleRender() {
    duDebugDraw& dd = m_sample->getDebugDraw();

    // Find height extent of the shape.
    float minh = FLT_MAX, maxh = 0;
    for (int i = 0; i < m_npts; ++i) minh = rcMin(minh, m_pts[i * 3 + 1]);
    minh -= m_boxDescent;
    maxh = minh + m_boxHeight;

    dd.begin(DU_DRAW_POINTS, 4.0f);
    for (int i = 0; i < m_npts; ++i) {
        unsigned int col = duRGBA(255, 255, 255, 255);
        if (i == m_npts - 1) col = duRGBA(240, 32, 16, 255);
        dd.vertex(m_pts[i * 3 + 0], m_pts[i * 3 + 1] + 0.1f, m_pts[i * 3 + 2], col);
    }
    dd.end();

    dd.begin(DU_DRAW_LINES, 2.0f);
    for (int i = 0, j = m_nhull - 1; i < m_nhull; j = i++) {
        const float* vi = &m_pts[m_hull[j] * 3];
        const float* vj = &m_pts[m_hull[i] * 3];
        dd.vertex(vj[0], minh, vj[2], duRGBA(255, 255, 255, 64));
        dd.vertex(vi[0], minh, vi[2], duRGBA(255, 255, 255, 64));
        dd.vertex(vj[0], maxh, vj[2], duRGBA(255, 255, 255, 64));
        dd.vertex(vi[0], maxh, vi[2], duRGBA(255, 255, 255, 64));
        dd.vertex(vj[0], minh, vj[2], duRGBA(255, 255, 255, 64));
        dd.vertex(vj[0], maxh, vj[2], duRGBA(255, 255, 255, 64));
    }
    dd.end();
}

void ConvexVolumeTool::handleRenderOverlay(double* /*proj*/, double* /*model*/, int* view) {
    // Tool help
    const int h = view[3];
    if (!m_npts) {
        ImGui::TextColored(ImVec4(255, 255, 255, 192),
                           "LMB: Create new shape.  SHIFT+LMB: Delete existing shape (click inside a shape).");
    } else {
        ImGui::TextColored(ImVec4(255, 255, 255, 192),
                           "Click LMB to add new points. Click on the red point to finish the shape.");
        ImGui::TextColored(ImVec4(255, 255, 255, 192), "The shape will be convex hull of all added points.");
    }
}
}  // namespace vox