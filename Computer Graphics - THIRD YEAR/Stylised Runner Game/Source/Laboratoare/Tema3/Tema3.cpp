
#include <vector>
#include <string>
#include <time.h>
#include <Component/SimpleScene.h>
#include <Core/Engine.h>

#include "Tema3.h"

using namespace std;

Tema3::Tema3()
{
}

Tema3::~Tema3()
{
}

void Tema3::Init()
{

	srand(time(NULL));

	const string textureLoc = "Source/Laboratoare/Tema3/Textures/";

	// Load textures
	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "space.jpg").c_str(), GL_REPEAT);
		mapTextures["space"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "platform.jpg").c_str(), GL_REPEAT);
		mapTextures["platform"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "emerald.jpg").c_str(), GL_REPEAT);
		mapTextures["emerald"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "portal.png").c_str(), GL_REPEAT);
		mapTextures["portal"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "ufo_body.jpg").c_str(), GL_REPEAT);
		mapTextures["ufo_body"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "ball.jpg").c_str(), GL_REPEAT);
		mapTextures["ball"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "gate.jpg").c_str(), GL_REPEAT);
		mapTextures["gate"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "planet1.png").c_str(), GL_REPEAT);
		mapTextures["planet1"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "planet2.jpg").c_str(), GL_REPEAT);
		mapTextures["planet2"] = texture;
	}

	{
		Texture2D* texture = new Texture2D();
		texture->Load2D((textureLoc + "sun.png").c_str(), GL_REPEAT);
		mapTextures["sun"] = texture;
	}

	// proprietati material / lumina
	{
		materialShininess = 100;
		materialKd = 0.5;
		materialKs = 0.5;
	}

	// variabila care retine timpul trecut de la executia programului
	time_passed = 0;

	cameraType = true; // true = 3rd person, false = 1st person
	camera = new LabCamera::Camera();
	// setare camera in 3rd person
	camera->Set(glm::vec3(0, 2, -1.5f), glm::vec3(0, 1, -5), glm::vec3(0, 1, 0));

	// marcare gameOver = false
	gameOver = false;

	// setare coordonate initiale sfera
	sphere_x = camera->GetTargetPosition().x;
	sphere_y = 2;
	sphere_z = camera->GetTargetPosition().z;

	gate_coord = -25;

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

	// setare coordonate skybox
	skybox_coord = glm::vec3(sphere_x, sphere_y, sphere_z);

	InitPlatforms();
	InitUFO();
	InitStars();

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
	// mesh emerald
	{
		Mesh* mesh = Objects::EmeraldMesh();
		meshes[mesh->GetMeshID()] = mesh;
	}
	// mesh inel
	{
		Mesh* mesh = Objects::RingMesh();
		meshes[mesh->GetMeshID()] = mesh;
	}

	// vectorul cu cele 3 cifre
	scoreDigits = {};
	scoreDigits.push_back(new Objects::Digit());
	scoreDigits.push_back(new Objects::Digit());
	scoreDigits.push_back(new Objects::Digit());

	// multimea coordonatelor emeraldelor si obstacolelor
	emerald_coord = {};
	obstacle_coord = {};

	// pozitia scorului pe ecran
	scorePosition = glm::vec3(0.85, 0.95, 0);

	// adaugare mesh-uri scor la lista
	meshes[scoreDigits[0]->mesh->GetMeshID()] = scoreDigits[0]->mesh;
	meshes[scoreDigits[1]->mesh->GetMeshID()] = scoreDigits[1]->mesh;
	meshes[scoreDigits[2]->mesh->GetMeshID()] = scoreDigits[2]->mesh;
	
	// score
	score = 0;

	// factor de scalare pentru scor - depinde de rezolutia ecranului
	glm::ivec2 resolution = window->GetResolution();
	float ratio = 1.0 * resolution.y / resolution.x;
	scoreScalingFactor = glm::vec3(0.1 * ratio, 0.1, 0);


	// setare matrice proiectie
	projectionMatrix = glm::perspective(RADIANS(60), window->props.aspectRatio, 0.01f, 200.0f);

	{
		Shader* shader = new Shader("ShaderTema2");
		shader->AddShader("Source/Laboratoare/Tema3/Shaders/VertexShader.glsl", GL_VERTEX_SHADER);
		shader->AddShader("Source/Laboratoare/Tema3/Shaders/FragmentShader.glsl", GL_FRAGMENT_SHADER);
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
vector<int> Tema3::GeneratePlatformConfig() {

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
			}
			else {
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

// alege o culoare random specifica stelelor
glm::vec3 Tema3::GenerateRandomStarColor() {
	int color_idx = rand() % 7;
	return star_colors[color_idx];
}

// functie care genereaza o culoare random
glm::vec3 Tema3::GenerateRandomColor() {

	float r, g, b;

	r = 1.0 * (rand() % 256) / 256;
	g = 1.0 * (rand() % 256) / 256;
	b = 1.0 * (rand() % 256) / 256;

	return glm::vec3(r, g, b);
}


// genereaza stelele initiale
void Tema3::InitStars() {
	stars = {};

	// se vor genera 2 stele initial (una in stanga si una in dreapta platformelor) 
	star s1, s2;


	s1.color = GenerateRandomStarColor();
	s1.radius = 0.005 + 1.0 * (rand() % 100) / 500; // raza intre 0.005 si 0.205
	s1.coord.x = -4.5 + 1.0 * (rand() % 300) / 200; // x intre -6 si -3
	s1.coord.y = 1 + 1.0 * (rand() % 250) / 100;  // y intre 0 si 2.5
	s1.coord.z = sphere_z - 5 - 2 + rand() % 5; // un interval de lungime 4 cu i in centru

	// planeta pentru steaua 1
	s1.planet1.color = GenerateRandomColor();
	s1.planet1.distanceFromStar = 1.0 + 1.0 * (rand() % 100) / 200;		//	distanta intre 1.0 si 1.5
	s1.planet1.hasRing = rand() % 2;									//	50% sanse sa aiba inel
	s1.planet1.radius = 0.2 + 1.0 * (rand() % 100) / 400;				//	raza intre 0.2 si 0.45
	s1.planet1.ringAngle = RADIANS(rand() % 60);						//	unghi fata de orizontala intre 0 si 60 grade
	s1.planet1.rotationAngle = 0.1 + 1.0 * (rand() % 100) / 500;		//	unghi de rotatie fata de orizontala - 0.1 - 0.3
	s1.planet1.velocity = 1 + 1.0 * (rand() % 100) / 100;				//	viteza de rotatie intre 1 si 2
	s1.planet1.initialPos = RADIANS(rand() % 360);						// faza initiala

	s1.hasPlanet2 = rand() % 2;											// 50% sanse sa aiba o a doua planeta
	if (s1.hasPlanet2) {
		s1.planet2.color = GenerateRandomColor();
		s1.planet2.distanceFromStar = 1.0 + 1.0 * (rand() % 100) / 200;		//	distanta intre 1.0 si 1.5
		s1.planet2.hasRing = rand() % 2;									//	50% sanse sa aiba inel
		s1.planet2.radius = 0.2 + 1.0 * (rand() % 100) / 400;				//	raza intre 0.2 si 0.45
		s1.planet2.ringAngle = RADIANS(rand() % 60);						//	unghi fata de orizontala intre 0 si 60 grade
		s1.planet2.rotationAngle = 0.7 + 1.0 * (rand() % 100) / 500;		//	unghi de rotatie fata de orizontala - 0.7 - 0.9
		s1.planet2.velocity = 1 + 1.0 * (rand() % 100) / 100;				//	viteza de rotatie intre 1 si 2
		s1.planet2.initialPos = RADIANS(rand() % 360);						// faza initiala
	}


	s2.color = GenerateRandomStarColor();
	s2.radius = 0.005 + 1.0 * (rand() % 100) / 500; // raza intre 0.005 si 0.025
	s2.coord.x = 3 + 1.0 * (rand() % 300) / 200; // x intre 3 si 4.5
	s2.coord.y = 1 + 1.0 * (rand() % 250) / 100;  // y intre 0 si 2.5
	s2.coord.z = sphere_z - 25 - 2 + rand() % 5; // un interval de lungime 4 cu i in centru

	// planeta pentru steaua 2
	s2.planet1.color = GenerateRandomColor();
	s2.planet1.distanceFromStar = 1.0 + 1.0 * (rand() % 100) / 200;		//	distanta intre 1.0 si 1.5
	s2.planet1.hasRing = rand() % 2;									//	50% sanse sa aiba inel
	s2.planet1.radius = 0.2 + 1.0 * (rand() % 100) / 400;				//	raza intre 0.2 si 0.45
	s2.planet1.ringAngle = RADIANS(rand() % 60);						//	unghi fata de orizontala intre 0 si 60 grade
	s2.planet1.rotationAngle = 0.1 + 1.0 * (rand() % 100) / 500;		//	unghi de rotatie fata de orizontala 0.1 - 0.3
	s2.planet1.velocity = 1 + 1.0 * (rand() % 100) / 100;				//	viteza de rotatie intre 1 si 2
	s2.planet1.initialPos = RADIANS(rand() % 360);						// faza initiala

	s2.hasPlanet2 = rand() % 2;											// 50% sanse sa aiba o a doua planeta
	if (s2.hasPlanet2) {
		s2.planet2.color = GenerateRandomColor();
		s2.planet2.distanceFromStar = 1.0 + 1.0 * (rand() % 100) / 200;		//	distanta intre 1.0 si 1.5
		s2.planet2.hasRing = rand() % 2;									//	50% sanse sa aiba inel
		s2.planet2.radius = 0.2 + 1.0 * (rand() % 100) / 400;				//	raza intre 0.2 si 0.45
		s2.planet2.ringAngle = RADIANS(rand() % 60);						//	unghi fata de orizontala intre 0 si 60 grade
		s2.planet2.rotationAngle = 0.7 + 1.0 * (rand() % 100) / 500;		//	unghi de rotatie fata de orizontala - 0.7 - 0.9
		s2.planet2.velocity = 1 + 1.0 * (rand() % 100) / 100;				//	viteza de rotatie intre 1 si 2
		s2.planet2.initialPos = RADIANS(rand() % 360);						// faza initiala
	}

	// adaugare la lista de stele
	stars.push_back(s1);
	stars.push_back(s2);

	star_coord.push_back(s1.coord);
	star_coord.push_back(s2.coord);

	star_color.push_back(s1.color);
	star_color.push_back(s2.color);
}

// genereaza platformele inititale
void Tema3::InitPlatforms() {
	// vector de platforme
	platforms = {};

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

// initializare coordonate si culoare ozn 
void Tema3::InitUFO() {

	float red, green, blue;

	red = 1.0 * (rand() % 256) / 256;
	green = 1.0 * (rand() % 256) / 256;
	blue = 1.0 * (rand() % 256) / 256;
	ufo.color = glm::vec3(red, green, blue);

	ufo.coord = glm::vec3(0, 2.25, sphere_z - 5);
}

void Tema3::FrameStart()
{
	// clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::ivec2 resolution = window->GetResolution();
	// sets the screen area where to draw
	glViewport(0, 0, resolution.x, resolution.y);
}

// verificare viata pierduta
void Tema3::CheckLifeLost() {
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
void Tema3::CheckFallenSphere() {
	// conditie de viata pierduta - daca sfera a cazut de pe platforme
	if (sphere_y < -50) {
		lives--;
		lifeLost = true;
		std::printf("You have fallen down the platforms!\n");
	}
}

void Tema3::Update(float deltaTimeSeconds)
{
	// fortare deltaTime sa fie minim 20ms, pentru a nu altera comportamentul jocului in caz de framerate mic
	if (deltaTimeSeconds > 0.02f) deltaTimeSeconds = 0.02f;

	if (gameOver == false) {

		time_passed += deltaTimeSeconds;

		// verificare daca s-a pierdut o viata in frame-ul curent
		CheckLifeLost();
		// verificare daca sfera a cazut sub nivelul minim in frame-ul curent
		CheckFallenSphere();

		// miscarea sferei
		ForwardMovement(deltaTimeSeconds);
		LateralMovement(deltaTimeSeconds);
		InAirMovement(deltaTimeSeconds);
		// generare rand nou de platforme + elemente colectabile si obstacole
		NewPlatformRow();
		// verificare coliziuni
		CheckSphereCollision();
		// verificare coliziuni cu emeralde
		CheckEmeraldCollision();
		// verificare coliziuni cu obstacole
		CheckObstacleCollision();
		// randare platforme
		RenderAllPlatforms();
		// randare emeralde
		RenderEmeralds();
		// randare obstacole
		RenderObstacles();
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
		// miscare / randaere skybox
		RenderSkybox();

		// Afisare scor
		DisplayScore();

		// Randare + deplasare UFO
		UFOMovement(deltaTimeSeconds);
		RenderUFO();

		// Randare + generare stele
		RenderStars();
		GenerateNewStars();
		// Randare planete
		RenderPlanets();

		// Randare + generare porti
		RenderGate();
		RespawnGate();

		if (gameOver == true) {
			std::printf("GAME OVER! Press ENTER to start a new game.\n\n");
		}
	}
}

// miscare inainte - camera + sfera
void Tema3::ForwardMovement(float deltaTimeSeconds) {
	sphere_z -= forwardSpeed * deltaTimeSeconds;
	camera->MoveForward(forwardSpeed * deltaTimeSeconds);
}

// miscare laterala
void Tema3::LateralMovement(float deltaTimeSeconds) {
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
void Tema3::InAirMovement(float deltaTimeSeconds) {
	// saritura
	if (onPlatform == false) {
		// ecuatia miscarii (accelerate)
		sphere_y += (jumpSpeed * deltaTimeSeconds - 7.5 * deltaTimeSeconds * deltaTimeSeconds);
		jumpSpeed -= 15 * deltaTimeSeconds;
	}
}

// miscare in modul FastMode
void Tema3::FastModeMovement(float deltaTimeSeconds) {
	// decrementare timp ramas fast mode
	if (fastMode) {
		fastModeTimeLeft -= deltaTimeSeconds;
		if (fastModeTimeLeft < 0) {
			fastMode = false;
			forwardSpeed = 2.0;
		}
	}
}


// calculeaza coordonatele ozn-ului in functie de timp
void Tema3::UFOMovement(float deltaTimeSeconds) {

	// oscilatie pe x stanga - dreapta
	ufo.coord.x = sin(2 * time_passed);
	// ozn-ul va executa si o miscare fata - spate in functie de deltaTime
	ufo.coord.z += 0.05 * cos(time_passed / 2) - deltaTimeSeconds;

	// eliminare ozn daca acesta este depasit de sfera, spawn-and altul nou in fata acesteia cu o culoare noua
	if (ufo.coord.z > sphere_z + 3) {
		ufo.coord.x = 0;
		ufo.coord.z = sphere_z - 15;

		float red = 1.0 * (rand() % 256) / 256;
		float green = 1.0 * (rand() % 256) / 256;
		float blue = 1.0 * (rand() % 256) / 256;
		ufo.color = glm::vec3(red, green, blue);
	}
}

// elimina planetele care nu mai sunt in campul vizual si genereaza altele
void Tema3::GenerateNewStars() {
	
	// prima stea din lista iese din aria vizuala
	if (stars[0].coord.z > sphere_z + 5) {
		float oldX = stars[0].coord.x;

		// eliminare prima stea din lista
		stars.erase(stars.begin());

		star_coord.erase(star_coord.begin());
		star_color.erase(star_color.begin());

		// initializare stea noua
		star newStar;

		
		newStar.color = GenerateRandomStarColor();
		newStar.radius = 0.005 + 1.0 * (rand() % 100) / 500; // raza intre 0.005 si 0.025
		if (oldX < 0) {
			newStar.coord.x = -4.5 + 1.0 * (rand() % 300) / 200; // x intre -4.5 si -3
		}
		else {
			newStar.coord.x = 3 + 1.0 * (rand() % 300) / 100; // x intre -6 si -3
		}
		
		newStar.coord.y = 1 + 1.0 * (rand() % 250) / 100;  // y intre 0 si 2.5
		newStar.coord.z = stars[stars.size() - 1].coord.z - 20 - 2 + rand() % 5; // un interval de lungime 4

		newStar.planet1.color = GenerateRandomColor();
		newStar.planet1.distanceFromStar = 1.0 + 1.0 * (rand() % 100) / 200;		//	distanta intre 1.0 si 1.5
		newStar.planet1.hasRing = rand() % 2;										//	50% sanse sa aiba inel
		newStar.planet1.radius = 0.2 + 1.0 * (rand() % 100) / 400;					//	raza intre 0.2 si 0.45
		newStar.planet1.ringAngle = RADIANS(rand() % 60);							//	unghi fata de orizontala intre 0 si 60 grade
		newStar.planet1.rotationAngle = 0.1 + 1.0 * (rand() % 100) / 500;			//	unghi de rotatie fata de orizontala
		newStar.planet1.velocity = 1 + 1.0 * (rand() % 100) / 100;					//	viteza de rotatie intre 1 si 2
		newStar.planet1.initialPos = RADIANS(rand() % 360);							// faza initiala

		newStar.hasPlanet2 = rand() % 2;											// 50% sanse sa aiba o a doua planeta
		if (newStar.hasPlanet2) {
			newStar.planet2.color = GenerateRandomColor();
			newStar.planet2.distanceFromStar = 1.0 + 1.0 * (rand() % 100) / 200;		//	distanta intre 1.0 si 1.5
			newStar.planet2.hasRing = rand() % 2;									//	50% sanse sa aiba inel
			newStar.planet2.radius = 0.2 + 1.0 * (rand() % 100) / 400;				//	raza intre 0.2 si 0.45
			newStar.planet2.ringAngle = RADIANS(rand() % 60);						//	unghi fata de orizontala intre 0 si 60 grade
			newStar.planet2.rotationAngle = 0.7 + 1.0 * (rand() % 100) / 500;		//	unghi de rotatie fata de orizontala - 0.7 - 0.9
			newStar.planet2.velocity = 1 + 1.0 * (rand() % 100) / 100;				//	viteza de rotatie intre 1 si 2
			newStar.planet2.initialPos = RADIANS(rand() % 360);						// faza initiala
		}


		stars.push_back(newStar);

		star_coord.push_back(newStar.coord);
		star_color.push_back(newStar.color);
	}
}

// elimina prima linie de platforme (care nu mai sunt vizibile) si genereaza alta
void Tema3::NewPlatformRow() {

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

		float newZ = platforms.back().platform_centre.coord.z - 1;

		// stergere emeralde vechi + obstacole vechi
		RemoveOldEmeralds();
		RemoveOldObstacles();

		bool leftGen = false;
		bool centreGen = false;
		bool rightGen = false;

		// marcare platforme existente + generare emeralde si obstacole
		for (int plat_id : generatedPlatforms) {
			if (plat_id == 0) {
				left.exists = true;
				generatedPlatformCount++;

				if (rand() % 2) {
					GenerateEmerald(left, -1, newZ);
					leftGen = true;
				}
			}

			if (plat_id == 1) {
				centre.exists = true;
				generatedPlatformCount++;
				if (rand() % 2) {
					GenerateEmerald(centre, 0, newZ);
					centreGen = true;
				}
			}

			if (plat_id == 2) {
				right.exists = true;
				generatedPlatformCount++;
				if (rand() % 2) {
					GenerateEmerald(right, 1, newZ);
					rightGen = true;
				}
			}
		}

		bool emeraldGenerated = leftGen || centreGen || rightGen;
		// generare obstacole daca pe linia curenta nu sunt emeralde
		if (emeraldGenerated == false && (int)newZ % 2 == 0) {
			GenerateObstacle(left, -1, newZ);
			GenerateObstacle(centre, 0, newZ);
			GenerateObstacle(right, 1, newZ);
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

// functie care genereaza emeralde noi
void Tema3::GenerateEmerald(platform p, float newX, float newZ) {
	// se va genera cu sansa de 20% (daca platforma exista)
	if (rand() % 5 == 0 && p.exists) {
		emerald_coord.push_back(glm::vec3(newX, sphere_y_init, newZ));
	}
}

// functie care genereaza obstacole noi
void Tema3::GenerateObstacle(platform p, float newX, float newZ) {
	// se va genera daca platforma exista
	if (p.exists) {
		obstacle_coord.push_back(glm::vec3(newX, sphere_y_init - 0.1, newZ));
	}
}

// respawnare poarta - atunci cand aceasta nu se mai afla in campul vizual
void Tema3::RespawnGate() {
	if (gate_coord > sphere_z + 3) {
		gate_coord = sphere_z - 25;
	}
}


// functie prin care se elimina emeraledele care nu mai sunt in campul vizual
void Tema3::RemoveOldEmeralds() {
	int start = sphere_z + 2;

	int end = emerald_coord.size();
	if (end > 3) end = 3;

	for (int i = 0; i < end; i++) {
		if (emerald_coord[i].z > start) {
			emerald_coord.erase(emerald_coord.begin() + i);
			i--;
			end--;
		}
	}
}

// functie prin care se elimina obstacolele care nu mai sunt in campul vizual
void Tema3::RemoveOldObstacles() {
	int start = sphere_z + 2;

	int end = obstacle_coord.size();
	if (end > 3) end = 3;

	for (int i = 0; i < end; i++) {
		if (obstacle_coord[i].z > start) {
			obstacle_coord.erase(obstacle_coord.begin() + i);
			i--;
			end--;
		}
	}
}

// randarea tuturor platformelor
void Tema3::RenderAllPlatforms() {
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

// randeaza emeraldele
void Tema3::RenderEmeralds() {

	for (int i = 0; i < emerald_coord.size(); i++) {
		glm::mat4 modelMatrix = glm::mat4(1);
		// animatie - miscare sus-jos
		modelMatrix = glm::translate(modelMatrix, glm::vec3(0, 0.1 * cos(3 * time_passed), 0));
		modelMatrix = glm::translate(modelMatrix, emerald_coord[i]);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.2, 0.2, 0.2));
		// animatie - rotatie in jurul propriei axe
		modelMatrix = glm::rotate(modelMatrix, time_passed, glm::vec3(0, 1, 0));
		RenderSimpleMesh(meshes["emerald"], shaders["ShaderTema2"], modelMatrix, emerald_color, true, false);
	}
}

// randeaza obstascolele - gauri negre / portale
void Tema3::RenderObstacles() {

	for (int i = 0; i < obstacle_coord.size(); i++) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, obstacle_coord[i]);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.8, 0.4, 0.0001));
		RenderSimpleMesh(meshes["box"], shaders["ShaderTema2"], modelMatrix, glm::vec3(0), true, false, mapTextures["portal"]);
	}
}

// randeaza o platforma 1x1
void Tema3::RenderSmallPlatform(glm::vec3 position, glm::vec3 color) {
	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, position);
	modelMatrix = glm::scale(modelMatrix, glm::vec3(1, 0.2, 1));
	RenderSimpleMesh(meshes["box"], shaders["ShaderTema2"], modelMatrix, color, true, false, mapTextures["platform"], true);
}

// randare sfera
void Tema3::RenderSphere() {
	if (cameraType)
	{
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(sphere_x, sphere_y, sphere_z));
		modelMatrix = glm::scale(modelMatrix, glm::vec3(0.25f));
		RenderSimpleMesh(meshes["sphere"], shaders["ShaderTema2"], modelMatrix, glm::vec3(0.8, 0, 0), true, sphere_deformed || fastMode, mapTextures["ball"]);
	}
}

