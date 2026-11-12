// Recovered vector.cpp functions, organized as in the supplied reference tree.
#include <cmath>
#include <cstdint>
#include <cstring>
#include "vector.h"

static const float pi = 3.141592654f;

// 0x08164be0; binary 0x0822b490 contains the double constant 1.0e-12.
void Normalize(VECTOR* input, VECTOR* output)
{
    const long double x = input->x;
    const long double y = input->y;
    const long double z = input->z;
    const long double length = ::sqrtf(static_cast<float>(x * x + y * y + z * z + 1.0e-12));
    output->x = static_cast<float>(x / length);
    output->y = static_cast<float>(y / length);
    output->z = static_cast<float>(z / length);
}

// 0x08164c40
float DotProduct(VECTOR* left, VECTOR* right)
{
    const long double x = static_cast<long double>(left->x) * right->x;
    const long double y = static_cast<long double>(left->y) * right->y;
    const long double z = static_cast<long double>(left->z) * right->z;
    return static_cast<float>(x + y + z);
}

// 0x08164c70: the zero vector intentionally yields 90 degrees.
long double GetDegreeFromPosition(float x, float y)
{
    VECTOR direction = {x, y, 0.0f};
    VECTOR reference = {0.0f, 1.0f, 0.0f};
    Normalize(&direction, &direction);
    const long double result = static_cast<long double>(::acosf(DotProduct(&direction, &reference))) *
                               (180.0f / pi);
    return direction.x < 0.0f ? 360.0L - result : result;
}

// 0x08164cf0
long double GetDegreeFromPositionXY(long sx, long sy, long ex, long ey)
{
    const auto delta = [](long end, long start) {
        const std::uint32_t bits = static_cast<std::uint32_t>(end) - static_cast<std::uint32_t>(start);
        std::int32_t result;
        std::memcpy(&result, &bits, sizeof(result));
        return static_cast<float>(result);
    };
    return GetDegreeFromPosition(delta(ex, sx), delta(ey, sy));
}

// 0x08164d20: double-precision sin/cos, then convert the product to float.
void GetDeltaByDegree(float degree, float distance, float* x, float* y)
{
    const double radians = static_cast<double>(degree) * (pi / 180.0f);
    const float cosineRadians = static_cast<float>(radians);
    *x = static_cast<float>(static_cast<long double>(distance) * std::sin(radians));
    *y = static_cast<float>(static_cast<long double>(distance) * std::cos(static_cast<double>(cosineRadians)));
}

// 0x08164d70: one subtraction per argument; not a general shortest-arc function.
long double GetDegreeDelta(float first, float second)
{
    const long double a = first > 180.0f ? static_cast<long double>(first) - 360.0L : first;
    const long double b = second > 180.0f ? static_cast<long double>(second) - 360.0L : second;
    return std::fabs(a - b);
}
