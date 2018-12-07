//
//  Model.h
//  project4
//
//  Created by Alex Saalberg on 11/12/17.
//
//

#ifndef __project4__Model__
#define __project4__Model__

#include <iostream>
#include <stdio.h>
#include <vector>

#include <glad/glad.h>

#include "Shape.hpp"
#include "glm/glm.hpp"
#include "tiny_obj_loader.h"
#include "MatrixStack.hpp"
#include "Program.hpp"

class Model
{
    
public:
    //Variables
    float scale;
    glm::vec3 rotation;
    glm::vec3 translation;
    
    //Functions
    void createModel(std::shared_ptr<Shape> shape);
    void createModel(std::vector<tinyobj::shape_t> TOshapes, std::vector<tinyobj::material_t> objMaterials);
    void rotate(glm::vec3 deltaRotation);
    void draw(const std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> M) const;
    void draw(const std::shared_ptr<Program> prog, std::shared_ptr<MatrixStack> M, int shape_num) const;
    
    glm::vec3 gMin, gMax;
    std::vector<std::shared_ptr<Shape>> shapes;
private:
    
};



#endif /* defined(__project4__Model__) */
