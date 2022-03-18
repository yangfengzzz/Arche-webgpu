//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef editor_utils_hpp
#define editor_utils_hpp

#include <string>

namespace vox {
namespace editor {
std::string uChar2Hex(unsigned char c);

struct Hash {
    Hash() {
    }

    Hash(unsigned char *d, int l) {
        length = l;
        data = new unsigned char[length];
        std::memcpy(data, d, length);
    }

    ~Hash() {
        //delete[] data;
    }

    std::string toString() {
        std::string hash;

        for (int i = 0; i < length; i++) {
            hash += uChar2Hex(data[i]);
        }

        return hash;
    }

    int length = 0;
    unsigned char *data;
};


#define MAX(x, y) (x > y ? x : y)


#ifdef TERR3D_WIN32
std::wstring s2ws(const std::string &s);
#endif

void openURL(std::string url);

std::string showSaveFileDialog(std::string ext = ".terr3d");

std::string openfilename();

std::string showOpenFileDialog(std::string ext = "*.glsl");

std::string readShaderSourceFile(std::string path, bool *result);

std::string getExecutablePath();

std::string getExecutableDir();

std::string generateId(uint32_t length);

std::string fetchURL(std::string baseURL, std::string path);

char *uChar2Char(unsigned char *data, int length);

bool fileExists(std::string path, bool writeAccess = false);

bool pathExist(const std::string &s);

bool isNetWorkConnected();

char *readBinaryFile(std::string path, int *size, uint32_t sizeToLoad = -1);

char *readBinaryFile(std::string path, uint32_t sizeToLoad = -1);

Hash MD5File(std::string path);

void downloadFile(std::string baseURL, std::string urlPath, std::string path, int size = -1);

void saveToFile(std::string filename, std::string content = "");

void log(const char *log);

void log(std::string log);

#ifdef TERR3D_WIN32
#include <windows.h>
void regSet(HKEY hkeyHive, const char *pszVar, const char *pszValue);
#endif

void accocFileType();

void mkDir(std::string path);

void copyFileData(std::string source, std::string destination);

bool deleteFileT(std::string path);

bool isKeyDown(int key);

bool isMouseButtonDown(int button);

void showMessageBox(std::string message, std::string title = "Info");

void toggleSystemConsole();

}
}
#endif /* editor_utils_hpp */
