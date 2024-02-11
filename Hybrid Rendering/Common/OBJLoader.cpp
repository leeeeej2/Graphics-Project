/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: OBJLoader.cpp
Purpose: Load Shaders, get data from obj files and set buffer for render
Language: C++
Platform: VisualStudio 2019 / Window
Project: uijin.lee_CS300_2
Author: Uijin Lee, uijn.lee, 180003221
Creation date: Nov 11st, 2021
End Header --------------------------------------------------------*/
#include "OBJLoader.h"

#include "shader.hpp"

OBJLoader::OBJLoader():programID(0), ubo(0), objManager(nullptr)
{
	objs.clear();
}

OBJLoader::~OBJLoader()
{
	cleanUp();
}

void OBJLoader::ReloadOBJ(const char* vert, const char* frag)
{
	programID = LoadShaders(vert/*"../Common/shaders/phongShading.vert"*/,
		frag	/*"../Common/shaders/phongShading.frag"*/);

	sphere.program = programID;
	sphere_4.program = programID;
	bunny.program = programID;
	cube.program = programID;
	sphere_modified.program = programID;
	quad.program = programID;

	objs.clear();

	objs.push_back(sphere);
	objs.push_back(sphere_4);
	objs.push_back(bunny);
	objs.push_back(cube);
	objs.push_back(sphere_modified);
	objs.push_back(quad);
}

void OBJLoader::LoadObj(const char* vert, const char* frag)
{
	programID = LoadShaders(vert/*"../Common/shaders/phongShading.vert"*/,
		frag	/*"../Common/shaders/phongShading.frag"*/);
	{
		GLuint uniformBlockLight = glGetUniformBlockIndex(programID, "Light");

		glUniformBlockBinding(programID, uniformBlockLight, 0);

		glGenBuffers(1, &ubo);
		glBindBuffer(GL_UNIFORM_BUFFER, ubo);
		glBufferData(GL_UNIFORM_BUFFER, 16 * 112, NULL, GL_STATIC_DRAW);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);

		glBindBufferRange(GL_UNIFORM_BUFFER, 0, ubo, 0, 16 * 112);
	}

	objManager = new(OBJReader);
	{
		sphere.uniformBlock = ubo;
		sphere.program = programID;
		objManager->ReadOBJFile("../Common/models/sphere.obj", &sphere.objMesh, OBJReader::ReadMethod::LINE_BY_LINE, false);
		//objManager->ReadOBJFile("../Common/models/g12.obj", &sphere.objMesh, OBJReader::ReadMethod::LINE_BY_LINE, false);

		sphere.objMesh = *(objManager->getMesh());
	}
	{
		sphere_4.uniformBlock = ubo;
		sphere_4.program = programID;
		objManager->ReadOBJFile("../Common/models/4Sphere.obj", &sphere_4.objMesh, OBJReader::ReadMethod::LINE_BY_LINE, false);
		sphere_4.objMesh = *(objManager->getMesh());
	}
	{
		bunny.uniformBlock = ubo;
		bunny.program = programID;
		objManager->ReadOBJFile("../Common/models/bunny.obj", &bunny.objMesh, OBJReader::ReadMethod::LINE_BY_LINE, false);
		bunny.objMesh = *(objManager->getMesh());
	}
	{
		cube.uniformBlock = ubo;
		cube.program = programID;
		objManager->ReadOBJFile("../Common/models/cube.obj", &cube.objMesh, OBJReader::ReadMethod::LINE_BY_LINE, true);
		cube.objMesh = *(objManager->getMesh());
	}
	{
		sphere_modified.uniformBlock = ubo;
		sphere_modified.program = programID;
		objManager->ReadOBJFile("../Common/models/sphere_modified.obj", &sphere_modified.objMesh, OBJReader::ReadMethod::LINE_BY_LINE, false);
		sphere_modified.objMesh = *(objManager->getMesh());
	}
	{
		quad.uniformBlock = ubo;
		quad.program = programID;
		objManager->ReadOBJFile("../Common/models/quad.obj", &quad.objMesh, OBJReader::ReadMethod::LINE_BY_LINE, true);
		quad.objMesh = *(objManager->getMesh());
	}

	for (int i = 0; i < 6; i++)
	{
		SetupBuffers(i);
	}
}

