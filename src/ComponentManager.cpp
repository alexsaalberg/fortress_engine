//
//  Component_Manager.cpp
//  CastleSim
//
//  Created by Alex Saalberg on 3/28/18.
//

#include "ComponentManager.hpp"

template<> Active_Component* Component_Manager::add_component<Active_Component>(int entity_id) {
    return active_components.add_component(entity_id);
}

template<> Active_Component* Component_Manager::get_component<Active_Component>(int entity_id) {
    return active_components.get_component(entity_id);
}

template<> Position_Component* Component_Manager::add_component<Position_Component>(int entity_id) {
    return position_components.add_component(entity_id);
}

template<> Position_Component* Component_Manager::get_component<Position_Component>(int entity_id) {
    return position_components.get_component(entity_id);
}

template<> Model_Component* Component_Manager::add_component<Model_Component>(int entity_id) {
    return renderable_components.add_component(entity_id);
}

template<> Model_Component* Component_Manager::get_component<Model_Component>(int entity_id) {
    return renderable_components.get_component(entity_id);
}

template<> Camera_Component* Component_Manager::add_component<Camera_Component>(int entity_id) {
    return camera_components.add_component(entity_id);
}

template<> Camera_Component* Component_Manager::get_component<Camera_Component>(int entity_id) {
    return camera_components.get_component(entity_id);
}

template<> RawVolume_Component* Component_Manager::add_component<RawVolume_Component>(int entity_id) {
    return rawVolume_components.add_component(entity_id);
}

template<> RawVolume_Component* Component_Manager::get_component<RawVolume_Component>(int entity_id) {
    return rawVolume_components.get_component(entity_id);
}

template<> PagedVolume_Component* Component_Manager::add_component<PagedVolume_Component>(int entity_id) {
    return pagedVolume_components.add_component(entity_id);
}

template<> PagedVolume_Component* Component_Manager::get_component<PagedVolume_Component>(int entity_id) {
    return pagedVolume_components.get_component(entity_id);
}

template<> Selection_Component* Component_Manager::add_component<Selection_Component>(int entity_id) {
    return selection_components.add_component(entity_id);
}

template<> Selection_Component* Component_Manager::get_component<Selection_Component>(int entity_id) {
    return selection_components.get_component(entity_id);
}

template<> Physics_Component* Component_Manager::add_component<Physics_Component>(int entity_id) {
    return physics_components.add_component(entity_id);
}

template<> Physics_Component* Component_Manager::get_component<Physics_Component>(int entity_id) {
    return physics_components.get_component(entity_id);
}

template<> Colonist_Component* Component_Manager::add_component<Colonist_Component>(int entity_id) {
    return colonist_components.add_component(entity_id);
}

template<> Colonist_Component* Component_Manager::get_component<Colonist_Component>(int entity_id) {
    return colonist_components.get_component(entity_id);
}


