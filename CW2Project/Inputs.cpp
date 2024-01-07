#include "Inputs.h"
#include <iostream>

// Get or Set methods
vec3 Inputs::GetMovement() { return movement; }

void Inputs::CheckInputs(GLFWwindow* windowIn, bool& gui_active) {
	//Closes window on 'exit' key press
	if (glfwGetKey(windowIn, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(windowIn, true);
		
	}

	// Unlocks / locks the cursor on space_bar press
	if (glfwGetKey(windowIn, GLFW_KEY_SPACE) == GLFW_PRESS && space_pressed == false) {
		glfwSetInputMode(windowIn, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		gui_active = true;
	}

	movement = vec3(0.0f); // Reset movement to 0
	
	// Forward and backwards (W & S)
	if (glfwGetKey(windowIn, GLFW_KEY_W) == GLFW_PRESS) {
		movement.z = 1;
	}
	if (glfwGetKey(windowIn, GLFW_KEY_S) == GLFW_PRESS) {
		movement.z = -1;
	}

	// Left and right (A & D)
	if (glfwGetKey(windowIn, GLFW_KEY_A) == GLFW_PRESS) {
		movement.x = -1;
	}
	if (glfwGetKey(windowIn, GLFW_KEY_D) == GLFW_PRESS) {
		movement.x = 1;
	}

	// Up and down (Q & E)
	if (glfwGetKey(windowIn, GLFW_KEY_Q) == GLFW_PRESS) {
		movement.y = -1;
	}
	if (glfwGetKey(windowIn, GLFW_KEY_E) == GLFW_PRESS) {
		movement.y = 1;
	}
}