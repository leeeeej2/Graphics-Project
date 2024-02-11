/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Camera.h
Purpose: Camera
Language: C++
Platform: VisualStudio 2019 / Window
Project: uijin.lee_CS300_3
Author: Uijin Lee, uijn.lee, 180003221
Creation date: Dec 10st, 2021
End Header --------------------------------------------------------*/
#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>

class Camera
{
public:
	Camera();
	Camera(glm::vec3 Eye,glm::vec3 Front, glm::vec3 Up, 
		float Yaw, float Pitch);
	void updateCam();
	void forward(float velocity);
	void Right(float velocity);
	glm::vec3 getEye();
	 glm::mat4 getWorld2camera();
private:
	glm::vec3 eyes;
	glm::vec3 front;
	glm::vec3 up;

	glm::vec3 right;
	glm::vec3 worldUp;

	float yaw;
	float pitch;

	glm::mat4 world2camera;
};