#ifndef PHYSICS_FORCE_H
#define PHYSICS_FORCE_H

#include "Vec2.h"
#include "Particle.h"
#include "Body.h"

Vec2 GenerateDragForce(const Particle& particle, float k, float dt);
Vec2 GenerateFrictionForce(const Particle& particle, float k);
Vec2 GenerateGravitationalForce(const Particle& a, const Particle& b, float G);
Vec2 GenerateSpringForce(const Particle& particle, Vec2 anchor, float restLength, float k);

Vec2 GenerateDragForce(const Body& body, float k, float dt);
Vec2 GenerateFrictionForce(const Body& body, float k);
Vec2 GenerateGravitationalForce(const Body& a, const Body& b, float G);
Vec2 GenerateSpringForce(const Body& body, Vec2 anchor, float restLength, float k);

#endif
