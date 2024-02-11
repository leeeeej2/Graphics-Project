/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: SceneManager.cpp
Purpose: Render all objects, set buffer for some objects and update the GUI
Language: C++
Platform: VisualStudio 2019 / Window
Project: uijin.lee_CS300_2
Author: Uijin Lee, uijn.lee, 180003221
Creation date: Nov 11st, 2021
End Header --------------------------------------------------------*/
#include "SceneManager.h"
//#include "../1.0 - Simple Scene/GLApplication.cpp"

#include <iostream>
#include <glm/detail/type_mat4x4.hpp>
#include "shader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define PI 3.141f
#define CENTERCOLOR glm::vec3(0.8f, 1.f, 0.521f);
#define PLANECOLOR glm::vec3(1.f, 0.737f, 0.25f);

extern Camera cam;

void SceneManager::SetGBuffer()
{
	geometryID = LoadShaders("../Common/shaders/GeometryPass.vert", "../Common/shaders/GeometryPass.frag");
	lightID = LoadShaders("../Common/shaders/LightPass_phong.vert", "../Common/shaders/LightPass_phong.frag");

	glGenFramebuffers(1, &GBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, GBuffer);

	GLuint rbo;
	glGenTextures(1, &G_pos);
	glBindTexture(GL_TEXTURE_2D, G_pos);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1024.f, 768.f, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, G_pos, 0);

	glGenTextures(1, &G_norm);
	glBindTexture(GL_TEXTURE_2D, G_norm);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, 1024.f, 768.f, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, G_norm, 0);

	glGenTextures(1, &G_albedo);
	glBindTexture(GL_TEXTURE_2D, G_albedo);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1024.f, 768.f, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, G_albedo, 0);

	GLuint attaches[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

	glDrawBuffers(3, attaches);

	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024.f, 768.f);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	//if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		//std::cout << "Framebuffer not complete." << std::endl;

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glUseProgram(lightID);

	GLint gPos = glGetUniformLocation(lightID, "gPos");
	glUniform1i(gPos, 0);
	GLint gNormal = glGetUniformLocation(lightID, "gNormal");
	glUniform1i(gNormal, 1);
	GLint gAlbedoSpec = glGetUniformLocation(lightID, "gAlbedoSpec");
	glUniform1i(gAlbedoSpec, 2);

	diffuseID = phongMaterial("../Common/textures/metal_roof_diff_512x512.png");
	specularID = phongMaterial("../Common/textures/metal_roof_spec_512x512.png");

	//changeShader("lighting pass");

}

