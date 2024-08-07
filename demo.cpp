#include "alchemy/alchemy.hpp"
#include "alchemy/tensor.hpp"

#include <string>
#include <iostream>
#include <algorithm>
using namespace std;

namespace ALCHEMY {

void Object::get_limit() {
    limit = Vertex(0.0, 0.0, 0.0);
    for (Vertex v : vertices) {
        limit[0] = std::max(limit[0], std::abs(v[0]));
        limit[1] = std::max(limit[1], std::abs(v[1]));
        limit[2] = std::max(limit[2], std::abs(v[2]));
    }
}

void Object::get_MortonCode() {
    get_limit();
    double scale, double_scale;
    scale           = std::max({limit[0], limit[1], limit[2]});
    double_scale    = scale * 2;

    for (Face &f : faces) {
        f.MortonCode = morton3D((centroid(get_triangle(f)) + scale) / (double_scale));
    }
}

void Object::sort_faces() {
    get_MortonCode();
    std::sort(faces.begin(), faces.end(), [](const Face& f1, const Face& f2) { return f1.MortonCode < f2.MortonCode; });
}

} // namespace ALCHEMY

using namespace ALCHEMY;
using namespace TENSOR;
int main(int argc, char **argv) {
    Object obj("C:/Users/soupup/Desktop/Alchemy/source/cube.obj");
    obj.sort_faces();
    
    // centroid(obj.get_triangle(0)).print();
    // Matrix<float, 2, 2> a(1, 2, 3, 4);
    // a.print();
    // a = a + 1;
    // a.print();
    return 0;
}