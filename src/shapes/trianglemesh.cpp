#include "shapes/trianglemesh.h"

// TriangleMesh Method Definitions
TriangleMesh::TriangleMesh(const Transform *o2w, const Transform *w2o,
                           bool ro, int nt, int nv, const int *vi, const Point *P,
                           const Normal *N, const Vector *S, const float *uv)
: Shape(o2w, w2o, ro) {
    ntris = nt;
    nverts = nv;
    vertexIndex = new int[3 * ntris];
    memcpy(vertexIndex, vi, 3 * ntris * sizeof(int));
    // Copy _uv_, _N_, and _S_ vertex data, if present
    if (uv) {
        uvs = new float[2*nverts];
        memcpy(uvs, uv, 2*nverts*sizeof(float));
    }
    else uvs = NULL;
    p = new Point[nverts];
    if (N) {
        n = new Normal[nverts];
        memcpy(n, N, nverts*sizeof(Normal));
    }
    else n = NULL;
    if (S) {
        s = new Vector[nverts];
        memcpy(s, S, nverts*sizeof(Vector));
    }
    else s = NULL;
    
    // Transform mesh vertices to world space
    for (int i = 0; i < nverts; ++i)
        p[i] = (*ObjectToWorld)(P[i]);
}

TriangleMesh::~TriangleMesh() {
    delete[] vertexIndex;
    delete[] p;
    delete[] s;
    delete[] n;
    delete[] uvs;
}

// TODO: positions sent in should be object space
// maybe a point to optimize
BBox TriangleMesh::ObjectBound() const {
    BBox objectBounds;
    for (int i = 0; i < nverts; i++)
        objectBounds = Union(objectBounds, (*WorldToObject)(p[i]));
    return objectBounds;
}


BBox TriangleMesh::WorldBound() const {
    BBox worldBounds;
    for (int i = 0; i < nverts; i++)
        worldBounds = Union(worldBounds, p[i]);
    return worldBounds;
}

BBox Triangle::ObjectBound() const {
    // Get triangle vertices in _p1_, _p2_, and _p3_
    const Point &p1 = mesh->p[v[0]];
    const Point &p2 = mesh->p[v[1]];
    const Point &p3 = mesh->p[v[2]];
    return Union(BBox((*WorldToObject)(p1), (*WorldToObject)(p2)),
                 (*WorldToObject)(p3));
}

BBox Triangle::WorldBound() const {
    // Get triangle vertices in _p1_, _p2_, and _p3_
    const Point &p1 = mesh->p[v[0]];
    const Point &p2 = mesh->p[v[1]];
    const Point &p3 = mesh->p[v[2]];
    return Union(BBox(p1, p2), p3);
}

// ray in world space
bool Triangle::Intersect(const Ray &ray, float *tHit, float *rayEpsilon,
                         DifferentialGeometry *dg) const {
//    PBRT_RAY_TRIANGLE_INTERSECTION_TEST(const_cast<Ray *>(&ray), const_cast<Triangle *>(this));
    // Compute $\VEC{s}_1$
    
    // Get triangle vertices in _p1_, _p2_, and _p3_
    const Point &p1 = mesh->p[v[0]];
    const Point &p2 = mesh->p[v[1]];
    const Point &p3 = mesh->p[v[2]];
    Vector e1 = p2 - p1;
    Vector e2 = p3 - p1;
    Vector s1 = Cross(ray.d, e2);
    float divisor = Dot(s1, e1);
    
    if (divisor == 0.)
        return false;
    float invDivisor = 1.f / divisor;
    
    // Compute first barycentric coordinate
    Vector s = ray.o - p1;
    float b1 = Dot(s, s1) * invDivisor;
    if (b1 < 0. || b1 > 1.)
        return false;
    
    // Compute second barycentric coordinate
    Vector s2 = Cross(s, e1);
    float b2 = Dot(ray.d, s2) * invDivisor;
    if (b2 < 0. || b1 + b2 > 1.)
        return false;
    
    // Compute _t_ to intersection point
    float t = Dot(e2, s2) * invDivisor;
    if (t < ray.mint || t > ray.maxt)
        return false;
    
    // Compute triangle partial derivatives
    Vector dpdu, dpdv;
    float uvs[3][2];
    GetUVs(uvs);
    
    // Compute deltas for triangle partial derivatives
    float du1 = uvs[0][0] - uvs[2][0];
    float du2 = uvs[1][0] - uvs[2][0];
    float dv1 = uvs[0][1] - uvs[2][1];
    float dv2 = uvs[1][1] - uvs[2][1];
    Vector dp1 = p1 - p3, dp2 = p2 - p3;
    float determinant = du1 * dv2 - dv1 * du2;
    if (determinant == 0.f) {
        // Handle zero determinant for triangle partial derivative matrix
        CoordinateSystem(Normalize(Cross(e2, e1)), &dpdu, &dpdv);
    }
    else {
        float invdet = 1.f / determinant;
        dpdu = ( dv2 * dp1 - dv1 * dp2) * invdet;
        dpdv = (-du2 * dp1 + du1 * dp2) * invdet;
    }
    
    // Interpolate $(u,v)$ triangle parametric coordinates
    float b0 = 1 - b1 - b2;
    float tu = b0*uvs[0][0] + b1*uvs[1][0] + b2*uvs[2][0];
    float tv = b0*uvs[0][1] + b1*uvs[1][1] + b2*uvs[2][1];
    
    // Test intersection against alpha texture, if present
    if (ray.depth != -1) {
        if (mesh->alphaTexture) {
            DifferentialGeometry dgLocal(ray(t), dpdu, dpdv,
                                         Normal(0,0,0), Normal(0,0,0),
                                         tu, tv, this);
            if (mesh->alphaTexture->Evaluate(dgLocal) == 0.f)
                return false;
        }
    }
    
    // Fill in _DifferentialGeometry_ from triangle hit
    *dg = DifferentialGeometry(ray(t), dpdu, dpdv,
                               Normal(0,0,0), Normal(0,0,0),
                               tu, tv, this);
    *tHit = t;
    *rayEpsilon = 1e-3f * *tHit;
//    PBRT_RAY_TRIANGLE_INTERSECTION_HIT(const_cast<Ray *>(&ray), t);
    return true;
}


