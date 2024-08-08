#include "alchemy/alchemy.hpp"
#include "linearBVH.h"

Point3D centroid(Triangle tri) {
    return (tri[0] + tri[1] + tri[2]) / 3;
}

namespace ALCHEMY {

void Object::init_BVH() {
    int N = faces.size();
    mortonCode.resize(N);
    Sorted_AABB.resize(N * 2 - 1);
    Sorted_Id.resize(N);
    for (int i = 0; i < N; i++)     Sorted_Id[i] = i;
}

void Object::get_limit() {
    limit = Vertex(0.0, 0.0, 0.0);
    for (Vertex v : vertices) {
        limit[0] = std::max(limit[0], std::abs(v[0]));
        limit[1] = std::max(limit[1], std::abs(v[1]));
        limit[2] = std::max(limit[2], std::abs(v[2]));
    }
}

void Object::get_mortonCode() {
    int N = faces.size();
    get_limit();
    double scale, double_scale;
    scale           = std::max({limit[0], limit[1], limit[2]});
    double_scale    = scale * 2;

    for (int i = 0; i < N; i++) {
        mortonCode[i] = morton3D((centroid(get_triangle(i)) + scale) / (double_scale));
    }
    
}

void Object::sort_faces() {
    get_mortonCode();
    std::sort(Sorted_Id.begin(), Sorted_Id.end(), [&](int a, int b) { return mortonCode[a] < mortonCode[b]; });
    // int N = faces.size();
    // for (int i = 0; i < N; i++) {
    //     std:: cout << mortonCode[Sorted_Id[i]] << "\n";
    // }
    
}





// Object::init(Object& _obj): obj(_obj) {
//     node = (int*)malloc(sizeof(int) *( (obj.faces.size() - 1)));
//     leaf = (int*)malloc(sizeof(int) *( (obj.faces.size())));
// }

// int Object::get_father(int i, int &j, int &gamma) {
//     // * (STEP 1.)  determine direction
//     int d = delta(i, i + 1) - delta(i, i - 1) > 0 ? 1 : -1;
//     // * (STEP 2.)  find other end
//     int delta_min = delta(i, i - d);
//     int len_max = 2;
//     while (delta(i, i + len_max * d) > delta_min) len_max *= 2;
//     // Find other end
//     int len = 0;
//     for (int t = len_max / 2; t > 0; t /= 2) {
//         if (delta(i, i + (len + t) * d) > delta_min)
//             len += t;
//     }
    
//     j = i + len * d;
//     // * (STEP 3.)  split_point
//     int delta_node = delta(i, j);
//     int s = 0;
//     for (int t = (len + 1) / 2; t > 0; = t == 1 ? 0 : (t + 1) / 2) {
//         if (delta(i, i + (s + t) * d) > delta_node)
//             s += t;
//     }
//     gamma = i + s * d + std::min(d, 0);
// }
// // init AABB in a leaf node
// void Object::construct_leaf(int i) {
//     int N = obj.faces.size();
//     if (i >= N) return;
//     int idx = i + N - 1;
//     node[idx].aabb = obj.faces[i].aabb;
//     if (i >= N - 1) return ;
//     auto delte = [&](int _i, int _j) -> int {
//         if (_j < 0 || _j >= N) return -1;
//         return __clzll(obj.faces[_i] ^ obj.faces[_j]);
//     }
//     int j, gamma;
//     get_father(i, j, gamma);
// 	int l_idx = std::min(i, j) == gamma ? gamma + N - 1 : gamma;
// 	int r_idx = std::max(i, j) == gamma + 1 ? gamma + N : gamma + 1;
//     node[i].l = l_idx;
//     node[i].r = r_idx;
    
//     node[l_idx].parent = i;
    
    
    
// }
// // update AABB from a leaf node
// void Object::update_AABB(int i) {
//     int N = obj.faces.size();
//     if (i >= N) return;
//     int idx = node[i + N - 1].parent;
//     while (idx != 0) {
//         // ! if both l & r were constructed
//         const int l_idx = node[idx].l;
//         const int r_idx = node[idx].r;
//         const AABB l_aabb = get_aabb(l_idx);
//         const AABB r_aabb = get_aabb(r_idx);
//         aabbs[idx] = merge_AABB(l_aabb, r_aabb);
//         idx = node[idx].parent;
//     }
// }
// void Object::construct_LBVH() {
//     for f:faces
//         construct_leaf(i);
//     for f:faces
//         update_AABB(i);
// }


} // namespace ALCHEMY
