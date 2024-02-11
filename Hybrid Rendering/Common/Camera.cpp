/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Camera.cpp
Purpose: Implement camera
Language: C++
Platform: VisualStudio 2019 / Window
Project: uijin.lee_CS350_1
Author: Uijin Lee, uijn.lee, 180003221
Creation date: Feb 6st, 2022
End Header --------------------------------------------------------*/
#include "Camera.h"

Camera::Camera() : eye(glm::vec3(0, 0, 0)), target(glm::vec3(0, 0, -1.f)), up(glm::vec3(0, 1, 0)), yaw(-90.f),  pitch(0.f), speed(2.5f), zoom(45.f)
{
	Vp = up;
	updateCamera();
}

Camera::Camera(glm::vec3 Eye, glm::vec3 Target, glm::vec3 Up, float Yaw, float Pitch)
{
	eye = Eye;
	target = Target;
	Vp = Up;
	yaw = Yaw;
	pitch = Pitch;

	speed = 2.5f;
	zoom = 45.f;
	updateCamera();
}

void Camera::updateCamera()
{
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	target = glm::normalize(front);
	
	right = glm::normalize(glm::cross(target, Vp));
	up = glm::normalize(glm::cross(right, target));
}

glm::mat4 Camera::getLookAt()
{
	return glm::lookAt(eye, eye+target, up);
}

void Camera::KeyboardMove(char key, float dt)
{
	float v = speed * dt;
	if (key == 'w') //forward
	{
		eye += target * v;
	}
	else if (key == 's') //backward
	{
		eye -= target * v;
	}
	else if (key == 'a') //left
	{
		eye -= right * v;
	}
	else if (key == 'd') //right
	{
		eye += right * v;
	}
}

