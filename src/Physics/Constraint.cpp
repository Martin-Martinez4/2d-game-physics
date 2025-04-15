#include "Constraint.h"
#include "MatMxN.h"
#include "Vec2.h"
#include <algorithm>

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

JointConstraint::JointConstraint(): Constraint(), jacobian(1, 6), cachedLambda(1), bias(0.0f){
    cachedLambda.Zero();
}

JointConstraint::JointConstraint(Body* a, Body* b, const Vec2& anchorPoint): Constraint(), jacobian(1, 6), cachedLambda(1), bias(0.0f){
    cachedLambda.Zero();
    this->a = a;
    this->b = b;

    this->aPoint = a->WorldSpaceToLocalSpace(anchorPoint);
    this->bPoint = b->WorldSpaceToLocalSpace(anchorPoint);
} 

void JointConstraint::PreSolve(const float dt){
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
    // B angular velocity
    jacobian.rows[0][5] = J4;   

    const MatMxN Jt = jacobian.Transpose();

    // Warm Start
    // Compute the final impulses with direction and magnitude
    VecN impulses = Jt * cachedLambda;

    // Apply the impulses to both A and B
    a->ApplyImpulseLinear(Vec2(impulses[0], impulses[1])); 
    a->ApplyImpulseAngular(impulses[2]);                  
    b->ApplyImpulseLinear(Vec2(impulses[3], impulses[4])); 
    b->ApplyImpulseAngular(impulses[5]);

    // Compute the bias term (baumgarte stabilization)
    const float beta = 0.1f;

    // Positional Error
    float C = (pb - pa).Dot(pb-pa);
    C = std::max(0.0f, C - 0.01f);

    bias = (beta / dt) * C; 
}

void JointConstraint::Solve(){
    const VecN V = GetVelocities();
    const MatMxN invM = GetInvM();
  
    const MatMxN J = jacobian;
    const MatMxN Jt = jacobian.Transpose();
    
    // Calculate the numerator
    MatMxN lhs = J * invM * Jt; 
    VecN rhs = J * V * -1.0f;
    rhs[0] -= bias;

    VecN lambda = MatMxN::SolveGaussSeidel(lhs, rhs);
    cachedLambda += lambda;

    // Compute the final impulses with direction and magnitude
    VecN impulses = Jt * lambda;

    // Apply the impulses to both A and B
    // A linear impulse
    a->ApplyImpulseLinear(Vec2(impulses[0], impulses[1])); 
    // A angular impulse
    a->ApplyImpulseAngular(impulses[2]);   

    // B linear impulse                
    b->ApplyImpulseLinear(Vec2(impulses[3], impulses[4])); 
     // B angular impulse
    b->ApplyImpulseAngular(impulses[5]);                  
}

void JointConstraint::PostSolve(){}

PenetrationConstraint::PenetrationConstraint(): Constraint(), jacobian(2, 6), cachedLambda(2), bias(0.0f) {
    cachedLambda.Zero();
    friction = 0.0f;
}


PenetrationConstraint::PenetrationConstraint(Body* a, Body* b, const Vec2& aCollisionPoint, const Vec2& bCollisionPoint, const Vec2& normal): Constraint(), jacobian(2, 6), cachedLambda(2), bias(0.0f) {
    this->a = a;
    this->b = b;
    this->aPoint = a->WorldSpaceToLocalSpace(aCollisionPoint);
    this->bPoint = b->WorldSpaceToLocalSpace(bCollisionPoint);
    this->normal = a->WorldSpaceToLocalSpace(normal);
    cachedLambda.Zero();
    friction = 0.0f;
}

