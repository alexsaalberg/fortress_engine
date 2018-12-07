//
//  ColonistSystem.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 5/23/18.
//

#ifndef ColonistSystem_hpp
#define ColonistSystem_hpp

#include "System.hpp"

#include "PolyVox/Vector.h"
#include <glm/gtc/type_ptr.hpp> //glm stuff (vec3, quat)

class ColonistSystem : public System {
public:
    // virtual functions
    virtual void step(double t, double dt);
private:
    bool isColonistAtPoint(glm::vec3 position, PolyVox::Vector3DInt32 point);
    PolyVox::Vector3DInt32 getVectorTowardsPoint(glm::vec3 position, PolyVox::Vector3DInt32 point);
};

#endif /* ColonistSystem_hpp */
