//
//  SelectionSystem.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/25/18.
//

#ifndef SelectionSystem_hpp
#define SelectionSystem_hpp

#include <memory>

#include "Program.hpp"
#include "System.hpp"

class SelectionSystem : public System {
public:
    virtual void step(double t, double dt);
};

#endif /* SelectionSystem_hpp */
