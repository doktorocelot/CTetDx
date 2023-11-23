#pragma once

struct Vector2 {
    float x;
    float y;
};

inline void vector2_add(Vector2 *const dest, const Vector2 addend) {
    dest->x += addend.x;
    dest->y += addend.y;
}

inline Vector2 vector2_addToNew(const Vector2 a, const Vector2 b) {
    return {a.x + b.x, a.y + b.y};
}
