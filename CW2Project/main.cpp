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

// imgui
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Inputs.h"
#include "Entity.h"
#include "LightSource.h"

using namespace std;
using namespace glm;

// Global variables
GLFWwindow* window;

// Objects required for program flow
Camera myCamera;
Inputs myInputs;

// Scene objects
Shader* shaders = nullptr;
Shader* lightShaders = nullptr;
Shader* terrainShaders = nullptr;
Entity* rock = nullptr;
Entity* cactus = nullptr;
Entity* cactus2 = nullptr;
Entity* mainTerrain = nullptr;
Entity* signature = nullptr;
LightSource* defaultLight = nullptr;

// When the UI is active
bool is_active = false;
vec3 lightColour = vec3(1.0f);

// Time management
float deltaTime = 0.0f; // The deltatime of the last frame
float lastFrame = 0.0f; // The time of the last frame

// Function that handles what to do with mouse inputs
void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	// Only do main movements when the UI is not open and active
	if (!is_active) {
		myCamera.MouseCallback(window, xpos, ypos);
	}
	// Pass mouse position to ImGui
	io.MousePos = ImVec2(static_cast<float>(xpos), static_cast<float>(ypos));
	
}

// Function to manage the current camera movement
void MoveCamera()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Only doing keyboard input management when the UI is not active
	if (!is_active) {
		// Get the current movement keys input
		vec3 currentMovement = myInputs.GetMovement();

		// Extent to which to move in one instance
		const float movementSpeed = 3.0f * deltaTime;

		// Debug to output the current input movement vector
		//cout << currentMovement.x << " " << currentMovement.y << " " << currentMovement.z << "\n";

		// Moving based on inputs recieved
		// Forward / Backwards
		if (currentMovement.z != 0.0f) { myCamera.position += movementSpeed * (myCamera.forward * currentMovement.z); }
		// Left / Right
		if (currentMovement.x != 0.0f) { myCamera.position += normalize(cross((myCamera.forward * currentMovement.x), myCamera.up)) * movementSpeed; }
		// Up / Down
		if (currentMovement.y != 0.0f) { myCamera.position += movementSpeed * (myCamera.up * currentMovement.y); }
	}
}

// Function to calculate and return the last delta time
float GetDeltaTime() {
	// Get the current time and work out the time passed since last frame
	float currentTime = glfwGetTime();
	float delta = currentTime - lastFrame;

	// Prepare next frame
	lastFrame = currentTime;
	return delta;
}

// Create the ImGui GUI
void GuiRender() {
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::Begin("Edit"); // Create window and start rendering

	ImGui::Text("Light Settings"); // Management to let user edit light properties
	ImGui::DragFloat3("Light Position", value_ptr(defaultLight->position), 0.01); // Edit light position 
	ImGui::ColorEdit3("Light Colour", value_ptr(lightColour));
	ImGui::Text("Ground Settings"); // Settings to allow user to change ground properties
	ImGui::DragFloat3("Ground position", value_ptr(mainTerrain->position), 0.01); // Edit ground position
	ImGui::DragFloat3("Ground rotation", value_ptr(mainTerrain->rotation), 0.1, 0, 360.0); // Edit ground rotation
	ImGui::DragFloat3("Ground Scale", value_ptr(mainTerrain->scale), 0.01); // Edit ground scale
	ImGui::Text("Cactus Settings"); // Settings to allow user to change Cactus properties
	ImGui::DragFloat3("Cactus position", value_ptr(cactus->position), 0.01); // Edit Cactus position
	ImGui::DragFloat3("Cactus rotation", value_ptr(cactus->rotation), 0.1, 0, 360.0); // Edit Cactus rotation
	ImGui::DragFloat3("Cactus Scale", value_ptr(cactus->scale), 0.01); // Edit Cactus scale
	ImGui::Text("Cactus2 Settings"); // Settings to allow user to change second Cactus properties
	ImGui::DragFloat3("Cactus2 position", value_ptr(cactus2->position), 0.01); // Edit Cactus2 position
	ImGui::DragFloat3("Cactus2 rotation", value_ptr(cactus2->rotation), 0.1, 0, 360.0); // Edit Cactus2 rotation
	ImGui::DragFloat3("Cactus2 Scale", value_ptr(cactus2->scale), 0.01); // Edit Cactus2 scale
	ImGui::Text("Rock Settings"); // Settings to allow user to change Rock settings
	ImGui::DragFloat3("Rock position", value_ptr(rock->position), 0.01); // Edit ground position
	ImGui::DragFloat3("Rock rotation", value_ptr(rock->rotation), 0.1, 0, 360.0); // Edit ground rotation
	ImGui::DragFloat3("Rock Scale", value_ptr(rock->scale), 0.01);

	// Button that closes UI and allows the game to recieve inputs
	if (ImGui::Button("ExitUI")) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		is_active = false;
	}

	ImGui::End(); // End the rendering of the UI
}

