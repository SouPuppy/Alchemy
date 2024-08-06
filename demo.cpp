#include "alchemy/alchemy.hpp"
#include "alchemy/tensor.hpp"
#include <string>
#include <iostream>
using namespace std;


int main(int argc, char **argv) {
    // TENSOR::Tensor<float, 4> a(2, 2, 4, 2);
    // a.print();

    TENSOR::Matrix<float, 2, 2> b(0, 1, 2, 3);
    b.print();

    // TENSOR::Vector<float, 3> c(1, 2, 3);
    // c.print();

    // const string demo_obj = "C:/Users/soupup/Desktop/Alchemy/source/cube.obj";


    // ALCHEMY::Canvas viewer;
    // ALCHEMY::Object& demo = viewer.add_obj(demo_obj);

    // 
    // void update = [&](bool object_updated = false) {
    //     // not updated
    //     if (object_updated == false) {
    //         return;
    //     }

    //     // update
    // }

    // update();

    // // GUI
    // viewer.gui = [&]() {
        
    // }


    // viewer.show();
    return 0;
}
