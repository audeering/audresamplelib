#include <stdio.h>
#include <errno.h>
#include <math.h>
#include <audresample.h>
#include <audeering/wavelib-audeering.h>

#undef M_PI /* Sometimes missing, so ensure that it is defined: */
#define M_PI 3.14159265358979323846
#define FSR 65536.0


FLOAT_TYPE clipSample(FLOAT_TYPE value)
{
  if (fabs((double) value) > 1.0f) {
    value = value > 0.0 ? 1.0 : -1.0;
  }
  return value;
}
