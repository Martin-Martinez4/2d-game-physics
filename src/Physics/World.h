#ifndef PHYSICS_WORLD_H
#define PHYSICS_WORLD_H

#include "Body.h"
#include "Constraint.h"
#include "Particle.h"
#include "Vec2.h"
#include <vector>
class World {

    private:
        float gravity = 9.8;
        float pixelsPerMeter = 50.0f;

        std::vector<Body*> bodies;
        std::vector<Particle*> particles;

        std::vector<Constraint*> constraints;

        std::vector<Vec2> forces;
        std::vector<float> torques;

    public:
        World(float gravity, float pixelsPerMeter);
        ~World();

        void AddBody(Body* body);
        std::vector<Body*>& GetBodies();

        void AddConstraint(Constraint* constraint);
        std::vector<Constraint*>& GetConstraints();

        void AddForce(const Vec2& force);
        void AddTorque(float torque);

        void Update(float dt);

        // void CheckCollisions();
        
};

#endif
