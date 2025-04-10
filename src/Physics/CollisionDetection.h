#ifndef PHYSICS_COLLISION_DETECTION_H
#define PHYSICS_COLLISION_DETECTION_H

#include "Body.h"
#include "Vec2.h"

namespace CollisionDetection {

    struct Contact {
        Body* a;
        Body* b;

        Vec2 start;
        Vec2 end;

        Vec2 normal;
        float depth;
    };

    bool IsColliding(Body* a, Body* b, Contact& contact);
    bool IsCollidingCircleCircle(Body* a, Body* b, Contact& contact);

}

#endif
