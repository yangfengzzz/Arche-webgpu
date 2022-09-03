//  Copyright (c) 2022 Feng Yang
//
//  I am making my contributions/submissions to this project solely in my
//  personal capacity and am not conveying any rights to any intellectual
//  property of any third parties.

#pragma once

#include <algorithm>
#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>
#include <vector>

#include "vox.render/error.h"
#include "vox.render/graphing/graph_node.h"

namespace vox::graphing {
struct Edge {
    size_t id;
    size_t from;
    size_t to;
    nlohmann::json options;
    Edge(size_t id, size_t s, size_t t) : id(id), from(s), to(t){};
};

/**
 * @brief Graph is an implementation of an adjacency list graph. The nodes are created from a variadic function and
 * their implementation is defined by the given NodeType
 */
class Graph {
public:
    explicit Graph(const char *name);

    static const size_t node_not_found_ = 0;

    /**
     * @brief Create a new style
     *
     * @param style_name the group name
     * @param color the hex color of the group
     */
    void NewStyle(const std::string &style_name, const std::string &color);

    /**
     * @brief Create a node object
     *
     * @param title of node
     * @param style corresponds to the key used when using new_type(style, color)
     * @param data json data to be displayed with node
     * @return size_t id of node
     */
    size_t CreateNode(const char *title = "Node", const char *style = nullptr, const nlohmann::json &data = {}) {
        size_t id = NewId();
        nodes_[id] = std::make_unique<Node>(id, title, style, data);
        return id;
    }

    /**
     * @brief Find a node from a reference
     * 		  If the node does not exist then the reference will be node_not_found
     *
     * @param name of node
     * @return size_t if of node
     */
    size_t FindRef(std::string &name);

    /**
     * @brief Add a readable reference to a node
     *
     * @param name of reference
     * @param id of node
     */
    void AddRef(std::string &name, size_t id);

    /**
     * @brief Remove a readable reference to a node
     *
     * @param name of ref
     */
    void RemoveRef(std::string &name);

    /**
     * @brief Add an edge to the graph
     * @param from source node
     * @param to target node
     */
    void AddEdge(size_t from, size_t to);

    /**
     * @brief Remove Edge from the graph
     * @param from source node
     * @param to target node
     */
    void RemoveEdge(size_t from, size_t to);

    /**
     * @brief Dump the graphs state to json in the given file name
     * @param file_name to dump to
     */
    bool DumpToFile(const std::string &file_name);

    size_t NewId();

private:
    size_t next_id_ = 1;
    std::vector<Edge> adj_;
    std::unordered_map<size_t, std::unique_ptr<Node>> nodes_;
    std::unordered_map<std::string, size_t> refs_;
    std::string name_;
    std::unordered_map<std::string, std::string> style_colors_;
};

}  // namespace vox::graphing
