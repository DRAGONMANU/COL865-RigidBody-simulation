#ifndef WORLD_HPP
#define WORLD_HPP

#include "common.hpp"
#include "draw.hpp"
#include "rb.hpp"
#include <math.h>

using namespace std;

class World {
public:
    vector<RigidBody*> rbs;

    void update(float dt)
    {
        for (int i = 0; i < rbs.size(); ++i)
        {
            for (int j = i+1; j < rbs.size(); ++j)
            {
                rbs[i]->collisionBody(rbs[j],dt);
            }
            rbs[i]->collisionGround(dt);
        }
        for(RigidBody* rb : rbs)
            rb->update(dt);
    }

    void draw(bool surface, bool arrow)
    {
        for(RigidBody* rb : rbs)
            rb->draw(surface,arrow);
    }
};
#endif
