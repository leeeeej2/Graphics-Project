/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: phongShading.frag
Purpose: phongShading.frag
Language: glsl
Platform: VisualStudio 2019 / Window
Project: uijin.lee_CS300_2
Author: Uijin Lee, uijn.lee, 180003221
Creation date: Nov 11st, 2021
End Header --------------------------------------------------------*/
#version 400 core

struct Material
{
    // sampler2D grid;
	// sampler2D diffuse;
	// sampler2D specular;
	float shininess;
};

 struct multipleLight
 {
    float constant;//4 //0
    float linear;//4	//4
    float quadratic;//4	//8
    
    float theta;//4	//12
    float phi;//4	//16
    float falloff;//4	//20
    int lightType;//4   //24
    vec3 direction;//16	//32
 	vec3 position;//16	//48
 	vec3 ambient;//16	//64
 	vec3 diffuse;//16	//80
 	vec3 specular;//16	//96
 };

 layout (std140) uniform Light
 {   
    multipleLight LightSources[16];
 };

uniform sampler2D ref_texture[6]; //left right bottom top back front 순서

uniform Material material; 
uniform vec3 viewPos;

uniform vec3 I_emmisive;
uniform vec3 globalAmbient;
uniform vec3 Ka;

uniform bool Reflect;
uniform bool Refract;
uniform float materialType;

uniform bool IsPlane;
uniform int activeLight;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

in vec3 uvNormal;
in vec3 uvPos;

uniform bool IsUV;
uniform int IsGPU;
uniform bool Entity;
uniform vec3 min_;
uniform vec3 max_;

uniform vec3 fogcolor;
uniform float fogMin;
uniform float fogMax;

out vec4 FragColor;

vec3 CalcDirLight(multipleLight light);
vec3 CalPointLight(multipleLight light);
vec3 CalSpotLight(multipleLight light);
vec2 CalUV(vec3 centroidVec);
vec2 realTex;
int index;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflection  = (2 * (dot(norm, viewDir)) * norm) - viewDir;
    vec3 color;
    vec3 refractcolor;
    vec3 refraction;
    float materialValue = materialType;

    float k = 1.000293/materialValue;
    float KR = k*1.1;
    float KG = k*1.2;
    float KB = k*1.3;

    refraction = (k*dot(norm, viewDir) - (1 - (k*k)*pow((1 - pow(dot(norm, viewDir), 2)), 0.5)))*norm - (k*viewDir);

    vec3 refractionR = (KR*dot(norm, viewDir) - (1 - (KR*KR)*pow((1 - pow(dot(norm, viewDir), 2)), 0.5)))*norm - (KR*viewDir);
    vec3 refractionG = (KG*dot(norm, viewDir) - (1 - (KG*KG)*pow((1 - pow(dot(norm, viewDir), 2)), 0.5)))*norm - (KG*viewDir);
    vec3 refractionB = (KB*dot(norm, viewDir) - (1 - (KB*KB)*pow((1 - pow(dot(norm, viewDir), 2)), 0.5)))*norm - (KB*viewDir);
    vec3 combine;
    realTex = TexCoords;
    {
        {
            vec3 normal = normalize(uvNormal);
            vec2 uv = vec2(1.0);
            
            if(Reflect == true && Refract ==false)
            {
                uv = CalUV(reflection);
                color = texture(ref_texture[index], uv).rgb;
            }

            if(Refract == true && Reflect == false)
            {
                uv = CalUV(refraction);
                color = texture(ref_texture[index], uv).rgb;
            }

            if(Reflect && Refract)
            {
                uv = CalUV(reflection);
                color = texture(ref_texture[index], uv).rgb;

                uv = CalUV(refractionR);
                refractcolor.x = texture(ref_texture[index], uv).r;
                uv = CalUV(refractionG);
                refractcolor.y = texture(ref_texture[index], uv).g;
                uv = CalUV(refractionB);
                refractcolor.z = texture(ref_texture[index], uv).b;

                color = mix(color, refractcolor, 0.5);
            }
        }
    }    
      

    for(int i = 0; i < activeLight; i++)
    {
        if(LightSources[i].lightType == 0) //directional
        {
            combine += CalcDirLight(LightSources[i]);         
        }
        else if(LightSources[i].lightType == 1) //point
        {
            combine += CalPointLight(LightSources[i]);  
        }
        else if(LightSources[i].lightType == 2) //spotlight
        {
            combine += CalSpotLight(LightSources[i]);       
        }
    }
    if(IsPlane)
    {
    combine += globalAmbient;
    }
    else{
    combine += I_emmisive + globalAmbient*Ka;
    }

    //fog
    float s = (fogMax - length(viewPos))/(fogMax - fogMin);
    vec3 result = s * combine + (1 - s) * fogcolor;
    FragColor = vec4(mix(color, result, 0.1), 1.0);
}

