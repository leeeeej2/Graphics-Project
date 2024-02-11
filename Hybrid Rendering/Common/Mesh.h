/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Mesh.h
Purpose: Create Mesh, calculate and provide mesh data
Language: C++
Platform: VisualStudio 2019 / Window
Project: uijin.lee_CS300_2
Author: Uijin Lee, uijn.lee, 180003221
Creation date: Nov 11st, 2021
End Header --------------------------------------------------------*/
#ifndef SIMPLE_OBJ_SCENE_MESH_H
#define SIMPLE_OBJ_SCENE_MESH_H

#include <vector>

#include <glm/glm.hpp>


class Mesh
{
public:
	friend class OBJReader;
	friend class SceneManager;
	friend class OBJLoader;

	// Get attribute values buffer
	GLfloat* getVertexBuffer();             // attribute 0
	GLfloat* getVertexNormals();            // attribute 1
	GLfloat* getVertexUVs();                // attribute 2

	GLfloat* getVertexNormalsForDisplay();  // attribute 0

	unsigned int getVertexBufferSize();
	unsigned int getVertexCount();
	unsigned int getVertexNormalCount();

	// Get vertex index buffer
	GLuint* getIndexBuffer();
	unsigned int getIndexBufferSize();
	unsigned int getTriangleCount();

	glm::vec3   getModelScale();
	glm::vec3   getModelCentroid();
	glm::vec3   getCentroidVector(glm::vec3 vVertex);


	GLfloat& getNormalLength();
	void setNormalLength(GLfloat nLength);

	// initialize the data members
	void initData();

	// calculate vertex normals
	int calcVertexNormals(GLboolean bFlipNormals = false);

	// calculate the "display" normals
	void calcVertexNormalsForDisplay(GLboolean bFlipNormals = false);

	// calculate texture coordinates
	enum class UVType {
		PLANAR_UV = 0,
		CYLINDRICAL_UV,
		SPHERICAL_UV,
		CUBE_MAPPED_UV
	};

	int         calcUVs(bool Entity, Mesh::UVType uvType = UVType::PLANAR_UV);
	glm::vec2   calcCubeMap(glm::vec3 vEntity);

private:
	std::vector<glm::vec3>    vertexBuffer;
	std::vector<GLuint>       vertexIndices;
	std::vector<glm::vec2>    vertexUVs;
	std::vector<glm::vec3>    vertexNormals, vertexNormalDisplay;
	std::vector<glm::vec3>    faceNormalsDisplay;

	glm::vec3               boundingBox[2];
	GLfloat                 normalLength = 0.f;
};



#endif //SIMPLE_OBJ_SCENE_MESH_H
