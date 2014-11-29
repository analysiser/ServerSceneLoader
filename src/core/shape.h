/*
    Shape class is referenced from PBRT shape, but with a simplified version
 */

#if defined(_MSC_VER)
#pragma once
#endif

#ifndef CORE_SHAPE_H
#define CORE_SHAPE_H

#include "pbrt.h"
#include "geometry.h"
#include "transform.h"

class Shape {
public:
    Shape (const Transform *o2w, const Transform *w2o, bool ro);
    virtual ~Shape();
    virtual BBox ObjectBound() const = 0;
    virtual BBox WorldBound() const;
    virtual bool Intersect(const Ray &ray, float *tHit,
                           float *rayEpsilon, DifferentialGeometry *dg) const;
    virtual bool IntersectP(const Ray &ray) const;
    
    virtual float Area() const;
    virtual Point Sample(float u1, float u2, Normal *Ns) const {
        Severe("Unimplemented Shape::Sample() method called");
        return Point();
    }
    
    virtual float Pdf(const Point &Pshape) const {
        return 1.f / Area();
    }
    virtual Point Sample(const Point &P, float u1, float u2,
                         Normal *Ns) const {
        return Sample(u1, u2, Ns);
    }
    virtual float Pdf(const Point &p, const Vector &wi) const;
    
    // Shape Public Data
    const Transform *ObjectToWorld, *WorldToObject;
    const bool ReverseOrientation, TransformSwapsHandedness;
    const uint32_t shapeId;
    static uint32_t nextshapeId;
};

#endif