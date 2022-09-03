//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#include "vox.render/graphing/framework_graph.h"

#include "vox.render/strings.h"

namespace vox::graphing::framework_graph {
bool Generate(RenderContext &context) {
    Graph graph("Framework");
    graph.NewStyle("Core", "#00BCD4");
    graph.NewStyle("Rendering", "#4CAF50");
    graph.NewStyle("Framework", "#FFC107");
    graph.NewStyle("Vulkan", "#F44336");

    size_t device_id = DeviceNode(graph, context.GetDevice());

    // Device
    auto &device = context.GetDevice();

    auto &resource_cache = device.GetResourceCache();
    size_t resource_cache_id = ResourceCacheNode(graph, resource_cache);
    graph.AddEdge(device_id, resource_cache_id);

    const auto &resource_cache_state = resource_cache.GetInternalState();

    auto it_pipeline_layouts = resource_cache_state.pipeline_layouts.begin();
    while (it_pipeline_layouts != resource_cache_state.pipeline_layouts.end()) {
        size_t pipeline_layouts_id = PipelineLayoutNode(graph, it_pipeline_layouts->second);
        graph.AddEdge(resource_cache_id, pipeline_layouts_id);

        auto &shader_modules = it_pipeline_layouts->second.GetShaderModules();
        for (const auto *shader_module : shader_modules) {
            size_t shader_modules_id = ShaderModuleNode(graph, *shader_module);
            graph.AddEdge(pipeline_layouts_id, shader_modules_id);

            const auto &resources = shader_module->GetResources();
            for (const auto &resource : resources) {
                size_t resource_id = ShaderResourceNode(graph, resource);
                graph.AddEdge(shader_modules_id, resource_id);
            }
        }

        it_pipeline_layouts++;
    }

    auto it_descriptor_set_layouts = resource_cache_state.descriptor_set_layouts.begin();
    while (it_descriptor_set_layouts != resource_cache_state.descriptor_set_layouts.end()) {
        size_t descriptor_set_layouts_id = DescriptorSetLayoutNode(graph, it_descriptor_set_layouts->second);
        graph.AddEdge(resource_cache_id, descriptor_set_layouts_id);
        it_descriptor_set_layouts++;
    }
    auto it_graphics_pipelines = resource_cache_state.graphics_pipelines.begin();
    while (it_graphics_pipelines != resource_cache_state.graphics_pipelines.end()) {
        size_t pipeline_layout =
                PipelineLayoutNode(graph, it_graphics_pipelines->second.GetState().GetPipelineLayout());
        graph.AddEdge(resource_cache_id, pipeline_layout);

        size_t graphics_pipelines_id = GraphicsPipelineNode(graph, it_graphics_pipelines->second);
        graph.AddEdge(pipeline_layout, graphics_pipelines_id);

        size_t graphics_pipelines_state_id = PipelineStateNode(graph, it_graphics_pipelines->second.GetState());
        graph.AddEdge(graphics_pipelines_id, graphics_pipelines_state_id);

        size_t render_pass = RenderPassNode(graph, *it_graphics_pipelines->second.GetState().GetRenderPass());
        graph.AddEdge(graphics_pipelines_state_id, render_pass);

        size_t specialization_constant_state = SpecializationConstantStateNode(
                graph, it_graphics_pipelines->second.GetState().GetSpecializationConstantState());
        graph.AddEdge(graphics_pipelines_state_id, specialization_constant_state);

        size_t vertex_input_state =
                VertexInputStateNode(graph, it_graphics_pipelines->second.GetState().GetVertexInputState());
        graph.AddEdge(graphics_pipelines_state_id, vertex_input_state);

        size_t input_assembly_state =
                InputAssemblyStateNode(graph, it_graphics_pipelines->second.GetState().GetInputAssemblyState());
        graph.AddEdge(graphics_pipelines_state_id, input_assembly_state);

        size_t rasterization_state =
                RasterizationStateNode(graph, it_graphics_pipelines->second.GetState().GetRasterizationState());
        graph.AddEdge(graphics_pipelines_state_id, rasterization_state);

        size_t viewport_state = ViewportStateNode(graph, it_graphics_pipelines->second.GetState().GetViewportState());
        graph.AddEdge(graphics_pipelines_state_id, viewport_state);

        size_t multisample_state =
                MultisampleStateNode(graph, it_graphics_pipelines->second.GetState().GetMultisampleState());
        graph.AddEdge(graphics_pipelines_state_id, multisample_state);

        size_t depth_stencil_state =
                DepthStencilStateNode(graph, it_graphics_pipelines->second.GetState().GetDepthStencilState());
        graph.AddEdge(graphics_pipelines_state_id, depth_stencil_state);

        size_t color_blend_state =
                ColorBlendStateNode(graph, it_graphics_pipelines->second.GetState().GetColorBlendState());
        graph.AddEdge(graphics_pipelines_state_id, color_blend_state);
        it_graphics_pipelines++;
    }
    auto it_compute_pipelines = resource_cache_state.compute_pipelines.begin();
    while (it_compute_pipelines != resource_cache_state.compute_pipelines.end()) {
        size_t compute_pipelines_id = ComputePipelineNode(graph, it_compute_pipelines->second);
        graph.AddEdge(resource_cache_id, compute_pipelines_id);
        it_compute_pipelines++;
    }

    auto it_framebuffers = resource_cache_state.framebuffers.begin();
    while (it_framebuffers != resource_cache_state.framebuffers.end()) {
        size_t framebuffers_id = FramebufferNode(graph, it_framebuffers->second);
        graph.AddEdge(resource_cache_id, framebuffers_id);
        it_framebuffers++;
    }

    size_t render_context_id = RenderContextNode(graph, context);
    graph.AddEdge(device_id, render_context_id);
    size_t swapchain_id = SwapchainNode(graph, context.GetSwapchain());

    for (auto image : context.GetSwapchain().GetImages()) {
        size_t vkimage_id = CreateVkImage(graph, image);
        graph.AddEdge(vkimage_id, swapchain_id);
    }

    const auto &const_context = context;
    const auto &frames = context.GetRenderFrames();
    uint32_t i = 0;
    for (auto &frame : frames) {
        std::string label = "Render Frame";
        if (i == const_context.GetActiveFrameIndex()) {
            label = "Last " + label;
        }
        i++;

        size_t frame_id = RenderFrameNode(graph, frame, label);
        graph.AddEdge(render_context_id, frame_id);

        size_t semaphore_pool_id = SemaphorePoolNode(graph, frame->GetSemaphorePool());
        size_t fence_pool_id = FencePoolNode(graph, frame->GetFencePool());
        size_t render_target_id = RenderTargetNode(graph, frame->GetRenderTargetConst());
        graph.AddEdge(frame_id, semaphore_pool_id);
        graph.AddEdge(frame_id, fence_pool_id);
        graph.AddEdge(frame_id, render_target_id);

        for (const auto &view : frame->GetRenderTargetConst().GetViews()) {
            size_t image_view_id = ImageViewNode(graph, view);
            const auto &image = view.GetImage();
            size_t image_id = ImageNode(graph, image);

            graph.AddEdge(render_target_id, image_view_id);
            graph.AddEdge(image_view_id, image_id);

            size_t vkimage_id = CreateVkImage(graph, image.GetHandle());
            graph.AddEdge(image_id, vkimage_id);

            size_t vkimageview_id = CreateVkImageView(graph, view.GetHandle());
            graph.AddEdge(image_view_id, vkimageview_id);
        }
    }

    return graph.DumpToFile("framework.json");
}

size_t CreateVkImage(Graph &graph, const VkImage &image) { return CreateVkNode(graph, "VkImage", image); }

size_t CreateVkImageView(Graph &graph, const VkImageView &image) { return CreateVkNode(graph, "VkImageView", image); }

size_t DeviceNode(Graph &graph, const Device &device) {
    auto pd_props = device.GetGpu().GetProperties();

    nlohmann::json device_properties = {{"deviceID", pd_props.deviceID},
                                        {"deviceName", pd_props.deviceName},
                                        {"deviceType", ToString(pd_props.deviceType)},
                                        {"driverVersion", pd_props.driverVersion},
                                        {"apiVersion", pd_props.apiVersion},
                                        {"vendorID", pd_props.vendorID}};

    nlohmann::json data = {{"VkPhysicalDeviceProperties", device_properties}};

    return graph.CreateNode("Device", "Core", data);
}

size_t RenderContextNode(Graph &graph, const RenderContext &context) {
    auto surface = context.GetSurfaceExtent();

    nlohmann::json data = {{"VkExtent2D", {{"width", surface.width}, {"height", surface.height}}},
                           {"active_frame_index", context.GetActiveFrameIndex()}};

    return graph.CreateNode("Render Context", "Rendering", data);
}

size_t SemaphorePoolNode(Graph &graph, const SemaphorePool &semaphore_pool) {
    nlohmann::json data = {{"active_semaphore_count", semaphore_pool.GetActiveSemaphoreCount()}};
    return graph.CreateNode("Semaphore Pool", "Framework", data);
}

size_t FencePoolNode(Graph &graph, const FencePool &fence_pool) { return graph.CreateNode("Fence Pool", "Framework"); }

size_t RenderFrameNode(Graph &graph, const std::unique_ptr<RenderFrame> &frame, const std::string &label) {
    return graph.CreateNode(label.c_str(), "Rendering");
}

size_t RenderTargetNode(Graph &graph, const RenderTarget &render_target) {
    VkExtent2D surface = render_target.GetExtent();

    nlohmann::json data = {{"VkExtent2D", {{"width", surface.width}, {"height", surface.height}}},
                           {"ImageView_count", render_target.GetViews().size()},
                           {"Attachment_count", render_target.GetAttachments().size()},
                           {"output_attachment_count", render_target.GetOutputAttachments().size()}};

    return graph.CreateNode("Render Target", "Rendering", data);
}

size_t ImageViewNode(Graph &graph, const core::ImageView &image_view) {
    auto subresource_range = image_view.GetSubresourceRange();
    auto subresource_layers = image_view.GetSubresourceLayers();

    nlohmann::json data = {{"VkFormat", ToString(image_view.GetFormat())},
                           {"VkImageSubresourceRange",
                            {{"VkImageAspectFlags", ImageAspectToString(subresource_range.aspectMask)},
                             {"base_mip_level", subresource_range.baseMipLevel},
                             {"level_count", subresource_range.levelCount},
                             {"base_array_layer", subresource_range.baseArrayLayer},
                             {"layer_count", subresource_range.layerCount}}},
                           {"VkImageSubresourceLayers",
                            {{"VkImageAspectFlags", ImageAspectToString(subresource_layers.aspectMask)},
                             {"mip_level", subresource_layers.mipLevel},
                             {"base_array_layer", subresource_layers.baseArrayLayer},
                             {"layer_count", subresource_layers.layerCount}}}};

    return graph.CreateNode("Image View", "Core", data);
}

size_t ImageNode(Graph &graph, const core::Image &image) {
    std::string result;
    bool append = false;
    auto flags = image.GetUsage();
    if (flags & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT) {
        result += append ? " / " : "";
        result += "COLOR";
        append = true;
    }
    if (flags & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT) {
        result += append ? " / " : "";
        result += "DEPTH STENCIL";
        append = true;
    }
    auto subresource = image.GetSubresource();

    nlohmann::json data = {{"VkExtent2D", {{"width", image.GetExtent().width}, {"height", image.GetExtent().height}}},
                           {"VkFormat", ToString(image.GetFormat())},
                           {"VkImageUsageFlags", ImageUsageToString(image.GetUsage())},
                           {"VkSampleCountFlagBits", ToString(image.GetSampleCount())},
                           {"VkImageTiling", ToString(image.GetTiling())},
                           {"VkImageType", ToString(image.GetType())},
                           {"VkSubresource",
                            {{"VkImageAspectFlags", ImageAspectToString(subresource.aspectMask)},
                             {"mip_level", subresource.mipLevel},
                             {"array_layer", subresource.arrayLayer}}}};

    return graph.CreateNode(result.c_str(), "Core", data);
}

size_t SwapchainNode(Graph &graph, const Swapchain &swapchain) {
    auto surface = swapchain.GetExtent();
    auto format = swapchain.GetFormat();
    auto image_count = swapchain.GetImages().size();

    nlohmann::json data = {{"VkExtent2D", {{"width", surface.width}, {"height", surface.height}}},
                           {"VkFormat", ToString(format)},
                           {"image_count", image_count},
                           {"VkSurfaceTransformFlagBitsKHR", ToString(swapchain.GetTransform())},
                           {"VkPresentModeKHR", ToString(swapchain.GetPresentMode())},
                           {"VkImageUsageFlags", ImageUsageToString(swapchain.GetUsage())}};

    return graph.CreateNode("Swapchain", "Core", data);
}

size_t ResourceCacheNode(Graph &graph, const ResourceCache &resource_cache) {
    return graph.CreateNode("Resource Cache", "Core");
}

size_t DescriptorSetLayoutNode(Graph &graph, const DescriptorSetLayout &descriptor_set_layout) {
    std::vector<nlohmann::json> bindings;

    auto it = descriptor_set_layout.GetBindings().begin();
    while (it != descriptor_set_layout.GetBindings().end()) {
        bindings.push_back({{"binding", it->binding},
                            {"descriptorCount", it->descriptorCount},
                            {"stageFlags", ToString(it->stageFlags)}});
        it++;
    }

    nlohmann::json data = {{"handle", Node::HandleToUintptrT(descriptor_set_layout.GetHandle())},
                           {"VkDescriptorSetLayoutBinding", bindings}};

    return graph.CreateNode("Descriptor Set Layout", "Core", data);
}

size_t FramebufferNode(Graph &graph, const Framebuffer &framebuffer) {
    nlohmann::json data = {{"handle", Node::HandleToUintptrT(framebuffer.GetHandle())}};

    return graph.CreateNode("Frame Buffer", "Core", data);
}

size_t RenderPassNode(Graph &graph, const RenderPass &render_pass) {
    nlohmann::json data = {{"handle", Node::HandleToUintptrT(render_pass.GetHandle())}};

    return graph.CreateNode("Render Pass", "Rendering", data);
}

size_t ShaderModuleNode(Graph &graph, const ShaderModule &shader_module) {
    std::string stage = ShaderStageToString(shader_module.GetStage());
    std::transform(stage.begin(), stage.end(), stage.begin(), [](unsigned char c) { return std::tolower(c); });

    nlohmann::json data = {{"stage", stage},
                           {"infoLog", shader_module.GetInfoLog()},
                           {"entry_point", shader_module.GetEntryPoint()},
                           {"id", shader_module.GetId()}};

    stage = "Shader Module: " + stage;

    return graph.CreateNode(stage.c_str(), "Rendering", data);
}

size_t ShaderResourceNode(Graph &graph, const ShaderResource &shader_resource) {
    std::string label = fmt::format("{}: {}", ToString(shader_resource.type), shader_resource.name);

    nlohmann::json data = {{"ShaderResourceType", ToString(shader_resource.type)},
                           {"VkShaderStageFlags", ToString(shader_resource.stages)},
                           {"set", shader_resource.set},
                           {"binding", shader_resource.binding},
                           {"location", shader_resource.location},
                           {"input_attachment_index", shader_resource.input_attachment_index},
                           {"vec_size", shader_resource.vec_size},
                           {"columns", shader_resource.columns},
                           {"array_size", shader_resource.array_size},
                           {"offset", shader_resource.offset},
                           {"size", shader_resource.size},
                           {"constant_id", shader_resource.constant_id},
                           {"mode", shader_resource.mode},
                           {"name", shader_resource.name}};

    return graph.CreateNode(label.c_str(), "Rendering", data);
}

size_t PipelineLayoutNode(Graph &graph, const PipelineLayout &pipeline_layout) {
    nlohmann::json data = {{"handle", Node::HandleToUintptrT(pipeline_layout.GetHandle())}};

    return graph.CreateNode("Pipeline Layout", "Core", data);
}

size_t GraphicsPipelineNode(Graph &graph, const GraphicsPipeline &graphics_pipeline) {
    nlohmann::json data = {{"handle", Node::HandleToUintptrT(graphics_pipeline.GetHandle())}};

    return graph.CreateNode("Graphics Pipeline", "Core", data);
}

size_t ComputePipelineNode(Graph &graph, const ComputePipeline &compute_pipeline) {
    nlohmann::json data = {{"handle", Node::HandleToUintptrT(compute_pipeline.GetHandle())}};

    return graph.CreateNode("Compute Pipeline", "Core", data);
}

size_t PipelineStateNode(Graph &graph, const PipelineState &pipeline_state) {
    nlohmann::json data = {{"subpass_index", pipeline_state.GetSubpassIndex()}};

    return graph.CreateNode("Pipeline State", "Core", data);
}

size_t DescriptorSetNode(Graph &graph, const DescriptorSet &descriptor_set) {
    nlohmann::json data = {{"handle", Node::HandleToUintptrT(descriptor_set.GetHandle())}};

    return graph.CreateNode("Descriptor Set", "Core", data);
}

size_t SpecializationConstantStateNode(Graph &graph, const SpecializationConstantState &specialization_constant_state) {
    nlohmann::json data = {};

    const auto &state = specialization_constant_state.GetSpecializationConstantState();
    auto it = state.begin();
    while (it != state.end()) {
        std::stringstream str;
        str << it->first;
        data.push_back({str.str(), it->second});
    }

    return graph.CreateNode("Specialization Constant state", "Core", data);
}

size_t VertexInputStateNode(Graph &graph, const VertexInputState &vertex_input_state) {
    std::vector<nlohmann::json> bindings;

    for (auto binding : vertex_input_state.bindings) {
        bindings.push_back({{"binding", binding.binding},
                            {"stride", binding.stride},
                            {"VkVertexInputRate", ToString(binding.inputRate)}});
    }

    std::vector<nlohmann::json> binding;

    for (auto attribute : vertex_input_state.attributes) {
        binding.push_back({{"location", attribute.location},
                           {"binding", attribute.binding},
                           {"format", ToString(attribute.format)},
                           {"offset", attribute.offset}});
    }

    nlohmann::json data = {{"VkVertexInputBindingDescription", bindings},
                           {"VkVertexInputAttributeDescription", binding}};

    return graph.CreateNode("Vertex Input State", "Core", data);
}

size_t InputAssemblyStateNode(Graph &graph, const InputAssemblyState &input_assembly_state) {
    nlohmann::json data = {
            {"VkPrimitiveTopology", ToString(input_assembly_state.topology)},
            {"primitive_restart_enabled", ToStringVkBool(input_assembly_state.primitive_restart_enable)}};

    return graph.CreateNode("Input Assembly State", "Core", data);
}

size_t RasterizationStateNode(Graph &graph, const RasterizationState &rasterization_state) {
    nlohmann::json data = {
            {"depth_clamp_enable", ToStringVkBool(rasterization_state.depth_clamp_enable)},
            {"rasterizer_discard_enable", ToStringVkBool(rasterization_state.rasterizer_discard_enable)},
            {"polygon_mode", ToString(rasterization_state.polygon_mode)},
            {"cull_mode", CullModeToString(rasterization_state.cull_mode)},
            {"front_face", ToString(rasterization_state.front_face)},
            {"depth_bias_enable", ToStringVkBool(rasterization_state.depth_bias_enable)},
    };

    return graph.CreateNode("Rasterization State", "Core", data);
}

size_t ViewportStateNode(Graph &graph, const ViewportState &viewport_state) {
    nlohmann::json data = {{"viewport_count", viewport_state.viewport_count},
                           {"scissor_count", viewport_state.scissor_count}};

    return graph.CreateNode("Viewport State", "Core", data);
}

size_t MultisampleStateNode(Graph &graph, const MultisampleState &multisample_state) {
    nlohmann::json data = {{"rasterization_samples", multisample_state.rasterization_samples},
                           {"sample_shading_enable", ToStringVkBool(multisample_state.sample_shading_enable)},
                           {"min_sample_shading", multisample_state.min_sample_shading},
                           {"sample_mask", multisample_state.sample_mask},
                           {"alpha_to_coverage_enable", ToStringVkBool(multisample_state.alpha_to_coverage_enable)},
                           {"alpha_to_one_enable", ToStringVkBool(multisample_state.alpha_to_one_enable)}};

    return graph.CreateNode("Multisample State", "Core", data);
}

size_t DepthStencilStateNode(Graph &graph, const DepthStencilState &depth_stencil_state) {
    nlohmann::json data = {{"depth_test_enable", ToStringVkBool(depth_stencil_state.depth_test_enable)},
                           {"depth_write_enable", ToStringVkBool(depth_stencil_state.depth_write_enable)},
                           {"depth_compare_op", ToString(depth_stencil_state.depth_compare_op)},
                           {"depth_bounds_test_enable", ToStringVkBool(depth_stencil_state.depth_bounds_test_enable)},
                           {"stencil_test_enable", ToStringVkBool(depth_stencil_state.stencil_test_enable)},
                           {"front",
                            {{"fail_op", ToString(depth_stencil_state.front.fail_op)},
                             {"pass_op", ToString(depth_stencil_state.front.pass_op)},
                             {"depth_fail_op", ToString(depth_stencil_state.front.depth_fail_op)},
                             {"compare_op", ToString(depth_stencil_state.front.compare_op)}}},
                           {"back",
                            {{"fail_op", ToString(depth_stencil_state.back.fail_op)},
                             {"pass_op", ToString(depth_stencil_state.back.pass_op)},
                             {"depth_fail_op", ToString(depth_stencil_state.back.depth_fail_op)},
                             {"compare_op", ToString(depth_stencil_state.back.compare_op)}}}};

    return graph.CreateNode("Depth Stencil State", "Core", data);
}

size_t ColorBlendStateNode(Graph &graph, const ColorBlendState &color_blend_state) {
    nlohmann::json data = {{"logic_op_enable", ToStringVkBool(color_blend_state.logic_op_enable)},
                           {"logic_op", ToString(color_blend_state.logic_op)}};

    return graph.CreateNode("Color Blend State", "Core", data);
}

size_t ColorBlendAttachmentStateNode(Graph &graph, const ColorBlendAttachmentState &state) {
    nlohmann::json data = {{"blend_enable", ToStringVkBool(state.blend_enable)},
                           {"src_color_blend_factor", ToString(state.src_color_blend_factor)},
                           {"dst_color_blend_factor", ToString(state.dst_color_blend_factor)},
                           {"color_blend_op", ToString(state.color_blend_op)},
                           {"src_alpha_blend_factor", ToString(state.src_alpha_blend_factor)},
                           {"dst_alpha_blend_factor", ToString(state.dst_alpha_blend_factor)},
                           {"alpha_blend_op", ToString(state.alpha_blend_op)},
                           {"color_write_mask", ColorComponentToString(state.color_write_mask)}};

    return graph.CreateNode("Color Blend Attachment State", "Core", data);
}

}  // namespace vox::graphing::framework_graph