void SceneManager::SetOrbit()
{
	for (int i = 0; i < 720; i++)
	{
		orbit.vertexIndices.emplace_back(i);
		float x = 1.5f * sin(i * PI / 180);
		float z = 1.5f * cos(i * PI / 180);
		orbit.vertexBuffer.emplace_back(x, 0.0f, z);
		x = 1.5f * sin((i + 1) * PI / 180);
		z = 1.5f * cos((i + 1) * PI / 180);
		orbit.vertexBuffer.emplace_back(x, 0.0f, z);
		x = 1.5f * sin((i + 2) * PI / 180);
		z = 1.5f * cos((i + 2) * PI / 180);
		orbit.vertexBuffer.emplace_back(x, 0.0f, z);

		if (i + 1 < 360)
		{
			orbit.vertexIndices.emplace_back(i + 1);
		}
	}

	GLuint vbo;
	GLuint ebo;
	GLuint norm;

	glGenVertexArrays(1, &orbitVao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &norm);
	glGenBuffers(1, &ebo);

	glBindVertexArray(orbitVao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(orbit.getVertexBufferSize()) * 3 * sizeof(GLfloat),
		orbit.getVertexBuffer(), GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, norm);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(orbit.getVertexBufferSize()) * 3 * sizeof(GLfloat),
		orbit.getVertexNormals(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, orbit.getIndexBufferSize() * sizeof(GLuint),
		orbit.getIndexBuffer(), GL_STATIC_DRAW);

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

	glBindVertexArray(0);

	orbitColor = glm::vec3(1.f, 1.f, 1.f);
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
SceneManager::~SceneManager()
{
	CleanUp();
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

SceneManager::SceneManager(int windowWidth_, int windowHeight_) : Scene(windowWidth_, windowHeight_), OBJLoaderManager(new(OBJLoader)),
centerID(0), centerColor(glm::vec3(0.f)), centerMat(glm::mat4(1.f)), centerVao(0), angleOfRotation(0.39f), angleOfRotation2(0.f),
afterInit(false), orbitVao(0), orbitColor(glm::vec3(0.f)), orbitMat(glm::mat4(1.0f)),
SphereVao(0), SphereNorm(0), SphereColor(glm::vec3(0.f)), SphereMat(glm::mat4(1.0f)),
staticMat(glm::mat4(1.0f)), NormalVao(0), NormalfaceVao(0), NormalID(0), FinishVNormal(false), vertexCheck(false), faceCheck(false)
{

	centerOBJ = Mesh();
	planeOBJ = Mesh();
	orbit = Mesh();
	procedureSphere = Mesh();
	current_item = nullptr;
	type = 0;
	depth = true;
	//scene;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
void SceneManager::CleanUp()
{
	glDeleteProgram(centerID);
	glDeleteBuffers(1, &centerVao);
	glDeleteBuffers(1, &orbitVao);
	glDeleteBuffers(1, &SphereVao);
	glDeleteBuffers(1, &SphereNorm);
	glDeleteBuffers(1, &NormalVao);
	glDeleteBuffers(1, &NormalfaceVao);
	glDeleteProgram(NormalID);

	OBJLoaderManager->cleanUp();
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
int SceneManager::Init()
{
	once = false;
	InitLightData();

	Emissive = glm::vec3(0.f, 0.f, 0.f);
	globalAmbient = glm::vec3(0.01f, 0.01f, 0.01f);
	afterInit = true;
	OBJLoaderManager->LoadObj("../Common/shaders/LightPass_phong.vert", "../Common/shaders/LightPass_phong.frag");
	GPUCheck = false;
	currentShader = 1;
	currentUV = 0;
	currentEntity = false;
	ka = glm::vec3(1.0f, 0.f, 0.f);
	fogColor = glm::vec3(0.49, 0.49, 0.49);
	fogMax = 30;
	fogMin = 2;

	glm::vec3 origin(0, 0, 0);

	SetOrbit();

	procedureSphere = *createSphere(0.05f, 50);
	sphere_setBuffer();

	centerOBJ = OBJLoaderManager->GetObj(type::bunny).objMesh;
	centerID = OBJLoaderManager->GetObj(type::bunny).program;
	centerVao = OBJLoaderManager->GetObj(type::bunny).VAO;
	Centerubo = OBJLoaderManager->GetObj(type::bunny).uniformBlock;

	change = type::bunny;

	UVVisible = false;
	IsGPU = 4; // planar, cylinder, spherical, cube // 4 cpu
	Entity = false; // false: position true: normal
	min = centerOBJ.boundingBox[0];
	max = centerOBJ.boundingBox[1];

	planeOBJ = OBJLoaderManager->GetObj(type::quad).objMesh;
	planeID = OBJLoaderManager->GetObj(type::quad).program;
	planeVao = OBJLoaderManager->GetObj(type::quad).VAO;
	Planeubo = OBJLoaderManager->GetObj(type::quad).uniformBlock;

	planeMat = glm::translate(glm::vec3(0.f, -0.75f, 0.f)) * glm::rotate((GLfloat)1.57, glm::vec3(1.f, 0.f, 0.f)) * glm::scale(glm::vec3(6.f, 6.f, 6.f));

	DisplayNormalBuffer();
	FinishVNormal = true;
	DisplayNormalBuffer();
	FinishVNormal = false;

	glm::vec3 orbitcenter = orbit.getModelCentroid();
	glm::vec3 sphereCentroid = procedureSphere.getModelCentroid();

	centerColor = CENTERCOLOR;
	planeColor = PLANECOLOR;

	staticMat = glm::translate(glm::vec3(1.5f, 0.f, 0.f)) * glm::translate(sphereCentroid);//glm::translate(glm::vec3(1.5f, 0.0f, 0.0f)) * glm::scale(glm::vec3(0.1f, 0.1f, 0.1f)) * glm::translate(sphereCentroid);

	//ID_grid = phongMaterial("../Common/textures/grid.png");

	//diffuseID = phongMaterial("../Common/textures/metal_roof_diff_512x512.png");
	//specularID = phongMaterial("../Common/textures/metal_roof_spec_512x512.png");

	centerMat = glm::scale(glm::vec3(1.5f, 1.5f, 1.5f));
	//glUseProgram(centerID);

	/*GLint grid_m = glGetUniformLocation(centerID, "material.grid");
	if (grid_m >= 0)
		glUniform1i(grid_m, 0);
	else
		std::cout << "error" << std::endl;

	GLint diffusePos_m = glGetUniformLocation(centerID, "diffuse");
	if (diffusePos_m >= 0)
		glUniform1i(diffusePos_m, 0);
	else
		std::cout << "error" << std::endl;

	GLint specularPos_m = glGetUniformLocation(centerID, "specular");
	if (specularPos_m >= 0)
		glUniform1i(specularPos_m, 1);
	else
		std::cout << "error" << std::endl;*/

	SetGBuffer();
	glEnable(GL_DEPTH_TEST);


	return Scene::Init();
}
void SceneManager::changeLightPosition(int i, glm::mat4 matrix)
{
	light[i].x = matrix[3].x;
	light[i].y = matrix[3].y;
	light[i].z = matrix[3].z;

	lightDir[i] = -light[i];
}
//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
int SceneManager::Render()
{


	SphereColor = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec4 lightPosition = glm::rotate(angleOfRotation2, glm::vec3(0.f, 1.f, 0.f)) * glm::vec4(1.5f, 0.f, 0.f, 1.0f);

	angleOfRotation = 360.f / LightNum;

	SphereMat = glm::rotate(angleOfRotation2, glm::vec3(0.f, 1.f, 0.f)) * staticMat;
	//SphereMat = glm::rotate(angleOfRotation2 + angleOfRotation, glm::vec3(0.f, 1.f, 0.f)) * staticMat;
	SphereMat2 = glm::rotate(angleOfRotation2, glm::vec3(0.f, 1.f, 0.f)) * glm::rotate(angleOfRotation, glm::vec3(0.f, 1.f, 0.f)) * staticMat;
	SphereMat3 = glm::rotate(angleOfRotation2, glm::vec3(0.f, 1.f, 0.f)) * glm::rotate(angleOfRotation * 2, glm::vec3(0.f, 1.f, 0.f)) * staticMat;
	SphereMat4 = glm::rotate(angleOfRotation2, glm::vec3(0.f, 1.f, 0.f)) * glm::rotate(angleOfRotation * 3, glm::vec3(0.f, 1.f, 0.f)) * staticMat;
	SphereMat5 = glm::rotate(angleOfRotation2, glm::vec3(0.f, 1.f, 0.f)) * glm::rotate(angleOfRotation * 4, glm::vec3(0.f, 1.f, 0.f)) * staticMat;
	SphereMat6 = glm::rotate(angleOfRotation2, glm::vec3(0.f, 1.f, 0.f)) * glm::rotate(angleOfRotation * 5, glm::vec3(0.f, 1.f, 0.f)) * staticMat;
	SphereMat7 = glm::rotate(angleOfRotation2, glm::vec3(0.f, 1.f, 0.f)) * glm::rotate(angleOfRotation * 6, glm::vec3(0.f, 1.f, 0.f)) * staticMat;
	SphereMat8 = glm::rotate(angleOfRotation2, glm::vec3(0.f, 1.f, 0.f)) * glm::rotate(angleOfRotation * 7, glm::vec3(0.f, 1.f, 0.f)) * staticMat;
	SphereMat9 = glm::rotate(angleOfRotation2, glm::vec3(0.f, 1.f, 0.f)) * glm::rotate(angleOfRotation * 8, glm::vec3(0.f, 1.f, 0.f)) * staticMat;
	SphereMat10 = glm::rotate(angleOfRotation2, glm::vec3(0.f, 1.f, 0.f)) * glm::rotate(angleOfRotation * 9, glm::vec3(0.f, 1.f, 0.f)) * staticMat;
	SphereMat11 = glm::rotate(angleOfRotation2, glm::vec3(0.f, 1.f, 0.f)) * glm::rotate(angleOfRotation * 10, glm::vec3(0.f, 1.f, 0.f)) * staticMat;
	SphereMat12 = glm::rotate(angleOfRotation2, glm::vec3(0.f, 1.f, 0.f)) * glm::rotate(angleOfRotation * 11, glm::vec3(0.f, 1.f, 0.f)) * staticMat;
	SphereMat13 = glm::rotate(angleOfRotation2, glm::vec3(0.f, 1.f, 0.f)) * glm::rotate(angleOfRotation * 12, glm::vec3(0.f, 1.f, 0.f)) * staticMat;
	SphereMat14 = glm::rotate(angleOfRotation2, glm::vec3(0.f, 1.f, 0.f)) * glm::rotate(angleOfRotation * 13, glm::vec3(0.f, 1.f, 0.f)) * staticMat;
	SphereMat15 = glm::rotate(angleOfRotation2, glm::vec3(0.f, 1.f, 0.f)) * glm::rotate(angleOfRotation * 14, glm::vec3(0.f, 1.f, 0.f)) * staticMat;
	SphereMat16 = glm::rotate(angleOfRotation2, glm::vec3(0.f, 1.f, 0.f)) * glm::rotate(angleOfRotation * 15, glm::vec3(0.f, 1.f, 0.f)) * staticMat;

	std::vector<glm::mat4> mats;
	mats.push_back(SphereMat);
	mats.push_back(SphereMat2);
	mats.push_back(SphereMat3);
	mats.push_back(SphereMat4);
	mats.push_back(SphereMat5);
	mats.push_back(SphereMat6);
	mats.push_back(SphereMat7);
	mats.push_back(SphereMat8);
	mats.push_back(SphereMat9);
	mats.push_back(SphereMat10);
	mats.push_back(SphereMat11);
	mats.push_back(SphereMat12);
	mats.push_back(SphereMat13);
	mats.push_back(SphereMat14);
	mats.push_back(SphereMat15);
	mats.push_back(SphereMat16);

	for (int i = 0; i < LightNum; i++)
	{
		changeLightPosition(i, mats.at(i));
	}

	glClearColor(fogColor.x, fogColor.y, fogColor.z, 1.0f);
	//glClearDepth(1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);



	glBindFramebuffer(GL_FRAMEBUFFER, GBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 world2camera = cam.getLookAt();
	glm::mat4 camera2ndc = glm::perspective(glm::radians(cam.zoom), 1024.f / 768.f/*(float)height / (float)width*/, 0.1f, 100.f);
	glm::mat4 model2world = centerMat;

	glUseProgram(geometryID);

	if (once == false)
	{
		GLint diffusePos_m = glGetUniformLocation(geometryID, "diffuse");
		if (diffusePos_m >= 0)
			glUniform1i(diffusePos_m, 0);
		else
			std::cout << "diffuse error" << std::endl;

		GLint specularPos_m = glGetUniformLocation(geometryID, "specular");
		if (specularPos_m >= 0)
			glUniform1i(specularPos_m, 1);
		else
			std::cout << "specular error" << std::endl;

		once = true;
	}

	GLint vertexmodelLoc = glGetUniformLocation(geometryID, "model");
	glUniformMatrix4fv(vertexmodelLoc, 1, GL_FALSE, &model2world[0][0]);
	GLint vertexcameraLoc = glGetUniformLocation(geometryID, "camera");
	glUniformMatrix4fv(vertexcameraLoc, 1, GL_FALSE, &world2camera[0][0]);
	GLint vertexprojectionLoc = glGetUniformLocation(geometryID, "projection");
	glUniformMatrix4fv(vertexprojectionLoc, 1, GL_FALSE, &camera2ndc[0][0]);

	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseID);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularID);

		glBindVertexArray(centerVao);
		glDrawElements(GL_TRIANGLES, centerOBJ.getIndexBufferSize(), GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
	}

	{
		model2world = planeMat;
		glUniformMatrix4fv(vertexmodelLoc, 1, GL_FALSE, &model2world[0][0]);
		glUniformMatrix4fv(vertexcameraLoc, 1, GL_FALSE, &world2camera[0][0]);
		glUniformMatrix4fv(vertexprojectionLoc, 1, GL_FALSE, &camera2ndc[0][0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseID);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularID);

		glBindVertexArray(planeVao);
		glDrawElements(GL_TRIANGLES, planeOBJ.getIndexBufferSize(), GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);

	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(lightID);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, G_pos);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, G_norm);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, G_albedo);

	{
		for (int i = 0; i < LightNum; i++)
		{
			allocateUniformBlock(i, i * 112);
		}

		GLint FogPos = glGetUniformLocation(lightID, "fogcolor");
		glUniform3f(FogPos, fogColor.x, fogColor.y, fogColor.z);
		GLint FogminPos = glGetUniformLocation(lightID, "fogMin");
		glUniform1f(FogminPos, fogMin);
		GLint FogmaxPos = glGetUniformLocation(lightID, "fogMax");
		glUniform1f(FogmaxPos, fogMax);

		GLint shininess = glGetUniformLocation(lightID, "shininess");
		glUniform1f(shininess, 64.0f);

		GLint lightNumPos = glGetUniformLocation(lightID, "activeLight");
		glUniform1i(lightNumPos, LightNum);

		GLint min_pos = glGetUniformLocation(lightID, "min_");
		glUniform3f(min_pos, min.x, min.y, min.z);

		GLint max_pos = glGetUniformLocation(lightID, "max_");
		glUniform3f(max_pos, max.x, max.y, max.z);

		GLint EmissivePos = glGetUniformLocation(lightID, "I_emmisive");
		glUniform3f(EmissivePos, Emissive.x, Emissive.y, Emissive.z);

		GLint globalAmbientPos = glGetUniformLocation(lightID, "globalAmbient");

		if (globalAmbientPos >= 0)
			glUniform3f(globalAmbientPos, globalAmbient.x, globalAmbient.y, globalAmbient.z);

		else
			std::cout << "global ambient error" << std::endl;

		GLint KaPos = glGetUniformLocation(lightID, "Ka");
		glUniform3f(KaPos, ka.x, ka.y, ka.z);

		GLint typePos = glGetUniformLocation(lightID, "type");
		glUniform1i(typePos, type);
	}


	GLint viewLoc = glGetUniformLocation(lightID, "viewPos");
	glUniform3f(viewLoc, cam.eye.x, cam.eye.y, cam.eye.z);

	if (FSQCheck == false)
	{
		FSQCheck = true;
		float quads[] = {
			-1.0f,  1.0f, 0.0f,		0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f,		0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f,		1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f,		1.0f, 0.0f,
		};

		GLuint quadVbo;

		glGenVertexArrays(1, &quadVao);

		glGenBuffers(1, &quadVbo);
		glBindVertexArray(quadVao);
		glBindBuffer(GL_ARRAY_BUFFER, quadVbo);

		glBufferData(GL_ARRAY_BUFFER, sizeof(quads), &quads, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(quadVao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	/*glBindVertexArray(quadVao);
	glDrawElements(GL_TRIANGLES, quadOBJ.getIndexBufferSize(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);*/

	glBindFramebuffer(GL_READ_FRAMEBUFFER, GBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);

	glBlitFramebuffer(0, 0, 1024.f, 768.f, 0, 0, 1024.f, 768.f, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	if (depth == true)
	{
		if (vertexCheck == true)
		{
			drawNormal(NormalID, centerMat, glm::vec3(0.f, 0.f, 1.f), NormalVao, centerOBJ, false, true);//vertexnormal
		}
		if (faceCheck == true)
		{
			drawNormal(NormalID, centerMat, glm::vec3(1.f, 0.f, 0.f), NormalfaceVao, centerOBJ, false, true);//facenormal
		}
	}

	UpadateGUI();

	return 0;
}

void SceneManager::InitLightData()
{
	constant = 1.0f;
	linear = 0.09f;
	quadratic = 0.032f;

	cols.push_back(glm::vec3(0.230f, 0.917f, 0.081f));
	cols.push_back(glm::vec3(0.086f, 0.350f, 0.971f));
	cols.push_back(glm::vec3(0.887f, 0.409f, 0.113f));
	cols.push_back(glm::vec3(0.795f, 0.113f, 0.887f));
	cols.push_back(glm::vec3(0.887f, 0.842f, 0.113f));
	cols.push_back(glm::vec3(0.113f, 0.887f, 0.705f));
	cols.push_back(glm::vec3(0.795f, 0.113f, 0.887f));
	cols.push_back(glm::vec3(0.795f, 0.113f, 0.887f));
	cols.push_back(glm::vec3(0.113f, 0.887f, 0.705f));
	cols.push_back(glm::vec3(0.887f, 0.113f, 0.864f));
	cols.push_back(glm::vec3(0.956f, 0.056f, 0.056f));
	cols.push_back(glm::vec3(0.230f, 0.917f, 0.081f));
	cols.push_back(glm::vec3(0.086f, 0.350f, 0.971f));
	cols.push_back(glm::vec3(0.887f, 0.409f, 0.113f));
	cols.push_back(glm::vec3(0.795f, 0.113f, 0.887f));
	cols.push_back(glm::vec3(0.887f, 0.842f, 0.113f));

	for (int i = 0; i < LightNum; i++)
	{
		lightType[i] = 1;
		theta[i] = 10.f;
		phi[i] = 20.f;
		fallout[i] = 1.f;
		light[i] = glm::vec3(0.f, 0.f, 0.f);
		lightDir[i] = -light[i];
		L_ambient[i] = glm::vec3(0.1f, 0.1f, 0.1f); //
		L_diffuse[i] = cols[i]; //
		L_specular[i] = glm::vec3(1.0f, 1.0f, 1.0f); // have to send it to the shader
	}

}

GLuint  SceneManager::phongMaterial(const char* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format = 0;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}


void SceneManager::allocateUniformBlock(int numLight, int byte)
{
	glBindBuffer(GL_UNIFORM_BUFFER, Centerubo);
	glBufferSubData(GL_UNIFORM_BUFFER, byte, sizeof(float), &constant);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, Centerubo);
	glBufferSubData(GL_UNIFORM_BUFFER, static_cast<long long>(byte) + 4, sizeof(float), &linear);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, Centerubo);
	glBufferSubData(GL_UNIFORM_BUFFER, static_cast<long long>(byte) + 8, sizeof(float), &quadratic);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, Centerubo);
	glBufferSubData(GL_UNIFORM_BUFFER, static_cast<long long>(byte) + 12, sizeof(float), &theta[numLight]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, Centerubo);
	glBufferSubData(GL_UNIFORM_BUFFER, static_cast<long long>(byte) + 16, sizeof(float), &phi[numLight]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, Centerubo);
	glBufferSubData(GL_UNIFORM_BUFFER, static_cast<long long>(byte) + 20, sizeof(float), &fallout[numLight]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, Centerubo);
	glBufferSubData(GL_UNIFORM_BUFFER, static_cast<long long>(byte) + 24, sizeof(float), &lightType[numLight]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	glBindBuffer(GL_UNIFORM_BUFFER, Centerubo);
	glBufferSubData(GL_UNIFORM_BUFFER, static_cast<long long>(byte) + 32, sizeof(glm::vec3), &lightDir[numLight]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0); //DIRECTION

	glBindBuffer(GL_UNIFORM_BUFFER, Centerubo);
	glBufferSubData(GL_UNIFORM_BUFFER, static_cast<long long>(byte) + 48, sizeof(glm::vec3), &light[numLight]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);//POSITION

	glBindBuffer(GL_UNIFORM_BUFFER, Centerubo);
	glBufferSubData(GL_UNIFORM_BUFFER, static_cast<long long>(byte) + 64, sizeof(glm::vec3), &L_ambient[numLight]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	if (Scenario1 == true)
	{
		glBindBuffer(GL_UNIFORM_BUFFER, Centerubo);
		glBufferSubData(GL_UNIFORM_BUFFER, static_cast<long long>(byte) + 80, sizeof(glm::vec3), &L_diffuse[1]);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	else if (Scenario2 == true)
	{
		//Scenario1 = false;
		glBindBuffer(GL_UNIFORM_BUFFER, Centerubo);
		glBufferSubData(GL_UNIFORM_BUFFER, static_cast<long long>(byte) + 80, sizeof(glm::vec3), &L_diffuse[numLight]);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}
	else
	{
		glBindBuffer(GL_UNIFORM_BUFFER, Centerubo);
		glBufferSubData(GL_UNIFORM_BUFFER, static_cast<long long>(byte) + 80, sizeof(glm::vec3), &L_diffuse[numLight]);
		glBindBuffer(GL_UNIFORM_BUFFER, 0);
	}


	glBindBuffer(GL_UNIFORM_BUFFER, Centerubo);
	glBufferSubData(GL_UNIFORM_BUFFER, static_cast<long long>(byte) + 96, sizeof(glm::vec3), &L_specular[numLight]);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void SceneManager::drawNormal(GLuint programID, glm::mat4 model, glm::vec3 color, GLuint vao, Mesh pMesh, bool IsOrbit, bool IsNormal)
{
	glUseProgram(programID);

	GLint vertexColorLoc = glGetUniformLocation(programID, "ourColor");
	glm::mat4 world2camera = cam.getLookAt();
	glm::mat4 camera2ndc = glm::perspective(glm::radians(cam.zoom), 1024.f / 768.f/*(float)height / (float)width*/, 0.1f, 100.f);
	glm::mat4 model2world = model;

	GLint vertexcameraLoc = glGetUniformLocation(programID, "camera");
	GLint vertexprojectionLoc = glGetUniformLocation(programID, "projection");
	GLint vertexmodelLoc = glGetUniformLocation(programID, "model");

	glUniform3f(vertexColorLoc, color.x, color.y, color.z);
	glUniformMatrix4fv(vertexcameraLoc, 1, GL_FALSE, &world2camera[0][0]);
	glUniformMatrix4fv(vertexprojectionLoc, 1, GL_FALSE, &camera2ndc[0][0]);
	glUniformMatrix4fv(vertexmodelLoc, 1, GL_FALSE, &model2world[0][0]);

	glBindVertexArray(vao);

	glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(pMesh.vertexNormalDisplay.size() * 2));
	//glDrawArrays(GL_LINES, 0, pMesh.faceNormalsDisplay.size() * 2);

	glBindVertexArray(0);
}

Mesh* SceneManager::createSphere(float radius, int numDivisions)
{
	Mesh* sphereMesh = new(Mesh);

	float stepsizeTheta = 360.f / numDivisions; //sector
	float stepsizePhi = 180.f / numDivisions;	//stack

	glm::vec3 vertices(0.1f, 0.1f, 0.1f);
	glm::vec3 nvertices(0.1f, 0.1f, 0.1f);

	float length = 1.0f / radius;

	float theta;
	float phi;

	for (float i = 0; i <= numDivisions; i++)
	{
		phi = (180.f / 2) - (i * stepsizePhi);
		for (float j = 0; j <= 180.f; j++)
		{
			theta = j * stepsizeTheta;
			vertices.x = radius * cos(phi) * cos(theta);
			vertices.y = radius * cos(phi) * sin(theta);
			vertices.z = radius * sin(phi);

			sphereMesh->vertexBuffer.emplace_back(vertices);

			nvertices.x = vertices.x * length;
			nvertices.y = vertices.y * length;
			nvertices.z = vertices.z * length;

			sphereMesh->vertexNormals.emplace_back(nvertices);
		}
	}

	int point1;
	int point2;

	for (int i = 0; i < 360; i++)
	{
		point1 = i * (numDivisions + 1);
		point2 = point1 + numDivisions + 1;

		for (int j = 0; j < numDivisions; j++, point1++, point2++)
		{
			if (i != 0)
			{
				sphereMesh->vertexIndices.emplace_back(point1);
				sphereMesh->vertexIndices.emplace_back(point2);
				sphereMesh->vertexIndices.emplace_back(point1 + 1);
			}
			if (i != (numDivisions - 1))
			{
				sphereMesh->vertexIndices.emplace_back(point1 + 1);
				sphereMesh->vertexIndices.emplace_back(point2);
				sphereMesh->vertexIndices.emplace_back(point2 + 1);
			}
		}
	}

	sphereID = LoadShaders("../Common/shaders/newVertexShader.vert", "../Common/shaders/newFragmentShader.frag");
	return sphereMesh;
}

void SceneManager::sphere_setBuffer()
{
	GLuint vbo;
	GLuint ebo;
	GLuint SphereNorm;

	glGenVertexArrays(1, &SphereVao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &SphereNorm);
	glGenBuffers(1, &ebo);

	glBindVertexArray(SphereVao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(procedureSphere.getVertexBufferSize()) * 3 * sizeof(GLfloat),
		procedureSphere.getVertexBuffer(), GL_STATIC_DRAW);//

	glBindBuffer(GL_ARRAY_BUFFER, SphereNorm);
	glBufferData(GL_ARRAY_BUFFER, static_cast<GLsizeiptr>(procedureSphere.getVertexBufferSize()) * 3 * sizeof(GLfloat),
		procedureSphere.getVertexNormals(), GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, procedureSphere.getIndexBufferSize() * sizeof(GLuint),
		procedureSphere.getIndexBuffer(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0,
		3,
		GL_FLOAT,
		GL_FALSE,
		3 * sizeof(GLfloat),
		(void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, SphereNorm);
	glVertexAttribPointer(1,
		3,
		GL_FLOAT,
		GL_FALSE,
		3 * sizeof(GLfloat),
		(void*)0);

	glBindVertexArray(0);
}

void SceneManager::DisplayNormalBuffer()
{
	NormalID = LoadShaders("../Common/shaders/displayNormal.vert",
		"../Common/shaders/displayNormal.frag");

	GLuint vbo = 0;
	GLuint ebo = 0;

	if (FinishVNormal == false)
	{
		glGenVertexArrays(1, &NormalVao);

		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindVertexArray(NormalVao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, centerOBJ.vertexNormalDisplay.size() * 3 * sizeof(GLfloat),
			centerOBJ.vertexNormalDisplay.data(), GL_STATIC_DRAW);
	}
	else if (FinishVNormal == true)
	{
		glGenVertexArrays(1, &NormalfaceVao);

		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		glBindVertexArray(NormalfaceVao);

		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		glBufferData(GL_ARRAY_BUFFER, centerOBJ.faceNormalsDisplay.size() * 3 * sizeof(GLfloat),
			centerOBJ.faceNormalsDisplay.data(), GL_STATIC_DRAW);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, centerOBJ.vertexIndices.size() * sizeof(GLfloat),
		centerOBJ.vertexIndices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glVertexAttribPointer(0,
		3,
		GL_FLOAT,
		GL_FALSE,
		3 * sizeof(GLfloat),
		(void*)0);

	glBindVertexArray(0);
}

void SceneManager::UpadateGUI()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::Begin("Control");

	const char* mesh[] = { "sphere", "sphere_4", "bunny", "cube", "sphere_modified" };
	static int item_current = 2;
	ImGui::Combo("Object Mesh", &item_current, mesh, IM_ARRAYSIZE(mesh));
	if (change != item_current)
	{
		changeObj(mesh[item_current]);
	}

	static float vec4a[4] = { 1.0f, 0.09f, 0.032f, 0.44f };
	ImGui::DragFloat3("constant / linear / quadratic", vec4a, 0.001f, 0.0f, 2.0f);

	constant = vec4a[0];
	linear = vec4a[1];
	quadratic = vec4a[2];
	ImGui::ColorEdit3("Global Ambient", reinterpret_cast<float*>(&globalAmbient));
	ImGui::ColorEdit3("Ka", reinterpret_cast<float*>(&ka));
	ImGui::ColorEdit3("Emissive", reinterpret_cast<float*>(&Emissive));

	ImGui::ColorEdit3("FogColor", reinterpret_cast<float*>(&fogColor));
	ImGui::DragFloat("FogMax", &fogMax, 0.1f, 5, 30.0f);

	ImGui::SliderInt("Number of lights", &LightNum, 1, 16);


	const char* Type1[] = { "directional light", "position light", "spotlight" };
	static int light_current1 = 1;
	ImGui::Combo("LightType", &light_current1, Type1, IM_ARRAYSIZE(Type1));
	if (lightType[0] != light_current1)
	{
		for (int i = 0; i < LightNum; i++)
		{
			lightType[i] = light_current1;
		}
	}


	const char* uv[] = { "CYLINDRICAL_UV", "SPHERICAL_UV" , "CUBE_MAPPED_UV" };
	static int uv_current = 0;
	ImGui::Combo("uv type", &uv_current, uv, IM_ARRAYSIZE(uv));
	if (currentUV != uv_current)
	{
		//gpu cpu 
		currentUV = uv_current;
		changeUV(uv_current);
		//changeObj(mesh[item_current]);
	}

	ImGui::Text("Deferred: centerOBJ & plane\nForward: normal Vector");
	ImGui::Text("\n");
	ImGui::Text("On: hybrid rendering (obj + plane + normal)\nOff: Deferred rendering(obj + plane)");

	ImGui::Checkbox("Depth copying", &depth);

	ImGui::Checkbox("Display VertexNormal", &vertexCheck);
	ImGui::Checkbox("Display FaceNormal", &faceCheck);
	ImGui::Text("\n");

	if (ImGui::Button("position"))
	{
		type = 1;
	}

	if (ImGui::Button("normal"))
	{
		type = 2;
	}

	if (ImGui::Button("albedo"))
	{
		type = 3;
	}

	if (ImGui::Button("specular"))
	{
		type = 4;
	}

	if (ImGui::Button("Final Result"))
	{
		type = 0;
	}

	ImGui::End();
}

void SceneManager::changeUV(int uvCurrent)
{
	OBJLoaderManager->reloadUV(Entity, currentUV);
	centerOBJ = OBJLoaderManager->GetObj(change).objMesh;
	centerVao = OBJLoaderManager->GetObj(change).VAO;

	DisplayNormalBuffer();
	FinishVNormal = true;
	DisplayNormalBuffer();
	FinishVNormal = false;
}
void SceneManager::changeObj(std::string obj_)
{
	if (obj_ == "sphere")
	{
		change = type::sphere;
	}
	else if (obj_ == "sphere_4")
	{
		change = type::sphere_4;
	}
	else if (obj_ == "bunny")
	{
		change = type::bunny;
	}
	else if (obj_ == "cube")
	{
		change = type::cube;

	}
	else if (obj_ == "sphere_modified")
	{
		change = type::sphere_modified;

	}

	centerOBJ = OBJLoaderManager->GetObj(change).objMesh;
	centerID = OBJLoaderManager->GetObj(change).program;
	centerVao = OBJLoaderManager->GetObj(change).VAO;

	min = centerOBJ.boundingBox[0];
	max = centerOBJ.boundingBox[1];

	DisplayNormalBuffer();
	FinishVNormal = true;
	DisplayNormalBuffer();
	FinishVNormal = false;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
int SceneManager::postRender()
{
	angleOfRotation2 += 0.01f;
	return 0;
}