bool Triangle::IntersectP(const Ray &ray) const {
//    PBRT_RAY_TRIANGLE_INTERSECTIONP_TEST(const_cast<Ray *>(&ray), const_cast<Triangle *>(this));
    // Compute $\VEC{s}_1$
    
    // Get triangle vertices in _p1_, _p2_, and _p3_
    const Point &p1 = mesh->p[v[0]];
    const Point &p2 = mesh->p[v[1]];
    const Point &p3 = mesh->p[v[2]];
    Vector e1 = p2 - p1;
    Vector e2 = p3 - p1;
    Vector s1 = Cross(ray.d, e2);
    float divisor = Dot(s1, e1);
    
    if (divisor == 0.)
        return false;
    float invDivisor = 1.f / divisor;
    
    // Compute first barycentric coordinate
    Vector d = ray.o - p1;
    float b1 = Dot(d, s1) * invDivisor;
    if (b1 < 0. || b1 > 1.)
        return false;
    
    // Compute second barycentric coordinate
    Vector s2 = Cross(d, e1);
    float b2 = Dot(ray.d, s2) * invDivisor;
    if (b2 < 0. || b1 + b2 > 1.)
        return false;
    
    // Compute _t_ to intersection point
    float t = Dot(e2, s2) * invDivisor;
    if (t < ray.mint || t > ray.maxt)
        return false;
    
    // Test shadow ray intersection against alpha texture, if present
//    if (ray.depth != -1 && mesh->alphaTexture) {
//        // Compute triangle partial derivatives
//        Vector dpdu, dpdv;
//        float uvs[3][2];
//        GetUVs(uvs);
//        
//        // Compute deltas for triangle partial derivatives
//        float du1 = uvs[0][0] - uvs[2][0];
//        float du2 = uvs[1][0] - uvs[2][0];
//        float dv1 = uvs[0][1] - uvs[2][1];
//        float dv2 = uvs[1][1] - uvs[2][1];
//        Vector dp1 = p1 - p3, dp2 = p2 - p3;
//        float determinant = du1 * dv2 - dv1 * du2;
//        if (determinant == 0.f) {
//            // Handle zero determinant for triangle partial derivative matrix
//            CoordinateSystem(Normalize(Cross(e2, e1)), &dpdu, &dpdv);
//        }
//        else {
//            float invdet = 1.f / determinant;
//            dpdu = ( dv2 * dp1 - dv1 * dp2) * invdet;
//            dpdv = (-du2 * dp1 + du1 * dp2) * invdet;
//        }
//        
//        // Interpolate $(u,v)$ triangle parametric coordinates
//        float b0 = 1 - b1 - b2;
//        float tu = b0*uvs[0][0] + b1*uvs[1][0] + b2*uvs[2][0];
//        float tv = b0*uvs[0][1] + b1*uvs[1][1] + b2*uvs[2][1];
//        DifferentialGeometry dgLocal(ray(t), dpdu, dpdv,
//                                     Normal(0,0,0), Normal(0,0,0),
//                                     tu, tv, this);
//        if (mesh->alphaTexture->Evaluate(dgLocal) == 0.f)
//            return false;
//    }
//    PBRT_RAY_TRIANGLE_INTERSECTIONP_HIT(const_cast<Ray *>(&ray), t);
    return true;
}

