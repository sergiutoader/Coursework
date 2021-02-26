#pragma once

#pragma once
#include <Laboratoare/Tema2/LabCamera.h>
#include <Component/SimpleScene.h>
#include "Objects.h"


class Tema3 : public SimpleScene
{
public:
	Tema3();
	~Tema3();

	void Init() override;

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


	struct ufo {
		glm::vec3 coord;
		glm::vec3 color;
	};

	struct planet {
		float radius;					// raza
		float distanceFromStar;			// distanta fata de stea
		glm::vec3 color;				// culoare
		bool hasRing;					// daca are sau nu inel
		float ringAngle;				// unghiul de inclinatie al inelului fata de orizontala
		
		float velocity;					// viteza de rotatie
		float rotationAngle;			// unghiul de rotatie fata de orizontala

		float initialPos;				// faza initiala
	};

	// structura pentru o stea (sistem solar)
	struct star {
		float radius;
		glm::vec3 coord;
		glm::vec3 color;

		planet planet1;
		planet planet2;
		bool hasPlanet2;
	};

	// vectori pentru coordonate si culori stele
	std::vector<glm::vec3> star_coord;
	std::vector<glm::vec3> star_color;

	// ozn
	ufo ufo;

	// stele
	std::vector <star> stars;


	// vector care contine randuri de platforme
	std::vector <platform_row> platforms;
	// vector care contine coordonate pentru emeralde
	std::vector <glm::vec3> emerald_coord;
	// vector care contine coordonate pentru obstacole
	std::vector <glm::vec3> obstacle_coord;

	// coordonata portii (z)
	float gate_coord;


	// bool care se seteaza pe true la contactul sferei cu o platforma
	bool onPlatform;
	LabCamera::Camera* camera;
	glm::mat4 projectionMatrix;
	bool cameraType;

	float sphere_x, sphere_y, sphere_z; // coordonatele sferei;
	float sphere_y_init; // coodonata initiala pentru sfera
	float column; // coloana pe care se afla obiectul

	glm::vec3 skybox_coord; // coordonatele skybox_ului

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

	std::unordered_map<std::string, Texture2D*> mapTextures;

	std::vector<Objects::Digit*> scoreDigits;
	int score;

	glm::vec3 scorePosition;
	glm::vec3 scoreScalingFactor;

	// variabila incrementata cu timpul
	float time_passed;

	// culori
	glm::vec3 emerald_color = glm::vec3(0.314, 0.784, 0.471);

	std::vector<glm::vec3> star_colors = {
		glm::vec3(1.0 * 155 / 255, 1.0 * 176 / 255,  1),
		glm::vec3(1.0 * 170 / 255, 1.0 * 191 / 255,  1),
		glm::vec3(1.0 * 202 / 255, 1.0 * 215 / 255,  1),
		glm::vec3(1.0 * 248 / 255, 1.0 * 247 / 255,  1),
		glm::vec3(1, 1.0 * 244 / 255,  1.0 * 234 / 255),
		glm::vec3(1, 1.0 * 210 / 255,  1.0 * 161 / 255),
		glm::vec3(1, 1.0 * 204 / 255,  1.0 * 111 / 255)
	};


	// constante iluminare
	unsigned int materialShininess;
	float materialKd;
	float materialKs;
	float materialKe = 0.25f;

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

	void RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, glm::vec3 color, bool tridimensionalRender, bool isDeformed, Texture2D* texture = NULL, bool is_color_mixed = NULL, bool emits_light = NULL);
	void InitPlatforms();
	void RenderSmallPlatform(glm::vec3 position, glm::vec3 color); // randeaza o platforma mica
	void ForwardMovement(float deltaTimeSeconds);
	void LateralMovement(float deltaTimeSeconds);
	void InAirMovement(float deltaTimeSeconds);
	void RenderSphere();
	void RenderHearts();
	void RenderSkybox();
	void NewPlatformRow();
	void CheckSphereCollision();
	void RenderAllPlatforms();
	void RenderEmeralds();
	void RenderObstacles();
	void RenderPlanets();
	std::vector<int> GeneratePlatformConfig();
	glm::vec3 GenerateRandomStarColor();
	glm::vec3 GenerateRandomColor();
	void GenerateEmerald(platform p, float newX, float newZ);
	void GenerateObstacle(platform p, float newX, float newZ);
	void RemoveOldEmeralds();
	void RemoveOldObstacles();
	void SetCameraMode();
	void RenderFuelBar(float deltaTimeSeconds);
	void CheckSpecialPlatformCollision(glm::vec3 plat_color);
	void CheckEmeraldCollision();
	void CheckObstacleCollision();
	void FastModeMovement(float deltaTimeSeconds);
	bool SphereRectCollision(glm::vec3 sphere_centre, float radius, glm::vec3 rect_centre, glm::vec3 rect_dimensions);\
	void CheckLifeLost();
	void CheckFallenSphere();
	void DisplayScore();

	// functii ufo
	void InitUFO();
	void UFOMovement(float deltaTimeSeconds);
	void RenderUFO();

	// functii stele
	void InitStars();
	void RenderStars();
	void GenerateNewStars();

	// functii poarta
	void RenderGate();
	void RespawnGate();
};