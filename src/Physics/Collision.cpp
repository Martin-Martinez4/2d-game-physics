#include "Collision.h"
#include "Body.h"
#include "Vec2.h"
#include <limits>
#include <utility>
#include <vector>


bool Collision::IsColliding(Body* a, Body* b, std::vector<Contact>& contacts){

    bool aIsCircle = a->shape->GetType() == ShapeType::CIRCLE;
    bool bIsCircle = b->shape->GetType() == ShapeType::CIRCLE;
    bool aIsPolygon = a->shape->GetType() == ShapeType::POLYGON || a->shape->GetType() == ShapeType::BOX;
    bool bIsPolygon = b->shape->GetType() == ShapeType::POLYGON || b->shape->GetType() == ShapeType::BOX;
    
    if(aIsCircle && bIsCircle){
        return IsCollidingCircleCircle(a, b, contacts);
    }
    if(aIsPolygon && bIsPolygon){
        return IsCollidingPolygonPolygon(a, b, contacts);
    }
    if(aIsPolygon && bIsCircle){
        return IsCollidingPolygonCircle(a, b, contacts);
    }
    if(aIsCircle && bIsPolygon){
        return IsCollidingPolygonCircle(b, a, contacts);
    }
    
    return false;
}
bool Collision::IsCollidingCircleCircle(Body* a, Body* b, std::vector<Contact>& contacts){
    CircleShape* aCircleShape = (CircleShape*) a->shape;
    CircleShape* bCircleShape = (CircleShape*) b->shape;

    const Vec2 ab = b->position - a->position;
    const float radiusSum = aCircleShape->radius + bCircleShape->radius;

    bool isColliding = ab.MagnitudeSquared() <= (radiusSum * radiusSum);

    if (!isColliding) {
        return false;
    }

    Contact contact;

    contact.a = a;
    contact.b = b;

    contact.normal = ab;
    contact.normal.Normalize();

    contact.start = b->position - contact.normal * bCircleShape->radius;
    contact.end = a->position + contact.normal * aCircleShape->radius;

    contact.depth = (contact.end - contact.start).Magnitude();

    contacts.push_back(contact);
    
    return true;
}

float Collision::FindMinSeparation(const PolygonShape& a, const PolygonShape& b, int& indexReferenceEdge, Vec2& supportPoint){
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
            indexReferenceEdge = i;
            supportPoint = minVertex;
        }
    }

    return separation;
}
bool Collision::IsCollidingPolygonPolygon(Body* a, Body* b, std::vector<Contact>& contacts){
    PolygonShape* aPolygonShape = static_cast<PolygonShape*> (a->shape);
    PolygonShape* bPolygonShape = static_cast<PolygonShape*> (b->shape);

    int aIndexReferenceEdge, bIndexReferenceEdge;
    Vec2 aSupportPoint, bSupportPoint;

    float abSeparation = FindMinSeparation(*aPolygonShape, *bPolygonShape, aIndexReferenceEdge, aSupportPoint);
    if(abSeparation >= 0){
        return false;
    }

    float baSeparation = FindMinSeparation(*bPolygonShape, *aPolygonShape, bIndexReferenceEdge, bSupportPoint);
    if(baSeparation >= 0){
        return false;
    }

    PolygonShape* referenceShape;
    PolygonShape* incidentShape;
    int indexReferenceEdge;

    if(abSeparation > baSeparation){
        referenceShape = aPolygonShape;
        incidentShape = bPolygonShape;
        indexReferenceEdge = aIndexReferenceEdge;

    }else{
        referenceShape = bPolygonShape;
        incidentShape = aPolygonShape;
        indexReferenceEdge = bIndexReferenceEdge;
    }

    Vec2 referenceEdge = referenceShape->EdgeAt(indexReferenceEdge);

    int incidentIndex = incidentShape->FindIncidentEdge(referenceEdge.Normal());
    int incidentNextIndex = (incidentIndex + 1) % incidentShape->worldVertices.size();

    Vec2 v0 = incidentShape->worldVertices[incidentIndex];
    Vec2 v1 = incidentShape->worldVertices[incidentNextIndex];

    std::vector<Vec2> contactPoints = {v0, v1};
    std::vector<Vec2> clippedPoints = contactPoints;

    for(int i = 0; i < referenceShape->worldVertices.size(); ++i){
        if(i == indexReferenceEdge){
            continue;
        }

        Vec2 c0 = referenceShape->worldVertices[i];
        Vec2 c1 = referenceShape->worldVertices[(i+1)%referenceShape->worldVertices.size()];

        int numClipped = referenceShape->ClipSegmentToLine(contactPoints, clippedPoints, c0, c1);
        if(numClipped > 2){
            break;
        }

        contactPoints = clippedPoints;
    }

    auto vref = referenceShape->worldVertices[indexReferenceEdge];

    for(auto& vclip: clippedPoints){
        float separation = (vclip - vref).Dot(referenceEdge.Normal());

        if(separation <= 0){
            Contact contact;
            contact.a = a;
            contact.b = b;
            contact.normal = referenceEdge.Normal();
            contact.start = vclip;
            contact.end = vclip + contact.normal * -separation;
            if(baSeparation >= abSeparation){
                std::swap(contact.start, contact.end);
                contact.normal *= -1.0;
            }
            contacts.push_back(contact);
        }

    }

    return true;
}

