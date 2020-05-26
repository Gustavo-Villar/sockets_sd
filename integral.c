#include "stdio.h"
#include "stdlib.h"
#include <math.h>

double integracao(double x)
{

  return sqrt((100 * 100) - (x * x));
}

int main(int argc, char const *argv[])
{

  double xa, xb;
  double area, incr;

  int i, n;

  xa = 0;
  xb = 100;
  n = 1000;

  incr = (xb - xa) / n;
  area = 0;

  for (i = 0; i < n; i++)
  {

    xb = xa + incr;

    area = area + (xb - xa) * (integracao(xa) + integracao(xb)) / 2.0;

    xa = xb;

  }

  printf("Area = %f \n", area);

  return 0;
}
