#pragma once

#include <ctet/ctet.h>
#include <DirectXMath.h>

static DirectX::XMFLOAT2 ctPointToDx(CTetPoint point) {
    return {static_cast<float>(point.x), static_cast<float>(point.y)};
}