bool Collision::IsCollidingPolygonCircle(Body* polygon, Body* circle, std::vector<Contact>& contacts){
   const PolygonShape* polygonShape = (PolygonShape*) polygon->shape;
    const CircleShape* circleShape = (CircleShape*) circle->shape;
    const std::vector<Vec2>& polygonVertices = polygonShape->worldVertices;

    bool isOutside = false;
    Vec2 minCurrVertex;
    Vec2 minNextVertex;
    float distanceCircleEdge = std::numeric_limits<float>::lowest();

    float maxProjection = std::numeric_limits<float>::lowest();

    // Loop all the edges of the polygon/box finding the nearest edge to the circle center
    for (int i = 0; i < polygonVertices.size(); i++) {
        int currVertex = i;
        int nextVertex = (i + 1) % polygonVertices.size();
        Vec2 edge = polygonShape->EdgeAt(currVertex);
        Vec2 normal = edge.Normal();

        // Compare the circle center with the rectangle vertex
        Vec2 vertexToCircleCenter = circle->position - polygonVertices[currVertex];
        float projection = vertexToCircleCenter.Dot(normal);

        // If we found a dot product projection that is in the positive/outside side of the normal
        if (projection > 0.0f && projection > maxProjection) {

            maxProjection = projection;
            
            // Circle center is outside the polygon
            distanceCircleEdge = projection;
            minCurrVertex = polygonVertices[currVertex];
            minNextVertex = polygonVertices[nextVertex];
            isOutside = true;
            break;
        } else {
            // Circle center is inside the rectangle, find the min edge (the one with the least negative projection)
            if (projection > distanceCircleEdge) {
                distanceCircleEdge = projection;
                minCurrVertex = polygonVertices[currVertex];
                minNextVertex = polygonVertices[nextVertex];
            }
        }
    }

    Contact contact;

    if (isOutside) {
        ///////////////////////////////////////
        // Check if we are inside region A:
        ///////////////////////////////////////
        Vec2 v1 = circle->position - minCurrVertex; // vector from the nearest vertex to the circle center
        Vec2 v2 = minNextVertex - minCurrVertex; // the nearest edge (from curr vertex to next vertex)
        if (v1.Dot(v2) < 0) {
            // Distance from vertex to circle center is greater than radius... no collision
            if (v1.Magnitude() > circleShape->radius) {
                return false;
            } else {
                // Detected collision in region A:
                contact.a = polygon;
                contact.b = circle;
                contact.depth = circleShape->radius - v1.Magnitude();
                contact.normal = v1.Normalize();
                contact.start = circle->position + (contact.normal * -circleShape->radius);
                contact.end = contact.start + (contact.normal * contact.depth);
            }
        } else {
            ///////////////////////////////////////
            // Check if we are inside region B:
            ///////////////////////////////////////
            v1 = circle->position - minNextVertex; // vector from the next nearest vertex to the circle center
            v2 = minCurrVertex - minNextVertex;   // the nearest edge
            if (v1.Dot(v2) < 0) {
                // Distance from vertex to circle center is greater than radius... no collision
                if (v1.Magnitude() > circleShape->radius) {
                    return false;
                } else {
                    // Detected collision in region B:
                    contact.a = polygon;
                    contact.b = circle;
                    contact.depth = circleShape->radius - v1.Magnitude();
                    contact.normal = v1.Normalize();
                    contact.start = circle->position + (contact.normal * -circleShape->radius);
                    contact.end = contact.start + (contact.normal * contact.depth);
                }
            } else {
                ///////////////////////////////////////
                // We are inside region C:
                ///////////////////////////////////////
                if (distanceCircleEdge > circleShape->radius) {
                    // No collision... Distance between the closest distance and the circle center is greater than the radius.
                    return false;
                } else {
                    // Detected collision in region C:
                    contact.a = polygon;
                    contact.b = circle;
                    contact.depth = circleShape->radius - distanceCircleEdge;
                    contact.normal = (minNextVertex - minCurrVertex).Normal();
                    contact.start = circle->position - (contact.normal * circleShape->radius);
                    contact.end = contact.start + (contact.normal * contact.depth);
                }
            }
        }
    } else {
        // The center of circle is inside the polygon... it is definitely colliding!
        contact.a = polygon;
        contact.b = circle;
        contact.depth = circleShape->radius - distanceCircleEdge;
        contact.normal = (minNextVertex - minCurrVertex).Normal();
        contact.start = circle->position - (contact.normal * circleShape->radius);
        contact.end = contact.start + (contact.normal * contact.depth);
    }

    contacts.push_back(contact);

    return true;
}

