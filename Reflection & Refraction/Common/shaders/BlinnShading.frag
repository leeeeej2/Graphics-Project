/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: BlinnShading.frag
Purpose: BlinnShading.frag
Language: glsl
Platform: VisualStudio 2019 / Window
Project: uijin.lee_CS300_2
Author: Uijin Lee, uijn.lee, 180003221
Creation date: Nov 11st, 2021
End Header --------------------------------------------------------*/
#version 400 core

struct Material
{
    sampler2D grid;
	sampler2D diffuse;
	sampler2D specular;
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

uniform Material material; 
uniform vec3 viewPos;
//uniform int lightType;

uniform vec3 I_emmisive;
uniform vec3 globalAmbient;
uniform vec3 Ka;

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
vec2 realTex;
void main()
{
    vec3 combine;
    realTex = TexCoords;
    if(IsUV)
    {
        if(IsGPU != 4)
        {
            vec3 normal = normalize(uvNormal);
            vec3 centroidVec = uvPos;
            if(Entity)
            {
                centroidVec = normal;
            }

            if(IsGPU == 0)
            {
                float theta_ = degrees(atan(centroidVec.z, centroidVec.x));
                theta_ += 180;

                realTex.x = theta_/360;
                realTex.y = (centroidVec.y - min_.y)/(max_.y - min_.y);
            } //cylindrical
            if(IsGPU == 1)
            {
                float theta_ = degrees(atan(centroidVec.z, centroidVec.x));
                theta_ += 180;
                float phi_ = degrees(acos(centroidVec.y/sqrt(pow(centroidVec.x, 2) + pow(centroidVec.y, 2) + pow(centroidVec.z, 2))));

                realTex.x = theta_/360;
                realTex.y = 1 - (phi_/180);
            } //spherical
            if(IsGPU == 2)
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

                if(bool(isXpositive) && (absX >= absY) && (absX >= absZ))
                {
                    maxAsis = absX;
                    uc = -z;
                    vc = y;
                }
                else if(!bool(isXpositive) && absX >= absY && absX >= absZ)
                {
                    maxAsis = absX;
                    uc = z;
                    vc = y;
                }
                else if(bool(isYpositive) && absY >= absX && absY >= absZ)
                {
                    maxAsis = absY;
                    uc = x;
                    vc = -z;
                }
                else if(!bool(isYpositive) && absY >= absX && absY >= absZ)
                {
                    maxAsis = absY;
                    uc = x;
                    vc = z;
                }
                else if(bool(isZpositive) && absZ >= absX && absZ >= absY)
                {
                    maxAsis = absZ;
                    uc = x;
                    vc = y;
                }
                else if(!bool(isZpositive) && absZ >= absX && absZ >= absY)
                {
                    maxAsis = absZ;
                    uc = -x;
                    uc = y;
                }
                realTex.x = 0.5 *(uc/maxAsis + 1);
                realTex.y = 0.5 *(vc/maxAsis + 1);
            } //cube
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
    FragColor = vec4(result, 1.0);
}

vec3 CalcDirLight(multipleLight light)
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 I_ambient = light.ambient * texture(material.diffuse, realTex).rgb;   
    if(IsUV)
    {
        I_ambient = light.ambient * texture(material.grid, realTex).rgb;   
    }
    vec3    lightDir = normalize(-light.direction);
    float    diff = max(dot(norm, lightDir), 0.0);
    vec3    I_diffuse = light.diffuse * diff * texture(material.diffuse, realTex).rgb;
    if(IsUV)
    {
        I_diffuse = light.diffuse * diff * texture(material.grid, realTex).rgb;
    }
    vec3 halfVector = normalize(viewDir + lightDir);
    float    spec = pow(max(dot(viewDir, halfVector), 0.0), material.shininess);
    vec3    I_specular = light.specular * spec * texture(material.specular, realTex).rgb;
    if(IsUV)
    {
        I_specular = light.specular * spec * texture(material.grid, realTex).rgb;
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

    vec3 I_ambient = light.ambient * texture(material.diffuse, realTex).rgb;   
    if(IsUV)
    {
        I_ambient = light.ambient * texture(material.grid, realTex).rgb;
    }
    vec3    lightDir = normalize(light.position - FragPos);
    float    diff = max(dot(norm, lightDir), 0.0);
    vec3    I_diffuse = light.diffuse * diff * texture(material.diffuse, realTex).rgb;
    if(IsUV)
    {
        I_diffuse = light.diffuse * diff * texture(material.grid, realTex).rgb;
    }
    vec3 halfVector = normalize(viewDir + lightDir);
    float    spec = pow(max(dot(viewDir, halfVector), 0.0), material.shininess);
    vec3    I_specular = light.specular * spec * texture(material.specular, realTex).rgb;    
    if(IsUV)
    {
        I_specular = light.specular * spec * texture(material.grid, realTex).rgb;
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

    vec3 I_ambient = light.ambient * texture(material.diffuse, realTex).rgb;
    if(IsUV)
    {
        I_ambient = light.ambient * texture(material.grid, realTex).rgb;
    }   
    vec3   lightDir = normalize(light.position - FragPos);
    float    diff = max(dot(norm, lightDir), 0.0);
    vec3    I_diffuse = light.diffuse * diff * texture(material.diffuse, realTex).rgb;
    if(IsUV)
    {
        I_diffuse = light.diffuse * diff * texture(material.grid, realTex).rgb;
    }
    vec3 halfVector = normalize(viewDir + lightDir);
    float    spec = pow(max(dot(viewDir, halfVector), 0.0), material.shininess);
    vec3    I_specular = light.specular * spec * texture(material.specular, realTex).rgb;    
    if(IsUV)
    {
        I_specular = light.specular * spec * texture(material.grid, realTex).rgb;
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
