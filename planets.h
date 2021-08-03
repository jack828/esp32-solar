#ifndef __PLANETS_H__
#define __PLANETS_H__

#include <time.h>

#define NUM_PLANETS 8

struct Position {
  double xeclip;
  double yeclip;
  double zeclip;
  double long2;
  double lat2;
  double r;
};

double normalize(double degrees);

double toRadians(double degrees);

double toDegrees(double radians);

void coordinates(Position* planets, const tm *time);

Position from_sun(double m, double e, double a, double n, double w, double i);
#endif
