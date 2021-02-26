#include <vector>
#include <string>
#include <time.h>
#include <Component/SimpleScene.h>
#include <Core/Engine.h>

#include "Tema2.h"

using namespace std;

Tema2::Tema2()
{
}

Tema2::~Tema2()
{
}

void Tema2::Init()
{
	cameraType = true; // true = 3rd person, false = 1st person
	camera = new LabCamera::Camera();
	// setare camera in 3rd person
	camera->Set(glm::vec3(0, 2, -1.5f), glm::vec3(0, 1, -5), glm::vec3(0, 1, 0));

	// marcare gameOver = false
	gameOver = false;

	// setare coordonate initiale sfera
	sphere_x = camera->GetTargetPosition().x;
	sphere_y = camera->GetTargetPosition().y;
	sphere_z = camera->GetTargetPosition().z;

	sphere_deformed = false; // sfera este initial nedeformata
	
	// setare valori pentru viteza
	forwardSpeed = 2;
	lateralSpeed = 2;

	// sfera se afla initial pe coloana din mijloc
	column = 0;

	// viteza de salt initiala
	jumpSpeed = 0;
	// sfera se afla initial pe platforma
	onPlatform = true;

	// initializare variabile combustibil

	fuel_max_ammount = 1.8;
	fuel_ammount = 1.8;

	// initial fastMode este dezactivat
	fastMode = false;

	InitPlatforms();

	// mesh cub
	{
		Mesh* mesh = new Mesh("box");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "box.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}
	// mesh sfera
	{
		Mesh* mesh = new Mesh("sphere");
		mesh->LoadMesh(RESOURCE_PATH::MODELS + "Primitives", "sphere.obj");
		meshes[mesh->GetMeshID()] = mesh;
	}
	// setare matrice proiectie
	projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);

	{
		Shader* shader = new Shader("ShaderTema2");
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Tema2/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
		shader->CreateAndLink();
		shaders[shader->GetName()] = shader;
	}

	// pozitie default a coordonatei y (la suprafata de contact cu platformele)
	sphere_y_init = camera->GetTargetPosition().y - 0.125f;

	// numarul de vieti este 5 initial
	lives = 5;
	lifeLost = false;
}

// genereaza o configuratie random a unei linii de platforme
vector<int> Tema2::GeneratePlatformConfig() {

	vector<int> generatedPlatforms = {};
	// platformele generate
	int firstGeneratedPlatform = rand() % 3;
	int secondGeneratedPlatform = -1;
	int thirdGeneratedPlatform = -1;

	// platforma stanga generata garantat
	if (firstGeneratedPlatform == 0) {

		// 50% sanse ca a doua platforma sa se genereze - trebuie ales intre 1 si 2
		if (rand() % 2 == 0) {
			secondGeneratedPlatform = rand() % 2 + 1;
		}

		// 25 % sanse ca a treia platforma sa se genereze - trebuie ales 1 daca second = 2 si 2 daca second = 1
		if (rand() % 4 == 0) {
			if (secondGeneratedPlatform != -1) {
				thirdGeneratedPlatform = 3 - secondGeneratedPlatform;
			}
			else {
				thirdGeneratedPlatform = rand() % 2 + 1;
			}
			
		}
	}

	// platforma centru generata garantat
	if (firstGeneratedPlatform == 1) {

		// 50% sanse ca a doua platforma sa se genereze - trebuie ales intre 0 si 2
		if (rand() % 2 == 0) {
			secondGeneratedPlatform = (rand() % 2) * 2;
		}

		// 25 % sanse ca a treia platforma sa se genereze - trebuie ales 0 daca second = 2 si 2 daca second = 0
		if (rand() % 4 == 0) {
			if (secondGeneratedPlatform != -1) {
				thirdGeneratedPlatform = 2 - secondGeneratedPlatform;
			} else {
				thirdGeneratedPlatform = (rand() % 2) * 2;
			}
		}
	}

	// platforma dreapta generata garantat
	if (firstGeneratedPlatform == 2) {

		// 50% sanse ca a doua platforma sa se genereze - trebuie ales intre 0 si 1
		if (rand() % 2 == 0) {
			secondGeneratedPlatform = rand() % 2;
		}

		// 25 % sanse ca a treia platforma sa se genereze - trebuie ales 0 daca second = 1 si 1 daca second = 0
		if (rand() % 4 == 0) {
			if (secondGeneratedPlatform != -1) {
				thirdGeneratedPlatform = 1 - secondGeneratedPlatform;
			}
			else {
				thirdGeneratedPlatform = rand() % 2;
			}
			
		}
	}

	generatedPlatforms.push_back(firstGeneratedPlatform);
	generatedPlatforms.push_back(secondGeneratedPlatform);
	generatedPlatforms.push_back(thirdGeneratedPlatform);

	return generatedPlatforms;
}


