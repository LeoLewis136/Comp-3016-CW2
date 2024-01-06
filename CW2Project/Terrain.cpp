#include "Terrain.h"

ProceduralTerrain::ProceduralTerrain(float terrainFrequency, float biomeFrequency, float amplitude) {
	// ---- Terrain noise setup ----
	terrainNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	terrainNoise.SetFrequency(terrainFrequency);
	terrainNoise.SetSeed(rand() & 100);

	biomeNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	biomeNoise.SetFrequency(biomeFrequency);
	biomeNoise.SetSeed(rand() % 100);

	const int trianglesPerSquare = 2;
	const int squaresRow = MAP_WIDTH - 1;
	const int trianglesGrid = squaresRow * squaresRow * trianglesPerSquare;
	


	float drawingStartPosition = 1.0f;
	float columnVerticesOffset = drawingStartPosition;
	float rowVerticesOffset = drawingStartPosition;

	int rowIndex = 0;

	GLfloat terrainVertices[MAP_SIZE][6];

	for (int i = 0; i < MAP_SIZE; i++)
	{
		//Generation of x & z vertices for horizontal plane
		terrainVertices[i][0] = columnVerticesOffset;
		terrainVertices[i][2] = rowVerticesOffset;

		//Shifts x position across for next triangle along grid
		columnVerticesOffset = columnVerticesOffset + -0.0625f;

		//Indexing of each chunk within row
		rowIndex++;
		//True when all triangles of the current row have been generated
		if (rowIndex == MAP_WIDTH)
		{
			//Resets for next row of triangles
			rowIndex = 0;
			//Resets x position for next row of triangles
			columnVerticesOffset = drawingStartPosition;
			//Shifts y position
			rowVerticesOffset = rowVerticesOffset + -0.0625f;
		}
	}

	//Generation of height map indices
	GLuint terrainIndices[trianglesGrid][3];

	//Positions to start mapping indices from
	int columnIndicesOffset = 0;
	int rowIndicesOffset = 0;

	//Generation of map indices in the form of chunks (1x1 right angle triangle squares)
	rowIndex = 0;
	for (int i = 0; i < trianglesGrid - 1; i += 2)
	{
		terrainIndices[i][0] = columnIndicesOffset + rowIndicesOffset; //top left
		terrainIndices[i][2] = MAP_WIDTH + columnIndicesOffset + rowIndicesOffset; //bottom left
		terrainIndices[i][1] = 1 + columnIndicesOffset + rowIndicesOffset; //top right

		terrainIndices[i + 1][0] = 1 + columnIndicesOffset + rowIndicesOffset; //top right
		terrainIndices[i + 1][2] = MAP_WIDTH + columnIndicesOffset + rowIndicesOffset; //bottom left
		terrainIndices[i + 1][1] = 1 + MAP_WIDTH + columnIndicesOffset + rowIndicesOffset; //bottom right

		//Shifts x position across for next chunk along grid
		columnIndicesOffset = columnIndicesOffset + 1;

		//Indexing of each chunk within row
		rowIndex++;

		//True when all chunks of the current row have been generated
		if (rowIndex == squaresRow)
		{
			//Resets for next row of chunks
			rowIndex = 0;
			//Resets x position for next row of chunks
			columnIndicesOffset = 0;
			//Shifts y position
			rowIndicesOffset = rowIndicesOffset + MAP_WIDTH;
		}
	}


	//Terrain vertice index
	int i = 0;
	//Using x & y nested for loop in order to apply noise 2-dimensionally
	for (int y = 0; y < MAP_WIDTH; y++)
	{
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			//Retrieval of biome to set
			float biomeValue = biomeNoise.GetNoise((float)x, (float)y);
			if (biomeValue <= -0.0f) //Standard Desert
			{
				terrainVertices[i][3] = 0.94f;
				terrainVertices[i][4] = 0.57f;
				terrainVertices[i][5] = 0.32f;
			}
			else //Darker Desert
			{
				terrainVertices[i][3] = 0.77f;
				terrainVertices[i][4] = 0.43f;
				terrainVertices[i][5] = 0.23f;
			}

			//Setting of height from 2D noise value at respective x & y coordinate
			terrainVertices[i][1] = terrainNoise.GetNoise((float)x, (float)y) * amplitude;
			i++;
		}
	}

	// Create Vertex Array Object, Vertex Buffer Object, and Element Buffer Object
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(terrainVertices), terrainVertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(terrainIndices), terrainIndices, GL_STATIC_DRAW);

	// Allocation & indexing of vertex attribute memory for vertex shader
	// Positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Colours
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
}

void ProceduralTerrain::Draw(Shader &shaderProgram) {
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, MAP_SIZE * 32, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

// Function to generate terrian vertices
/*void GenerateTerrain() {
	float frequency = 0.02f;
	float amplitude = 0.6f;

	// Vertices and Indicies setup
	const int squaresRow = RENDER_DISTANCE - 1;
	const int traianglesPerSquare = 2;
	const int trianglesGrid = squaresRow * squaresRow * traianglesPerSquare;

	
}
*/