/* Start Header -------------------------------------------------------
Copyright (C) 2021 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the prior written
consent of DigiPen Institute of Technology is prohibited.
File Name: GLApplication.cpp
Purpose: Run the whole program to render
Language: C++
Platform: VisualStudio 2019 / Window
Project: uijin.lee_CS300_2
Author: Uijin Lee, uijn.lee, 180003221
Creation date: Nov 11st, 2021
End Header --------------------------------------------------------*/
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include <Windows.h>
// Include standard headers
#include <cstdio>
#include <vector>

// Include GLEW
#include <GL/glew.h>

// Include GLFW
#include <GLFW/glfw3.h>

// Include GLM
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

// Local / project headers
#include "../Common/Scene.h"
#include "shader.hpp"
#include "../Common/OBJLoader.h"
#include "../Common/SceneManager.h"
//////////////////////////////////////////////////////////////////////

GLFWwindow* window;
Scene* scene;

int Widthwindow = 1024;
int Heightwindow = 768;
Camera cam(glm::vec3(0.f, 0.f, 5.f), glm::vec3(0, 0.f, -1.f), glm::vec3(0, 1, 0), -90.f, 0.f);

float dt = 0.f;
float pre = 0.f;

void inputProcess(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);		
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cam.KeyboardMove('w', dt);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cam.KeyboardMove('s', dt);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cam.KeyboardMove('a', dt);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cam.KeyboardMove('d', dt);
}

///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////
int main()
{
	// Initialise GLFW
	if (!glfwInit())
	{
		fprintf(stderr, "Failed to initialize GLFW\n");
		(void)getchar();
		return -1;
	}

	// Setting up OpenGL properties
	glfwWindowHint(GLFW_SAMPLES, 1); // change for anti-aliasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	 
	// Open a window and create its OpenGL context
	window = glfwCreateWindow(Widthwindow, Heightwindow, // window dimensions
		"cs350_uijin.lee_Assignment1", // window title
		nullptr, // which monitor (if full-screen mode)
		nullptr); // if sharing context with another window
	if (window == nullptr)
	{
		fprintf(stderr,
			"Failed to open GLFW window. Check if your GPU is 4.0 compatible.\n");
		(void)getchar();
		glfwTerminate();
		return -1;
	}

	// OpenGL resource model - "glfwCreateWindow" creates the necessary data storage for the OpenGL
	// context but does NOT make the created context "current". We MUST make it current with the following
	// call
	glfwMakeContextCurrent(window);

	// Initialize GLEW
	glewExperimental = static_cast<GLboolean>(true); // Needed for core profile
	if (glewInit() != GLEW_OK)
	{
		fprintf(stderr, "Failed to initialize GLEW\n");
		(void)getchar();
		glfwTerminate();
		return -1;
	}

	// Ensure we can capture the escape key being pressed below
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	// Initialize the scene
	scene = new SceneManager(Widthwindow, Heightwindow);

	//scenemanager = new SceneManager();
	// Scene::Init encapsulates setting up the geometry and the texture
	// information for the scene
	scene->Init();
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 400");

	while(!glfwWindowShouldClose(window))
	{
		float current = glfwGetTime();
		dt = current - pre;
		pre = current;

		// Now render the scene
		// Scene::Display method encapsulates pre-, render, and post- rendering operations
		inputProcess(window);
		scene->Display();
	

		ImGui::Render();

		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap buffers
		glfwSwapBuffers(window);
		glfwPollEvents();
	}


	scene->CleanUp();
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	// Close OpenGL window and terminate GLFW
	glfwTerminate();

	return 0;
}

//////////////////////////////////////////////////////
//////////////////////////////////////////////////////

