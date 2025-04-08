#ifndef PHYSICS_FORCE_H
#define PHYSICS_FORCE_H

#include "Vec2.h"
#include "Particle.h"

Vec2 GenerateDragForce(const Particle& particle, float k, float dt);

#endif
