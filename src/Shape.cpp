
#include "Shape.hpp"
#include <iostream>
#include <cassert>

#include "GLSL.hpp"
#include "Program.hpp"

using namespace std;

#define PI 3.14f


// copy the data from the shape to this object
void Shape::createShape(tinyobj::shape_t & shape)
{
	posBuf = shape.mesh.positions;
	norBuf = shape.mesh.normals;
	texBuf = shape.mesh.texcoords;
	eleBuf = shape.mesh.indices;
}

void Shape::measure()
{
	float minX, minY, minZ;
	float maxX, maxY, maxZ;

	minX = minY = minZ = std::numeric_limits<float>::max();
	maxX = maxY = maxZ = -std::numeric_limits<float>::max();

	//Go through all vertices to determine min and max of each dimension
	for (size_t v = 0; v < posBuf.size() / 3; v++)
	{
		if (posBuf[3*v+0] < minX) minX = posBuf[3 * v + 0];
		if (posBuf[3*v+0] > maxX) maxX = posBuf[3 * v + 0];

		if (posBuf[3*v+1] < minY) minY = posBuf[3 * v + 1];
		if (posBuf[3*v+1] > maxY) maxY = posBuf[3 * v + 1];

		if (posBuf[3*v+2] < minZ) minZ = posBuf[3 * v + 2];
		if (posBuf[3*v+2] > maxZ) maxZ = posBuf[3 * v + 2];
	}

	min.x = minX;
	min.y = minY;
	min.z = minZ;
	max.x = maxX;
	max.y = maxY;
	max.z = maxZ;
}

void Shape::init()
{
	// Initialize the vertex array object
	CHECKED_GL_CALL(glGenVertexArrays(1, &vaoID));
	CHECKED_GL_CALL(glBindVertexArray(vaoID));

	// Send the position array to the GPU
	CHECKED_GL_CALL(glGenBuffers(1, &posBufID));
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, posBufID));
	CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, posBuf.size()*sizeof(float), &posBuf[0], GL_STATIC_DRAW));

	// Send the normal array to the GPU
	if (norBuf.empty())
	{
		norBufID = 0;
	}
	else
	{
		CHECKED_GL_CALL(glGenBuffers(1, &norBufID));
		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, norBufID));
		CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, norBuf.size()*sizeof(float), &norBuf[0], GL_STATIC_DRAW));
	}

	// Send the texture array to the GPU
	if (texBuf.empty())
	{
		texBufID = 0;
	}
	else
	{
		CHECKED_GL_CALL(glGenBuffers(1, &texBufID));
		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, texBufID));
		CHECKED_GL_CALL(glBufferData(GL_ARRAY_BUFFER, texBuf.size()*sizeof(float), &texBuf[0], GL_STATIC_DRAW));
	}

	// Send the element array to the GPU
	CHECKED_GL_CALL(glGenBuffers(1, &eleBufID));
	CHECKED_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID));
	CHECKED_GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, eleBuf.size()*sizeof(unsigned int), &eleBuf[0], GL_STATIC_DRAW));

	// Unbind the arrays
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	CHECKED_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}

void Shape::draw(const shared_ptr<Program> prog) const
{
	int h_pos, h_nor, h_tex;
	h_pos = h_nor = h_tex = -1;

	CHECKED_GL_CALL(glBindVertexArray(vaoID));

	// Bind position buffer
	h_pos = prog->getAttribute("vPosition");
	GLSL::enableVertexAttribArray(h_pos);
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, posBufID));
	CHECKED_GL_CALL(glVertexAttribPointer(h_pos, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0));

	// Bind normal buffer
	h_nor = prog->getAttribute("vNormal");
	if (h_nor != -1 && norBufID != 0)
	{
		GLSL::enableVertexAttribArray(h_nor);
		CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, norBufID));
		CHECKED_GL_CALL(glVertexAttribPointer(h_nor, 3, GL_FLOAT, GL_FALSE, 0, (const void *)0));
	}

	if (texBufID != 0)
	{
		// Bind texcoords buffer
		h_tex = prog->getAttribute("vTextureCoordinates");

		if (h_tex != -1 && texBufID != 0)
		{
			GLSL::enableVertexAttribArray(h_tex);
			CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, texBufID));
			CHECKED_GL_CALL(glVertexAttribPointer(h_tex, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0));
		}
	}

	// Bind element buffer
	CHECKED_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eleBufID));

	// Draw
	CHECKED_GL_CALL(glDrawElements(GL_TRIANGLES, (int)eleBuf.size(), GL_UNSIGNED_INT, (const void *)0));

	// Disable and unbind
	if (h_tex != -1)
	{
		GLSL::disableVertexAttribArray(h_tex);
	}
	if (h_nor != -1)
	{
		GLSL::disableVertexAttribArray(h_nor);
	}
	GLSL::disableVertexAttribArray(h_pos);
	CHECKED_GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	CHECKED_GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
}


