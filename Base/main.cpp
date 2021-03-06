#include "camera.hpp"
#include "draw.hpp"
#include "gui.hpp"
#include "lighting.hpp"
#include "shape.hpp"
#include "text.hpp"
#include "rb.hpp"
#include "world.hpp"

#include <cmath>

using namespace std;

Window window;
Camera camera;
Lighting lighting;
Text text;
World world;

float dt = 1/60.;
float t = 0;
bool paused = false;
bool surface = true;
bool arrow = false;

void drawWorld() {
    camera.apply(window);
    lighting.apply();
    clear(vec3(0.9,0.9,0.9));
    setColor(vec3(0.7,0.7,0.7));
    for (int i = -3; i <= 3; i++) {
        drawLine(vec3(-3,0,i), vec3(3,0,i));
        drawLine(vec3(i,0,-3), vec3(i,0,3));
    }
    world.draw(surface, arrow);
    
    setColor(vec3(0,0,0));
    if(paused)
        text.draw("PAUSED", 0.8, 0.80);    
    text.draw("WASD and LShift/LCtrl to move camera", -0.9, 0.90);
    text.draw("Mouse to rotate view", -0.9, 0.85);
    text.draw("P to play/pause animation", -0.9, 0.80);
    text.draw("V to toggle surface view", -0.9, 0.75);
    if(arrow)
    {
        text.draw("Green arrow - angular momentum", -0.9, 0.70);
        text.draw("Red arrow - angular velocity", -0.9, 0.65);    
    }
}

void update(float dt) {
    world.update(dt);
    t += dt;
}

void keyPressed(int key) {
    // See http://www.glfw.org/docs/latest/group__keys.html for key codes
    if (key == GLFW_KEY_P)
        paused = !paused;
    if (key == GLFW_KEY_V)
        surface = !surface;
    if (key == GLFW_KEY_ESCAPE)
        exit(0);
}

int main(int argc, char **argv) {
    window.create("Animation", 1024, 768);
    window.onKeyPress(keyPressed);
    camera.lookAt(vec3(1,1.5,5), vec3(0,0.5,0));
    lighting.createDefault();
    text.initialize();
    
// PART 1

    RigidBody rb;
    rb.setTransform(vec3(0,0,0),quat(1,0,0,0));
    rb.color = vec3(1,0,1);
    rb.init(1,1.0,0.25,vec3(0.2,0.4,0.05));
    rb.applyImpulse(vec3(0,0,-1),vec3(0.2,0,0));
    world.rbs.push_back(new RigidBody(rb));
    
    // RigidBody rb;
    // rb.setTransform(vec3(0,0,0),quat(sqrt(2)/2.0,0,0,sqrt(2)/2.0));
    // rb.color = vec3(1,0,1);
    // rb.init(1,1.0,0.25,vec3(0.4,0.2,0.05));
    // rb.applyImpulse(vec3(0,0,-1),vec3(0.2,0.,0));
    // world.rbs.push_back(new RigidBody(rb));

// PART 2

    // arrow = true;
    // RigidBody rb;
    // rb.setTransform(vec3(0,0,0),quat(1,0,0,0));
    // rb.color = vec3(1,0,1);
    // rb.init(1,1.0,0.25,vec3(0.2,0.4,0.05));
    // // rb.angular_velocity = vec3(5.0,0.1,0.1);
    // // rb.angular_velocity = vec3(0.1,5.0,0.1);
    // rb.angular_velocity = vec3(0.1,0.1,5.0);
    // world.rbs.push_back(new RigidBody(rb));

    while (!window.shouldClose()) {
        camera.processInput(window);
        if (!paused)
            update(dt);
        window.prepareDisplay();
        drawWorld();
        window.updateDisplay();
        window.waitForNextFrame(dt);
    }
}
