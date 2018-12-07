//
//  Application.cpp
//  Helico-opter
//
//  Created by Alex Saalberg on 1/25/18.
//

#include <btBoxShape.h>
#include <btSphereShape.h>

#include "Application.hpp"

using namespace std;
using namespace glm;
//using namespace PolyVox;

void Application::init(double t, const std::string& resourceDirectory) {
    //Bullet
    initBullet();
    
    
    //Systems
    event_handler = make_shared<EventHandler>();
    
    entity_manager = new EntityManager();
    
    System::entity_manager = entity_manager;
    System::input_system = &input_system;
    System::bullet_dynamics_world = bullet_dynamics_world;
    System::window_manager = window_manager;
    
    system_manager.add(&input_system);
    system_manager.add(&chunk_system);
    system_manager.add(&volume_render_system);
    system_manager.add(&pick_system);
    system_manager.add(&player_system);
    system_manager.add(&render_system);
    system_manager.add(&selection_system);
    system_manager.add(&physics_system);
    system_manager.add(&colonist_system);
    
    system_manager.init(resourceDirectory);
    system_manager.init();
    
    input_system.event_handler = event_handler;
    
    pick_system.chunk_system = &chunk_system;
    player_system.chunk_system = &chunk_system;
    
	initShaders(resourceDirectory+"/shaders");
    initGeom(resourceDirectory+"/models");
    event_handler->subscribe<MouseClickEvent>(&chunk_system);
    
    chunk_system.addLoader(t, player_system.player_id);
}

void Application::initBullet() {
    // Build the broadphase
    broadphase = new btDbvtBroadphase();
    
    // Set up the collision configuration and dispatcher
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    
    // The actual physics solver
    solver = new btSequentialImpulseConstraintSolver;
    
    // The world.
    bullet_dynamics_world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    bullet_dynamics_world->setGravity(btVector3(0, -16, 0));
 
    
    //Init debug draw
    bullet_draw = new BulletDraw();
    bullet_draw->setDebugMode(btIDebugDraw::DBG_DrawWireframe);
    bullet_draw->setDebugMode(1);

    bullet_dynamics_world->setDebugDrawer(bullet_draw);
}

void Application::initColonists() {
    float spawn_half_length = 10.0f;
    
    
    for(int i = 0; i < 10; i++) {
        btCollisionShape* collision_shape = new btBoxShape( btVector3(0.5, 0.5, 0.5) );
        btDefaultMotionState* motion_state = new btDefaultMotionState(btTransform(btQuaternion(1, 0, 0, 0), btVector3(0, 0, 0)));
        btScalar mass = 5.0f;
        btVector3 fallInertia(0, 0, 0);
        collision_shape->calculateLocalInertia(mass, fallInertia);
        
        btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass, motion_state, collision_shape, fallInertia);
        
        vec3 spawn = vec3(randomFloatNegativePossible(), 0.0f, randomFloatNegativePossible());
        spawn *= spawn_half_length;
        spawn.y = 200.0f;
        
        // Create Entity
        EntityId colonist_id = entity_manager->create_entity();
        Physics_Component* physics = entity_manager->add_component<Physics_Component>(colonist_id);
        Model_Component* model = entity_manager->add_component<Model_Component>(colonist_id);
        Position_Component* position = entity_manager->add_component<Position_Component>(colonist_id);
        Colonist_Component* colonist_component = entity_manager->add_component<Colonist_Component>(colonist_id);
        
        // set position to defaults
        position->position = vec3(0.0f);
        position->rotation = quat(1.0f, 0.0f, 0.0f, 0.0f);
        
        // Create Rigid Body
        btRigidBody* fallRigidBody = new btRigidBody(fallRigidBodyCI);
        fallRigidBody->setDamping(0.2, 0.5);
        fallRigidBody->setAngularFactor(btVector3(0.0f, 0.0f, 0.0f));
        
        bullet_dynamics_world->addRigidBody(fallRigidBody);
        fallRigidBody->setActivationState(DISABLE_DEACTIVATION); // always active, important
        physics->body = fallRigidBody;
        physics->eye_offset = vec3(0.0f);
        
        // put rigid body at proper location
        btTransform initial_trans;
        initial_trans.setIdentity(); //Important to set rotation (if not set causes many issues)
        initial_trans.setOrigin(btVector3(spawn.x, spawn.y, spawn.z));
        
        physics->body->setCenterOfMassTransform(initial_trans);
        physics->body->setUserIndex(colonist_id); // physics bodies can hold some data for us, very handy
        
        // add model
        model->model = cubeModel;
    }
}

void Application::initShaders(const std::string& resourceDirectory)
{
    /*
    int width, height;
    glfwGetFramebufferSize(windowManager->getHandle(), &width, &height);
     */
    GLSL::checkVersion();
    glClearColor(.9f, .9f, 1.0f, 1.0f); // Set background color.
    glEnable(GL_DEPTH_TEST); // Enable z-buffer test.
    
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    initSimpleProgram(resourceDirectory);
    initVoxelProgram(resourceDirectory);
}

