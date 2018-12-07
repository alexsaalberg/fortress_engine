//
//  EntityManager.cpp
//  CastleSim
//
//  Created by Alex Saalberg on 3/27/18.
//

#include "EntityManager.hpp"

using namespace std;

/* PUBLIC */
EntityId EntityManager::create_entity() {
    EntityId new_index = find_free_id();
   
    Entity_Entry new_entity;
    
    if(new_index == -1) {
        new_index = entities.size();
        entities.push_back(new_entity);
    } else {
        entities[new_index] = new_entity;
    }
    
    add_component<Active_Component>(new_index);
    return new_index;
}

void EntityManager::delete_entity(EntityId id) {
    remove_component<Active_Component>(id);
}

bool EntityManager::entityExists(EntityId id) {
    if(id < 0 || id > entities.size()) {
        return false; //entity id out of range
    }
    if(entity_has_component<Active_Component>(id)) {
        return true; //entity is active, therefore exists
    }
    return false;
}


template <class ComponentType>
void EntityManager::remove_component(EntityId id) {
    Entity_Entry* entry = get_entry_pointer_from_id(id);
    Component_Index index = get_component_index<ComponentType>();
    
    entry->reset(index);
}


/*
template <class ComponentTypeA, class ComponentTypeB>
vector<Entity_Id> EntityManager::get_ids_with_components() {
    vector<Entity_Id> return_list;
    
    Component_Index a_index = get_component_index<ComponentTypeA>();
    Component_Index b_index = get_component_index<ComponentTypeB>();
    
    for(std::vector<Entity_Entry>::size_type i = 0; i != entities.size(); i++) {
        Entity_Entry* entity = &entities[i];
        
        if(entity_has_component(entity, a_index)) {
            if(entity_has_component(entity, b_index)) {
                return_list.push_back(i);
            }
        }
    }
    
    return return_list;
}
*/
 
/* PRIVATE */
bool EntityManager::entity_has_component(Entity_Entry* entity, Component_Index component) {
    if(entity->operator[](component)) {
        return true;
    }
    return false;
}



Entity_Entry EntityManager::get_entry_from_id(EntityId id) {
    return entities[id];
}

Entity_Entry* EntityManager::get_entry_pointer_from_id(EntityId id) {
    return &entities[id];
}

EntityId EntityManager::find_free_id() {
    EntityId num_entities = entities.size();
    
    for(EntityId entity = 0; entity < num_entities; entity++) {
        if( !entity_has_component<Active_Component>(entity) ) {
            //Reuse entity
            return entity;
        }
    }
    
    //New entity
    return -1;
}


