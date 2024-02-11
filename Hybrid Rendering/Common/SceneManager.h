/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: SceneManager.h
Purpose: Render all objects, set buffer for some objects and update the GUI
Language: C++
Platform: VisualStudio 2019 / Window
Project: uijin.lee_CS300_2
Author: Uijin Lee, uijn.lee, 180003221
Creation date: Nov 11st, 2021
End Header --------------------------------------------------------*/
#ifndef SCENE_MANAGER_H
#define SCENE_MANAGER_H

#include "Scene.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "OBJLoader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "OBJReader.h"
#include "Mesh.h"
#include "Camera.h"

class OBJLoader;

class SceneManager : public Scene
{
	friend OBJLoader;
public:
	SceneManager() = default;
	SceneManager(int windowWidth, int windowHeight);
	virtual ~SceneManager();

	enum type
	{
		sphere,
		sphere_4,
		bunny,
		cube,
		sphere_modified,
		quad,
	};

	int Init() override;
	void CleanUp() override;

	int Render() override;
	int postRender() override;

public:
	GLuint GBuffer;

	GLuint G_pos;
	GLuint G_norm;
	GLuint G_albedo;

	int type;
	bool depth;
	//void renderQuad();
	GLuint quadVao;
	bool  FSQCheck = false;
	void SetGBuffer();
	GLuint geometryID;
	GLuint lightID;
	bool once;
	// member functions
	void SetOrbit();

	void drawNormal(GLuint programID, glm::mat4 model, glm::vec3 color, GLuint vao, Mesh pMesh, bool IsOrbit = false, bool IsNormal = false);
	void allocateUniformBlock(int numLight, int byte);
	void InitLightData();
	void changeLightPosition(int i, glm::mat4 matrix);
	GLuint phongMaterial(const char* path);
	void changeUV(int uvCurrent);
	GLuint Centerubo;
	GLuint Planeubo;

	std::vector<glm::vec3> diffuse;
	std::vector<glm::vec3> specular;
	GLuint diffuseID;
	GLuint specularID;
	GLuint ID_grid;

	bool UVVisible;
	int IsGPU; //01234 4is cpu
	bool GPUCheck;
	bool currentGPUCheck = false;
	bool Entity;
	glm::vec3 min;
	glm::vec3 max;

	int currentUV;
	bool currentEntity;

	int lightType[16];
	float constant;
	float linear;
	float quadratic;
	float theta[16];
	float phi[16];
	float fallout[16];
	glm::vec3 lightDir[16];
	glm::vec3 light[16];
	glm::vec3 L_ambient[16];
	glm::vec3 L_diffuse[16];
	glm::vec3 L_specular[16];
	int LightNum = 16;
	std::vector<glm::vec3>cols;
	glm::vec3 Emissive;
	glm::vec3 globalAmbient;
	glm::vec3 ka;
	glm::vec3 fogColor;
	float fogMax;
	float fogMin;

	bool Scenario1;
	bool Scenario2;
	bool Scenario3;
	bool check3;
	Mesh* createSphere(float radius, int numDivisions);
	void sphere_setBuffer();
	void DisplayNormalBuffer();
	void UpadateGUI();
	void changeObj(std::string obj_);
	//void changeShader(std::string shaders_);
	void setPlane();
	OBJLoader* OBJLoaderManager;

	Mesh centerOBJ;
	GLuint  centerID;
	glm::vec3 centerColor;
	glm::mat4 centerMat = glm::mat4(1.0f);
	GLuint centerVao;

	GLfloat   angleOfRotation;
	GLfloat   angleOfRotation2;
	bool afterInit;

	Mesh planeOBJ;
	GLuint  planeID;
	glm::vec3 planeColor;
	glm::mat4 planeMat = glm::mat4(1.0f);
	GLuint planeVao;

	Mesh orbit;
	GLuint orbitVao;
	glm::vec3 orbitColor;
	glm::mat4 orbitMat;


	Mesh procedureSphere;
	GLuint SphereVao;
	GLuint SphereNorm;
	
	glm::vec3 SphereColor;
	glm::mat4 SphereMat;
	glm::mat4 SphereMat2;
	glm::mat4 SphereMat3;
	glm::mat4 SphereMat4;
	glm::mat4 SphereMat5;
	glm::mat4 SphereMat6;
	glm::mat4 SphereMat7;
	glm::mat4 SphereMat8;
	glm::mat4 SphereMat9;
	glm::mat4 SphereMat10;
	glm::mat4 SphereMat11;
	glm::mat4 SphereMat12;
	glm::mat4 SphereMat13;
	glm::mat4 SphereMat14;
	glm::mat4 SphereMat15;
	glm::mat4 SphereMat16;

	glm::mat4 staticMat;
	GLuint sphereID;
	GLuint NormalVao;
	GLuint NormalfaceVao;

	GLuint NormalID;

	bool FinishVNormal;
	bool vertexCheck;
	bool faceCheck;

	const char* current_item;
	const char* current_shader;
	int change = 0;

	int currentShader;


};


#endif //SIMPLE_SCENE_SIMPLESCENE_QUAD_H
