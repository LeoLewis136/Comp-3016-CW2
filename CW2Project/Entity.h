#pragma once

#include "Camera.h"
#include "Terrain.h"

// GLM imports
#include "glm/ext/vector_float3.hpp"
#include <glm/ext/matrix_transform.hpp>

// OpenGL
#include "learnopengl/model.h"

using namespace glm;

class Entity {
public:
	// Constructors
	Entity(Camera &_mainScene, const string fileLocation);
	Entity(Camera& _mainScene, const string fileLocation, vec3 position, vec3 rotation, vec3 scale);
	Entity(Camera& _mainScene, float biomeFrequency, float terrainFrequency, float amplitude, vec3 position, vec3 rotation, vec3 scale);

	// Model parameters
	vec3 position;
	vec3 rotation;
	vec3 scale;

	// Allowing parameters to be set correctly outside object
	void setPosition(Shader &shaderProgram, vec3 newPosition);
	void rotate(Shader& shaderProgram, vec3 rotationAmount);
	void setScale(Shader& shaderProgram, vec3 newScale);

	void Draw(Shader& shaderProgram);
private:
	// The model for visuals of a model entity
	Model* myModel = nullptr;
	// The terrain object for a terrain entity
	ProceduralTerrain* myTerrain = nullptr;
	// Storing a reference to the camera
	Camera& mainScene;
};