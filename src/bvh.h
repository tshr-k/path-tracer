#pragma once

#include <cstddef>
#include <memory>
#include <ostream>
#include <vector>
#include <array>
#include <algorithm>
#include <cmath>

#include "aabb.h"
#include "instance.h"
#include "mesh.h"


constexpr std::size_t null_node = std::size_t(-1);

struct BVHNodeChildren {
    BVHNodeChildren(std::size_t left, std::size_t right) : left{left}, right{right} {}

    std::size_t left = null_node;
    std::size_t right = null_node;
};


template <typename LeafData>  // Either std::size_t (an index to an Instance in Scene) or TriangleRange
struct BVHNode {
    BVHNode() : bbox{}, children{null_node, null_node}, is_leaf{false} {}
    BVHNode(AABB bbox, BVHNodeChildren children) : bbox{bbox}, children{children}, is_leaf{false} {}
    BVHNode(AABB bbox, LeafData leaf_data) : bbox{bbox}, leaf_data{leaf_data}, is_leaf{true} {}

    AABB bbox;
    union {
        struct BVHNodeChildren children;
        LeafData leaf_data;
    };
    bool is_leaf;  // Since the union doesn't keep track of the active member
};


template <typename LeafData>
class BVH {  // Not Hittable because it is essentially just a helper class for TLAS and BLAS
private:
    std::vector<BVHNode<LeafData>> nodes_;

    std::size_t build_recursive(const std::vector<LeafData>& leaf_items,
                                const std::vector<AABB>& leaf_bboxes,
                                std::vector<std::size_t>& build_indices,
                                std::size_t start,
                                std::size_t end) {
        if (start >= end) {
            throw std::runtime_error("Invalid BVH build range");
        }

        AABB current_node_bbox;
        for (std::size_t i = start; i < end; ++i) {
            current_node_bbox = AABB::merge(current_node_bbox, leaf_bboxes[build_indices[i]]);
        }

        std::size_t current_node_idx = nodes_.size();
        nodes_.emplace_back();  // Reserve space for current node
        
        if (end - start == 1) {  // Leaf node
            const std::size_t leaf_index = build_indices[start];
            nodes_[current_node_idx] = BVHNode<LeafData>{current_node_bbox, leaf_items[leaf_index]};
        } else {  // Interior node
            std::size_t longest_axis = current_node_bbox.longest_axis();
            std::size_t mid = start + (end - start) / 2;
            std::nth_element(build_indices.begin() + start,
                             build_indices.begin() + mid,
                             build_indices.begin() + end,
                [&leaf_bboxes, longest_axis](std::size_t a, std::size_t b) {
                    const auto& bbox_a = leaf_bboxes[a];
                    const auto& bbox_b = leaf_bboxes[b];
                    const double centroid_a = bbox_a[longest_axis].min + bbox_a[longest_axis].max;
                    const double centroid_b = bbox_b[longest_axis].min + bbox_b[longest_axis].max;
                    return centroid_a < centroid_b;
                }  // Compare bounding box centroids along the longest axis
            );
         
            std::size_t left_child = build_recursive(leaf_items, leaf_bboxes, build_indices, start, mid);
            std::size_t right_child = build_recursive(leaf_items, leaf_bboxes, build_indices, mid, end);
            
            BVHNode<LeafData>& current_node = nodes_[current_node_idx];  // Refetch since nodes_ may have been reallocated during recursive calls
            current_node.bbox = current_node_bbox;
            current_node.children = BVHNodeChildren{left_child, right_child};
            current_node.is_leaf = false;
        }

        return current_node_idx;
    }

public:
    void build(const std::vector<LeafData>& leaf_data_array, const std::vector<AABB>& leaf_bboxes) {
        nodes_.clear();
        if (leaf_data_array.empty()) return;

        if (leaf_data_array.size() != leaf_bboxes.size()) {
            throw std::runtime_error("BVH build error: leaf data and bbox arrays must be the same size");
        }

        // Initialize build indices to [0, 1, 2, ..., N-1], which will be permuted during the recursive build
        std::vector<std::size_t> build_indices(leaf_data_array.size());
        for (std::size_t i = 0; i < build_indices.size(); ++i) {
            build_indices[i] = i;
        }

        build_recursive(leaf_data_array, leaf_bboxes, build_indices, 0, build_indices.size());
    }

    template <typename LeafHitFunc>
    constexpr bool hit(const Ray& ray, HitRecord& hit_record, double t_min, LeafHitFunc leaf_hit_func) const {
        bool hit_anything = false;

        std::size_t cur_node_idx = 0;
        
        std::array<std::size_t, 64> stack;
        std::size_t stack_size = 0;
        stack[stack_size++] = cur_node_idx;  // Root node index

        while (stack_size > 0) {
            cur_node_idx = stack[--stack_size];  // Pop the top node index from the stack

            const BVHNode<LeafData>& cur_node = nodes_[cur_node_idx];
            
            if (cur_node.bbox.hit(ray, math::Interval{t_min, hit_record.t})) {  // Bounding box hit
                if (cur_node.is_leaf) {  // Leaf node
                    if (leaf_hit_func(ray, hit_record, t_min, cur_node.leaf_data)) {  // Actual hit
                        hit_anything = true;
                    }
                } else {  // Interior node
                    if (cur_node.children.left != null_node) stack[stack_size++] = cur_node.children.left;
                    if (cur_node.children.right != null_node) stack[stack_size++] = cur_node.children.right;
                }
            }
        }

        return hit_anything;
    }

    constexpr AABB bounding_box() const {
        return nodes_[0].bbox;  // Root node
    }

    std::size_t num_nodes() const {
        return nodes_.size();
    }

    std::ostream& print(std::ostream& os, std::size_t node_idx = 0, int depth = 0) const {
        const BVHNode<LeafData>& node = nodes_[node_idx];
        os << std::string(depth * 2, ' ') << "Node " << node_idx << ": " << node.bbox;
        if (node.is_leaf) {
            os << " [Leaf]";
        }
        os << "\n";
        if (!node.is_leaf) {
            if (node.children.left != null_node) print(os, node.children.left, depth + 1);
            if (node.children.right != null_node) print(os, node.children.right, depth + 1);
        }
        return os;
    }
};