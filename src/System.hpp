//
//  System.h
//  CastleSim
//
//  Created by Alex Saalberg on 3/27/18.
//

#ifndef System_h
#define System_h

#include <memory>
#include <vector>
#include <btBulletDynamicsCommon.h>

#include "Program.hpp"
#include "WindowManager.hpp"

//Forward declarations
class EntityManager;
class InputSystem;

// System
class System {
public:
    static EntityManager* entity_manager;
    static EntityManager* getEntityManager() { return entity_manager; }
    
    static InputSystem* input_system;
    static InputSystem* getInputSystem() { return input_system; }
    
    static btDiscreteDynamicsWorld* bullet_dynamics_world;
    static btDiscreteDynamicsWorld* getBulletWorld() { return bullet_dynamics_world; }
    
    static WindowManager* window_manager;
    static WindowManager* getWindowManager();
    
    virtual void init() {}
    virtual void init(const std::string& resourceDirectory) {}
    virtual void step(double t, double dt) {}
    virtual void render(double t, std::shared_ptr<Program> program) {}
};

// SystemManager
class SystemManager {
private:
    std::vector<System*> systems;
public:
    void add(System* system);
    void init();
    void init(const std::string& resourceDirectory);
    void step(double t, double dt);
    void render(double t, std::shared_ptr<Program> program);
};

#endif /* System_h */
