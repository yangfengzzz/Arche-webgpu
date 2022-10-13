//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.toolkit/debug_draw/recast_debug_draw.h"

#include <cmath>

#include "vox.base/constants.h"

namespace vox::debug {
namespace {
void drawLayerPortals(DebugDraw* dd, const rcHeightfieldLayer* layer) {
    const float cs = layer->cs;
    const float ch = layer->ch;
    const int w = layer->width;
    const int h = layer->height;

    unsigned int pcol = int2RGBA(255, 255, 255, 255);

    const int segs[4 * 4] = {0, 0, 0, 1, 0, 1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0};

    // Layer portals
    dd->begin(wgpu::PrimitiveTopology::LineList, 2.0f);
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            const int idx = x + y * w;
            const int lh = (int)layer->heights[idx];
            if (lh == 255) continue;

            for (int dir = 0; dir < 4; ++dir) {
                if (layer->cons[idx] & (1 << (dir + 4))) {
                    const int* seg = &segs[dir * 4];
                    const float ax = layer->bmin[0] + (x + seg[0]) * cs;
                    const float ay = layer->bmin[1] + (lh + 2) * ch;
                    const float az = layer->bmin[2] + (y + seg[1]) * cs;
                    const float bx = layer->bmin[0] + (x + seg[2]) * cs;
                    const float by = layer->bmin[1] + (lh + 2) * ch;
                    const float bz = layer->bmin[2] + (y + seg[3]) * cs;
                    dd->vertex(ax, ay, az, pcol);
                    dd->vertex(bx, by, bz, pcol);
                }
            }
        }
    }
    dd->end();
}

void getContourCenter(const rcContour* cont, const float* orig, float cs, float ch, float* center) {
    center[0] = 0;
    center[1] = 0;
    center[2] = 0;
    if (!cont->nverts) return;
    for (int i = 0; i < cont->nverts; ++i) {
        const int* v = &cont->verts[i * 4];
        center[0] += (float)v[0];
        center[1] += (float)v[1];
        center[2] += (float)v[2];
    }
    const float s = 1.0f / cont->nverts;
    center[0] *= s * cs;
    center[1] *= s * ch;
    center[2] *= s * cs;
    center[0] += orig[0];
    center[1] += orig[1] + 4 * ch;
    center[2] += orig[2];
}

const rcContour* findContourFromSet(const rcContourSet& cset, unsigned short reg) {
    for (int i = 0; i < cset.nconts; ++i) {
        if (cset.conts[i].reg == reg) return &cset.conts[i];
    }
    return nullptr;
}

}  // namespace

void debugDrawTriMesh(DebugDraw* dd,
                      const float* verts,
                      int /*nverts*/,
                      const int* tris,
                      const float* normals,
                      int ntris,
                      const unsigned char* flags,
                      const float texScale) {
    if (!dd) return;
    if (!verts) return;
    if (!tris) return;
    if (!normals) return;

    float uva[2];
    float uvb[2];
    float uvc[2];

    const unsigned int unwalkable = int2RGBA(192, 128, 0, 255);

    dd->texture(true);

    dd->begin(wgpu::PrimitiveTopology::TriangleList);
    for (int i = 0; i < ntris * 3; i += 3) {
        const float* norm = &normals[i];
        unsigned int color;
        auto a = (unsigned char)(220 * (2 + norm[0] + norm[1]) / 4);
        if (flags && !flags[i / 3])
            color = lerpCol(int2RGBA(a, a, a, 255), unwalkable, 64);
        else
            color = int2RGBA(a, a, a, 255);

        const float* va = &verts[tris[i + 0] * 3];
        const float* vb = &verts[tris[i + 1] * 3];
        const float* vc = &verts[tris[i + 2] * 3];

        int ax = 0, ay = 0;
        if (rcAbs(norm[1]) > rcAbs(norm[ax])) ax = 1;
        if (rcAbs(norm[2]) > rcAbs(norm[ax])) ax = 2;
        ax = (1 << ax) & 3;  // +1 mod 3
        ay = (1 << ax) & 3;  // +1 mod 3

        uva[0] = va[ax] * texScale;
        uva[1] = va[ay] * texScale;
        uvb[0] = vb[ax] * texScale;
        uvb[1] = vb[ay] * texScale;
        uvc[0] = vc[ax] * texScale;
        uvc[1] = vc[ay] * texScale;

        dd->vertex(va, color, uva);
        dd->vertex(vb, color, uvb);
        dd->vertex(vc, color, uvc);
    }
    dd->end();
    dd->texture(false);
}

