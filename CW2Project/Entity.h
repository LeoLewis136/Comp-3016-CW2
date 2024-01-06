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
	Entity(Camera &_mainScene, const string fileLocation);
	Entity(Camera& _mainScene, const string fileLocation, vec3 position, vec3 rotation, vec3 scale);
	Entity(Camera& _mainScene, float biomeFrequency, float terrainFrequency, float amplitude, vec3 position, vec3 rotation, vec3 scale);



	void setPosition(Shader &shaderProgram, vec3 newPosition);
	void rotate(Shader& shaderProgram, vec3 rotationAmount);
	void setScale(Shader& shaderProgram, vec3 newScale);

	void Draw(Shader& shaderProgram);
private:
	vec3 position;
	vec3 rotation;
	vec3 scale;

	Model* myModel = nullptr;
	ProceduralTerrain* myTerrain = nullptr;
	Camera& mainScene;
};