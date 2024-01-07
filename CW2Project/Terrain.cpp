#include "Terrain.h"

ProceduralTerrain::ProceduralTerrain(float terrainFrequency, float biomeFrequency, float amplitude){
	// ---- Terrain noise setup ----
	// Perlin noise to create the terrain height variation
	terrainNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	terrainNoise.SetFrequency(terrainFrequency);
	terrainNoise.SetSeed(rand() & 100);

	// Perlin noise to create biome colouration
	biomeNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	biomeNoise.SetFrequency(biomeFrequency);
	biomeNoise.SetSeed(rand() % 100);

	// Setting the values of various parameters for the map
	const int trianglesPerSquare = 2;
	const int squaresRow = MAP_WIDTH - 1;
	const int trianglesGrid = squaresRow * squaresRow * trianglesPerSquare;
	
	// Start position for vertices
	float drawingStartPosition = 4.0f;
	float columnVerticesOffset = drawingStartPosition;
	float rowVerticesOffset = drawingStartPosition;

	int rowIndex = 0;

	// Creating array to store all the created vertices
	GLfloat terrainVertices[MAP_SIZE][9];

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

	// Calculating normals for each created vertex
	for (int y = 0; y < MAP_WIDTH; y++) {
		for (int x = 0; x < MAP_WIDTH; x++) {
			// Calculate normals using cross-products of adjacent triangles
			glm::vec3 normal(0.0f, 0.0f, 0.0f); 

			// Calculate normal for the current vertex
			if (x > 0 && y > 0 && x < MAP_WIDTH - 1 && y < MAP_WIDTH - 1) { 
				int iCurrent = y * MAP_WIDTH + x; 

				// Calculate normals for the surrounding triangles
				glm::vec3 v1(terrainVertices[iCurrent - MAP_WIDTH][0], terrainVertices[iCurrent - MAP_WIDTH][1], terrainVertices[iCurrent - MAP_WIDTH][2]); 
				glm::vec3 v2(terrainVertices[iCurrent - 1][0], terrainVertices[iCurrent - 1][1], terrainVertices[iCurrent - 1][2]); 
				glm::vec3 v3(terrainVertices[iCurrent][0], terrainVertices[iCurrent][1], terrainVertices[iCurrent][2]); 

				glm::vec3 edge1 = v2 - v1; 
				glm::vec3 edge2 = v3 - v1; 

				normal = glm::normalize(glm::cross(edge1, edge2)); 
			}

			// Set the normal vector for the current vertex
			int i = y * MAP_WIDTH + x; 
			terrainVertices[i][6] = normal.x; 
			terrainVertices[i][7] = normal.y; 
			terrainVertices[i][8] = normal.z; 
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Colours
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
}

// Draw function to draw all the triangles in this object
void ProceduralTerrain::Draw(Shader &shaderProgram) {
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, MAP_SIZE * 32, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}