void debugDrawTriMeshSlope(DebugDraw* dd,
                           const float* verts,
                           int /*nverts*/,
                           const int* tris,
                           const float* normals,
                           int ntris,
                           const float walkableSlopeAngle,
                           const float texScale) {
    if (!dd) return;
    if (!verts) return;
    if (!tris) return;
    if (!normals) return;

    const float walkableThr = cosf(walkableSlopeAngle / 180.0f * kPiF);

    float uva[2];
    float uvb[2];
    float uvc[2];

    dd->texture(true);

    const unsigned int unwalkable = int2RGBA(192, 128, 0, 255);

    dd->begin(wgpu::PrimitiveTopology::TriangleList);
    for (int i = 0; i < ntris * 3; i += 3) {
        const float* norm = &normals[i];
        unsigned int color;
        auto a = (unsigned char)(220 * (2 + norm[0] + norm[1]) / 4);
        if (norm[1] < walkableThr)
            color = lerpCol(int2RGBA(a, a, a, 255), unwalkable, 64);
        else
            color = int2RGBA(a, a, a, 255);

        const float* va = &verts[tris[i + 0] * 3];
        const float* vb = &verts[tris[i + 1] * 3];
        const float* vc = &verts[tris[i + 2] * 3];

        int ax = 0, ay = 0;
        if (rcAbs(norm[1]) > rcAbs(norm[ax])) ax = 1;
        if (rcAbs(norm[2]) > rcAbs(norm[ax])) ax = 2;
        ax = (1 << ax) & 3;  // +1 mod 3
        ay = (1 << ax) & 3;  // +1 mod 3

        uva[0] = va[ax] * texScale;
        uva[1] = va[ay] * texScale;
        uvb[0] = vb[ax] * texScale;
        uvb[1] = vb[ay] * texScale;
        uvc[0] = vc[ax] * texScale;
        uvc[1] = vc[ay] * texScale;

        dd->vertex(va, color, uva);
        dd->vertex(vb, color, uvb);
        dd->vertex(vc, color, uvc);
    }
    dd->end();

    dd->texture(false);
}

void debugDrawHeightfieldSolid(DebugDraw* dd, const rcHeightfield& hf) {
    if (!dd) return;

    const float* orig = hf.bmin;
    const float cs = hf.cs;
    const float ch = hf.ch;

    const int w = hf.width;
    const int h = hf.height;

    unsigned int fcol[6];
    calcBoxColors(fcol, int2RGBA(255, 255, 255, 255), int2RGBA(255, 255, 255, 255));

    dd->begin(wgpu::PrimitiveTopology::TriangleStrip);  // todo

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            float fx = orig[0] + x * cs;
            float fz = orig[2] + y * cs;
            const rcSpan* s = hf.spans[x + y * w];
            while (s) {
                debugAppendBox(dd, fx, orig[1] + s->smin * ch, fz, fx + cs, orig[1] + s->smax * ch, fz + cs, fcol);
                s = s->next;
            }
        }
    }
    dd->end();
}

void debugDrawHeightfieldWalkable(DebugDraw* dd, const rcHeightfield& hf) {
    if (!dd) return;

    const float* orig = hf.bmin;
    const float cs = hf.cs;
    const float ch = hf.ch;

    const int w = hf.width;
    const int h = hf.height;

    unsigned int fcol[6];
    calcBoxColors(fcol, int2RGBA(255, 255, 255, 255), int2RGBA(217, 217, 217, 255));

    dd->begin(wgpu::PrimitiveTopology::TriangleStrip);  // todo

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            float fx = orig[0] + x * cs;
            float fz = orig[2] + y * cs;
            const rcSpan* s = hf.spans[x + y * w];
            while (s) {
                if (s->area == RC_WALKABLE_AREA)
                    fcol[0] = int2RGBA(64, 128, 160, 255);
                else if (s->area == RC_NULL_AREA)
                    fcol[0] = int2RGBA(64, 64, 64, 255);
                else
                    fcol[0] = multCol(dd->areaToCol(s->area), 200);

                debugAppendBox(dd, fx, orig[1] + s->smin * ch, fz, fx + cs, orig[1] + s->smax * ch, fz + cs, fcol);
                s = s->next;
            }
        }
    }

    dd->end();
}

