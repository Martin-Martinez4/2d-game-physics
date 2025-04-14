#include "Body.h"
#include "Graphics.h"
#include "Vec2.h"
#include <SDL_rect.h>
#include <SDL_image.h>
#include <SDL_surface.h>
#include <cmath>
#include <iostream>

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
void CircleShape::UpdateVertices(float angle, const Vec2& position){
  return;
}


PolygonShape::PolygonShape(const std::vector<Vec2> vertices){
  // TODO:
  for(auto vertex: vertices){
    this->vertices.push_back(vertex);
    worldVertices.push_back(vertex);
  }
}
PolygonShape::~PolygonShape(){
  // TODO
}
ShapeType PolygonShape::GetType() const{
  return ShapeType::POLYGON;
}
float PolygonShape::GetMomentOfInertia() const{
  return 5000.0f;
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

// Would take out if real engine
void Body::SetTexture(const char *textureFileName){
  SDL_Surface* surface = IMG_Load(textureFileName);

  if(surface){
    texture = SDL_CreateTextureFromSurface(Graphics::renderer, surface);
    SDL_FreeSurface(surface);
  }
}

bool Body::IsStatic() const {
  const float epsilon = 0.005f;
  return fabs(invMass - 0.0f) < epsilon;
}

Vec2 Body::LocalSpaceToWorldSpace(const Vec2& point) const {
  Vec2 rotated = point.Rotate(rotation);
  return rotated + position;
}

Vec2 Body::WorldSpaceToLocalSpace(const Vec2& point) const {
  float translatedX = point.x - position.x;
  float translatedY = point.y - position.y;
  float rotatedX = cos(-rotation) * translatedX - sin(-rotation) * translatedY;
  float rotatedY = cos(-rotation) * translatedY + sin(-rotation) * translatedX;
  return Vec2(rotatedX, rotatedY);
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

void Body::ApplyImpulseLinear(const Vec2& j){
  if(IsStatic()){
    return;
  }

  velocity += j * invMass;
}

void Body::ApplyImpulseAngular(const float j){
   if(IsStatic()){
    return;
  }

  angularVelocity += j * invI;
}

void Body::ApplyImpulseAtPoint(const Vec2& j, const Vec2& r){
  if(IsStatic()){
    return;
  }

  velocity += j * invMass;
  angularVelocity += r.Cross(j) * invI;
}


// void Body::Integrate(float dt) {

//   if(IsStatic()){
//     return;
//   }

//   acceleration = sumForces * invMass;
//   velocity += acceleration * dt;

//   position += velocity * dt;

//   ClearForces();
// }

void Body::IntegrateAngular(float dt){
  if(IsStatic()){
    return;
  }

  angularAcceleration = sumTorque * invI;

  angularVelocity += angularAcceleration * dt;

  rotation += angularVelocity * dt;

  ClearTorque();
}

void Body::IntegrateForces(const float dt){
  if(IsStatic()){
    return;
  }

  acceleration = sumForces * invMass;
  velocity += acceleration * dt;

  angularAcceleration = sumTorque * invI;
  angularVelocity += angularAcceleration * dt;

  ClearForces();
  ClearTorque();
}

void Body::IntegrateVelocities(const float dt){
  if (IsStatic()){

    return;
  }

  position += velocity * dt;

  rotation += angularVelocity * dt;

  shape->UpdateVertices(rotation, position);
}







