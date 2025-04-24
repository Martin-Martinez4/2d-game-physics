#ifndef PHYSICS_COLLISION_H
#define PHYSICS_COLLISION_H

#include "Body.h"
#include "Vec2.h"
#include <vector>

namespace Collision {

    struct Contact {
        Body* a;
        Body* b;

        Vec2 start;
        Vec2 end;

        Vec2 normal;
        float depth;


        // void ResolvePenetration();
        // void ResolveCollision();
    };


    bool IsColliding(Body* a, Body* b, std::vector<Contact>& contacts);
    bool IsCollidingCircleCircle(Body* a, Body* b, std::vector<Contact>& contacts);

    float FindMinSeparation(const PolygonShape& a, const PolygonShape& b, int& indexReferenceEdge, Vec2& supportPoint);
    bool IsCollidingPolygonPolygon(Body* a, Body* b, std::vector<Contact>& contacts);

    bool IsCollidingPolygonCircle(Body* polygon, Body* circle, std::vector<Contact>& contacts);


}

#endif
