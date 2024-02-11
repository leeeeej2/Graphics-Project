/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: LightPass_phong.frag
Purpose: get value from GBuffer and implement phong shading
Language: C++
Platform: VisualStudio 2019 / Window
Project: uijin.lee_CS350_1
Author: Uijin Lee, uijn.lee, 180003221
Creation date: Feb 6st, 2022
End Header --------------------------------------------------------*/
#version 400 core
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
 	vec3 position;//16	//48 //position
 	vec3 ambient;//16	//64
 	vec3 diffuse;//16	//80 //color
 	vec3 specular;//16	//96
 };

 layout (std140) uniform Light
 {   
    multipleLight LightSources[16];
 };

out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D gPos;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

//uniform Material material; 
uniform vec3 viewPos;
uniform int type;

uniform float shininess;

uniform vec3 I_emmisive;
uniform vec3 globalAmbient;
uniform vec3 Ka;

//uniform bool IsPlane;
uniform int activeLight;

//in vec3 Normal;
//in vec3 FragPos;

//uniform bool IsUV;
//uniform int IsGPU;
//uniform bool Entity;
uniform vec3 min_;
uniform vec3 max_;

uniform vec3 fogcolor;
uniform float fogMin;
uniform float fogMax;

vec3 CalcDirLight(multipleLight light);
vec3 CalPointLight(multipleLight light);
vec3 CalSpotLight(multipleLight light);

vec2 realTex;
vec3 FragPos;
vec3 Normal;
vec3 diffuse;
float specular;

void main()
{
    vec3 combine;
    realTex = TexCoords;        

    FragPos = texture(gPos, TexCoords).rgb;
    Normal = texture(gNormal, TexCoords).rgb;
    diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    specular = texture(gAlbedoSpec, TexCoords).a;

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
    //if(IsPlane)
    {
    //combine += globalAmbient;
    }
    //else{
    combine += I_emmisive + globalAmbient*Ka;
    //}

    //fog
    float s = (fogMax - length(viewPos))/(fogMax - fogMin);
    vec3 result = s * combine + (1 - s) * fogcolor;
    if(type == 0)
    {
        FragColor = vec4(result, 1.0);
    }
    else if(type == 1) //gpos
    {
        FragColor = vec4(FragPos, 1.0);
    }
    else if(type == 2)
    {
        FragColor = vec4(Normal, 1.0);
    }
    else if(type == 3)
    {
        FragColor = vec4(diffuse, 1.0);
    }
    else if(type == 4)
    {
        FragColor = vec4(vec3(specular),1.0);
    }
}

vec3 CalcDirLight(multipleLight light)
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 I_ambient = light.ambient * diffuse;   
    //if(IsUV)
    {
        //I_ambient = light.ambient * texture(material.grid, realTex).rgb;   
    }
    vec3    lightDir = normalize(-light.direction);
    float    diff = max(dot(norm, lightDir), 0.0);
    vec3    I_diffuse = light.diffuse * diff * diffuse;
    //if(IsUV)
    {
        //I_diffuse = light.diffuse * diff * texture(material.grid, realTex).rgb;
    }
    vec3    reflectDir = 2*(dot(norm, lightDir)) * norm - lightDir;
    float    spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3    I_specular = light.specular * spec * specular;
    //if(IsUV)
    {
        //I_specular = light.specular * spec * texture(material.grid, realTex).rgb;
    }
    //if(IsPlane == true)
    {
        //I_ambient = light.ambient;
        //I_diffuse = light.diffuse * diff;
        //I_specular = light.specular * spec;
    }

    return (I_ambient/16 + I_diffuse + I_specular);
}
vec3 CalPointLight(multipleLight light)
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 I_ambient = light.ambient * diffuse;   
    //if(IsUV)
    {
        //I_ambient = light.ambient * texture(material.grid, realTex).rgb;
    }
    vec3    lightDir = normalize(light.position - FragPos);
    float    diff = max(dot(norm, lightDir), 0.0);
    vec3    I_diffuse = light.diffuse * diff * diffuse;
    //if(IsUV)
    {
        //I_diffuse = light.diffuse * diff * texture(material.grid, realTex).rgb;
    }
    vec3    reflectDir = 2*(dot(norm, lightDir)) * norm - lightDir;
        
    float    spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3    I_specular = light.specular * spec * specular;    
    //if(IsUV)
    {
        //I_specular = light.specular * spec * texture(material.grid, realTex).rgb;
    }
    float distance = length(light.position-FragPos);
    float att = min(1.0/(light.constant + light.linear* distance + light.quadratic *(distance * distance)), 1.0);

    I_ambient *= att;
    I_diffuse *= att;
    I_specular *= att; 
   
    //if(IsPlane == true)
    {
        //I_ambient = light.ambient;
        //I_diffuse = light.diffuse * diff;
        //I_specular = light.specular * spec;
    }
    return (I_ambient/16 + I_diffuse + I_specular);
}
vec3 CalSpotLight(multipleLight light)
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 I_ambient = light.ambient * diffuse;
    //if(IsUV)
    {
        //I_ambient = light.ambient * texture(material.grid, realTex).rgb;
    }   
    vec3   lightDir = normalize(light.position - FragPos);
    float    diff = max(dot(norm, lightDir), 0.0);
    vec3    I_diffuse = light.diffuse * diff * diffuse;
    //if(IsUV)
    {
        //I_diffuse = light.diffuse * diff * texture(material.grid, realTex).rgb;
    }
    vec3    reflectDir = 2*(dot(norm, lightDir)) * norm - lightDir;
    float    spec = pow(max(dot(viewDir, reflectDir), 0.0), shininess);
    vec3    I_specular = light.specular * spec * specular;    
    //if(IsUV)
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

    //if(IsPlane == true)
    {
        //I_ambient = light.ambient;
        //I_diffuse = light.diffuse * diff;
        //I_specular = light.specular * spec;
    }
    vec3 result = (I_ambient/16) +spotlightEfect*(I_diffuse + I_specular);    
    return vec3(min(result.x, 1), min(result.y, 1), min(result.z, 1));
}
