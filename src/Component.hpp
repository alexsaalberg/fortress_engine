//
//  Component.hpp
//  Helico-opter
//
//  Created by Alex Saalberg on 1/28/18.
//

#ifndef Component_hpp
#define Component_hpp

#include <memory> //shared_ptr

#include <glm/gtc/type_ptr.hpp> //glm stuff (vec3, quat)
#include <btRigidBody.h>

#include "Model.hpp"
#include "PolyVox/PagedVolume.h"
#include "PolyVox/FilePager.h"
#include "NoisePager.h"
#include "Selection.hpp"

#include "CastleDef.h"

class Component {
};

class Active_Component : Component {
};

class Model_Component : Component {
public:
    std::shared_ptr<Model> model;
    bool draw_as_outline = false;
};

class RawVolume_Component: Component {
public:
    double dirty_time;
    std::shared_ptr<PolyVox::RawVolume<CASTLE_VOXELTYPE> > volume;
};

class PagedVolume_Component: Component {
public:
    double dirty_time;
    std::shared_ptr<PolyVox::PagedVolume<uint8_t> > volume;
    
    PagedVolume_Component() {
        volume = std::make_shared<PolyVox::PagedVolume<CASTLE_VOXELTYPE>>(new NoisePager());
    }
};

class Position_Component : Component {
public:
    glm::vec3 scale = glm::vec3(1.0f);
    glm::vec3 position;
    glm::quat rotation;
};

class Player_Component : Component {
};

class Camera_Component : Component {
    glm::vec3 eye_offset = glm::vec3(0.0f);
};

class Colonist_Component : Component {
public:
    bool selected = false;
    bool has_target = false;
    std::list<PolyVox::Vector3DInt32> path_to_target;
};

class Selection_Component : Component {
public:
    double dirty_time;
    Selection<CASTLE_VOXELTYPE> selection;
    int outline_length;
    
    Selection_Component() {
    }
    ~Selection_Component() {
    }
};

class Physics_Component : Component {
public:
    btRigidBody* body;
    glm::vec3 eye_offset = glm::vec3(0.0f);
};

#endif /* Component_hpp */