void debugDrawCompactHeightfieldSolid(DebugDraw* dd, const rcCompactHeightfield& chf) {
    if (!dd) return;

    const float cs = chf.cs;
    const float ch = chf.ch;

    dd->begin(wgpu::PrimitiveTopology::TriangleStrip);  // todo

    for (int y = 0; y < chf.height; ++y) {
        for (int x = 0; x < chf.width; ++x) {
            const float fx = chf.bmin[0] + x * cs;
            const float fz = chf.bmin[2] + y * cs;
            const rcCompactCell& c = chf.cells[x + y * chf.width];

            for (unsigned i = c.index, ni = c.index + c.count; i < ni; ++i) {
                const rcCompactSpan& s = chf.spans[i];

                const unsigned char area = chf.areas[i];
                unsigned int color;
                if (area == RC_WALKABLE_AREA)
                    color = int2RGBA(0, 192, 255, 64);
                else if (area == RC_NULL_AREA)
                    color = int2RGBA(0, 0, 0, 64);
                else
                    color = dd->areaToCol(area);

                const float fy = chf.bmin[1] + (s.y + 1) * ch;
                dd->vertex(fx, fy, fz, color);
                dd->vertex(fx, fy, fz + cs, color);
                dd->vertex(fx + cs, fy, fz + cs, color);
                dd->vertex(fx + cs, fy, fz, color);
            }
        }
    }
    dd->end();
}

void debugDrawCompactHeightfieldRegions(DebugDraw* dd, const rcCompactHeightfield& chf) {
    if (!dd) return;

    const float cs = chf.cs;
    const float ch = chf.ch;

    dd->begin(wgpu::PrimitiveTopology::TriangleStrip);  // todo

    for (int y = 0; y < chf.height; ++y) {
        for (int x = 0; x < chf.width; ++x) {
            const float fx = chf.bmin[0] + x * cs;
            const float fz = chf.bmin[2] + y * cs;
            const rcCompactCell& c = chf.cells[x + y * chf.width];

            for (unsigned i = c.index, ni = c.index + c.count; i < ni; ++i) {
                const rcCompactSpan& s = chf.spans[i];
                const float fy = chf.bmin[1] + (s.y) * ch;
                unsigned int color;
                if (s.reg)
                    color = intToCol(s.reg, 192);
                else
                    color = int2RGBA(0, 0, 0, 64);

                dd->vertex(fx, fy, fz, color);
                dd->vertex(fx, fy, fz + cs, color);
                dd->vertex(fx + cs, fy, fz + cs, color);
                dd->vertex(fx + cs, fy, fz, color);
            }
        }
    }

    dd->end();
}

void debugDrawCompactHeightfieldDistance(DebugDraw* dd, const rcCompactHeightfield& chf) {
    if (!dd) return;
    if (!chf.dist) return;

    const float cs = chf.cs;
    const float ch = chf.ch;

    float maxd = chf.maxDistance;
    if (maxd < 1.0f) maxd = 1;
    const float dscale = 255.0f / maxd;

    dd->begin(wgpu::PrimitiveTopology::TriangleStrip);  // todo

    for (int y = 0; y < chf.height; ++y) {
        for (int x = 0; x < chf.width; ++x) {
            const float fx = chf.bmin[0] + x * cs;
            const float fz = chf.bmin[2] + y * cs;
            const rcCompactCell& c = chf.cells[x + y * chf.width];

            for (unsigned i = c.index, ni = c.index + c.count; i < ni; ++i) {
                const rcCompactSpan& s = chf.spans[i];
                const float fy = chf.bmin[1] + (s.y + 1) * ch;
                const auto cd = (unsigned char)(chf.dist[i] * dscale);
                const unsigned int color = int2RGBA(cd, cd, cd, 255);
                dd->vertex(fx, fy, fz, color);
                dd->vertex(fx, fy, fz + cs, color);
                dd->vertex(fx + cs, fy, fz + cs, color);
                dd->vertex(fx + cs, fy, fz, color);
            }
        }
    }
    dd->end();
}