// genereaza platformele inititale
void Tema2::InitPlatforms() {
	// vector de platforme
	platforms = {};
	srand(time(NULL));

	int start = sphere_z + 1;
	glm::vec3 color_blue = glm::vec3(0, 0, 1);
	glm::vec3 color_light_blue = glm::vec3(0.2, 0.2, 1);

	// adaugare platforme initiale
	for (int i = start; i > start - 20; i--) {

		// generarea unei linii de coloane
		vector<int> generatedPlatforms = GeneratePlatformConfig();

		platform left, right, centre;

		left.exists = false;
		centre.exists = false;
		right.exists = false;

		// setare boolean powerup
		left.isPowerUp = false;
		centre.isPowerUp = false;
		right.isPowerUp = false;

		// setare platforme
		for (int plat_id : generatedPlatforms) {
			if (plat_id == 0) {
				left.exists = true;
			}

			if (plat_id == 1) {
				centre.exists = true;
			}

			if (plat_id == 2) {
				right.exists = true;
			}
		}

		// primele 10 randuri de platforme vor fi complete
		if (start - i < 10) {
			left.exists = true;
			centre.exists = true;
			right.exists = true;
		}

		// setare coordonate
		left.coord = glm::vec3(-1, 1, i);
		centre.coord = glm::vec3(0, 1, i);
		right.coord = glm::vec3(1, 1, i);

		platform_row plat_row;

		// setare culori + paritate
		if (i % 2 == 0) {
			left.color = color_light_blue;
			centre.color = color_blue;
			right.color = color_light_blue;

			plat_row.parity = false;
		}
		else {
			left.color = color_blue;
			centre.color = color_light_blue;
			right.color = color_blue;

			plat_row.parity = true;
		}

		plat_row.platform_left = left;
		plat_row.platform_centre = centre;
		plat_row.platform_right = right;

		platforms.push_back(plat_row);
	}
}

void Tema2::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

// verificare viata pierduta
void Tema2::CheckLifeLost() {
	// incheiere joc la 0 vieti
	if (lifeLost == true) {
		if (lives == 0) {
			gameOver = true;
		}
		lifeLost = false;

		// spawn-are sfera inapoi pe platforma daca jucatorul nu a pierdut toate vietile
		sphere_y = 2.5;
		jumpSpeed = 0;
		sphere_x = 0;
		column = 0;
	}
}

// verifica daca sfera a cazut din harta
void Tema2::CheckFallenSphere() {
	// conditie de viata pierduta - daca sfera a cazut de pe platforme
	if (sphere_y < -50) {
		lives--;
		lifeLost = true;
		std::printf("You have fallen down the platforms!\n");
	}
}

void Tema2::Update(float deltaTimeSeconds)
{ 
	if (gameOver == false) {

		// verificare daca s-a pierdut o viata in frame-ul curent
		CheckLifeLost();
		// verificare daca sfera a cazut sub nivelul minim in frame-ul curent
		CheckFallenSphere();

		// miscarea sferei
		ForwardMovement(deltaTimeSeconds);
		LateralMovement(deltaTimeSeconds);
		InAirMovement(deltaTimeSeconds);
		// generare rand nou de platforme
		NewPlatformRow();
		// verificare coliziuni
		CheckSphereCollision();
		// randare platforme
		RenderAllPlatforms();
		// randare sfera
		RenderSphere();
		// setare tip camera
		SetCameraMode();
		// randare fuel bar
		RenderFuelBar(deltaTimeSeconds);
		// randare vieti	
		RenderHearts();

		// miscare in fast mode
		FastModeMovement(deltaTimeSeconds);

		if (gameOver == true) {
			std::printf("GAME OVER! Press ENTER to start a new game.\n\n");
		}

	}
}

// miscare inainte - camera + sfera
void Tema2::ForwardMovement(float deltaTimeSeconds) {
	sphere_z -= forwardSpeed * deltaTimeSeconds;
	camera->MoveForward(forwardSpeed * deltaTimeSeconds);
}

