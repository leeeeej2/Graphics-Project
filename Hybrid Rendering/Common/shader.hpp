/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: shader.hpp
Purpose: Create shader for render
Language: C++
Platform: VisualStudio 2019 / Window
Project: uijin.lee_CS300_2
Author: Uijin Lee, uijn.lee, 180003221
Creation date: Nov 11st, 2021
End Header --------------------------------------------------------*/
#ifndef SHADER_HPP
#define SHADER_HPP

enum class Primitive_Enum
{
    TriangleStrip = 0,
    Points,
    LineStrip
};

///////////////////
GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

///////////////////
GLuint LoadPipeline(const char * vertex_file_path,const char * fragment_file_path, GLuint * programIDs );

// Load shaders where multiple shader files == one complete shader
// Show how code can be reused across shaders
GLuint LoadMultiShaders(const char *vertex_file_path, const char *fragment_file_path,
                        const char *geom_file_path, Primitive_Enum  out_primitive_type );


#endif
