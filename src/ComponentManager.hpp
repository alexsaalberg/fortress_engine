//
//  ComponentManager.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 3/28/18.
//

#ifndef Component_Manager_hpp
#define Component_Manager_hpp

#include "ComponentStorage.hpp"
#include "Component.hpp"

class Component_Manager {
public:
    Component_Storage<Active_Component> active_components;
    Component_Storage<Position_Component> position_components;
    Component_Storage<Model_Component> renderable_components;
    Component_Storage<Camera_Component> camera_components;
    Component_Storage<RawVolume_Component> rawVolume_components;
    Component_Storage<PagedVolume_Component> pagedVolume_components;
    Component_Storage<Selection_Component> selection_components;
    Component_Storage<Physics_Component> physics_components;
    Component_Storage<Colonist_Component> colonist_components;
    
    template<class Component_Type>
    Component_Type* add_component(int entity_id);
    
    template<class Component_Type>
    Component_Type* get_component(int entity_id);

private:

};

#endif /* Component_Manager_hpp */
