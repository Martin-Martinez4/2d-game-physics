#include "World.h"
#include "Body.h"
#include "Constants.h"
#include "Collision.h"
#include "Constraint.h"
#include "Vec2.h"
#include <vector>

World::World(float gravity):gravity(-gravity){}
World::~World(){
    for(auto body: bodies){
        delete body;
    }
    for(auto particle: particles){
        delete particle;
    }
}

void World::AddBody(Body* body){
    bodies.push_back(body);
}
std::vector<Body*>& World::GetBodies(){
    return bodies;
}

void World::AddConstraint(Constraint* constraint){
    constraints.push_back(constraint);
}
std::vector<Constraint*>& World::GetConstraints(){
    return constraints;
}

void World::AddForce(const Vec2& force){
    forces.push_back(force);
}
void World::AddTorque(float torque){
    torques.push_back(torque);
}

void World::Update(float dt){
    for(auto body: bodies){

        Vec2 weight = Vec2(0.0, body->mass * gravity * PIXELS_PER_METER);

        body->AddForce(weight);

        for(auto force: forces){

            body->AddForce(force);
        }

        for(auto torque: torques){

            body->AddTorque(torque);
        }

    }
    
    for(auto body: bodies){
        body->IntegrateForces(dt);
        
    }
    
    for(auto constraint: constraints){
        constraint->Solve();
    }
    
    for(auto body: bodies){
        body->IntegrateVelocities(dt);
    }


    CheckCollisions();
}

void World::CheckCollisions(){
    for(int i = 0; i <= bodies.size() - 1; ++i){
        for(int j = i+1; j < bodies.size(); ++j){
            Body* a = bodies[i];
            Body* b = bodies[j];

            a->isColliding = false;
            b->isColliding = false;

            Collision::Contact contact;
            if(Collision::IsColliding(a, b, contact)){

                // Resolve the collision using the impulse method
                contact.ResolveCollision();

            }
        }
    }

}