void PenetrationConstraint::PreSolve(const float dt) {
    // Get the collision points in world space
    const Vec2 pa = a->LocalSpaceToWorldSpace(aPoint);
    const Vec2 pb = b->LocalSpaceToWorldSpace(bPoint);
    Vec2 n = a->LocalSpaceToWorldSpace(normal);

    const Vec2 ra = pa - a->position;
    const Vec2 rb = pb - b->position;

    jacobian.Zero();

    // first row is normal force
    Vec2 J1 = -n;
    jacobian.rows[0][0] = J1.x; // A linear velocity.x
    jacobian.rows[0][1] = J1.y; // A linear velocity.y

    float J2 = -ra.Cross(n);
    jacobian.rows[0][2] = J2;   // A angular velocity

    Vec2 J3 = n;
    jacobian.rows[0][3] = J3.x; // B linear velocity.x
    jacobian.rows[0][4] = J3.y; // B linear velocity.y

    float J4 = rb.Cross(n);
    jacobian.rows[0][5] = J4;   // B angular velocity

    // second row is tangent force (friction)
    friction = std::max(a->friction, b->friction);
    if(friction > 0.0f){
        Vec2 t = n.Normal();
        jacobian.rows[1][0] = -t.x;
        jacobian.rows[1][1] = -t.y;
        jacobian.rows[1][2] = -ra.Cross(t);
        jacobian.rows[1][3] = t.x;
        jacobian.rows[1][4] = t.y;
        jacobian.rows[1][5] = rb.Cross(t);
    }

    // Warm starting (apply cached lambda)
    const MatMxN Jt = jacobian.Transpose();
    VecN impulses = Jt * cachedLambda;

    // Apply the impulses to both bodies 
    a->ApplyImpulseLinear(Vec2(impulses[0], impulses[1])); // A linear impulse
    a->ApplyImpulseAngular(impulses[2]);                   // A angular impulse
    b->ApplyImpulseLinear(Vec2(impulses[3], impulses[4])); // B linear impulse
    b->ApplyImpulseAngular(impulses[5]);                   // B angular impulse

    // Compute the bias term (baumgarte stabilization)
    const float beta = 0.2f;
    float C = (pb - pa).Dot(-n);
    C = std::min(0.0f, C + 0.01f);
    
    Vec2 va = a->velocity + Vec2(-a->angularVelocity * ra.y, a->angularVelocity * ra.x);
    Vec2 vb= b->velocity + Vec2(-b->angularVelocity * rb.y, b->angularVelocity * rb.x);
    float vrelDotNormal = (va - vb).Dot(n);
    
    float e = std::min(a->restitution, b->restitution);
    
    bias = (beta / dt) * C + (e * vrelDotNormal);
}

void PenetrationConstraint::Solve() {
    const VecN V = GetVelocities();
    const MatMxN invM = GetInvM();
  
    const MatMxN J = jacobian;
    const MatMxN Jt = jacobian.Transpose();
    
    // Compute lambda using Ax=b (Gauss-Seidel method) 
    MatMxN lhs = J * invM * Jt;  // A
    VecN rhs = J * V * -1.0f;   // b
    rhs[0] -= bias;
    VecN lambda = MatMxN::SolveGaussSeidel(lhs, rhs);
    
    // Accumulate impulses and clamp it within constraint limits
    VecN oldLambda = cachedLambda;
    cachedLambda += lambda;
    cachedLambda[0] = (cachedLambda[0] < 0.0f) ? 0.0f : cachedLambda[0];

    if(friction > 0.0){
        const float maxFriction = cachedLambda[0] * friction;
        cachedLambda[1] = std::clamp(cachedLambda[1], -maxFriction, maxFriction);
    }

    lambda = cachedLambda - oldLambda;

    // Compute the impulses with both direction and magnitude
    VecN impulses = Jt * lambda;

    // Apply the impulses to both bodies 
    a->ApplyImpulseLinear(Vec2(impulses[0], impulses[1])); // A linear impulse
    a->ApplyImpulseAngular(impulses[2]);                   // A angular impulse
    b->ApplyImpulseLinear(Vec2(impulses[3], impulses[4])); // B linear impulse
    b->ApplyImpulseAngular(impulses[5]);                   // B angular impulse
}

void PenetrationConstraint::PostSolve(){}

