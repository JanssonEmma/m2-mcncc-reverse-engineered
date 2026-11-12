#include <cassert>
#include <cmath>
#include "../../game/src/vector.h"

static bool Near(float actual, float expected, float tolerance = 0.0001f)
{
    return std::fabs(actual - expected) < tolerance;
}

int main()
{
    VECTOR vector = {3.0f, 4.0f, 0.0f};
    Normalize(&vector, &vector);
    assert(Near(vector.x, 0.6f) && Near(vector.y, 0.8f) && vector.z == 0);
    VECTOR zero = {0.0f, 0.0f, 0.0f};
    Normalize(&zero, &zero);
    assert(zero.x == 0 && zero.y == 0 && zero.z == 0);
    VECTOR tiny = {0.000001f, 0.0f, 0.0f};
    VECTOR output;
    Normalize(&tiny, &output);
    assert(Near(output.x, 0.70710678f) && tiny.x == 0.000001f);
    VECTOR left = {1, 2, 3};
    VECTOR right = {4, 5, 6};
    assert(DotProduct(&left, &right) == 32);
    assert(Near(GetDegreeFromPosition(0, 1), 0));
    assert(Near(GetDegreeFromPosition(1, 0), 90));
    assert(Near(GetDegreeFromPosition(0, -1), 180));
    assert(Near(GetDegreeFromPosition(-1, 0), 270));
    assert(Near(GetDegreeFromPosition(0, 0), 90));
    assert(Near(GetDegreeFromPositionXY(10, 20, 11, 20), 90));
    float x, y;
    GetDeltaByDegree(90, 10, &x, &y);
    assert(Near(x, 10) && Near(y, 0));
    GetDeltaByDegree(0, 10, &x, &y);
    assert(Near(x, 0) && Near(y, 10));
    assert(GetDegreeDelta(359, 1) == 2);
    assert(GetDegreeDelta(179, 181) == 358); // Preserve the observed discontinuity.
    assert(GetDegreeDelta(900, 0) == 540); // Does not repeatedly wrap large values.
    return 0;
}
