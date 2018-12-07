//
//  Camera.h
//  CastleSim
//
//  Created by Alex Saalberg on 4/8/18.
//

#ifndef Camera_h
#define Camera_h

#include <memory>
#include <glm/gtc/type_ptr.hpp>

#include "GLSL.hpp" //CHECK_GL_CALL, among others

#include "MatrixStack.hpp"
#include "WindowManager.hpp"
#include "EntityManager.hpp"
#include "Component.hpp"

#include "GLMUtils.hpp"

using namespace std;
using namespace glm;

class Camera {
public:
    static float aspect;
    
    static void setMVPE(double t, WindowManager* window_manager, EntityManager* entity_manager, std::shared_ptr<Program> program) {
        int windowWidth, windowHeight;
        glfwGetFramebufferSize(window_manager->getHandle(), &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);
        
        float aspect = windowWidth/(float)windowHeight;
        Camera::aspect = aspect;
        
        vector<EntityId> camera_ids = entity_manager->get_ids_with_component<Camera_Component>();
        Camera_Component* camera = entity_manager->get_component<Camera_Component>(camera_ids.at(0));
        Position_Component* position = entity_manager->get_component<Position_Component>(camera_ids.at(0));
        
        Camera::setModelIdentityMatrix(program);
        Camera::setViewMatrix(camera, position, program);
        Camera::setProjectionMatrix(program);
        Camera::setEyePosition(position->position, program);
        Camera::setLight(program);
    }
    
    static void setModelIdentityMatrix(shared_ptr<Program> program) {
        auto M = make_shared<MatrixStack>();
        M->pushMatrix();
        M->loadIdentity();
        CHECKED_GL_CALL( glUniformMatrix4fv(program->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix())) );
        M->popMatrix();
    }

    static void setViewMatrix(Camera_Component* camera,  Position_Component* camera_position, std::shared_ptr<Program> program) {
        //std::shared_ptr<MatrixStack> V = Camera::getViewMatrix(camera, camera_position);
        glm::mat4 V = Camera::getViewMatrix(camera, camera_position);
        glm::mat4 trans_inv_V = glm::transpose(glm::inverse(V));
        CHECKED_GL_CALL( glUniformMatrix4fv(program->getUniform("V"), 1, GL_FALSE, value_ptr(V) ) );
        CHECKED_GL_CALL( glUniformMatrix4fv(program->getUniform("trans_inv_V"), 1, GL_FALSE, value_ptr(V) ) );
    }
    
    static void setProjectionMatrix(shared_ptr<Program> program) {
        glm::mat4 P = Camera::getProjectionMatrix();
        CHECKED_GL_CALL( glUniformMatrix4fv(program->getUniform("P"), 1, GL_FALSE, value_ptr(P)) );
    }
    
    static void setEyePosition(vec3 position, shared_ptr<Program> prog) {
        CHECKED_GL_CALL( glUniform3fv(prog->getUniform("eyePosition"), 1, value_ptr(position)) );
    }
    
    static void setLight(shared_ptr<Program> program) {
        vec3 directionFromLight = vec3(0.0f) - vec3(50.0f, 60.0f, 90.0f); //from X to origin
        vec3 directionTowardsLight = -directionFromLight;
        CHECKED_GL_CALL( glUniform3f(program->getUniform("directionTowardsLight"), directionTowardsLight.x, directionTowardsLight.y, directionTowardsLight.z) );
    }
    
    static mat4 getViewMatrix(Camera_Component* camera, Position_Component* camera_position) {
        vec3 norot_identity = vec3(0.0f, 0.0f, 1.0f);
        
        vec3 position = camera_position->position;
        vec3 identity = camera_position->rotation * norot_identity;
        identity = position + identity;
        
        std::shared_ptr<MatrixStack> V = make_shared<MatrixStack>();
        V->pushMatrix();
        
        V->loadIdentity();
        V->lookAt(position, identity, vec3(0, 1, 0)); //Look at identity from position
        //V->translate(-1.0f * position); //Negative
        return V->topMatrix();
    }

    static mat4 getProjectionMatrix() {
        
        std::shared_ptr<MatrixStack> P = make_shared<MatrixStack>();
        P->pushMatrix();
        P->perspective(45.0f, aspect, 0.1f, 1000.0f);
        return P->topMatrix();
        
        /*
        std::shared_ptr<MatrixStack> P = make_shared<MatrixStack>();
        P->pushMatrix();
        
        float width = 400;
        float left = width / -2.0f;
        float right = width / 2.0f;
        
        float height = 800.0f;
        float top = height / 2.0f;
        float bottom = height / -2.0f;
        
        float near = 0.1f;
        float far = 300.0f;
        
        P->ortho(left, right, bottom, top, near, far);
        //P->perspective(45.0f, aspect, 0.1f, 1000.0f);
        return P->topMatrix();
         */
    }
    
    static void setMaterial(const std::shared_ptr<Program> prog, int i)
    {
        CHECKED_GL_CALL( glUniform3f(prog->getUniform("mSpecularCoefficient"), 0.3f, 0.2f, 0.1f) );
        CHECKED_GL_CALL( glUniform1f(prog->getUniform("mSpecularAlpha"), 5.0f) );
        
        switch (i)
        {
            case 0: //shiny blue plastic
                glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.02f, 0.04f, 0.2f);
                glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.0f, 0.16f, 0.9f);;
                break;
            case 1: // flat grey
                glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.13f, 0.13f, 0.14f);
                glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.3f, 0.3f, 0.4f);
                break;
            case 2: //brass
                glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.3294f, 0.2235f, 0.02745f);
                glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.7804f, 0.5686f, 0.11373f);
                break;
            case 3: //copper
                glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.1913f, 0.0735f, 0.0225f);
                glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.7038f, 0.27048f, 0.0828f);
                break;
            case 4: //green man
                glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.0913f, 0.735f, 0.0225f);
                glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.038f, 0.048f, 0.028f);
                break;
            case 5: //radiation
                glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.4f, 0.7735f, 0.225f);
                glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.4038f, 0.57048f, 0.3828f);
                break;
            case 6: //stone
                glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.0913f, 0.1735f, 0.1225f);
                glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.438f, 0.4048f, 0.428f);
                break;
            case 7:
                glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.17f, 0.01f, 0.01f);
                glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.61f, 0.04f, 0.04f);
                break;
            case 8:
                glUniform3f(prog->getUniform("mAmbientCoefficient"), 0.6f, 0.01f, 0.01f);
                glUniform3f(prog->getUniform("mDiffusionCoefficient"), 0.61f, 0.04f, 0.04f);
                break;
        }
    }
    
};

#endif /* Camera_h */
