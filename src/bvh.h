#pragma once

#include <cstddef>
#include <vector>
#include <stack>  // TODO: Replace with fixed-size array

#include "aabb.h"
#include "instance.h"
#include "mesh.h"


struct BVHNodeChildren {
    std::size_t left;
    std::size_t right;
};


template <typename LeafData>
struct BVHNode {
    AABB bbox;
    union {
        struct BVHNodeChildren children;
        LeafData leaf_data;
    };
    bool is_leaf;  // Since the union doesn't keep track of the active member
};


template <typename LeafData>  // Either Instance* or TriangleRange
class BVH : public Hittable {
private:
    std::vector<LeafData> nodes_;

public:
    constexpr bool hit(const Ray& ray, HitRecord& hit_record, const math::Interval t_range) const override {
        bool hit_anything = false;
        
        std::stack<std::size_t> stack;
        std::size_t cur_node_idx = 0;
        stack.push(cur_node_idx);

        while (!stack.empty()) {
            cur_node_idx = stack.pop();
            const BVHNode<LeafData>& cur_node = nodes_[cur_node_idx];
            
            if (!cur_node.bbox.hit(ray, hit_record)) continue;

            if (cur_node.is_leaf) {  // Leaf node
                if constexpr (std::is_same_v<LeafData, Instance*>) {
                    // TLAS
                    if (cur_node.)
                }
            }
            else {  // Interior node
                if (cur_node.children.left >= 0) stack.push(cur_node.children.left);
                if (cur_node.children.right >= 0) stack.push(cur_node.children.right);
            }
        }
    }

    constexpr AABB bounding_box() const override {
        return nodes_[0].bbox;  // Root node
    }
};