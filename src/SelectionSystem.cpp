//
//  SelectionSystem.cpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/25/18.
//

#include "SelectionSystem.hpp"

#include "Camera.hpp"
#include "PolyVox/RawVolume.h"
#include "PolyVox/PagedVolume.h"
#include "CastleDef.h"

#include "RegionUtils.hpp"

static std::shared_ptr<PolyVox::RawVolume<CASTLE_VOXELTYPE>> makeOutlineVolume(int thickness, Vector3DInt32 edge_lengths) {
    PolyVox::Region model_region(0, 0, 0, edge_lengths.getX(), edge_lengths.getY(), edge_lengths.getZ());
    
    auto model_volume = make_shared<PolyVox::RawVolume<CASTLE_VOXELTYPE>> (model_region);
    
    for(int x = 0; x < edge_lengths.getX(); x++) {
        for(int y = 0; y < edge_lengths.getY(); y++) {
            for(int z = 0; z < edge_lengths.getZ(); z++) {
                int x_distance = std::min(x, edge_lengths.getX() - 1 - x);
                int y_distance = std::min(y, edge_lengths.getY() - 1 - y);
                int z_distance = std::min(z, edge_lengths.getZ() - 1 - z);
                
                int distance_to_edge_xy = std::max(x_distance, y_distance);
                int distance_to_edge_yz = std::max(y_distance, z_distance);
                int distance_to_edge_zx = std::max(z_distance, x_distance);
                
                int distance_to_edge = std::min(distance_to_edge_xy, distance_to_edge_yz);
                distance_to_edge = std::min(distance_to_edge, distance_to_edge_zx);
                
                if(distance_to_edge < thickness) {
                    model_volume->setVoxel(x, y, z, 1);
                }
            }
        }
    }
    
    return model_volume;
}
static std::shared_ptr<PolyVox::RawVolume<CASTLE_VOXELTYPE>> makeFilledVolume(CASTLE_VOXELTYPE voxel_type, std::shared_ptr<PolyVox::PagedVolume<CASTLE_VOXELTYPE>> world_volume, Vector3DInt32 edge_lengths, Vector3DInt32 offset) {
    
    PolyVox::Region model_region(0, 0, 0, edge_lengths.getX(), edge_lengths.getY(), edge_lengths.getZ());
    
    auto model_volume = make_shared<PolyVox::RawVolume<CASTLE_VOXELTYPE>> (model_region);
    
    for(int x = 0; x < edge_lengths.getX(); x++) {
        for(int y = 0; y < edge_lengths.getY(); y++) {
            for(int z = 0; z < edge_lengths.getZ(); z++) {
                Vector3DInt32 point(x, y, z);
                if(world_volume->getVoxel(offset + point) == 0) {
                    model_volume->setVoxel(point, voxel_type);
                }
            }
        }
    }
    
    return model_volume;
}


void SelectionSystem::step(double t, double dt) {
    vector<EntityId> ids = entity_manager->get_ids_with_component<Selection_Component>();
    
    PagedVolume_Component* pagedVolume_component = entity_manager->get_first_component_of_type<PagedVolume_Component>();
    
    for(auto id : ids) {
        Selection_Component* selection_component = entity_manager->get_component<Selection_Component>(id);
        
        Region& region = selection_component->selection.region;
        Region properRegion = PolyVoxExtensions::createProperRegion(region);
        Vector3DInt32 edge_lengths = properRegion.getUpperCorner() - properRegion.getLowerCorner();
        edge_lengths += 1;
        
        RawVolume_Component* rawVolume_component;
        Position_Component* position_component;
        
        //Get pointer to this selections rawVolumeComponent (create one if it doesn't exist)
        if( !entity_manager->entity_has_component<RawVolume_Component>(id) ) {
            rawVolume_component = entity_manager->add_component<RawVolume_Component>(id);
            rawVolume_component->volume = makeFilledVolume(BLOCK_REDOUTLINE, pagedVolume_component->volume, edge_lengths, properRegion.getLowerCorner());
            rawVolume_component->dirty_time = t;
        } else {
            rawVolume_component = entity_manager->get_component<RawVolume_Component>(id);
        }
        
        //Get pointer to this selections positionComponent (create one if it doesn't exist)
        if( !entity_manager->entity_has_component<Position_Component>(id)) {
            position_component = entity_manager->add_component<Position_Component>(id);
            position_component->position = glm::vec3(properRegion.getLowerX(), properRegion.getLowerY(), properRegion.getLowerZ());
        } else {
            position_component = entity_manager->get_component<Position_Component>(id);
        }
        
        //Update volume and position if selection has changed
        if(selection_component->dirty_time > rawVolume_component->dirty_time) {
            rawVolume_component->volume = makeFilledVolume(BLOCK_REDOUTLINE, pagedVolume_component->volume, edge_lengths, properRegion.getLowerCorner());
            rawVolume_component->dirty_time = t;
            
            position_component->scale = vec3(1.0f);
            position_component->position = glm::vec3(properRegion.getLowerX(), properRegion.getLowerY(), properRegion.getLowerZ());
        }
    }
}