// randare ufo - contine 2 componente: motor (care lumineazza de tip spot) si corp - texturat
void Tema3::RenderUFO() {

	// randare lumina ozn
	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(ufo.coord));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.175f));
	RenderSimpleMesh(meshes["sphere"], shaders["ShaderTema2"], modelMatrix, ufo.color, true, false, mapTextures["ufo_body"], true);

	// randare corp ozn
	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(ufo.coord));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.5f, 0.1f, 0.5f));
	RenderSimpleMesh(meshes["sphere"], shaders["ShaderTema2"], modelMatrix, glm::vec3(0), true, false, mapTextures["ufo_body"]);

}

// randare stele
void Tema3::RenderStars() {
	for (int i = 0; i < stars.size(); i++) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, stars[i].coord);
		modelMatrix - glm::scale(modelMatrix, glm::vec3(stars[i].radius));
		// stelele au prorietatea de emitere lumina setata pe true
		RenderSimpleMesh(meshes["sphere"], shaders["ShaderTema2"], modelMatrix, stars[i].color, true, false, mapTextures["sun"], NULL, true);
	}
}

// Randare planete
void Tema3::RenderPlanets() {

	for (int i = 0; i < stars.size(); i++) {
		// calcul viteza de rotatie a planetei in jurul stelei
		float xVelocity1 = stars[i].planet1.distanceFromStar * cos(time_passed * stars[i].planet1.velocity);
		float zVelocity1 = stars[i].planet1.distanceFromStar * sin(time_passed * stars[i].planet1.velocity);
		// daca planeta are inel, acesta va fi randat
		if (stars[i].planet1.hasRing) {
			glm::mat4 modelMatrix = glm::mat4(1);

			modelMatrix = glm::translate(modelMatrix, glm::vec3(xVelocity1, 0, zVelocity1));		// rotatia
			modelMatrix = glm::translate(modelMatrix, stars[i].coord);
			modelMatrix = glm::scale(modelMatrix, glm::vec3(stars[i].planet1.radius));
			modelMatrix = glm::rotate(modelMatrix, RADIANS(90), glm::vec3(1, 0, 0));
			modelMatrix = glm::rotate(modelMatrix, -stars[i].planet1.ringAngle, glm::vec3(0, 1, 0));
			RenderSimpleMesh(meshes["ring"], shaders["ShaderTema2"], modelMatrix, stars[i].planet1.color, true, false);
		}
		
		// randare planeta
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, glm::vec3(xVelocity1, 0, zVelocity1));
		modelMatrix = glm::translate(modelMatrix, stars[i].coord);
		modelMatrix = glm::scale(modelMatrix, glm::vec3(stars[i].planet1.radius));
		RenderSimpleMesh(meshes["sphere"], shaders["ShaderTema2"], modelMatrix, stars[i].planet1.color, true, false, mapTextures["planet1"], true);

		// daca steaua are si a 2-a planeta, se repeta pasii anteriori pentru randarea celei de-a 2-a
		if (stars[i].hasPlanet2) {
			float xVelocity2 = stars[i].planet2.distanceFromStar * sin(time_passed * stars[i].planet2.velocity);
			float zVelocity2 = stars[i].planet2.distanceFromStar * cos(time_passed * stars[i].planet2.velocity);
			if (stars[i].planet2.hasRing) {
				glm::mat4 modelMatrix = glm::mat4(1);

				modelMatrix = glm::translate(modelMatrix, glm::vec3(xVelocity2, 0, zVelocity2));		// rotatia
				modelMatrix = glm::translate(modelMatrix, stars[i].coord);
				modelMatrix = glm::scale(modelMatrix, glm::vec3(stars[i].planet2.radius));
				modelMatrix = glm::rotate(modelMatrix, RADIANS(90), glm::vec3(1, 0, 0));
				modelMatrix = glm::rotate(modelMatrix, -stars[i].planet2.ringAngle, glm::vec3(0, 1, 0));
				RenderSimpleMesh(meshes["ring"], shaders["ShaderTema2"], modelMatrix, stars[i].planet2.color, true, false);
			}
			// randare planeta
			glm::mat4 modelMatrix = glm::mat4(1);
			modelMatrix = glm::translate(modelMatrix, glm::vec3(xVelocity2, 0, zVelocity2));
			modelMatrix = glm::translate(modelMatrix, stars[i].coord);
			modelMatrix = glm::scale(modelMatrix, glm::vec3(stars[i].planet2.radius));
			RenderSimpleMesh(meshes["sphere"], shaders["ShaderTema2"], modelMatrix, stars[i].planet2.color, true, false, mapTextures["planet2"], true);
		}	
	}
}


