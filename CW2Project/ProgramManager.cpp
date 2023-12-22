#include "ProgramManager.h"

#include "stb_image.h"

#define RENDER_DISTANCE 128
#define MAP_SIZE RENDER_DISTANCE * RENDER_DISTANCE

ProgramManager::ProgramManager() : window(nullptr) {}

// Safely destroy anything required
ProgramManager::~ProgramManager() {
}

bool ProgramManager::Setup(int width, int height, const char* windowName) {



	// --- Window ---
	// Initialise GLFW window object and check for errors
	window = glfwCreateWindow(width, height, windowName, NULL, NULL);
	if (!window) {
		// Handle unsucsessful window creation
		cerr << "Failed to create GLFW window\n";
		glfwTerminate();
		return false;
	}

	// Bind cursor to window 
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Bind OpenGL to window
	glfwMakeContextCurrent(window);
	glewInit();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// --- Shaders ---
	// Shaders to load
	ShaderInfo shaders[] = {
		{GL_VERTEX_SHADER, "shaders/vertexShader.vert", 0},
		{GL_FRAGMENT_SHADER, "shaders/fragmentShader.frag", 0},
		{GL_NONE, NULL, 0}
	};

	// Load shaders and setup use of shaders
	shaderProgram = LoadShaders(shaders);
	glUseProgram(shaderProgram);

	// --- Misc ---
	// Loading a texture to use
	LoadTexture("media/woodPlanks.jpg");
	// Create test mesh
	//SetupQuad();

	GenerateTerrain();

	// Setup camera object to manage the world "Camera" 	
	myCamera = Camera(width, height, 45.0f);
	myInputs = Inputs();

	// Successful loading
	return true;
}

void ProgramManager::MoveCamera()
{
	vec3 currentMovement = myInputs.GetMovement();
	//Extent to which to move in one instance
	const float movementSpeed = 3.0f * deltaTime;
	
	cout << currentMovement.x << " " << currentMovement.y << " " << currentMovement.z << "\n";

	//WASD controls
	if (currentMovement.z != 0.0f){ myCamera.position += movementSpeed * (myCamera.forward * currentMovement.z); }
	if (currentMovement.x != 0.0f){ myCamera.position += normalize(cross((myCamera.forward * currentMovement.x), myCamera.up)) * movementSpeed; }
	if (currentMovement.y != 0.0f){ myCamera.position += movementSpeed * (myCamera.up * currentMovement.y); }
	

}

void ProgramManager::SetupQuad() {
	float vertices[] = {
		//Positions             //Textures
		0.5f, 0.5f, 0.0f,       1.0f, 1.0f, //top right
		0.5f, -0.5f, 0.0f,      1.0f, 0.0f, //bottom right
		-0.5f, -0.5f, 0.0f,     0.0f, 0.0f, //bottom left
		-0.5f, 0.5f, 0.0f,      0.0f, 1.0f  //top left
	};

	unsigned int indices[] = {
		0, 1, 3, //first triangle
		1, 2, 3 //second triangle
	};

	// Create Vertex Array Object, Vertex Buffer Object, and Element Buffer Object
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	// Bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	//Allocation & indexing of vertex attribute memory for vertex shader
	//Positions
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	//Textures
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0); // Unbind VAO
}