// miscare laterala
void Tema2::LateralMovement(float deltaTimeSeconds) {
	if (movingLeft == true) {
		// miscare stanga
		sphere_x -= lateralSpeed * deltaTimeSeconds;

		// oprire miscare stanga
		if (sphere_x < column) {
			sphere_x = column;
			movingLeft = false;
		}
	}

	if (movingRight == true) {
		// miscare dreapta
		sphere_x += lateralSpeed * deltaTimeSeconds;
		// oprire miscare dreapta
		if (sphere_x > column) {
			sphere_x = column;
			movingRight = false;
		}
	}
}
// miscare in aer
void Tema2::InAirMovement(float deltaTimeSeconds) {
	// saritura
	if (onPlatform == false) {
		// ecuatia miscarii (accelerate)
		sphere_y += (jumpSpeed * deltaTimeSeconds - 10 * deltaTimeSeconds * deltaTimeSeconds);
		jumpSpeed -= 20 * deltaTimeSeconds;
	}
}

// miscare in modul FastMode
void Tema2::FastModeMovement(float deltaTimeSeconds) {
	// decrementare timp ramas fast mode
	if (fastMode) {
		fastModeTimeLeft -= deltaTimeSeconds;
		if (fastModeTimeLeft < 0) {
			fastMode = false;
			forwardSpeed = 2.0;
		}
	}
}

// elimina prima linie de platforme (care nu mai sunt vizibile) si genereaza alta
void Tema2::NewPlatformRow() {

	int start = sphere_z + 2;
	glm::vec3 color_blue = glm::vec3(0, 0, 1);
	glm::vec3 color_light_blue = glm::vec3(0.2, 0.2, 1);

	// se elimina prima intrare si se adauga alta
	if (platforms.begin()->platform_centre.coord.z > start) {
		platforms.erase(platforms.begin());

		// noua linie va avea paritate diferita de cea anterioara
		bool new_parity = !(platforms.back().parity);

		platform left, right, centre;
		// generare configuratie noua de platforme
		vector<int> generatedPlatforms = GeneratePlatformConfig();

		left.exists = false;
		centre.exists = false;
		right.exists = false;

		// setare boolean powerup
		left.isPowerUp = false;
		centre.isPowerUp = false;
		right.isPowerUp = false;

		int generatedPlatformCount = 0;
		// marcare platforme existente
		for (int plat_id : generatedPlatforms) {
			if (plat_id == 0) {
				left.exists = true;
				generatedPlatformCount++;
			}

			if (plat_id == 1) {
				centre.exists = true;
				generatedPlatformCount++;
			}

			if (plat_id == 2) {
				right.exists = true;
				generatedPlatformCount++;
			}
		}

		// generam platforma rosie doar daca sunt cel putin 2 platforme generate
		// (pentru a nu fi imposibil de trecut mai departe)
		if (generatedPlatformCount > 1) {
			// 12.5% sanse sa se genereze platforma rosie
			if (rand() % 8 == 0) {
				int plat_pos = rand() % 3;
				
				glm::vec3 color_red = glm::vec3(1, 0, 0);

				if (plat_pos == 0 && left.exists) {
					left.color = color_red;
					left.isPowerUp = true;
				}
				if (plat_pos == 1 && centre.exists) {
					centre.color = color_red;
					centre.isPowerUp = true;
				}
				if (plat_pos == 2 && right.exists) {
					right.color = color_red;
					right.isPowerUp = true;
				}
			}

			// generare platforme galbene (12.5%)
			if (rand() % 8 == 1) {
				int plat_pos = rand() % 3;
				glm::vec3 color_yellow = glm::vec3(1, 1, 0);

				if (plat_pos == 0 && left.exists) {
					left.color = color_yellow;
					left.isPowerUp = true;
				}
				if (plat_pos == 1 && centre.exists) {
					centre.color = color_yellow;
					centre.isPowerUp = true;
				}
				if (plat_pos == 2 && right.exists) {
					right.color = color_yellow;
					right.isPowerUp = true;
				}
			}
		}

		// generare platforme verzi (33%)
		if (rand() % 3 == 0) {
			int plat_pos = rand() % 3;
			glm::vec3 color_green = glm::vec3(0, 1, 0);

			if (plat_pos == 0 && left.exists) {
				left.color = color_green;
				left.isPowerUp = true;
			}
			if (plat_pos == 1 && centre.exists) {
				centre.color = color_green;
				centre.isPowerUp = true;
			}
			if (plat_pos == 2 && right.exists) {
				right.color = color_green;
				right.isPowerUp = true;
			}
		}
		// platforme portocalii (25%)
		else if (rand() % 4 == 0) {
			int plat_pos = rand() % 3;
			glm::vec3 color_orange = glm::vec3(1, 0.65, 0);

			if (plat_pos == 0 && left.exists) {
				left.color = color_orange;
				left.isPowerUp = true;
			}
			if (plat_pos == 1 && centre.exists) {
				centre.color = color_orange;
				centre.isPowerUp = true;
			}
			if (plat_pos == 2 && right.exists) {
				right.color = color_orange;
				right.isPowerUp = true;
			}
		}


		float newZ = platforms.back().platform_centre.coord.z - 1;
		// setare coordonate
		left.coord = glm::vec3(-1, 1, newZ);
		centre.coord = glm::vec3(0, 1, newZ);
		right.coord = glm::vec3(1, 1, newZ);

		platform_row plat_row;
		// setare culoare
		if (new_parity == false) {
			if (left.isPowerUp == false) left.color = color_light_blue;
			if (centre.isPowerUp == false) centre.color = color_blue;
			if (right.isPowerUp == false) right.color = color_light_blue;
		}
		else {
			if (left.isPowerUp == false) left.color = color_blue;
			if (centre.isPowerUp == false) centre.color = color_light_blue;
			if (right.isPowerUp == false) right.color = color_blue;
		}

		plat_row.parity = new_parity;

		plat_row.platform_left = left;
		plat_row.platform_centre = centre;
		plat_row.platform_right = right;

		platforms.push_back(plat_row);
	}
}

