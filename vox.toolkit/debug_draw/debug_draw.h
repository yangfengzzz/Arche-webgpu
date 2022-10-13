//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <webgpu/webgpu_cpp.h>

namespace vox::debug {

/// Abstract debug draw interface.
struct DebugDraw {
    virtual ~DebugDraw() = 0;

    virtual void depthMask(bool state) = 0;

    virtual void texture(bool state) = 0;

    /// Begin drawing primitives.
    ///  @param prim [in] primitive type to draw, one of rcDebugDrawPrimitives.
    ///  @param size [in] size of a primitive, applies to point size and line width only.
    virtual void begin(wgpu::PrimitiveTopology prim, float size = 1.0f) = 0;

    /// Submit a vertex
    ///  @param pos [in] position of the verts.
    ///  @param color [in] color of the verts.
    virtual void vertex(const float* pos, unsigned int color) = 0;

    /// Submit a vertex
    ///  @param x,y,z [in] position of the verts.
    ///  @param color [in] color of the verts.
    virtual void vertex(float x, float y, float z, unsigned int color) = 0;

    /// Submit a vertex
    ///  @param pos [in] position of the verts.
    ///  @param color [in] color of the verts.
    virtual void vertex(const float* pos, unsigned int color, const float* uv) = 0;

    /// Submit a vertex
    ///  @param x,y,z [in] position of the verts.
    ///  @param color [in] color of the verts.
    virtual void vertex(float x, float y, float z, unsigned int color, float u, float v) = 0;

    /// End drawing primitives.
    virtual void end() = 0;

