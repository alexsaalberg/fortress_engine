//
//  ChunkSystem.cpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/16/18.
//
#include <btTriangleMesh.h>

#include "ChunkSystem.hpp"

#include "GLSL.hpp" //CHECK_GL_CALL, among others
#include "PolyVox/Picking.h"

#include "CastleDef.h"

#include "PolyVox/AmbientOcclusionCalculator.h"

using namespace PolyVox;

//Public
void ChunkSystem::receive(const MouseClickEvent& collision) {
    click_events.push_back(collision);
}

void ChunkSystem::processClickEvent(double t, MouseClickEvent& click) {
    vector<EntityId> voxel_list = entity_manager->get_ids_with_component<PagedVolume_Component>();
    PagedVolume_Component* voxel_component;
    
    voxel_component = entity_manager->get_component<PagedVolume_Component>(voxel_list.at(0));
    
    vector<EntityId> camera_ids = entity_manager->get_ids_with_component<Camera_Component>();
    Camera_Component* camera = entity_manager->get_component<Camera_Component>(camera_ids.at(0));
    Position_Component* camera_position = entity_manager->get_component<Position_Component>(camera_ids.at(0));
    
    if(camera == NULL) {
        printf("Trying to do picking without camera. Very odd.\n");
        exit(-1);
    }
    
    
    mat4 V = Camera::getViewMatrix(camera, camera_position);
    mat4 P = Camera::getProjectionMatrix();
    mat4 PV = P * V;
    
    mat4 invPV = inverse( PV );
    
    float xpercent = click.x;
    float ypercent = click.y;
    
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
    
    if(result.didHit) {
        printf("Setting at (%d %d %d)\n", result.hitVoxel.getX(), result.hitVoxel.getY(), result.hitVoxel.getZ());
        //const int radius = 5;
        for(int x = -radius; x < radius; x++) {
            for(int y = -radius; y < radius; y++) {
                for(int z = -radius; z < radius; z++) {
                    locToSet = Vector3DInt32(x + result.hitVoxel.getX(), y + result.hitVoxel.getY(), z + result.hitVoxel.getZ());
                    if(click.button == 1) {
                        voxel_component->volume->setVoxel(locToSet, 255-128);
                        setDirtyTimeViaVoxel(t, locToSet);
                    } else if (click.button == 0) {
                        voxel_component->volume->setVoxel(locToSet, 0);
                        setDirtyTimeViaVoxel(t, locToSet);
                    }
                    //printf("Mouse button %d\n", click.button);
                }
            }
        }
    }
}
void ChunkSystem::init() {
    EntityId voxel_id = entity_manager->create_entity();
    
    PagedVolume_Component* voxels = entity_manager->add_component<PagedVolume_Component>(voxel_id);
    voxels->dirty_time = -1.0f;
}