// randarea tuturor platformelor
void Tema2::RenderAllPlatforms() {
	glm::vec3 color_blue = glm::vec3(0, 0, 1);
	glm::vec3 color_light_blue = glm::vec3(0.2, 0.2, 1);

	for (platform_row row : platforms) {
		float z = row.platform_centre.coord.z;
		float parity = row.parity;

		// randare platforme
		if (row.platform_left.exists) RenderSmallPlatform(glm::vec3(-1, 1, z), row.platform_left.color);
		if (row.platform_centre.exists) RenderSmallPlatform(glm::vec3(0, 1, z), row.platform_centre.color);
		if (row.platform_right.exists) RenderSmallPlatform(glm::vec3(1, 1, z), row.platform_right.color);
	}
}

// randeaza o platforma 1x1
void Tema2::RenderSmallPlatform(glm::vec3 position, glm::vec3 color) {
	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, position);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 0.2, 1));
	RenderSimpleMesh(meshes["box"], shaders["ShaderTema2"], modelMatrix, color, true, false);
}

// randare sfera
void Tema2::RenderSphere() {
	if (cameraType)
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(sphere_x, sphere_y, sphere_z));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));
		RenderSimpleMesh(meshes["sphere"], shaders["ShaderTema2"], modelMatrix, glm::vec3(0.8, 0, 0), true, sphere_deformed);
	}
}

// randare vieti
void Tema2::RenderHearts() {
	float hearts_start_x = -0.95;

	// randare vieti pline
	for (int i = 0; i < lives; i++) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(hearts_start_x + 0.065 * i, 0.925, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05, 0.1, 1));
		RenderSimpleMesh(meshes["box"], shaders["ShaderTema2"], modelMatrix, glm::vec3(0.8, 0, 0), false, false);
	}

	// randare vieti goale
	for (int i = lives; i < 5; i++) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(hearts_start_x + 0.065 * i, 0.925, 0));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05, 0.1, 1));
		RenderSimpleMesh(meshes["box"], shaders["ShaderTema2"], modelMatrix, glm::vec3(0.8, 0.8, 0.8), false, false);
	}
}

// switch intre modurile camerei (1st person si 3rd person)
void Tema2::SetCameraMode() {
	if (cameraType) {
		camera->Set(glm::vec3(0, 3, sphere_z + 2.5), glm::vec3(0, 1, sphere_z), glm::vec3(0, 1, 0));
	}
	else {
		camera->Set(glm::vec3(sphere_x, sphere_y, sphere_z), glm::vec3(sphere_x, sphere_y, sphere_z - 1), glm::vec3(0, 1, 0));
	}
}

