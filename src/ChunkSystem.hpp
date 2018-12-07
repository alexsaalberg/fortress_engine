//
//  ChunkSystem.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/16/18.
//

#ifndef ChunkSystem_hpp
#define ChunkSystem_hpp

//GL
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

//STD
#include <set>

//Bullet Physics
#include <btBvhTriangleMeshShape.h>
#include <btTriangleIndexVertexArray.h>
#include <btTriangleIndexVertexMaterialArray.h>
#include <btDiscreteDynamicsWorld.h>
#include <btStridingMeshInterface.h>
#include <btDefaultMotionState.h>
#include <btRigidBody.h>

//PolyVox
#include "PolyVox/Region.h"
#include "PolyVox/Vector.h"

//Castle
#include "WindowManager.hpp"

#include "System.hpp"
#include "EntityManager.hpp"
#include "Program.hpp"
#include "Camera.hpp"
#include "Messaging.hpp"
#include "InputSystem.hpp"

const int32_t Chunk_X_Length = 16; //x
const int32_t Chunk_Y_Length = 64; //y
const int32_t Chunk_Z_Length = 16; //z

struct VolumeMeshData
{
    GLuint number_of_indices;
    GLenum index_type;
    GLuint index_buffer;
    GLuint vertex_buffer;
    GLuint vertex_array_object;
    glm::vec3 translation;
    float scale;
    double clean_time;
};

struct BulletPhysicsChunkData {
    btTriangleIndexVertexArray* triangle_array;
    btBvhTriangleMeshShape* triangle_mesh_shape;
    btCollisionShape* collision_shape;
    btDefaultMotionState* motion_state;
    btRigidBody* chunk_body;
};


struct ChunkData {
    PolyVox::Vector3DInt32 coords;
    int refs;
    VolumeMeshData mesh_data;
    BulletPhysicsChunkData physics_data;
    PolyVox::Mesh<PolyVox::Vertex<unsigned char>, unsigned int> mesh;
    double dirty_time;
};

struct ChunkLoader {
    PolyVox::Vector3DInt32 current_chunk_coord;
    PolyVox::Vector3DInt32 previous_chunk_coord;
};

//These two are necessary because Vector3DInt32 don't have < operators.
struct ChunkCompare {
    bool operator() (const PolyVox::Vector3DInt32& left, const PolyVox::Vector3DInt32& right) const {
        std::hash<PolyVox::Vector3DInt32> hasher;
        return hasher(left) < hasher(right);
    }
};
static bool chunk_comp(const PolyVox::Vector3DInt32& left, const PolyVox::Vector3DInt32& right) {
    std::hash<PolyVox::Vector3DInt32> hasher;
    return hasher(left) < hasher(right);
}
//^^^
class CastleIsQuadNeeded {
public:
    bool operator()(CASTLE_VOXELTYPE back, CASTLE_VOXELTYPE front, CASTLE_VOXELTYPE& materialToUse)
    {
        if ((back > 0) && (front == 0))
        {
            materialToUse = static_cast<CASTLE_VOXELTYPE>(back);
            return true;
        }
        else if(front != BLOCK_REDOUTLINE && back == BLOCK_REDOUTLINE)
        {
            materialToUse = static_cast<CASTLE_VOXELTYPE>(back);
            return true;
        }
        else if(front == BLOCK_REDOUTLINE && back != BLOCK_REDOUTLINE)
        {
            materialToUse = static_cast<CASTLE_VOXELTYPE>(front);
            return true; 
        }
        else
        {
            return false;
        }
    }
};
/*
template <typename VoxelType>
class CastleIsQuadNeeded {
public:
    bool operator()(VoxelType back, VoxelType front, VoxelType& materialToUse)
    {
        if ((back > 0) && (front == 0))
        {
            materialToUse = static_cast<VoxelType>(back);
            return true;
        }
        else if(front == 1 && back != 1)
        {
            materialToUse = static_cast<VoxelType>(back);
            return true;
        }
        else
        {
            return false;
        }
    }
};*/


class ChunkSystem : public System, public Receiver<MouseClickEvent> {
public:
//Variables
    //InputSystem* input_system;
    
    int radius = 5;
    //std::shared_ptr<EntityManager> entity_manager;
    //WindowManager* window_manager;
    //btDiscreteDynamicsWorld* bullet_dynamics_world;
    
    //std::set<ChunkLoader> entities;
    std::multimap<EntityId, ChunkLoader> chunk_loaders;
    std::map<Vector3DInt32, ChunkData, ChunkCompare> chunks;
    std::vector<MouseClickEvent> click_events;
    //std::vector<Entity_Id> entities;
//Virtual Functions
    virtual void init();
    virtual void receive(const MouseClickEvent& collision);
    void processClickEvent(double t, MouseClickEvent& click);
    
    virtual void step(double time, double delta_time);
    //void update(double t);
    void renderAllChunks(double t, std::shared_ptr<Program> program);
    
//Function
    static std::set<PolyVox::Vector3DInt32, ChunkCompare> calculateChunkSetAroundCoord(PolyVox::Vector3DInt32 chunk_coord);
    static PolyVox::Vector3DInt32 voxelCoordToChunkCoord(PolyVox::Vector3DInt32 voxel_coord);
    static PolyVox::Vector3DInt32 positionToChunkCoord(glm::vec3 position);
    static PolyVox::Region chunkCoordToRegion(PolyVox::Vector3DInt32 chunk_coord);
    
    void addLoader(double t, EntityId entity);
    void removeLoader(EntityId entity);
    
    void setDirtyTimeViaVoxel(double t, Vector3DInt32& voxel_coord);
    void setDirtyTimeViaChunk(double t, Vector3DInt32 chunk_coord);
    void recalculateAllMeshes();
private:
    
    void addChunk(double t, Vector3DInt32 chunk, Vector3DInt32 center);
    void removeChunk(Vector3DInt32 chunk);
    
    void eraseMeshData(VolumeMeshData& mesh);
    void erasePhysicsData(BulletPhysicsChunkData& mesh);
    
    void calculateMeshAndShape(double t, ChunkData& chunk_data);
    
    template <typename MeshType>
    BulletPhysicsChunkData createRigidBodyManually(MeshType& surfaceMesh);
    
    template <typename MeshType>
    BulletPhysicsChunkData createRigidBody(MeshType& surfaceMesh);
    
    template <typename MeshType>
    VolumeMeshData bindMesh(double t, const MeshType& surfaceMesh,  const PolyVox::Vector3DInt32& translation = PolyVox::Vector3DInt32(0, 0, 0), float scale = 1.0f);
    
};

#endif /* ChunkSystem_hpp */
