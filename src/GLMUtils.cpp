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
void printQuatRotationAsRadians(glm::quat rotation) {
    printVec3(glm::eulerAngles(rotation));
}

float getQuatDegreeRotationAroundX(glm::quat rotation) {
    glm::vec3 noRotationIdentity = glm::vec3(0,0,1);
    glm::vec3 rotationVector = rotation * noRotationIdentity;
    rotationVector = glm::normalize(rotationVector);
    float rotationAroundX = asin(rotationVector.y);
    float rotationAroundXInDegrees = glm::degrees(rotationAroundX);
    return rotationAroundXInDegrees;
}

glm::vec3 getRotatedNoRotationIdentity(glm::quat rotation) {
    glm::vec3 noRotationIdentity = glm::vec3(0,0,1);
    glm::vec3 rotationVector = rotation * noRotationIdentity;
    return rotationVector;
}

