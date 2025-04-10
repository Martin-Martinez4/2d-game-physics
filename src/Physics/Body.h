#ifndef PHYSICS_BODY_H
#define PHYSICS_BODY_H

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
};
struct CircleShape: public Shape {
  float radius;

  CircleShape(const float radius);
  ~CircleShape();
  ShapeType GetType() const override;
  float GetMomentOfInertia() const override;
  Shape* Clone() const override;
};
struct PolygonShape: public Shape {
  std::vector<Vec2> vertices;
  std::vector<Vec2> worldVertices;

  PolygonShape() = default;
  PolygonShape(const std::vector<Vec2> vertices);
  virtual ~PolygonShape();
  ShapeType GetType() const override;
  float GetMomentOfInertia() const override;
  Vec2 PolygonShape::EdgeAt(int index) const;
  Shape* Clone() const override;

  void UpdateVertices(float angle, const Vec2& position);
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

    bool isColliding;

    float restitution = 0.0f;

    Shape* shape = nullptr;

    Body(const Shape& shape, float x, float y, float mass);
    ~Body();

    bool IsStatic() const;

    void AddForce(const Vec2& force);
    void ClearForces();

    void AddTorque(float torque);
    void ClearTorque();

    void ApplyImpulse(const Vec2& j);

    void Integrate(float dt);
    void IntegrateAngular(float dt);

    void Update(float dt);
};

#endif