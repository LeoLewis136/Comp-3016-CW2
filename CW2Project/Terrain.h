#pragma once

#define MAP_WIDTH 128 //Render width of map
#define MAP_SIZE MAP_WIDTH * MAP_WIDTH //Size of map in x & z space

#include <iostream>
#include <vector>

#include "glm/ext/vector_float3.hpp"
#include "learnopengl/shader_m.h"

#include "FastNoiseLite.h"

// Class which holds terrain data
class ProceduralTerrain {
public:
	ProceduralTerrain(float terrainFrequency, float biomeFrequency, float amplitude);
	void Draw(Shader& shader);
private:
	// Noise generators for terrain
	FastNoiseLite terrainNoise;
	FastNoiseLite biomeNoise;

	// OpenGl arrays
	unsigned int VAO, VBO, EBO;
};


