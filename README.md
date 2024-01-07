# Comp-3016 CW2
## Usage
Controls:
- W - move forward
- S - move backwards
- A - move left
- D - move right
- Q - move down
- E - move up
- Mouse - look around
- Space Bar - open UI to edit and move objects
- Press the ExitUI button on the ui to close it

Use the inputs above to look around and move around the scene. All the objects within the scene other than the signature cube can be moved around with the UI by dragging on the positional coordinates to increase or decrease coordinate values. The light colour can also be edited in this UI so that the scene lighting can be edited.

![image](https://github.com/LeoLewis136/Comp-3016-CW2/assets/91668872/31b8eb84-c9ec-4fe9-8f6a-d3412c167c96)


## Functionality
### Base functionality
This program is using OpenGl to render 3d graphics. Within my main function I simply call some functions to setup the scene before starting the main game loop. This setup involves anything that will need to persist across the rest of the program such as OpenGL requirements, 3d objects to load or any objects that are core to the program's functionality. In my main game loop there are 3 separate sections: event checking where I check for the state of various events such as inputs or GLFW events, rendering where I tell objects to render themself and finally GUI rendering where applicable. Here I set up and render the UI for the game and then check for and manage any events within that UI. This main game loop then repeats until the user closes the game where I manage safe deletion of objects that require it.
### Base objects
I have a few objects that are required for the functionality of my game. These are: an input manager, a camera manager and an entity class. The Input manager is there to recieve input events and allow other objects to check the state of those events such as movement keys or mouse input. The camera object is where all the camera functionality is. This functionality includes managing the projection of the game, having functions to allow models to be positioned within the scene when rendering and allowing the camera to store its own position. Finally the entity class is there to allow entities to be drawn to the screen. It stores a position, rotation and scale for the entity and allows these to be changed to move the entity throughout the scene.
### Other objects
There are a few final less important objects within my game. These are a light source object and a terrain object. The light source object is an alternative to the entity object however doesn't contain any of the code to manage textures or lighting for the shaders to use. And the procedural terrain is there to create and store all the meshes that are used to create a procedural terrain object.

### What I started with and unique
To develop this project I started with the code that was developed during the labs and augmented it to add some extra OOP functionality to it along with adding all the extra features.
My project contains a cube with my name on it as my main signature object which is just below the standard position for the terrain.

### Video link
[Video](https://youtu.be/3gGIkz_wlso)