void ChunkSystem::step(double t, double dt) {
    
    if(input_system->isControlDownThisStep("mouse_left")) {
        MouseClickEvent click(input_system->getCurrentControlValue("mouse_x"),
                              input_system->getCurrentControlValue("mouse_y"),
                              GLFW_MOUSE_BUTTON_LEFT);
        //processClickEvent(t, click);
    }
    
    for(MouseClickEvent& click : click_events) {
        //processClickEvent(t, click);
    }
    
    
    
    click_events.clear();
    
    for(auto& chunk_loader_pair : chunk_loaders) {
        EntityId id = chunk_loader_pair.first;
        ChunkLoader& loader = chunk_loader_pair.second;
        
        Position_Component* position_component = entity_manager->get_component<Position_Component>(id);
        
        loader.previous_chunk_coord = loader.current_chunk_coord;
        loader.current_chunk_coord = positionToChunkCoord(position_component->position);
        
        if(loader.previous_chunk_coord != loader.current_chunk_coord) {
            std::set<Vector3DInt32, ChunkCompare> previous_set;
            std::set<Vector3DInt32, ChunkCompare> current_set;
            std::set<Vector3DInt32, ChunkCompare> chunks_to_remove;
            std::set<Vector3DInt32, ChunkCompare> chunks_to_add;
            
            //Update references
            previous_set = calculateChunkSetAroundCoord(loader.previous_chunk_coord);
            current_set = calculateChunkSetAroundCoord(loader.current_chunk_coord);
            
            std::set_difference(previous_set.begin(), previous_set.end(), current_set.begin(), current_set.end(), std::inserter(chunks_to_remove, chunks_to_remove.end()), chunk_comp);
            std::set_difference(current_set.begin(), current_set.end(), previous_set.begin(), previous_set.end(), std::inserter(chunks_to_add, chunks_to_add.end()), chunk_comp);
            
            for(auto chunk : chunks_to_remove) {
                removeChunk(chunk);
            }
            for(auto chunk : chunks_to_add) {
                addChunk(t, chunk, loader.current_chunk_coord);
            }
        }
    }
    
    const int max_mesh_gen_per_step = 6;
    int mesh_gen_this_step = 0;
    
    for(auto& chunk_pair : chunks) {
        Vector3DInt32 chunk = chunk_pair.first;
        ChunkData& chunk_data = chunk_pair.second;
        
        //printf("%lf | %lf\n", chunk_data.dirty_time, chunk_data.mesh.clean_time);
        if(mesh_gen_this_step < max_mesh_gen_per_step) {
            if(chunk_data.dirty_time > t) {
                //This is in the future, do nothing
                //printf("%lf in future (%lf)\n", chunk_data.dirty_time, t);
            } else if(chunk_data.dirty_time > chunk_data.mesh_data.clean_time) {
                //recalculate mesh
                //printf("Recalculating Mesh of Chunk(%d %d %d)\n", chunk.getX(), chunk.getY(), chunk.getZ());
                eraseMeshData(chunk_data.mesh_data);
                erasePhysicsData(chunk_data.physics_data);
                
                calculateMeshAndShape(t, chunk_data);
                mesh_gen_this_step++;
            }
        }
    }
    
}

