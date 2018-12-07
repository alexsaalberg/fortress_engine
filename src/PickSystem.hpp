//
//  PickSystem.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/24/18.
//

#ifndef PickSystem_hpp
#define PickSystem_hpp

#include <memory>

#include "System.hpp"
#include "EntityManager.hpp"
#include "ChunkSystem.hpp"
#include "InputSystem.hpp"

#include "Model.hpp"

#include "PolyVox/Vector.h"

#include "CastleDef.h"



class PickSystem : public System {
public:
    //Variables
    EntityId cursor_id = -1;
    EntityId selection_id = -1;
    //std::shared_ptr<EntityManager> entity_manager;
    //InputSystem* input_system;
    ChunkSystem* chunk_system;

    //Virtual Functions
    virtual void init(const std::string& resourceDirectory);
    virtual void step(double t, double dt);
    
    //Member Functions
    void fillRegion(double t, Region& region, CASTLE_VOXELTYPE voxel_type);
    Vector3DInt32 pickScreen(float screenX, float screenY, bool previous);
    
};

#endif /* PickSystem_hpp */
