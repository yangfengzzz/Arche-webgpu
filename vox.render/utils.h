//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/platform/filesystem.h"
#include "vox.render/rendering/render_context.h"

namespace vox {
/**
 * @brief Extracts the extension from an uri
 * @param uri An uniform Resource Identifier
 * @return The extension
 */
std::string GetExtension(const std::string &uri);

/**
 * @param name String to convert to snake case
 * @return a snake case version of the string
 */
std::string ToSnakeCase(const std::string &name);

class CommandBuffer;

/**
 * @brief Takes a screenshot of the app by writing the swapchain image to file (slow function)
 * @param render_context The RenderContext to use
 * @param filename The name of the file to save the output to
 */
void Screenshot(RenderContext &render_context, const std::string &filename);

}  // namespace vox
