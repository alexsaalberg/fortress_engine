# Feat-AO

So this one went pretty straight forward.

First, I cloned the [PolyVox Repo](https://bitbucket.org/volumesoffun/polyvox) to [github](https://github.com/alexsaalberg/polyvox). Then, I added [this path](https://gist.github.com/mgerhardy/e3e06822f7b8813fa31b88fb3e23bae6) from [here](https://mgerhardy.blogspot.com/2016/06/ambient-occlusion-for-polyvox.html) to it.

This patch adds Ambient Occlusions support to PolyVox. It had to be modified (just slightly) to add a new file, `DefaultContributeToAO.h`, to be installed to `usr/local/include`.

Then I had to modify FortressEngine to support this new version of PolyVox. I created a `FortressContributeToAO`, which determines which block types contribute to AO or not (similar to `IsQuadNeeded`). 

I created a new shader set, `voxel_AOCalc_{frag,vert}.glsl`, and a new `Program`, `voxelAmbientOcclusionProgram` to render the extra AO data.

`ChunkSystem` also had to be modified to correctly create the new VAOs for the new shader programs.

I also changed `VolumeRenderSystem` to use AO as well. (Which now seems to render with even more artifacts than before :/). 