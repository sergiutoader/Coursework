#include "Laborator3.h"

#include <vector>
#include <iostream>

#include <Core/Engine.h>
#include "Transform2D.h"
#include "Object2D.h"

using namespace std;

Laborator3::Laborator3()
{
}

Laborator3::~Laborator3()
{
}

float incrementVariable;

void Laborator3::Init()
{
	glm::ivec2 resolution = window->GetResolution();
	auto camera = GetSceneCamera();
	camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
	camera->SetPosition(glm::vec3(0, 0, 50));
	camera->SetRotation(glm::vec3(0, 0, 0));
	camera->Update();
	GetCameraInput()->SetActive(false);

	glm::vec3 corner = glm::vec3(0, 0, 0);
	float squareSide = 100;

	// compute coordinates of square center
	float cx = corner.x + squareSide / 2;
	float cy = corner.y + squareSide / 2;
	
	// initialize tx and ty (the translation steps)
	translateX = 0;
	translateY = 0;

	// initialize sx and sy (the scale factors)
	scaleX = 1;
	scaleY = 1;
	
	// initialize angularStep
	angularStep = 0;

	// initialize increment variable
	incrementVariable = 0;
	

	Mesh* square1 = Object2D::CreateSquare("square1", corner, squareSide, glm::vec3(1, 0, 0), true);
	AddMeshToList(square1);
	
	Mesh* square2 = Object2D::CreateSquare("square2", corner, squareSide, glm::vec3(0, 1, 0));
	AddMeshToList(square2);

	Mesh* square3 = Object2D::CreateSquare("square3", corner, squareSide, glm::vec3(0, 0, 1));
	AddMeshToList(square3);

	Mesh* square4 = Object2D::CreateSquare("square4", corner, squareSide, glm::vec3(1, 1, 1), true);
	AddMeshToList(square4);

	Mesh* square5 = Object2D::CreateSquare("square5", corner, squareSide, glm::vec3(0, 0, 1), true);
	AddMeshToList(square5);
}

void Laborator3::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Laborator3::Update(float deltaTimeSeconds)
{	
	// incrementare variabila folosita pentru calculul valorilor care oscileaza
	incrementVariable += deltaTimeSeconds;

	float squareSide = 100;
	// coordonate patrat_1
	float square1_x = 150;

	// ecuatia miscarii pe axa y
	float square1_y_motion = 200 * sin(5 * incrementVariable);
	float square1_y_middle = 250;
	float square1_y = square1_y_middle + square1_y_motion;

	modelMatrix = glm::mat3(1);
	// miscare sus-jos
	modelMatrix *= Transform2D::Translate(0, square1_y_motion);
	// translatie pozitie initiala
	modelMatrix *= Transform2D::Translate(square1_x, square1_y_middle);
	RenderMesh2D(meshes["square1"], shaders["VertexColor"], modelMatrix);
	



	// coordonate patrat_2
	float square2_x = 550;
	float square2_y = 250;

	modelMatrix = glm::mat3(1);
	modelMatrix *= Transform2D::Translate(square2_x, square2_y);
	// Translatie de revenire    
	modelMatrix *= Transform2D::Translate(squareSide / 2, squareSide / 2);
	// Rotatie
	// Translatie pentru centrarea patratului in origine
	modelMatrix *= Transform2D::Rotate(incrementVariable);
	// Translatie pentru centrarea patratului in origine
	modelMatrix *= Transform2D::Translate(-squareSide / 2, -squareSide / 2);
	RenderMesh2D(meshes["square2"], shaders["VertexColor"], modelMatrix);




	modelMatrix = glm::mat3(1);
	// factor de scalare care oscileaza intre 0.5 si 1.5 folosind functia sin
	float scalingFactor = 1 + sin(10 * incrementVariable) / 2;

	// Aplicare functii pentru efectul de pulsatie:
	// Translatie pana la pozitia de pe ecran
	modelMatrix *= Transform2D::Translate(950, 250);
	// Translatie de revenire
	modelMatrix *= Transform2D::Translate(squareSide / 2, squareSide / 2);
	// Scalare cu factorul de scalare
	modelMatrix *= Transform2D::Scale(scalingFactor, scalingFactor);
	// translatie pentru centrarea patratului in origine
	modelMatrix *= Transform2D::Translate(-squareSide / 2, -squareSide / 2);
	RenderMesh2D(meshes["square3"], shaders["VertexColor"], modelMatrix);




	// bonus1 - obiect care orbiteaza in jurul altui obiect in miscare
	modelMatrix = glm::mat3(1);
	// miscare in jurul obiectului
	modelMatrix *= Transform2D::Translate(120 * cos(incrementVariable), 200 * sin(incrementVariable));
	// translatie la coordonatele obiectului
	modelMatrix *= Transform2D::Translate(square1_x, square1_y);
	RenderMesh2D(meshes["square4"], shaders["VertexColor"], modelMatrix);

	// bonus2 - obiect care orbiteaza in jurul altui obiect stationar
	modelMatrix = glm::mat3(1);
	// miscare in jurul obiectului
	modelMatrix *= Transform2D::Translate(120 * cos(incrementVariable), 200 * sin(incrementVariable));
	// translatie la coordonatele obiectului 
	modelMatrix *= Transform2D::Translate(square2_x, square2_y);
	RenderMesh2D(meshes["square5"], shaders["VertexColor"], modelMatrix);

}

void Laborator3::FrameEnd()
{

}

void Laborator3::OnInputUpdate(float deltaTime, int mods)
{
	
}

void Laborator3::OnKeyPress(int key, int mods)
{
	// add key press event
}

void Laborator3::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Laborator3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
}

void Laborator3::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Laborator3::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Laborator3::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Laborator3::OnWindowResize(int width, int height)
{
}
