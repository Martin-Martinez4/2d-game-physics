#include "World.h"
#include "Body.h"
#include "Constants.h"
#include "Collision.h"
#include "Constraint.h"
#include "Vec2.h"
#include <vector>

World::World(float gravity, float pixelsPerMeter):gravity(-gravity), pixelsPerMeter(pixelsPerMeter){}
World::~World(){
    for(auto body: bodies){
        delete body;
    }
    for(auto particle: particles){
        delete particle;
    }
    for(auto c: constraints){
        delete c;
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

    // Create a vector of penetration constraints that will be solved frame per frame
    std::vector<PenetrationConstraint> penetrations;

    for(auto body: bodies){

        Vec2 weight = Vec2(0.0, body->mass * gravity * pixelsPerMeter);

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

     for(int i = 0; i <= bodies.size() - 1; ++i){
        for(int j = i+1; j < bodies.size(); ++j){
            Body* a = bodies[i];
            Body* b = bodies[j];


            Collision::Contact contact;
            if(Collision::IsColliding(a, b, contact)){

                PenetrationConstraint pen(contact.a, contact.b, contact.start, contact.end, contact.normal);
                penetrations.push_back(pen);

            }
        }
    }
    
    // Solve all constraints
    for (auto& constraint: constraints) {
        constraint->PreSolve(dt);
    }
    for (auto& constraint: penetrations) {
        constraint.PreSolve(dt);
    }
    for (int i = 0; i < 5; i++) { 
        for (auto& constraint: constraints) {
            constraint->Solve();
        }
        for (auto& constraint: penetrations) {
            constraint.Solve();
        }
    }
    for (auto& constraint: constraints) {
        constraint->PostSolve();
    }
    for (auto& constraint: penetrations) {
        constraint.PostSolve();
    }
    
    for(auto body: bodies){
        body->IntegrateVelocities(dt);
    }


    // CheckCollisions();
}

// void World::CheckCollisions(){
//     for(int i = 0; i <= bodies.size() - 1; ++i){
//         for(int j = i+1; j < bodies.size(); ++j){
//             Body* a = bodies[i];
//             Body* b = bodies[j];

//             a->isColliding = false;
//             b->isColliding = false;

//             Collision::Contact contact;
//             if(Collision::IsColliding(a, b, contact)){

//                 // Resolve the collision using the impulse method
//                 contact.ResolveCollision();

//             }
//         }
//     }

// }