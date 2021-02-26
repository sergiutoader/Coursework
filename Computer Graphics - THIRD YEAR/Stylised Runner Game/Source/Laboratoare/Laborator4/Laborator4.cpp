#include "Laborator4.h"

#include <vector>
#include <string>
#include <iostream>

#include <Core/Engine.h>
#include "Transform3D.h"

using namespace std;

Laborator4::Laborator4()
{
}

Laborator4::~Laborator4()
{
}

void Laborator4::Init()
{
	polygonMode = GL_FILL;

	Mesh* mesh = new Mesh("box");
	mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
	meshes[mesh->GetMeshID()] = mesh;

	Mesh* mesh_2 = new Mesh("sphere");
	mesh_2->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
	meshes[mesh_2->GetMeshID()] = mesh_2;

	// initializare viteza (de translatie)
	speed = 1;
	incr_var = 0;

	// initialize tx, ty and tz (the translation steps)
	translateX = 0;
	translateY = 0;
	translateZ = 0;

	// initialize sx, sy and sz (the scale factors)
	scaleX = 1;
	scaleY = 1;
	scaleZ = 1;
	
	// initialize angularSteps
	angularStepOX = 0;
	angularStepOY = 0;
	angularStepOZ = 0;

	sunX = 0;
	sunY = 0;
	sunZ = -5;
	sunSpeed = 0.5f;


	// coordonate initiale box
	boxX = 0;
	boxY = 0.5;
	boxZ = 0.5;
}

void Laborator4::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Laborator4::Update(float deltaTimeSeconds)
{
	incr_var += deltaTimeSeconds;

	glLineWidth(3);
	glPointSize(5);
	glPolygonMode(GL_FRONT_AND_BACK, polygonMode);

	// cub1
	modelMatrix = glm::mat4(1);
	modelMatrix *= Transform3D::Translate(-2.5f, 0.5f,-1.5f);
	modelMatrix *= Transform3D::Translate(translateX, translateY, translateZ);
	RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);

	// cub2
	modelMatrix = glm::mat4(1);
	modelMatrix *= Transform3D::Translate(0.0f, 0.5f, -1.5f);
	modelMatrix *= Transform3D::Scale(scaleX, scaleY, scaleZ);
	RenderMesh(meshes["box"], shaders["Simple"], modelMatrix);
		
	// cub3
	modelMatrix = glm::mat4(1);
	modelMatrix *= Transform3D::Translate(2.5f, 0.5f, -1.5f);
	modelMatrix *= Transform3D::RotateOX(angularStepOX);
	modelMatrix *= Transform3D::RotateOY(angularStepOY);
	modelMatrix *= Transform3D::RotateOZ(angularStepOZ);
	RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);


	// bonus 1 - sistem solar
	solarSystem(deltaTimeSeconds);

	// bonus 2 - jump and flip
	jumpAndFlip(deltaTimeSeconds);

}

void Laborator4::jumpAndFlip(float deltaTimeSeconds) {
	// miscare rectilinie pe OX
	boxX += speed * deltaTimeSeconds;
	// saritura
	jumpY = abs(cos(2 * incr_var));
	// rotatie fata de OZ
	rotation = -2 * incr_var;

	modelMatrix = glm::mat4(1);
	modelMatrix *= Transform3D::Translate(boxX, boxY + jumpY, boxZ);
	modelMatrix *= Transform3D::RotateOZ(rotation);
	RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
}

void Laborator4::solarSystem(float deltaTimeSeconds) {
	// miscarea soarelui
	sunX += sunSpeed * deltaTimeSeconds;
	sunY += sunSpeed * deltaTimeSeconds;
	sunZ -= sunSpeed * deltaTimeSeconds;

	// miscarea pamantului
	earthX = sunX;
	earthY = sunY;
	earthZ = sunZ;

	// rotatia pamantului
	rotateEarthX = 2 * cos(incr_var + 1);
	rotateEarthY = 0;
	rotateEarthZ = 2 * sin(incr_var);

	// soarele
	modelMatrix = glm::mat4(1);
	modelMatrix *= Transform3D::Translate(sunX, sunY, sunZ);
	modelMatrix *= Transform3D::RotateOY(0.5 * incr_var);
	RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);

	// pamantul
	modelMatrix = glm::mat4(1);
	modelMatrix *= Transform3D::Translate(earthX, earthY, earthZ);
	modelMatrix *= Transform3D::Translate(rotateEarthX, rotateEarthY, rotateEarthZ);
	modelMatrix *= Transform3D::RotateOY(0.75 * incr_var);
	modelMatrix *= Transform3D::Scale(0.35, 0.35, 0.35);
	RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);

	// miscare si rotatie luna
	moonX = earthX + rotateEarthX;
	moonY = earthY + rotateEarthY;
	moonZ = earthZ + rotateEarthZ;

	rotateMoonX = 0.35 * sin(2 * incr_var);
	rotateMoonY = 0;
	rotateMoonZ = 0.35 * cos(2 * incr_var);

	// luna
	modelMatrix = glm::mat4(1);
	modelMatrix *= Transform3D::Translate(moonX, moonY, moonZ);
	modelMatrix *= Transform3D::Translate(rotateMoonX, rotateMoonY, rotateMoonZ);
	modelMatrix *= Transform3D::Scale(0.15, 0.15, 0.15);
	modelMatrix *= Transform3D::RotateOY(incr_var);
	RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);

}

void Laborator4::FrameEnd()
{
	DrawCoordinatSystem();
}

void Laborator4::OnInputUpdate(float deltaTime, int mods)
{
	if (!window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT)) {
		if (window->KeyHold(GLFW_KEY_W) == true) {
			translateZ -= speed * deltaTime;
		}

		if (window->KeyHold(GLFW_KEY_S) == true) {
			translateZ += speed * deltaTime;
		}

		if (window->KeyHold(GLFW_KEY_A) == true) {
			translateX -= speed * deltaTime;
		}

		if (window->KeyHold(GLFW_KEY_D) == true) {
			translateX += speed * deltaTime;
		}
	}

	if (window->KeyHold(GLFW_KEY_R) == true) {
		translateY += speed * deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_F) == true) {
		translateY -= speed * deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_1) == true) {
		
		scaleX += speed * deltaTime;
		scaleY += speed * deltaTime;
		scaleZ += speed * deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_2) == true) {

		// conditie pusa pt ca corpul sa nu se mareasca dupa ce dimensiunea lui a ajuns la 0
		if(scaleX >= 0) {
			scaleX -= speed * deltaTime;
			scaleY -= speed * deltaTime;
			scaleZ -= speed * deltaTime;
		}
	}

	if (window->KeyHold(GLFW_KEY_3) == true) {

		angularStepOX += speed * deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_4) == true) {

		angularStepOX -= speed * deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_5) == true) {

		angularStepOY += speed * deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_6) == true) {

		angularStepOY -= speed * deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_7) == true) {

		angularStepOZ += speed * deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_8) == true) {

		angularStepOZ -= speed * deltaTime;
	}


}

void Laborator4::OnKeyPress(int key, int mods)
{
	// add key press event
	if (key == GLFW_KEY_SPACE)
	{
		switch (polygonMode)
		{
		case GL_POINT:
			polygonMode = GL_FILL;
			break;
		case GL_LINE:
			polygonMode = GL_POINT;
			break;
		default:
			polygonMode = GL_LINE;
			break;
		}
	}
}

void Laborator4::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Laborator4::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event
}

void Laborator4::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Laborator4::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Laborator4::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Laborator4::OnWindowResize(int width, int height)
{
}
