//
//  PhysicsSystem.cpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/8/18.
//

#include <vector>
#include "EntityManager.hpp"

#include "PhysicsSystem.hpp"

void PhysicsSystem::step(double t, double dt) {
    std::vector<EntityId> id_list = entity_manager->get_ids_with_components<Position_Component, Physics_Component>();
    
    Physics_Component* physics_component;
    Position_Component* position_component;
    
    for(EntityId id : id_list) {
        physics_component = entity_manager->get_component<Physics_Component>(id);
        position_component = entity_manager->get_component<Position_Component>(id);
        
        btTransform trans;
        physics_component->body->getMotionState()->getWorldTransform(trans);
        
        btVector3 bPosition = trans.getOrigin();
        position_component->position.x = bPosition.getX();
        position_component->position.y = bPosition.getY();
        position_component->position.z = bPosition.getZ();
        
        position_component->position += physics_component->eye_offset;
        
        btQuaternion bRotation = trans.getRotation();
        //position_component->rotation.w = bRotation.getW();
        //position_component->rotation.x = bRotation.getX();
        //position_component->rotation.y = bRotation.getY();
        //position_component->rotation.z = bRotation.getZ();
    }
}