void OBJLoader::SetupBuffers(int index)
{
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	GLuint norm;
	GLuint uv;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &norm);
	glGenBuffers(1, &ebo);
	glGenBuffers(1, &uv);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	switch (index)
	{
	case 0:
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sphere.objMesh.getVertexBufferSize()) * 3 *sizeof(GLfloat),
			sphere.objMesh.getVertexBuffer(), GL_STATIC_DRAW);//

		glBindBuffer(GL_ARRAY_BUFFER, norm);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sphere.objMesh.getVertexBufferSize()) * 3 * sizeof(GLfloat),
			sphere.objMesh.getVertexNormals(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere.objMesh.getIndexBufferSize() * sizeof(GLuint),
			sphere.objMesh.getIndexBuffer(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, uv);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sphere.objMesh.getVertexBufferSize()) * 2 * sizeof(GLfloat),
			sphere.objMesh.getVertexUVs(), GL_STATIC_DRAW);
		break;
	case 1:
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sphere_4.objMesh.getVertexBufferSize()) * 3 * sizeof(GLfloat),
			sphere_4.objMesh.getVertexBuffer(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, norm);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sphere_4.objMesh.getVertexBufferSize()) * 3 * sizeof(GLfloat),
			sphere_4.objMesh.getVertexNormals(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere_4.objMesh.getIndexBufferSize() * sizeof(GLuint),
			sphere_4.objMesh.getIndexBuffer(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, uv);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sphere_4.objMesh.getVertexBufferSize()) * 2 * sizeof(GLfloat),
			sphere_4.objMesh.getVertexUVs(), GL_STATIC_DRAW);
		break;
	case 2:
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(bunny.objMesh.getVertexBufferSize())* 3 * sizeof(GLfloat),
			bunny.objMesh.getVertexBuffer(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, norm);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(bunny.objMesh.getVertexBufferSize()) * 3 * sizeof(GLfloat),
			bunny.objMesh.getVertexNormals(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(bunny.objMesh.getIndexBufferSize()) * sizeof(GLuint),
			bunny.objMesh.getIndexBuffer(), GL_STATIC_DRAW);
	
		glBindBuffer(GL_ARRAY_BUFFER, uv);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(bunny.objMesh.getVertexBufferSize()) * 2 * sizeof(GLfloat),
			bunny.objMesh.getVertexUVs(), GL_STATIC_DRAW);
		break;
	case 3:
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(cube.objMesh.getVertexBufferSize()) * 3 * sizeof(GLfloat),
			cube.objMesh.getVertexBuffer(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, norm);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(cube.objMesh.getVertexBufferSize()) * 3 * sizeof(GLfloat),
			cube.objMesh.getVertexNormals(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube.objMesh.getIndexBufferSize() * sizeof(GLuint),
			cube.objMesh.getIndexBuffer(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, uv);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(cube.objMesh.getVertexBufferSize()) * 2 * sizeof(GLfloat),
			cube.objMesh.getVertexUVs(), GL_STATIC_DRAW);
		break;
	case 4:
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sphere_modified.objMesh.getVertexBufferSize()) * 3 * sizeof(GLfloat),
			sphere_modified.objMesh.getVertexBuffer(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, norm);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sphere_modified.objMesh.getVertexBufferSize()) * 3 * sizeof(GLfloat),
			sphere_modified.objMesh.getVertexNormals(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere_modified.objMesh.getIndexBufferSize() * sizeof(GLuint),
			sphere_modified.objMesh.getIndexBuffer(), GL_STATIC_DRAW);
		
		glBindBuffer(GL_ARRAY_BUFFER, uv);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sphere_modified.objMesh.getVertexBufferSize()) * 2 * sizeof(GLfloat),
			sphere_modified.objMesh.getVertexUVs(), GL_STATIC_DRAW);
		break;
	case 5:
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(quad.objMesh.getVertexBufferSize()) * 3 * sizeof(GLfloat),
			quad.objMesh.getVertexBuffer(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, norm);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(quad.objMesh.getVertexBufferSize()) * 3 * sizeof(GLfloat),
			quad.objMesh.getVertexNormals(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, quad.objMesh.getIndexBufferSize() * sizeof(GLuint),
			quad.objMesh.getIndexBuffer(), GL_STATIC_DRAW);
		
		glBindBuffer(GL_ARRAY_BUFFER, uv);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(quad.objMesh.getVertexBufferSize()) * 2 * sizeof(GLfloat),
			quad.objMesh.getVertexUVs(), GL_STATIC_DRAW);
		break;
	}

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0,
		3,
		GL_FLOAT,
		GL_FALSE,
		3 * sizeof(GLfloat),
		(void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, norm);
	glVertexAttribPointer(1,
		3,
		GL_FLOAT,
		GL_FALSE,
		3 * sizeof(GLfloat),
		(void*)0);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, uv);
	glVertexAttribPointer(2,
		2,
		GL_FLOAT,
		GL_FALSE,
		2 * sizeof(GLfloat),
		(void*)0);

	switch (index)
	{
	case 0:
		sphere.VAO = vao;
		sphere.normBuf = norm;
		objs.push_back(sphere);
		break;
	case 1:
		sphere_4.VAO = vao;
		sphere_4.normBuf = norm;
		objs.push_back(sphere_4);
		break;
	case 2:
		bunny.VAO = vao;
		bunny.normBuf = norm;
		objs.push_back(bunny);
		break;
	case 3:
		cube.VAO = vao;
		cube.normBuf = norm;
		objs.push_back(cube);
		break;
	case 4:
		sphere_modified.VAO = vao;
		sphere_modified.normBuf = norm;
		objs.push_back(sphere_modified);
		break;
	case 5:
		quad.VAO = vao;
		quad.normBuf = norm;
		objs.push_back(quad);
		break;
	}

	glBindVertexArray(0);
}

