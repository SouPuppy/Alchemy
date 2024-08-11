#include "alchemy/alchemy.hpp"
#include "linearBVH.h"

namespace ALCHEMY {

uint commonPrefix(uint m1, uint m2) {
    uint diff = m1 ^ m2;
    if (diff == 0) return m1;
    int leadingZeroes = __builtin_clz(diff);
    uint mask = UINT_MAX << (32 - leadingZeroes);
    return m1 & mask;
}

uint commonPrefixOnly(uint m1, uint m2) {
    int l = __builtin_clz(m1 ^ m2);
    return m1 >> (32 - l);
}

int len(uint m) {
    int l = 0;
    while (m) m >>= 1, l++;
    return l;
}

int common_length(uint m1, uint m2) {
    int cp = commonPrefixOnly(m1, m2);
    return len(cp);
}

Triangle Object::get_triangle(Face& f) {
    return Triangle(vertices[f[0] - 1], vertices[f[1] - 1], vertices[f[2] - 1]);
}

void Object::build_LinearBVH() {
    const int N = faces.size();
    // * Init
    Sort_idx    .   resize(N);
    Origin_idx  .   resize(N);
    mortonCode  .   resize(N);
    AABBs       .   resize(N);
    for (int i = 0; i < N; i++) id(i) = i;

    Sorted_mortonCode   .   resize(N * 2 - 1);
    Sorted_AABBs        .   resize(N * 2 - 1);
    node                .   resize(N * 2 - 1);
    
    // * Get AABBs
    for (int i = 0; i < N; i++) {
        AABBs[i] = get_AABB(get_triangle(faces[i]));
    }
    // * Get MortonCode & Get Sorted Information
    Point3D limit = Vertex(0.0, 0.0, 0.0);
    for (Vertex v : vertices) {
        limit[0] = std::max(limit[0], std::abs(v[0]));
        limit[1] = std::max(limit[1], std::abs(v[1]));
        limit[2] = std::max(limit[2], std::abs(v[2]));
    }
    double scale, double_scale;
    scale           = std::max({limit[0], limit[1], limit[2]});
    double_scale    = scale * 2;
    // Get MortonCode
    for (int i = 0; i < N; i++) {
        mortonCode[i] = morton3D((centroid(get_triangle(faces[i])) + scale) / (double_scale));
    }
    // Sort
    std::sort(Sort_idx.begin(), Sort_idx.end(), [&](int a, int b) { return mortonCode[a] < mortonCode[b]; });
    for (int i = 0; i < N; i++) {
        origin(id(i)) = i;
    }
    // Get Sorted mortonCode
    for (int i = 0; i < N; i++) {
        Sorted_mortonCode[i + N - 1] = mortonCode[id(i)];
    }
    // Build BVH
    auto delta = [&](int _i, int _j) -> int {
        if (_j < 0 || _j >= N) return -1;
        return __builtin_clz(mortonCode[id(_i)] ^ mortonCode[id(_j)]);
    };

    auto merge_AABB = [&](AABB a, AABB b) -> AABB {
        AABB result = a;
        for (int i = 0; i < 3; i++) {
            result[0][i] = std::min({result[0][i], b[0][i], b[1][i]});
            result[1][i] = std::max({result[1][i], b[0][i], b[1][i]});
        }
        return result;
    };

    auto build_BVHTree_node = [&](int i) -> void {
        if (i >= N) return;
        Sorted_AABBs[i + N - 1] = AABBs[id(i)];
        if (i >= N - 1) return ;

        // (STEP 1.)  determine direction
        int d = delta(i, i + 1) - delta(i, i - 1) > 0 ? 1 : -1;

        // (STEP 2.)  find other end
        const int delta_min = delta(i, i - d);
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
        const int delta_node = delta(i, j);
        int s = 0;
        for (int t = (len + 1) / 2; t > 0; t = t == 1 ? 0 : (t + 1) / 2) {
            if (delta(i, i + (s + t) * d) > delta_node)
                s += t;
        }
        int gamma = i + s * d + std::min(d, 0);

        const int l_idx = std::min(i, j) == gamma ? gamma + N - 1 : gamma;
        const int r_idx = std::max(i, j) == gamma + 1 ? gamma + N : gamma + 1;
        node[i].l = l_idx;
        node[i].r = r_idx;

        node[l_idx].parent = i;
        node[r_idx].parent = i;
        // std:: cout << i << " " << d << "\n";
    };

    auto update_BVHTree_node = [&](int i) -> void {
        if (i > N) return;
        int idx = node[i + N - 1].parent;
        while (idx != 0) {
            // only if both l & r were constructed
            if (visit[idx] == false) { visit[idx] = true; return ; }

            const int l_idx = node[idx].l;
            const int r_idx = node[idx].r;
            const AABB l_aabb = Sorted_AABBs[l_idx];
            const AABB r_aabb = Sorted_AABBs[r_idx];
            Sorted_AABBs[idx] = merge_AABB(l_aabb, r_aabb);
            Sorted_mortonCode[idx] = commonPrefix(Sorted_mortonCode[l_idx], Sorted_mortonCode[r_idx]);

            idx = node[idx].parent;
        }  
    };
    // * Constructing node
    for (int i = 0; i < N; i++) {
        build_BVHTree_node(i);
    }
    visit.resize(N * 2 - 1);
    for (int i = 0; i < N; i++) {
        update_BVHTree_node(i);
    }

    // * node[0] ?
    const int l_idx = node[0].l;
    const int r_idx = node[0].r;
    const AABB l_aabb = Sorted_AABBs[l_idx];
    const AABB r_aabb = Sorted_AABBs[r_idx];
    Sorted_AABBs[0] = merge_AABB(l_aabb, r_aabb);
    Sorted_mortonCode[0] = commonPrefix(Sorted_mortonCode[l_idx], Sorted_mortonCode[r_idx]);

}

void Object::build_OCTree_from_BVH() {
    const int N = faces.size();




            auto KEY = [&](uint m) -> std::string {
                if (m == 0) return "/";
                std::string s = "";
                while (m) {
                    s = "01"[m & 1] + s;
                    m >>= 1;
                }
                return s;
            };


    auto build_OCTree_node_from_BVH_node = [&](int i) -> void {
        OctNode *p;
        // * Leaf node
        if (i > N) return ;
        node[i + N - 1].ref_oct = (OctNode*)new OctNode();
        p = (OctNode*)node[i + N - 1].ref_oct;
        p->aabb = Sorted_AABBs[i + N - 1];
        p->key = -1;
        p->l_board = i;
        p->r_borad = i;

        int idx = node[i + N - 1].parent;

        std::vector<int> son;
        while (idx != 0) {
            // only if both l & r were constructed
            if (visit[idx] == false) { visit[idx] = true; return ; }

            const int l_idx = node[idx].l;
            const int r_idx = node[idx].r;
            uint oct_idx = commonPrefixOnly(Sorted_mortonCode[l_idx], Sorted_mortonCode[r_idx]);
            std::cout << KEY(oct_idx) << "\n" << BS(Sorted_mortonCode[l_idx]) << "\n" << BS(Sorted_mortonCode[r_idx]) << "\n\n";

            int oct_idx_len = common_length(Sorted_mortonCode[l_idx], Sorted_mortonCode[r_idx]);
            int prefix, suffix;
            prefix  =   oct_idx >> (oct_idx_len % 3);
            suffix  =   oct_idx & ((1 << (oct_idx_len % 3)) - 1);
            //* new oct node
            node[idx].ref_oct = (OctNode*)new OctNode();
            p = (OctNode*)node[idx].ref_oct;
            //* building node
            p->key = oct_idx;

            // l,r directory
            uint l_dir = (Sorted_mortonCode[l_idx] & ((1 << (30 - oct_idx_len)) - 1)) | (suffix << oct_idx_len); // 111 | [001]0110
            uint r_dir = (Sorted_mortonCode[r_idx] & ((1 << (30 - oct_idx_len)) - 1)) | (suffix << oct_idx_len); // 111 | [011]0101
            // l,r positioning
            uint l_pos = (l_dir >> (len(l_dir) - 6)) & 7;     // [001]
            uint r_pos = (r_dir >> (len(r_dir) - 6)) & 7;     // [011]
            
            p->son[l_pos] = (OctNode*)node[l_idx].ref_oct;
            p->son[r_pos] = (OctNode*)node[r_idx].ref_oct;

            idx = node[idx].parent;
        }
    };

    // * p2p BVH node to OCT node
    // node
    visit.resize(N * 2 - 1);
    node[0].ref_oct = root;
    for (int i = 0; i < N; i++) {
        build_OCTree_node_from_BVH_node(i);
    }
    //* updata octtree
    }

} // namespace ALCHEMY
