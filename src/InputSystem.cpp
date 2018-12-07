//
//  InputSystem.cpp
//  CastleSim
//
//  Created by Alex Saalberg on 3/29/18.
//

#include "InputSystem.hpp"

#include <vector>

#include "MatrixStack.hpp"
#include "PolyVox/Picking.h"

#include "GLMUtils.hpp"

using namespace std;
using namespace glm;

void InputSystem::init() {
    input_system->addKeyControl("key_x", GLFW_KEY_X);
    input_system->addKeyControl("key_b", GLFW_KEY_B);
    input_system->addKeyControl("key_p", GLFW_KEY_P);
    input_system->addKeyControl("key_v", GLFW_KEY_V);
    input_system->addKeyControl("key_y", GLFW_KEY_Y);
    input_system->addKeyControl("key_g", GLFW_KEY_G);
    input_system->addKeyControl("key_t", GLFW_KEY_T);
    input_system->addKeyControl("key_c", GLFW_KEY_C);
    input_system->addKeyControl("key_f", GLFW_KEY_F);
    
    input_system->addKeyControl("key_w", GLFW_KEY_W);
    input_system->addKeyControl("key_a", GLFW_KEY_A);
    input_system->addKeyControl("key_s", GLFW_KEY_S);
    input_system->addKeyControl("key_d", GLFW_KEY_D);
    
    input_system->addKeyControl("key_1", GLFW_KEY_1);
    input_system->addKeyControl("key_2", GLFW_KEY_2);
    input_system->addKeyControl("key_3", GLFW_KEY_3);
    input_system->addKeyControl("key_4", GLFW_KEY_4);
    input_system->addKeyControl("key_5", GLFW_KEY_5);
    input_system->addKeyControl("key_6", GLFW_KEY_6);
    input_system->addKeyControl("key_7", GLFW_KEY_7);
    input_system->addKeyControl("key_8", GLFW_KEY_8);
    input_system->addKeyControl("key_9", GLFW_KEY_9);
    
    input_system->addKeyControl("key_up", GLFW_KEY_UP);
    input_system->addKeyControl("key_down", GLFW_KEY_DOWN);
    input_system->addKeyControl("key_left", GLFW_KEY_LEFT);
    input_system->addKeyControl("key_right", GLFW_KEY_RIGHT);
    
    input_system->addKeyControl("key_i", GLFW_KEY_I);
    input_system->addKeyControl("key_q", GLFW_KEY_Q);
    
    
    input_system->addKeyControl("key_space", GLFW_KEY_SPACE);
    input_system->addMouseclickControl("mouse_left", GLFW_MOUSE_BUTTON_LEFT);
    input_system->addMouseclickControl("mouse_right", GLFW_MOUSE_BUTTON_RIGHT);
    input_system->addMouseposXControl("mouse_x");
    input_system->addMouseposYControl("mouse_y");
}

void InputSystem::step(double t, double dt) {
    for(auto& map_entry : control_map) {
        map_entry.second.previousValue = map_entry.second.currentValue;
        //map_entry.second.currentValue = 0.0f;
        map_entry.second.pressedLastStep = map_entry.second.pressedThisStep;
        map_entry.second.pressedThisStep = false;
    }
    
    glfwPollEvents();
}

void InputSystem::addKeyControl(string name, int key) {
    Control control;
    control.type = INPUT_KEYBOARD;
    control.key = key;
    control.currentValue = 0.0f;
    
    std::pair<string, Control> element(name, control);
    control_map.insert(element);
}

void InputSystem::addMouseclickControl(string name, int button) {
    Control control;
    control.type = INPUT_MOUSE_BUTTON;
    control.key = button;
    control.currentValue = 0.0f;
    
    std::pair<string, Control> element(name, control);
    control_map.insert(element);
}
void InputSystem::addMouseposXControl(string name) {
    Control control;
    control.type = INPUT_MOUSE_POSITION_X;
    control.currentValue = 0.0f;
    
    std::pair<string, Control> element(name, control);
    control_map.insert(element);
}

