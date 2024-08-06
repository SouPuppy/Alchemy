#include "alchemy/alchemy.hpp"
#include "alchemy/tensor.hpp"
#include <cmath>
const double M_PI = 3.14159265358979323846;

namespace ALCHEMY {

void Camera::reset_view() {
    world_up = Norm3D(0.f, 1.f, 0.f);
    dist_to_center = 3.f;

    yaw = -M_PI / 2;
    pitch = roll = 0.0f;
    
    update_view();
}

} // namespace ALCHEMY
