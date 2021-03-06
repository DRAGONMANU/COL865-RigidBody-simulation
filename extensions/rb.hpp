#ifndef RB_HPP
#define RB_HPP

#include "common.hpp"
#include "draw.hpp"
#include "shape.hpp"

float GRAVITY = 0.2;

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

    vec3 forces;
    vec3 torques;

    float eta;
    float nu;

    void update(float dt)
    {
        calcIMatrix();
        linear_velocity += calcForces()/mass*dt;
        position = position + linear_velocity*dt;
        // std::cout<<linear_velocity<<"\nlin\n";        
        // std::cout<<angular_velocity<<"\nang\n";        
        angular_velocity += inverse_inertia_matrix * calcTorque() * dt;
        quat temp = quat(0,angular_velocity[0],angular_velocity[1],angular_velocity[2]);
        temp = temp * rotation;
        rotation.coeffs() += 1.0/2 * temp.coeffs() * dt;
        rotation = rotation.normalized();
        forces = vec3(0,0,0);
        torques = vec3(0,0,0);
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
            drawArrow(position,angular_velocity.normalized(),0.001);
            //TODO
            setColor(vec3(0,1,0));
            drawArrow(position,(inertia_matrix*angular_velocity).normalized(),0.01);
        }
        popTransform();
    }

    void init(int op,float m,float e,float n,float r=0,vec3 dim=vec3(0,0,0))
    {
        mass = m;
        eta = e;
        nu = n;
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
        return forces + vec3(0,-1*GRAVITY,0);
    }

    vec3 calcTorque()
    {
        return torques;
    }

    void applyImpulse(vec3 imp,vec3 r)
    {
    	forces = forces + imp;
    	torques = torques + r.cross(imp);
    }
    
    void collisionBody(RigidBody* collider,float dt)
    {
        if(shape.type == 0)
        {
            if (collider->shape.type == 0)
            {
                if((position-collider->position).norm() <= shape.radius + collider->shape.radius)
                {
        			// std::cout<<"collide"<<"\n";        

                    vec3 normal = (collider->position - position).normalized();
                    vec3 collide = position + shape.radius * normal;
                    vec3 ra = collide - collider->position;
                    vec3 rb = collide - position;
                    float e = (eta < collider->eta) ? eta : collider->eta;
                    float n = (nu > collider->nu) ? nu : collider->nu;

                    float relative = (collider->linear_velocity + collider->angular_velocity.cross(ra) - (linear_velocity + angular_velocity.cross(rb))).dot(normal);
                    if(relative<0)
                    {
        			// std::cout<<"inside\n";        
        			// std::cout<<"relative = "<<relative<<"\n";        

                        float num = -1*(1+e)*relative/dt;
                        float denom = 1/mass + 1/collider->mass;
                        vec3 imp_N = num/denom * normal;
                    	vec3 imp_fr = -1*n * imp_N.norm() * (collider->angular_velocity.cross(ra) - angular_velocity.cross(rb)).normalized(); 
                		applyImpulse(-1*imp_N,rb);
                		collider->applyImpulse(imp_N,ra);
                    	applyImpulse(imp_fr,rb);
                		collider->applyImpulse(-1*imp_fr,ra);
                    }
                }
            }
            else
            {
            	float d;
            	vec3 normal;
            	collider->shape.collisionTest(position - collider->position, d,normal);
            	if(d < shape.radius)
            	{
        			std::cout<<"normal = "<<normal<<"\n";        
                    normal = -1*normal;
                    vec3 collide = position + shape.radius * normal;
					vec3 ra = collide - collider->position;
                    vec3 rb = collide - position;
                    float e = (eta < collider->eta) ? eta : collider->eta;
                    float n = (nu > collider->nu) ? nu : collider->nu;
                    float relative = (collider->linear_velocity + collider->angular_velocity.cross(ra) - (linear_velocity + angular_velocity.cross(rb))).dot(normal);
                    if(relative<0)
                    {
                    	float num = -1*(1+e)*relative/dt;
                        float denom = 1/mass + 1/collider->mass + normal.dot((collider->inverse_inertia_matrix * ra.cross(normal)).cross(ra));
                        vec3 imp_N = num/denom * normal;
                    	// vec3 imp_fr = -1*n * imp_N.norm() * (collider->angular_velocity.cross(ra) - angular_velocity.cross(rb)).normalized(); 
                    	applyImpulse(-1*imp_N,rb);
                		collider->applyImpulse(imp_N,ra);
                    	// applyImpulse(imp_fr,rb);
                		// collider->applyImpulse(-1*imp_fr,ra);
        			}
                    
            	}
            }
        }
       else
       {
       		if (collider->shape.type == 0)
            {
				float d;
            	vec3 normal;
            	shape.collisionTest(collider->position - position, d,normal);
            	if(d < collider->shape.radius)
            	{
        			std::cout<<"asd = "<<normal<<"\n";        
                    vec3 collide = collider->position - collider->shape.radius * normal;
					vec3 ra = collide - collider->position;
                    vec3 rb = collide - position;
                    float e = (eta < collider->eta) ? eta : collider->eta;
                    float n = (nu > collider->nu) ? nu : collider->nu;
                    float relative = (collider->linear_velocity + collider->angular_velocity.cross(ra) - (linear_velocity + angular_velocity.cross(rb))).dot(normal);
                    if(relative<0)
                    {
                    	float num = -1*(1+e)*relative/dt;
                        float denom = 1/mass + 1/collider->mass + normal.dot((inverse_inertia_matrix * rb.cross(normal)).cross(rb));
                        vec3 imp_N = num/denom * normal;
                    	// vec3 imp_fr = -1*n * imp_N.norm() * (collider->angular_velocity.cross(ra) - angular_velocity.cross(rb)).normalized(); 
                    	applyImpulse(-1*imp_N,rb);
                		collider->applyImpulse(imp_N,ra);
                    	// applyImpulse(imp_fr,rb);
                		// collider->applyImpulse(-1*imp_fr,ra);
        			}
                    
            	}
            }
            else
            {

            }
       }
    }

    void collisionGround(float dt)
    {
        if(shape.type == 0)
        {
            if(position[1]<=shape.radius)
            {
                vec3 collide = vec3(position[0],0,position[2]);
                vec3 imp_N = vec3(0,0,0);
                if(linear_velocity.dot(vec3(0,-1,0))>0)
                {
                	imp_N = (1+eta)*(linear_velocity+angular_velocity.cross(collide - position)).dot(vec3(0,-1,0))*vec3(0,1,0)/dt;
                    vec3 imp_fr = -1*nu * imp_N.norm() * (linear_velocity + angular_velocity.cross(collide - position)).normalized(); 
	                applyImpulse(imp_N,vec3(0,-1*shape.radius,0));
	                applyImpulse(imp_fr,vec3(0,-1*shape.radius,0));
              	}
              	else
              	{
                    vec3 imp_fr = -1*nu * mass * GRAVITY * (linear_velocity + angular_velocity.cross(collide - position)).normalized(); 
	                applyImpulse(imp_fr,vec3(0,-1*shape.radius,0));
              	}
            }
        }
        else
        {
    		std::vector<vec3> points;
        	points.clear();
        	points.push_back(position + vec3(shape.halfSize[0],shape.halfSize[1],shape.halfSize[2]));
        	points.push_back(position + vec3(shape.halfSize[0],shape.halfSize[1],-1*shape.halfSize[2]));
        	points.push_back(position + vec3(shape.halfSize[0],-1*shape.halfSize[1],shape.halfSize[2]));
        	points.push_back(position + vec3(shape.halfSize[0],-1*shape.halfSize[1],-1*shape.halfSize[2]));
        	points.push_back(position + vec3(-1*shape.halfSize[0],shape.halfSize[1],shape.halfSize[2]));
        	points.push_back(position + vec3(-1*shape.halfSize[0],shape.halfSize[1],-1*shape.halfSize[2]));
        	points.push_back(position + vec3(-1*shape.halfSize[0],-1*shape.halfSize[1],shape.halfSize[2]));
        	points.push_back(position + vec3(-1*shape.halfSize[0],-1*shape.halfSize[1],-1*shape.halfSize[2]));
        	
        	int count = 0;
        	vec3 avg_f = vec3(0,0,0);
        	vec3 avg_t = vec3(0,0,0);
        	for (int i = 0; i < points.size(); ++i)
        	{
        		vec3 p = points[i];
        		if(p[1]<=0)
        		{
        			// std::cout<<"normal = "<<p<<"\n";        
		    		vec3 r = p - position;
		            vec3 imp_N = vec3(0,0,0);
		    		if ((linear_velocity+angular_velocity.cross(r)).dot(vec3(0,-1,0))>0)
		    		{
		    			count++;
		    			imp_N = (1+eta)*(linear_velocity+angular_velocity.cross(r)).dot(vec3(0,-1,0))*vec3(0,1,0)/dt;
		                vec3 imp_fr = -1*nu * imp_N.norm() * (linear_velocity + angular_velocity.cross(r)).normalized(); 
		                // applyImpulse(imp_N,p-position);
		                // applyImpulse(imp_fr,p-position);
		                avg_f += imp_N;
		                avg_f += imp_fr;
		                avg_t += r.cross(imp_N);
		                avg_t += r.cross(imp_fr);
		    		}
		    		else
		    		{
		    			count++; 
		    			vec3 imp_fr = -1*nu * mass * GRAVITY * (linear_velocity + angular_velocity.cross(r)).normalized(); 
                		// applyImpulse(imp_fr,p-position);
		                avg_f += imp_fr;
		                avg_t += r.cross(imp_fr);

		    		}
		    	}
        	}
        	if(count>0)
        	{
				forces += avg_f/count;
				torques += avg_t/count;	
   	 	    }
    }
    }


};
#endif
