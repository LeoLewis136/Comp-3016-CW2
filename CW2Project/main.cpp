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
Entity* sphere = nullptr;
Entity* mainTerrain = nullptr;
LightSource* defaultLight = nullptr;

// ImGUI setup
bool is_active = false;

// Time management
float deltaTime = 0.0f; // The deltatime of the last frame
float lastFrame = 0.0f; // The time of the last frame

void MouseCallback(GLFWwindow* window, double xpos, double ypos) {
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	if (!is_active) {
		myCamera.MouseCallback(window, xpos, ypos);
	}

	io.MousePos = ImVec2(static_cast<float>(xpos), static_cast<float>(ypos));
	
}

// Function to manage the current camera movement
void MoveCamera()
{
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	if (!is_active) {
		vec3 currentMovement = myInputs.GetMovement();
		//Extent to which to move in one instance
		const float movementSpeed = 3.0f * deltaTime;

		cout << currentMovement.x << " " << currentMovement.y << " " << currentMovement.z << "\n";

		//Moving based on inputs recieved
		if (currentMovement.z != 0.0f) { myCamera.position += movementSpeed * (myCamera.forward * currentMovement.z); }
		if (currentMovement.x != 0.0f) { myCamera.position += normalize(cross((myCamera.forward * currentMovement.x), myCamera.up)) * movementSpeed; }
		if (currentMovement.y != 0.0f) { myCamera.position += movementSpeed * (myCamera.up * currentMovement.y); }
	}
}

// Function to calculate and return the last delta time
float GetDeltaTime() {
	float currentTime = glfwGetTime();
	float delta = currentTime - lastFrame;

	lastFrame = currentTime;
	return delta;
}

void GuiRender() {
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	static vec4 position;
	static int counter = 0;

	ImGui::Begin("Edit"); // Create window and start rendering

	ImGui::DragFloat3("Light Position", value_ptr(defaultLight->position), 0.01); // Edit light position 
	ImGui::DragFloat3("Ground position", value_ptr(mainTerrain->position), 0.01); // Edit ground position
	ImGui::DragFloat3("Ground rotation", value_ptr(mainTerrain->rotation), 0.1, 0, 360.0); // Edit ground rotation

	if (ImGui::Button("ExitUI")) {
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		is_active = false;
	}

	ImGui::End();
}

// Main "GameLoop" function to run each stage of the gameloop in order
void MainLoop() {
	// Game loop
	while (!glfwWindowShouldClose(window)) {
		deltaTime = GetDeltaTime(); // Calculate deltaTime for this frame

		myInputs.CheckInputs(window, is_active); // Check which keys are pressed
		MoveCamera(); // Move the camera based on the inputs collected above

		glClearColor(0.25f, 0.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		 

		// Draw all stardard objects switching to the correct shader first
		shaders->use();
		shaders->setVec3("lightColour", vec3(1.0f, 1.0f, 1.0f));
		shaders->setVec3("lightPos", defaultLight->position);

		rock->Draw(*shaders);
		cactus->Draw(*shaders);
		cactus2->Draw(*shaders);

		sphere->Draw(*shaders);
		mainTerrain->Draw(*shaders);

		lightShaders->use();
		defaultLight->Draw(*lightShaders);

		

		// Refresh
		glfwPollEvents();
		
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		GuiRender();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	
		glfwSwapBuffers(window);
	}

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

	shaders = new Shader("shaders/vertexShader.vert", "shaders/fragmentShader.frag");
	lightShaders = new Shader("shaders/lightShader.vert", "shaders/lightShader.frag");
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

	defaultLight = new LightSource(myCamera, "media/Sphere/sphere1.obj", vec3(5, 1, 5), vec3(0), vec3(0.1));

	rock = new Entity(myCamera, "media/rock/Rock07-Base.fbx", vec3(3, 0, 1), vec3(0.0f), vec3(0.05));
	cactus = new Entity(myCamera, "media/cactus/cactus.3ds", vec3(3, 0, 4), vec3(0.0f, 90.0f, 0.0f), vec3(0.01));
	cactus2 = new Entity(myCamera, "media/cactus/cactus.3ds", vec3(5, 0, 2), vec3(90.0f, 0.0f, 0.0f), vec3(0.01));
	sphere = new Entity(myCamera, "media/Sphere/sphere1.obj", vec3(5, 1, 2), vec3(0, 0, 0), vec3(1));
	mainTerrain = new Entity(myCamera, 0.03, 0.01, 0.4, vec3(0), vec3(0), vec3(1));
	
	// ImGui Setup
	IMGUI_CHECKVERSION(); 
	ImGui::CreateContext();

	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 460");
	ImGui::StyleColorsDark();

	

	// Successful loading
	return true;
}

// Program entrypoint
int main() {
	glfwInit();

	Setup(1280, 720, "Window");

	glfwSetCursorPosCallback(window, MouseCallback);

	MainLoop();

	return 0;
}