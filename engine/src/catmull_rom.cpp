#ifdef __APPLE__
#include <GL/freeglut.h>
#include <GLUT/glut.h>
#elif _WIN32
#include <GL/glew.h>
// DONT REMOVE THIS COMMENT
#include <GL/freeglut.h>
#else
#include <GL/glew.h>
// DONT REMOVE THIS COMMENT
#include <GL/freeglut.h>
#include <GL/glut.h>
#include <cmath>
#endif

#include "catmull_rom.hpp"

extern float globalTimer;
extern float timeFactor;

void buildRotMatrix(float* x, float* y, float* z, float* m)
{

    m[0] = x[0];
    m[1] = x[1];
    m[2] = x[2];
    m[3] = 0;
    m[4] = y[0];
    m[5] = y[1];
    m[6] = y[2];
    m[7] = 0;
    m[8] = z[0];
    m[9] = z[1];
    m[10] = z[2];
    m[11] = 0;
    m[12] = 0;
    m[13] = 0;
    m[14] = 0;
    m[15] = 1;
}

void cross(float* a, float* b, float* res)
{
    res[0] = a[1] * b[2] - a[2] * b[1];
    res[1] = a[2] * b[0] - a[0] * b[2];
    res[2] = a[0] * b[1] - a[1] * b[0];
}

void normalize(float* a)
{
    float l = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
    a[0] = a[0] / l;
    a[1] = a[1] / l;
    a[2] = a[2] / l;
}

float length(float* v)
{
    float res = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
    return res;
}

void multMatrixVector(float* m, float* v, float* res)
{

    for (int j = 0; j < 4; ++j) {
        res[j] = 0;
        for (int k = 0; k < 4; ++k) {
            res[j] += v[k] * m[j * 4 + k];
        }
    }
}

void getCatmullRomPoint(float t, float* p0, float* p1, float* p2, float* p3, float* pos, float* deriv)
{

    // catmull-rom matrix
    float m[4][4] = { { -0.5f, 1.5f, -1.5f, 0.5f },
        { 1.0f, -2.5f, 2.0f, -0.5f },
        { -0.5f, 0.0f, 0.5f, 0.0f },
        { 0.0f, 1.0f, 0.0f, 0.0f } };

    float T[4] = { t * t * t, t * t, t, 1 };
    float T_deriv[4] = { 3 * t * t, 2 * t, 1, 0 };

    for (int i = 0; i < 3; i++) {
        float P[4] = { p0[i], p1[i], p2[i], p3[i] };

        float A[4] = { 0 };
        for (int j = 0; j < 4; j++) {
            for (int k = 0; k < 4; k++) {
                A[j] += m[j][k] * P[k];
            }
        }

        pos[i] = T[0] * A[0] + T[1] * A[1] + T[2] * A[2] + T[3] * A[3];
        deriv[i] = T_deriv[0] * A[0] + T_deriv[1] * A[1] + T_deriv[2] * A[2] + T_deriv[3] * A[3];
    }
}

// given  global t, returns the point in the curve
void getGlobalCatmullRomPoint(float time, float* pos, float* deriv, const Transform transform)
{
    size_t pointCount = transform.numberCurvePoints;

    float newTime = time == -1 ? (globalTimer / 10000) : time;
    float t = newTime * pointCount; // this is the real global t
    int index = floor(t); // which segment
    t = t - index; // where within  the segment

    // indices store the points
    int indices[4];
    indices[0] = (index + pointCount - 1) % pointCount;
    indices[1] = (indices[0] + 1) % pointCount;
    indices[2] = (indices[1] + 1) % pointCount;
    indices[3] = (indices[2] + 1) % pointCount;

    float** p = transform.curvePoints;

    getCatmullRomPoint(t, p[indices[0]], p[indices[1]], p[indices[2]], p[indices[3]], pos, deriv);
}

float* getRotMatrix(float* forward, float* up)
{
    normalize(forward);

    float right[3];
    cross(forward, up, right);
    normalize(right);

    cross(right, forward, up);
    normalize(up);

    float* rotMatrix = (float*)malloc(sizeof(float) * 16);
    buildRotMatrix(forward, up, right, rotMatrix);

    return rotMatrix;
}
