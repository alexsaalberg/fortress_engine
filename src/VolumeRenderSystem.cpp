//
//  VolumeRenderSystem.cpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/30/18.
//

#include "VolumeRenderSystem.hpp"

#include <vector>
#include <set>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp> //quat stuff
#include <glm/gtc/quaternion.hpp> //quat stuff

#include "Camera.hpp"

using namespace std;
using namespace PolyVoxExtensions;

void VolumeRenderSystem::render(double t, std::shared_ptr<Program> program) {
    //Get list of RawVolume_Component entity ids
    vector<EntityId> actual_ids = entity_manager->get_ids_with_component<RawVolume_Component>();
    
    set<EntityId> current_ids_in_map;
    for(auto const &pair : model_map) {
        current_ids_in_map.insert(pair.first);
    }
    
    for(auto const& id : actual_ids) {
        RawVolume_Component* rawVolume_component = entity_manager->get_component<RawVolume_Component>(id);
        
        auto iterator = model_map.find(id);
        if(iterator != model_map.end()) {
            //id is already in map
            current_ids_in_map.erase(iterator->first);
            
            VolumeMeshData& mesh_data = iterator->second;
            
            
            if(mesh_data.clean_time < rawVolume_component->dirty_time) {
                //regen mesh
                iterator->second = genMeshForRawVolume(t, rawVolume_component->volume);
            }
            
        } else {
            //id is not in map, gen mesh and add
            VolumeMeshData mesh_data = genMeshForRawVolume(t, rawVolume_component->volume);
            
            std::pair<EntityId, VolumeMeshData> element(id, mesh_data);
            model_map.insert(element);
        }
    }
    
    //current_ids_in_map is now the list of non-active ids in map, so remove these
    for(auto const& id : current_ids_in_map) {
        model_map.erase(id);
    }
    
    program->bind();
    
    Camera::setMaterial(program, 1);
    
    auto M = make_shared<MatrixStack>();
    M->loadIdentity();
    //Render ids
    for(auto& iterator : model_map) {
        const EntityId& id = iterator.first;
        VolumeMeshData& mesh_data = iterator.second;
        Position_Component* position_component = entity_manager->get_component<Position_Component>(id);
        
        M->pushMatrix();
            M->translate(position_component->position);
            M->multMatrix(glm::toMat4(position_component->rotation));
            M->scale(position_component->scale);
        
            renderVolume(t, mesh_data, M, program);
        
        M->popMatrix();
    }
    
    program->unbind();
}
