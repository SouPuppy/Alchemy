#include "alchemy/alchemy.hpp"
#include "linearBVH.h"

Point3D centroid(Triangle tri) {
    return (tri[0] + tri[1] + tri[2]) / 3;
}

namespace ALCHEMY {

uint commonPrefix(uint m1, uint m2) {
    uint diff = m1 ^ m2;
    int prefixLength = __builtin_clz(diff);
    uint mask = ~((1U << (32 - prefixLength)) - 1);
    return m1 & mask;
}

void Object::build_LinearBVH() {
    int N = faces.size();
    Sort_idx            .   resize(N);
    mortonCode          .   resize(N);
    AABBs               .   resize(N);

    Sorted_mortonCode   .   resize(N * 2 - 1);
    Sorted_AABBs        .   resize(N * 2 - 1);
    node                .   resize(N * 2 - 1);
    visit               .   resize(N * 2 - 1);

    for (int i = 0; i < N; i++)     Sort_idx[i] = i;
    // * Get mortonCode
    get_limit();
    double scale, double_scale;
    scale           = std::max({limit[0], limit[1], limit[2]});
    double_scale    = scale * 2;

    for (int i = 0; i < N; i++) {
        mortonCode[i] = morton3D((centroid(get_triangle(i)) + scale) / (double_scale));
    }

    // * Sort triangles by mortonCode and get Sorted Morton Code
    // Sort
    std::sort(Sort_idx.begin(), Sort_idx.end(), [&](int a, int b) { return mortonCode[a] < mortonCode[b]; });
    // Get Sorted mortonCode
    // TODO shold do in parrallel
    for (int i = 0; i < N; i++) {
        Sorted_mortonCode[i + N - 1] = mortonCode[id(i)];
    }
    
    // * Calculate leaf node's AABBs
    auto triangle_AABB = [&](Triangle tri) -> AABB {
        AABB result = AABB(tri[0], tri[0]);
        for (int i = 1; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                result[0][j] = std::min(result[0][j], tri[i][j]);
                result[1][j] = std::max(result[1][j], tri[i][j]);
            }
        }
        return result;
    };

    for (int i = 0; i < N; i++) {
        AABBs[i] = triangle_AABB(get_triangle(i));
    }

    // * build Binary Radix Tree
    // TODO In parallel using CUDA
    // common prefix length


    auto delta = [&](int _i, int _j) -> int {
        if (_j < 0 || _j >= N) return -1;
        return __builtin_clz(mortonCode[_i] ^ mortonCode[_j]);
    };

    auto merge_AABB = [&](AABB a, AABB b) -> AABB {
        AABB result = a;
        for (int i = 0; i < 3; i++) {
            result[0][i] = std::min({result[0][i], b[0][i], b[1][i]});
            result[1][i] = std::max({result[1][i], b[0][i], b[1][i]});
        }
        return result;
    };

    auto build_BRTree_node = [&](int i) -> void {
        if (i >= N) return;
        Sorted_AABBs[i + N - 1] = AABBs[id(i)];
        if (i >= N - 1) return ;
        
        // (STEP 1.)  determine direction
        int d = delta(i, i + 1) - delta(i, i - 1) > 0 ? 1 : -1;

        // (STEP 2.)  find other end
        int delta_min = delta(i, i - d);
        int len_max = 2;
        while (delta(i, i + len_max * d) > delta_min) len_max *= 2;
        // Find other end
        int len = 0;
        for (int t = len_max / 2; t > 0; t /= 2) {
            if (delta(i, i + (len + t) * d) > delta_min)
                len += t;
        }
        
        int j = i + len * d;

        // (STEP 3.)  split_point
        int delta_node = delta(i, j);
        int s = 0;
        for (int t = (len + 1) / 2; t > 0; t = t == 1 ? 0 : (t + 1) / 2) {
            if (delta(i, i + (s + t) * d) > delta_node)
                s += t;
        }
        int gamma = i + s * d + std::min(d, 0);

        int l_idx = std::min(i, j) == gamma ? gamma + N - 1 : gamma;
        int r_idx = std::max(i, j) == gamma + 1 ? gamma + N : gamma + 1;
        node[i].l = l_idx;
        node[i].r = r_idx;
        
        node[l_idx].parent = i;
        node[r_idx].parent = i;

        int idx = node[i + N - 1].parent;
        while (idx != 0) {
            // ! if both l & r were constructed 
            if (visit[idx] == false) { visit[idx] = true; return ; }
            std:: cout << "N " << idx << "\n";
            const int l_idx = node[idx].l;
            const int r_idx = node[idx].r;
            const AABB l_aabb = Sorted_AABBs[l_idx];
            const AABB r_aabb = Sorted_AABBs[r_idx];
            // Sorted_AABBs[idx] = merge_AABB(l_aabb, r_aabb);
            // Sorted_mortonCode[idx] = commonPrefix(Sorted_mortonCode[l_idx], Sorted_mortonCode[r_idx]);
            idx = node[idx].parent;
        }
    };

    // Todo: where this should be using CUDA and proccess in parallel
    // * Constructing node
    for (int i = 0; i < N; i++) {
        build_BRTree_node(i);
    }

    std::cout << "Done\n";
}

std::vector<int> mortonCode2OCTreeindex(uint m) {
    std::vector<int> idx;
    while (m > 0) {
        int index = m & 0x7;
        idx.push_back(index);
        m >>= 3;
    }
    std::reverse(idx.begin(), idx.end());
    return idx;
}

void Object::build_OCTree_with_BVH() {
    int N = faces.size();

    auto build_OCTree_node = [&](int i) -> void {
        std::vector<int> OCTree_idx;
        if (node[i].isLeaf()) {
            // ? If leaf
            OCTree_idx = mortonCode2OCTreeindex(Sorted_mortonCode[i]);
            octree.add(OCTree_idx, Sorted_AABBs[i]);
        }
        else {
            // ? If node
            int l_idx = node[i].l;
            int r_idx = node[i].r;
            // OCTree_idx = mortonCode2OCTreeindex(commonPrefix(Sorted_mortonCode[l_idx], Sorted_mortonCode[r_idx]));
            OCTree_idx = mortonCode2OCTreeindex(Sorted_mortonCode[i]);
            octree.add(OCTree_idx, Sorted_AABBs[i]);
        }
    };

    for (int i = 0; i < 2 * N - 1; i++) {
        build_OCTree_node(i);
    }
}

void Object::get_limit() {
    limit = Vertex(0.0, 0.0, 0.0);
    for (Vertex v : vertices) {
        limit[0] = std::max(limit[0], std::abs(v[0]));
        limit[1] = std::max(limit[1], std::abs(v[1]));
        limit[2] = std::max(limit[2], std::abs(v[2]));
    }
}

} // namespace ALCHEMY