void debugDrawHeightfieldLayer(DebugDraw* dd, const struct rcHeightfieldLayer& layer, const int idx) {
    const float cs = layer.cs;
    const float ch = layer.ch;
    const int w = layer.width;
    const int h = layer.height;

    unsigned int color = intToCol(idx + 1, 255);

    // Layer bounds
    float bmin[3], bmax[3];
    bmin[0] = layer.bmin[0] + layer.minx * cs;
    bmin[1] = layer.bmin[1];
    bmin[2] = layer.bmin[2] + layer.miny * cs;
    bmax[0] = layer.bmin[0] + (layer.maxx + 1) * cs;
    bmax[1] = layer.bmax[1];
    bmax[2] = layer.bmin[2] + (layer.maxy + 1) * cs;
    debugDrawBoxWire(dd, bmin[0], bmin[1], bmin[2], bmax[0], bmax[1], bmax[2], transCol(color, 128), 2.0f);

    // Layer height
    dd->begin(wgpu::PrimitiveTopology::TriangleStrip);  // todo
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            const int lidx = x + y * w;
            const int lh = (int)layer.heights[lidx];
            if (h == 0xff) continue;
            const unsigned char area = layer.areas[lidx];

            unsigned int col;
            if (area == RC_WALKABLE_AREA)
                col = lerpCol(color, int2RGBA(0, 192, 255, 64), 32);
            else if (area == RC_NULL_AREA)
                col = lerpCol(color, int2RGBA(0, 0, 0, 64), 32);
            else
                col = lerpCol(color, dd->areaToCol(area), 32);

            const float fx = layer.bmin[0] + x * cs;
            const float fy = layer.bmin[1] + (lh + 1) * ch;
            const float fz = layer.bmin[2] + y * cs;

            dd->vertex(fx, fy, fz, col);
            dd->vertex(fx, fy, fz + cs, col);
            dd->vertex(fx + cs, fy, fz + cs, col);
            dd->vertex(fx + cs, fy, fz, col);
        }
    }
    dd->end();

    // Portals
    drawLayerPortals(dd, &layer);
}

void debugDrawHeightfieldLayers(DebugDraw* dd, const struct rcHeightfieldLayerSet& lset) {
    if (!dd) return;
    for (int i = 0; i < lset.nlayers; ++i) debugDrawHeightfieldLayer(dd, lset.layers[i], i);
}

void debugDrawRegionConnections(DebugDraw* dd, const rcContourSet& cset, const float alpha) {
    if (!dd) return;

    const float* orig = cset.bmin;
    const float cs = cset.cs;
    const float ch = cset.ch;

    // Draw centers
    float pos[3], pos2[3];

    unsigned int color = int2RGBA(0, 0, 0, 196);

    dd->begin(wgpu::PrimitiveTopology::LineList, 2.0f);

    for (int i = 0; i < cset.nconts; ++i) {
        const rcContour* cont = &cset.conts[i];
        getContourCenter(cont, orig, cs, ch, pos);
        for (int j = 0; j < cont->nverts; ++j) {
            const int* v = &cont->verts[j * 4];
            if (v[3] == 0 || (unsigned short)v[3] < cont->reg) continue;
            const rcContour* cont2 = findContourFromSet(cset, (unsigned short)v[3]);
            if (cont2) {
                getContourCenter(cont2, orig, cs, ch, pos2);
                debugAppendArc(dd, pos[0], pos[1], pos[2], pos2[0], pos2[1], pos2[2], 0.25f, 0.6f, 0.6f, color);
            }
        }
    }

    dd->end();

    auto a = (unsigned char)(alpha * 255.0f);

    dd->begin(wgpu::PrimitiveTopology::PointList, 7.0f);

    for (int i = 0; i < cset.nconts; ++i) {
        const rcContour* cont = &cset.conts[i];
        unsigned int col = darkenCol(intToCol(cont->reg, a));
        getContourCenter(cont, orig, cs, ch, pos);
        dd->vertex(pos, col);
    }
    dd->end();
}

