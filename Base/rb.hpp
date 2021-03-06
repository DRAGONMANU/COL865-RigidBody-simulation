#ifndef RB_HPP
#define RB_HPP

#include "common.hpp"
#include "draw.hpp"
#include "shape.hpp"

class RigidBody {
public:
    Shape shape;
    float mass;
    vec3 color;
    vec3 position;
    quat rotation;
    mat3 inertia_matrix;    
    mat3 inverse_inertia_matrix;
    vec3 linear_velocity;
    vec3 angular_velocity;

    void update(float t)
    {
        calcIMatrix();
        linear_velocity += calcForces()/mass;
        position = position + linear_velocity*t;
        // std::cout<<linear_velocity<<"\nlin\n";        
        // std::cout<<angular_velocity<<"\nang\n";       
        angular_velocity += inverse_inertia_matrix * calcTorque();
        quat temp = quat(0,angular_velocity[0],angular_velocity[1],angular_velocity[2]);
        temp = temp * rotation;
        rotation.coeffs() += 1.0/2 * temp.coeffs() * t;
        rotation = rotation.normalized();
    }

    void draw(bool surface,bool arrow)
    {
        pushTransform();
        translate(position);
        rotate(rotation);
        setColor(color);
        shape.draw(surface);
        if(arrow)
        {
            setColor(vec3(1,0,0));
            drawArrow(position,angular_velocity.normalized(),0.01);
            //TODO
            setColor(vec3(0,1,0));
            drawArrow(position,(inertia_matrix*angular_velocity).normalized(),0.01);
        }
        popTransform();
    }

    void init(int op,float m,float r=0,vec3 dim=vec3(0,0,0))
    {
        mass = m;
        if(op==0) // sphere
        {
            shape = Shape::makeSphere(r);
            inertia_matrix = shape.moment()*mass;
        }
        else // box
        {
            shape = Shape::makeBox(dim);
            inertia_matrix = shape.moment()*mass;
        }
        inverse_inertia_matrix = inertia_matrix.inverse();
        calcIMatrix();
        // std::cout<<inverse_inertia_matrix<<"\ninv\n";
    }

    void setTransform(vec3 pos, quat rot)
    {
        position = pos;
        rotation = rot;
    }

    void calcIMatrix()
    {
        mat3 r = rotation.normalized().toRotationMatrix();
        inverse_inertia_matrix = r * inverse_inertia_matrix * r.transpose();
    }

    vec3 calcForces()
    {
        return vec3(0,0,0);
    }

    vec3 calcTorque()
    {
        return vec3(0,0,0);
    }

    void applyImpulse(vec3 imp,vec3 r)
    {
        linear_velocity = imp / mass;
        angular_velocity = inverse_inertia_matrix * r.cross(imp);
        std::cout<<"angular\n"<<angular_velocity<<"\n";       
        std::cout<<"linear\n"<<linear_velocity<<"\n";       
    }
        


};
#endif
