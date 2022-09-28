//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.base/containers/string_archive.h"

#include "vox.base/io/archive.h"

namespace vox::io {
void Extern<string>::Save(OArchive& _archive, const string* _values, size_t _count) {
    for (size_t i = 0; i < _count; i++) {
        const vox::string& string = _values[i];

        // Get size excluding null terminating character.
        auto size = static_cast<uint32_t>(string.size());
        _archive << size;
        _archive << vox::io::MakeArray(string.c_str(), size);
    }
}

void Extern<string>::Load(IArchive& _archive, string* _values, size_t _count, uint32_t _version) {
    (void)_version;
    for (size_t i = 0; i < _count; i++) {
        vox::string& string = _values[i];

        // Ensure an existing string is reset.
        string.clear();

        uint32_t size;
        _archive >> size;
        string.reserve(size);

        // Prepares temporary buffer used for reading.
        char buffer[128];
        for (size_t to_read = size; to_read != 0;) {
            // Read from the archive to the local temporary buffer.
            const size_t to_read_this_loop = std::min(to_read, VOX_ARRAY_SIZE(buffer));
            _archive >> vox::io::MakeArray(buffer, to_read_this_loop);
            to_read -= to_read_this_loop;

            // Append to the string.
            string.append(buffer, to_read_this_loop);
        }
    }
}
}  // namespace vox::io
