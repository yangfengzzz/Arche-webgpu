//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.animation/offline/tools/import2vox.h"

// Mocks OzzImporter so it can be used to dump default and reference
// configurations.
class DumpConverter : public vox::animation::offline::OzzImporter {
public:
    DumpConverter() {}
    ~DumpConverter() {}

private:
    virtual bool Load(const char*) { return true; }

    virtual bool Import(vox::animation::offline::RawSkeleton*, const NodeType&) { return true; }

    virtual AnimationNames GetAnimationNames() { return AnimationNames(); }

    virtual bool Import(const char*, const vox::animation::Skeleton&, float, vox::animation::offline::RawAnimation*) {
        return true;
    }

    virtual NodeProperties GetNodeProperties(const char*) { return NodeProperties(); }

    virtual bool Import(
            const char*, const char*, const char*, NodeProperty::Type, float, vox::animation::offline::RawFloatTrack*) {
        return true;
    }

    virtual bool Import(const char*,
                        const char*,
                        const char*,
                        NodeProperty::Type,
                        float,
                        vox::animation::offline::RawFloat2Track*) {
        return true;
    }

    virtual bool Import(const char*,
                        const char*,
                        const char*,
                        NodeProperty::Type,
                        float,
                        vox::animation::offline::RawFloat3Track*) {
        return true;
    }
    virtual bool Import(const char*,
                        const char*,
                        const char*,
                        NodeProperty::Type,
                        float,
                        vox::animation::offline::RawFloat4Track*) {
        return true;
    }
};

int main(int _argc, const char** _argv) {
    DumpConverter converter;
    return converter(_argc, _argv);
}