    /// Compute a color for given area.
    virtual unsigned int areaToCol(unsigned int area);
};

inline unsigned int duRGBA(int r, int g, int b, int a) {
    return ((unsigned int)r) | ((unsigned int)g << 8) | ((unsigned int)b << 16) | ((unsigned int)a << 24);
}

inline unsigned int duRGBAf(float fr, float fg, float fb, float fa) {
    auto r = (unsigned char)(fr * 255.0f);
    auto g = (unsigned char)(fg * 255.0f);
    auto b = (unsigned char)(fb * 255.0f);
    auto a = (unsigned char)(fa * 255.0f);
    return duRGBA(r, g, b, a);
}

unsigned int duIntToCol(int i, int a);
void duIntToCol(int i, float* col);

inline unsigned int duMultCol(const unsigned int col, const unsigned int d) {
    const unsigned int r = col & 0xff;
    const unsigned int g = (col >> 8) & 0xff;
    const unsigned int b = (col >> 16) & 0xff;
    const unsigned int a = (col >> 24) & 0xff;
    return duRGBA((r * d) >> 8, (g * d) >> 8, (b * d) >> 8, a);
}

inline unsigned int duDarkenCol(unsigned int col) { return ((col >> 1) & 0x007f7f7f) | (col & 0xff000000); }

inline unsigned int duLerpCol(unsigned int ca, unsigned int cb, unsigned int u) {
    const unsigned int ra = ca & 0xff;
    const unsigned int ga = (ca >> 8) & 0xff;
    const unsigned int ba = (ca >> 16) & 0xff;
    const unsigned int aa = (ca >> 24) & 0xff;
    const unsigned int rb = cb & 0xff;
    const unsigned int gb = (cb >> 8) & 0xff;
    const unsigned int bb = (cb >> 16) & 0xff;
    const unsigned int ab = (cb >> 24) & 0xff;

    unsigned int r = (ra * (255 - u) + rb * u) / 255;
    unsigned int g = (ga * (255 - u) + gb * u) / 255;
    unsigned int b = (ba * (255 - u) + bb * u) / 255;
    unsigned int a = (aa * (255 - u) + ab * u) / 255;
    return duRGBA(r, g, b, a);
}

inline unsigned int duTransCol(unsigned int c, unsigned int a) { return (a << 24) | (c & 0x00ffffff); }

void duCalcBoxColors(unsigned int* colors, unsigned int colTop, unsigned int colSide);

void debugDrawCylinderWire(struct DebugDraw* dd,
                           float minx,
                           float miny,
                           float minz,
                           float maxx,
                           float maxy,
                           float maxz,
                           unsigned int col,
                           float lineWidth);

void debugDrawBoxWire(struct DebugDraw* dd,
                      float minx,
                      float miny,
                      float minz,
                      float maxx,
                      float maxy,
                      float maxz,
                      unsigned int col,
                      float lineWidth);

void debugDrawArc(struct DebugDraw* dd,
                  float x0,
                  float y0,
                  float z0,
                  float x1,
                  float y1,
                  float z1,
                  float h,
                  float as0,
                  float as1,
                  unsigned int col,
                  float lineWidth);

void debugDrawArrow(struct DebugDraw* dd,
                    float x0,
                    float y0,
                    float z0,
                    float x1,
                    float y1,
                    float z1,
                    float as0,
                    float as1,
                    unsigned int col,
                    float lineWidth);

void debugDrawCircle(struct DebugDraw* dd, float x, float y, float z, float r, unsigned int col, float lineWidth);

void debugDrawCross(struct DebugDraw* dd, float x, float y, float z, float size, unsigned int col, float lineWidth);

void debugDrawBox(struct DebugDraw* dd,
                  float minx,
                  float miny,
                  float minz,
                  float maxx,
                  float maxy,
                  float maxz,
                  const unsigned int* fcol);

void debugDrawCylinder(
        struct DebugDraw* dd, float minx, float miny, float minz, float maxx, float maxy, float maxz, unsigned int col);

void debugDrawGridXZ(struct DebugDraw* dd,
                     float ox,
                     float oy,
                     float oz,
                     int w,
                     int h,
                     float size,
                     unsigned int col,
                     float lineWidth);

// Versions without begin/end, can be used to draw multiple primitives.
void duAppendCylinderWire(
        struct DebugDraw* dd, float minx, float miny, float minz, float maxx, float maxy, float maxz, unsigned int col);

void duAppendBoxWire(
        struct DebugDraw* dd, float minx, float miny, float minz, float maxx, float maxy, float maxz, unsigned int col);

void duAppendBoxPoints(
        struct DebugDraw* dd, float minx, float miny, float minz, float maxx, float maxy, float maxz, unsigned int col);

void duAppendArc(struct DebugDraw* dd,
                 float x0,
                 float y0,
                 float z0,
                 float x1,
                 float y1,
                 float z1,
                 float h,
                 float as0,
                 float as1,
                 unsigned int col);

void duAppendArrow(struct DebugDraw* dd,
                   float x0,
                   float y0,
                   float z0,
                   float x1,
                   float y1,
                   float z1,
                   float as0,
                   float as1,
                   unsigned int col);

void duAppendCircle(struct DebugDraw* dd, float x, float y, float z, float r, unsigned int col);

void duAppendCross(struct DebugDraw* dd, float x, float y, float z, float size, unsigned int col);

void duAppendBox(struct DebugDraw* dd,
                 float minx,
                 float miny,
                 float minz,
                 float maxx,
                 float maxy,
                 float maxz,
                 const unsigned int* fcol);

void duAppendCylinder(
        struct DebugDraw* dd, float minx, float miny, float minz, float maxx, float maxy, float maxz, unsigned int col);

class duDisplayList : public DebugDraw {
    float* m_pos;
    unsigned int* m_color;
    int m_size;
    int m_cap;

    bool m_depthMask;
    wgpu::PrimitiveTopology m_prim;
    float m_primSize;

    void resize(int cap);

public:
    duDisplayList(int cap = 512);
    ~duDisplayList() override;
    void depthMask(bool state) override;
    void begin(wgpu::PrimitiveTopology prim, float size = 1.0f) override;
    void vertex(float x, float y, float z, unsigned int color) override;
    void vertex(const float* pos, unsigned int color) override;
    void end() override;
    void clear();
    void draw(struct DebugDraw* dd);

private:
    // Explicitly disabled copy constructor and copy assignment operator.
    duDisplayList(const duDisplayList&);
    duDisplayList& operator=(const duDisplayList&);
};

}  // namespace vox::debug