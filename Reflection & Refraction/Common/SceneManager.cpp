/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: SceneManager.cpp
Purpose: Render all objects, set buffer for some objects and update the GUI
Language: C++
Platform: VisualStudio 2019 / Window
Project: uijin.lee_CS300_3
Author: Uijin Lee, uijn.lee, 180003221
Creation date: Nov 11st, 2021
End Header --------------------------------------------------------*/
#include "SceneManager.h"
#include <iostream>
#include <glm/detail/type_mat4x4.hpp>
#include "shader.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define PI 3.141f
#define CENTERCOLOR glm::vec3(0.8f, 1.f, 0.521f);
#define PLANECOLOR glm::vec3(1.f, 0.737f, 0.25f);

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

SceneManager::SceneManager(int windowWidth, int windowHeight) : Scene(windowWidth, windowHeight), OBJLoaderManager(new(OBJLoader)),
centerID(0), centerColor(glm::vec3(0.f)), centerMat(glm::mat4(1.f)), centerVao(0), angleOfRotation(0.39f), angleOfRotation2(0.f),
afterInit(false), orbitVao(0), orbitColor(glm::vec3(0.f)), orbitMat(glm::mat4(1.0f)),
SphereVao(0), SphereNorm(0), SphereColor(glm::vec3(0.f)), SphereMat(glm::mat4(1.0f)),
staticMat(glm::mat4(1.0f)), NormalVao(0), NormalfaceVao(0), NormalID(0), FinishVNormal(false), vertexCheck(false), faceCheck(false)
{
	centerOBJ = Mesh();
	planeOBJ = Mesh();
	skyboxMesh = Mesh();
	orbit = Mesh();
	procedureSphere = Mesh();
	current_item = nullptr;
	eye = glm::vec3(0.3f, 0.f, 4.2f);
	Reflection = true;
	Refraction = false;
}