void ChunkSystem::renderAllChunks(double t, std::shared_ptr<Program> program) {
    const int max_mesh_gen_per_frame = 8;
    int mesh_gen_this_frame = 0;
    
    //printf("%lf: Rendering %d chunks\n", t, chunks.size());
    
    std::shared_ptr<MatrixStack> M = std::make_shared<MatrixStack>();
    M->loadIdentity();
    
    program->bind();
    
    Camera::setMVPE(t, window_manager, entity_manager, program);
    Camera::setMaterial(program, 2);
    
    for(auto& chunk_pair : chunks) {
        Vector3DInt32 chunk = chunk_pair.first;
        ChunkData& chunk_data = chunk_pair.second;
        
        auto meshData = chunk_data.mesh_data;
        
        if(meshData.clean_time >= 0.0f) { // -1.0f means not created yet
            M->pushMatrix();
            M->translate(meshData.translation);
            M->scale(meshData.scale);
            glUniformMatrix4fv(program->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
            M->popMatrix();
            
            // Bind the vertex array for the current mesh
            CHECKED_GL_CALL( glBindVertexArray(meshData.vertex_array_object) );
            // Draw the mesh
            CHECKED_GL_CALL( glDrawElements(GL_TRIANGLES, meshData.number_of_indices, meshData.index_type, 0) );
            // Unbind the vertex array.
            CHECKED_GL_CALL( glBindVertexArray(0) );
        }
    }
    
    program->unbind();
}

std::set<Vector3DInt32, ChunkCompare> ChunkSystem::calculateChunkSetAroundCoord(Vector3DInt32 chunk_coord) {
    const float radius = 12.1f;
    const float max_vert_distance = 2.0f;
    const float horz_distance = 10.0f;
    const int max_chunks = 1000;
    
    std::set<Vector3DInt32, ChunkCompare> return_chunks;
    std::set<Vector3DInt32, ChunkCompare> current_chunks;
    std::set<Vector3DInt32, ChunkCompare> next_chunks;
    
    return_chunks.insert(chunk_coord);
    current_chunks.insert(chunk_coord);
    
    std::set<Vector3DInt32, ChunkCompare> delta_chunks;
    delta_chunks.insert(Vector3DInt32( 1, 0, 0));
    delta_chunks.insert(Vector3DInt32(-1, 0, 0));
    delta_chunks.insert(Vector3DInt32( 0, 1, 0));
    delta_chunks.insert(Vector3DInt32( 0,-1, 0));
    delta_chunks.insert(Vector3DInt32( 0, 0, 1));
    delta_chunks.insert(Vector3DInt32( 0, 0,-1));
    
    while(current_chunks.size() > 0 && return_chunks.size() < max_chunks) {
        next_chunks.clear();
        for(auto chunk : current_chunks) {
            for(auto delta_chunk : delta_chunks) {
                Vector3DInt32 adjacent_chunk = chunk + delta_chunk;
                if(return_chunks.count(adjacent_chunk) == 0) { //Only check this chunk if we haven't already
                    Vector3DInt32 difference_vec = chunk_coord - adjacent_chunk;
                    if(difference_vec.length() < radius && abs(difference_vec.getY()) < max_vert_distance) {
                    //if(difference_vec.getY() < vert_distance && difference_vec.getX() < horz_distance && difference_vec.getZ() < horz_distance) {
                        return_chunks.insert(adjacent_chunk);
                        next_chunks.insert(adjacent_chunk);
                    }
                }
            }
        }
        
        current_chunks = next_chunks;
    }
    
    return return_chunks;
}

void ChunkSystem::addLoader(double t, EntityId entity) {
    ChunkLoader chunk_loader;
    
    Position_Component* position_component = entity_manager->get_component<Position_Component>(entity);
    chunk_loader.current_chunk_coord = positionToChunkCoord(position_component->position);
    //chunk_loader.current_chunk_coord = NULL;
    chunk_loader.previous_chunk_coord = Vector3DInt32(INT32_MIN, INT32_MIN, INT32_MIN);
    //todo: init references
    
    
    auto chunk_set = calculateChunkSetAroundCoord(chunk_loader.current_chunk_coord);
    for(auto chunk : chunk_set) {
        addChunk(t, chunk, chunk_loader.current_chunk_coord);
    }
    
    std::pair<EntityId, ChunkLoader> element(entity, chunk_loader);
    chunk_loaders.insert(element);
}


void ChunkSystem::removeLoader(EntityId entity) {
    chunk_loaders.erase(entity);
    //Remove references
}

//Static
Vector3DInt32 ChunkSystem::voxelCoordToChunkCoord(Vector3DInt32 voxel_coord) {
    //assuming uniform 16 lengths
    //(10, 23, 37) -> (0, 1, 2)
    //10/16 = 0
    //23/16 = 1
    //37/16 = 2
    int32_t chunk_x = abs(voxel_coord.getX()) / Chunk_X_Length;
    int32_t chunk_y = abs(voxel_coord.getY()) / Chunk_Y_Length;
    int32_t chunk_z = abs(voxel_coord.getZ()) / Chunk_Z_Length;
    
    if(voxel_coord.getX() < 0) {
        chunk_x *= -1;
        chunk_x -= 1; //Because (-5,0,-6) should be in chunk (-1,0,-1) not (0,0,0)
    }
    if(voxel_coord.getY() < 0) {
        chunk_y *= -1;
        chunk_y -= 1;
    }
    if(voxel_coord.getZ() < 0) {
        chunk_z *= -1;
        chunk_z -= 1;
    }
    
    Vector3DInt32 chunk_coord(chunk_x, chunk_y, chunk_z);
    
    return chunk_coord;
}

Vector3DInt32 ChunkSystem::positionToChunkCoord(glm::vec3 position) {
    int32_t x = floor(position.x + 0.5f);
    int32_t y = floor(position.y + 0.5f);
    int32_t z = floor(position.z + 0.5f);
    
    Vector3DInt32 chunk_coord;
    chunk_coord.setX(x / Chunk_X_Length);
    chunk_coord.setY(y / Chunk_Y_Length);
    chunk_coord.setZ(z / Chunk_Z_Length);
    
    return chunk_coord;
}

Region ChunkSystem::chunkCoordToRegion(Vector3DInt32 chunk_coord) {
    Region region;
    region.setLowerX(chunk_coord.getX() * Chunk_X_Length);
    region.setLowerY(chunk_coord.getY() * Chunk_Y_Length);
    region.setLowerZ(chunk_coord.getZ() * Chunk_Z_Length);
    
    region.setUpperX(chunk_coord.getX() * Chunk_X_Length + Chunk_X_Length - 1); //0 + 16 - 1 = 15
    region.setUpperY(chunk_coord.getY() * Chunk_Y_Length + Chunk_Y_Length - 1);
    region.setUpperZ(chunk_coord.getZ() * Chunk_Z_Length + Chunk_Z_Length - 1);
    
    return region;
}

void ChunkSystem::setDirtyTimeViaVoxel(double t, Vector3DInt32& voxel_coord) {
    Vector3DInt32 chunk_coord = voxelCoordToChunkCoord(voxel_coord);
    
    for(int x = -1; x <= 1; x++) {
        for(int y = -1; y <= 1; y++) {
            for(int z = -1; z <= 1; z++) {
                Vector3DInt32 direction(x, y, z);
                //Vector3DInt32 adjacent_voxel = (voxel_coord.operator+=(direction));
                if(chunk_coord != voxelCoordToChunkCoord(voxel_coord + direction)) {
                   setDirtyTimeViaChunk(t, chunk_coord + direction);
                }
            }
        }
    }
    setDirtyTimeViaChunk(t, chunk_coord);
}

void ChunkSystem::setDirtyTimeViaChunk(double t, Vector3DInt32 chunk_coord) {
    auto iterator = chunks.find(chunk_coord);
    if(iterator != chunks.end()) {
        ChunkData& chunk_data = chunks.at(chunk_coord);
        chunk_data.dirty_time = t;
    } else {
        printf("Chunk (%d %d %d) not in range.\n", chunk_coord.getX(), chunk_coord.getY(), chunk_coord.getZ());
    }
}

void ChunkSystem::recalculateAllMeshes() {
    for(auto& chunk : chunks) {
        chunk.second.mesh_data.clean_time = -1.0f;
    }
}

/*
 * PRIVATE
 */
void ChunkSystem::addChunk(double t, Vector3DInt32 chunk, Vector3DInt32 loader) {
    if(chunks.count(chunk) > 0) {
        chunks.at(chunk).refs += 1;
    } else {
        Vector3DInt32 distance_vector = chunk - Vector3DInt32(loader.getX() + 1, 0, loader.getZ() -1);
        float length = abs( distance_vector.length() );
        
        //assuming 1 chunk away is roughly 0.1 seconds...
        float extra_time = length * 0.1f;
        
        ChunkData new_chunk_data;
        new_chunk_data.coords = chunk;
        new_chunk_data.refs = 1;
        new_chunk_data.mesh_data.clean_time = -1.0f;
        //calculateMesh(t, chunk);
        new_chunk_data.dirty_time = t + extra_time;
        
        new_chunk_data.physics_data.chunk_body = nullptr;
        new_chunk_data.physics_data.collision_shape = nullptr;
        new_chunk_data.physics_data.motion_state = nullptr;
        new_chunk_data.physics_data.triangle_array = nullptr;
        new_chunk_data.physics_data.triangle_mesh_shape = nullptr;
        
        std::pair<Vector3DInt32, ChunkData> element(chunk, new_chunk_data);
        chunks.insert(element);
    }
}

void ChunkSystem::removeChunk(Vector3DInt32 chunk) {
    auto iterator = chunks.find(chunk);
    if(iterator != chunks.end()) {
        ChunkData& chunk_data = chunks.at(chunk);
        chunk_data.refs -= 1;
        if(chunk_data.refs <= 0) {
            if(chunk_data.physics_data.chunk_body != nullptr) {
                bullet_dynamics_world->removeRigidBody(chunk_data.physics_data.chunk_body);
            }
            chunks.erase(chunk);
        }
    } else {
        printf("Trying to remove chunk x%d y%d z%d that is not in system.\n", chunk.getX(), chunk.getY(), chunk.getZ());
    }
}

void ChunkSystem::eraseMeshData(VolumeMeshData& mesh) {
    CHECKED_GL_CALL( glDeleteVertexArrays(1, &mesh.vertex_array_object) );
    CHECKED_GL_CALL( glDeleteBuffers(1, &mesh.vertex_buffer) );
    CHECKED_GL_CALL( glDeleteBuffers(1, &mesh.index_buffer) );
    //CHECKED_GL_CALL( glDeleteBuffers(1, &mesh.number_of_indices) );
    //printf("Deleting mesh: VAO#%d VB#%d IB#%d\n", mesh.vertex_array_object, mesh.vertex_buffer, mesh.index_buffer);
}

void ChunkSystem::erasePhysicsData(BulletPhysicsChunkData& physics_data) {
    if(physics_data.chunk_body != nullptr) {
        bullet_dynamics_world->removeRigidBody(physics_data.chunk_body);
    
    delete physics_data.chunk_body;
    delete physics_data.collision_shape;
    delete physics_data.motion_state;
    //delete physics_data.triangle_mesh_shape;
    delete physics_data.triangle_array;
    }
}

void ChunkSystem::calculateMeshAndShape(double t, ChunkData& chunk_data) {
    //0+16 = 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 = 17 things
    //so need 0+16-1 = 16 things
    Vector3DInt32 chunk = chunk_data.coords;
    /*
    int32_t lower_x = chunk.getX() * Chunk_X_Length;
    int32_t lower_y = chunk.getY() * Chunk_Y_Length;
    int32_t lower_z = chunk.getZ() * Chunk_Z_Length;
    int32_t upper_x = lower_x + Chunk_X_Length - 1;
    int32_t upper_y = lower_y + Chunk_Y_Length - 1;
    int32_t upper_z = lower_z + Chunk_Z_Length - 1;
    
    PolyVox::Region region(lower_x, lower_y, lower_z, upper_x, upper_y, upper_z);
    //regToExtract.shrink(1);
    //PolyVox::Region*/
    
    PolyVox::Region region;
    region = chunkCoordToRegion(chunk);
    
    PagedVolume_Component* voxel_component = entity_manager->get_first_component_of_type<PagedVolume_Component>();
    
    // Perform the extraction for this region of the volume
    auto mesh = extractCubicMesh(voxel_component->volume.get(), region, CastleIsQuadNeeded(), true);
    //auto mesh = extractMarchingCubesMesh(pagedData.get(), regToExtract);
    
    // The returned mesh needs to be decoded to be appropriate for GPU rendering.
    auto decodedMesh = decodeMesh(mesh);
    
    //printf("Calculating mesh for (%d %d %d)(%d %d %d). %zu indices.\n", lower_x, lower_y, lower_z, upper_x, upper_y, upper_z, decodedMesh.getNoOfIndices());
    // Pass the surface to the OpenGL window. Note that we are also passing an offset in this multi-mesh example. This is because
    // the surface extractors return a mesh with 'local space' positions to reduce storage requirements and precision problems.
    chunk_data.mesh = decodedMesh;
    if(decodedMesh.getNoOfIndices() > 0) {
        chunk_data.physics_data = createRigidBodyManually(chunk_data.mesh);
    }
    chunk_data.mesh_data = bindMesh(t, decodedMesh, decodedMesh.getOffset());
}

btVector3 getVertexAtIndex(uint8_t* raw_vertex_pointer, int stride, int index) {
    float* vertex_pointer = (float*) (raw_vertex_pointer + stride*index);
    btVector3 returnVec(*vertex_pointer, *(vertex_pointer+1), *(vertex_pointer+2));
    return returnVec;
}

void printBtVector3(btVector3 vec) {
    printf("(%f %f %f)", vec.getX(), vec.getY(), vec.getZ());
}

template <typename MeshType>
BulletPhysicsChunkData ChunkSystem::createRigidBodyManually(MeshType& surfaceMesh) {
    btTriangleMesh* triangle_mesh = new btTriangleMesh();
    
    int sizeof_one_vertex_and_material = sizeof(typename MeshType::VertexType);
    int sizeof_one_vertex = sizeof(float) * 3;
    uint8_t* raw_vertex_pointer = (uint8_t*) surfaceMesh.getRawVertexData();
    
    for(int i = 0; i < surfaceMesh.getNoOfIndices(); i+=3) {
        int index1 = surfaceMesh.getIndex(i);
        int index2 = surfaceMesh.getIndex(i+1);
        int index3 = surfaceMesh.getIndex(i+2);
        
       
        btVector3 bVertex1 = getVertexAtIndex(raw_vertex_pointer, sizeof_one_vertex_and_material, index1);
        btVector3 bVertex2 = getVertexAtIndex(raw_vertex_pointer, sizeof_one_vertex_and_material, index2);
        btVector3 bVertex3 = getVertexAtIndex(raw_vertex_pointer, sizeof_one_vertex_and_material, index3);
        
        /*
        printf("Vectors %d %d %d: ", index1, index2, index3);
        printBtVector3(bVertex1);
        printBtVector3(bVertex2);
        printBtVector3(bVertex3);
        printf("\n");
        */
        triangle_mesh->addTriangle(bVertex1, bVertex2, bVertex3);
    }
    
    btBvhTriangleMeshShape* triangle_mesh_shape = new btBvhTriangleMeshShape(triangle_mesh, true);
    
    Vector3DInt32 int_position = surfaceMesh.getOffset();
    btVector3 float_position = btVector3(int_position.getX(), int_position.getY(), int_position.getZ());
    
    btCollisionShape* collisionShapeTerrain = triangle_mesh_shape;
    btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), float_position));
    
    btRigidBody::btRigidBodyConstructionInfo rigidBodyConstructionInfo(0.0f, motionState, collisionShapeTerrain, btVector3(0.0f,0.0f,0.0f));
    
    btRigidBody* rigidBodyTerrain = new btRigidBody(rigidBodyConstructionInfo);
    
    rigidBodyTerrain->setFriction(btScalar(0.5));
    
    bullet_dynamics_world->addRigidBody(rigidBodyTerrain);
    
    BulletPhysicsChunkData physics_data;
    physics_data.triangle_array = triangle_mesh;
    physics_data.triangle_mesh_shape = triangle_mesh_shape;
    physics_data.collision_shape = collisionShapeTerrain;
    physics_data.motion_state = motionState;
    physics_data.chunk_body = rigidBodyTerrain;
    
    return physics_data;


}


