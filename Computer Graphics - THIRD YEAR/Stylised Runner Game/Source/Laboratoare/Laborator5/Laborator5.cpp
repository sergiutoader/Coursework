#include "Laborator5.h"

#include <vector>
#include <string>
#include <iostream>

#include <Core/Engine.h>

using namespace std;

Laborator5::Laborator5()
{
}

Laborator5::~Laborator5()
{
}

void Laborator5::Init()
{
	renderCameraTarget = false;

	camera = new Laborator::Camera();
	camera->Set(glm::vec3(0, 2, 3.5f), glm::vec3(0, 1, 0), glm::vec3(0, 1, 0));

	// unghi initial fov
	fov = 60;
	// false -> perspective, true -> orto
	projection = false;
	// latime si inaltime orto
	orto_width = 12.0f;
	orto_height = 10.0f;

	{
		Mesh* mesh = new Mesh("box");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	{
		Mesh* mesh = new Mesh("sphere");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}

	projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, 0.01f, 200.0f);
}

void Laborator5::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

void Laborator5::Update(float deltaTimeSeconds)
{

	// setare projectionMatrix
	if (projection) {
		projectionMatrix = glm::ortho(-orto_width / 2, orto_width / 2, -orto_height / 2, orto_height / 2, 0.01f, 200.0f);
	}
	else {
		projectionMatrix = glm::perspective(RADIANS(fov), window->props.aspectRatio, 0.01f, 200.0f);
	}

	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 1, 0));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(45.0f), glm::vec3(0, 1, 0));

		// Attention! The RenderMesh function overrides the usual RenderMesh that we used until now
		// It uses the viewMatrix from Laborator::Camera instance and the local projectionMatrix
		RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
	}

	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(2, 0.5f, 0));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(60.0f), glm::vec3(1, 0, 0));
		RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
	}

	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(-2, 0.5f, 0));
		RenderMesh(meshes["box"], shaders["Simple"], modelMatrix);
	}

	// 6
	{
		// obiect 1 - scalat cu 0.5, rotit fata de OZ cu 45
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(4, 0.5, -5));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(45.0f), glm::vec3(0, 1, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5, 0.5, 0.5));
		RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
	}

	{
		// obiect 2 - scalat cu 1.5 doar pe 2 directii, rotit fata de OZ cu 30
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(8, 0.5, -5));
		modelMatrix = glm::rotate(modelMatrix, RADIANS(30.0f), glm::vec3(0, 0, 1));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 1.5, 1.5));
		RenderMesh(meshes["box"], shaders["VertexNormal"], modelMatrix);
	}


	// Render the camera target. Useful for understanding where is the rotation point in Third-person camera movement
	if (renderCameraTarget)
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, camera->GetTargetPosition());
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f));
		RenderMesh(meshes["sphere"], shaders["VertexNormal"], modelMatrix);
	}
}

void Laborator5::FrameEnd()
{
	DrawCoordinatSystem(camera->GetViewMatrix(), projectionMatrix);
}

void Laborator5::RenderMesh(Mesh * mesh, Shader * shader, const glm::mat4 & modelMatrix)
{
	if (!mesh || !shader || !shader->program)
		return;

	// render an object using the specified shader and the specified position
	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	mesh->Render();
}

// Documentation for the input functions can be found in: "/Source/Core/Window/InputController.h" or
// https://github.com/UPB-Graphics/Framework-EGC/blob/master/Source/Core/Window/InputController.h

void Laborator5::OnInputUpdate(float deltaTime, int mods)
{
	// move the camera only if MOUSE_RIGHT button is pressed
	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float cameraSpeed = 2.0f;

		if (window->KeyHold(GLFW_KEY_W)) {
			// TODO : translate the camera forward
			camera->TranslateForward(cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_A)) {
			// TODO : translate the camera to the left
			camera->TranslateRight(-cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_S)) {
			// TODO : translate the camera backwards
			camera->TranslateForward(-cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_D)) {
			// TODO : translate the camera to the right
			camera->TranslateRight(cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_Q)) {
			// TODO : translate the camera down
			camera->TranslateUpword(-cameraSpeed * deltaTime);
		}

		if (window->KeyHold(GLFW_KEY_E)) {
			// TODO : translate the camera up
			camera->TranslateUpword(cameraSpeed * deltaTime);
		}
	}

	if (window->KeyHold(GLFW_KEY_1)) {
		fov += 10*deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_2)) {
		fov -= 10*deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_3)) {
		orto_width += 10 * deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_4)) {
		orto_width -= 10 * deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_5)) {
		orto_height += 10 * deltaTime;
	}

	if (window->KeyHold(GLFW_KEY_6)) {
		orto_height -= 10 * deltaTime;
	}
}

void Laborator5::OnKeyPress(int key, int mods)
{
	// add key press event
	if (key == GLFW_KEY_T)
	{
		renderCameraTarget = !renderCameraTarget;
	}

	// proiectie ortografica
	if (key == GLFW_KEY_O) {
		projection = true;
	}

	// proiectie perspectiva
	if (window->KeyHold(GLFW_KEY_P)) {
		projection = false;
	}


}

void Laborator5::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Laborator5::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// add mouse move event

	if (window->MouseHold(GLFW_MOUSE_BUTTON_RIGHT))
	{
		float sensivityOX = 0.001f;
		float sensivityOY = 0.001f;

		if (window->GetSpecialKeyState() == 0) {
			renderCameraTarget = false;
			// TODO : rotate the camera in First-person mode around OX and OY using deltaX and deltaY
			// use the sensitivity variables for setting up the rotation speed
			camera->RotateFirstPerson_OX(-sensivityOX * deltaY);
			camera->RotateFirstPerson_OY(-sensivityOY * deltaX);
		}

		if (window->GetSpecialKeyState() && GLFW_MOD_CONTROL) {
			renderCameraTarget = true;
			// TODO : rotate the camera in Third-person mode around OX and OY using deltaX and deltaY
			// use the sensitivity variables for setting up the rotation speed
			camera->RotateThirdPerson_OX(-sensivityOX * deltaY);
			camera->RotateThirdPerson_OY(-sensivityOY * deltaX);

		}

	}
}

void Laborator5::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button press event
}

void Laborator5::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// add mouse button release event
}

void Laborator5::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Laborator5::OnWindowResize(int width, int height)
{
}