void Application::initSimpleProgram(const std::string& resourceDirectory) {
    // Initialize the GLSL program.
    simpleProgram = make_shared<Program>();
    simpleProgram->setVerbose(true);
    simpleProgram->setShaderNames(resourceDirectory + "/simple_vert.glsl",
                                resourceDirectory + "/simple_frag.glsl");
    
    if (! simpleProgram->init()) {
        std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
        exit(1);
    }
    simpleProgram->addUniform("P");
    simpleProgram->addUniform("V");
    simpleProgram->addAttribute("vPosition");
}

void Application::initVoxelProgram(const std::string& resourceDirectory) {
    voxelProgram = make_shared<Program>();
    voxelProgram->setVerbose(true);
    voxelProgram->setShaderNames(resourceDirectory + "/voxel_normalCalc_vert.glsl",
                                  resourceDirectory + "/voxel_normalCalc_frag.glsl");
    
    if (! voxelProgram->init()) {
        std::cerr << "One or more shaders failed to compile... exiting!" << std::endl;
        exit(1);
    }
    //Transformation Matrices
    voxelProgram->addUniform("P");
    voxelProgram->addUniform("V");
    voxelProgram->addUniform("trans_inv_V");
    voxelProgram->addUniform("M");
    //Material constants
    voxelProgram->addUniform("mAmbientCoefficient");
    voxelProgram->addUniform("mDiffusionCoefficient");
    voxelProgram->addUniform("mSpecularCoefficient");
    voxelProgram->addUniform("mSpecularAlpha");
    //Lighting
    voxelProgram->addUniform("eyePosition");
    voxelProgram->addUniform("directionTowardsLight");
    
    voxelProgram->addAttribute("vPosition");
}

void Application::initGeom(const std::string& resourceDirectory) {
    // this is the tiny obj shapes - not to be confused with our shapes
    vector<tinyobj::shape_t> TOshapes;
    vector<tinyobj::material_t> objMaterials;
    
    string errStr;
    
    //load in the mesh and make the shapes
    bool rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
                               (resourceDirectory + "/sphere.obj").c_str());
    if (!rc)
    {
        cerr << errStr << endl;
    } else {
        sphereModel = make_shared<Model>();
        sphereModel->createModel(TOshapes, objMaterials);
    }
    rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
                          (resourceDirectory + "/Helicopter.obj").c_str());
    if (!rc)
    {
        cerr << errStr << endl;
    } else {
        helicopterModel = make_shared<Model>();
        helicopterModel->createModel(TOshapes, objMaterials);
        helicopterModel->rotate( vec3(0.0f, 0.0f, 0.0f) );
        helicopterModel->scale *= 2.0f;
    }
    
    rc = tinyobj::LoadObj(TOshapes, objMaterials, errStr,
                            (resourceDirectory + "/cube.obj").c_str());
    if (!rc)
    {
        cerr << errStr << endl;
    } else {
        cubeModel = make_shared<Model>();
        cubeModel->createModel(TOshapes, objMaterials);
        cubeModel->rotate( vec3(0.0f, 0.0f, 0.0f) );
        cubeModel->scale *= 0.51f;
    }
}

void Application::integrate(double t, float dt) {
    // Do Physics
    btScalar timestep = dt;
    bullet_dynamics_world->stepSimulation(timestep, 10);
    
    if(!colonists_created && t > 4.0f) {
        initColonists();
        colonists_created = true;
    }
    
    // Step the Systems
    system_manager.step(t, dt);
}

void Application::render(double t, float alpha) {
    CHECKED_GL_CALL( glBindFramebuffer(GL_FRAMEBUFFER, 0) );
    CHECKED_GL_CALL( glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT) );
    CHECKED_GL_CALL( glCullFace(GL_BACK) ); //Cull back faces
    
    if( !input_system.isControlDownThisStep("key_i") ) {
        // Normal Draw
        render_system.render(t, mainProgram); //Renders models
        chunk_system.renderAllChunks(t, voxelProgram); //Renders world
        volume_render_system.render(t, voxelProgram); //Renders non-world volumes
    } else {
        // If 'i' is held down, debug draw
        simpleProgram->bind();
            vector<EntityId> camera_ids = entity_manager->get_ids_with_component<Camera_Component>();
            Camera_Component* camera = entity_manager->get_component<Camera_Component>(camera_ids.at(0));
            Position_Component* position = entity_manager->get_component<Position_Component>(camera_ids.at(0));
        
            Camera::setViewMatrix(camera, position, simpleProgram);
            Camera::setProjectionMatrix(simpleProgram);
        
            bullet_dynamics_world->debugDrawWorld();
            bullet_draw->drawAllLines(simpleProgram);
        simpleProgram->unbind();
    }
}

//[0,1.0]
float Application::randomFloat() {
    return static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
}

//[-1.0, 1.0]
float Application::randomFloatNegativePossible() {
    return (randomFloat() * 2.0f) - 1.0f;
}
