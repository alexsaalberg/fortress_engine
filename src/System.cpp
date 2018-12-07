//
//  System.cpp
//  CastleSim
//
//  Created by Alex Saalberg on 3/27/18.
//

#include "System.hpp"

//System
EntityManager* System::entity_manager = nullptr;
InputSystem* System::input_system = nullptr;
btDiscreteDynamicsWorld* System::bullet_dynamics_world = nullptr;
WindowManager* System::window_manager = nullptr;

//SystemManager
void SystemManager::add(System *system) {
    systems.push_back(system);
}

void SystemManager::init() {
    for(System* system : systems) {
        system->init();
    }
}

void SystemManager::init(const std::string& resourceDirectory) {
    for(System* system : systems) {
        system->init(resourceDirectory);
    }
}

void SystemManager::step(double t, double dt) {
    for(System* system : systems) {
        system->step(t, dt);
    }
}

void SystemManager::render(double t, std::shared_ptr<Program> program) {
    for(System* system : systems) {
        system->render(t, program);
    }
}
