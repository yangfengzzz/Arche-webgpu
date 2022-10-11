//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "asset_pipeline/importer/import2ozz.h"

// Mocks OzzImporter, so it can be used to dump default and reference
// configurations.
class DumpConverter : public vox::animation::offline::OzzImporter {
public:
    DumpConverter() = default;
    ~DumpConverter() override = default;

private:
    bool Load(const char*) override { return true; }

    bool Import(vox::animation::offline::RawSkeleton*, const NodeType&) override { return true; }

    AnimationNames GetAnimationNames() override { return {}; }

    bool Import(const char*, const vox::animation::Skeleton&, float, vox::animation::offline::RawAnimation*) override {
        return true;
    }

    NodeProperties GetNodeProperties(const char*) override { return {}; }

    bool Import(const char*,
                const char*,
                const char*,
                NodeProperty::Type,
                float,
                vox::animation::offline::RawFloatTrack*) override {
        return true;
    }

    bool Import(const char*,
                const char*,
                const char*,
                NodeProperty::Type,
                float,
                vox::animation::offline::RawFloat2Track*) override {
        return true;
    }

    bool Import(const char*,
                const char*,
                const char*,
                NodeProperty::Type,
                float,
                vox::animation::offline::RawFloat3Track*) override {
        return true;
    }
    bool Import(const char*,
                const char*,
                const char*,
                NodeProperty::Type,
                float,
                vox::animation::offline::RawFloat4Track*) override {
        return true;
    }
};

int main(int _argc, const char** _argv) {
    DumpConverter converter;
    return converter(_argc, _argv);
}
