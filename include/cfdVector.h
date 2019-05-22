/*------------------------------------------------------------------------*/
/*  HOFlow - Higher Order Flow                                            */
/*  CFD Solver based ond CVFEM                                            */
/*------------------------------------------------------------------------*/
#ifndef CFDVECTOR_H
#define CFDVECTOR_H

#include <cmath>

typedef double scalar;
typedef int label;

/** Abstract class for a spatial vector */
class cfdVector {
public:
    cfdVector() {}
    virtual ~cfdVector() {}
    inline scalar & operator[](const label idx) {}
    inline cfdVector & operator=(cfdVector theVector) { return *this; }
    inline cfdVector & operator=(scalar C) {}
};

/** Implementation of the 2D vector */
class cfdVector2 : public cfdVector {
public:
    cfdVector2() {};
    cfdVector2(scalar x, scalar y) {
        data_[0] = x;
        data_[1] = y;
    }
    
    void set(const scalar x, const scalar y) {
        data_[0]=x;
        data_[1]=y;
    }
       
    inline scalar & operator[](const label idx) {
        return  data_[idx];
    }
    
    inline cfdVector2 & operator=(scalar C) {
        data_[0]=C;
        data_[1]=C;
        return *this;
    }
    
    inline cfdVector2 & operator=(cfdVector2 theVector) {
        data_[0] = theVector.data_[0];
        data_[1] = theVector.data_[1];
        return *this;
    }
    
    /** Product: Vector * Scalar */
    inline friend cfdVector2 operator*(const cfdVector2 & vec, const scalar & r) {
        cfdVector2 result(vec.data_[0]*r,
                          vec.data_[1]*r);
        return result;
    }
    
    /** Product: Scalar * Vector */
    inline friend cfdVector2 operator*(const scalar & r, const cfdVector2 & vec) {
        return vec * r;
    }
    
    /** Dot-Product: Vector & Vector */
    inline friend scalar operator&(const cfdVector2 & v1, const cfdVector2 & v2) {
        return (v1.data_[0]*v2.data_[0] +
                v1.data_[1]*v2.data_[1]);
    }
    
    /** Magnitude: sqrt(x^2 + y^2) */
    inline friend scalar magnitude(const cfdVector2 & v) {
        return sqrt(v.data_[0]*v.data_[0]+
                    v.data_[1]*v.data_[1]);
    }
    
private:
    scalar data_[2];
};

/** Implementation of the 3D vector */
class cfdVector3 : public cfdVector {
public:
    cfdVector3() {};
    cfdVector3(scalar x, scalar y, scalar z) {
        data_[0] = x;
        data_[1] = y;
        data_[2] = y;
    }
    
    void set(const scalar x, const scalar y, const scalar z) {
        data_[0]=x;
        data_[1]=y;
        data_[2]=z;
    }
    
    inline cfdVector3 & operator=(scalar C) {
        data_[0]=C;
        data_[1]=C;
        data_[2]=C;
        return *this;
    }
    
    inline cfdVector3 & operator=(cfdVector3 theVector) {
        data_[0] = theVector.data_[0];
        data_[1] = theVector.data_[1];
        data_[2] = theVector.data_[2];
        return *this;
    }
    
    inline scalar & operator[](const label idx) {
        return  data_[idx];
    }
    
    /** Product: Vector * Scalar */
    inline friend cfdVector3 operator*(const cfdVector3 & vec, const scalar & r) {
        cfdVector3 result(vec.data_[0]*r,
                          vec.data_[1]*r,
                          vec.data_[2]*r);
        return result;
    }
    
    /** Product: Scalar * Vector */
    inline friend cfdVector3 operator*(const scalar & r, const cfdVector3 & vec) {
        return vec * r;
    }
    
    /** Dot-Product: Vector & Vector */
    inline friend scalar operator&(const cfdVector3 & v1,const cfdVector3 & v2) {
        return (v1.data_[0]*v2.data_[0] +
                v1.data_[1]*v2.data_[1] +
                v1.data_[2]*v2.data_[2]);
    }
    
    /** Magnitude: sqrt(x^2 + y^2 + z^2) */
    inline friend scalar magnitude(const cfdVector3 & v) {
        return sqrt(v.data_[0]*v.data_[0]+
                    v.data_[1]*v.data_[1]+
                    v.data_[2]*v.data_[2]);
    }
    
private:
    scalar data_[3];
};

#endif /* CFDVECTOR_H */