vec2 CalUV(vec3 centroidVec)
{
                float x = centroidVec.x;
                float y = centroidVec.y;
                float z = centroidVec.z;

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
                    index = 1;
                    //color = texture(ref_texture[1], realTex).rgb;
                }

                //	// NEGATIVE X - left
                else if(!bool(isXpositive) && absX >= absY && absX >= absZ)
                {
                    maxAsis = absX;
                    uc = z;
                    vc = y;
                    realTex.x = 0.5 *(uc/maxAsis + 1);
                    realTex.y = 0.5 *(vc/maxAsis + 1);
                    index = 0;
                    //color = texture(ref_texture[0], realTex).rgb;  
                }
                	// POSITIVE Y -top
                else if(bool(isYpositive) && absY >= absX && absY >= absZ)
                {
                    maxAsis = absY;
                    uc = x;
                    vc = -z;
                    realTex.x = 0.5 *(uc/maxAsis + 1);
                    realTex.y = 0.5 *(vc/maxAsis + 1);
                    index = 3;
                    //color = texture(ref_texture[3], realTex).rgb;
                }

                //	// NEGATIVE Y -bottom
                else if(!bool(isYpositive) && absY >= absX && absY >= absZ)
                {
                    maxAsis = absY;
                    uc = x;
                    vc = z;
                    realTex.x = 0.5 *(uc/maxAsis + 1);
                    realTex.y = 0.5 *(vc/maxAsis + 1);
                    index = 2;
                    //color = texture(ref_texture[2], realTex).rgb; 
                }
                	// POSITIVE Z - front
                else if(bool(isZpositive) && absZ >= absX && absZ >= absY)
                {
                    maxAsis = absZ;
                    uc = x;
                    vc = y;
                    realTex.x = 0.5 *(uc/maxAsis + 1);
                    realTex.y = 0.5 *(vc/maxAsis + 1);
                    index = 5;
                    //color = texture(ref_texture[5], realTex).rgb; 
                }
                	// NEGATIVE Z -back
                else if(!bool(isZpositive) && absZ >= absX && absZ >= absY)
                {
                    maxAsis = absZ;
                    uc = -x;
                    vc = y;
                    realTex.x = 0.5 *(uc/maxAsis + 1);
                    realTex.y = 0.5 *(vc/maxAsis + 1);
                    index = 4;
                    //color = texture(ref_texture[4], realTex).rgb;
                }
                
           return realTex;
}
vec3 CalcDirLight(multipleLight light)
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
   // vec3 I_ambient = light.ambient * texture(material.diffuse, realTex).rgb;   
    vec3 I_ambient = light.ambient;   

    if(IsUV)
    {
        //I_ambient = light.ambient * texture(material.grid, realTex).rgb;   
    }
    vec3    lightDir = normalize(-light.direction);
    float    diff = max(dot(norm, lightDir), 0.0);
    //vec3    I_diffuse = light.diffuse * diff * texture(material.diffuse, realTex).rgb;
    vec3    I_diffuse = light.diffuse * diff ;

    if(IsUV)
    {
        //I_diffuse = light.diffuse * diff * texture(material.grid, realTex).rgb;
    }
    vec3    reflectDir = 2*(dot(norm, lightDir)) * norm - lightDir;
    float    spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    //vec3    I_specular = light.specular * spec * texture(material.specular, realTex).rgb;
    vec3    I_specular = light.specular * spec;

    if(IsUV)
    {
        //I_specular = light.specular * spec * texture(material.grid, realTex).rgb;
    }
    if(IsPlane == true)
    {
        I_ambient = light.ambient;
        I_diffuse = light.diffuse * diff;
        I_specular = light.specular * spec;
    }

    return (I_ambient/16 + I_diffuse + I_specular);
}
vec3 CalPointLight(multipleLight light)
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

   // vec3 I_ambient = light.ambient * texture(material.diffuse, realTex).rgb;   
    vec3 I_ambient = light.ambient;   

    if(IsUV)
    {
        //I_ambient = light.ambient * texture(material.grid, realTex).rgb;
    }
    vec3    lightDir = normalize(light.position - FragPos);
    float    diff = max(dot(norm, lightDir), 0.0);
    //vec3    I_diffuse = light.diffuse * diff * texture(material.diffuse, realTex).rgb;
    vec3    I_diffuse = light.diffuse * diff ;

    if(IsUV)
    {
        //I_diffuse = light.diffuse * diff * texture(material.grid, realTex).rgb;
    }
    vec3    reflectDir = 2*(dot(norm, lightDir)) * norm - lightDir;
        
    float    spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    //vec3    I_specular = light.specular * spec * texture(material.specular, realTex).rgb;    
    vec3    I_specular = light.specular * spec;    
    
    if(IsUV)
    {
        //I_specular = light.specular * spec * texture(material.grid, realTex).rgb;
    }
    float distance = length(light.position-FragPos);
    float att = min(1.0/(light.constant + light.linear* distance + light.quadratic *(distance * distance)), 1.0);

    I_ambient *= att;
    I_diffuse *= att;
    I_specular *= att; 
   
    if(IsPlane == true)
    {
        I_ambient = light.ambient;
        I_diffuse = light.diffuse * diff;
        I_specular = light.specular * spec;
    }
    return (I_ambient/16 + I_diffuse + I_specular);
}
vec3 CalSpotLight(multipleLight light)
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    //vec3 I_ambient = light.ambient * texture(material.diffuse, realTex).rgb;
    vec3 I_ambient = light.ambient;

    if(IsUV)
    {
        //I_ambient = light.ambient * texture(material.grid, realTex).rgb;
    }   
    vec3   lightDir = normalize(light.position - FragPos);
    float    diff = max(dot(norm, lightDir), 0.0);
    //vec3    I_diffuse = light.diffuse * diff * texture(material.diffuse, realTex).rgb;
    vec3    I_diffuse = light.diffuse * diff;

    if(IsUV)
    {
        //I_diffuse = light.diffuse * diff * texture(material.grid, realTex).rgb;
    }
    vec3    reflectDir = 2*(dot(norm, lightDir)) * norm - lightDir;
    float    spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
   // vec3    I_specular = light.specular * spec * texture(material.specular, realTex).rgb;    
    vec3    I_specular = light.specular * spec;    

    if(IsUV)
    {
        //I_specular = light.specular * spec * texture(material.grid, realTex).rgb;
    }
    float spotlightEfect = 1.0f;
    float distance = length(light.position-FragPos);
    float att = min(1.0/(light.constant + light.linear* distance + light.quadratic *(distance * distance)), 1.0);

        I_ambient *= att;
        I_diffuse *= att;
        I_specular *= att; 

        vec3 lightUnit = normalize(light.direction);  //spotlight directoin vector    
        float alpha = dot(lightUnit, -lightDir);
        float radianTheta = light.theta * (3.14f/180.f);
        float radianPhi = light.phi * (3.14f/180.f);

        if(alpha > cos(radianTheta)) //inside inner angle
        {
            spotlightEfect = 1.0f;
        }
        else if(alpha < cos(radianPhi)) //outside outer angle
        {
            spotlightEfect = 0.f;
        }
        else
        {
            float result = (alpha-cos(radianPhi))/(cos(radianTheta)-cos(radianPhi));
            spotlightEfect = pow(result, light.falloff);
        } 

    if(IsPlane == true)
    {
        I_ambient = light.ambient;
        I_diffuse = light.diffuse * diff;
        I_specular = light.specular * spec;

    }
    vec3 result = (I_ambient/16) +spotlightEfect*(I_diffuse + I_specular);    
    return vec3(min(result.x, 1), min(result.y, 1), min(result.z, 1));
}
