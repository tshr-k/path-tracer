#pragma once

#include "vector3.h"
#include "point.h"

class Ray {
private:
    const Point origin_;
    const Vector direction_;  // Not necessarily a unit vector

public:
    Ray(const Point& origin, const Vector& direction);
    Ray(const Point& from, const Point& to);

    const Point& origin() const;
    const Vector& direction() const;

    Point at(double t) const;
    Point operator()(double t) const;
};
