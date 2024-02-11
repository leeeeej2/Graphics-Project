/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Camera.h
Purpose: Implement camera
Language: C++
Platform: VisualStudio 2019 / Window
Project: uijin.lee_CS350_1
Author: Uijin Lee, uijn.lee, 180003221
Creation date: Feb 6st, 2022
End Header --------------------------------------------------------*/
#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

class Camera
{
public:
	Camera();
	Camera(glm::vec3 Eye, glm::vec3 Target, glm::vec3 Up, float Yaw, float Pitch);
	void updateCamera();
	glm::mat4 getLookAt();
	
	void KeyboardMove(char key, float dt);
	//glm::vec3 getEye();
	
	glm::vec3 eye; //position
	glm::vec3 target; //front;
	glm::vec3 up;
	glm::vec3 right;
	glm::vec3 Vp;

	float yaw;
	float pitch;
	float speed;
	float zoom;
};