//
//  PlayerSystem.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 5/20/18.
//

#ifndef PlayerSystem_hpp
#define PlayerSystem_hpp

#include <btRigidBody.h>

#include "PolyVox/Vector.h"

#include "System.hpp"
#include "ChunkSystem.hpp"
#include "EntityManager.hpp"

#include "CastleDef.h"

enum PlayerState {FPS_MODE, RTS_MODE};
enum SelectionState {BUILD_MODE, COLONIST_MODE};

class PlayerSystem : public System {
public:
// Variables
    EntityId player_id;
    btRigidBody* player_body;
    
    ChunkSystem* chunk_system;
    
    EntityId selection_id = -1;
    EntityId cursor_id = -1;
    
    vector<EntityId> selected_colonists;
    
    SelectedBlock selected_block = BLOCK1;
    
    PlayerState player_state = FPS_MODE;
    SelectionState selection_state = BUILD_MODE;
    
    // If the mouse is held and released < the below time it's considered a single click, not a click-and-drag-and-release
    const float maxtime_for_click = 0.1f;
    float mouse_left_downtime = 0.0f;
    float min_drag_length = 0.0001f;
    
    float initial_screenx;
    float initial_screeny;
    bool isDrag = false;

    
// Virtual Functions
    virtual void init(const std::string& resourceDirectory);
    virtual void step(double t, double dt);
private:
    void clearSelectedColonists();
    void selectColonist(EntityId id);
    void selectColonistsInRegion(PolyVox::Region& region);
    bool isColonistInRegion(EntityId id, PolyVox::Region& region);
    bool pointInBox(btVector3 point, btVector3 boxMin, btVector3 boxMax);
    void switchSelectedBlock();
    void initCursor(const std::string& resourceDirectory);
    void sharedStep(double t, double dt);
    void fpsStep(double t, double dt);
    void rtsStep(double t, double dt);
    void moveCameraWithMouse(double t, double dt);
    Vector3DInt32 polyVoxPickScreen(float screenX, float screenY, bool previous);
    EntityId bulletPhysicsPickScreen(float screen_x, float screen_y);
    void fillRegion(double t, Region& region, CASTLE_VOXELTYPE voxel_type);
};


template< typename VolumeType>
bool colonistVoxelValidator(const VolumeType* volData, const Vector3DInt32& v3dPos)
{
    //Voxels are considered valid candidates for the path if they are inside the volume...
    
    typename VolumeType::VoxelType oneAbove = volData->getVoxel(v3dPos.getX(), v3dPos.getY()+1, v3dPos.getZ());
    typename VolumeType::VoxelType voxel = volData->getVoxel(v3dPos);
    typename VolumeType::VoxelType oneBelow = volData->getVoxel(v3dPos.getX(), v3dPos.getY()-1, v3dPos.getZ());
    if (voxel != BLOCK_AIR) {
        return false;
    }
    if(oneAbove != BLOCK_AIR) {
        return false;
    }
    if(oneBelow == BLOCK_AIR) {
        return false;
    }
        
    return true;
}

#endif /* PlayerSystem_hpp */
