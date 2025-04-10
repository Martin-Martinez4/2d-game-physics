#include "CollisionDetection.h"
#include "Body.h"


bool CollisionDetection::IsColliding(Body* a, Body* b, Contact& contact){

    bool aIsCircle = a->shape->GetType() == ShapeType::CIRCLE;
    bool bIsCircle = b->shape->GetType() == ShapeType::CIRCLE;
    
    if(aIsCircle && bIsCircle){
        return IsCollidingCircleCircle(a, b, contact);
    }
    
    return false;
}
bool CollisionDetection::IsCollidingCircleCircle(Body* a, Body* b, Contact& contact){
    CircleShape* aCircleShape = static_cast<CircleShape*>(a->shape);
    CircleShape* bCircleShape = static_cast<CircleShape*>(b->shape);

    const Vec2 ab = b->position - a->position;
    const float radiusSum = aCircleShape->radius + bCircleShape->radius;

    bool isColliding = ab.MagnitudeSquared() <= (radiusSum * radiusSum);

    if(!isColliding){
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

void CollisionDetection::Contact::ResolvePenetration(){
    if(a->IsStatic() && b->IsStatic()){
        return;
    }

    float da = depth / (a->invMass + b->invMass) * a->invMass;
    float db = depth / (a->invMass + b->invMass) * b->invMass;

    a->position -= normal * da;
    b->position += normal * db;
}


