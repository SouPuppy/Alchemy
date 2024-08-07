#include "alchemy/alchemy.hpp"
#include "alchemy/tensor.hpp"
#include <string>
#include <iostream>
#include <algorithm>
#include <bitset>
using namespace std;

using uint = unsigned int;

// Expands a 10-bit interger into 30 bits.
uint expandBits(uint v) {
    
    v = (v * 0x00010001u) & 0xFF0000FFu;
    v = (v * 0x00000101u) & 0x0F00F00Fu;
    v = (v * 0x00000011u) & 0xC30C30C3u;
    v = (v * 0x00000005u) & 0x49249249u;

    return v;
}

// Calculates a 30-bit Morton code for the
// given 3D point located within the unit cube [0,1].
// Morton encoding function using std::clamp
template<typename R>
uint morton3D(R x, R y, R z) {
    uint x_exp = expandBits(clamp(static_cast<int>(x * 1024), 0, 1023));
    uint y_exp = expandBits(clamp(static_cast<int>(y * 1024), 0, 1023));
    uint z_exp = expandBits(clamp(static_cast<int>(z * 1024), 0, 1023));
    return (x_exp << 2) | (y_exp << 1) | z_exp;
}

int main(int argc, char **argv) {
    for (double i = 0; i <= 1; i += 0.5) {
        for (double j = 0; j <= 1; j += 0.5) {
            for (double k = 0; k <= 1; k += 0.5) {
                uint mortonCode = morton3D(i, j, k);
                cout << i << ", " << j << ", " << k << " ";
                cout << "Binary: " << bitset<30>(mortonCode) << endl;
            }
        }
    }
    
    return 0;
}