// Main "GameLoop" function to run each stage of the gameloop in order
void MainLoop() {
	// Game loop
	while (!glfwWindowShouldClose(window)) {
		deltaTime = GetDeltaTime(); // Calculate deltaTime for this frame

		myInputs.CheckInputs(window, is_active); // Check which keys are pressed
		MoveCamera(); // Move the camera based on the inputs collected above

		// Refresh
		glfwPollEvents();

		// Clear the screen to a pale blue as the background
		glClearColor(0.74f, 0.96f, 0.99f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Animating signature object
		signature->rotation.y = fmod(signature->rotation.y + (10 * deltaTime), 359.99);

		// Switch to the standard shader and pass all required info
		shaders->use();
		shaders->setVec3("lightColour", lightColour);
		shaders->setVec3("lightPos", defaultLight->position);

		// Draw all objects that need the standard lit shader
		rock->Draw(*shaders);
		cactus->Draw(*shaders);
		cactus2->Draw(*shaders);
		signature->Draw(*shaders);

		// Switch to the terrain shader
		terrainShaders->use();
		terrainShaders->setVec3("lightColour", lightColour);
		terrainShaders->setVec3("lightPos", defaultLight->position);

		// Draw all terrain objects
		mainTerrain->Draw(*terrainShaders);

		// Switch to special shader for light objects
		lightShaders->use();
		lightShaders->setVec3("lightColour", lightColour);

		// Draw all light sources
		defaultLight->Draw(*lightShaders);
		
		// ---- ImGui rendering ----
		// Start Imgui for this frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// Render gui call when UI should be active
		if (is_active) {
			GuiRender();
		}

		// Render everything to the screen
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	
		glfwSwapBuffers(window);
	}

	// Ensuring safe deletion of any pointers
	delete shaders;
	delete lightShaders;
	delete terrainShaders;
	delete rock;
	delete cactus;
	delete cactus2;
	delete mainTerrain;
	delete defaultLight;
	delete signature;

	// ImGUI cleanup
	ImGui_ImplOpenGL3_Shutdown(); 
	ImGui_ImplGlfw_Shutdown(); 
	ImGui::DestroyContext(); 

	// GLFW cleanup 
	glfwDestroyWindow(window);
	glfwTerminate();
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

	// Load all the shaders into shader objects
	shaders = new Shader("shaders/vertexShader.vert", "shaders/fragmentShader.frag");
	lightShaders = new Shader("shaders/lightShader.vert", "shaders/lightShader.frag");
	terrainShaders = new Shader("shaders/vertexShader.vert", "shaders/terrainShader.frag");
	// Start using the standard shaders
	shaders->use();

	// Stopping back faces being rendered 
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	// ---- Enable wireframes for debugging -----
	//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// Set up camera and input objects	
	myCamera = Camera(width, height, 45.0f);
	myInputs = Inputs();

	// Create a Light source object
	defaultLight = new LightSource(myCamera, "media/Sphere/sphere1.obj", vec3(5, 1, 5), vec3(0), vec3(0.1));
	// Create terrain object
	mainTerrain = new Entity(myCamera, 0.03, 0.01, 0.4, vec3(0), vec3(0), vec3(1));

	// Create miscellanious visual entities
	rock = new Entity(myCamera, "media/rock/Rock07-Base.fbx", vec3(0.6, 0.2, 1), vec3(0.0f), vec3(0.02));
	cactus = new Entity(myCamera, "media/cactus/cactus.3ds", vec3(0.85, 0, -2.15), vec3(-90.0f, 0.0f, 0.0f), vec3(0.01));
	cactus2 = new Entity(myCamera, "media/cactus/cactus.3ds", vec3(-2.45, 0.0, 2.49), vec3(-90.0f, 0.0f, 0.0f), vec3(0.01));
	signature = new Entity(myCamera, "media/Signature/signature.obj", vec3(0.0, -2.0, 0.0), vec3(0.0f, 0.0f, 0.0f), vec3(1));

	// ImGui Setup
	IMGUI_CHECKVERSION(); 
	ImGui::CreateContext();

	// Enable keyboard controls
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	// Ensuring ImGui knows which rendering type to use
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");
	ImGui::StyleColorsDark();

	// Successful loading
	return true;
}

// Program entrypoint
int main() {
	// Setup
	glfwInit();
	Setup(1280, 720, "Window");
	glfwSetCursorPosCallback(window, MouseCallback);
	
	// Start game
	MainLoop();

	return 0;
}