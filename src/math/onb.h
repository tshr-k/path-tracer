#pragma once

#include "vector3.h"

struct ONB {
public:
    UnitVector t, b, n;

    ONB(const UnitVector& normal);

    Vector global_to_local(const Vector& v) const;
    Vector local_to_global(const Vector& v) const;
};
