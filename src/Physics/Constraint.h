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


        MatMxN GetInvM() const;
        VecN GetVelocities() const;
        virtual void Solve() {};
};

// class DistanceConstraint: public Constraint {

// };

class PenetrationConstraint: public Constraint {

};

class JointConstraint: public Constraint {
    private:
        MatMxN jacobian;

    public:
        JointConstraint();
        JointConstraint(Body* a, Body* b, const Vec2& anchorPoint);

        void Solve() override;
};


#endif