template <typename MeshType>
BulletPhysicsChunkData ChunkSystem::createRigidBody(MeshType& surfaceMesh) {    //Create Bullet Shape
    int sizeof_one_index = sizeof(typename MeshType::IndexType);
    int sizeof_one_vertex_and_material = sizeof(typename MeshType::VertexType);
    //int sizeof_one_vertex = sizeof(float) * 3;
    //int sizeof_one_material = sizeof_one_vertex_and_material;
    
    btTriangleIndexVertexArray* bullet_triangles = new btTriangleIndexVertexArray(surfaceMesh.getNoOfIndices()/3, (int *)surfaceMesh.getRawIndexData(), sizeof_one_index * 3, surfaceMesh.getNoOfVertices(), (float *)surfaceMesh.getRawVertexData(), sizeof_one_vertex_and_material * 3);
    //btTriangleIndexVertexMaterialArray*
    //bullet_triangles = new btTriangleIndexVertexMaterialArray(
        //surfaceMesh.getNoOfIndices()/3, (int *)surfaceMesh.getRawIndexData(), sizeof_one_index * 3,
        //surfaceMesh.getNoOfVertices(), (float *)surfaceMesh.getRawVertexData(), sizeof_one_vertex * 3),
    
    
    btBvhTriangleMeshShape* triangle_mesh_shape = new btBvhTriangleMeshShape(bullet_triangles, true);
    
    Vector3DInt32 int_position = surfaceMesh.getOffset();
    btVector3 float_position = btVector3(int_position.getX(), int_position.getY(), int_position.getZ());
    
    btCollisionShape* collisionShapeTerrain = triangle_mesh_shape;
    btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), float_position));
    
    btRigidBody::btRigidBodyConstructionInfo rigidBodyConstructionInfo(0.0f, motionState, collisionShapeTerrain, btVector3(0.0f,0.0f,0.0f));
    
    btRigidBody* rigidBodyTerrain = new btRigidBody(rigidBodyConstructionInfo);
    
    rigidBodyTerrain->setFriction(btScalar(0.9));
    
    bullet_dynamics_world->addRigidBody(rigidBodyTerrain);
    
    BulletPhysicsChunkData physics_data;
    physics_data.triangle_array = bullet_triangles;
    physics_data.triangle_mesh_shape = triangle_mesh_shape;
    physics_data.collision_shape = collisionShapeTerrain;
    physics_data.motion_state = motionState;
    physics_data.chunk_body = rigidBodyTerrain;
    
    return physics_data;
}

