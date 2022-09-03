//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <sys/stat.h>

#include "vox.base/string_utils.h"
#ifdef WIN32
#include <direct.h>
#define NOMINMAX
#include <commdlg.h>

#include "windows.h"
#else
#include <dirent.h>
#include <unistd.h>
#endif

#ifndef S_ISDIR
#define S_ISDIR(mode) (((mode)&S_IFMT) == S_IFDIR)
#endif

#ifndef S_ISREG
#define S_ISREG(mode) (((mode)&S_IFMT) == S_IFREG)
#endif

namespace vox::utilities {
std::string normalizePath(const std::string &path) {
    if (path.empty()) return path;
    std::string result = path;
    std::replace(result.begin(), result.end(), '\\', '/');
    std::vector<std::string> tokens;
    tokenize(result, tokens, "/");
    unsigned int index = 0;
    while (index < tokens.size()) {
        if ((tokens[index] == "..") && (index > 0)) {
            tokens.erase(tokens.begin() + index - 1, tokens.begin() + index + 1);
            index -= 2;
        }
        index++;
    }
    result = "";
    if (path[0] == '/') result = "/";
    result = result + tokens[0];
    for (unsigned int i = 1; i < tokens.size(); i++) result.append("/" + tokens[i]);

    return result;
}

std::string getFilePath(const std::string &path) {
    std::string npath = normalizePath(path);

    std::string result = npath;
    size_t i = result.rfind('.', result.length());
    if (i != std::string::npos) {
        result = result.substr(0, i);
    }
    size_t p1 = result.rfind('\\', result.length());
    size_t p2 = result.rfind('/', result.length());
    if ((p1 != std::string::npos) && (p2 != std::string::npos))
        result = result.substr(0, std::max(p1, p2));
    else if (p1 != std::string::npos)
        result = result.substr(0, p1);
    else if (p2 != std::string::npos)
        result = result.substr(0, p2);
    return result;
}

std::string getFileName(const std::string &path) {
    std::string npath = normalizePath(path);

    std::string result = npath;
    size_t i = result.rfind('.', result.length());
    if (i != std::string::npos) {
        result = result.substr(0, i);
    }
    size_t p1 = result.rfind('\\', result.length());
    size_t p2 = result.rfind('/', result.length());
    if ((p1 != std::string::npos) && (p2 != std::string::npos))
        result = result.substr(std::max(p1, p2) + 1, result.length());
    else if (p1 != std::string::npos)
        result = result.substr(p1 + 1, result.length());
    else if (p2 != std::string::npos)
        result = result.substr(p2 + 1, result.length());
    return result;
}

std::string getFileNameWithExt(const std::string &path) {
    std::string npath = normalizePath(path);

    std::string result = npath;
    size_t p1 = result.rfind('\\', result.length());
    size_t p2 = result.rfind('/', result.length());
    if ((p1 != std::string::npos) && (p2 != std::string::npos))
        result = result.substr(std::max(p1, p2) + 1, result.length());
    else if (p1 != std::string::npos)
        result = result.substr(p1 + 1, result.length());
    else if (p2 != std::string::npos)
        result = result.substr(p2 + 1, result.length());
    return result;
}

std::string getFileExt(const std::string &path) {
    std::string npath = normalizePath(path);

    std::string result = npath;
    size_t i = result.rfind('.', result.length());
    if (i != std::string::npos) {
        result = result.substr(i + 1, result.length());
    }
    return result;
}

bool isRelativePath(const std::string &path) {
    std::string npath = normalizePath(path);

    // Windows
    size_t i = npath.find(':');
    if (i != std::string::npos)
        return false;
    else if (npath[0] == '/')
        return false;
    return true;
}

int makeDir(const std::string &path) {
    std::string npath = normalizePath(path);

    struct stat st {};
    int status = 0;

    if (stat(path.c_str(), &st) != 0) {
#if WIN32
        status = _mkdir(path.c_str());
#else
        status = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
#endif
        if (status != 0 && errno != EEXIST) status = -1;
    } else if (!(S_IFDIR & st.st_mode)) {
        errno = ENOTDIR;
        status = -1;
    }

    return status;
}

/** Make all subdirectories.
 */
int makeDirs(const std::string &path) {
    char *pp;
    char *sp;
    int status;
#ifdef WIN32
    char *copyOfPath = _strdup(path.c_str());
#else
    char *copyOfPath = strdup(path.c_str());
#endif

    status = 0;
    pp = copyOfPath;
    pp = pp + 3;  // Cut away Drive:
    while ((status == 0) && (((sp = strchr(pp, '/')) != nullptr) || ((sp = strchr(pp, '\\')) != nullptr))) {
        if (sp != pp) {
            *sp = '\0';
            status = makeDir(copyOfPath);
            *sp = '/';
        }
        pp = sp + 1;
    }
    if (status == 0) status = makeDir(path);
    free(copyOfPath);
    return status;
}

bool fileExists(const std::string &fileName) {
    if (FILE *file = fopen(fileName.c_str(), "r")) {
        fclose(file);
        return true;
    } else
        return false;
}

std::string getProgramPath() {
    char buffer[1000];
#ifdef WIN32
    GetModuleFileName(NULL, buffer, 1000);
#else
    char szTmp[32];
    sprintf(szTmp, "/proc/%d/exe", getpid());
    int bytes = std::min((int)readlink(szTmp, buffer, 1000), 999);
    buffer[bytes] = '\0';
#endif
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");
    return std::string(buffer).substr(0, pos);
}

bool copyFile(const std::string &source, const std::string &dest) {
    const size_t bufferSize = 8192;
    char buffer[bufferSize];
    size_t size;

    FILE *sourceFile = fopen(source.c_str(), "rb");
    FILE *destFile = fopen(dest.c_str(), "wb");

    if ((sourceFile == nullptr) || (destFile == nullptr)) return false;

    while ((size = fread(buffer, 1, bufferSize, sourceFile))) {
        fwrite(buffer, 1, size, destFile);
    }

    fclose(sourceFile);
    fclose(destFile);

    return true;
}

bool isFile(const std::string &path) {
    struct stat st {};
    if (!stat(path.c_str(), &st)) return S_ISREG(st.st_mode);
    return false;
}

bool isDirectory(const std::string &path) {
    struct stat st {};
    if (!stat(path.c_str(), &st)) return S_ISDIR(st.st_mode);
    return false;
}

bool getFilesInDirectory(const std::string &path, std::vector<std::string> &res) {
#ifdef WIN32
    std::string p = path + "\\*";
    WIN32_FIND_DATA data;
    HANDLE hFind = FindFirstFile(p.c_str(), &data);
    if (hFind != INVALID_HANDLE_VALUE) {
        do {
            res.push_back(data.cFileName);
        } while (FindNextFile(hFind, &data) != 0);
        FindClose(hFind);
        return true;
    }
    return false;
#else
    DIR *dir = opendir(path.c_str());
    if (dir != nullptr) {
        struct dirent *dp;
        while ((dp = readdir(dir)) != nullptr) res.emplace_back(dp->d_name);
        closedir(dir);
        return true;
    }
    return false;
#endif
}

}  // namespace vox::utilities