// randare poarta
void Tema3::RenderGate() {
	// partea de sus
	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0.0f, 2.0f, gate_coord));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(3.2f, 0.1f, 0.1f));
	RenderSimpleMesh(meshes["box"], shaders["ShaderTema2"], modelMatrix, glm::vec3(1), true, false, mapTextures["gate"]);
	
	// pilon stanga
	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-1.66f, 1.6f, gate_coord));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f, 1.0f, 0.1f));
	RenderSimpleMesh(meshes["box"], shaders["ShaderTema2"], modelMatrix, glm::vec3(1), true, false, mapTextures["gate"]);
	
	// pilon dreapta
	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(1.66f, 1.6f, gate_coord));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.1f, 1.0f, 0.1f));
	RenderSimpleMesh(meshes["box"], shaders["ShaderTema2"], modelMatrix, glm::vec3(1), true, false, mapTextures["gate"]);

	// baza stanga
	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-2.0f, 1.1f, gate_coord));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 0.4f, 1.0f));
	RenderSimpleMesh(meshes["box"], shaders["ShaderTema2"], modelMatrix, glm::vec3(1), true, false, mapTextures["gate"]);

	// baza dreapta
	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(2.0f, 1.1f, gate_coord));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(1.0f, 0.4, 1.0f));
	RenderSimpleMesh(meshes["box"], shaders["ShaderTema2"], modelMatrix, glm::vec3(1), true, false, mapTextures["gate"]);
}

