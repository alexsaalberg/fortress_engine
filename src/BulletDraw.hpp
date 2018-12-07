//
//  BulletDraw.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 5/5/18.
//

#ifndef BulletDraw_hpp
#define BulletDraw_hpp

#include <btIDebugDraw.h>
#include <vector>
#include <memory>
#include "Program.hpp"
#include <glm/gtc/type_ptr.hpp>


struct
{
    glm::vec3 p1;
    glm::vec3 p2;
} typedef DebugLine;

class BulletDraw : public btIDebugDraw {
public:
    std::vector<DebugLine> lines;
    int debug_mode;
    
    virtual void drawLine(const btVector3 &from, const btVector3 &to, const btVector3 &color);
    virtual void setDebugMode(int debugMode);
    virtual int getDebugMode() const;
    
    virtual void drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color);
    
    virtual void reportErrorWarning(const char* warningString);
    
    virtual void draw3dText(const btVector3& location,const char* textString);

    
    void drawAllLines(std::shared_ptr<Program> program);
};
#endif /* BulletDraw_hpp */
