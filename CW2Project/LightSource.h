#pragma once

#include "Camera.h"

// GLM imports
#include "glm/ext/vector_float3.hpp"
#include <glm/ext/matrix_transform.hpp>

// OpenGL
#include "learnopengl/model.h"

// Light source object
class LightSource {
public:
	vec3 position;
	vec3 rotation;
	vec3 scale;

	LightSource(Camera& _mainScene, const string fileLocation, vec3 position, vec3 rotation, vec3 scale);
	void Draw(Shader &shaderProgram);
private:
	

	Model* myModel = nullptr;
	Camera& mainScene;
};