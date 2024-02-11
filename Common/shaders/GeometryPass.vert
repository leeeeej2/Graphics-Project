/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: GeometryPass.vert
Purpose: store texture information
Language: C++
Platform: VisualStudio 2019 / Window
Project: uijin.lee_CS350_1
Author: Uijin Lee, uijn.lee, 180003221
Creation date: Feb 6st, 2022
End Header --------------------------------------------------------*/
#version 400 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 camera;
uniform mat4 projection;

void main()
{
    vec4 wPos = model * vec4(aPos, 1.0);
    FragPos = vec3(wPos);
    TexCoords = aTexCoords;
    Normal = mat3(transpose(inverse(model))) * aNormal;

    gl_Position = projection * camera * wPos;
}