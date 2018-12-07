//
//  InputSystem.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 3/29/18.
//
#ifndef InputSystem_hpp
#define InputSystem_hpp

#include "System.hpp"

#include <string>

#include "WindowManager.hpp" //EventCallbacks

#include "EntityManager.hpp"
#include "Messaging.hpp"

/*
 
 In this file there's an important distinction between a "Control" and an "Input"
 
 An input is things like keyboard presses, mouse movement, etc.
 (All the stuff with callbacks basically)
 
 A control is addressed via a string ("Move_Forward") Control names must be unique
 This allows them to be remappable or multiple inputs to one control
 or multiple systems responding to one control or wahtever
 
 */

enum INPUT_TYPE{INPUT_KEYBOARD, INPUT_MOUSE_BUTTON, INPUT_MOUSE_POSITION_X, INPUT_MOUSE_POSITION_Y, INPUT_MOUSE_SCROLL};

struct Control {
    INPUT_TYPE type;
    int key;
    int mods;
    float currentValue;
    float previousValue;
    //bool downThisStep;
    //bool downLastStep;
    bool pressedThisStep;
    bool pressedLastStep;
};

class InputSystem : public System, public EventCallbacks {
public:
    const float press_threshold = 0.9f;
    
    //WindowManager* window_manager;
    
    //std::shared_ptr<EntityManager> entity_manager;
    std::shared_ptr<EventHandler> event_handler;
    
    std::map<std::string, Control> control_map;
    //Virtual
    virtual void init();
    virtual void step(double time, double delta_time);
    
    //Controls
    void addKeyControl(std::string name, int key);
    void addMouseclickControl(std::string name, int button);
    void addMouseposXControl(std::string name);
    void addMouseposYControl(std::string name);
    Control getControl(std::string name);
    
    bool isControlDownThisStep(std::string name);
    bool wasControlDownLastStep(std::string name);
    bool wasControlPressedThisStep(std::string name);
    bool wasControlPressedLastStep(std::string name);
    bool wasControlReleasedThisStep(std::string name);
    float getCurrentControlValue(std::string name);
    float getPreviousControlValue(std::string name);
    
    //Input Callbacks
    void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    void scrollCallback(GLFWwindow* window, double deltaX, double deltaY);
    void mouseCallback(GLFWwindow *window, int button, int action, int mods);
    void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);
    void resizeCallback(GLFWwindow *window, int width, int height);
};

#endif /* InputSystem_hpp */