void debugDrawRawContours(DebugDraw* dd, const rcContourSet& cset, const float alpha) {
    if (!dd) return;

    const float* orig = cset.bmin;
    const float cs = cset.cs;
    const float ch = cset.ch;

    const auto a = (unsigned char)(alpha * 255.0f);

    dd->begin(wgpu::PrimitiveTopology::LineList, 2.0f);

    for (int i = 0; i < cset.nconts; ++i) {
        const rcContour& c = cset.conts[i];
        unsigned int color = intToCol(c.reg, a);

        for (int j = 0; j < c.nrverts; ++j) {
            const int* v = &c.rverts[j * 4];
            float fx = orig[0] + v[0] * cs;
            float fy = orig[1] + (v[1] + 1 + (i & 1)) * ch;
            float fz = orig[2] + v[2] * cs;
            dd->vertex(fx, fy, fz, color);
            if (j > 0) dd->vertex(fx, fy, fz, color);
        }
        // Loop last segment.
        const int* v = &c.rverts[0];
        float fx = orig[0] + v[0] * cs;
        float fy = orig[1] + (v[1] + 1 + (i & 1)) * ch;
        float fz = orig[2] + v[2] * cs;
        dd->vertex(fx, fy, fz, color);
    }
    dd->end();

    dd->begin(wgpu::PrimitiveTopology::PointList, 2.0f);

    for (int i = 0; i < cset.nconts; ++i) {
        const rcContour& c = cset.conts[i];
        unsigned int color = darkenCol(intToCol(c.reg, a));

        for (int j = 0; j < c.nrverts; ++j) {
            const int* v = &c.rverts[j * 4];
            float off = 0;
            unsigned int colv = color;
            if (v[3] & RC_BORDER_VERTEX) {
                colv = int2RGBA(255, 255, 255, a);
                off = ch * 2;
            }

            float fx = orig[0] + v[0] * cs;
            float fy = orig[1] + (v[1] + 1 + (i & 1)) * ch + off;
            float fz = orig[2] + v[2] * cs;
            dd->vertex(fx, fy, fz, colv);
        }
    }
    dd->end();
}

void debugDrawContours(DebugDraw* dd, const rcContourSet& cset, const float alpha) {
    if (!dd) return;

    const float* orig = cset.bmin;
    const float cs = cset.cs;
    const float ch = cset.ch;

    const auto a = (unsigned char)(alpha * 255.0f);

    dd->begin(wgpu::PrimitiveTopology::LineList, 2.5f);

    for (int i = 0; i < cset.nconts; ++i) {
        const rcContour& c = cset.conts[i];
        if (!c.nverts) continue;
        const unsigned int color = intToCol(c.reg, a);
        const unsigned int bcolor = lerpCol(color, int2RGBA(255, 255, 255, a), 128);
        for (int j = 0, k = c.nverts - 1; j < c.nverts; k = j++) {
            const int* va = &c.verts[k * 4];
            const int* vb = &c.verts[j * 4];
            unsigned int col = (va[3] & RC_AREA_BORDER) ? bcolor : color;
            float fx, fy, fz;
            fx = orig[0] + va[0] * cs;
            fy = orig[1] + (va[1] + 1 + (i & 1)) * ch;
            fz = orig[2] + va[2] * cs;
            dd->vertex(fx, fy, fz, col);
            fx = orig[0] + vb[0] * cs;
            fy = orig[1] + (vb[1] + 1 + (i & 1)) * ch;
            fz = orig[2] + vb[2] * cs;
            dd->vertex(fx, fy, fz, col);
        }
    }
    dd->end();

    dd->begin(wgpu::PrimitiveTopology::PointList, 3.0f);

    for (int i = 0; i < cset.nconts; ++i) {
        const rcContour& c = cset.conts[i];
        unsigned int color = darkenCol(intToCol(c.reg, a));
        for (int j = 0; j < c.nverts; ++j) {
            const int* v = &c.verts[j * 4];
            float off = 0;
            unsigned int colv = color;
            if (v[3] & RC_BORDER_VERTEX) {
                colv = int2RGBA(255, 255, 255, a);
                off = ch * 2;
            }

            float fx = orig[0] + v[0] * cs;
            float fy = orig[1] + (v[1] + 1 + (i & 1)) * ch + off;
            float fz = orig[2] + v[2] * cs;
            dd->vertex(fx, fy, fz, colv);
        }
    }
    dd->end();
}

