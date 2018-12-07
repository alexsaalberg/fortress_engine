//
//  PhysicsSystem.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/8/18.
//

#ifndef PhysicsSystem_hpp
#define PhysicsSystem_hpp

#include "System.hpp"

class PhysicsSystem : public System {
public:
    virtual void step(double t, double dt);
};

#endif /* PhysicsSystem_hpp */
