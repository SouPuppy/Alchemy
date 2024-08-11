#pragma once

namespace ALCHEMY {

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
uint morton3D(Point3D p) {
    uint x_exp = expandBits(std::clamp(static_cast<int>(p[0] * 1024), 0, 1023));
    uint y_exp = expandBits(std::clamp(static_cast<int>(p[1] * 1024), 0, 1023));
    uint z_exp = expandBits(std::clamp(static_cast<int>(p[2] * 1024), 0, 1023));
    return (x_exp << 2) | (y_exp << 1) | z_exp;
}

Point3D centroid(Triangle tri) {
    return (tri[0] + tri[1] + tri[2]) / 3;
}

AABB get_AABB(Triangle tri) {
    AABB ret = AABB(tri[0], tri[0]);
    for (int i = 1; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            ret[0][j] = std::min(ret[0][j], tri[i][j]);
            ret[1][j] = std::max(ret[1][j], tri[i][j]);
        }
    }
    return ret;
}

} // namespace ALCHEMY
