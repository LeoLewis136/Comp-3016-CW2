// Required includes
#include <iostream>

// GLAD
#include <glad/glad.h>

// glm
#include "glm/ext/vector_float3.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// Assimp
#include <assimp/Importer.hpp> 
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Inputs.h"
#include "Entity.h"

//LEARNOPENGL
//#include <learnopengl/shader_m.h>

using namespace std;
using namespace glm;

// Global variables
GLFWwindow* window;

// Objects required for program flow
Camera myCamera;
Inputs myInputs;

// LearnOpenGL Reqired Objects
Shader* shaders = nullptr;
Shader* lightShaders = nullptr;
Entity* rock = nullptr;
Entity* cactus = nullptr;
Entity* cactus2 = nullptr;
Entity* mainTerrain = nullptr;

// Rendering variables
GLuint shaderProgram = NULL;
GLuint VAO, VBO, EBO = NULL;

// *OLD* Temporarily used
GLuint texture;
mat4 quadTransform = mat4(1.0f); // ---- Temporary ---- transform of the currently rendered quad

// Time management
float deltaTime = 0.0f; // The deltatime of the last frame
float lastFrame = 0.0f; // The time of the last frame

void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
	myCamera.MouseCallback(window, xpos, ypos);
}

// Function to manage the current camera movement
void MoveCamera()
{
	vec3 currentMovement = myInputs.GetMovement();
	//Extent to which to move in one instance
	const float movementSpeed = 3.0f * deltaTime;

	cout << currentMovement.x << " " << currentMovement.y << " " << currentMovement.z << "\n";

	//Moving based on inputs recieved
	if (currentMovement.z != 0.0f) { myCamera.position += movementSpeed * (myCamera.forward * currentMovement.z); }
	if (currentMovement.x != 0.0f) { myCamera.position += normalize(cross((myCamera.forward * currentMovement.x), myCamera.up)) * movementSpeed; }
	if (currentMovement.y != 0.0f) { myCamera.position += movementSpeed * (myCamera.up * currentMovement.y); }

}

// Function to calculate and return the last delta time
float GetDeltaTime() {
	float currentTime = glfwGetTime();
	float delta = currentTime - lastFrame;

	lastFrame = currentTime;
	return delta;
}

// Main "GameLoop" function to run each stage of the gameloop in order
void MainLoop() {
	// Game loop
	while (!glfwWindowShouldClose(window)) {
		deltaTime = GetDeltaTime(); // Calculate deltaTime for this frame


		myInputs.CheckInputs(window); // Check which keys are pressed
		MoveCamera(); // Move the camera based on the inputs collected above


		glClearColor(0.25f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw all stardard objects switching to the correct shader first
		shaders->use();
		rock->Draw(*shaders);
		cactus->Draw(*shaders);
		cactus2->Draw(*shaders);
		mainTerrain->Draw(*shaders);

		lightShaders->use();

		// Refresh
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// GLFW cleanup 
	glfwDestroyWindow(window);
	glfwTerminate();
}

// Setting up 3D projection variables for later use
void SetupProjection(int windowWidth, int windowHeight, float FOV) {
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

// Setup function to set up required objects
bool Setup(int width, int height, const char* windowName) {
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
	//Initialisation of GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		cout << "GLAD failed to initialise\n";
		return -1;
	}

	shaders = new Shader("shaders/vertexShader.vert", "shaders/fragmentShader.frag");
	lightShaders = new Shader("shaders/vertexShader.vert", "shaders/lightShader.frag");
	//Loading of shaders
	shaders->use();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// ---- Enable wireframes for debugging -----
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	//GenerateTerrain();
	// Setup camera object to manage the world "Camera" 	
	myCamera = Camera(width, height, 45.0f);
	myInputs = Inputs();

	
	rock = new Entity(myCamera, "media/rock/Rock07-Base.fbx", vec3(3, 0, 1), vec3(0.0f), vec3(0.05));
	cactus = new Entity(myCamera, "media/cactus/cactus.3ds", vec3(3, 0, 4), vec3(0.0f, 90.0f, 0.0f), vec3(0.01));
	cactus2 = new Entity(myCamera, "media/cactus/cactus.3ds", vec3(5, 0, 2), vec3(90.0f, 0.0f, 0.0f), vec3(0.01));
	mainTerrain = new Entity(myCamera, 0.03, 0.01, 0.4, vec3(0), vec3(0), vec3(1));
	

	// Successful loading
	return true;
}

// Function to set up required information to view a quad in 3D *OLD* -unused-
void SetupQuad() {
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

// Program entrypoint
int main() {
	glfwInit();

	Setup(1280, 720, "Window");

	glfwSetCursorPosCallback(window, MouseCallback);

	MainLoop();

	return 0;
}