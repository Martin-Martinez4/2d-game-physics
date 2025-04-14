#include "MatMxN.h"
#include "VecN.h"

MatMxN::MatMxN(): M(0), N(0), rows(nullptr) {}

MatMxN::MatMxN(int M, int N): M(M), N(N){
    rows = new VecN[M];
    for(int i = 0; i < M; ++i){
        rows[i] = VecN(N);
    }
}

MatMxN::MatMxN(const MatMxN& m){
    *this = m;
}

MatMxN::~MatMxN(){
    delete[] rows;
}

void MatMxN::Zero(){
    for(int i = 0; i < M; ++i){
        rows[i].Zero();
    }
}

MatMxN MatMxN::Transpose() const{
    MatMxN result(N,M);
    for(int i = 0; i < M; ++i){
        for(int j = 0; j < N; ++j){
            result.rows[j][i] = rows[i][j];
        }
    }

    return result;
}

const MatMxN& MatMxN::operator = (const MatMxN& m){
    M = m.M;
    N = m.N;
    rows = new VecN[M];

    for(int i = 0; i < M; ++i){
        rows[i] = m.rows[i];
    }

    return *this;
}

VecN MatMxN::operator * (const VecN& v) const {
    if(v.N != N){
        return v;
    }

    VecN result(M);
    for(int i = 0; i < M; ++i){
        result[i] = v.Dot(rows[i]);
    }
    return result;
}

MatMxN MatMxN::operator * (const MatMxN& m) const {

    if(m.M != N && m.N != M){
        return m;
    }

    MatMxN transposed = m.Transpose();
    MatMxN result(M, m.N);

    for(int i = 0; i < M; ++i){
        for(int j = 0; j < m.N; ++j){
            result.rows[i][j] = rows[i].Dot(transposed.rows[j]);
        }
    }

    return result;

}

VecN MatMxN::SolveGaussSeidel(const MatMxN& A, const VecN& b){
    const int N = b.N;

    VecN X(N);
    X.Zero();

    for(int iterations = 0; iterations < N; ++iterations){
        for(int i = 0; i < N; ++i){
            if(A.rows[i][i] != 0.0f){
                X[i] += (b[i] / A.rows[i][i]) - (A.rows[i].Dot(X) / A.rows[i][i]);
            }
        }
    }

    return X;
}



