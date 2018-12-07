//
//  GLMUtils.cpp
//  CastleSim
//
//  Created by Alex Saalberg on 5/2/18.
//

#include "GLMUtils.hpp"

void printVec3(glm::vec3 v) {
    printf("(%f %f %f)", v.x, v.y, v.z);
}

void printQuatRotationAsAngles(glm::quat rotation) {
    printVec3(glm::degrees(glm::eulerAngles(rotation)));
}
