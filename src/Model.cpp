//
//  Model.cpp
//  project4
//
//  Created by Alex Saalberg on 11/12/17.
//
//

#include "Model.hpp"

using namespace std;
using namespace glm;
using namespace tinyobj;

void Model::createModel(shared_ptr<Shape> shape) {
    gMin = vec3(std::numeric_limits<float>::max());
    gMax = vec3(-std::numeric_limits<float>::max());
    
    shapes.reserve(1);
    shapes.push_back(shape);
    
    // 2. createShape for each tiny obj shape
    int i = 0;
    
    // 3. measure each shape to find out its AABB
    shapes[i]->measure();
    
    // 4. call init on each shape to create the GPU data
    shapes[i]->init();
    
    gMin.x = fmin(shapes[i]->min.x, gMin.x);
    gMin.y = fmin(shapes[i]->min.y, gMin.y);
    gMin.z = fmin(shapes[i]->min.z, gMin.z);
    
    gMax.x = fmax(shapes[i]->max.x, gMax.x);
    gMax.y = fmax(shapes[i]->max.y, gMax.y);
    gMax.z = fmax(shapes[i]->max.z, gMax.z);
    
    translation = 0.5f*(gMax + gMin);
    
    if (gMax.x > gMax.y && gMax.x > gMax.z)
    {
        scale = 2.0/(gMax.x-gMin.x);
    }
    else if (gMax.y > gMax.x && gMax.y > gMax.z)
    {
        scale = 2.0/(gMax.y-gMin.y);
    }
    else
    {
        scale = 2.0/(gMax.z-gMin.z);
    }
}

void Model::createModel(vector<shape_t> inShapes, vector<material_t> inMaterials) {
    gMin = vec3(std::numeric_limits<float>::max());
    gMax = vec3(-std::numeric_limits<float>::max());
    shared_ptr<Shape> shape;
    
    shapes.reserve(inShapes.size());
    for (size_t i = 0; i < inShapes.size(); i++)
    {
        // TODO -- Initialize each mesh
        // 1. make a shared pointer
        shapes.push_back(make_shared<Shape>());
        
        // 2. createShape for each tiny obj shape
        shapes[i]->createShape(inShapes[i]);
        
        // 3. measure each shape to find out its AABB
        shapes[i]->measure();
        
        // 4. call init on each shape to create the GPU data
        shapes[i]->init();
        
        // perform some record keeping to keep track of global min and max
        gMin.x = fmin(shapes[i]->min.x, gMin.x);
        gMin.y = fmin(shapes[i]->min.y, gMin.y);
        gMin.z = fmin(shapes[i]->min.z, gMin.z);
        
        gMax.x = fmax(shapes[i]->max.x, gMax.x);
        gMax.y = fmax(shapes[i]->max.y, gMax.y);
        gMax.z = fmax(shapes[i]->max.z, gMax.z);
        // think about scale and translate....
        // based on the results of calling measure on each peice
    }
    
    translation = 0.5f*(gMax+gMin);
    //mTranslate = gMin + 0.5f*(gMax - gMin);
    if (gMax.x > gMax.y && gMax.x > gMax.z)
    {
        scale = 2.0/(gMax.x-gMin.x);
    }
    else if (gMax.y > gMax.x && gMax.y > gMax.z)
    {
        scale = 2.0/(gMax.y-gMin.y);
    }
    else
    {
        scale = 2.0/(gMax.z-gMin.z);
    }
}

void Model::rotate(vec3 deltaRotation) {
    rotation += deltaRotation;
}

void Model::draw(const std::shared_ptr<Program> prog, shared_ptr<MatrixStack> M) const {
    M->pushMatrix();
    M->scale(scale);
    M->rotate(radians(rotation.x), vec3(1, 0, 0));
    M->rotate(radians(rotation.y), vec3(0, 1, 0));
    M->rotate(radians(rotation.z), vec3(0, 0, 1));
    M->translate(-translation); //Negative translation
    
    //mat4 normMatrix = glm::transpose(glm::inverse(M->topMatrix()));
    //glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(normMatrix));
    
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
    M->popMatrix();
    
    for (auto &shape : shapes) // access by reference to avoid copying
    {
        shape->draw(prog);
    }
}

void Model::draw(const std::shared_ptr<Program> prog, shared_ptr<MatrixStack> M, int shape_num) const {
    
    M->pushMatrix();
    M->scale(scale);
    M->rotate(radians(rotation.x), vec3(1, 0, 0));
    M->rotate(radians(rotation.y), vec3(0, 1, 0));
    M->rotate(radians(rotation.z), vec3(0, 0, 1));
    M->translate(-translation); //Negative translation
    
    //mat4 normMatrix = glm::transpose(glm::inverse(M->topMatrix()));
    //glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(normMatrix));
    
    glUniformMatrix4fv(prog->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
    M->popMatrix();
    
    shapes.at(shape_num)->draw(prog);
}


