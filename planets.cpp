#include "planets.h"
#include <malloc.h>
#include <math.h>

#define RADIANS_TO_DEGREES 0.017453292519943295
#define DEGREES_TO_RADIANS 57.29577951308232

double normalize(double degrees) {
  return degrees - floor(degrees / 360.0) * 360.0;
}

double toRadians(double degrees) {
  return degrees / DEGREES_TO_RADIANS;
}

double toDegrees(double radians) {
  return radians / RADIANS_TO_DEGREES;
}

Planet::Planet() {}

// micropython math uses radians :tada:
Position *coordinates(double year, double month, double day, double hour,
                      double minute) {
  // Return value
  Position *ret = (Position *)malloc(NUM_PLANETS * sizeof(Position));
  // SECTION 4.
  // Compute day number by converting provided calendar date to Julian Day
  // Number
  double julianDayNumber =
      ((367 * year - floor(7 * (year + floor((month + 9) / 12)) / 4)) +
       floor(275 * month / 9) + (day + 1721013.5));
  // Add the current time
  julianDayNumber += (hour / 24.0 + minute / 1440.0);
  // d = our day number
  double d = julianDayNumber - 2451543.5;

  double w = 282.9404 + 4.70935E-5 * d;
  double e = (0.016709 - (1.151E-9 * d));
  double m2 = normalize(356.047 + 0.9856002585 * d);

  double m = toRadians(m2);
  double e_capt = m2 + (180 / M_PI) * e * sin(m) * (1 + e * cos(m));
  e_capt = toRadians(e_capt);
  double x = cos(e_capt) - e;
  double y = sin(e_capt) * sqrt(1 - e * e);

  double r = hypot(x, y);

  double v = atan2(y, x);
  v = toDegrees(v);
  double lon = (v + w);
  lon = normalize(lon);
  lon = toRadians(lon);
  double x2 = r * cos(lon);
  double y2 = r * sin(lon);

  double earthX = -1 * x2;
  double earthY = -1 * y2;
  double earthZ = 0;

  // N = Longitude of Ascending Node
  // i = Inclination
  // w = Argument of perihelion
  // a = Semi-major axis
  // e = Eccentricity
  // M = Mean anomaly

  double N_MERCURY = 48.3313 + 3.24587E-5 * d;
  double i_MERCURY = 7.0047 + 5.00E-8 * d;
  double w_MERCURY = 29.1241 + 1.01444E-5 * d;
  double a_MERCURY = 0.387098;
  double e_MERCURY = 0.205635 + 5.59E-10 * d;
  double M_MERCURY = 168.6562 + 4.0923344368 * d;

  M_MERCURY = normalize(M_MERCURY);

  double N_VENUS = 76.6799 + 2.46590E-5 * d;
  double i_VENUS = 3.3946 + 2.75E-8 * d;
  double w_VENUS = 54.8910 + 1.38374E-5 * d;
  double a_VENUS = 0.723330;
  double e_VENUS = 0.006773 - 1.30E-9 * d;
  double M_VENUS = 48.0052 + 1.6021302244 * d;

  M_VENUS = normalize(M_VENUS);

  double N_MARS = 49.5574 + 2.11081E-5 * d;
  double i_MARS = 1.8497 - 1.78E-8 * d;
  double w_MARS = 286.5016 + 2.92961E-5 * d;
  double a_MARS = 1.523688;
  double e_MARS = 0.093405 + 2.51E-9 * d;
  double M_MARS = 18.6021 + 0.5240207766 * d;

  M_MARS = normalize(M_MARS);

  double N_JUPITER = 100.4542 + 2.76854E-5 * d;
  double i_JUPITER = 1.3030 - 1.557E-7 * d;
  double w_JUPITER = 273.8777 + 1.6450E-5 * d;
  double a_JUPITER = 5.20256;
  double e_JUPITER = 0.048498 + 4.469E-9 * d;
  double M_JUPITER = 19.8950 + 0.0830853001 * d;

  M_JUPITER = normalize(M_JUPITER);

  double N_SATURN = 113.6634 + 2.38980E-5 * d;
  double i_SATURN = 2.4886 - 1.081E-7 * d;
  double w_SATURN = 339.3939 + 2.97661E-5 * d;
  double a_SATURN = 9.55475;
  double e_SATURN = 0.055546 - 9.499E-9 * d;
  double M_SATURN = 316.9670 + 0.0334442282 * d;

  M_SATURN = normalize(M_SATURN);

  double N_URANUS = 74.0005 + 1.3978E-5 * d;
  double i_URANUS = 0.7733 + 1.9E-8 * d;
  double w_URANUS = 96.6612 + 3.0565E-5 * d;
  double a_URANUS = 19.18171 - 1.55E-8 * d;
  double e_URANUS = 0.047318 + 7.45E-9 * d;
  double M_URANUS = 142.5905 + 0.011725806 * d;

  M_URANUS = normalize(M_URANUS);

  double N_NEPTUNE = 131.7806 + 3.0173E-5 * d;
  double i_NEPTUNE = 1.7700 - 2.55E-7 * d;
  double w_NEPTUNE = 272.8461 - 6.027E-6 * d;
  double a_NEPTUNE = 30.05826 + 3.313E-8 * d;
  double e_NEPTUNE = 0.008606 + 2.15E-9 * d;
  double M_NEPTUNE = 260.2471 + 0.005995147 * d;

  M_NEPTUNE = normalize(M_NEPTUNE);

  Position mercuryPosition = from_sun(M_MERCURY, e_MERCURY, a_MERCURY,
                                      N_MERCURY, w_MERCURY, i_MERCURY);
  Position venusPosition =
      from_sun(M_VENUS, e_VENUS, a_VENUS, N_VENUS, w_VENUS, i_VENUS);
  Position marsPosition =
      from_sun(M_MARS, e_MARS, a_MARS, N_MARS, w_MARS, i_MARS);
  Position jupiterPosition = from_sun(M_JUPITER, e_JUPITER, a_JUPITER,
                                      N_JUPITER, w_JUPITER, i_JUPITER);
  Position saturnPosition =
      from_sun(M_SATURN, e_SATURN, a_SATURN, N_SATURN, w_SATURN, i_SATURN);
  Position uranusPosition =
      from_sun(M_URANUS, e_URANUS, a_URANUS, N_URANUS, w_URANUS, i_URANUS);
  Position neptunePosition = from_sun(M_NEPTUNE, e_NEPTUNE, a_NEPTUNE,
                                      N_NEPTUNE, w_NEPTUNE, i_NEPTUNE);

  M_JUPITER = normalize(M_JUPITER);
  M_SATURN = normalize(M_SATURN);
  M_URANUS = normalize(M_URANUS);

  double JUPITER_diat1 =
      -0.332 * sin(toRadians(2 * M_JUPITER - 5 * M_SATURN - 67.6));
  double JUPITER_diat2 =
      -0.056 * sin(toRadians(2 * M_JUPITER - 2 * M_SATURN + 21));
  double JUPITER_diat3 =
      0.042 * sin(toRadians(3 * M_JUPITER - 5 * M_SATURN + 21));
  double JUPITER_diat4 = -0.036 * sin(toRadians(M_JUPITER - 2 * M_SATURN));
  double JUPITER_diat5 = 0.022 * cos(toRadians(M_JUPITER - M_SATURN));
  double JUPITER_diat6 =
      0.023 * sin(toRadians(2 * M_JUPITER - 3 * M_SATURN + 52));
  double JUPITER_diat7 = -0.016 * sin(toRadians(M_JUPITER - 5 * M_SATURN - 69));

  double SATURN_diat1 =
      0.812 * sin(toRadians(2 * M_JUPITER - 5 * M_SATURN - 67.6));
  double SATURN_diat2 =
      -0.229 * cos(toRadians(2 * M_JUPITER - 4 * M_SATURN - 2));
  double SATURN_diat3 = 0.119 * sin(toRadians(M_JUPITER - 2 * M_SATURN - 3));
  double SATURN_diat4 =
      0.046 * sin(toRadians(2 * M_JUPITER - 6 * M_SATURN - 69));
  double SATURN_diat5 = 0.014 * sin(toRadians(M_JUPITER - 3 * M_SATURN + 32));

  double SATURN_diat6 =
      -0.02 * cos(toRadians(2 * M_JUPITER - 4 * M_SATURN - 2));
  double SATURN_diat7 =
      0.018 * sin(toRadians(2 * M_JUPITER - 6 * M_SATURN - 49));

  double URANUS_diat1 = 0.04 * sin(toRadians(M_SATURN - 2 * M_URANUS + 6));
  double URANUS_diat2 = 0.035 * sin(toRadians(M_SATURN - 3 * M_URANUS + 33));
  double URANUS_diat3 = -0.015 * sin(toRadians(M_JUPITER - M_URANUS + 20));

  double diataraxes_long_JUPITER =
      (JUPITER_diat1 + JUPITER_diat2 + JUPITER_diat3 + JUPITER_diat4 +
       JUPITER_diat5 + JUPITER_diat6 + JUPITER_diat7);
  double diataraxes_long_SATURN = (SATURN_diat1 + SATURN_diat2 + SATURN_diat3 +
                                   SATURN_diat4 + SATURN_diat5);
  double diataraxes_lat_SATURN = (SATURN_diat6 + SATURN_diat7);
  double diataraxes_long_URANUS = (URANUS_diat1 + URANUS_diat2 + URANUS_diat3);

  jupiterPosition.long2 = jupiterPosition.long2 + diataraxes_long_JUPITER;
  saturnPosition.long2 = saturnPosition.long2 + diataraxes_long_SATURN;
  saturnPosition.lat2 = saturnPosition.lat2 + diataraxes_lat_SATURN;
  uranusPosition.long2 = uranusPosition.long2 + diataraxes_long_URANUS;

  jupiterPosition.long2 = (toRadians(jupiterPosition.long2));
  jupiterPosition.lat2 = (toRadians(jupiterPosition.lat2));
  saturnPosition.long2 = (toRadians(saturnPosition.long2));
  saturnPosition.lat2 = (toRadians(saturnPosition.lat2));
  uranusPosition.long2 = (toRadians(uranusPosition.long2));
  uranusPosition.lat2 = (toRadians(uranusPosition.lat2));

  jupiterPosition.xeclip = jupiterPosition.r * cos(jupiterPosition.long2) *
                           cos(jupiterPosition.lat2);
  jupiterPosition.yeclip = jupiterPosition.r * sin(jupiterPosition.long2) *
                           cos(jupiterPosition.lat2);
  jupiterPosition.zeclip = jupiterPosition.r * sin(jupiterPosition.lat2);

  saturnPosition.xeclip =
      saturnPosition.r * cos(saturnPosition.long2) * cos(saturnPosition.lat2);
  saturnPosition.yeclip =
      saturnPosition.r * sin(saturnPosition.long2) * cos(saturnPosition.lat2);
  saturnPosition.zeclip = saturnPosition.r * sin(saturnPosition.lat2);

  uranusPosition.xeclip =
      uranusPosition.r * cos(uranusPosition.long2) * cos(uranusPosition.lat2);
  uranusPosition.yeclip =
      uranusPosition.r * sin(uranusPosition.long2) * cos(uranusPosition.lat2);
  uranusPosition.zeclip = uranusPosition.r * sin(uranusPosition.lat2);

  int planetIndex = 0;
  ret[planetIndex++] = mercuryPosition;
  ret[planetIndex++] = venusPosition;
  Position earthPosition;
  earthPosition.xeclip = earthX;
  earthPosition.yeclip = earthY;
  earthPosition.zeclip = earthZ;
  ret[planetIndex++] = earthPosition;
  ret[planetIndex++] = marsPosition;
  ret[planetIndex++] = jupiterPosition;
  ret[planetIndex++] = saturnPosition;
  ret[planetIndex++] = uranusPosition;
  ret[planetIndex] = neptunePosition;
  return ret;
}

