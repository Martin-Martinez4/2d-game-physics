#include "Force.h"
#include <algorithm>

Vec2 GenerateDragForce(const Particle& particle, float k, float dt){
  Vec2 dragForce = Vec2(0,0);

  float speedSquared = particle.velocity.MagnitudeSquared();
  float speed = particle.velocity.Magnitude();

  if(speedSquared > 0){
    
    Vec2 dragDirection = particle.velocity.UnitVector() * -1.0;

    float dragMagnitude = k * speedSquared;

    dragMagnitude = std::min(dragMagnitude, speed * (1/dt));
    
    dragForce = dragDirection * dragMagnitude;
  }

  return dragForce;
}
