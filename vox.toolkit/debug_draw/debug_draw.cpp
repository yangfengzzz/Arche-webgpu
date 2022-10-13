//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.toolkit/debug_draw/debug_draw.h"

#include <DetourMath.h>

#include <cstring>

#include "vox.base/constants.h"

namespace vox::debug {
DebugDraw::~DebugDraw() = default;

unsigned int DebugDraw::areaToCol(unsigned int area) {
    if (area == 0) {
        // Treat zero area type as default.
        return int2RGBA(0, 192, 255, 255);
    } else {
        return intToCol(area, 255);
    }
}

//----------------------------------------------------------------------------------------------------------------------
inline int bit(int a, int b) { return (a & (1 << b)) >> b; }

unsigned int intToCol(int i, int a) {
    int r = bit(i, 1) + bit(i, 3) * 2 + 1;
    int g = bit(i, 2) + bit(i, 4) * 2 + 1;
    int b = bit(i, 0) + bit(i, 5) * 2 + 1;
    return int2RGBA(r * 63, g * 63, b * 63, a);
}

void intToCol(int i, float* col) {
    int r = bit(i, 0) + bit(i, 3) * 2 + 1;
    int g = bit(i, 1) + bit(i, 4) * 2 + 1;
    int b = bit(i, 2) + bit(i, 5) * 2 + 1;
    col[0] = 1 - r * 63.0f / 255.0f;
    col[1] = 1 - g * 63.0f / 255.0f;
    col[2] = 1 - b * 63.0f / 255.0f;
}

void calcBoxColors(unsigned int* colors, unsigned int colTop, unsigned int colSide) {
    if (!colors) return;

    colors[0] = multCol(colTop, 250);
    colors[1] = multCol(colSide, 140);
    colors[2] = multCol(colSide, 165);
    colors[3] = multCol(colSide, 217);
    colors[4] = multCol(colSide, 165);
    colors[5] = multCol(colSide, 217);
}

//----------------------------------------------------------------------------------------------------------------------
void debugDrawCylinderWire(struct DebugDraw* dd,
                           float minx,
                           float miny,
                           float minz,
                           float maxx,
                           float maxy,
                           float maxz,
                           unsigned int col,
                           const float lineWidth) {
    if (!dd) return;

    dd->begin(wgpu::PrimitiveTopology::LineList, lineWidth);
    debugAppendCylinderWire(dd, minx, miny, minz, maxx, maxy, maxz, col);
    dd->end();
}

void debugDrawBoxWire(struct DebugDraw* dd,
                      float minx,
                      float miny,
                      float minz,
                      float maxx,
                      float maxy,
                      float maxz,
                      unsigned int col,
                      const float lineWidth) {
    if (!dd) return;

    dd->begin(wgpu::PrimitiveTopology::LineList, lineWidth);
    debugAppendBoxWire(dd, minx, miny, minz, maxx, maxy, maxz, col);
    dd->end();
}

void debugDrawArc(struct DebugDraw* dd,
                  const float x0,
                  const float y0,
                  const float z0,
                  const float x1,
                  const float y1,
                  const float z1,
                  const float h,
                  const float as0,
                  const float as1,
                  unsigned int col,
                  const float lineWidth) {
    if (!dd) return;

    dd->begin(wgpu::PrimitiveTopology::LineList, lineWidth);
    debugAppendArc(dd, x0, y0, z0, x1, y1, z1, h, as0, as1, col);
    dd->end();
}

void debugDrawArrow(struct DebugDraw* dd,
                    const float x0,
                    const float y0,
                    const float z0,
                    const float x1,
                    const float y1,
                    const float z1,
                    const float as0,
                    const float as1,
                    unsigned int col,
                    const float lineWidth) {
    if (!dd) return;

    dd->begin(wgpu::PrimitiveTopology::LineList, lineWidth);
    debugAppendArrow(dd, x0, y0, z0, x1, y1, z1, as0, as1, col);
    dd->end();
}

void debugDrawCircle(struct DebugDraw* dd,
                     const float x,
                     const float y,
                     const float z,
                     const float r,
                     unsigned int col,
                     const float lineWidth) {
    if (!dd) return;

    dd->begin(wgpu::PrimitiveTopology::LineList, lineWidth);
    debugAppendCircle(dd, x, y, z, r, col);
    dd->end();
}

void debugDrawCross(struct DebugDraw* dd,
                    const float x,
                    const float y,
                    const float z,
                    const float size,
                    unsigned int col,
                    const float lineWidth) {
    if (!dd) return;

    dd->begin(wgpu::PrimitiveTopology::LineList, lineWidth);
    debugAppendCross(dd, x, y, z, size, col);
    dd->end();
}

void debugDrawBox(struct DebugDraw* dd,
                  float minx,
                  float miny,
                  float minz,
                  float maxx,
                  float maxy,
                  float maxz,
                  const unsigned int* fcol) {
    if (!dd) return;

    dd->begin(wgpu::PrimitiveTopology::TriangleStrip);  // todo
    debugAppendBox(dd, minx, miny, minz, maxx, maxy, maxz, fcol);
    dd->end();
}

void debugDrawCylinder(struct DebugDraw* dd,
                       float minx,
                       float miny,
                       float minz,
                       float maxx,
                       float maxy,
                       float maxz,
                       unsigned int col) {
    if (!dd) return;

    dd->begin(wgpu::PrimitiveTopology::TriangleList);
    debugAppendCylinder(dd, minx, miny, minz, maxx, maxy, maxz, col);
    dd->end();
}

void debugDrawGridXZ(struct DebugDraw* dd,
                     const float ox,
                     const float oy,
                     const float oz,
                     const int w,
                     const int h,
                     const float size,
                     const unsigned int col,
                     const float lineWidth) {
    if (!dd) return;

    dd->begin(wgpu::PrimitiveTopology::LineList, lineWidth);
    for (int i = 0; i <= h; ++i) {
        dd->vertex(ox, oy, oz + i * size, col);
        dd->vertex(ox + w * size, oy, oz + i * size, col);
    }
    for (int i = 0; i <= w; ++i) {
        dd->vertex(ox + i * size, oy, oz, col);
        dd->vertex(ox + i * size, oy, oz + h * size, col);
    }
    dd->end();
}

void debugAppendCylinderWire(struct DebugDraw* dd,
                          float minx,
                          float miny,
                          float minz,
                          float maxx,
                          float maxy,
                          float maxz,
                          unsigned int col) {
    if (!dd) return;

    static const int NUM_SEG = 16;
    static float dir[NUM_SEG * 2];
    static bool init = false;
    if (!init) {
        init = true;
        for (int i = 0; i < NUM_SEG; ++i) {
            const float a = (float)i / (float)NUM_SEG * kPiF * 2;
            dir[i * 2] = dtMathCosf(a);
            dir[i * 2 + 1] = dtMathSinf(a);
        }
    }

    const float cx = (maxx + minx) / 2;
    const float cz = (maxz + minz) / 2;
    const float rx = (maxx - minx) / 2;
    const float rz = (maxz - minz) / 2;

    for (int i = 0, j = NUM_SEG - 1; i < NUM_SEG; j = i++) {
        dd->vertex(cx + dir[j * 2 + 0] * rx, miny, cz + dir[j * 2 + 1] * rz, col);
        dd->vertex(cx + dir[i * 2 + 0] * rx, miny, cz + dir[i * 2 + 1] * rz, col);
        dd->vertex(cx + dir[j * 2 + 0] * rx, maxy, cz + dir[j * 2 + 1] * rz, col);
        dd->vertex(cx + dir[i * 2 + 0] * rx, maxy, cz + dir[i * 2 + 1] * rz, col);
    }
    for (int i = 0; i < NUM_SEG; i += NUM_SEG / 4) {
        dd->vertex(cx + dir[i * 2 + 0] * rx, miny, cz + dir[i * 2 + 1] * rz, col);
        dd->vertex(cx + dir[i * 2 + 0] * rx, maxy, cz + dir[i * 2 + 1] * rz, col);
    }
}

void debugAppendBoxWire(struct DebugDraw* dd,
                     float minx,
                     float miny,
                     float minz,
                     float maxx,
                     float maxy,
                     float maxz,
                     unsigned int col) {
    if (!dd) return;
    // Top
    dd->vertex(minx, miny, minz, col);
    dd->vertex(maxx, miny, minz, col);
    dd->vertex(maxx, miny, minz, col);
    dd->vertex(maxx, miny, maxz, col);
    dd->vertex(maxx, miny, maxz, col);
    dd->vertex(minx, miny, maxz, col);
    dd->vertex(minx, miny, maxz, col);
    dd->vertex(minx, miny, minz, col);

    // bottom
    dd->vertex(minx, maxy, minz, col);
    dd->vertex(maxx, maxy, minz, col);
    dd->vertex(maxx, maxy, minz, col);
    dd->vertex(maxx, maxy, maxz, col);
    dd->vertex(maxx, maxy, maxz, col);
    dd->vertex(minx, maxy, maxz, col);
    dd->vertex(minx, maxy, maxz, col);
    dd->vertex(minx, maxy, minz, col);

    // Sides
    dd->vertex(minx, miny, minz, col);
    dd->vertex(minx, maxy, minz, col);
    dd->vertex(maxx, miny, minz, col);
    dd->vertex(maxx, maxy, minz, col);
    dd->vertex(maxx, miny, maxz, col);
    dd->vertex(maxx, maxy, maxz, col);
    dd->vertex(minx, miny, maxz, col);
    dd->vertex(minx, maxy, maxz, col);
}

void debugAppendBoxPoints(struct DebugDraw* dd,
                       float minx,
                       float miny,
                       float minz,
                       float maxx,
                       float maxy,
                       float maxz,
                       unsigned int col) {
    if (!dd) return;
    // Top
    dd->vertex(minx, miny, minz, col);
    dd->vertex(maxx, miny, minz, col);
    dd->vertex(maxx, miny, minz, col);
    dd->vertex(maxx, miny, maxz, col);
    dd->vertex(maxx, miny, maxz, col);
    dd->vertex(minx, miny, maxz, col);
    dd->vertex(minx, miny, maxz, col);
    dd->vertex(minx, miny, minz, col);

    // bottom
    dd->vertex(minx, maxy, minz, col);
    dd->vertex(maxx, maxy, minz, col);
    dd->vertex(maxx, maxy, minz, col);
    dd->vertex(maxx, maxy, maxz, col);
    dd->vertex(maxx, maxy, maxz, col);
    dd->vertex(minx, maxy, maxz, col);
    dd->vertex(minx, maxy, maxz, col);
    dd->vertex(minx, maxy, minz, col);
}

void debugAppendBox(struct DebugDraw* dd,
                 float minx,
                 float miny,
                 float minz,
                 float maxx,
                 float maxy,
                 float maxz,
                 const unsigned int* fcol) {
    if (!dd) return;
    const float verts[8 * 3] = {
            minx, miny, minz, maxx, miny, minz, maxx, miny, maxz, minx, miny, maxz,
            minx, maxy, minz, maxx, maxy, minz, maxx, maxy, maxz, minx, maxy, maxz,
    };
    static const unsigned char inds[6 * 4] = {
            7, 6, 5, 4, 0, 1, 2, 3, 1, 5, 6, 2, 3, 7, 4, 0, 2, 6, 7, 3, 0, 4, 5, 1,
    };

    const unsigned char* in = inds;
    for (int i = 0; i < 6; ++i) {
        dd->vertex(&verts[*in * 3], fcol[i]);
        in++;
        dd->vertex(&verts[*in * 3], fcol[i]);
        in++;
        dd->vertex(&verts[*in * 3], fcol[i]);
        in++;
        dd->vertex(&verts[*in * 3], fcol[i]);
        in++;
    }
}

void debugAppendCylinder(struct DebugDraw* dd,
                      float minx,
                      float miny,
                      float minz,
                      float maxx,
                      float maxy,
                      float maxz,
                      unsigned int col) {
    if (!dd) return;

    static const int NUM_SEG = 16;
    static float dir[NUM_SEG * 2];
    static bool init = false;
    if (!init) {
        init = true;
        for (int i = 0; i < NUM_SEG; ++i) {
            const float a = (float)i / (float)NUM_SEG * kPiF * 2;
            dir[i * 2] = cosf(a);
            dir[i * 2 + 1] = sinf(a);
        }
    }

    unsigned int col2 = multCol(col, 160);

    const float cx = (maxx + minx) / 2;
    const float cz = (maxz + minz) / 2;
    const float rx = (maxx - minx) / 2;
    const float rz = (maxz - minz) / 2;

    for (int i = 2; i < NUM_SEG; ++i) {
        const int a = 0, b = i - 1, c = i;
        dd->vertex(cx + dir[a * 2 + 0] * rx, miny, cz + dir[a * 2 + 1] * rz, col2);
        dd->vertex(cx + dir[b * 2 + 0] * rx, miny, cz + dir[b * 2 + 1] * rz, col2);
        dd->vertex(cx + dir[c * 2 + 0] * rx, miny, cz + dir[c * 2 + 1] * rz, col2);
    }
    for (int i = 2; i < NUM_SEG; ++i) {
        const int a = 0, b = i, c = i - 1;
        dd->vertex(cx + dir[a * 2 + 0] * rx, maxy, cz + dir[a * 2 + 1] * rz, col);
        dd->vertex(cx + dir[b * 2 + 0] * rx, maxy, cz + dir[b * 2 + 1] * rz, col);
        dd->vertex(cx + dir[c * 2 + 0] * rx, maxy, cz + dir[c * 2 + 1] * rz, col);
    }
    for (int i = 0, j = NUM_SEG - 1; i < NUM_SEG; j = i++) {
        dd->vertex(cx + dir[i * 2 + 0] * rx, miny, cz + dir[i * 2 + 1] * rz, col2);
        dd->vertex(cx + dir[j * 2 + 0] * rx, miny, cz + dir[j * 2 + 1] * rz, col2);
        dd->vertex(cx + dir[j * 2 + 0] * rx, maxy, cz + dir[j * 2 + 1] * rz, col);

        dd->vertex(cx + dir[i * 2 + 0] * rx, miny, cz + dir[i * 2 + 1] * rz, col2);
        dd->vertex(cx + dir[j * 2 + 0] * rx, maxy, cz + dir[j * 2 + 1] * rz, col);
        dd->vertex(cx + dir[i * 2 + 0] * rx, maxy, cz + dir[i * 2 + 1] * rz, col);
    }
}

inline void evalArc(const float x0,
                    const float y0,
                    const float z0,
                    const float dx,
                    const float dy,
                    const float dz,
                    const float h,
                    const float u,
                    float* res) {
    res[0] = x0 + dx * u;
    res[1] = y0 + dy * u + h * (1 - (u * 2 - 1) * (u * 2 - 1));
    res[2] = z0 + dz * u;
}

inline void vcross(float* dest, const float* v1, const float* v2) {
    dest[0] = v1[1] * v2[2] - v1[2] * v2[1];
    dest[1] = v1[2] * v2[0] - v1[0] * v2[2];
    dest[2] = v1[0] * v2[1] - v1[1] * v2[0];
}

inline void vnormalize(float* v) {
    float d = 1.0f / sqrtf(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    v[0] *= d;
    v[1] *= d;
    v[2] *= d;
}

inline void vsub(float* dest, const float* v1, const float* v2) {
    dest[0] = v1[0] - v2[0];
    dest[1] = v1[1] - v2[1];
    dest[2] = v1[2] - v2[2];
}

inline float vdistSqr(const float* v1, const float* v2) {
    const float x = v1[0] - v2[0];
    const float y = v1[1] - v2[1];
    const float z = v1[2] - v2[2];
    return x * x + y * y + z * z;
}

void appendArrowHead(struct DebugDraw* dd, const float* p, const float* q, const float s, unsigned int col) {
    const float eps = 0.001f;
    if (!dd) return;
    if (vdistSqr(p, q) < eps * eps) return;
    float ax[3], ay[3] = {0, 1, 0}, az[3];
    vsub(az, q, p);
    vnormalize(az);
    vcross(ax, ay, az);
    vcross(ay, az, ax);
    vnormalize(ay);

    dd->vertex(p, col);
    //	dd->vertex(p[0]+az[0]*s+ay[0]*s/2, p[1]+az[1]*s+ay[1]*s/2, p[2]+az[2]*s+ay[2]*s/2, col);
    dd->vertex(p[0] + az[0] * s + ax[0] * s / 3, p[1] + az[1] * s + ax[1] * s / 3, p[2] + az[2] * s + ax[2] * s / 3,
               col);

    dd->vertex(p, col);
    //	dd->vertex(p[0]+az[0]*s-ay[0]*s/2, p[1]+az[1]*s-ay[1]*s/2, p[2]+az[2]*s-ay[2]*s/2, col);
    dd->vertex(p[0] + az[0] * s - ax[0] * s / 3, p[1] + az[1] * s - ax[1] * s / 3, p[2] + az[2] * s - ax[2] * s / 3,
               col);
}

void debugAppendArc(struct DebugDraw* dd,
                 const float x0,
                 const float y0,
                 const float z0,
                 const float x1,
                 const float y1,
                 const float z1,
                 const float h,
                 const float as0,
                 const float as1,
                 unsigned int col) {
    if (!dd) return;
    static const int NUM_ARC_PTS = 8;
    static const float PAD = 0.05f;
    static const float ARC_PTS_SCALE = (1.0f - PAD * 2) / (float)NUM_ARC_PTS;
    const float dx = x1 - x0;
    const float dy = y1 - y0;
    const float dz = z1 - z0;
    const float len = sqrtf(dx * dx + dy * dy + dz * dz);
    float prev[3];
    evalArc(x0, y0, z0, dx, dy, dz, len * h, PAD, prev);
    for (int i = 1; i <= NUM_ARC_PTS; ++i) {
        const float u = PAD + i * ARC_PTS_SCALE;
        float pt[3];
        evalArc(x0, y0, z0, dx, dy, dz, len * h, u, pt);
        dd->vertex(prev[0], prev[1], prev[2], col);
        dd->vertex(pt[0], pt[1], pt[2], col);
        prev[0] = pt[0];
        prev[1] = pt[1];
        prev[2] = pt[2];
    }

    // End arrows
    if (as0 > 0.001f) {
        float p[3], q[3];
        evalArc(x0, y0, z0, dx, dy, dz, len * h, PAD, p);
        evalArc(x0, y0, z0, dx, dy, dz, len * h, PAD + 0.05f, q);
        appendArrowHead(dd, p, q, as0, col);
    }

    if (as1 > 0.001f) {
        float p[3], q[3];
        evalArc(x0, y0, z0, dx, dy, dz, len * h, 1 - PAD, p);
        evalArc(x0, y0, z0, dx, dy, dz, len * h, 1 - (PAD + 0.05f), q);
        appendArrowHead(dd, p, q, as1, col);
    }
}

void debugAppendArrow(struct DebugDraw* dd,
                   const float x0,
                   const float y0,
                   const float z0,
                   const float x1,
                   const float y1,
                   const float z1,
                   const float as0,
                   const float as1,
                   unsigned int col) {
    if (!dd) return;

    dd->vertex(x0, y0, z0, col);
    dd->vertex(x1, y1, z1, col);

    // End arrows
    const float p[3] = {x0, y0, z0}, q[3] = {x1, y1, z1};
    if (as0 > 0.001f) appendArrowHead(dd, p, q, as0, col);
    if (as1 > 0.001f) appendArrowHead(dd, q, p, as1, col);
}

void debugAppendCircle(
        struct DebugDraw* dd, const float x, const float y, const float z, const float r, unsigned int col) {
    if (!dd) return;
    static const int NUM_SEG = 40;
    static float dir[40 * 2];
    static bool init = false;
    if (!init) {
        init = true;
        for (int i = 0; i < NUM_SEG; ++i) {
            const float a = (float)i / (float)NUM_SEG * kPiF * 2;
            dir[i * 2] = cosf(a);
            dir[i * 2 + 1] = sinf(a);
        }
    }

    for (int i = 0, j = NUM_SEG - 1; i < NUM_SEG; j = i++) {
        dd->vertex(x + dir[j * 2 + 0] * r, y, z + dir[j * 2 + 1] * r, col);
        dd->vertex(x + dir[i * 2 + 0] * r, y, z + dir[i * 2 + 1] * r, col);
    }
}

void debugAppendCross(struct DebugDraw* dd, const float x, const float y, const float z, const float s, unsigned int col) {
    if (!dd) return;
    dd->vertex(x - s, y, z, col);
    dd->vertex(x + s, y, z, col);
    dd->vertex(x, y - s, z, col);
    dd->vertex(x, y + s, z, col);
    dd->vertex(x, y, z - s, col);
    dd->vertex(x, y, z + s, col);
}

//----------------------------------------------------------------------------------------------------------------------
DisplayList::DisplayList(int cap)
    : m_pos(nullptr),
      m_color(nullptr),
      m_size(0),
      m_cap(0),
      m_depthMask(true),
      m_prim(wgpu::PrimitiveTopology::LineList),
      m_primSize(1.0f) {
    if (cap < 8) cap = 8;
    resize(cap);
}

DisplayList::~DisplayList() {
    delete[] m_pos;
    delete[] m_color;
}

void DisplayList::resize(int cap) {
    auto* newPos = new float[cap * 3];
    if (m_size) memcpy(newPos, m_pos, sizeof(float) * 3 * m_size);
    delete[] m_pos;
    m_pos = newPos;

    auto* newColor = new unsigned int[cap];
    if (m_size) memcpy(newColor, m_color, sizeof(unsigned int) * m_size);
    delete[] m_color;
    m_color = newColor;

    m_cap = cap;
}

void DisplayList::clear() { m_size = 0; }

void DisplayList::depthMask(bool state) { m_depthMask = state; }

void DisplayList::begin(wgpu::PrimitiveTopology prim, float size) {
    clear();
    m_prim = prim;
    m_primSize = size;
}

void DisplayList::vertex(const float x, const float y, const float z, unsigned int color) {
    if (m_size + 1 >= m_cap) resize(m_cap * 2);
    float* p = &m_pos[m_size * 3];
    p[0] = x;
    p[1] = y;
    p[2] = z;
    m_color[m_size] = color;
    m_size++;
}

void DisplayList::vertex(const float* pos, unsigned int color) { vertex(pos[0], pos[1], pos[2], color); }

void DisplayList::end() {}

void DisplayList::draw(struct DebugDraw* dd) {
    if (!dd) return;
    if (!m_size) return;
    dd->depthMask(m_depthMask);
    dd->begin(m_prim, m_primSize);
    for (int i = 0; i < m_size; ++i) dd->vertex(&m_pos[i * 3], m_color[i]);
    dd->end();
}
}  // namespace vox::debug