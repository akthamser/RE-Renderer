#pragma once
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include "RayTracer.h" 

namespace Re_Renderer {

  
    struct AABB {
        glm::vec3 bmin = glm::vec3(1e30f);
        glm::vec3 bmax = glm::vec3(-1e30f);

        void grow(glm::vec3 p) {
            bmin = glm::min(bmin, p);
            bmax = glm::max(bmax, p);
        }

        void grow(const AABB& b) {
            if (b.bmin.x > b.bmax.x) return;
            bmin = glm::min(bmin, b.bmin);
            bmax = glm::max(bmax, b.bmax);
        }

        float area() const {
            glm::vec3 e = bmax - bmin;
            return e.x * e.y + e.y * e.z + e.z * e.x;
        }
    };

    // The Node format the GPU expects
    struct GPUBVHNode {
        glm::vec3 bmin;
        int leftChild;  // +Index = Child Node, -Index = Triangle Start
        glm::vec3 bmax;
        int triCount;   // 0 = Internal Node, >0 = Leaf Node
    };

    struct BVHBuilder {
        // Input Data helper
        struct TriRef { int index; glm::vec3 center; };
        std::vector<TriRef> triRefs;
        const std::vector<GPUTriangle>* sourceTriangles;

        // Output Data
        std::vector<GPUBVHNode> nodes;
        std::vector<int> finalTriIndices;

        void Build(const std::vector<GPUTriangle>& triangles) {
            sourceTriangles = &triangles;
            nodes.clear();
            finalTriIndices.clear();
            triRefs.clear();

            if (triangles.empty()) return;

            // 1. Pre-calculate centroids for sorting
            for (int i = 0; i < triangles.size(); i++) {
                glm::vec3 c = (triangles[i].v0 + triangles[i].v1 + triangles[i].v2) / 3.0f;
                triRefs.push_back({ i, c });
            }

            // 2. Create Root Node
            nodes.emplace_back();
            SplitNode(0, 0, triRefs.size());

            std::cout << "Custom BVH Built: " << nodes.size() << " nodes for " << triangles.size() << " tris." << std::endl;
        }

    private:
        void SplitNode(int nodeIdx, int start, int count) {
            GPUBVHNode& node = nodes[nodeIdx];

            // 1. Calculate Bounding Box for this node
            AABB bounds;
            for (int i = 0; i < count; i++) {
                const auto& tri = (*sourceTriangles)[triRefs[start + i].index];
                bounds.grow(tri.v0); bounds.grow(tri.v1); bounds.grow(tri.v2);
            }
            node.bmin = bounds.bmin;
            node.bmax = bounds.bmax;

            // 2. LEAF CONDITION
            // Stop if 4 or fewer triangles
            if (count <= 4) {
                node.leftChild = -((int)finalTriIndices.size());
                node.triCount = count;
                for (int i = 0; i < count; i++) {
                    finalTriIndices.push_back(triRefs[start + i].index);
                }
                return;
            }

            // 3. SAH SPLIT LOGIC
            int bestAxis = -1;
            float bestPos = 0;
            float bestCost = 1e30f;

            // Check all 3 axes
            for (int axis = 0; axis < 3; axis++) {
                float boundsMin = node.bmin[axis];
                float boundsMax = node.bmax[axis];
                if (boundsMax - boundsMin < 0.0001f) continue;

                // Test 12 split planes per axis
                for (int i = 1; i < 12; i++) {
                    float splitPos = boundsMin + (boundsMax - boundsMin) * (i / 12.0f);

                    int countLeft = 0, countRight = 0;
                    AABB boxLeft, boxRight;

                    for (int k = 0; k < count; k++) {
                        const auto& tri = (*sourceTriangles)[triRefs[start + k].index];
                        float center = (tri.v0[axis] + tri.v1[axis] + tri.v2[axis]) / 3.0f;
                        if (center < splitPos) {
                            countLeft++;
                            boxLeft.grow(tri.v0); boxLeft.grow(tri.v1); boxLeft.grow(tri.v2);
                        }
                        else {
                            countRight++;
                            boxRight.grow(tri.v0); boxRight.grow(tri.v1); boxRight.grow(tri.v2);
                        }
                    }

                    if (countLeft == 0 || countRight == 0) continue;

                    // SAH Cost = SurfaceArea * TriCount
                    float cost = countLeft * boxLeft.area() + countRight * boxRight.area();
                    if (cost < bestCost) {
                        bestCost = cost;
                        bestAxis = axis;
                        bestPos = splitPos;
                    }
                }
            }

            // Fallback: If split failed (e.g. all centers stacked), make leaf
            if (bestAxis == -1) {
                node.leftChild = -((int)finalTriIndices.size());
                node.triCount = count;
                for (int i = 0; i < count; i++) finalTriIndices.push_back(triRefs[start + i].index);
                return;
            }

            // 4. PERFORM SORT
            int mid = start;
            for (int i = start; i < start + count; i++) {
                if (triRefs[i].center[bestAxis] < bestPos) {
                    std::swap(triRefs[i], triRefs[mid]);
                    mid++;
                }
            }
            

            // 5. RECURSE
            node.triCount = 0;
            int leftChildIdx = nodes.size();
            nodes.emplace_back(); // Left
            int rightChildIdx = nodes.size();
            nodes.emplace_back(); // Right

            nodes[nodeIdx].leftChild = leftChildIdx;

            SplitNode(leftChildIdx, start, mid - start);
            SplitNode(rightChildIdx, mid, (start + count) - mid);
        }
    };
}