#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glm/ext/vector_float3.hpp"

using namespace glm;

class Inputs {
public:
	void CheckInputs(GLFWwindow* WindowIn);

	vec3 GetMovement();

private:
	vec3 movement = vec3(0.0f);

	
};