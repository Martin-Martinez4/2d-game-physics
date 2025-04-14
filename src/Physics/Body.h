#ifndef PHYSICS_BODY_H
#define PHYSICS_BODY_H

#include "SDL_render.h"
#include <SDL.h>
#include <SDL_image.h>
#include "Vec2.h"
#include <vector>

enum class ShapeType {
  CIRCLE,
  POLYGON,
  BOX
};

struct Shape {
  virtual ~Shape() = default;
  virtual ShapeType GetType() const = 0;
  virtual float GetMomentOfInertia() const = 0;
  virtual Shape* Clone() const = 0;
  virtual void UpdateVertices(float angle, const Vec2& position) = 0;
};
struct CircleShape: public Shape {
  float radius;

  CircleShape(const float radius);
  ~CircleShape();
  ShapeType GetType() const override;
  float GetMomentOfInertia() const override;
  Shape* Clone() const override;
  virtual void UpdateVertices(float angle, const Vec2& position) override;

};
struct PolygonShape: public Shape {
  std::vector<Vec2> vertices;
  std::vector<Vec2> worldVertices;

  PolygonShape() = default;
  PolygonShape(const std::vector<Vec2> vertices);
  virtual ~PolygonShape();
  ShapeType GetType() const override;
  float GetMomentOfInertia() const override;
  Vec2 EdgeAt(int index) const;
  Shape* Clone() const override;

  virtual void UpdateVertices(float angle, const Vec2& position) override;
};
struct BoxShape: public PolygonShape {
  float width, height;

  BoxShape(float width, float height);
  virtual ~BoxShape();
  ShapeType GetType() const override;
  float GetMomentOfInertia() const override;
  Shape* Clone() const override;

};

struct Body {
    Vec2 position;
    Vec2 velocity;
    Vec2 acceleration;

    Vec2 sumForces;

    float mass;
    float invMass;

    float rotation;
    float angularVelocity;
    float angularAcceleration;
    float I;
    float invI;

    float sumTorque;

    bool isColliding = false;

    float restitution = 0.0f;
    float friction = 0.7f;

    Shape* shape = nullptr;
    SDL_Texture* texture = nullptr;

    Body(const Shape& shape, float x, float y, float mass);
    ~Body();

    void SetTexture(const char* textureFileName);
    bool IsStatic() const;

    
    void AddForce(const Vec2& force);
    void ClearForces();
    
    void AddTorque(float torque);
    void ClearTorque();

    Vec2 LocalSpaceToWorldSpace(const Vec2& point) const;
    Vec2 WorldSpaceToLocalSpace(const Vec2& point) const;

    void ApplyImpulseLinear(const Vec2& j);
    void ApplyImpulseAngular(const float j);
    void ApplyImpulseAtPoint(const Vec2& j, const Vec2& r);

    void IntegrateLinear(float dt);
    void IntegrateAngular(float dt);

    void IntegrateForces(const float dt);
    void IntegrateVelocities(const float dt);
};

#endif