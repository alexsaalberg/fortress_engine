//
//  Application.hpp
//  Helico-opter
//
//  Created by Alex Saalberg on 1/25/18.
//

#ifndef Application_hpp
#define Application_hpp

//Bullet Physics
#include <btBulletDynamicsCommon.h>
#include <btTriangleIndexVertexArray.h>

//Messaging System
#include "Messaging.hpp"

#include <iostream>
#include <glad/glad.h>

#include "GLSL.hpp" //CHECK_GL_CALL, among others

//Rendering Related Stuff
#include "Program.hpp"
#include "Shape.hpp"

//value_ptr for glm
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

//Entity-Component-System
#include "EntityManager.hpp"
#include "WindowManager.hpp" //WindowManager

#include "Component.hpp"

#include "System.hpp"
#include "InputSystem.hpp"
#include "RenderSystem.hpp"
#include "ChunkSystem.hpp"
#include "SelectionSystem.hpp"
#include "PickSystem.hpp"
#include "PhysicsSystem.hpp"
#include "VolumeRenderSystem.hpp"
#include "PlayerSystem.hpp"
#include "ColonistSystem.hpp"

#include "BulletDraw.hpp"

class Application
{
public:
//Variables
    //Bullet Physics
    btBroadphaseInterface* broadphase;
    btDefaultCollisionConfiguration* collisionConfiguration;
    btCollisionDispatcher* dispatcher;
    btSequentialImpulseConstraintSolver* solver;
    btDiscreteDynamicsWorld* bullet_dynamics_world;
    btRigidBody* camera_body;
    btMotionState* camera_motion_state;
    
    BulletDraw* bullet_draw;
    
    //Messaging
    std::shared_ptr<EventHandler> event_handler;
    EntityManager* entity_manager;
 
    EntityId voxel_id = -1;
    
    //Systems
    SystemManager system_manager;
    
    ChunkSystem chunk_system;
    InputSystem input_system;
    PickSystem pick_system;
    PlayerSystem player_system;
    RenderSystem render_system;
    SelectionSystem selection_system;
    VolumeRenderSystem volume_render_system;
    PhysicsSystem physics_system;
    ColonistSystem colonist_system;
    
    //Shader Programs
    std::shared_ptr<Program> mainProgram;
    std::shared_ptr<Program> simpleProgram;
    std::shared_ptr<Program> voxelProgram;
    
    std::shared_ptr<Model> temporaryModel;
    std::shared_ptr<Model> sphereModel;
    std::shared_ptr<Model> cubeModel;
    std::shared_ptr<Model> helicopterModel;
    
    //Window
    WindowManager* window_manager;
    
    bool colonists_created = false;
    
//Functions
    /* Initilizations */
    void init(double t, const std::string& resourceDirectory);
    
    void initBullet();
    void initColonists();
    
    void initShaders(const std::string& resourceDirectory);
    void initSimpleProgram(const std::string& resourceDirectory);
    void initVoxelProgram(const std::string& resourceDirectory);

    void initGeom(const std::string& resourceDirectory);

    //Physics
    void integrate(double t, float dt);
    
    //Graphics
    void render(double t,  float alpha);
    
    //[0,1.0]
    float randomFloat();
    //[-1.0, 1.0]
    float randomFloatNegativePossible();
};

#endif /* Application_hpp */
