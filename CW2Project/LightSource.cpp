#include "LightSource.h"

LightSource::LightSource(Camera& _mainScene, const string fileLocation, vec3 _position, vec3 _rotation, vec3 _scale) : mainScene(_mainScene) {
	myModel = new Model(fileLocation);

	position = _position;
	scale = _scale;
	rotation = _rotation;
}

void LightSource::Draw(Shader& shaderProgram) {
	// Reset the model matrix to 1
	mainScene.ResetModel(shaderProgram);

	// Move the model into its correct position for each fram
	mainScene.MoveModel(position, shaderProgram);
	// Rotate the object the correct amount around each axis for each frame
	for (int i = 0; i < 3; i++) { // Loop through each rotation axis 
		if (rotation[i] != 0) {

			// Find the correct axis before rotating it the correct amount
			vec3 temp = vec3(0.0f);

			temp[i] = 1.0f;
			mainScene.RotateModel(temp, radians(rotation[i]), shaderProgram);
		}
	}
	// Scaling the object to the correct scale
	mainScene.Scale(scale, shaderProgram);

	// Draw the model at this setup position, scale and rotation
	myModel->Draw(shaderProgram);
}