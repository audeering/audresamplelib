#include "dr_wav.h"
#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <audresample.h>
#include <remix.h>


#undef M_PI /* Sometimes missing, so ensure that it is defined: */
#define M_PI 3.14159265358979323846
#define FSR 65536.0


float clipSample(float value)
{
  if (fabs((double) value) > 1.0f) {
    value = value > 0.0 ? 1.0 : -1.0;
  }
  return value;
}
