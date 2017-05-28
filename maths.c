#include "maths.h"

float random(float a, float b)
{
    return (rand() / (float)RAND_MAX) * (b - a) + a;
}
