/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: phongShading.frag
Purpose: skyboxShader.frag
Language: glsl
Platform: VisualStudio 2019 / Window
Project: uijin.lee_CS300_2
Author: Uijin Lee, uijn.lee, 180003221
Creation date: Nov 11st, 2021
End Header --------------------------------------------------------*/
#version 400 core

out vec4 FragColor;

in vec3 uvPos;
uniform sampler2D skybox_texture[6];
in  mat4 camera_test;
void main()
{  
    vec2 realTex;

     float x = uvPos.x;
	 float y = uvPos.y;
	 float z = uvPos.z;

     float absX = abs(x);
     float absY = abs(y);
     float absZ = abs(z);

     int isXpositive = x > 0 ? 1 : 0;
     int isYpositive = y > 0 ? 1 : 0;
     int isZpositive = z > 0 ? 1 : 0;

     float maxAsis, uc, vc;
     vec2 uv = vec2(0, 0);

		// POSITIVE X - right
	if(bool(isXpositive) && (absX >= absY) && (absX >= absZ))
	{
		maxAsis = absX;
        uc = -z;
        vc = y;
        realTex.x = 0.5 *(uc/maxAsis + 1);
        realTex.y = 0.5 *(vc/maxAsis + 1);

        FragColor = texture(skybox_texture[1], realTex);

	}

	// NEGATIVE X - left
    else if(!bool(isXpositive) && absX >= absY && absX >= absZ)
    {
        maxAsis = absX;
        uc = z;
        vc = y;
        realTex.x = 0.5 *(uc/maxAsis + 1);
        realTex.y = 0.5 *(vc/maxAsis + 1);     
        FragColor = texture(skybox_texture[0], realTex);   
    }

	// POSITIVE Y -top
	else if(bool(isYpositive) && absY >= absX && absY >= absZ)
    {
        maxAsis = absY;
        uc = x;
        vc = -z;
        realTex.x = 0.5 *(uc/maxAsis + 1);
        realTex.y = 0.5 *(vc/maxAsis + 1);     
        FragColor = texture(skybox_texture[3], realTex);   
    }

	// NEGATIVE Y -bottom
	else if(!bool(isYpositive) && absY >= absX && absY >= absZ)
    {
        maxAsis = absY;
        uc = x;
        vc = z;
        realTex.x = 0.5 *(uc/maxAsis + 1);
        realTex.y = 0.5 *(vc/maxAsis + 1);    
        FragColor = texture(skybox_texture[2], realTex);    
    }

	// POSITIVE Z - front
	 else if(bool(isZpositive) && absZ >= absX && absZ >= absY)
    {
        maxAsis = absZ;
        uc = x;
        vc = y;
        realTex.x = 0.5 *(uc/maxAsis + 1);
        realTex.y = 0.5 *(vc/maxAsis + 1);    
        FragColor = texture(skybox_texture[5], realTex);   
    }

	// NEGATIVE Z -back
	 else if(!bool(isZpositive) && absZ >= absX && absZ >= absY)
    {
        maxAsis = absZ;
        uc = -x;
        vc = y;
        realTex.x = 0.5 *(uc/maxAsis + 1);
        realTex.y = 0.5 *(vc/maxAsis + 1);
        FragColor = texture(skybox_texture[4], realTex);
    }
}