// TODO positional object + better args
Position from_sun(double m, double e, double a, double n, double w, double i) {
  double m2 = toRadians(m);
  double e0 = m + (180 / M_PI) * e * sin(m2) * (1 + e * cos(m2));
  e0 = normalize(e0);
  double e02 = toRadians(e0);
  double e1 = normalize(e0 - (e0 - (180 / M_PI) * e * sin(e02) - m) /
                                 (1 - e * cos(e02)));
  double e_capt = toRadians(e1);
  double x = a * (cos(e_capt) - e);
  double y = a * (sqrt(1 - e * e)) * sin(e_capt);

  double r = hypot(x, y);
  double v = atan2(y, x);
  v = normalize(toDegrees(v));

  double xeclip =
      r * (cos(toRadians(n)) * cos(toRadians(v + w)) -
           sin(toRadians(n)) * sin(toRadians(v + w)) * cos(toRadians(i)));
  double yeclip =
      r * (sin(toRadians(n)) * cos(toRadians(v + w)) +
           cos(toRadians(n)) * sin(toRadians(v + w)) * cos(toRadians(i)));
  double zeclip = r * sin(toRadians(v + w)) * sin(toRadians(i));
  double long2 = atan2(yeclip, xeclip);
  long2 = normalize(toDegrees(long2));
  double lat2 = atan2(zeclip, sqrt(xeclip * xeclip + yeclip * yeclip));
  lat2 = toDegrees(lat2);

  Position position;
  position.xeclip = xeclip;
  position.yeclip = yeclip;
  position.zeclip = zeclip;
  position.long2 = long2;
  position.lat2 = lat2;
  position.r = r;

  // maybe?
  // Position position2 = {xeclip = xeclip, yeclip = yeclip, zeclip = zeclip,
  // long2 = long2,   lat2 = lat2,     r = r};
  return position;
}
