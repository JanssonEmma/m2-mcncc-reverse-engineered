#ifndef MCNCC_VECTOR_H
#define MCNCC_VECTOR_H

typedef struct SVector
{
    float x;
    float y;
    float z;
} VECTOR;

void Normalize(VECTOR* input, VECTOR* output);
float DotProduct(VECTOR* left, VECTOR* right);
long double GetDegreeFromPosition(float x, float y);
long double GetDegreeFromPositionXY(long sx, long sy, long ex, long ey);
void GetDeltaByDegree(float degree, float distance, float* x, float* y);
long double GetDegreeDelta(float first, float second);

#endif