void InputSystem::addMouseposYControl(string name) {
    Control control;
    control.type = INPUT_MOUSE_POSITION_Y;
    control.currentValue = 0.0f;
    
    std::pair<string, Control> element(name, control);
    control_map.insert(element);
}

Control InputSystem::getControl(std::string name) {
    Control control;
    
    auto iterator = control_map.find(name);
    if(iterator != control_map.end()) {
        control = control_map.at(name);
    } else {
        //Control doesn't exist....
        control.key = GLFW_KEY_UNKNOWN;
        printf("Unknown control: %s\n", name.c_str());
    }
    return control;
}

bool InputSystem::isControlDownThisStep(std::string name) {
    const Control& control = getControl(name);
    
    if(control.currentValue > press_threshold) {
        return true;
    }
    
    return false;
}

bool InputSystem::wasControlDownLastStep(std::string name) {
    const Control& control = getControl(name);
    
    if(control.previousValue > press_threshold) {
        return true;
    }
    
    return false;
}

float InputSystem::getCurrentControlValue(std::string name) {
    const Control& control = getControl(name);
    
    return control.currentValue;
}

float InputSystem::getPreviousControlValue(std::string name) {
    const Control& control = getControl(name);
    
    return control.previousValue;
}

bool InputSystem::wasControlPressedThisStep(std::string name) {
    const Control& control = getControl(name);
    
    return control.pressedThisStep;
}
bool InputSystem::wasControlPressedLastStep(std::string name) {
    const Control& control = getControl(name);
    
    return control.pressedLastStep;
}
bool InputSystem::wasControlReleasedThisStep(std::string name) {
    
    return (!isControlDownThisStep(name) && wasControlDownLastStep(name));
}

void InputSystem::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    for(auto& map_entry : control_map) {
        if(map_entry.second.type == INPUT_KEYBOARD) {
            if(map_entry.second.key == key) {
                if(action == GLFW_PRESS) {
                    map_entry.second.currentValue = 1.0f;
                    map_entry.second.pressedThisStep = true;
                }
                if(action == GLFW_RELEASE) {
                    map_entry.second.currentValue = 0.0f;
                }
            }
        }
    }
    
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}

void InputSystem::scrollCallback(GLFWwindow* window, double deltaX, double deltaY)
{
    
}

void InputSystem::mouseCallback(GLFWwindow *window, int button, int action, int mods)
{
    for(auto& map_entry : control_map) {
        if(map_entry.second.type == INPUT_MOUSE_BUTTON) {
            if(map_entry.second.key == button) {
                if(action == GLFW_PRESS) {
                    map_entry.second.currentValue = 1.0f;
                    map_entry.second.pressedThisStep = true;
                }
                if(action == GLFW_RELEASE) {
                    map_entry.second.currentValue = 0.0f;
                    map_entry.second.pressedThisStep = false;
                }
            }
        }
    }
}
void InputSystem::cursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
    
    int windowWidth, windowHeight;
    glfwGetWindowSize(window_manager->getHandle(), &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);
    
    float xPercent = xpos / windowWidth;
    float yPercent = ypos / windowHeight;
    //cur [0,1.0]
    
    xPercent = (xPercent * 2.0) - 1.0f;
    yPercent = (yPercent * 2.0) - 1.0f;
    //cur [-1.0, 1.0]
    
    yPercent *= -1.0f;
    //y starts 0 at top
    //(-1, -1)     (-1, -1)
    //
    //(-1, +1)     (+1, +1)
    
    for(auto& map_entry : control_map) {
        if(map_entry.second.type == INPUT_MOUSE_POSITION_X) {
            map_entry.second.currentValue = xPercent;
        }
        if(map_entry.second.type == INPUT_MOUSE_POSITION_Y) {
            map_entry.second.currentValue = yPercent;
        }
    }
}

void InputSystem::resizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}
