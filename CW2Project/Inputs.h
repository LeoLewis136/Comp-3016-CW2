#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glm/ext/vector_float3.hpp"

using namespace glm;

// Object to manage player inputs
class Inputs {
public:
	// Check for player inputs
	void CheckInputs(GLFWwindow* WindowIn, bool& gui_active);

	// Allow external objects to check what the current movement amount is
	vec3 GetMovement();

private:
	bool space_pressed;
	vec3 movement = vec3(0.0f);

	
};