//
//  RenderSystem.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 3/29/18.
//

#ifndef RenderSystem_h
#define RenderSystem_h

#include <memory> //shared_ptr

#include "System.hpp"

#include "EntityManager.hpp"
#include "WindowManager.hpp" //WindowManager
#include "Program.hpp"

struct VoxelMeshData
{
    GLuint noOfIndices;
    GLenum indexType;
    GLuint indexBuffer;
    GLuint vertexBuffer;
    GLuint vertexArrayObject;
    glm::vec3 translation;
    float scale;
    double dirty_time;
};

class RenderSystem : public System {
public:
    //Variables
    //WindowManager* window_manager;
    std::shared_ptr<Program> program;
    std::shared_ptr<Model> arrow_model;
    
    //Virtual Functions
    virtual void render(double t, std::shared_ptr<Program> program);
    virtual void init(const std::string& resourceDirectory);
    
private:
    
    void initArrowModel(const std::string& resourceDirectory);
    void draw_entities(double t, std::shared_ptr<Program> program);
    void setMVPE(double t, std::shared_ptr<Program> program);
    void renderGUI();
};

#endif /* RenderSystem_h */
