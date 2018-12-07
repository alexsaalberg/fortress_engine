//
//  ColonistSystem.cpp
//  CastleSim
//
//  Created by Alex Saalberg on 5/23/18.
//

#include <vector>

#include "ColonistSystem.hpp"

#include "EntityManager.hpp"

#include "CastleDef.h"

using namespace PolyVox;
using namespace std;
using namespace glm;

void ColonistSystem::step(double t, double dt) {
    vector<EntityId> colonist_ids = getEntityManager()->get_ids_with_component<Colonist_Component>();
    
    for(EntityId id : colonist_ids) {
        Colonist_Component* colonist_component = getEntityManager()->get_component<Colonist_Component>(id);
        Physics_Component* physics_component = getEntityManager()->get_component<Physics_Component>(id);
        Position_Component* position_component = getEntityManager()->get_component<Position_Component>(id);
        
        if(colonist_component->has_target) {
            Vector3DInt32& next_point = colonist_component->path_to_target.front();
            
            if(isColonistAtPoint(position_component->position, next_point)) {
                colonist_component->path_to_target.pop_front();
                if(colonist_component->path_to_target.empty()) {
                    colonist_component->has_target = false;
                    printf("Arrived!\n");
                }
                btVector3 velocity = physics_component->body->getLinearVelocity();
                btScalar speed = velocity.length();
                btScalar maxSpeed = 0.5f;
                if(speed > maxSpeed) {
                    velocity *= maxSpeed/speed;
                    physics_component->body->setLinearVelocity(velocity);
                }
            } else {
                Vector3DInt32 to_point = getVectorTowardsPoint(position_component->position, next_point);
                vec3 float_to_point = normalize(vec3(to_point.getX(), to_point.getY(), to_point.getZ()));
                float force_scalar = 300.0f * dt;
                
                int num_points_left = colonist_component->path_to_target.size();
                int slowdown_threshold = 5;
                if(num_points_left < slowdown_threshold) {
                   // force_scalar *= (num_points_left + 3 / slowdown_threshold);
                }
                btVector3 force_direction = btVector3(float_to_point.x, 0.0f, float_to_point.z);
                
                physics_component->body->applyCentralImpulse(force_direction * force_scalar);
                
                if(float_to_point.y > 0.0f && abs(physics_component->body->getLinearVelocity().getY()) < 0.01f) {
                    physics_component->body->applyCentralImpulse(btVector3(0.0, dt * (550.0f / physics_component->body->getInvMass()), 0.0));
                    printf("jump!\n");
                }
                
                btVector3 velocity = physics_component->body->getLinearVelocity();
                btScalar speed = velocity.length();
                btScalar maxSpeed = 5.0f;
                if(speed > maxSpeed) {
                    velocity *= maxSpeed/speed;
                    //physics_component->body->setLinearVelocity(velocity);
                }
            }
        }
    }
}

bool ColonistSystem::isColonistAtPoint(vec3 position, Vector3DInt32 point) {
    position += 0.5f;
    Vector3DInt32 block_position = Vector3DInt32(position.x, position.y, position.z);
    vec3 float_point = vec3(point.getX(), point.getY(), point.getZ());
    
    /*
    if(distance(position, float_point) < 0.05f) {
        return true;
    }*/
    if(block_position == point)
        return true;
    return false;
}

Vector3DInt32 ColonistSystem::getVectorTowardsPoint(vec3 position, Vector3DInt32 point) {
    position += 0.5f;
    Vector3DInt32 block_position = Vector3DInt32(position.x, position.y, position.z);
    vec3 float_point = vec3(point.getX(), point.getY(), point.getZ());
    
    return point - block_position;
}