// miscare / randare skybox
void Tema3::RenderSkybox() {
	skybox_coord = camera->GetTargetPosition();


	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, skybox_coord);
	if (cameraType) modelMatrix = glm::rotate(modelMatrix, RADIANS(60.0f), glm::vec3(1, 0, 0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(144.0f, 90.0f, 100.0f));
	RenderSimpleMesh(meshes["box"], shaders["ShaderTema2"], modelMatrix, glm::vec3(0), true, false, mapTextures["space"]);
}

// randare vieti
void Tema3::RenderHearts() {
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
void Tema3::SetCameraMode() {
	if (cameraType) {
		camera->Set(glm::vec3(0, 3, sphere_z + 2.75), glm::vec3(0, 1, sphere_z), glm::vec3(0, 1, 0));
	}
	else {
		camera->Set(glm::vec3(sphere_x, sphere_y, sphere_z), glm::vec3(sphere_x, sphere_y, sphere_z - 1), glm::vec3(0, 1, 0));
	}
}

// randare fuel bar
void Tema3::RenderFuelBar(float deltaTimeSeconds) {

	// reducere combustibil
	fuel_ammount -= deltaTimeSeconds * forwardSpeed / 100;

	// terminare joc cand combustibilul a ajuns 0
	if (fuel_ammount < 0) {
		gameOver = true;
		std::printf("You have run out of fuel!\n");
	}

	// randare cub galben
	glm::mat4 modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.95, -0.05, 0));
	modelMatrix = glm::translate(modelMatrix, glm::vec3(0, (fuel_ammount - fuel_max_ammount) / 2, 0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05, fuel_ammount, 1));

	RenderSimpleMesh(meshes["box"], shaders["ShaderTema2"], modelMatrix, glm::vec3(0.9, 0.675, 0.15), false, false);

	// randare cub alb (fundal)
	modelMatrix = glm::mat4(1);
	modelMatrix = glm::translate(modelMatrix, glm::vec3(-0.95, -0.05, 0));
	modelMatrix = glm::scale(modelMatrix, glm::vec3(0.05, fuel_max_ammount, 1));

	RenderSimpleMesh(meshes["box"], shaders["ShaderTema2"], modelMatrix, glm::vec3(0.7), false, false);
}


