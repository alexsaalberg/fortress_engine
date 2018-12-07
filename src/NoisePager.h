//
//  NoisePager.h
//  CastleSim
//
//  Created by Alex Saalberg on 4/2/18.
//

#ifndef NoisePager_h
#define NoisePager_h

#include "PolyVox/MaterialDensityPair.h"
#include "PolyVox/CubicSurfaceExtractor.h"
#include "PolyVox/MarchingCubesSurfaceExtractor.h"
#include "PolyVox/Mesh.h"
#include "PolyVox/PagedVolume.h"

#include "FastNoise.h"
#include "CastleDef.h"

using namespace PolyVox;

/**
 * Generates data using Perlin noise.
 */
class NoisePager : public PolyVox::PagedVolume<uint8_t>::Pager
{
public:
    FastNoise myNoise; // Create a FastNoise object
    
    /// Constructor
    NoisePager():PagedVolume<uint8_t>::Pager()
    {
        myNoise.SetNoiseType(FastNoise::SimplexFractal); // Set the desired noise type
        
        myNoise.SetFractalOctaves(4);
        myNoise.SetFractalLacunarity(3);
        myNoise.SetFractalGain(0.5f);
    }
    
    /// Destructor
    virtual ~NoisePager() {};
    
    virtual void pageIn(const PolyVox::Region& region, PagedVolume<uint8_t>::Chunk* pChunk)
    {
        static int counter = 0;
        counter++;
        //printf("Paging in chunk%d, %d-%d %d-%d %d-%d\n", counter, region.getLowerX(), region.getUpperX(), region.getLowerY(), region.getUpperY(), region.getLowerZ(), region.getUpperZ());
        
        uint8_t voxelVal = 0;
        
        for(int x = region.getLowerX(); x <= region.getUpperX(); x++) {
            for(int z = region.getLowerZ(); z <= region.getUpperZ(); z++) {
                
                float val = myNoise.GetValue(x, z);
                
                val += 1.0f;
                val *= 0.5f;
                
                int height = (int) (val * 100.0f);
                
                //printf("X%d Z%d, height %d\n", x, z, height);
                
                for(int y = region.getLowerY(); y <= region.getUpperY(); y++) {
                    voxelVal = BLOCK_AIR;
                    
                    if(y < height) {
                        //printf("X%d Z%d, Y%d H%d\n", x, z, y, height);
                        voxelVal = BLOCK_GRASS;
                    }
                    
                    // Voxel position within a chunk always start from zero. So if a chunk represents region (4, 8, 12) to (11, 19, 15)
                    // then the valid chunk voxels are from (0, 0, 0) to (7, 11, 3). Hence we subtract the lower corner position of the
                    // region from the volume space position in order to get the chunk space position.
                    pChunk->setVoxel(x - region.getLowerX(), y - region.getLowerY(), z - region.getLowerZ(), voxelVal);
                }
            }
        }
    }
    
    virtual void pageOut(const PolyVox::Region& region, PagedVolume<uint8_t>::Chunk* /*pChunk*/)
    {
        std::cout << "warning unloading region: " << region.getLowerCorner() << " -> " << region.getUpperCorner() << std::endl;
    }
};

#endif /* NoisePager_h */
