#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "glm/ext/vector_float3.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "learnopengl/shader_m.h"

using namespace glm;

class Camera {
public:
	mat4 model;

	vec3 position = vec3(0.0f, 0.0f, 0.0f); // The "Position" of the camera
	vec3 forward = vec3(0.0f, 0.0f, -1.0f); // Where the camera is facing
	vec3 up = vec3(0.0f, 1.0f, 0.0f); // The up direction of the camera

	float yaw = -90.0f;
	float pitch = 0.0f;

	float lastXPos;
	float lastYPos;
	bool mouseEntered = false;

	Camera();
	Camera(int windowWidth, int windowHeight, float FOV);

	void RotateModel(vec3 _rotationAxis, float rotation, Shader shaderProgram);
	void MoveModel(vec3 _translation, Shader shaderProgram);
	void ResetModel(Shader shaderProgram);
	void Scale(vec3 _scale, Shader shaderProgram);
	void SetScale(vec3 _scale, Shader shaderProgram);
	void CalculateProjection(Shader shaderProgram);
	void MouseCallback(GLFWwindow* window, double xpos, double ypos);
private:
	mat4 projection;
	
};