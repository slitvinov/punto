#ifndef _MATHS_H
#define _MATHS_H

#include "punto.h"

double aleatorio(double a);
void Dec2Hex(long int n, char *a);
void Rapida(long *a, int izq, int drch, struct Punto *p);

void TCR(float a, float b, float c, long, long, struct Point r, float comp,
         short field, int *reset_box, struct Punto *p);

#endif
