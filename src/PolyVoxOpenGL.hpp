//
//  PolyVoxOpenGL.h
//  CastleSim
//
//  Created by Alex Saalberg on 4/30/18.
//

#ifndef PolyVoxOpenGL_h
#define PolyVoxOpenGL_h

#include <memory>
#include <glad/glad.h>

#include "Program.hpp"
#include "MatrixStack.hpp"

#include "GLSL.hpp" //CHECK_GL_CALL, among others

namespace PolyVoxExtensions {

    struct VolumeMeshData
    {
        GLuint number_of_indices;
        GLenum index_type;
        GLuint index_buffer;
        GLuint vertex_buffer;
        GLuint vertex_array_object;
        glm::vec3 translation;
        float scale;
        double clean_time;
    };

    static void renderVolume(double t, VolumeMeshData& mesh_data, std::shared_ptr<MatrixStack> M, std::shared_ptr<Program> program) {
        if(mesh_data.clean_time >= 0.0f) { // -1.0f means not created yet
            M->pushMatrix();
                M->translate(mesh_data.translation);
                M->scale(mesh_data.scale);
                glUniformMatrix4fv(program->getUniform("M"), 1, GL_FALSE, value_ptr(M->topMatrix()));
            M->popMatrix();
            
            // Bind the vertex array for the current mesh
            CHECKED_GL_CALL( glBindVertexArray(mesh_data.vertex_array_object) );
            // Draw the mesh
            CHECKED_GL_CALL( glDrawElements(GL_TRIANGLES, mesh_data.number_of_indices, mesh_data.index_type, 0) );
            // Unbind the vertex array.
            CHECKED_GL_CALL( glBindVertexArray(0) );
        }
    }
    
    // Convert a PolyVox mesh to OpenGL index/vertex buffers. Inlined because it's templatised.
    template <typename MeshType>
    static VolumeMeshData bindMesh(double t, const MeshType& surfaceMesh, const PolyVox::Vector3DInt32& translation, float scale)
    {
        // This struct holds the OpenGL properties (buffer handles, etc) which will be used
        // to render our mesh. We copy the data from the PolyVox mesh into this structure.
        VolumeMeshData meshData;
        
        // Create the VAO for the mesh
        CHECKED_GL_CALL( glGenVertexArrays(1, &(meshData.vertex_array_object)) );
        glBindVertexArray(meshData.vertex_array_object);
        
        // The GL_ARRAY_BUFFER will contain the list of vertex positions
        CHECKED_GL_CALL( glGenBuffers(1, &(meshData.vertex_buffer)) );
        glBindBuffer(GL_ARRAY_BUFFER, meshData.vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER, surfaceMesh.getNoOfVertices() * sizeof(typename MeshType::VertexType), surfaceMesh.getRawVertexData(), GL_STATIC_DRAW);
        
        // and GL_ELEMENT_ARRAY_BUFFER will contain the indices
        CHECKED_GL_CALL( glGenBuffers(1, &(meshData.index_buffer)) );
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, meshData.index_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, surfaceMesh.getNoOfIndices() * sizeof(typename MeshType::IndexType), surfaceMesh.getRawIndexData(), GL_STATIC_DRAW);
        
        // Every surface extractor outputs valid positions for the vertices, so tell OpenGL how these are laid out
        glEnableVertexAttribArray(0); // Attrib '0' is the vertex positions
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(typename MeshType::VertexType), (GLvoid*)(offsetof(typename MeshType::VertexType, position))); //take the first 3 floats from every sizeof(decltype(vecVertices)::value_type)
        
        // Some surface extractors also generate normals, so tell OpenGL how these are laid out. If a surface extractor
        // does not generate normals then nonsense values are written into the buffer here and sghould be ignored by the
        // shader. This is mostly just to simplify this example code - in a real application you will know whether your
        // chosen surface extractor generates normals and can skip uploading them if not.
        glEnableVertexAttribArray(1); // Attrib '1' is the vertex normals.
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(typename MeshType::VertexType), (GLvoid*)(offsetof(typename MeshType::VertexType, normal)));
        
        // Finally a surface extractor will probably output additional data. This is highly application dependant. For this example code
        // we're just uploading it as a set of bytes which we can read individually, but real code will want to do something specialised here.
        glEnableVertexAttribArray(2); //We're talking about shader attribute '2'
        GLint size = (std::min)(sizeof(typename MeshType::VertexType::DataType), size_t(4)); // Can't upload more that 4 components (vec4 is GLSL's biggest type)
        glVertexAttribIPointer(2, size, GL_UNSIGNED_BYTE, sizeof(typename MeshType::VertexType), (GLvoid*)(offsetof(typename MeshType::VertexType, data)));
        
        // We're done uploading and can now unbind.
        glBindVertexArray(0);
        
        // A few additional properties can be copied across for use during rendering.
        meshData.number_of_indices = surfaceMesh.getNoOfIndices();
        meshData.translation = glm::vec3(translation.getX(), translation.getY(), translation.getZ());
        meshData.scale = scale;
        
        // Set 16 or 32-bit index buffer size.
        meshData.index_type = sizeof(typename MeshType::IndexType) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT;
        
        meshData.clean_time = t;
        
        return meshData;
    }
    


    template <typename VoxelType>
    static VolumeMeshData genMeshForRawVolume(double t, std::shared_ptr< PolyVox::RawVolume<VoxelType> > volume) {

        // Perform the extraction for this region of the volume
        auto region = volume->getEnclosingRegion();
        region.grow(1);
        auto mesh = extractCubicMesh(volume.get(), region);
        //auto mesh = extractMarchingCubesMesh(pagedData.get(), regToExtract);
        
        // The returned mesh needs to be decoded to be appropriate for GPU rendering.
        auto decodedMesh = decodeMesh(mesh);
        
        if(decodedMesh.getNoOfIndices() > 0) {
            //printf("Calculating mesh for (%d %d %d)(%d %d %d). %zu indices.\n", lower_x, lower_y, lower_z, upper_x, upper_y, upper_z, decodedMesh.getNoOfIndices());
        }
        
        // Pass the surface to the OpenGL window. Note that we are also passing an offset in this multi-mesh example. This is because
        // the surface extractors return a mesh with 'local space' positions to reduce storage requirements and precision problems.
        return bindMesh(t, decodedMesh, decodedMesh.getOffset(), 1.0f);
    }

}

#endif /* PolyVoxOpenGL_h */