// Convert a PolyVox mesh to OpenGL index/vertex buffers. Inlined because it's templatised.
template <typename MeshType>
VolumeMeshData ChunkSystem::bindMesh(double t, const MeshType& surfaceMesh, const PolyVox::Vector3DInt32& translation, float scale)
{
    // This struct holds the OpenGL properties (buffer handles, etc) which will be used
    // to render our mesh. We copy the data from the PolyVox mesh into this structure.
    VolumeMeshData meshData;
    
    // Create the VAO for the mesh
    CHECKED_GL_CALL( glGenVertexArrays(1, &(meshData.vertex_array_object)) );
    glBindVertexArray(meshData.vertex_array_object);
    
    // The GL_ARRAY_BUFFER will contain the list of vertex positions
    CHECKED_GL_CALL( glGenBuffers(1, &(meshData.vertex_buffer)) );
    glBindBuffer(GL_ARRAY_BUFFER, meshData.vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, surfaceMesh.getNoOfVertices() * sizeof(typename MeshType::VertexType), surfaceMesh.getRawVertexData(), GL_STATIC_DRAW);
    
    // and GL_ELEMENT_ARRAY_BUFFER will contain the indices
    CHECKED_GL_CALL( glGenBuffers(1, &(meshData.index_buffer)) );
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData.index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, surfaceMesh.getNoOfIndices() * sizeof(typename MeshType::IndexType), surfaceMesh.getRawIndexData(), GL_STATIC_DRAW);
    
    // Every surface extractor outputs valid positions for the vertices, so tell OpenGL how these are laid out
    glEnableVertexAttribArray(0); // Attrib '0' is the vertex positions
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(typename MeshType::VertexType), (GLvoid*)(offsetof(typename MeshType::VertexType, position))); //take the first 3 floats from every sizeof(decltype(vecVertices)::value_type)
    
    // Some surface extractors also generate normals, so tell OpenGL how these are laid out. If a surface extractor
    // does not generate normals then nonsense values are written into the buffer here and sghould be ignored by the
    // shader. This is mostly just to simplify this example code - in a real application you will know whether your
    // chosen surface extractor generates normals and can skip uploading them if not.
    glEnableVertexAttribArray(1); // Attrib '1' is the vertex normals.
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(typename MeshType::VertexType), (GLvoid*)(offsetof(typename MeshType::VertexType, normal)));
    
    // Finally a surface extractor will probably output additional data. This is highly application dependant. For this example code
    // we're just uploading it as a set of bytes which we can read individually, but real code will want to do something specialised here.
    glEnableVertexAttribArray(2); //We're talking about shader attribute '2'
    GLint size = (std::min)(sizeof(typename MeshType::VertexType::DataType), size_t(4)); // Can't upload more that 4 components (vec4 is GLSL's biggest type)
    glVertexAttribIPointer(2, size, GL_UNSIGNED_BYTE, sizeof(typename MeshType::VertexType), (GLvoid*)(offsetof(typename MeshType::VertexType, data)));
    
    // We're done uploading and can now unbind.
    glBindVertexArray(0);
    
    // A few additional properties can be copied across for use during rendering.
    meshData.number_of_indices = surfaceMesh.getNoOfIndices();
    meshData.translation = glm::vec3(translation.getX(), translation.getY(), translation.getZ());
    meshData.scale = scale;
    
    // Set 16 or 32-bit index buffer size.
    meshData.index_type = sizeof(typename MeshType::IndexType) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
    
    meshData.clean_time = t;
    
    return meshData;
}

