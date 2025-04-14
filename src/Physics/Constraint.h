#ifndef PHYSICS_CONSTRAINT_H
#define PHYSICS_CONSTRAINT_H

#include "MatMxN.h"
#include "Vec2.h"
#include "VecN.h"

#include "Body.h"
class Constraint {
    public:
        Body* a;
        Body* b;

        // anchor point in a's local space
        Vec2 aPoint;
        // anchor point in b's local space
        Vec2 bPoint;

        virtual ~Constraint() = default;

        MatMxN jacobian;
        VecN cachedLambda;


        MatMxN GetInvM() const;
        VecN GetVelocities() const;
        virtual void PreSolve(const float dt) {};
        virtual void Solve() {};
        virtual void PostSolve() {};
};

// class DistanceConstraint: public Constraint {

// };

class PenetrationConstraint: public Constraint {
    private:
        MatMxN jacobian;
        VecN cachedLambda;
        float bias;
        // in a's local space
        Vec2 normal;
        float friction;


    public:
        PenetrationConstraint();
        PenetrationConstraint(Body* a, Body* b, const Vec2& aCollisionPoint, const Vec2& bCollisionPoint, const Vec2& normal);
        void PreSolve(const float dt) override;
        void Solve() override;
        void PostSolve() override;

};

class JointConstraint: public Constraint {
    private:
        MatMxN jacobian;
        VecN cachedLambda;
        float bias;

        Vec2 normal;

    public:
        JointConstraint();
        JointConstraint(Body* a, Body* b, const Vec2& anchorPoint);

        void PreSolve(const float dt) override;
        void Solve() override;
        void PostSolve() override;
};


#endif
