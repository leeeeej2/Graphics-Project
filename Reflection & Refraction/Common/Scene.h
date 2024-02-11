/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: Scene.h
Purpose: source file for scene and it controls the derived class.
Language: C++
Platform: VisualStudio 2019 / Window
Project: uijin.lee_CS300_3
Author: Uijin Lee, uijn.lee, 180003221
Creation date: Nov 11st, 2021
End Header --------------------------------------------------------*/
#ifndef SAMPLE3_2_FBO_SCENE_H
#define SAMPLE3_2_FBO_SCENE_H

#include <GL/glew.h>
#include <GLFW/glfw3.h>

//#include <nanogui/nanogui.h>

//#include "VertexDataManager.h"


#define _GET_GL_ERROR   { GLenum err = glGetError(); std::cout << "[OpenGL Error] " << glewGetErrorString(err) << std::endl; }

class Scene
{

public:
    Scene();
    Scene( int windowWidth, int windowHeight );
    virtual ~Scene();

    // Public methods

    // Init: called once when the scene is initialized
    virtual int Init();

    // LoadAllShaders: This is the placeholder for loading the shader files
    virtual void LoadAllShaders();

    // Display : encapsulates per-frame behavior of the scene
    virtual int Display();

    // preRender : called to setup stuff prior to rendering the frame
    virtual int preRender();

    // Render : per frame rendering of the scene
    virtual int Render();

    // postRender : Any updates to calculate after current frame
    virtual int postRender();

    // cleanup before destruction
    virtual void CleanUp();

protected:
    int _windowHeight, _windowWidth;

    // Common functionality for all scenes
//    SceneManager                    _scnManager;
//    std::vector<VertexDataManager>   VAOs;

};


#endif //SAMPLE3_2_FBO_SCENE_H
