/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Camera.cpp
Purpose: Camera
Language: C++
Platform: VisualStudio 2019 / Window
Project: uijin.lee_CS300_3
Author: Uijin Lee, uijn.lee, 180003221
Creation date: Dec 10st, 2021
End Header --------------------------------------------------------*/
#include "Camera.h"

Camera::Camera():eyes(glm::vec3(0, 0, 0)), front(glm::vec3(0, 0, 1.f)), worldUp(glm::vec3(0, 1, 0)), up(glm::vec3(0, 1, 0)), yaw(0.f), pitch(0.f)
{
}

Camera::Camera(glm::vec3 Eye, glm::vec3 Front, glm::vec3 Up, float Yaw, float Pitch)
	:eyes(Eye), front(Front), worldUp(Up), up(Up), yaw(Yaw), pitch(Pitch)
{
	right = glm::normalize(glm::cross(front, worldUp));
}

void Camera::updateCam()
{
	glm::vec3 tmp;
	tmp.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	tmp.y = sin(glm::radians(pitch));
	tmp.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));

	front = glm::normalize(tmp);

	right = glm::normalize(glm::cross(front, worldUp));
	up = glm::normalize(glm::cross(right, front));
	//world2camera = glm::lookAt(eyes, eyes + front, up);
}

void Camera::forward(float velocity)
{
	eyes += front * velocity;
	//world2camera = glm::lookAt(eyes, eyes + front, up);
}

void Camera::Right(float velocity)
{
	eyes += right * velocity;
	//world2camera = glm::lookAt(eyes, eyes + front, up);
}

glm::vec3 Camera::getEye()
{
	//world2camera = 
	return eyes;
}

 glm::mat4 Camera::getWorld2camera()
{
	return glm::lookAt(eyes, eyes + front, up);
}