void ProgramManager::GenerateTerrain() {
	// ---- Terrain noise setup ----
	terrainNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	terrainNoise.SetFrequency(0.05f);
	terrainNoise.SetSeed(rand() & 100);

	biomeNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
	biomeNoise.SetFrequency(0.05f);
	biomeNoise.SetSeed(rand() % 100);

	// Vertices and Indicies setup
	const int squaresRow = RENDER_DISTANCE - 1;
	const int traianglesPerSquare = 2;
	const int trianglesGrid = squaresRow * squaresRow * traianglesPerSquare;

	GLfloat terrainVertices[MAP_SIZE][6];

	float drawingStartPosition = 1.0f;
	float columnVerticesOffset = drawingStartPosition;
	float rowVerticesOffset = drawingStartPosition;

	int rowIndex = 0;

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
		if (rowIndex == RENDER_DISTANCE) 
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
		terrainIndices[i][2] = RENDER_DISTANCE + columnIndicesOffset + rowIndicesOffset; //bottom left
		terrainIndices[i][1] = 1 + columnIndicesOffset + rowIndicesOffset; //top right

		terrainIndices[i + 1][0] = 1 + columnIndicesOffset + rowIndicesOffset; //top right
		terrainIndices[i + 1][2] = RENDER_DISTANCE + columnIndicesOffset + rowIndicesOffset; //bottom left
		terrainIndices[i + 1][1] = 1 + RENDER_DISTANCE + columnIndicesOffset + rowIndicesOffset; //bottom right

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
			rowIndicesOffset = rowIndicesOffset + RENDER_DISTANCE;
		}
	}

	//Terrain vertice index
	int i = 0;
	//Using x & y nested for loop in order to apply noise 2-dimensionally
	for (int y = 0; y < RENDER_DISTANCE; y++)
	{
		for (int x = 0; x < RENDER_DISTANCE; x++)
		{
			//Retrieval of biome to set
			float biomeValue = biomeNoise.GetNoise((float)x, (float)y);

			if (biomeValue <= -0.75f) //Plains
			{
				terrainVertices[i][3] = 0.0f;
				terrainVertices[i][4] = 0.75f;
				terrainVertices[i][5] = 0.25f;
			}
			else //Desert
			{
				terrainVertices[i][3] = 1.0f;
				terrainVertices[i][4] = 1.0f;
				terrainVertices[i][5] = 0.5f;
			}

			//Setting of height from 2D noise value at respective x & y coordinate
			terrainVertices[i][1] = terrainNoise.GetNoise((float)x, (float)y);
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

void ProgramManager::MainLoop() {
	// Game loop
	while (!glfwWindowShouldClose(window)) {
		deltaTime = GetDeltaTime(); // Calculate deltaTime for this frame

		myInputs.CheckInputs(window); // Check which keys are pressed
		MoveCamera();

		glClearColor(0.25f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		myCamera.CalculateProjection(shaderProgram);

		glBindVertexArray(VAO);
		
		glDrawElements(GL_TRIANGLES, MAP_SIZE * 32, GL_UNSIGNED_INT, 0);

		// Refresh
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// GLFW cleanup 
	glfwDestroyWindow(window);
	glfwTerminate();
}

// Function to calculate and return the last delta time
float ProgramManager::GetDeltaTime() {
	float currentTime = glfwGetTime();
	float delta = currentTime - lastFrame;

	lastFrame = currentTime;
	return delta;

}

void ProgramManager::LoadTexture(const char * textureName) {

	glBindVertexArray(VAO);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load and generate the texture
	int width, height, colourChannels;
	unsigned char* data = stbi_load(textureName, &width, &height, &colourChannels, 0);
	if (data)
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);
}

void ProgramManager::SetupProjection(int windowWidth, int windowHeight, float FOV) {
	//Model matrix
	mat4 model = mat4(1.0f); 
	//model = mat4(1.0f);
	//Scaling to zoom in
	model = scale(model, vec3(2.0f, 2.0f, 2.0f));
	//Rotation to look down
	model = rotate(model, radians(-45.0f), vec3(1.0f, 0.0f, 0.0f)); 
	//Movement to position further back 
	model = translate(model, vec3(0.0f, 1.f, -1.5f)); 

	//View matrix
	mat4 view = lookAt(vec3(0.0f, 0.0f, 3.0f), vec3(0.0f, 0.0f, 2.0f), vec3(0.0f, 1.0f, 0.0f)); 

	//Projection matrix
	mat4 projection = perspective(radians(FOV), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f); 

	// Model-view-projection matrix uniform for vertex shader
	mat4 mvp = projection * view * model; 
	int mvpLoc = glGetUniformLocation(shaderProgram, "mvpIn");
	glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, value_ptr(mvp)); 
}