void debugDrawPolyMesh(DebugDraw* dd, const struct rcPolyMesh& mesh) {
    if (!dd) return;

    const int nvp = mesh.nvp;
    const float cs = mesh.cs;
    const float ch = mesh.ch;
    const float* orig = mesh.bmin;

    dd->begin(wgpu::PrimitiveTopology::TriangleList);

    for (int i = 0; i < mesh.npolys; ++i) {
        const unsigned short* p = &mesh.polys[i * nvp * 2];
        const unsigned char area = mesh.areas[i];

        unsigned int color;
        if (area == RC_WALKABLE_AREA)
            color = int2RGBA(0, 192, 255, 64);
        else if (area == RC_NULL_AREA)
            color = int2RGBA(0, 0, 0, 64);
        else
            color = dd->areaToCol(area);

        unsigned short vi[3];
        for (int j = 2; j < nvp; ++j) {
            if (p[j] == RC_MESH_NULL_IDX) break;
            vi[0] = p[0];
            vi[1] = p[j - 1];
            vi[2] = p[j];
            for (unsigned short k : vi) {
                const unsigned short* v = &mesh.verts[k * 3];
                const float x = orig[0] + v[0] * cs;
                const float y = orig[1] + (v[1] + 1) * ch;
                const float z = orig[2] + v[2] * cs;
                dd->vertex(x, y, z, color);
            }
        }
    }
    dd->end();

    // Draw neighbours edges
    const unsigned int coln = int2RGBA(0, 48, 64, 32);
    dd->begin(wgpu::PrimitiveTopology::LineList, 1.5f);
    for (int i = 0; i < mesh.npolys; ++i) {
        const unsigned short* p = &mesh.polys[i * nvp * 2];
        for (int j = 0; j < nvp; ++j) {
            if (p[j] == RC_MESH_NULL_IDX) break;
            if (p[nvp + j] & 0x8000) continue;
            const int nj = (j + 1 >= nvp || p[j + 1] == RC_MESH_NULL_IDX) ? 0 : j + 1;
            const int vi[2] = {p[j], p[nj]};

            for (int k : vi) {
                const unsigned short* v = &mesh.verts[k * 3];
                const float x = orig[0] + v[0] * cs;
                const float y = orig[1] + (v[1] + 1) * ch + 0.1f;
                const float z = orig[2] + v[2] * cs;
                dd->vertex(x, y, z, coln);
            }
        }
    }
    dd->end();

    // Draw boundary edges
    const unsigned int colb = int2RGBA(0, 48, 64, 220);
    dd->begin(wgpu::PrimitiveTopology::LineList, 2.5f);
    for (int i = 0; i < mesh.npolys; ++i) {
        const unsigned short* p = &mesh.polys[i * nvp * 2];
        for (int j = 0; j < nvp; ++j) {
            if (p[j] == RC_MESH_NULL_IDX) break;
            if ((p[nvp + j] & 0x8000) == 0) continue;
            const int nj = (j + 1 >= nvp || p[j + 1] == RC_MESH_NULL_IDX) ? 0 : j + 1;
            const int vi[2] = {p[j], p[nj]};

            unsigned int col = colb;
            if ((p[nvp + j] & 0xf) != 0xf) col = int2RGBA(255, 255, 255, 128);
            for (int k : vi) {
                const unsigned short* v = &mesh.verts[k * 3];
                const float x = orig[0] + v[0] * cs;
                const float y = orig[1] + (v[1] + 1) * ch + 0.1f;
                const float z = orig[2] + v[2] * cs;
                dd->vertex(x, y, z, col);
            }
        }
    }
    dd->end();

    dd->begin(wgpu::PrimitiveTopology::PointList, 3.0f);
    const unsigned int colv = int2RGBA(0, 0, 0, 220);
    for (int i = 0; i < mesh.nverts; ++i) {
        const unsigned short* v = &mesh.verts[i * 3];
        const float x = orig[0] + v[0] * cs;
        const float y = orig[1] + (v[1] + 1) * ch + 0.1f;
        const float z = orig[2] + v[2] * cs;
        dd->vertex(x, y, z, colv);
    }
    dd->end();
}

