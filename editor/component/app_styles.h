//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#ifndef app_styles_hpp
#define app_styles_hpp

#include <string>

void loadDefaultStyle();

void loadDarkCoolStyle();

void loadLightOrngeStyle();

void loadBlackAndWhite();

void loadMayaStyle();

void showStyleEditor(bool *pOpen);

bool loadThemeFromFile(std::string filename);

bool loadThemeFromStr(std::string data);

std::string getStyleData();

#endif /* app_styles_hpp */