void SceneManager::inputProcess(GLFWwindow* window)
{

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
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
	cam = Camera(eye, glm::vec3(0.0f, 0.0f, -1.0f),  glm::vec3(0, 1, 0), 0.f, 0.f); //center object camera

	camLeft = Camera(glm::vec3(0, 0, 0), glm::vec3(-1, 0, 0), glm::vec3(0, 1, 0), 0.f, 0.f);
	camRight = Camera(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), glm::vec3(0, 1, 0), 0.f, 0.f);
	camBottom = Camera(glm::vec3(0, 0, 0), glm::vec3(0, -1, 0), glm::vec3(0, 0, -1), 0.f, 0.f);
	camTop = Camera(glm::vec3(0, 0, 0), glm::vec3(0, 1, 0), glm::vec3(0, 0, 1), 0.f, 0.f);
	camBack = Camera(glm::vec3(0, 0, 0), glm::vec3(0, 0, -1), glm::vec3(0, 1, 0), 0.f, 0.f);
	camFront = Camera(glm::vec3(0, 0, 0), glm::vec3(0, 0, 1), glm::vec3(0, 1, 0), 0.f, 0.f);

	currentMaterial = 3;
	MaterialRatio = 1.47f;

	FBOCam[0] = camLeft;
	FBOCam[1] = camRight;
	FBOCam[2] = camBottom;
	FBOCam[3] = camTop;
	FBOCam[4] = camBack;
	FBOCam[5] = camFront;

	width = 1024;
	height = 768;

	GenerateSkyBoxCube();
	InitLightData();
	Emissive = glm::vec3(0.f, 0.f, 0.f);
	globalAmbient = glm::vec3(0.01f, 0.01f, 0.01f);
	afterInit = true;
	OBJLoaderManager->LoadObj("../Common/shaders/phongShading.vert", "../Common/shaders/phongShading.frag");
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

	centerOBJ = OBJLoaderManager->GetObject(type::bunny).objMesh;
	centerID = OBJLoaderManager->GetObject(type::bunny).program;
	centerVao = OBJLoaderManager->GetObject(type::bunny).VAO;
	Centerubo = OBJLoaderManager->GetObject(type::bunny).uniformBlock;

	change = type::bunny;

	UVVisible = false;
	IsGPU = 4; // planar, cylinder, spherical, cube // 4 cpu
	Entity = false; // false: position true: normal
	min = centerOBJ.boundingBox[0];
	max = centerOBJ.boundingBox[1];

	DisplayNormalBuffer();
	FinishVNormal = true;
	DisplayNormalBuffer();
	FinishVNormal = false;

	glm::vec3 orbitcenter = orbit.getModelCentroid();
	glm::vec3 sphereCentroid = procedureSphere.getModelCentroid();

	centerColor = CENTERCOLOR;

	staticMat = glm::translate(glm::vec3(1.5f, 0.f, 0.f)) * glm::translate(sphereCentroid);//glm::translate(glm::vec3(1.5f, 0.0f, 0.0f)) * glm::scale(glm::vec3(0.1f, 0.1f, 0.1f)) * glm::translate(sphereCentroid);

	/*ID_grid = Textureload("../Common/textures/grid.png");*/

	/*diffuseID = Textureload("../Common/textures/metal_roof_diff_512x512.png");
	specularID = Textureload("../Common/textures/metal_roof_spec_512x512.png");*/

	centerMat = glm::scale(glm::vec3(1.5f, 1.5f, 1.5f));
	glUseProgram(centerID);

	//GLint grid_m = glGetUniformLocation(centerID, "material.grid");
	//if (grid_m >= 0)
	//	glUniform1i(grid_m, 0);
	//else
	//	std::cout << "uniform variable" << "material.grid" << "not exist" << std::endl;

	/*GLint diffusePos_m = glGetUniformLocation(centerID, "material.diffuse");
	if (diffusePos_m >= 0)
		glUniform1i(diffusePos_m, 0);
	else
		std::cout << "uniform variable" << "material.diffuse" << "not exist" << std::endl;

	GLint specularPos_m = glGetUniformLocation(centerID, "material.specular");
	if (specularPos_m >= 0)
		glUniform1i(specularPos_m, 1);
	else
		std::cout << "uniform variable" << "material.specular" << "not exist" << std::endl;*/


	generateFBO();
	//GenerateSkyBoxCube();

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
	glEnable(GL_DEPTH_TEST);
	glClearColor(fogColor.x, fogColor.y, fogColor.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	UpadateGUI();

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

	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	skyboxDraw();
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);

	draw1(centerID, centerMat, centerColor, centerVao, centerOBJ);

	GLint EmissivePos = glGetUniformLocation(centerID, "I_emmisive");
	glUniform3f(EmissivePos, Emissive.x, Emissive.y, Emissive.z);

	GLint globalAmbientPos = glGetUniformLocation(centerID, "globalAmbient");

	if (globalAmbientPos >= 0)
		glUniform3f(globalAmbientPos, globalAmbient.x, globalAmbient.y, globalAmbient.z);

	else
		std::cout << "uniform variable" << "globalAmbient" << "not exist" << std::endl;

	GLint KaPos = glGetUniformLocation(centerID, "Ka");
	glUniform3f(KaPos, ka.x, ka.y, ka.z);


	if (vertexCheck == true)
	{
		draw(NormalID, centerMat, glm::vec3(0.f, 0.f, 1.f), NormalVao, centerOBJ,glm::mat4(1.0), false, true, false);//vertexnormal
	}
	if (faceCheck == true)
	{
		draw(NormalID, centerMat, glm::vec3(1.f, 0.f, 0.f), NormalfaceVao, centerOBJ, glm::mat4(1.0), false, true, false);//facenormal
	}

	for (int i = 0; i < LightNum; i++)
	{
		draw(sphereID, mats.at(i), cols[i], SphereVao, procedureSphere, glm::mat4(1.0), false, false, false);
	}

	if (Reflection || Refraction)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
		glUseProgram(centerID);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, textureBuffer[0]);
		GLint skyboxTex_samp1 = glGetUniformLocation(centerID, "ref_texture[0]");
		glUniform1i(skyboxTex_samp1, 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, textureBuffer[1]);
		GLint skyboxTex_samp2 = glGetUniformLocation(centerID, "ref_texture[1]");
		glUniform1i(skyboxTex_samp2, 3);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, textureBuffer[2]);
		GLint skyboxTex_samp3 = glGetUniformLocation(centerID, "ref_texture[2]");
		glUniform1i(skyboxTex_samp3, 4);

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, textureBuffer[3]);
		GLint skyboxTex_samp4 = glGetUniformLocation(centerID, "ref_texture[3]");
		glUniform1i(skyboxTex_samp4, 5);

		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, textureBuffer[4]);
		GLint skyboxTex_samp5 = glGetUniformLocation(centerID, "ref_texture[4]");
		glUniform1i(skyboxTex_samp5, 6);

		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, textureBuffer[5]);
		GLint skyboxTex_samp6 = glGetUniformLocation(centerID, "ref_texture[5]");
		glUniform1i(skyboxTex_samp6, 7);

		for (int i = 0; i < 6; i++)
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glUseProgram(centerID);
			glm::mat4 world2camera = FBOCam[i].getWorld2camera();
			glm::mat4 camera2ndc = glm::perspective(glm::radians(90.f), 1.f, 0.1f, 100.f);
			GLint vertexcameraLoc = glGetUniformLocation(centerID, "camera");
			GLint vertexprojectionLoc = glGetUniformLocation(centerID, "projection");
			glUniformMatrix4fv(vertexcameraLoc, 1, GL_FALSE, &world2camera[0][0]);
			glUniformMatrix4fv(vertexprojectionLoc, 1, GL_FALSE, &camera2ndc[0][0]);

			glDisable(GL_DEPTH_TEST);
			glDepthMask(GL_FALSE);

			glUseProgram(skyboxID);
			glm::mat4 skyboxCam = glm::mat4(glm::mat3(world2camera));

			GLint skyboxvertexcameraLoc = glGetUniformLocation(skyboxID, "camera");
			GLint skyboxvertexprojectionLoc = glGetUniformLocation(skyboxID, "projection");

			glUniformMatrix4fv(skyboxvertexcameraLoc, 1, GL_FALSE, &skyboxCam[0][0]);
			glUniformMatrix4fv(skyboxvertexprojectionLoc, 1, GL_FALSE, &camera2ndc[0][0]);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, skyboxTextureID[0]);
			GLint skyboxTex_samp1 = glGetUniformLocation(skyboxID, "skybox_texture[0]");
			glUniform1i(skyboxTex_samp1, 0);

			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, skyboxTextureID[1]);
			GLint skyboxTex_samp2 = glGetUniformLocation(skyboxID, "skybox_texture[1]");
			glUniform1i(skyboxTex_samp2, 1);

			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, skyboxTextureID[2]);
			GLint skyboxTex_samp3 = glGetUniformLocation(skyboxID, "skybox_texture[2]");
			glUniform1i(skyboxTex_samp3, 2);

			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, skyboxTextureID[3]);
			GLint skyboxTex_samp4 = glGetUniformLocation(skyboxID, "skybox_texture[3]");
			glUniform1i(skyboxTex_samp4, 3);

			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, skyboxTextureID[4]);
			GLint skyboxTex_samp5 = glGetUniformLocation(skyboxID, "skybox_texture[4]");
			glUniform1i(skyboxTex_samp5, 4);

			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, skyboxTextureID[5]);
			GLint skyboxTex_samp6 = glGetUniformLocation(skyboxID, "skybox_texture[5]");
			glUniform1i(skyboxTex_samp6, 5);

			glBindVertexArray(skyboxVao);
			glDrawElements(GL_TRIANGLES, skyboxMesh.getIndexBufferSize(), GL_UNSIGNED_INT, 0);
			
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);

			glUseProgram(sphereID);
			GLint vertexcameraLoc_ = glGetUniformLocation(sphereID, "camera");
			GLint vertexprojectionLoc_ = glGetUniformLocation(sphereID, "projection");
			//world2camera[3][2] = 0;
			glUniformMatrix4fv(vertexcameraLoc_, 1, GL_FALSE, &skyboxCam[0][0]);
			glUniformMatrix4fv(vertexprojectionLoc_, 1, GL_FALSE, &camera2ndc[0][0]);
			
			for (int j = 0; j < LightNum; j++)
			{
				glm::mat4 model2world = mats[j];

				GLint vertexmodelLoc = glGetUniformLocation(sphereID, "model");
				GLint vertexColorLoc = glGetUniformLocation(sphereID, "ourColor");
				glUniform3f(vertexColorLoc, cols[j].x, cols[j].y, cols[j].z);
				glUniformMatrix4fv(vertexmodelLoc, 1, GL_FALSE, &model2world[0][0]);

				glBindVertexArray(SphereVao);
				glDrawElements(GL_TRIANGLES, procedureSphere.getIndexBufferSize(), GL_UNSIGNED_INT, 0);
			}

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureBuffer[i], 0);
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	draw1(NormalID, orbitMat, orbitColor, orbitVao, orbit, true);
	
	return 0;
}

