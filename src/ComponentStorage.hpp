//
//  ComponentStorage.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 3/28/18.
//

#ifndef Component_Storage_hpp
#define Component_Storage_hpp

#include <type_traits> //is_same
#include <vector>
#include <unordered_map>

#include "Component.hpp"

enum Storage_Type {MAP, VECTOR, NO_DATA};

template <class Component_Type>
class Component_Storage {
    
private:
    Storage_Type type;
    int highest_id = 0;
    int num_elements = 0;
    
    std::vector<Component_Type> components;
    std::unordered_map<int, Component_Type> components_map;
    
public:
    Component_Storage() {
        type = get_storage_type();
    }
    
    Component_Type* add_component(int entity_id) {
        Component_Type new_component;
        
        switch(type) {
            case MAP:
                components_map[entity_id] = new_component;
                return &components_map[entity_id];
                //components_map.insert(entity_id, new_component);
                break;
            case VECTOR:
                if(entity_id == components.size()) {
                    components.push_back(new_component);
                } else if (entity_id < components.size()) {
                    components.insert(components.begin() + entity_id, new_component);
                } else {
                    //Todo: Fix this
                    //Right now if the type is vector you MUST have a component for each entity
                    printf("Error: Component_Storage.cpp\n");
                    exit(-1);
                }
                return &components[entity_id];
                break;
            case NO_DATA:
                //printf("Error, NO_DATA component type.\n");
                break;
            default:
                printf("Error, default: in Component_Manager::add_entity\n");
        }
        return nullptr;
    }
    
    Component_Type* get_component(int entity_id) {
        switch(type) {
            case MAP:
                return &components_map[entity_id];
            case VECTOR:
                return &components[entity_id];
            case NO_DATA:
                return nullptr;
            default:
                return nullptr;
        }
    }
    
    Storage_Type get_storage_type() {
        if(std::is_same<Component_Type, Active_Component>::value)
            return NO_DATA;
        if(std::is_same<Component_Type, Position_Component>::value)
            return MAP;
        if(std::is_same<Component_Type, Model_Component>::value)
            return MAP;
        if(std::is_same<Component_Type, Camera_Component>::value)
            return MAP;
        if(std::is_same<Component_Type, RawVolume_Component>::value)
            return MAP;
        if(std::is_same<Component_Type, PagedVolume_Component>::value)
            return MAP;
        if(std::is_same<Component_Type, Physics_Component>::value)
            return MAP;
        if(std::is_same<Component_Type, Selection_Component>::value)
            return MAP;
        return MAP;
    }
    
};

#endif /* Component_Storage_hpp */
