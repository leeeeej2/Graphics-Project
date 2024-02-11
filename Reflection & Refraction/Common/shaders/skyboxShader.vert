/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: phongShading.frag
Purpose: skyboxShader.vert
Language: glsl
Platform: VisualStudio 2019 / Window
Project: uijin.lee_CS300_2
Author: Uijin Lee, uijn.lee, 180003221
Creation date: Nov 11st, 2021
End Header --------------------------------------------------------*/
#version 400 core
layout (location = 0) in vec3 aPos;

out vec3 uvPos;

uniform mat4 camera;
uniform mat4 projection;

out mat4 camera_test;
void main()
{
    camera_test = camera;
    uvPos = aPos;
    mat4 newCam = mat4(mat3(camera));
    //camera = mat4(mat3(camera));
    vec4 position = projection * newCam * vec4(aPos, 1.0f);
	//vec4 position = projection * camera * vec4(aPos, 1.0f);
    gl_Position = position.xyww;
}