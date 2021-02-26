#include "Laborator1.h"

#include <vector>
#include <iostream>

#include <Core/Engine.h>

using namespace std;

// Order of function calling can be seen in "Source/Core/World.cpp::LoopUpdate()"
// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/World.cpp

// viteza obiectului care se misca
int speed = 1;

// variabila care retine daca tasta space a fost apasata
bool space_pressed = false;

// variabila care retine daca obiectul este in aer
bool inAir = false;

// variabila pentru viteza obiectului 
float v_y = 0;

// variabila prin care se alege obiectul rendered
int rendered_object = 0;

int color = 0;

// coordonatele initiale ale obiectului care se misca
float x = 3;
float y = 0.5f;
float z = 0;

// coordonatele obiectului care sare
float x_q = -5;
float y_q = 0;
float z_q = 0;


Laborator1::Laborator1()
{
}

Laborator1::~Laborator1()
{
}

void Laborator1::Init()
{
	// Load a mesh from file into GPU memory
	{
		
		Mesh* mesh_box = new Mesh("box");
		mesh_box->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		meshes[mesh_box->GetMeshID()] = mesh_box;

		// incarcare alte 2 mesh-uri cu o sfera, respectiv un ceainic
		Mesh* mesh_sphere = new Mesh("sphere");
		mesh_sphere->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh_sphere->GetMeshID()] = mesh_sphere;
		
		Mesh* mesh_teapot = new Mesh("teapot");
		mesh_teapot->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "teapot.obj");
		meshes[mesh_teapot->GetMeshID()] = mesh_teapot;

		Mesh* mesh_quad = new Mesh("quad");
		mesh_quad->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "quad.obj");
		meshes[mesh_quad->GetMeshID()] = mesh_quad;
	}
}

void Laborator1::FrameStart()
{

}

void Laborator1::Update(float deltaTimeSeconds)
{
	glm::ivec2 resolution = window->props.resolution;

	// sets the clear color for the color buffer
	// daca p este apasat, se realizeaza tranzitia de la culoarea albastru la culoarea negru sau invers
	if (color % 2 == 1) {
		glClearColor(0, 0, 1, 1);
	}
	else {
		glClearColor(0, 0, 0, 1);
	}

	// clears the color buffer (using the previously set color) and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);

	// render the box object
	RenderMesh(meshes["box"], glm::vec3(1, 0.5f, 0), glm::vec3(0.5f));

	// render the box object again but with different properties
	RenderMesh(meshes["box"], glm::vec3(-1, 0.5f, 0));

	// render the sphere object

	// in functie de valoarea rendered_object setata prin apasarea tastei 'c',
	// se alege obiectul rendered (restul impartirii la 3)
	// x, y, z -> valorile coordonatelor obiectului
	if (rendered_object % 3 == 0) {
		RenderMesh(meshes["sphere"], glm::vec3(x, y, z));
	}
	else if (rendered_object % 3 == 1) {
		RenderMesh(meshes["box"], glm::vec3(x, y, z));
	}
	else {
		RenderMesh(meshes["teapot"], glm::vec3(x, y, z));
	}

	// obiectul care sare
	RenderMesh(meshes["quad"], glm::vec3(x_q, y_q, z_q));
}

void Laborator1::FrameEnd()
{
	DrawCoordinatSystem();
}

// Read the documentation of the following functions in: "Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h

void Laborator1::OnInputUpdate(float deltaTime, int mods)
{
	// in functie de tasta apasata, modificam pozitia obiectului
	// folosim valoarea deltaTime pentru ca viteza sa fie independenta de framerate
	// astfel, viteza obiectului pe o directie va fi de 1 unitate / secunda, atunci cand
	// tasta corespunzatoare miscarii in acea directie este apasata
	if (window->KeyHold(GLFW_KEY_W) == true) {
		z -= speed * deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_S) == true) {
		z += speed * deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_A) == true) {
		x -= speed * deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_D) == true) {
		x += speed * deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_E) == true) {
		y += speed * deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_Q) == true) {
		y -= speed * deltaTime;
	}


	// jump-ul unui obiect 
	if (window->KeyHold(GLFW_KEY_SPACE) == true) {
		space_pressed = true;
	}

	if (space_pressed) {
		// setare viteza initiala
		v_y = 10;
		space_pressed = false;
		inAir = true;
	}

	// daca a ajuns pe sol, se "dezactiveaza" efectul gravitatiei
	if (y_q < 0) {
		y_q = 0;
		inAir = false;
	}
	
	// formule fizice pentru miscarea sub actiunea greutatii
	if (inAir) {
		v_y -= deltaTime * 10;
		y_q += v_y * deltaTime - 10 * deltaTime * deltaTime / 2;
	}

};

void Laborator1::OnKeyPress(int key, int mods)
{
	// Cand se apasa tasta C, se schimba al 3-lea obiect rendered
	if (key == GLFW_KEY_C) {
		rendered_object++;
	}

	if (key == GLFW_KEY_P) {
		color++;
	}

};

void Laborator1::OnKeyRelease(int key, int mods)
{
	// add key release event
};

void Laborator1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
};

void Laborator1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
};

void Laborator1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Laborator1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
	// treat mouse scroll event
}

void Laborator1::OnWindowResize(int width, int height)
{
	// treat window resize event
}
