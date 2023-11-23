#pragma once

#include "vector2.hpp"
#include <ctet/ctet.h>

inline Vector2 vector2_fromCtPoint(const CTetPoint point) {
    return {static_cast<float>(point.x), static_cast<float>(point.y)};
}