// void Collision::Contact::ResolvePenetration(){
//     if (a->IsStatic() && b->IsStatic()) {
//         return;
//     }

//     float da = depth / (a->invMass + b->invMass) * a->invMass;
//     float db = depth / (a->invMass + b->invMass) * b->invMass;
    
//     a->position -= normal * da;
//     b->position += normal * db;

//     a->shape->UpdateVertices(a->rotation, a->position);
//     b->shape->UpdateVertices(b->rotation, b->position);
// }

// void Collision::Contact::ResolveCollision(){

//     ResolvePenetration();

//     float e = std::min(a->restitution, b->restitution);
//     float f = std::min(a->friction, b-> friction);

//     // Vec2 vrel = a->velocity - b->velocity;
//     Vec2 ra = end -a->position;
//     Vec2 rb = start - b->position;
//     Vec2 va = a->velocity + Vec2(-a->angularVelocity * ra.y, a->angularVelocity * ra.x);
//     Vec2 vb = b->velocity + Vec2(-b->angularVelocity * rb.y, b->angularVelocity * rb.x);
//     const Vec2 vrel = va - vb;

//     float vrelDotNormal = vrel.Dot(normal);
//     float impluseMagnitude =-(1 + e) * vrelDotNormal / ((a->invMass + b->invMass) + ra.Cross(normal) * ra.Cross(normal) * a->invI + rb.Cross(normal) * rb.Cross(normal) * b->invI);
    
//     Vec2 impluseDirection = normal;

//     Vec2 jN = impluseDirection * impluseMagnitude;

//     Vec2 tangent = normal.Normal();
//     float vrelDotTangent = vrel.Dot(tangent);
//     const Vec2 impluseDirectionT = tangent;
//     const float impluseMagnitudeT = f * -(1 + e) * vrelDotTangent / ((a->invMass + b->invMass) + ra.Cross(tangent) * a->invI + rb.Cross(tangent) * rb.Cross(tangent) * b->invI);
//     Vec2 jT = impluseDirectionT * impluseMagnitudeT;

//     Vec2 j = jN + jT;

//     a->ApplyImpulseAtPoint(j, ra);
//     b->ApplyImpulseAtPoint(-j, rb);
// }



