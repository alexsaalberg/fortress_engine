//
//  BulletDraw.cpp
//  CastleSim
//
//  Created by Alex Saalberg on 5/5/18.
//

#include <glad/glad.h>

#include "GLSL.hpp" //CHECK_GL_CALL, among others

#include "BulletDraw.hpp"

void BulletDraw::drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color)
{
    DebugLine line;
    line.p1 = glm::vec3(from.getX(), from.getY(), from.getZ());
    line.p2 = glm::vec3(to.getX(), to.getY(), to.getZ());
    
    lines.push_back(line);
}

void BulletDraw::setDebugMode(int debugMode) {
    debug_mode = debugMode;
}

int BulletDraw::getDebugMode() const {
    return debug_mode;
}

void BulletDraw::drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color) {
    printf("Contact point!\n");
}

void BulletDraw::reportErrorWarning(const char* warningString) {
    printf("BulletWarning: %s\n", warningString);
}

void BulletDraw::draw3dText(const btVector3& location,const char* textString) {
    printf("BulletText at (%f %f %f): %s\n", location.getX(), location.getY(), location.getZ(), textString);
}

void BulletDraw::drawAllLines(std::shared_ptr<Program> program) {
    GLuint vbo, vao;
    CHECKED_GL_CALL(glGenBuffers(1, &vbo));
    CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, sizeof(DebugLine) * lines.size(), &lines[0],  GL_STATIC_DRAW));
    
    CHECKED_GL_CALL(glGenVertexArrays(1, &vao));
    CHECKED_GL_CALL(glBindVertexArray(vao));
    CHECKED_GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0));
    CHECKED_GL_CALL(glEnableVertexAttribArray(0));

    CHECKED_GL_CALL(glDrawArrays(GL_LINES, 0, lines.size()*2));
    CHECKED_GL_CALL(glDeleteBuffers(1, &vbo));
    CHECKED_GL_CALL(glDeleteVertexArrays(1, &vao));
    
    lines.clear();
}
