//
//  PickSystem.cpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/24/18.
//

#include "PickSystem.hpp"

#include <vector>

#include <glm/gtc/type_ptr.hpp> //mat4

#include "PolyVox/Picking.h"
#include "Camera.hpp"

#include "RegionUtils.hpp"

using namespace PolyVox;

int differenceToClampBounds(int value, int min, int max) {
    if(value > max) {
        return max - value;
    }
    if(value < min) {
        return min - value;
    }
    return 0;
}

Vector3DInt32 limitSelectionToMax(int max, Vector3DInt32 origin, Vector3DInt32 point) {

}

void PickSystem::init(const std::string& resourceDirectory) {
    // This is the tiny obj shapes - not to be confused with our shapes
    vector<tinyobj::shape_t> TOshapes;
    vector<tinyobj::material_t> objMaterials;
    
    string errStr;
    
    std::shared_ptr<Model> cubeModel;
    
    //load in the mesh and make the shapes
    bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
                          (resourceDirectory + "/models/cube.obj").c_str());
    if (!rc)
    {
        cerr << errStr << endl;
    } else {
        cubeModel = make_shared<Model>();
        cubeModel->createModel(TOshapes, objMaterials);
        cubeModel->rotate( vec3(0.0f, 0.0f, 0.0f) );
        cubeModel->scale *= 0.51f;
    }
    
    // Cursor Entity
    cursor_id = entity_manager->create_entity();
    Position_Component* position = entity_manager->add_component<Position_Component>(cursor_id);
    Model_Component* renderable = entity_manager->add_component<Model_Component>(cursor_id);
    position->position = vec3(0.0f);
    position->rotation = quat(1.0f, 0.0f, 0.0f, 0.0f);
    renderable->model = cubeModel;
}

void PickSystem::step(double t, double dt) {
    return;
    
    float screenx = input_system->getCurrentControlValue("mouse_x");
    float screeny = input_system->getCurrentControlValue("mouse_y");
    
    bool onTop = true;
    
    if(input_system->isControlDownThisStep("key_x")) { 
        onTop = false; //select the existing block, not the air block on top
    }
    
    //Calculate picked point
    Vector3DInt32 point = pickScreen(screenx, screeny, onTop);
    
    //Update cursor position
    Position_Component* position_component = entity_manager->get_component<Position_Component>(cursor_id);
    glm::vec3 new_cursor_position = vec3((float)point.getX(), (float)point.getY(), (float)point.getZ());
    position_component->position = new_cursor_position;
    
    if(input_system->wasControlPressedThisStep("mouse_left")) {
        entity_manager->delete_entity(selection_id);
    }
    if(input_system->isControlDownThisStep("mouse_left")) {
        if(entity_manager->entityExists(selection_id)) { //we're completing a selection region
            
            Selection_Component* selection_component = entity_manager->get_component<Selection_Component>(selection_id);
            
            Region& region = selection_component->selection.region;
            
            //Vector3DInt32 lower_corner = region.getLowerCorner();
            //Vector3DInt32 distance = point - lower_corner;
            
            //make it no bigger than maximum
            //int max_distance = 250;
            //point.setX(point.getX() + differenceToClampBounds(distance.getX(), -max_distance, max_distance));
            //point.setY(point.getY() + differenceToClampBounds(distance.getY(), -max_distance, max_distance));
            //point.setZ(point.getZ() + differenceToClampBounds(distance.getZ(), -max_distance, max_distance));
            
            region.setUpperCorner(point);
            
            selection_component->dirty_time = t;
            
            
            //printf("Selection Finished: (%d %d %d)(%d %d %d)\n", region.getLowerX(), region.getLowerY(), region.getLowerZ(), region.getUpperX(), region.getUpperY(), region.getUpperZ());
            
        } else { //we're creating a new selection
            Vector3DInt32 point = pickScreen(screenx, screeny, onTop);
            
            selection_id = entity_manager->create_entity();
            Selection_Component* selection_component = entity_manager->add_component<Selection_Component>(selection_id);
            
            Region& region = selection_component->selection.region;
            
            region.setLowerCorner(point);
            region.setUpperCorner(point);
            
            selection_component->dirty_time = t;
            
            
            printf("Selection Started: (%d %d %d)(%d %d %d)\n", region.getLowerX(), region.getLowerY(), region.getLowerZ(), region.getUpperX(), region.getUpperY(), region.getUpperZ());
            
        }
    }
    
    if(input_system->wasControlPressedThisStep("key_v")) {
        Selection_Component* selection_component = entity_manager->get_component<Selection_Component>(selection_id);
        Region& region = selection_component->selection.region;
        
        printf("Deleting Region: (%d %d %d)(%d %d %d)\n", region.getLowerX(), region.getLowerY(), region.getLowerZ(), region.getUpperX(), region.getUpperY(), region.getUpperZ());
        fillRegion(t, region, 0);
        entity_manager->delete_entity(selection_id);
    }
    else if(input_system->wasControlPressedThisStep("key_b")) {
        Selection_Component* selection_component = entity_manager->get_component<Selection_Component>(selection_id);
        Region& region = selection_component->selection.region;
        
        printf("Deleting Region: (%d %d %d)(%d %d %d)\n", region.getLowerX(), region.getLowerY(), region.getLowerZ(), region.getUpperX(), region.getUpperY(), region.getUpperZ());
        
        fillRegion(t, region, 3);
        entity_manager->delete_entity(selection_id);
    }
    else if(input_system->wasControlPressedThisStep("key_g")) {
        Selection_Component* selection_component = entity_manager->get_component<Selection_Component>(selection_id);
        Region& region = selection_component->selection.region;
        
        printf("Deleting Region: (%d %d %d)(%d %d %d)\n", region.getLowerX(), region.getLowerY(), region.getLowerZ(), region.getUpperX(), region.getUpperY(), region.getUpperZ());
        
        fillRegion(t, region, 2);
        entity_manager->delete_entity(selection_id);
    }
    else if(input_system->wasControlPressedThisStep("key_c")) {
        Selection_Component* selection_component = entity_manager->get_component<Selection_Component>(selection_id);
        
        entity_manager->delete_entity(selection_id);
    }
}

