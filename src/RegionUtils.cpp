//
//  RegionUtils.cpp
//  CastleSim
//
//  Created by Alex Saalberg on 5/1/18.
//

#include "RegionUtils.hpp"

using namespace PolyVox;

Region PolyVoxExtensions::createProperRegion(Region& region) {
    Region new_region;
    
    if(region.getLowerX() < region.getUpperX()) {
        new_region.setLowerX(region.getLowerX());
        new_region.setUpperX(region.getUpperX());
    } else {
        new_region.setLowerX(region.getUpperX());
        new_region.setUpperX(region.getLowerX());
    }
    
    if(region.getLowerY() < region.getUpperY()) {
        new_region.setLowerY(region.getLowerY());
        new_region.setUpperY(region.getUpperY());
    } else {
        new_region.setLowerY(region.getUpperY());
        new_region.setUpperY(region.getLowerY());
    }
    
    if(region.getLowerZ() < region.getUpperZ()) {
        new_region.setLowerZ(region.getLowerZ());
        new_region.setUpperZ(region.getUpperZ());
    } else {
        new_region.setLowerZ(region.getUpperZ());
        new_region.setUpperZ(region.getLowerZ());
    }
    
    
    
    return new_region;
}
