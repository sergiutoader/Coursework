#pragma once

#include <Component/SimpleScene.h>
#include "LabCamera.h"

class Tema2 : public SimpleScene
{
public:
	Tema2();
	~Tema2();

	void Init() override;

private:
	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;

	void RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix) override;

	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnKeyRelease(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;
	void OnWindowResize(int width, int height) override;

	void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, glm::vec3 color, bool tridimensionalRender, bool isDeformed);
	void InitPlatforms();
	void RenderSmallPlatform(glm::vec3 position, glm::vec3 color); // randeaza o platforma mica
	void ForwardMovement(float deltaTimeSeconds);
	void LateralMovement(float deltaTimeSeconds);
	void InAirMovement(float deltaTimeSeconds);
	void RenderSphere();
	void RenderHearts();
	void NewPlatformRow();
	void CheckSphereCollision();
	void RenderAllPlatforms();
	std::vector<int> GeneratePlatformConfig();
	void SetCameraMode();
	void RenderFuelBar(float deltaTimeSeconds);
	void CheckSpecialPlatformCollision(glm::vec3 plat_color);
	void FastModeMovement(float deltaTimeSeconds);
	bool SphereRectCollision(glm::vec3 sphere_centre, float radius, glm::vec3 rect_centre, glm::vec3 rect_dimensions);\
	void CheckLifeLost();
	void CheckFallenSphere();

protected:
	// structura pentru o platforma
	struct platform {
		glm::vec3 coord;
		glm::vec3 color;
		bool exists;
		bool isPowerUp;
	};

	// structura care contine 3 platforme situate pe aceeasi linie
	struct platform_row {
		platform platform_left;
		platform platform_centre;
		platform platform_right;

		bool parity; // folosita pentru a schimba alternativ culorile pentru platformele albastre
	};


	// vector care contine randuri de platforme
	std::vector <platform_row> platforms;
	// bool care se seteaza pe true la contactul sferei cu o platforma
	bool onPlatform;
	LabCamera::Camera* camera;
	glm::mat4 projectionMatrix;
	bool cameraType;

	float sphere_x, sphere_y, sphere_z; // coordonatele sferei;
	float sphere_y_init; // coodonata initiala pentru sfera
	float column; // coloana pe care se afla obiectul

	bool sphere_deformed;

	float forwardSpeed;
	float lateralSpeed;

	float jumpSpeed;

	bool movingRight, movingLeft; // marcheaza daca obiectul se misca la stanga sau la dreapta

	bool gameOver;

	bool fastMode;
	float fastModeTimeLeft;
	float fuel_max_ammount;
	float fuel_ammount;

	int lives;
	bool lifeLost;
};