void Shape::makeCylinder(int numCircles, int pointsPerCircle, float circleRadius, float heightBetween) {
    assert(numCircles > 2);
    assert(pointsPerCircle > 2);
    
    std::vector<unsigned int> indexVector(3 * 2 * (numCircles-1) * pointsPerCircle);
    std::vector<float> vertexVector(3 * numCircles * pointsPerCircle);
    std::vector<float> normalVector(3 * numCircles * pointsPerCircle);
    std::vector<float> textureVector(2 * numCircles * pointsPerCircle);
    
    float degreesPerPoint = 360.0f / (float) pointsPerCircle;
    
    float thisAngle;
    unsigned int vertIndex, faceIndex, textureIndex;
    unsigned int a_vert, b_vert, c_vert; //vertices on a triangle
    
    /* VERTICES */
    for(int p = 0; p < pointsPerCircle; p++) { // p : pointNumber
        thisAngle = degreesPerPoint*(p);
        for(int c = 0; c < numCircles; c++ ) { // c : circleNumber
            vertIndex = p*numCircles + c; //index of this vertex
            vertIndex *= 3; //3 indices (XYZ) in vertexVector per vertex
            
            textureIndex = p*numCircles + c;
            textureIndex *= 2; //2 indices (UV) in textureVector per vertex
            
            
            vertexVector[vertIndex + 0] = (sin(glm::radians(thisAngle))) * circleRadius;
            vertexVector[vertIndex + 1] = (cos(glm::radians(thisAngle))) * circleRadius;
            vertexVector[vertIndex + 2] = c * heightBetween;
            
            normalVector[vertIndex + 0] = vertexVector[vertIndex + 0];
            normalVector[vertIndex + 1] = vertexVector[vertIndex + 1];
            normalVector[vertIndex + 2] = vertexVector[vertIndex + 2];
            
            //Coordinates in bitmap are (pointNumber, circleNumber)
            textureVector[textureIndex] = p / (float) pointsPerCircle; //Range [0, 1.0]
            textureVector[textureIndex + 1] = c / (float) numCircles; //Range [0, 1.0]
            
        }
    }
    
    /* FACES (Indices) */
    int numLayers = numCircles-1;
    for(int p = 0; p < pointsPerCircle; p++) { // P : pointNumber
        //s alternates between 0 and 1
        for(int s = 0; s < 2; s++) { // s: segment
            for(int l = 0; l < numLayers; l++) { // l : layerNumber
                //vertIndex = l + s*numLayers + p*2*numLayers; //vertexIndex
                
                a_vert = p * numCircles + l; //notice the numCircles
                if(s == 0) {
                    b_vert = a_vert + 1;
                    c_vert = b_vert + numCircles; //notice the numCircles
                } else if (s==1) {
                    c_vert = a_vert + numCircles;
                    b_vert = c_vert + 1;
                }
                
                faceIndex = l + s*numLayers + p*2*numLayers; //faceIndex
                faceIndex *= 3; //3 vertices (indices) per face
                indexVector[faceIndex + 0] = a_vert;
                indexVector[faceIndex + 1] = b_vert;
                indexVector[faceIndex + 2] = c_vert;
            }
        }
    }
    
    //Fix the last column of triangles/faces to wrap
    for(int s = 0; s < 2; s++) {
        for(int l = 0; l < numLayers; l++ ) {
            
            faceIndex = l + s*numLayers + (pointsPerCircle - 1)*2*numLayers;
            faceIndex *= 3; //3 vertices per face
            
            if(s == 0) {
                indexVector[faceIndex + 2] = l+1; //c
            } else if (s == 1) {
                indexVector[faceIndex + 1] = l+1; //b
                indexVector[faceIndex + 2] = l; //c
            }
        }
    }
    
    posBuf = vertexVector;
    eleBuf = indexVector;
    norBuf = normalVector;
    texBuf = textureVector;
    
}