void PickSystem::fillRegion(double t, Region& region, CASTLE_VOXELTYPE voxel_type) {
    
    Region region_to_delete = PolyVoxExtensions::createProperRegion(region);
    printf("Deleting Region: (%d %d %d)(%d %d %d)\n", region_to_delete.getLowerX(), region_to_delete.getLowerY(), region_to_delete.getLowerZ(), region_to_delete.getUpperX(), region_to_delete.getUpperY(), region_to_delete.getUpperZ());
    
    std::vector<EntityId> voxel_list = entity_manager->get_ids_with_component<PagedVolume_Component>();
    PagedVolume_Component* voxel_component;
    voxel_component = entity_manager->get_component<PagedVolume_Component>(voxel_list.at(0));
    
    for(int x = region_to_delete.getLowerX(); x <= region_to_delete.getUpperX(); x++) {
        for(int y = region_to_delete.getLowerY(); y <= region_to_delete.getUpperY(); y++) {
            for(int z = region_to_delete.getLowerZ(); z <= region_to_delete.getUpperZ(); z++) {
                Vector3DInt32 point(x, y, z);
                
                voxel_component->volume->setVoxel(point, voxel_type);
                chunk_system->setDirtyTimeViaVoxel(t, point);
            }
        }
    }
}

Vector3DInt32 PickSystem::pickScreen(float xpercent, float ypercent, bool previous) {
    std::vector<EntityId> voxel_list = entity_manager->get_ids_with_component<PagedVolume_Component>();
    PagedVolume_Component* voxel_component;
    
    voxel_component = entity_manager->get_component<PagedVolume_Component>(voxel_list.at(0));
    
    std::vector<EntityId> camera_ids = entity_manager->get_ids_with_component<Camera_Component>();
    Camera_Component* camera = entity_manager->get_component<Camera_Component>(camera_ids.at(0));
    Position_Component* camera_position = entity_manager->get_component<Position_Component>(camera_ids.at(0));
    
    if(camera == NULL) {
        printf("pickScreen: No camera. Very odd.\n");
        exit(-1);
    }
    
    mat4 V = Camera::getViewMatrix(camera, camera_position);
    mat4 P = Camera::getProjectionMatrix();
    mat4 PV = P * V;
    
    mat4 invPV = inverse( PV );
    
    vec4 start(xpercent, ypercent, 0.0f, 1.0f);
    vec4 end(xpercent, ypercent, 0.05f, 1.0f);
    
    start = invPV * start;
    start = start / start.w;
    
    end = invPV * end;
    end = end / end.w;
    
    const uint8_t emptyVoxelExample = 0; //A voxel value of zero will represent empty space.
    
    const float length = 300.0f;
    vec4 direction = normalize(end - start);
    vec4 directionAndLength = direction * length;
    
    
    Vector3DFloat rayStart(start.x, start.y, start.z);
    Vector3DFloat rayDirectionAndLength(directionAndLength.x, directionAndLength.y, directionAndLength.z);
    
    PickResult result = pickVoxel(voxel_component->volume.get(), rayStart, rayDirectionAndLength, emptyVoxelExample);
    
    Vector3DInt32 locToSet;
    
    if(previous && result.hasPreviousVoxel) {
        return result.previousVoxel;
    }
    
    if(result.didHit) {
        return result.hitVoxel;
    }
    printf("pickScreen ERROR: DID NOT HIT\n");
    return result.hitVoxel;
}