float Triangle::Area() const {
    // Get triangle vertices in _p1_, _p2_, and _p3_
    const Point &p1 = mesh->p[v[0]];
    const Point &p2 = mesh->p[v[1]];
    const Point &p3 = mesh->p[v[2]];
    return 0.5f * Cross(p2-p1, p3-p1).Length();
}


void Triangle::GetShadingGeometry(const Transform &obj2world,
                                  const DifferentialGeometry &dg,
                                  DifferentialGeometry *dgShading) const {
    if (!mesh->n && !mesh->s) {
        *dgShading = dg;
        return;
    }
    // Initialize _Triangle_ shading geometry with _n_ and _s_
    
    // Compute barycentric coordinates for point
    float b[3];
    
    // Initialize _A_ and _C_ matrices for barycentrics
    float uv[3][2];
    GetUVs(uv);
    float A[2][2] =
    { { uv[1][0] - uv[0][0], uv[2][0] - uv[0][0] },
        { uv[1][1] - uv[0][1], uv[2][1] - uv[0][1] } };
    float C[2] = { dg.u - uv[0][0], dg.v - uv[0][1] };
    if (!SolveLinearSystem2x2(A, C, &b[1], &b[2])) {
        // Handle degenerate parametric mapping
        b[0] = b[1] = b[2] = 1.f/3.f;
    }
    else
        b[0] = 1.f - b[1] - b[2];
    
    // Use _n_ and _s_ to compute shading tangents for triangle, _ss_ and _ts_
    Normal ns;
    Vector ss, ts;
    if (mesh->n) ns = Normalize(obj2world(b[0] * mesh->n[v[0]] +
                                          b[1] * mesh->n[v[1]] +
                                          b[2] * mesh->n[v[2]]));
    else   ns = dg.nn;
    if (mesh->s) ss = Normalize(obj2world(b[0] * mesh->s[v[0]] +
                                          b[1] * mesh->s[v[1]] +
                                          b[2] * mesh->s[v[2]]));
    else   ss = Normalize(dg.dpdu);
    
    ts = Cross(ss, ns);
    if (ts.LengthSquared() > 0.f) {
        ts = Normalize(ts);
        ss = Cross(ts, ns);
    }
    else
        CoordinateSystem((Vector)ns, &ss, &ts);
    Normal dndu, dndv;
    
    // Compute $\dndu$ and $\dndv$ for triangle shading geometry
    if (mesh->n) {
        float uvs[3][2];
        GetUVs(uvs);
        // Compute deltas for triangle partial derivatives of normal
        float du1 = uvs[0][0] - uvs[2][0];
        float du2 = uvs[1][0] - uvs[2][0];
        float dv1 = uvs[0][1] - uvs[2][1];
        float dv2 = uvs[1][1] - uvs[2][1];
        Normal dn1 = mesh->n[v[0]] - mesh->n[v[2]];
        Normal dn2 = mesh->n[v[1]] - mesh->n[v[2]];
        float determinant = du1 * dv2 - dv1 * du2;
        if (determinant == 0.f)
            dndu = dndv = Normal(0,0,0);
        else {
            float invdet = 1.f / determinant;
            dndu = ( dv2 * dn1 - dv1 * dn2) * invdet;
            dndv = (-du2 * dn1 + du1 * dn2) * invdet;
        }
    }
    else
        dndu = dndv = Normal(0,0,0);
    *dgShading = DifferentialGeometry(dg.p, ss, ts,
                                      obj2world(dndu), obj2world(dndv),
                                      dg.u, dg.v, dg.shape);
    dgShading->dudx = dg.dudx;  dgShading->dvdx = dg.dvdx;
    dgShading->dudy = dg.dudy;  dgShading->dvdy = dg.dvdy;
    dgShading->dpdx = dg.dpdx;  dgShading->dpdy = dg.dpdy;
}


Point Triangle::Sample(float u1, float u2, Normal *Ns) const {
    float b1, b2;
    UniformSampleTriangle(u1, u2, &b1, &b2);
    // Get triangle vertices in _p1_, _p2_, and _p3_
    const Point &p1 = mesh->p[v[0]];
    const Point &p2 = mesh->p[v[1]];
    const Point &p3 = mesh->p[v[2]];
    Point p = b1 * p1 + b2 * p2 + (1.f - b1 - b2) * p3;
    Normal n = Normal(Cross(p2-p1, p3-p1));
    *Ns = Normalize(n);
    if (ReverseOrientation) *Ns *= -1.f;
    return p;
}


