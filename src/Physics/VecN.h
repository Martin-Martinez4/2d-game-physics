#ifndef PHYSICS_VECN_H
#define PHYSICS_VECN_H

struct VecN{
    int N;
    float * data;

    VecN();
    VecN(int N);
    VecN(const VecN& v);
    ~VecN();

    void Zero();
    float Dot(const VecN& v) const;

    VecN& operator =(const VecN& v);
    VecN operator + (const VecN& v) const;
    VecN operator - (const VecN& v) const;
    VecN operator * (float n) const;

    const VecN& operator += (const VecN& v);
    const VecN& operator -= (const VecN& v);
    const VecN& operator *= (float n);

    float operator [] (const int index) const;
    float& operator [] (const int index);
};

#endif
