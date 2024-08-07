#include "alchemy/alchemy.hpp"
#include "alchemy/tensor.hpp"
#include "alchemy/common.hpp"

#include <string>
#include <iostream>
#include <algorithm>
#include <bitset>
using namespace std;

namespace ALCHEMY {

void Object::get_MortonCode() {
    
}

void Object::sort_faces() {
    get_MortonCode();
}

} // namespace ALCHEMY

using namespace ALCHEMY;
using namespace TENSOR;
int main(int argc, char **argv) {
    Object obj("C:/Users/soupup/Desktop/Alchemy/source/cube.obj");
    // obj.sort_faces();
    
    centroid_of_Face(obj, obj.faces[0]).print();
    return 0;
}