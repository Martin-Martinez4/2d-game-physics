#ifndef PHYSICS_COLLISION_H
#define PHYSICS_COLLISION_H

#include "Body.h"
#include "Vec2.h"

namespace Collision {

    struct Contact {
        Body* a;
        Body* b;

        Vec2 start;
        Vec2 end;

        Vec2 normal;
        float depth;


        void ResolvePenetration();
        void ResolveCollision();
    };


    bool IsColliding(Body* a, Body* b, Contact& contact);
    bool IsCollidingCircleCircle(Body* a, Body* b, Contact& contact);

    float FindMinSeparation(const PolygonShape& a, const PolygonShape& b, Vec2& axis, Vec2& point);
    bool IsCollidingPolygonPolygon(Body* a, Body* b, Contact& contact);


}

#endif
