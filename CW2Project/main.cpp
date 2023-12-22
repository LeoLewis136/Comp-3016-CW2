#include "ProgramManager.h"

ProgramManager mainProgram;

void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
	mainProgram.myCamera.MouseCallback(window, xpos, ypos);
}

int main() {
	glfwInit();

	mainProgram = ProgramManager();
	mainProgram.Setup(1280, 720, "Window");

	glfwSetCursorPosCallback(mainProgram.window, MouseCallback);

	mainProgram.MainLoop();

	return 0;
}

