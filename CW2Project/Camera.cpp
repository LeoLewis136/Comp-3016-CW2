#include "Camera.h"

void Camera::CalculateProjection(GLuint shaderProgram) {
	//Transformations
	mat4 view = lookAt(position, position + forward, up);
	mat4 mvp = projection * view * model;
	int mvpLoc = glGetUniformLocation(shaderProgram, "mvpIn");

	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, value_ptr(mvp));
}

void Camera::MouseCallback(GLFWwindow* window, double xpos, double ypos) {
	//Initially no last positions, so sets last positions to current positions
	if (!mouseEntered)
	{
		lastXPos = (float)xpos;
		lastYPos = (float)ypos;
		mouseEntered = true;
	}

	//Sets values for change in position since last frame to current frame
	float xOffset = (float)xpos - lastXPos;
	float yOffset = lastYPos - (float)ypos;

	//Sets last positions to current positions for next frame
	lastXPos = (float)xpos;
	lastYPos = (float)ypos;

	//Moderates the change in position based on sensitivity value
	const float sensitivity = 0.025f;
	xOffset *= sensitivity;
	yOffset *= sensitivity;

	//Adjusts yaw & pitch values against changes in positions
	yaw += xOffset;
	pitch += yOffset;

	//Prevents turning up & down beyond 90 degrees to look backwards
	if (pitch > 89.0f)
	{
		pitch = 89.0f;
	}
	else if (pitch < -89.0f)
	{
		pitch = -89.0f;
	}

	//Modification of direction vector based on mouse turning
	vec3 direction;
	direction.x = cos(radians(yaw)) * cos(radians(pitch));
	direction.y = sin(radians(pitch));
	direction.z = sin(radians(yaw)) * cos(radians(pitch));
	forward = normalize(direction);

	// Calculate the new up vector based on the current forward and world up vector
	vec3 worldUp = vec3(0.0f, 1.0f, 0.0f); 
	vec3 right = normalize(cross(worldUp, forward)); 
	up = normalize(cross(forward, right)); 
}

// ---- Constructors ----
Camera::Camera(int windowWidth, int windowHeight, float FOV) {
	//Model matrix
	model = mat4(1.0f);
	//Scaling to zoom in
	model = scale(model, vec3(2.0f, 2.0f, 2.0f));
	//Rotation to look down
	model = rotate(model, radians(0.0f), vec3(1.0f, 0.0f, 0.0f));
	//Movement to position further back
	model = translate(model, vec3(0.0f, -2.0f, -1.5f));

	//Projection matrix
	projection = perspective(radians(FOV), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
}

Camera::Camera() {}