void OBJLoader::bufferBind(int index)
{
	GLuint vao;
	GLuint vbo;
	GLuint ebo;
	GLuint norm;
	GLuint uv;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &norm);
	glGenBuffers(1, &ebo);
	glGenBuffers(1, &uv);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	switch (index)
	{
	case 0:
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sphere.objMesh.getVertexBufferSize()) * 3 * sizeof(GLfloat),
			sphere.objMesh.getVertexBuffer(), GL_STATIC_DRAW);//

		glBindBuffer(GL_ARRAY_BUFFER, norm);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sphere.objMesh.getVertexBufferSize()) * 3 * sizeof(GLfloat),
			sphere.objMesh.getVertexNormals(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere.objMesh.getIndexBufferSize() * sizeof(GLuint),
			sphere.objMesh.getIndexBuffer(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, uv);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sphere.objMesh.getVertexBufferSize()) * 2 * sizeof(GLfloat),
			sphere.objMesh.getVertexUVs(), GL_STATIC_DRAW);
		break;
	case 1:
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sphere_4.objMesh.getVertexBufferSize()) * 3 * sizeof(GLfloat),
			sphere_4.objMesh.getVertexBuffer(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, norm);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sphere_4.objMesh.getVertexBufferSize()) * 3 * sizeof(GLfloat),
			sphere_4.objMesh.getVertexNormals(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere_4.objMesh.getIndexBufferSize() * sizeof(GLuint),
			sphere_4.objMesh.getIndexBuffer(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, uv);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sphere_4.objMesh.getVertexBufferSize()) * 2 * sizeof(GLfloat),
			sphere_4.objMesh.getVertexUVs(), GL_STATIC_DRAW);
		break;
	case 2:
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(bunny.objMesh.getVertexBufferSize()) * 3 * sizeof(GLfloat),
			bunny.objMesh.getVertexBuffer(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, norm);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(bunny.objMesh.getVertexBufferSize()) * 3 * sizeof(GLfloat),
			bunny.objMesh.getVertexNormals(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, static_cast<GLsizeiptr>(bunny.objMesh.getIndexBufferSize()) * sizeof(GLuint),
			bunny.objMesh.getIndexBuffer(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, uv);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(bunny.objMesh.getVertexBufferSize()) * 2 * sizeof(GLfloat),
			bunny.objMesh.getVertexUVs(), GL_STATIC_DRAW);
		break;
	case 3:
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(cube.objMesh.getVertexBufferSize()) * 3 * sizeof(GLfloat),
			cube.objMesh.getVertexBuffer(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, norm);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(cube.objMesh.getVertexBufferSize()) * 3 * sizeof(GLfloat),
			cube.objMesh.getVertexNormals(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube.objMesh.getIndexBufferSize() * sizeof(GLuint),
			cube.objMesh.getIndexBuffer(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, uv);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(cube.objMesh.getVertexBufferSize()) * 2 * sizeof(GLfloat),
			cube.objMesh.getVertexUVs(), GL_STATIC_DRAW);
		break;
	case 4:
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sphere_modified.objMesh.getVertexBufferSize()) * 3 * sizeof(GLfloat),
			sphere_modified.objMesh.getVertexBuffer(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, norm);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sphere_modified.objMesh.getVertexBufferSize()) * 3 * sizeof(GLfloat),
			sphere_modified.objMesh.getVertexNormals(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere_modified.objMesh.getIndexBufferSize() * sizeof(GLuint),
			sphere_modified.objMesh.getIndexBuffer(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, uv);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(sphere_modified.objMesh.getVertexBufferSize()) * 2 * sizeof(GLfloat),
			sphere_modified.objMesh.getVertexUVs(), GL_STATIC_DRAW);
		break;
	case 5:
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(quad.objMesh.getVertexBufferSize()) * 3 * sizeof(GLfloat),
			quad.objMesh.getVertexBuffer(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, norm);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(quad.objMesh.getVertexBufferSize()) * 3 * sizeof(GLfloat),
			quad.objMesh.getVertexNormals(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, quad.objMesh.getIndexBufferSize() * sizeof(GLuint),
			quad.objMesh.getIndexBuffer(), GL_STATIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, uv);
		glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(quad.objMesh.getVertexBufferSize()) * 2 * sizeof(GLfloat),
			quad.objMesh.getVertexUVs(), GL_STATIC_DRAW);
		break;
	}

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0,
		3,
		GL_FLOAT,
		GL_FALSE,
		3 * sizeof(GLfloat),
		(void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, norm);
	glVertexAttribPointer(1,
		3,
		GL_FLOAT,
		GL_FALSE,
		3 * sizeof(GLfloat),
		(void*)0);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, uv);
	glVertexAttribPointer(2,
		2,
		GL_FLOAT,
		GL_FALSE,
		2 * sizeof(GLfloat),
		(void*)0);

	switch (index)
	{
	case 0:
		sphere.VAO = vao;
		sphere.normBuf = norm;
		break;
	case 1:
		sphere_4.VAO = vao;
		sphere_4.normBuf = norm;
		break;
	case 2:
		bunny.VAO = vao;
		bunny.normBuf = norm;
		break;
	case 3:
		cube.VAO = vao;
		cube.normBuf = norm;
		break;
	case 4:
		sphere_modified.VAO = vao;
		sphere_modified.normBuf = norm;
		break;
	case 5:
		quad.VAO = vao;
		quad.normBuf = norm;
		break;
	}

	glBindVertexArray(0);
}

