#include "Collision.h"
#include "Body.h"
#include "Vec2.h"
#include <limits>


bool Collision::IsColliding(Body* a, Body* b, Contact& contact){

    bool aIsCircle = a->shape->GetType() == ShapeType::CIRCLE;
    bool bIsCircle = b->shape->GetType() == ShapeType::CIRCLE;
    bool aIsPolygon = a->shape->GetType() == ShapeType::POLYGON || a->shape->GetType() == ShapeType::BOX;
    bool bIsPolygon = b->shape->GetType() == ShapeType::POLYGON || b->shape->GetType() == ShapeType::BOX;
    
    if(aIsCircle && bIsCircle){
        return IsCollidingCircleCircle(a, b, contact);
    }
    if(aIsPolygon && bIsPolygon){
        return IsCollidingPolygonPolygon(a, b, contact);
    }
    
    return false;
}
bool Collision::IsCollidingCircleCircle(Body* a, Body* b, Contact& contact){
     CircleShape* aCircleShape = (CircleShape*) a->shape;
    CircleShape* bCircleShape = (CircleShape*) b->shape;

    const Vec2 ab = b->position - a->position;
    const float radiusSum = aCircleShape->radius + bCircleShape->radius;

    bool isColliding = ab.MagnitudeSquared() <= (radiusSum * radiusSum);

    if (!isColliding) {
        return false;
    }

    contact.a = a;
    contact.b = b;

    contact.normal = ab;
    contact.normal.Normalize();

    contact.start = b->position - contact.normal * bCircleShape->radius;
    contact.end = a->position + contact.normal * aCircleShape->radius;

    contact.depth = (contact.end - contact.start).Magnitude();
    
    return true;
}

float Collision::FindMinSeparation(const PolygonShape& a, const PolygonShape& b, Vec2& axis, Vec2& point){
    float separation = std::numeric_limits<float>::lowest();

    for(int i = 0; i < a.worldVertices.size(); ++i){
        Vec2 va = a.worldVertices[i];
        Vec2 normal = a.EdgeAt(i).Normal();

        float minSep = std::numeric_limits<float>::max();
        Vec2 minVertex;
        for(int j = 0; j < b.worldVertices.size(); ++j){
            Vec2 vb = b.worldVertices[j];
           float proj = (vb-va).Dot(normal);

            if(proj < minSep){
                minSep = proj;
                minVertex = vb;
            }
        }

        if(minSep > separation){
            separation = minSep;
            axis = a.EdgeAt(i);
            point = minVertex;
        }
    }

    return separation;
}
bool Collision::IsCollidingPolygonPolygon(Body* a, Body* b, Contact& contact){
    const PolygonShape* aPolygonShape = static_cast<PolygonShape*> (a->shape);
    const PolygonShape* bPolygonShape = static_cast<PolygonShape*> (b->shape);

    Vec2 aAxis, bAxis;
    Vec2 aPoint, bPoint;

    float abSeparation = FindMinSeparation(*aPolygonShape, *bPolygonShape, aAxis, aPoint);
    if(abSeparation >= 0){
        return false;
    }

    float baSeparation = FindMinSeparation(*bPolygonShape, *aPolygonShape, bAxis, bPoint);
    if(baSeparation >= 0){
        return false;
    }

    contact.a = a;
    contact.b = b;

    if(abSeparation > baSeparation){
        contact.depth = -abSeparation;
        contact.normal = aAxis.Normal();
        contact.start = aPoint;
        contact.end = aPoint + contact.normal * contact.depth;
    }else{
        contact.depth = -baSeparation;
        contact.normal = -bAxis.Normal();
        contact.start = bPoint - contact.normal * contact.depth;
        contact.end = bPoint;
    }

    return true;
}

void Collision::Contact::ResolvePenetration(){
    if (a->IsStatic() && b->IsStatic()) {
        return;
    }

    float da = depth / (a->invMass + b->invMass) * a->invMass;
    float db = depth / (a->invMass + b->invMass) * b->invMass;
    
    a->position -= normal * da;
    b->position += normal * db;
}

void Collision::Contact::ResolveCollision(){

    ResolvePenetration();

    float e = std::min(a->restitution, a->restitution);

    Vec2 vrel = a->velocity - b->velocity;

    float impluseMagnitude = -(1 + e) * vrel.Dot(normal) / (a->invMass + b->invMass);
    Vec2 impluseDirection = normal;

    Vec2 jn = impluseDirection * impluseMagnitude;

    a->ApplyImpulse(jn);
    b->ApplyImpulse(-jn);
}


