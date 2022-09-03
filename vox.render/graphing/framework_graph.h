//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include "vox.render/core/device.h"
#include "vox.render/core/image_view.h"
#include "vox.render/core/swapchain.h"
#include "vox.render/fence_pool.h"
#include "vox.render/graphing/graph.h"
#include "vox.render/graphing/graph_node.h"
#include "vox.render/rendering/render_context.h"
#include "vox.render/rendering/render_frame.h"
#include "vox.render/rendering/render_target.h"
#include "vox.render/resource_cache.h"
#include "vox.render/semaphore_pool.h"
#include "vox.render/shader/shader_module.h"

namespace vox::graphing::framework_graph {
bool Generate(RenderContext &context);

template <typename T>
size_t CreateVkNode(Graph &graph, const char *name, const T &handle) {
    std::stringstream temp;
    temp << Node::HandleToUintptrT(handle);
    std::string tag = "VK_HANDLE-" + temp.str();

    size_t id = graph.FindRef(tag);

    if (id == Graph::node_not_found_) {
        id = graph.CreateNode(name, "Vulkan", nlohmann::json{{name, Node::HandleToUintptrT(handle)}});
        graph.AddRef(tag, id);
    }

    return id;
}

size_t CreateVkImage(Graph &graph, const VkImage &image);

size_t CreateVkImageView(Graph &graph, const VkImageView &image);

size_t DeviceNode(Graph &graph, const Device &device);
size_t RenderContextNode(Graph &graph, const RenderContext &context);
size_t SemaphorePoolNode(Graph &graph, const SemaphorePool &semaphore_pool);
size_t FencePoolNode(Graph &graph, const FencePool &fence_pool);
size_t RenderFrameNode(Graph &graph, const std::unique_ptr<RenderFrame> &frame, const std::string &label);
size_t RenderTargetNode(Graph &graph, const RenderTarget &render_target);
size_t ImageViewNode(Graph &graph, const core::ImageView &image_view);
size_t ImageNode(Graph &graph, const core::Image &image);
size_t SwapchainNode(Graph &graph, const Swapchain &swapchain);
size_t ResourceCacheNode(Graph &graph, const ResourceCache &resource_cache);
size_t DescriptorSetLayoutNode(Graph &graph, const DescriptorSetLayout &descriptor_set_layout);
size_t FramebufferNode(Graph &graph, const Framebuffer &framebuffer);
size_t RenderPassNode(Graph &graph, const RenderPass &render_pass);
size_t ShaderModuleNode(Graph &graph, const ShaderModule &shader_module);
size_t ShaderResourceNode(Graph &graph, const ShaderResource &shader_resource);
size_t PipelineLayoutNode(Graph &graph, const PipelineLayout &pipeline_layout);
size_t GraphicsPipelineNode(Graph &graph, const GraphicsPipeline &graphics_pipeline);
size_t ComputePipelineNode(Graph &graph, const ComputePipeline &compute_pipeline);
size_t PipelineStateNode(Graph &graph, const PipelineState &pipeline_state);
size_t DescriptorSetNode(Graph &graph, const DescriptorSet &descriptor_set);
size_t SpecializationConstantStateNode(Graph &graph, const SpecializationConstantState &specialization_constant_state);
size_t VertexInputStateNode(Graph &graph, const VertexInputState &vertex_input_state);
size_t InputAssemblyStateNode(Graph &graph, const InputAssemblyState &input_assembly_state);
size_t RasterizationStateNode(Graph &graph, const RasterizationState &rasterization_state);
size_t ViewportStateNode(Graph &graph, const ViewportState &viewport_state);
size_t MultisampleStateNode(Graph &graph, const MultisampleState &multisample_state);
size_t DepthStencilStateNode(Graph &graph, const DepthStencilState &depth_stencil_state);
size_t ColorBlendStateNode(Graph &graph, const ColorBlendState &color_blend_state);
size_t ColorBlendAttachmentStateNode(Graph &graph, const ColorBlendAttachmentState &state);

}  // namespace vox::graphing::framework_graph