void Tema3::RenderMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix) {
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
void Tema3::RenderSimpleMesh(Mesh* mesh, Shader* shader, const glm::mat4& modelMatrix, glm::vec3 color, bool tridimensionalRender, bool isDeformed, Texture2D* texture, bool is_color_mixed, bool emits_light)
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
	float seed = rand();
	int loc_seed = glGetUniformLocation(shader->program, "seed");
	glUniform1f(loc_seed, seed);

	int is_textured = 0;

	if (texture)
	{
		is_textured = 1;
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->GetTextureID());
		glUniform1i(glGetUniformLocation(shader->program, "texture"), 0);

		int loc_is_mixed = glGetUniformLocation(shader->program, "is_color_mixed");
		glUniform1i(loc_is_mixed, is_color_mixed);
	}

	int loc_is_textured = glGetUniformLocation(shader->program, "is_textured");
	glUniform1i(loc_is_textured, is_textured);


	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~ILUMINARE~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// trimitere pozitie camera la shader
	glm::vec3 eyePosition = camera->position;
	int eye_position = glGetUniformLocation(shader->program, "eye_position");
	glUniform3f(eye_position, eyePosition.x, eyePosition.y, eyePosition.z);

	// Set material property uniforms (shininess, kd, ks, ke)
	int material_shininess = glGetUniformLocation(shader->program, "material_shininess");
	glUniform1i(material_shininess, materialShininess);

	int material_kd = glGetUniformLocation(shader->program, "material_kd");
	glUniform1f(material_kd, materialKd);

	int material_ks = glGetUniformLocation(shader->program, "material_ks");
	glUniform1f(material_ks, materialKs);

	// daca obiectul emite lumina, va avea componenta emisiva mai mare
	int material_ke = glGetUniformLocation(shader->program, "material_ke");
	if (emits_light) glUniform1f(material_ke, 1.0f);
	else glUniform1f(material_ke, materialKe);

	// proprietati spotlight
	glm::vec3 spotlight_position = ufo.coord;
	int loc_spot_pos = glGetUniformLocation(shader->program, "spotlight_position");
	glUniform3f(loc_spot_pos, spotlight_position.x, spotlight_position.y, spotlight_position.z);

	glm::vec3 spotlight_direction = glm::vec3(0, -1, 0);
	int loc_spot_dir = glGetUniformLocation(shader->program, "spotlight_direction");
	glUniform3f(loc_spot_dir, spotlight_direction.x, spotlight_direction.y, spotlight_direction.z);

	glm::vec3 spotlight_color = ufo.color;
	int loc_spot_color = glGetUniformLocation(shader->program, "spotlight_color");
	glUniform3f(loc_spot_color, spotlight_color.x, spotlight_color.y, spotlight_color.z);

	// lumina de tip point

	// numarul de lumini
	int point_count = stars.size();
	int loc_point_count = glGetUniformLocation(shader->program, "point_count");
	glUniform1i(loc_point_count, point_count);

	// pozitii
	int loc_point_coord = glGetUniformLocation(shader->program, "point_coord");
	glUniform3fv(loc_point_coord, star_coord.size(), reinterpret_cast<GLfloat*>(star_coord.data()));

	// culori
	int loc_point_color = glGetUniformLocation(shader->program, "point_color");
	glUniform3fv(loc_point_color, star_color.size(), reinterpret_cast<GLfloat*>(star_color.data()));

	// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

	// Draw the object
	glBindVertexArray(mesh->GetBuffers()->VAO);
	glDrawElements(mesh->GetDrawMode(), static_cast<int>(mesh->indices.size()), GL_UNSIGNED_SHORT, 0);
}