OBJLoader::Object OBJLoader::GetObj(int index)
{
	return objs.at(index);
}

void OBJLoader::cleanUp()
{
	for (auto& object : objs)
	{
		glDeleteVertexArrays(1, &object.VAO);
		glDeleteBuffers(1, &object.normBuf);
		glDeleteProgram(object.program);

	}
	glDeleteProgram(programID);
}

void OBJLoader::reloadUV(bool entity, int type)
{
	for(int i = 0; i < 6; i++)
	{
		if(type == 0)
		{
			objs.at(i).objMesh.calcUVs(entity, Mesh::UVType::CYLINDRICAL_UV);

		}
		else if(type == 1)
		{
			objs.at(i).objMesh.calcUVs(entity, Mesh::UVType::SPHERICAL_UV);
		}
		else if(type == 2)
		{
			objs.at(i).objMesh.calcUVs(entity, Mesh::UVType::CUBE_MAPPED_UV);
		}
	}

	sphere.objMesh = objs.at(0).objMesh;
	sphere_4.objMesh = objs.at(1).objMesh;
	bunny.objMesh = objs.at(2).objMesh;
	cube.objMesh = objs.at(3).objMesh;
	sphere_modified.objMesh = objs.at(4).objMesh;
	quad.objMesh = objs.at(5).objMesh;

	for( int i = 0; i < 6; i++)
	{
		bufferBind(i);
	}

	objs.clear();

	objs.push_back(sphere);
	objs.push_back(sphere_4);
	objs.push_back(bunny);
	objs.push_back(cube);
	objs.push_back(sphere_modified);
	objs.push_back(quad);
}
