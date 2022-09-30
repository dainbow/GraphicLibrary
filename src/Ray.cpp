#include "Ray.hpp"

std::ostream& operator<<(std::ostream& stream, const Ray& ray) {
    stream << ray.point_ << " " << ray.vector_ << " " << ray.power_;

    return stream;
}