/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: newFragmentShader.frag
Purpose: newFragmentShader.frag
Language: glsl
Platform: VisualStudio 2019 / Window
Project: uijin.lee_CS300_2
Author: Uijin Lee, uijn.lee, 180003221
Creation date: Nov 11st, 2021
End Header --------------------------------------------------------*/
#version 400 core

out vec4 FragColor;

uniform vec3 ourColor;


void main()
{
	FragColor = vec4(ourColor, 1.f);
}