void SceneManager::InitLightData()
{
	Scenario1 = false;
	Scenario2 = false;
	Scenario3 = false;

	check3 = false;
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

GLuint  SceneManager::Textureload(const char* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width_, height_, nrComponents;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* data = stbi_load(path, &width_, &height_, &nrComponents, 0);
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
		glTexImage2D(GL_TEXTURE_2D, 0, format, width_, height_, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
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

void SceneManager::draw1(GLuint programID, glm::mat4 model, glm::vec3 color, GLuint vao, Mesh pMesh, bool IsOrbit, bool IsNormal, bool Isplane) //phong lighting draw
{
	glUseProgram(programID);
	for (int i = 0; i < LightNum; i++)
	{
		allocateUniformBlock(i, i * 112);
	}
	eye = cam.getEye();
	GLint viewPos = glGetUniformLocation(programID, "viewPos");
	glUniform3f(viewPos, eye.x, eye.y, eye.z);

	GLint FogPos = glGetUniformLocation(programID, "fogcolor");
	glUniform3f(FogPos, fogColor.x, fogColor.y, fogColor.z);
	GLint FogminPos = glGetUniformLocation(programID, "fogMin");
	glUniform1f(FogminPos, fogMin);
	GLint FogmaxPos = glGetUniformLocation(programID, "fogMax");
	glUniform1f(FogmaxPos, fogMax);

	GLint shininess = glGetUniformLocation(programID, "material.shininess");
	glUniform1f(shininess, 64.0f);

	GLint Plane = glGetUniformLocation(programID, "IsPlane");
	glUniform1i(Plane, Isplane);

	GLint IsGPUPos = glGetUniformLocation(programID, "IsGPU");
	glUniform1i(IsGPUPos, IsGPU);

	GLint lightNumPos = glGetUniformLocation(programID, "activeLight");
	glUniform1i(lightNumPos, LightNum);

	GLint IsEntityos = glGetUniformLocation(programID, "Entity");
	glUniform1i(IsEntityos, Entity);

	GLint min_pos = glGetUniformLocation(programID, "min_");
	glUniform3f(min_pos, min.x, min.y, min.z);

	GLint max_pos = glGetUniformLocation(programID, "max_");
	glUniform3f(max_pos, max.x, max.y, max.z);

	GLint uvPos = glGetUniformLocation(programID, "IsUV");
	glUniform1i(uvPos, UVVisible);

	GLint ReflectPos = glGetUniformLocation(programID, "Reflect");
	glUniform1i(ReflectPos, Reflection);
	GLint RefractionPos = glGetUniformLocation(programID, "Refract");
	glUniform1i(RefractionPos, Refraction);
	GLint materialTypePos = glGetUniformLocation(programID, "materialType");
	glUniform1f(materialTypePos, MaterialRatio);


	glm::mat4 world2camera = cam.getWorld2camera();
	glm::mat4 camera2ndc = glm::perspective(glm::radians(45.f), 1024.f / 768.f, 0.1f, 100.f);
	glm::mat4 model2world = model;

	GLint vertexcameraLoc = glGetUniformLocation(programID, "camera");
	GLint vertexprojectionLoc = glGetUniformLocation(programID, "projection");
	GLint vertexmodelLoc = glGetUniformLocation(programID, "model");
	//GLint LightTypeLoc = glGetUniformLocation(programID, "lightType");
	//glUniform1i(LightTypeLoc, lightType);

	if (IsNormal == true)
	{
		glUseProgram(programID);

		//GLint vertexColorLoc = glGetUniformLocation(programID, "ourColor");
		//glUniform3f(vertexColorLoc, color.x, color.y, color.z);
		glUniformMatrix4fv(vertexcameraLoc, 1, GL_FALSE, &world2camera[0][0]);
		glUniformMatrix4fv(vertexprojectionLoc, 1, GL_FALSE, &camera2ndc[0][0]);
		glUniformMatrix4fv(vertexmodelLoc, 1, GL_FALSE, &model2world[0][0]);

		glBindVertexArray(vao);

		glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(pMesh.vertexNormalDisplay.size() * 2));

		glBindVertexArray(0);
		return;
	}

	glUniformMatrix4fv(vertexcameraLoc, 1, GL_FALSE, &world2camera[0][0]);
	glUniformMatrix4fv(vertexprojectionLoc, 1, GL_FALSE, &camera2ndc[0][0]);
	glUniformMatrix4fv(vertexmodelLoc, 1, GL_FALSE, &model2world[0][0]);

	if (UVVisible)
	{
		//glActiveTexture(GL_TEXTURE0);
		//glBindTexture(GL_TEXTURE_2D, ID_grid);
	}
	else
	{
	/*	glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseID);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularID);*/
	}
	glBindVertexArray(vao);

	if (IsOrbit == true)
	{
		glDrawArrays(GL_LINES, 0, pMesh.getIndexBufferSize());
	}
	else
	{
		glDrawElements(GL_TRIANGLES, pMesh.getIndexBufferSize(), GL_UNSIGNED_INT, 0);
	}

	glBindVertexArray(0);
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

void SceneManager::draw(GLuint programID, glm::mat4 model, glm::vec3 color, GLuint vao, Mesh pMesh, glm::mat4 world2Camera_ , bool IsOrbit, bool IsNormal, bool IsEnvironment)
{
	glUseProgram(programID);

	GLint vertexColorLoc = glGetUniformLocation(programID, "ourColor");
	glm::mat4 world2camera = cam.getWorld2camera();
	glm::mat4 camera2ndc = glm::perspective(glm::radians(45.f), 1024.f / 768.f, 0.1f, 100.f);
	if (IsEnvironment)
	{
		world2camera = world2Camera_;
		camera2ndc = glm::perspective(glm::radians(90.f), 1.f, 0.1f, 100.f);
		world2camera[3][2] = 0;
	}
	
	glm::mat4 model2world = model;

	GLint vertexcameraLoc = glGetUniformLocation(programID, "camera");
	GLint vertexprojectionLoc = glGetUniformLocation(programID, "projection");
	GLint vertexmodelLoc = glGetUniformLocation(programID, "model");

	if (IsNormal == true)
	{
		glUseProgram(programID);

		glUniform3f(vertexColorLoc, color.x, color.y, color.z);
		glUniformMatrix4fv(vertexcameraLoc, 1, GL_FALSE, &world2camera[0][0]);
		glUniformMatrix4fv(vertexprojectionLoc, 1, GL_FALSE, &camera2ndc[0][0]);
		glUniformMatrix4fv(vertexmodelLoc, 1, GL_FALSE, &model2world[0][0]);

		glBindVertexArray(vao);

		glDrawArrays(GL_LINES, 0, static_cast<GLsizei>(pMesh.vertexNormalDisplay.size() * 2));
		//glDrawArrays(GL_LINES, 0, pMesh.faceNormalsDisplay.size() * 2);

		glBindVertexArray(0);
		return;
	}

	glm::vec3 lightColor(1.f);

	GLint lightLoc = glGetUniformLocation(programID, "lightPos");
	GLint lgihtColLoc = glGetUniformLocation(programID, "lightColor");


	glUniform3f(vertexColorLoc, color.x, color.y, color.z);
	glUniformMatrix4fv(vertexcameraLoc, 1, GL_FALSE, &world2camera[0][0]);
	glUniformMatrix4fv(vertexprojectionLoc, 1, GL_FALSE, &camera2ndc[0][0]);
	glUniformMatrix4fv(vertexmodelLoc, 1, GL_FALSE, &model2world[0][0]);
	glUniform3f(lightLoc, light[0].x, light[0].y, light[0].z);
	glUniform3f(lgihtColLoc, lightColor.x, lightColor.y, lightColor.z);

	glBindVertexArray(vao);

	if (IsOrbit == true)
	{
		glDrawArrays(GL_LINES, 0, pMesh.getIndexBufferSize());
	}
	else
	{
		glDrawElements(GL_TRIANGLES, pMesh.getIndexBufferSize(), GL_UNSIGNED_INT, 0);
	}

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
	ImGui::Checkbox("Reflection", &Reflection);
	ImGui::Checkbox("Refraction", &Refraction);

	if (check3 != Scenario3)
	{
		check3 = Scenario3;
		lightType[0] = 0;
		lightType[4] = 1;
		lightType[7] = 2;
		lightType[5] = 0;
		lightType[9] = 1;
		lightType[10] = 2;
		lightType[14] = 1;
	}

	const char* mesh[] = { "sphere", "sphere_4", "bunny", "cube", "sphere_modified" };
	static int item_current = 2;
	ImGui::Combo("Object Mesh", &item_current, mesh, IM_ARRAYSIZE(mesh));
	if (change != item_current)
	{
		changeObj(mesh[item_current]);
	}
	const char* sahders[] = { "Phong Shading", "Phong Shading" , "Blinn Shading" };
	static int shader_current = 1;
	ImGui::Combo("Shader", &shader_current, sahders, IM_ARRAYSIZE(sahders));
	if (currentShader != shader_current)
	{
		currentShader = shader_current;
		changeShader(sahders[shader_current]);
		draw1(centerID, centerMat, centerColor, centerVao, centerOBJ);
		draw1(planeID, planeMat, planeColor, planeVao, planeOBJ);
	}

	const char* materials[] = { "Air", "Hydrogen" , "Water", "Olive Oil", "Ice", "Quartz", "Diamond", "Acrylic/ Plexiglas/ Lucite"};
	static int materials_current = 3;
	ImGui::Combo("materials", &materials_current, materials, IM_ARRAYSIZE(materials));
	if (currentMaterial != materials_current)
	{
		currentMaterial = materials_current;
		if (currentMaterial == 0)
		{
			MaterialRatio = 1.000293f;
		}
		else if (currentMaterial == 1)
		{
			MaterialRatio = 1.000132f;
		}
		else if (currentMaterial == 2)
		{
			MaterialRatio = 1.333f;
		}
		else if (currentMaterial == 3)
		{
			MaterialRatio = 1.47f;
		}
		else if (currentMaterial == 4)
		{
			MaterialRatio = 1.31f;
		}
		else if (currentMaterial == 5)
		{
			MaterialRatio = 1.46f;
		}
		else if (currentMaterial == 6)
		{
			MaterialRatio = 2.42f;
		}
		else if (currentMaterial == 7)
		{
			MaterialRatio = 1.49f;
		}
		changeShader(sahders[shader_current]);
		draw1(centerID, centerMat, centerColor, centerVao, centerOBJ);
	}

	ImGui::DragFloat("ratio", &MaterialRatio, 0.001f, 0, 100.0f);

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


	ImGui::Checkbox("Display VertexNormal", &vertexCheck);
	ImGui::Checkbox("Display FaceNormal", &faceCheck);

	ImGui::End();
}

void SceneManager::changeUV(int uvCurrent)
{
	OBJLoaderManager->reloadUV(Entity, currentUV);
	centerOBJ = OBJLoaderManager->GetObject(change).objMesh;
	centerVao = OBJLoaderManager->GetObject(change).VAO;

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

	centerOBJ = OBJLoaderManager->GetObject(change).objMesh;
	centerID = OBJLoaderManager->GetObject(change).program;
	centerVao = OBJLoaderManager->GetObject(change).VAO;

	min = centerOBJ.boundingBox[0];
	max = centerOBJ.boundingBox[1];

	DisplayNormalBuffer();
	FinishVNormal = true;
	DisplayNormalBuffer();
	FinishVNormal = false;
}

void SceneManager::changeShader(std::string shaders_)
{
	if (shaders_ == "Phong Lighting")
	{
		OBJLoaderManager->ReloadOBJ("../Common/shaders/phongLighting.vert", "../Common/shaders/phongLighting.frag");
	}
	else if (shaders_ == "Phong Shading")
	{
		OBJLoaderManager->ReloadOBJ("../Common/shaders/phongShading.vert", "../Common/shaders/phongShading.frag");
	}
	else if (shaders_ == "Blinn Shading")
	{
		OBJLoaderManager->ReloadOBJ("../Common/shaders/BlinnShading.vert", "../Common/shaders/BlinnShading.frag");
	}

	glDeleteProgram(centerID);
	centerID = OBJLoaderManager->GetObject(change).program;
	centerID = OBJLoaderManager->GetObject(change).program;

	glDeleteProgram(planeID);
	planeID = OBJLoaderManager->GetObject(quad).program;
	planeID = OBJLoaderManager->GetObject(quad).program;

	DisplayNormalBuffer();
	FinishVNormal = true;
	DisplayNormalBuffer();
	FinishVNormal = false;

	glUseProgram(centerID);
	glUseProgram(planeID);

	GLint diffusePos_m = glGetUniformLocation(centerID, "material.diffuse");
	if (diffusePos_m >= 0)
		glUniform1i(diffusePos_m, 0);
	else
		std::cout << "error" << std::endl;
	GLint specularPos_m = glGetUniformLocation(centerID, "material.specular");
	if (specularPos_m >= 0)
		glUniform1i(specularPos_m, 1);
	else
		std::cout << "error" << std::endl;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////
int SceneManager::postRender()
{
	angleOfRotation2 += 0.01f;
	return 0;
}

void SceneManager::GenerateSkyBoxCube()
{
	OBJLoaderManager->LoadCube("../Common/shaders/skyboxShader.vert", "../Common/shaders/skyboxShader.frag");
	skyboxMesh = OBJLoaderManager->getCube().objMesh;
	skyboxVao = OBJLoaderManager->getCube().VAO;
	skyboxID = OBJLoaderManager->getCube().program;

	faceString.push_back("../Common/textures/skybox/left.jpg");//0
	faceString.push_back("../Common/textures/skybox/right.jpg");//1
	faceString.push_back("../Common/textures/skybox/bottom.jpg");//2
	faceString.push_back("../Common/textures/skybox/top.jpg");//3
	faceString.push_back("../Common/textures/skybox/back.jpg");//4
	faceString.push_back("../Common/textures/skybox/front.jpg");//5

	for (int i = 0; i < 6; i++)
	{
		skyboxTextureID[i] = (Textureload(faceString[i].c_str()));
	}

}

void SceneManager::skyboxDraw(bool fbo, glm::mat4 world2camera_)
{

	//glm::vec3 eye(0.f, 0.f, 0.f);
	glUseProgram(skyboxID);

	glm::mat4 world2camera = cam.getWorld2camera();
	glm::mat4 camera2ndc = glm::perspective(glm::radians(45.f), 1024.f / 768.f, 0.1f, 100.f);
	
	if (fbo == true)
	{
		world2camera = world2camera_;
		//world2camera = FBOCam[1].getWorld2camera();
		//world2
		camera2ndc = glm::perspective(glm::radians(90.f), 1.f, 0.1f, 100.f);
	}
	/*glm::mat4 world2camera = glm::lookAt(eye, glm::vec3(1.0f, 0, 0), glm::vec3(0, 1, 0));
	glm::mat4 camera2ndc = glm::perspective(glm::radians(90.f),1.f, 0.1f, 100.f);*/
	world2camera[3][2] = 0;

	GLint vertexcameraLoc = glGetUniformLocation(skyboxID, "camera");
	GLint vertexprojectionLoc = glGetUniformLocation(skyboxID, "projection");

	glUniformMatrix4fv(vertexcameraLoc, 1, GL_FALSE, &world2camera[0][0]);
	glUniformMatrix4fv(vertexprojectionLoc, 1, GL_FALSE, &camera2ndc[0][0]);

	//skyboxTextureSetting();

	if (fbo == false)
	{
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, skyboxTextureID[0]);
		GLint skyboxTex_samp1 = glGetUniformLocation(skyboxID, "skybox_texture[0]");
		glUniform1i(skyboxTex_samp1, 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, skyboxTextureID[1]);
		GLint skyboxTex_samp2 = glGetUniformLocation(skyboxID, "skybox_texture[1]");
		glUniform1i(skyboxTex_samp2, 3);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, skyboxTextureID[2]);
		GLint skyboxTex_samp3 = glGetUniformLocation(skyboxID, "skybox_texture[2]");
		glUniform1i(skyboxTex_samp3, 4);

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, skyboxTextureID[3]);
		GLint skyboxTex_samp4 = glGetUniformLocation(skyboxID, "skybox_texture[3]");
		glUniform1i(skyboxTex_samp4, 5);

		glActiveTexture(GL_TEXTURE6);
		glBindTexture(GL_TEXTURE_2D, skyboxTextureID[4]);
		GLint skyboxTex_samp5 = glGetUniformLocation(skyboxID, "skybox_texture[4]");
		glUniform1i(skyboxTex_samp5, 6);

		glActiveTexture(GL_TEXTURE7);
		glBindTexture(GL_TEXTURE_2D, skyboxTextureID[5]);
		GLint skyboxTex_samp6 = glGetUniformLocation(skyboxID, "skybox_texture[5]");
		glUniform1i(skyboxTex_samp6, 7);

		glBindVertexArray(skyboxVao);
		glDrawElements(GL_TRIANGLES, skyboxMesh.getIndexBufferSize(), GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);
	}
	else
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureBuffer[0]);
		GLint skyboxTex_samp1 = glGetUniformLocation(skyboxID, "skybox_texture[0]");
		glUniform1i(skyboxTex_samp1, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, textureBuffer[1]);
		GLint skyboxTex_samp2 = glGetUniformLocation(skyboxID, "skybox_texture[1]");
		glUniform1i(skyboxTex_samp2, 1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, textureBuffer[2]);
		GLint skyboxTex_samp3 = glGetUniformLocation(skyboxID, "skybox_texture[2]");
		glUniform1i(skyboxTex_samp3, 2);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, textureBuffer[3]);
		GLint skyboxTex_samp4 = glGetUniformLocation(skyboxID, "skybox_texture[3]");
		glUniform1i(skyboxTex_samp4, 3);

		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_2D, textureBuffer[4]);
		GLint skyboxTex_samp5 = glGetUniformLocation(skyboxID, "skybox_texture[4]");
		glUniform1i(skyboxTex_samp5, 4);

		glActiveTexture(GL_TEXTURE5);
		glBindTexture(GL_TEXTURE_2D, textureBuffer[5]);
		GLint skyboxTex_samp6 = glGetUniformLocation(skyboxID, "skybox_texture[5]");
		glUniform1i(skyboxTex_samp6, 5);
		

		glBindVertexArray(skyboxVao);
		glDrawElements(GL_TRIANGLES, skyboxMesh.getIndexBufferSize(), GL_UNSIGNED_INT, 0);
		//glBindVertexArray(0);
	}

	
}

void SceneManager::generateFBO()
{
	glGenFramebuffers(1, &framebuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);

	for (int i = 0; i < 6; i++)
	{
		glGenTextures(1, &textureBuffer[i]);
		glBindTexture(GL_TEXTURE_2D, textureBuffer[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		//glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureBuffer[i], 0);
	}

	glGenRenderbuffers(1, &renderbuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, renderbuffer);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "framebuffer is not complete" << std::endl;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

