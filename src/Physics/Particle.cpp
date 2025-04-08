#include "Particle.h"

Particle::Particle(float x, float y, float mass) {
    this->position = Vec2(x, y);
    this->mass = mass;

    invMass = mass == 0.0 ? 0.0 : 1/mass;
    
}

Particle::~Particle() {
}

void Particle::AddForce(const Vec2& force){
    sumForces += force;
}
void Particle::ClearForces(){
    sumForces.x = 0.0f;
    sumForces.y = 0.0f;
}
void Particle::Integrate(float dt){

    acceleration = sumForces * invMass;

    velocity += acceleration * dt;
    position += velocity * dt;

    ClearForces();
}

