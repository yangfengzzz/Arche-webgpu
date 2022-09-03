//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/stats/stats_provider.h"

namespace vox {
// Default graphing values for stats. May be overridden by individual providers.
std::map<StatIndex, StatGraphData> StatsProvider::default_graph_map_{
        // StatIndex    Name shown in graph    Format    Scale     Fixed_max Max_value
        {StatIndex::FRAME_TIMES, {"Frame Times", "{:3.1f} ms", 1000.0f}},
        {StatIndex::CPU_CYCLES, {"CPU Cycles", "{:4.1f} M/s", float(1e-6)}},
        {StatIndex::CPU_INSTRUCTIONS, {"CPU Instructions", "{:4.1f} M/s", float(1e-6)}},
        {StatIndex::CPU_CACHE_MISS_RATIO, {"Cache Miss Ratio", "{:3.1f}%", 100.0f, true, 100.0f}},
        {StatIndex::CPU_BRANCH_MISS_RATIO, {"Branch Miss Ratio", "{:3.1f}%", 100.0f, true, 100.0f}},
        {StatIndex::CPU_L_1_ACCESSES, {"CPU L1 Accesses", "{:4.1f} M/s", float(1e-6)}},
        {StatIndex::CPU_INSTR_RETIRED, {"CPU Instructions Retired", "{:4.1f} M/s", float(1e-6)}},
        {StatIndex::CPU_L_2_ACCESSES, {"CPU L2 Accesses", "{:4.1f} M/s", float(1e-6)}},
        {StatIndex::CPU_L_3_ACCESSES, {"CPU L3 Accesses", "{:4.1f} M/s", float(1e-6)}},
        {StatIndex::CPU_BUS_READS, {"CPU Bus Read Beats", "{:4.1f} M/s", float(1e-6)}},
        {StatIndex::CPU_BUS_WRITES, {"CPU Bus Write Beats", "{:4.1f} M/s", float(1e-6)}},
        {StatIndex::CPU_MEM_READS, {"CPU Memory Read Instructions", "{:4.1f} M/s", float(1e-6)}},
        {StatIndex::CPU_MEM_WRITES, {"CPU Memory Write Instructions", "{:4.1f} M/s", float(1e-6)}},
        {StatIndex::CPU_ASE_SPEC, {"CPU Speculatively Exec. SIMD Instructions", "{:4.1f} M/s", float(1e-6)}},
        {StatIndex::CPU_VFP_SPEC, {"CPU Speculatively Exec. FP Instructions", "{:4.1f} M/s", float(1e-6)}},
        {StatIndex::CPU_CRYPTO_SPEC, {"CPU Speculatively Exec. Crypto Instructions", "{:4.1f} M/s", float(1e-6)}},

        {StatIndex::GPU_CYCLES, {"GPU Cycles", "{:4.1f} M/s", float(1e-6)}},
        {StatIndex::GPU_VERTEX_CYCLES, {"Vertex Cycles", "{:4.1f} M/s", float(1e-6)}},
        {StatIndex::GPU_LOAD_STORE_CYCLES, {"Load Store Cycles", "{:4.0f} k/s", float(1e-6)}},
        {StatIndex::GPU_TILES, {"Tiles", "{:4.1f} k/s", float(1e-3)}},
        {StatIndex::GPU_KILLED_TILES, {"Tiles killed by CRC match", "{:4.1f} k/s", float(1e-3)}},
        {StatIndex::GPU_FRAGMENT_JOBS, {"Fragment Jobs", "{:4.0f}/s"}},
        {StatIndex::GPU_FRAGMENT_CYCLES, {"Fragment Cycles", "{:4.1f} M/s", float(1e-6)}},
        {StatIndex::GPU_TEX_CYCLES, {"Shader Texture Cycles", "{:4.0f} k/s", float(1e-3)}},
        {StatIndex::GPU_EXT_READS, {"External Reads", "{:4.1f} M/s", float(1e-6)}},
        {StatIndex::GPU_EXT_WRITES, {"External Writes", "{:4.1f} M/s", float(1e-6)}},
        {StatIndex::GPU_EXT_READ_STALLS, {"External Read Stalls", "{:4.1f} M/s", float(1e-6)}},
        {StatIndex::GPU_EXT_WRITE_STALLS, {"External Write Stalls", "{:4.1f} M/s", float(1e-6)}},
        {StatIndex::GPU_EXT_READ_BYTES, {"External Read Bytes", "{:4.1f} MiB/s", 1.0f / (1024.0f * 1024.0f)}},
        {StatIndex::GPU_EXT_WRITE_BYTES, {"External Write Bytes", "{:4.1f} MiB/s", 1.0f / (1024.0f * 1024.0f)}},
};

// Static
const StatGraphData &StatsProvider::DefaultGraphData(StatIndex index) { return default_graph_map_.at(index); }

}  // namespace vox
