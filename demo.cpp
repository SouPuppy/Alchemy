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

    obj.build_LinearBVH();
    obj.build_OCTree_from_BVH();
    obj.debug();
    return 0;
}