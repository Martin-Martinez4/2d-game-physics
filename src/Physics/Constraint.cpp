#include "Constraint.h"
#include "MatMxN.h"
#include "Vec2.h"

MatMxN Constraint::GetInvM() const {
    MatMxN invM(6, 6);
	invM.Zero();
	invM.rows[0][0] = a->invMass;
	invM.rows[1][1] = a->invMass;
    invM.rows[2][2] = a->invI;
    invM.rows[3][3] = b->invMass;
	invM.rows[4][4] = b->invMass;
    invM.rows[5][5] = b->invI;
	return invM;
}

VecN Constraint::GetVelocities() const {
    VecN V(6);
	V.Zero();
	V[0] = a->velocity.x;
	V[1] = a->velocity.y;
	V[2] = a->angularVelocity;
	V[3] = b->velocity.x;
	V[4] = b->velocity.y;
	V[5] = b->angularVelocity;
	return V;
}

JointConstraint::JointConstraint(): Constraint(), jacobian(1, 6){}

JointConstraint::JointConstraint(Body* a, Body* b, const Vec2& anchorPoint): Constraint(), jacobian(1, 6){
    this->a = a;
    this->b = b;

    this->aPoint = a->WorldSpaceToLocalSpace(anchorPoint);
    this->bPoint = b->WorldSpaceToLocalSpace(anchorPoint);
} 

void JointConstraint::Solve(){
    const Vec2 pa = a->LocalSpaceToWorldSpace(aPoint);
    const Vec2 pb = b->LocalSpaceToWorldSpace(bPoint);

    const Vec2 ra = pa - a->position;
    const Vec2 rb = pb - b->position;

    Vec2 J1 = (pa - pb) * 2.0f;
    jacobian.rows[0][0] = J1.x;
    jacobian.rows[0][1] = J1.y;

    float J2 = ra.Cross(pa-pb) * 2.0f;
    jacobian.rows[0][2] = J2;

    Vec2 J3 = (pb - pa) * 2.0f;
    jacobian.rows[0][3] = J3.x;
    jacobian.rows[0][4] = J3.y;

    float J4 = rb.Cross(pb - pa) * 2.0;
    jacobian.rows[0][5] = J4;   // B angular velocity
    
    const VecN V = GetVelocities();
    const MatMxN invM = GetInvM();
  
    const MatMxN J = jacobian;
    const MatMxN Jt = jacobian.Transpose();
    
    // Calculate the numerator
    MatMxN lhs = J * invM * Jt; // A
    VecN rhs = J * V * -1.0f;  // b

    VecN lambda = MatMxN::SolveGaussSeidel(lhs, rhs);

    // Compute the final impulses with direction and magnitude
    VecN impulses = Jt * lambda;

    // Apply the impulses to both A and B
    a->ApplyImpulseLinear(Vec2(impulses[0], impulses[1])); // A linear impulse
    a->ApplyImpulseAngular(impulses[2]);                   // A angular impulse
    b->ApplyImpulseLinear(Vec2(impulses[3], impulses[4])); // B linear impulse
    b->ApplyImpulseAngular(impulses[5]);                   // B angular impulse
}


