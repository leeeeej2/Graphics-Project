/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: LightPass_phong.vert
Purpose: get value from GBuffer and implement phong shading
Language: C++
Platform: VisualStudio 2019 / Window
Project: uijin.lee_CS350_1
Author: Uijin Lee, uijn.lee, 180003221
Creation date: Feb 6st, 2022
End Header --------------------------------------------------------*/
#version 400 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

void main()
{
    TexCoords = aTexCoords;
	gl_Position = vec4(aPos, 1.0f);
}