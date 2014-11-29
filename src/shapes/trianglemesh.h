#if defined(_MSC_VER)
#pragma once
#endif

#ifndef PBRT_SHAPES_TRIANGLEMESH_H
#define PBRT_SHAPES_TRIANGLEMESH_H

// shapes/trianglemesh.h*
#include "core/shape.h"
#include <map>
using std::map;

class TriangleMesh : public Shape
{
public:
    TriangleMesh(const Transform *o2w, const Transform *w2o, bool ro,
                 int ntris, int nverts, const int *vptr,
                 const Point *P, const Normal *N, const Vector *S,
                 const float *uv);
    
    ~TriangleMesh();
    BBox ObjectBound() const;
    BBox WorldBound() const;
    
    friend class Triangle;
    template <typename T> friend class VertexTexture;
protected:
    // TriangleMesh Protected Data
    int ntris, nverts;  // number of triangles, number of vertices
    int *vertexIndex;   // pointer to an array of vertex indices, for ith triangle,
                        // its vertex positions are P[vertexIndex[3 * i]], P[vertexIndex[3 * i + 1]], P[vertexIndex[3 * i + 2]]
    Point *p;           // Array of nv vertex positions
    Normal *n;          // (optional) Array of normal vectors
    Vector *s;          // (optional) Array of tangent vectors
    float *uvs;         // (optional) Array of uv values
//    Reference<Texture<float> > alphaTexture;  // Let's not worry about alpha texture for now
};

class Triangle : public Shape
{
public:
    // Triangle Public Methods
    Triangle(const Transform *o2w, const Transform *w2o, bool ro,
             TriangleMesh *m, int n)
    : Shape(o2w, w2o, ro) {
        mesh = m;
        v = &mesh->vertexIndex[3*n];
    }
    
    BBox ObjectBound() const;
    BBox WorldBound() const;
    bool Intersect(const Ray &ray, float *tHit, float *rayEpsilon,
                   DifferentialGeometry *dg) const;
    bool IntersectP(const Ray &ray) const;
    
    void GetUVs(float uv[3][2]) const {
        if (mesh->uvs) {
            uv[0][0] = mesh->uvs[2*v[0]];
            uv[0][1] = mesh->uvs[2*v[0]+1];
            uv[1][0] = mesh->uvs[2*v[1]];
            uv[1][1] = mesh->uvs[2*v[1]+1];
            uv[2][0] = mesh->uvs[2*v[2]];
            uv[2][1] = mesh->uvs[2*v[2]+1];
        }
        else {
            uv[0][0] = 0.; uv[0][1] = 0.;
            uv[1][0] = 1.; uv[1][1] = 0.;
            uv[2][0] = 1.; uv[2][1] = 1.;
        }
    }
    float Area() const;
    
    Point Sample(float u1, float u2, Normal *Ns) const;
    
private:
    // Triangle Private Data
    TriangleMesh *mesh;     // Which mesh it comes from
    int *v;                 // 3 vertices index in p

};

//TriangleMesh *CreateTriangleMeshShape(const Transform *o2w, const Transform *w2o,
//                                      bool reverseOrientation, const ParamSet &params,
//                                      map<string, Reference<Texture<float> > > *floatTextures = NULL);

#endif