//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.navigation/file_list.h"

#include <algorithm>
#ifdef WIN32
#include <io.h>
#else
#include <dirent.h>

#include <cstring>
#endif

namespace vox {

void scanDirectoryAppend(const std::string& path, const std::string& ext, std::vector<std::string>& filelist) {
#ifdef WIN32
    string pathWithExt = path + "/*" + ext;

    _finddata_t dir;
    intptr_t fh = _findfirst(pathWithExt.c_str(), &dir);
    if (fh == -1L) {
        return;
    }

    do {
        filelist.push_back(dir.name);
    } while (_findnext(fh, &dir) == 0);
    _findclose(fh);
#else
    dirent* current = nullptr;
    DIR* dp = opendir(path.c_str());
    if (!dp) {
        return;
    }

    int extLen = strlen(ext.c_str());
    while ((current = readdir(dp)) != nullptr) {
        int len = strlen(current->d_name);
        if (len > extLen && strncmp(current->d_name + len - extLen, ext.c_str(), extLen) == 0) {
            filelist.emplace_back(current->d_name);
        }
    }
    closedir(dp);
#endif

    // Sort the list of files alphabetically.
    std::sort(filelist.begin(), filelist.end());
}

void scanDirectory(const std::string& path, const std::string& ext, std::vector<std::string>& filelist) {
    filelist.clear();
    scanDirectoryAppend(path, ext, filelist);
}

}  // namespace vox