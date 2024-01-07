#include "Camera.h"

void Camera::CalculateProjection(Shader& shaderProgram) {
	

	//Transformations
	mat4 view = lookAt(position, position + forward, up);
	mat4 mvp = projection * view * model;
	shaderProgram.setMat4("mvpIn", mvp);
	shaderProgram.setVec3("camPos", position);

	//glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, value_ptr(mvp));
}

void Camera::RotateModel(vec3 rotationAxis, float rotationAmount, Shader shaderProgram) {
	model = rotate(model, rotationAmount, rotationAxis);
	CalculateProjection(shaderProgram);
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

	//Projection matrix
	projection = perspective(radians(FOV), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
}

// Restore the model back to original scaling
void Camera::ResetModel(Shader shaderProgram) {
	// Create a new model and do MVP to it
	model = mat4(1.0f);
	CalculateProjection(shaderProgram);
}

void Camera::MoveModel(vec3 _translation, Shader shaderProgram) {
	model = translate(model, _translation);
	CalculateProjection(shaderProgram);
}

// Simple scale the entire model
void Camera::Scale(vec3 _scale, Shader shaderProgram) {
	// Scale the current model scale
	model = scale(model, _scale);
	CalculateProjection(shaderProgram);
}

// Set the scale of the model to a specific scale
void Camera::SetScale(vec3 _scale, Shader shaderProgram) {
	// Reset the scale of the model before setting to the desired value
	ResetModel(shaderProgram);
	model = scale(model, _scale);
	CalculateProjection(shaderProgram);
}

Camera::Camera() {}