void debugDrawPolyMeshDetail(DebugDraw* dd, const struct rcPolyMeshDetail& dmesh) {
    if (!dd) return;

    dd->begin(wgpu::PrimitiveTopology::TriangleList);

    for (int i = 0; i < dmesh.nmeshes; ++i) {
        const unsigned int* m = &dmesh.meshes[i * 4];
        const unsigned int bverts = m[0];
        const unsigned int btris = m[2];
        const int ntris = (int)m[3];
        const float* verts = &dmesh.verts[bverts * 3];
        const unsigned char* tris = &dmesh.tris[btris * 4];

        unsigned int color = intToCol(i, 192);

        for (int j = 0; j < ntris; ++j) {
            dd->vertex(&verts[tris[j * 4 + 0] * 3], color);
            dd->vertex(&verts[tris[j * 4 + 1] * 3], color);
            dd->vertex(&verts[tris[j * 4 + 2] * 3], color);
        }
    }
    dd->end();

    // Internal edges.
    dd->begin(wgpu::PrimitiveTopology::LineList, 1.0f);
    const unsigned int coli = int2RGBA(0, 0, 0, 64);
    for (int i = 0; i < dmesh.nmeshes; ++i) {
        const unsigned int* m = &dmesh.meshes[i * 4];
        const unsigned int bverts = m[0];
        const unsigned int btris = m[2];
        const int ntris = (int)m[3];
        const float* verts = &dmesh.verts[bverts * 3];
        const unsigned char* tris = &dmesh.tris[btris * 4];

        for (int j = 0; j < ntris; ++j) {
            const unsigned char* t = &tris[j * 4];
            for (int k = 0, kp = 2; k < 3; kp = k++) {
                unsigned char ef = (t[3] >> (kp * 2)) & 0x3;
                if (ef == 0) {
                    // Internal edge
                    if (t[kp] < t[k]) {
                        dd->vertex(&verts[t[kp] * 3], coli);
                        dd->vertex(&verts[t[k] * 3], coli);
                    }
                }
            }
        }
    }
    dd->end();

    // External edges.
    dd->begin(wgpu::PrimitiveTopology::LineList, 2.0f);
    const unsigned int cole = int2RGBA(0, 0, 0, 64);
    for (int i = 0; i < dmesh.nmeshes; ++i) {
        const unsigned int* m = &dmesh.meshes[i * 4];
        const unsigned int bverts = m[0];
        const unsigned int btris = m[2];
        const int ntris = (int)m[3];
        const float* verts = &dmesh.verts[bverts * 3];
        const unsigned char* tris = &dmesh.tris[btris * 4];

        for (int j = 0; j < ntris; ++j) {
            const unsigned char* t = &tris[j * 4];
            for (int k = 0, kp = 2; k < 3; kp = k++) {
                unsigned char ef = (t[3] >> (kp * 2)) & 0x3;
                if (ef != 0) {
                    // Ext edge
                    dd->vertex(&verts[t[kp] * 3], cole);
                    dd->vertex(&verts[t[k] * 3], cole);
                }
            }
        }
    }
    dd->end();

    dd->begin(wgpu::PrimitiveTopology::PointList, 3.0f);
    const unsigned int colv = int2RGBA(0, 0, 0, 64);
    for (int i = 0; i < dmesh.nmeshes; ++i) {
        const unsigned int* m = &dmesh.meshes[i * 4];
        const unsigned int bverts = m[0];
        const int nverts = (int)m[1];
        const float* verts = &dmesh.verts[bverts * 3];
        for (int j = 0; j < nverts; ++j) dd->vertex(&verts[j * 3], colv);
    }
    dd->end();
}
}  // namespace vox::debug