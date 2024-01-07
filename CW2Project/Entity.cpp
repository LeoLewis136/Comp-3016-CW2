#include "Entity.h"


Entity::Entity(Camera &_mainScene, const string fileLocation) : mainScene(_mainScene) {
	// Load the model given to this object
	myModel = new Model(fileLocation);

	// Set placeholder values
	position = vec3(0.0f);
	scale = vec3(1.0f);
	rotation = vec3(0.0f);
}

Entity::Entity(Camera& _mainScene, const string fileLocation, vec3 _position, vec3 _rotation, vec3 _scale) : mainScene(_mainScene) {
	// Load the model given to this object
	myModel = new Model(fileLocation);

	position = _position;
	scale = _scale;
	rotation = _rotation;
}

// Constructor for procedural terrain
Entity::Entity(Camera& _mainScene, float biomeFrequency, float terrainFrequency, float amplitude, vec3 _position, vec3 _rotation, vec3 _scale) : mainScene(_mainScene) {
	// Create a new terrain
	myTerrain = new ProceduralTerrain(terrainFrequency, biomeFrequency, amplitude);

	position = _position;
	scale = _scale;
	rotation = _rotation;
}

void Entity::Draw(Shader& shaderProgram) {
	// Reset the model matrix to 1
	mainScene.ResetModel(shaderProgram);

	// Move the model into its correct position for each fram
	mainScene.MoveModel(position, shaderProgram);
	// Rotate the object the correct amount around each axis for each frame
	for (int i = 0; i < 3; i++) { // Loop through each rotation axis 
		if (rotation[i] != 0) {
			// Loop the rotation back to 0 when it reaches 360
			rotation[i] = fmod(rotation[i], 359.9);
			// Find the correct axis before rotating it the correct amount
			vec3 temp = vec3(0.0f);
			temp[i] = 1.0f;

			// Rotate the correct amount
			mainScene.RotateModel(temp, radians(rotation[i]), shaderProgram);
		}
	}
	// Scaling the object to the correct scale
	mainScene.Scale(scale, shaderProgram);

	shaderProgram.setMat4("modelIn", mainScene.model);

	if (myModel != nullptr) {
		// Draw the model at this setup position, scale and rotation
		myModel->Draw(shaderProgram);
	}
	else {
		// Draw terrain at the setup position, scale and rotation
		myTerrain->Draw(shaderProgram);
	}
		
}