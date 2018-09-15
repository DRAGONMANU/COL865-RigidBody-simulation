#ifndef SHAPE_HPP
#define SHAPE_HPP

#include "common.hpp"
#include "draw.hpp"

class Shape {
public:
    int type;
    float radius;
    vec3 halfSize;
    std::vector<vec3> collisionSamples;
    Shape();
    static Shape makeSphere(float radius);
    static Shape makeBox(vec3 halfSize);
    mat3 moment();
    void draw(bool surface);
    bool collisionTest(vec3 p, float &d, vec3 &n);
};

Shape::Shape():
    radius(0), halfSize(0,0,0) {
}

Shape Shape::makeSphere(float radius) {
    Shape shape;
    shape.type = 0;
    shape.radius = radius;
    return shape;
}

Shape Shape::makeBox(vec3 halfSize) {
    Shape shape;
    shape.type = 1;
    shape.halfSize = halfSize;
    vec3 o = -halfSize;
    vec3 x = vec3(2*halfSize[0],0,0);
    vec3 y = vec3(0,2*halfSize[1],0);
    vec3 z = vec3(0,0,2*halfSize[2]);
    shape.collisionSamples.push_back(o);
    shape.collisionSamples.push_back(o + x);
    shape.collisionSamples.push_back(o + y);
    shape.collisionSamples.push_back(o + z);
    shape.collisionSamples.push_back(o + x + y);
    shape.collisionSamples.push_back(o + y + z);
    shape.collisionSamples.push_back(o + z + x);
    shape.collisionSamples.push_back(o + x + y + z);
    float res = 0.1;
    int nx = ceil(2*halfSize[0]/res);
    for (int i = 1; i < nx; i++) {
        float t = (float)i/nx;
        shape.collisionSamples.push_back(o + t*x);
        shape.collisionSamples.push_back(o + t*x + y);
        shape.collisionSamples.push_back(o + t*x + z);
        shape.collisionSamples.push_back(o + t*x + y + z);
    }
    int ny = ceil(2*halfSize[1]/res);
    for (int i = 1; i < ny; i++) {
        float t = (float)i/ny;
        shape.collisionSamples.push_back(o + t*y);
        shape.collisionSamples.push_back(o + t*y + x);
        shape.collisionSamples.push_back(o + t*y + z);
        shape.collisionSamples.push_back(o + t*y + x + z);
    }
    int nz = ceil(2*halfSize[2]/res);
    for (int i = 1; i < nz; i++) {
        float t = (float)i/nz;
        shape.collisionSamples.push_back(o + t*z);
        shape.collisionSamples.push_back(o + t*z + x);
        shape.collisionSamples.push_back(o + t*z + y);
        shape.collisionSamples.push_back(o + t*z + x + y);
    }
    return shape;
}

mat3 Shape::moment() {

    // Implement this yourself!
    // Should return the matrix M such that mass*M = I_body
    
     if (type == 0) {

        mat3 mom;
        mom << radius*radius*2.0/5, 0, 0, 0, radius*radius*2.0/5, 0, 0, 0, radius*radius*2.0/5 ;
        return mom;
    } else { // type == BOX

        mat3 mom;
        mom << (halfSize[1]*halfSize[1]+halfSize[2]*halfSize[2])*1.0/3, 0, 0, 0,(halfSize[0]*halfSize[0]+halfSize[2]*halfSize[2])*1.0/3, 0, 0, 0, (halfSize[1]*halfSize[1]+halfSize[0]*halfSize[0])*1.0/3;
        return mom;
    }
}

void Shape::draw(bool surface) {
    if (type == 0) {
        drawSphere(vec3(0,0,0), radius, surface);
    } else { // type == BOX
        drawBox(-halfSize, halfSize, surface);
    }
}

inline int sgn(float x) {return (x<0) ? -1 : (x>0) ? 1 : 0;}

bool Shape::collisionTest(vec3 p, float &d, vec3 &n) {
    if (type == 0) {
        d = p.norm() - radius;
        n = p.normalized();
        return (d < 0);
    } else { // type == BOX
        d = -1e6;
        if (std::abs(p[0]) - halfSize[0] > d) {
            d = std::abs(p[0]) - halfSize[0];
            n = vec3(sgn(p[0]),0,0);
        }
        if (std::abs(p[1]) - halfSize[1] > d) {
            d = std::abs(p[1]) - halfSize[1];
            n = vec3(0,sgn(p[1]),0);
        }
        if (std::abs(p[2]) - halfSize[2] > d) {
            d = std::abs(p[2]) - halfSize[2];
            n = vec3(0,0,sgn(p[2]));
        }
        if (d > 0) {
            vec3 c = vec3(std::min(std::max(p[0], -halfSize[0]), halfSize[0]),
                          std::min(std::max(p[1], -halfSize[1]), halfSize[1]),
                          std::min(std::max(p[2], -halfSize[2]), halfSize[2]));
            d = (p - c).norm();
            n = (p - c).normalized();
        }
        return (d < 0);
    }
}

#endif