// randare fuel bar
void Tema2::RenderFuelBar(float deltaTimeSeconds) {

	// reducere combustibil
	fuel_ammount -= deltaTimeSeconds * forwardSpeed / 100;

	// terminare joc cand combustibilul a ajuns 0
	if (fuel_ammount < 0) {
		gameOver = true;
		std::printf("You have run out of fuel!\n");
	}

	// randare cub galben
	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.95, 0, 0));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0, (fuel_ammount - fuel_max_ammount) / 2, 0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05, fuel_ammount, 1));

	RenderSimpleMesh(meshes["box"], shaders["ShaderTema2"], modelMatrix, glm::vec3(0.9, 0.675, 0.15), false, false);

	// randare cub alb (fundal)
	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.95, 0, 0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05, fuel_max_ammount, 1));

	RenderSimpleMesh(meshes["box"], shaders["ShaderTema2"], modelMatrix, glm::vec3(0.7), false, false);
}


void Tema2::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix) {
	if (!mesh || !shader || !shader->program)
		return;

	// render an object using the specified shader and the specified position
	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	mesh->Render();
}

// functia de randare care trimite o culoare si 2 booleene catre shader
void Tema2::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, glm::vec3 color, bool tridimensionalRender, bool isDeformed)
{
	if (!mesh || !shader || !shader->program)
		return;

	// render an object using the specified shader and the specified position
	shader->Use();
	glUniformMatrix4fv(shader->loc_view_matrix, 1, GL_FALSE, glm::value_ptr(camera->GetViewMatrix()));
	glUniformMatrix4fv(shader->loc_projection_matrix, 1, GL_FALSE, glm::value_ptr(projectionMatrix));
	glUniformMatrix4fv(shader->loc_model_matrix, 1, GL_FALSE, glm::value_ptr(modelMatrix));

	// object color
	int loc_color = glGetUniformLocation(shader->program, "object_color");
	glUniform3fv(loc_color, 1, glm::value_ptr(color));

	// tip de randare (3D sau 2D)
	int loc_render_type = glGetUniformLocation(shader->program, "render_type");
	if (tridimensionalRender) {
		glUniform1i(loc_render_type, 1);
	}
	else {
		glUniform1i(loc_render_type, 0);
	}

	// deformare
	int loc_deformed = glGetUniformLocation(shader->program, "is_deformed");
	if (isDeformed) {
		glUniform1i(loc_deformed, 1);
	}
	else {
		glUniform1i(loc_deformed, 0);
	}
	

	// seed pentru deformare
	srand(time(NULL));
	float seed = rand();
	int loc_seed = glGetUniformLocation(shader->program, "seed");
	glUniform1f(loc_seed, seed);


	mesh->Render();
}

// functie care detecteaza coliziuni intre sfera si platforma
bool Tema2::SphereRectCollision(glm::vec3 sphere_centre, float radius, glm::vec3 rect_centre, glm::vec3 rect_dimensions) {
	float sphereXDistance = abs(sphere_centre.x - rect_centre.x);
	float sphereYDistance = abs(sphere_centre.y - rect_centre.y);
	float sphereZDistance = abs(sphere_centre.z - rect_centre.z);

	if (sphereXDistance >= radius + rect_dimensions.x) return false;
	if (sphereYDistance >= radius + rect_dimensions.y) return false;
	if (sphereZDistance >= radius + rect_dimensions.z) return false;

	if (sphereXDistance < rect_dimensions.x) return true;
	if (sphereYDistance < rect_dimensions.y) return true;
	if (sphereZDistance < rect_dimensions.z) return true;

	float cornerDistance_sq = ((sphereXDistance - rect_dimensions.x) * (sphereXDistance - rect_dimensions.x)) +
		((sphereYDistance - rect_dimensions.y) * (sphereYDistance - rect_dimensions.y) +
			((sphereZDistance - rect_dimensions.z) * (sphereZDistance - rect_dimensions.z)));

	return (cornerDistance_sq < (radius * radius));
}

