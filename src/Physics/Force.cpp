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

Vec2 GenerateFrictionForce(const Particle& particle, float k){
  return particle.velocity.UnitVector() * -k;
}

Vec2 GenerateGravitationalForce(const Particle& a, const Particle& b, float G){
  Vec2 d = (b.position - a.position);

  float distanceSquared = d.MagnitudeSquared();

  Vec2 attractionDirection = d.UnitVector();
  float attractionMagnitude = G * (a.mass * b.mass)/distanceSquared;

  Vec2 attractionForce = attractionDirection * attractionMagnitude;

  return attractionForce;
}

Vec2 GenerateSpringForce(const Particle& particle, Vec2 anchor, float restLength, float k){
  Vec2 d = particle.position - anchor;

  float displacement = d.Magnitude() - restLength;

  Vec2 springDirection = d.UnitVector();
  float springMagnitude = -k * displacement;

  Vec2 springForce = springDirection * springMagnitude;
  return springForce;

}

// ==== body ===

Vec2 GenerateDragForce(const Body& body, float k, float dt){
  Vec2 dragForce = Vec2(0,0);

  float speedSquared = body.velocity.MagnitudeSquared();
  float speed = body.velocity.Magnitude();

  if(speedSquared > 0){
    
    Vec2 dragDirection = body.velocity.UnitVector() * -1.0;

    float dragMagnitude = k * speedSquared;

    dragMagnitude = std::min(dragMagnitude, speed * (1/dt));
    
    dragForce = dragDirection * dragMagnitude;
  }

  return dragForce;
}

Vec2 GenerateFrictionForce(const Body& body, float k){
  return body.velocity.UnitVector() * -k;
}

Vec2 GenerateGravitationalForce(const Body& a, const Body& b, float G){
  Vec2 d = (b.position - a.position);

  float distanceSquared = d.MagnitudeSquared();

  Vec2 attractionDirection = d.UnitVector();
  float attractionMagnitude = G * (a.mass * b.mass)/distanceSquared;

  Vec2 attractionForce = attractionDirection * attractionMagnitude;

  return attractionForce;
}

Vec2 GenerateSpringForce(const Body& body, Vec2 anchor, float restLength, float k){
  Vec2 d = body.position - anchor;

  float displacement = d.Magnitude() - restLength;

  Vec2 springDirection = d.UnitVector();
  float springMagnitude = -k * displacement;

  Vec2 springForce = springDirection * springMagnitude;
  return springForce;

}

