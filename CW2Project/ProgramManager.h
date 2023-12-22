#pragma once

#include "shaders/LoadShaders.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "glm/ext/vector_float3.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "FastNoiseLite.h"

#include "Camera.h"
#include "Inputs.h"

using namespace std;
using namespace glm;

// Centralised program manager
class ProgramManager {
public:
	GLFWwindow* window;
	Camera myCamera;
	Inputs myInputs;

	// Program constructor
	ProgramManager();
	// Program destructor
	~ProgramManager();

	bool Setup(int width, int height, const char* windowName);
	void MainLoop();
	void LoadTexture(const char * textureName);

	static void FramebufferSizeCallback(GLFWwindow* window, int width, int height);

private:
	FastNoiseLite terrainNoise;
	FastNoiseLite biomeNoise;

	GLuint shaderProgram = NULL;
	GLuint VAO, VBO, EBO = NULL;

	GLuint texture;
	mat4 quadTransform = mat4(1.0f); // ---- Temporary ---- transform of the currently rendered quad

	

	// Time management
	float deltaTime; // The deltatime of the last frame
	float lastFrame; // The time of the last frame

	// ---- Main Loop functions ----
	void MoveCamera();
	void SetupProjection(int windowWidth, int windowHeight, float FOV);
	void BindCallbacks();
	
	float GetDeltaTime();

	// Geometry generation methods
	void SetupQuad();
	void GenerateTerrain();
};