// functie care detecteaza coliziuni intre sfera si platforma
bool Tema3::SphereRectCollision(glm::vec3 sphere_centre, float radius, glm::vec3 rect_centre, glm::vec3 rect_dimensions) {
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
void Tema3::CheckSphereCollision() {
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

// verifica coliziunile sferei cu emeraldele, crescand scorul si eliminand emeraldele lovite
void Tema3::CheckEmeraldCollision() {
	for (int i = 0; i < emerald_coord.size(); i++) {
		if (SphereRectCollision(glm::vec3(sphere_x, sphere_y, sphere_z), 0.25f, emerald_coord[i] + glm::vec3(0, 0.2, 0), glm::vec3(0.1, 0.2, 0.1))) {
			emerald_coord.erase(emerald_coord.begin() + i);
			i--;
			score++;
		}
	}
}

// verifica coliziunile sferei cu obstacolele, scazand numarul de vieti si eliminand obstacolele lovite
void Tema3::CheckObstacleCollision() {
	for (int i = 0; i < obstacle_coord.size(); i++) {
		if (SphereRectCollision(glm::vec3(sphere_x, sphere_y, sphere_z), 0.25f, obstacle_coord[i], glm::vec3(0.4, 0.2, 0.025))) {
			obstacle_coord.erase(obstacle_coord.begin() + i);
			i--;
			lives--;
			std::printf("You ran into an obstacle.\n");
			if (lives <= 0) {
				gameOver = true;
			}
		}
	}
}

// verificare coliziune cu o platforma speciala
void Tema3::CheckSpecialPlatformCollision(glm::vec3 plat_color) {

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


// ******************************************************************** 7 SEGMENTE ******************************************************************************

// afiseaza scorul
void Tema3::DisplayScore() {

	glm::vec3 color = glm::vec3(0);
	Objects::Digit* digit_1 = scoreDigits[0];
	Objects::Digit* digit_2 = scoreDigits[1];
	Objects::Digit* digit_3 = scoreDigits[2];

	// se vor afisa primele 3 cifre pentru scor
	digit_1->DisplayDigit(score % 10);
	digit_2->DisplayDigit((score / 10) % 10);
	digit_3->DisplayDigit((score / 100) % 10);

	// afisare segmente
	for (int i = 0; i < 7; i++) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, scorePosition);
		modelMatrix = glm::scale(modelMatrix, scoreScalingFactor);
		modelMatrix = glm::translate(modelMatrix, digit_1->segments[i]->position);
		if (digit_1->segments[i]->vertical) modelMatrix = glm::rotate(modelMatrix, RADIANS(-90), glm::vec3(0, 0, 1));
		if (digit_1->segments[i]->on) color.y = 1;
		else color.y = 0.5;
		RenderSimpleMesh(digit_1->mesh, shaders["ShaderTema2"], modelMatrix, color, false, false);
	}

	for (int i = 0; i < 7; i++) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, scorePosition + glm::vec3(-0.1, 0, 0));
		modelMatrix = glm::scale(modelMatrix, scoreScalingFactor);
		modelMatrix = glm::translate(modelMatrix, digit_2->segments[i]->position);
		if (digit_2->segments[i]->vertical) modelMatrix = glm::rotate(modelMatrix, RADIANS(-90), glm::vec3(0, 0, 1));
		if (digit_2->segments[i]->on) color.y = 1;
		else color.y = 0.5;
		RenderSimpleMesh(digit_2->mesh, shaders["ShaderTema2"], modelMatrix, color, false, false);
	}

	for (int i = 0; i < 7; i++) {
		glm::mat4 modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, scorePosition + glm::vec3(-0.2, 0, 0));
		modelMatrix = glm::scale(modelMatrix, scoreScalingFactor);
		modelMatrix = glm::translate(modelMatrix, digit_3->segments[i]->position);
		if (digit_3->segments[i]->vertical) modelMatrix = glm::rotate(modelMatrix, RADIANS(-90), glm::vec3(0, 0, 1));
		if (digit_3->segments[i]->on) color.y = 1;
		else color.y = 0.5;
		RenderSimpleMesh(digit_3->mesh, shaders["ShaderTema2"], modelMatrix, color, false, false);
	}
}

void Tema3::FrameEnd()
{
}

void Tema3::OnInputUpdate(float deltaTime, int mods)
{
}

void Tema3::OnKeyPress(int key, int mods)
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
			jumpSpeed = 5;
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

void Tema3::OnKeyRelease(int key, int mods)
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

void Tema3::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
}

void Tema3::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
}

void Tema3::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
}

void Tema3::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}

void Tema3::OnWindowResize(int width, int height)
{
}
