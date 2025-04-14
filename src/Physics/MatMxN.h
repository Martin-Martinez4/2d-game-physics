#ifndef PHYSICS_MATMxN_H
#define PHYSICS_MATMxN_H

#include "VecN.h"
struct MatMxN {
    // rows
    int M; 
    // columns
    int N;

    VecN* rows;

    MatMxN();
    MatMxN(int M, int N);
    MatMxN(const MatMxN& m);
    ~MatMxN();

    void Zero();
    MatMxN Transpose() const;

    const MatMxN& operator = (const MatMxN& m);
    VecN operator * (const VecN& v) const;
    MatMxN operator * (const MatMxN& m) const;

    static VecN SolveGaussSeidel(const MatMxN& A, const VecN& b);

};

#endif
