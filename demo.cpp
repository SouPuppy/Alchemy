#include "alchemy/alchemy.hpp"
#include "alchemy/tensor.hpp"

#include <string>
#include <iostream>
#include <algorithm>
using namespace std;
using namespace ALCHEMY;
using namespace TENSOR;

int main(int argc, char **argv) {
    Object obj("C:/Users/soupup/Desktop/Alchemy/sources/cube.obj");
    
    obj.init_BVH();
    obj.sort_faces();
    
    // centroid(obj.get_triangle(0)).print();
    // Matrix<float, 2, 2> a(1, 2, 3, 4);
    // a.print();
    // a = a + 1;
    // a.print();
    return 0;
}