// verificare coliziune sfera cu platforme
void Tema2::CheckSphereCollision() {
	bool onPlatformAux = false;
	for (std::size_t i = 0; i < platforms.size(); ++i) {

		float z = platforms[i].platform_centre.coord.z;
		float parity = platforms[i].parity;

		// verificare coliziune cu fiecare platforma
		for (int j = -1; j <= 1; j++) {
			if ((j == -1 && platforms[i].platform_left.exists) ||
				(j == 0 && platforms[i].platform_centre.exists) ||
				(j == 1 && platforms[i].platform_right.exists)) {
				// daca centrul sferei este mai jos decat marginea de sus a platformei, coliziunea nu se mai verifica
				// (obiectul va cadea chiar daca intra in contact cu platforma)
				if (SphereRectCollision(glm::vec3(sphere_x, sphere_y, sphere_z), 0.25f, glm::vec3(j, 1, z),
					glm::vec3(0.5, 0.1, 0.5)) == true && sphere_y > 1.25) {
					// colorare platforma mov + deformare sfera daca platforma este speciala
					if (j == -1) {
						// verificare power-ups
						glm::vec3 plat_color = platforms[i].platform_left.color;
						CheckSpecialPlatformCollision(plat_color);

						platforms[i].platform_left.color = glm::vec3(0.6, 0, 1);
						sphere_deformed = platforms[i].platform_left.isPowerUp;
					}
					else if (j == 0) {
						glm::vec3 plat_color = platforms[i].platform_centre.color;
						CheckSpecialPlatformCollision(plat_color);

						platforms[i].platform_centre.color = glm::vec3(0.6, 0, 1);
						sphere_deformed = platforms[i].platform_centre.isPowerUp;
					}
					else {
						glm::vec3 plat_color = platforms[i].platform_right.color;
						CheckSpecialPlatformCollision(plat_color);

						platforms[i].platform_right.color = glm::vec3(0.6, 0, 1);
						sphere_deformed = platforms[i].platform_right.isPowerUp;
					}

					jumpSpeed = 0;
					sphere_y = sphere_y_init;
					onPlatformAux = true;
				}
			}
		}
	}
	// resetare sphere_deformed atunci cand sfera este in aer
	if (onPlatformAux == false) {
		sphere_deformed = false;
	}
	onPlatform = onPlatformAux;
}

// verificare coliziune cu o platforma speciala
void Tema2::CheckSpecialPlatformCollision(glm::vec3 plat_color) {

	if (plat_color == glm::vec3(1, 0, 0)) {
		std::printf("You landed on a red platform.\n");
		gameOver = true;
	}
	// setare combustibil ramas pentru contactul cu platforme verzi / galbene
	else if (plat_color == glm::vec3(1, 1, 0)) {
		fuel_ammount -= 0.25;
	}
	else if (plat_color == glm::vec3(0, 1, 0)) {
		fuel_ammount += 0.5;
		if (fuel_ammount > fuel_max_ammount) {
			fuel_ammount = fuel_max_ammount;
		}
	}
	// platforme portocalii - setare mod rapid pentru 3 secunde
	else if (plat_color == glm::vec3(1, 0.65, 0)) {
		fastMode = true;
		fastModeTimeLeft = 3.0;
		forwardSpeed = 5;
	}
}



void Tema2::FrameEnd()
{
}

void Tema2::OnInputUpdate(float deltaTime, int mods)
{
}

void Tema2::OnKeyPress(int key, int mods)
{
	// add key press event
	if (key == GLFW_KEY_C)
	{
		cameraType = !cameraType;
	}
	
	// modificare viteza de inaintare la apasarea tastelor W si S
	if (key == GLFW_KEY_W) {
		if (fastMode == false) {
			forwardSpeed = 3;
		}
	}

	if (key == GLFW_KEY_S) {
		if (fastMode == false) {
			forwardSpeed = 1;
		}
	}

	// saritura
	if (key == GLFW_KEY_SPACE) {
		if (onPlatform) {
			jumpSpeed = 6;
			onPlatform = false;
		}
	}

	// miscare sfera stanga-dreapta
	if (key == GLFW_KEY_A) {

		if (movingRight == true) {
			movingRight = false;
		}

		if (column > -1) {
			column--;
			movingLeft = true;
		}
		
	}

	if (key == GLFW_KEY_D) {

		if (movingLeft == true) {
			movingLeft = false;
		}

		if (column < 1) {
			column++;
			movingRight = true;
		}
	}

	// incepere joc nou
	if (key == GLFW_KEY_ENTER) {
		if (gameOver) {
			gameOver = false;
			Init();
		}
	}
}

void Tema2::OnKeyRelease(int key, int mods)
{
	if (key == GLFW_KEY_W) {
		if (fastMode == false) {
			forwardSpeed = 2;
		}	
	}

	if (key == GLFW_KEY_S) {
		if (fastMode == false) {
			forwardSpeed = 2;
		}
	}
}

void Tema2::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
}

void Tema2::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
}

void Tema2::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
}

void Tema2::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema2::OnWindowResize(int width, int height)
{
}
