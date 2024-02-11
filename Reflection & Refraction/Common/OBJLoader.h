/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: OBJLoader.h
Purpose: Load Shaders, Read obj files and set buffer for render
Language: C++
Platform: VisualStudio 2019 / Window
Project: uijin.lee_CS300_3
Author: Uijin Lee, uijn.lee, 180003221
Creation date: Nov 11st, 2021
End Header --------------------------------------------------------*/
#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>

#include "SceneManager.h"

#include "OBJReader.h"
#include "Mesh.h"


class OBJLoader
{
public:
	OBJLoader();
	~OBJLoader();

	struct Object
	{
		Mesh objMesh = Mesh();
		GLuint VAO = 0;
		GLuint normBuf = 0;
		glm::mat4 model = glm::mat4(1.0f);
		glm::vec3 color = glm::vec3(1.0f);
		GLuint  program = 0;
		GLuint uniformBlock = 0;
	};

	struct TypeCube
	{
		Mesh objMesh = Mesh();
		GLuint VAO = 0;
		GLuint normBuf = 0;
		GLuint  program = 0;
	};

	void ReloadOBJ(const char* vert, const char* frag);
	void LoadObj(const char* vert, const char* frag);
	void SetupBuffers(int index);
	void bufferBind(int index);
	void LoadCube(const char* vert, const char* frag);
	TypeCube getCube();

	Object GetObject(int index);
	void cleanUp();
	void reloadUV(bool entity, int type);
private:
	GLuint  programID;
	GLuint ubo;

	OBJReader* objManager;

	Object sphere;
	Object orbit;
	Object sphere_4;
	Object bunny;
	Object cube;
	Object sphere_modified;
	Object quad;
	TypeCube Cube;

	std::vector<Object> objs;
};




