#ifndef __PLANETS_H__
#define __PLANETS_H__

#define NUM_PLANETS 8

class Planet {
  private:
    double x;
    double y;
  public:
    Planet();
    void calculatePosition();
};

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

Position *coordinates(double year, double month, double day, double hour, double minute);

Position from_sun(double m, double e, double a, double n, double w, double i);
#endif
