//
//  EntityManager.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 3/27/18.
//

#ifndef EntityManager_hpp
#define EntityManager_hpp

#include <stdio.h>

#include <vector>
#include <bitset>

#include "ComponentOrdering.h"
#include "ComponentManager.hpp"
#include "Component.hpp"

typedef int32_t EntityId;
typedef std::bitset<NUM_COMPONENTS> Entity_Entry;

class EntityManager {
public:
    std::vector< Entity_Entry > entities;
    
    EntityId last_found_id = 0;
    
    EntityId create_entity();
    
    bool entityExists(EntityId id);
    
    void delete_entity(EntityId id);
    
    template <class ComponentType>
    ComponentType* add_component(EntityId id) {
        Entity_Entry* entry = get_entry_pointer_from_id(id);
        Component_Index index = get_component_index<ComponentType>();
        entry->set(index);
        
        return component_manager.add_component<ComponentType>(id);
    }
    
    template <class ComponentType>
    void remove_component(EntityId entity);
    
    template <class ComponentType>
    ComponentType* get_component(EntityId id) {
        return component_manager.get_component<ComponentType>(id);
    }
    
    //Some functions have to be in header for template reasons
    template <class ComponentType>
    std::vector<EntityId> get_ids_with_component() {
        std::vector<EntityId> return_list;
        
        Component_Index index = get_component_index<ComponentType>();
        Component_Index active_index = get_component_index<Active_Component>();
        
        for(std::vector<Entity_Entry>::size_type i = 0; i != entities.size(); i++) {
            Entity_Entry* entity = &entities[i];
            
            
            
            if(entity_has_component(entity, active_index) && entity_has_component(entity, index)) {
                return_list.push_back(i);
            }
        }
        
        return return_list;
    }
    
    template <class ComponentTypeA, class ComponentTypeB>
    std::vector<EntityId> get_ids_with_components() {
        std::vector<EntityId> return_list;
        
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
    
    ///Created to get first camera entity (should only be one).
    template <class ComponentType>
    ComponentType* get_first_component_of_type() {
        Component_Index index = get_component_index<ComponentType>();
        
        for(std::vector<Entity_Entry>::size_type i = 0; i != entities.size(); i++) {
            Entity_Entry* entity = &entities[i];
            
            if(entity_has_component(entity, index)) {
                return get_component<ComponentType>(i);
            }
        }
        
        return nullptr;
    }
    
private:
    bool entity_has_component(Entity_Entry* entity, Component_Index component);
    
    template <class Component_Type>
    bool entity_has_component(Entity_Entry* entity) {
        Component_Index component_index = get_component_index<Component_Type>();
        
        return entity_has_component(entity, component_index);
    }

public:
    
    template <class Component_Type>
    bool entity_has_component(EntityId id) {
        return entity_has_component<Component_Type>(get_entry_pointer_from_id(id));
    }
    
private:
    Component_Manager component_manager;
    
    
    
    
    Entity_Entry get_entry_from_id(EntityId id);
    Entity_Entry* get_entry_pointer_from_id(EntityId id);
    
    EntityId find_free_id();
};



#endif /* EntityManager_hpp */
