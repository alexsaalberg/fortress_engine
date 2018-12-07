//
//  SelectionStack.hpp
//  CastleSim
//
//  Created by Alex Saalberg on 4/13/18.
//

#ifndef Selection_hpp
#define Selection_hpp

#include <vector>

#include <memory>

#include <glm/gtc/type_ptr.hpp>

#include "PolyVox/Region.h"
#include "PolyVox/RawVolume.h"

template <class VoxelType> class Selection; //forward declaration

//SOURCE
template <class VoxelType>
class GenericSource {
public:
    GenericSource(){}
    ~GenericSource(){}
    virtual VoxelType samplePosition(PolyVox::Vector3DInt32 position)=0;
};

template <class VoxelType>
class FillSource : GenericSource<VoxelType> {
    VoxelType samplePosition(PolyVox::Vector3DInt32 position);
    VoxelType voxel;
};

template <class VoxelType>
class VolumeSource : GenericSource<VoxelType> {
    VoxelType samplePosition(PolyVox::Vector3DInt32 position);
    PolyVox::RawVolume<VoxelType> volume;
};

//SAMPLER
class GenericSampler {
public:
    GenericSampler(){}
    ~GenericSampler(){}
    virtual PolyVox::Vector3DInt32 getTextureCoordinates(PolyVox::Region region, PolyVox::Vector3DInt32 position)=0;
};

class SimpleSampler {
public:
    PolyVox::Vector3DInt32 getTextureCoordinates(PolyVox::Region region, PolyVox::Vector3DInt32 position);
};

//SELECTION (also a source)
template <class VoxelType>
class Selection : GenericSource<VoxelType> {
public:
    PolyVox::Region region;
    std::shared_ptr<GenericSampler> sampler;
    std::shared_ptr<GenericSource<VoxelType>> source;
    PolyVox::Vector3DInt32 offset;
    
    //Implicitly deleted stuff because templates
    /*
    Selection();
    ~Selection();
    Selection(const Selection<VoxelType> &selection); */
    Selection() {
        
    }
    ~Selection() {
        
    }
    Selection(const Selection<VoxelType>& selection) {
        region = selection.region;
        sampler = selection.sampler;
        source = selection.source;
        offset = selection.offset;
    }

    VoxelType samplePosition(PolyVox::Vector3DInt32 position) {
        PolyVox::Vector3DInt32 textureCoordinates = sampler->getTextureCoordinates(region, position);
        
        return source->samplePosition(textureCoordinates);
    }

    //Generic Source Inheritance
    //VoxelType samplePosition(PolyVox::Vector3DInt32 position);
};

#endif /* Selection_hpp */
