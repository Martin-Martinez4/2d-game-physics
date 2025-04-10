#include "Body.h"
#include "Graphics.h"
#include <cmath>

CircleShape::CircleShape(float radius): radius{radius}{};
CircleShape::~CircleShape(){};
ShapeType CircleShape::GetType() const {
  return ShapeType::CIRCLE;
}
float CircleShape::GetMomentOfInertia() const{
  return 0.5f * radius * radius; 
}
Shape* CircleShape::Clone() const{
  return new CircleShape(radius);
}

PolygonShape::PolygonShape(const std::vector<Vec2> vertices){
  // TODO:
}
PolygonShape::~PolygonShape(){
  // TODO
}
ShapeType PolygonShape::GetType() const{
  return ShapeType::POLYGON;
}
float PolygonShape::GetMomentOfInertia() const{
  return 0.0f;
}
Shape* PolygonShape::Clone() const{
  return new PolygonShape(vertices);
}

Vec2 PolygonShape::EdgeAt(int index) const {
  int currVertex = index;
  int nextVertex = (index + 1) % worldVertices.size();
  return worldVertices[nextVertex] - worldVertices[currVertex];
}

void PolygonShape::UpdateVertices(float angle, const Vec2& position){
  for(int i = 0; i < vertices.size(); ++i){
    worldVertices[i] = vertices[i].Rotate(angle);
    worldVertices[i] += position;
  }
}

BoxShape:: BoxShape(float width, float height):width{width}, height{height}{

  vertices.push_back(Vec2(-width / 2.0f, -height / 2.0f));
  vertices.push_back(Vec2(width / 2.0f, -height / 2.0f));
  vertices.push_back(Vec2(width / 2.0f, height / 2.0f));
  vertices.push_back(Vec2(-width / 2.0f, height / 2.0f));

  worldVertices.push_back(Vec2(-width / 2.0f, -height / 2.0f));
  worldVertices.push_back(Vec2(width / 2.0f, -height / 2.0f));
  worldVertices.push_back(Vec2(width / 2.0f, height / 2.0f));
  worldVertices.push_back(Vec2(-width / 2.0f, height / 2.0f));
}
BoxShape::~BoxShape(){
  // TODO
}
ShapeType BoxShape::GetType() const{
  return ShapeType::BOX;
}
float BoxShape::GetMomentOfInertia() const{
  return (0.083333) * (width * width + height * height);
}
Shape* BoxShape::Clone() const{
  return new BoxShape(width, height);
}

Body::Body(const Shape& shape, float x, float y, float mass) {
  this->shape = shape.Clone();
    this->position = Vec2(x, y);
    this->velocity = Vec2(0, 0);
    this->acceleration = Vec2(0, 0);
    this->rotation = 0.0;
    this->angularVelocity = 0.0;
    this->angularAcceleration = 0.0;
    this->sumForces = Vec2(0, 0);
    this->sumTorque = 0.0;
    this->mass = mass;
    if (mass != 0.0) {
        this->invMass = 1.0 / mass;
    } else {
        this->invMass = 0.0;
    }
    I = shape.GetMomentOfInertia() * mass;
    if (I != 0.0) {
        this->invI = 1.0 / I;
    } else {
        this->invI = 0.0;
    }
}

Body::~Body() {
    delete shape;
}

bool Body::IsStatic() const {
  const float epsilon = 0.005f;
  return fabs(invMass - 0.0f) < epsilon;
}

void Body::AddForce(const Vec2& force) {
    sumForces += force;
}

void Body::ClearForces() {
    sumForces = Vec2(0.0, 0.0);
}

void Body::AddTorque(float torque) {
    sumTorque += torque;
}
void Body::ClearTorque(){
  sumTorque = 0.0;
}

void Body::ApplyImpulse(const Vec2& j){
  if(IsStatic()){
    return;
  }

  velocity += j * invMass;
}

void Body::Integrate(float dt) {

  if(IsStatic()){
    return;
  }

  acceleration = sumForces * invMass;
  velocity += acceleration * dt;

  position += velocity * dt;

  ClearForces();
}

void Body::IntegrateAngular(float dt){
  if(IsStatic()){
    return;
  }

  angularAcceleration = sumTorque * invI;

  angularVelocity += angularAcceleration * dt;

  rotation += angularVelocity * dt;

  ClearTorque();
}

void Body::Update(float dt){
  Integrate(dt);
  IntegrateAngular(dt);

  switch(shape->GetType()){
    
    case ShapeType::POLYGON:
    case ShapeType::BOX:
    {
        PolygonShape* ps = (PolygonShape*) shape;
        ps->UpdateVertices(rotation, position);
        break;
    }
    default:
        break;
}
}







