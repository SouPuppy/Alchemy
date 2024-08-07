#include "alchemy/alchemy.hpp"
#include "alchemy/tensor.hpp"
#include <cmath>
const double M_PI = 3.14159265358979323846;

namespace ALCHEMY {

void Camera::update_view() {

}

void Camera::update_proj() {

}

void Camera::reset_view() {

}

void Camera::reset_proj() {

}

void Camera::rotate_with_mouse(float xoffset, float yoffset) {
    xoffset *= rotate_speed;
    yoffset *= rotate_speed;

    float cr = cos(roll), sr = sin(roll);
    yaw += xoffset * cr + yoffset * sr;
    pitch -= yoffset * cr + xoffset * sr;

    // Clamp pitch
    static const float PITCH_CLAMP = M_PI * 0.49999f;
    if (std::fabs(pitch) > PITCH_CLAMP) {
        pitch = PITCH_CLAMP * (pitch > 0.f ? 1.f : -1.f);
        yaw = yaw += M_PI;
        roll = roll += M_PI;
    }
    update_view();
}

void Camera::roll_with_mouse(float xoffset, float yoffset) {
    xoffset *= rotate_speed;
    roll += xoffset;

    update_view();
}

void Camera::pan_with_mouse(float xoffset, float yoffset) {
    xoffset *= pan_speed * dist_to_center;
    yoffset *= pan_speed * dist_to_center;

    center_of_rot += -xoffset * right + yoffset * camera_up;
    update_view();
}

void Camera::zoom_with_mouse(float amount) {

}

} // namespace ALCHEMY
