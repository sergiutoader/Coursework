#include "Tema1.h"
#include <vector>
#include <iostream>
#include <Core/Engine.h>
#include "Transform2D.h"
#include "Object2D.h"
#include "Collisions.h"
#include <cmath>

using namespace std;

Tema1::Tema1()
{
}

Tema1::~Tema1()
{
}


// functie care intoarce un float random in intervalul [LO, HI]
float Tema1::getRandFloat(float LO, float HI) {
	return LO + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (HI - LO)));
}

// functie care intoarce true daca punctul se afla in spatiul logic
bool Tema1::isInLogicSpace(float x, float y) {
	if (x < logicSpace.x) return false;
	if (y < logicSpace.y) return false;
	if (x > logicSpace.width) return false;
	if (y > logicSpace.height) return false;
	return true;
}


void Tema1::Init()
{ 
	auto camera = GetSceneCamera();
	camera->SetPosition(glm::vec3(0, 0, 50));
	camera->SetRotation(glm::vec3(0, 0, 0));
	camera->Update();
	GetCameraInput()->SetActive(false);

	logicSpace.x = 0;		// logic x
	logicSpace.y = 0;		// logic y
	logicSpace.width = 16;	// logic width
	logicSpace.height = 9;	// logic height

	// creare sageata, baloane si erou
	Object2D::Arrow* arrow = new Object2D::Arrow();
	Object2D::Balloon* balloon_red = new Object2D::Balloon(glm::vec3(1, 0, 0));
	Object2D::Balloon* balloon_yellow = new Object2D::Balloon(glm::vec3(1, 1, 0));
	Object2D::Hero* hero = new Object2D::Hero();

	// adaugare componente la lista de obiecte
	AddMeshToList(balloon_red->body);
	AddMeshToList(balloon_yellow->body);
	AddMeshToList(balloon_red->triangle);
	AddMeshToList(balloon_yellow->triangle);
	AddMeshToList(balloon_yellow->string);
	AddMeshToList(hero->head);
	AddMeshToList(hero->body);

	// adaugare componente sageata la lista de obiecte
	AddMeshToList(arrow->body);
	AddMeshToList(arrow->tip);

	// initializare variabile sageata
	arrow_vel_x = 0;
	arrow_vel_y = 0;
	// initial sageata nu este aruncata
	isArrowFired = false;
	// viteza initiala sageata
	arrow_speed = 5;
	arrow_tip_x = 0;
	arrow_tip_y = 0;

	// initializare latime arc, coordonate initiale arc, margini arc, unghi arc
	bow_width = 0.55;
	bow_x = 0.7;
	bow_y = (logicSpace.height - bow_width) / 2;
	bow_edge = 0.1;
	bow_angle = 0;

	// initializare coordonate erou
	hero_x = 0.1;
	hero_y = bow_y + (bow_width - 0.75) / 2;
	
	isMousePressed = false;

	// creare arc
	Mesh* bow = Object2D::CreateBow(bow_width);
	AddMeshToList(bow);

	// creare shuriken
	Mesh* shuriken = Object2D::CreateShuriken("shuriken", glm::vec3(0), 0.25, glm::vec3(1));
	AddMeshToList(shuriken);

	// initializare parametrii powerbar
	powerbar_x = bow_x;
	powerbar_y = bow_y - bow_width / 2 - 0.25f;
	powerbar_width = 0.2f;
	powerbar_height = 0.05f;
	powerbar_scaling_factor = 0;
	
	// mesh powerbar
	Mesh* powerbar = Object2D::CreateRectangle("powerbar", glm::vec3(powerbar_width / 2, 0, 0), powerbar_width, powerbar_height, glm::vec3(1),  true);
	AddMeshToList(powerbar);
	// mesh viata plina
	Mesh* full_heart = Object2D::CreateHeart("full_heart", glm::vec3(1, 0, 0));
	AddMeshToList(full_heart);
	// mesh viata goala
	Mesh* empty_heart = Object2D::CreateHeart("empty_heart", glm::vec3(1, 0.8, 0.8));
	AddMeshToList(empty_heart);

	// variabile pentru intervale
	incr_var = 0;
	interval_1 = 0;
	interval_2 = 0;
	interval_3 = 0;
	interval_tick_1 = false;
	interval_tick_2 = false;
	interval_tick_3 = false;
	interval_1_limit = 3;

	// initializare variabile pentru shuriken si baloane
	shuriken_speed = -1;
	shuriken_coord = {};
	balloon_speed = 1;
	balloon_coord = {};

	// vector obiecte lovite
	hit_targets = {};

	// frecventa de aparitie a baloanelor rosii = 65%
	red_balloon_frequency = 0.65;

	// scor, vieti si gameOver
	score = 0;
	lives = 3;
	gameOver = false;
}

// uniform 2D visualization matrix (same scale factor on x and y axes)
glm::mat3 Tema1::VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace)
{
	float sx, sy, tx, ty, smin;
	sx = viewSpace.width / logicSpace.width;
	sy = viewSpace.height / logicSpace.height;
	if (sx < sy)
		smin = sx;
	else
		smin = sy;
	tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
	ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

	return glm::transpose(glm::mat3(
		smin, 0.0f, tx,
		0.0f, smin, ty,
		0.0f, 0.0f, 1.0f));
}

void Tema1::SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor, bool clear)
{
	glViewport(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

	glEnable(GL_SCISSOR_TEST);
	glScissor(viewSpace.x, viewSpace.y, viewSpace.width, viewSpace.height);

	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(colorColor.r, colorColor.g, colorColor.b, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_SCISSOR_TEST);

	GetSceneCamera()->SetOrthographic((float)viewSpace.x, (float)(viewSpace.x + viewSpace.width), (float)viewSpace.y, (float)(viewSpace.y + viewSpace.height), 0.1f, 400);
	GetSceneCamera()->Update();
}

void Tema1::FrameStart()
{
	// Clears the color buffer (using the previously set color) and depth buffer
	glClearColor(0, 0, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

}

void Tema1::FrameEnd() {}


void Tema1::Update(float deltaTimeSeconds)
{	
	// scena nu se mai deseneaza daca jocul este incheiat
	if (!gameOver) {
		// jocul se termina cand se pierd toate vietile
		if (lives <= 0) {
			gameOver = true;
			cout << "Game over! Score = " << score << ". Press ENTER to start a new game." << endl;
		}

		// setare dificultate in functie de scor
		IncreaseDifficulty();

		// incrementare variabila pentru calculul rotatiilor continue (Shuriken) + variabile pentru calculul intervalelor
		incr_var += deltaTimeSeconds;
		interval_1 += deltaTimeSeconds;
		interval_2 += deltaTimeSeconds;
		interval_3 += deltaTimeSeconds;

		// activare interval_tick_1 la fiecare 3 secunde (sau mai putin in functie de dificultate)
		if (interval_1 > interval_1_limit) {
			interval_1 = 0;
			interval_tick_1 = true;
		} 

		// activare interval_tick_2 la fiecare 0.8 secunde
		if (interval_2 > 0.8) {
			interval_2 = 0;
			interval_tick_2 = true;
		}

		if (interval_3 > 3) {
			interval_3 = 0;
			interval_tick_3 = true;
		}

		// la fiecare 3 secunde, se afiseaza scor si vieti ramase in consola
		if (interval_tick_3) {
			cout << "Score: " << score << endl;
			interval_tick_3 = false;
		}

		// generarea obiectelor noir pe ecran
		GenerateNewObjects();

		// miscarea obiectelor pe ecran (baloane + shuriken)
		MoveObjects(deltaTimeSeconds);


		// unghiul de rotatie al arcului
		bow_angle = atan((logic_mouseY - bow_y - bow_width / 2) / (logic_mouseX - bow_x));

		// miscarea sagetii - pe arc
		if (!isArrowFired) {
			arrow_x = bow_x;
			arrow_y = bow_y + bow_width / 2;
			// unghi sageata - similar cu al arcului
			arrow_angle = atan((logic_mouseY - arrow_y) / (logic_mouseX - arrow_x));

			// daca mouse-ul este apasat, creste viteza cu care va fi lansata sageata
			if (isMousePressed) {
				if (arrow_speed <= 15) {
					arrow_speed += 10 * deltaTimeSeconds;
				}
			}

			// setare latime powerbar
			powerbar_scaling_factor = (arrow_speed - 5) / 3;
		}
		//  miscarea sagetii - dupa ce a fost lansata
		else {
			// calcul miscare sageata in aer
			ComputeArrowMovement(deltaTimeSeconds);
			
			// powerbar-ul dispare cand sageata este in aer
			powerbar_scaling_factor = 0;
			
			// detectare coliziuni cu sageata
			DetectArrowCollisions();
		}
		// detectare coliziuni cu arcul
		DetectBowCollisions();

		// actualizare pozitie powerbar
		powerbar_y = bow_y - 0.5f;

		// actualizare pozitie erou
		hero_y = bow_y + (bow_width - 1.125) / 2;

		glm::ivec2 resolution = window->GetResolution();

		// Sets the screen area where to draw - the left half of the window
		viewSpace = ViewportSpace(0, 0, resolution.x, resolution.y);
		SetViewportArea(viewSpace, glm::vec3(0), true);

		// Compute uniform 2D visualization matrix
		visMatrix = glm::mat3(1);
		visMatrix *= VisualizationTransf2DUnif(logicSpace, viewSpace);

		DrawScene(visMatrix, deltaTimeSeconds);
	}
}
// metoda care deseneaza scena
void Tema1::DrawScene(glm::mat3 visMatrix, float deltaTimeSeconds)
{
	// randare inimi
	for (int i = 0; i < lives; i++) {
		modelMatrix = visMatrix;
		modelMatrix *= Transform2D::Translate(logicSpace.width - 1.5f + 0.4f * i, logicSpace.height - 0.25f);
		RenderMesh2D(meshes["full_heart"], shaders["VertexColor"], modelMatrix);
	}
	for (int i = lives; i < 3; i++) {
		modelMatrix = visMatrix;
		modelMatrix *= Transform2D::Translate(logicSpace.width - 1.5f + 0.4f * i, logicSpace.height - 0.25f);
		RenderMesh2D(meshes["empty_heart"], shaders["VertexColor"], modelMatrix);
	}
	// randare arc
	modelMatrix = visMatrix;  
	modelMatrix *= Transform2D::Translate(bow_x, bow_y);
	modelMatrix *= Transform2D::Translate(0, bow_width / 2);
	modelMatrix *= Transform2D::Rotate(bow_angle);
	modelMatrix *= Transform2D::Translate(0, -bow_width / 2);
	RenderMesh2D(meshes["bow"], shaders["VertexColor"], modelMatrix);

	// randare powerbar
	modelMatrix = visMatrix;
	modelMatrix *= Transform2D::Translate(powerbar_x, powerbar_y);
	modelMatrix *= Transform2D::Scale(powerbar_scaling_factor, 1);
	modelMatrix *= Transform2D::Translate(-powerbar_width / 1.5, 0);
	RenderMesh2D(meshes["powerbar"], shaders["VertexColor"], modelMatrix);

	// randare sageata
	modelMatrix = visMatrix;
	modelMatrix *= Transform2D::Translate(arrow_x, arrow_y);
	modelMatrix *= Transform2D::Rotate(arrow_angle);
	RenderArrow(modelMatrix);

	// randare shurikene
	for (int i = 0; i < shuriken_coord.size(); i++) {
		modelMatrix = visMatrix;
		modelMatrix *= Transform2D::Translate(shuriken_coord[i].x, shuriken_coord[i].y);
		modelMatrix *= Transform2D::Rotate(incr_var);
		RenderMesh2D(meshes["shuriken"], shaders["VertexColor"], modelMatrix);
	}
	// randare baloane
	for (int i = 0; i < balloon_coord.size(); i++) {
		modelMatrix = visMatrix;
		modelMatrix *= Transform2D::Translate(balloon_coord[i].x, balloon_coord[i].y);

		RenderMesh2D(meshes["balloon_string"], shaders["VertexColor"], modelMatrix);

		if (balloon_coord[i].color == glm::vec3(1, 0, 0)) {
			RenderMesh2D(meshes["balloon_body_red"], shaders["VertexColor"], modelMatrix);
			RenderMesh2D(meshes["balloon_triangle_red"], shaders["VertexColor"], modelMatrix);
		}
		else if (balloon_coord[i].color == glm::vec3(1, 1, 0)) {
			RenderMesh2D(meshes["balloon_body_yellow"], shaders["VertexColor"], modelMatrix);
			RenderMesh2D(meshes["balloon_triangle_yellow"], shaders["VertexColor"], modelMatrix);
		}
	}

	// randare hit_targets
	for (int i = 0; i < hit_targets.size(); i++) {
		modelMatrix = visMatrix;
		modelMatrix *= Transform2D::Translate(hit_targets[i].x, hit_targets[i].y);
		if (hit_targets[i].color == glm::vec3(1)) {
			modelMatrix *= Transform2D::Rotate(incr_var);
			modelMatrix *= Transform2D::Scale(hit_targets[i].scaling_factor, hit_targets[i].scaling_factor);
			RenderMesh2D(meshes["shuriken"], shaders["VertexColor"], modelMatrix);
		}
		modelMatrix *= Transform2D::Scale(hit_targets[i].scaling_factor, hit_targets[i].scaling_factor);

		if (hit_targets[i].color == glm::vec3(1, 0, 0)) {
			RenderMesh2D(meshes["balloon_string"], shaders["VertexColor"], modelMatrix);
			RenderMesh2D(meshes["balloon_body_red"], shaders["VertexColor"], modelMatrix);
			RenderMesh2D(meshes["balloon_triangle_red"], shaders["VertexColor"], modelMatrix);
		}
		else if (hit_targets[i].color == glm::vec3(1, 1, 0)) {
			RenderMesh2D(meshes["balloon_string"], shaders["VertexColor"], modelMatrix);
			RenderMesh2D(meshes["balloon_body_yellow"], shaders["VertexColor"], modelMatrix);
			RenderMesh2D(meshes["balloon_triangle_yellow"], shaders["VertexColor"], modelMatrix);
		}
	}

	modelMatrix = visMatrix;
	modelMatrix *= Transform2D::Translate(hero_x, hero_y);
	modelMatrix *= Transform2D::Scale(1.5, 1.5);
	RenderMesh2D(meshes["hero_head"], shaders["VertexColor"], modelMatrix);
	RenderMesh2D(meshes["hero_body"], shaders["VertexColor"], modelMatrix);

}

// functie care aplica functia RenderMesh2D pe ambele componente ale sagetii
void Tema1::RenderArrow(glm::mat3 modelMatrix) {
	RenderMesh2D(meshes["arrow_body"], shaders["VertexColor"], modelMatrix);
	RenderMesh2D(meshes["arrow_tip"], shaders["VertexColor"], modelMatrix);
}

void Tema1::DetectArrowCollisions() {
	// detectare coliziune cu shuriken
	for (int i = 0; i < shuriken_coord.size(); i++) {
		if (PointToCircleCollision(arrow_tip_x, arrow_tip_y, shuriken_coord[i].x, shuriken_coord[i].y, 0.25)) {

			// adaugare shuriken in multimea de obiecte hit_targets
			shuriken_coord[i].scaling_factor = 1.0;
			hit_targets.push_back(shuriken_coord[i]);

			shuriken_coord.erase(shuriken_coord.begin() + i);
			i--;

			// actualizare scor
			score += 5;
		}
	}

	// detectare coliziune cu baloane
	for (int i = 0; i < balloon_coord.size(); i++) {
		if (PointToEllipseCollision(arrow_tip_x, arrow_tip_y, balloon_coord[i].x, balloon_coord[i].y, 1, 1.0 / 3, 1.0 / 2)) {

			// actualizare scor
			if (balloon_coord[i].color == glm::vec3(1, 0, 0)) {
				score += 10;
			}
			else {
				score -= 10;
			}

			// adaugare balon la vectorul de tinte lovite cu factorul 1
			balloon_coord[i].scaling_factor = 1.0;
			hit_targets.push_back(balloon_coord[i]);

			balloon_coord.erase(balloon_coord.begin() + i);
			i--;
		}
	}
}

void Tema1::DetectBowCollisions() {
	// coliziune arc-shuriken
	for (int i = 0; i < shuriken_coord.size(); i++) {
		if (CircleToCircleCollision(bow_x, bow_y + bow_width / 2, 0.55f, shuriken_coord[i].x, shuriken_coord[i].y, 0.25)) {
			// adaugare shuriken la hit_targets
			shuriken_coord[i].scaling_factor = 1.0;
			hit_targets.push_back(shuriken_coord[i]);

			shuriken_coord.erase(shuriken_coord.begin() + i);
			i--;
			// se reduce numarul de vieti
			lives--;
		}
	}
}

void Tema1::MoveObjects(float deltaTimeSeconds) {
	// miscare shuriken-uri
	for (int i = 0; i < shuriken_coord.size(); i++) {
		shuriken_coord[i].x += shuriken_speed * deltaTimeSeconds;
		// eliminare shuriken-uri care nu se mai afla pe ecran
		if (shuriken_coord[i].x < -2) {
			shuriken_coord.erase(shuriken_coord.begin() + i);
			i--;
		}
	}

	// miscare baloane
	for (int i = 0; i < balloon_coord.size(); i++) {
		balloon_coord[i].y += balloon_speed * deltaTimeSeconds;
		// eliminare baloane care nu se mai afla pe ecran
		if (balloon_coord[i].y > logicSpace.height + 1) {
			balloon_coord.erase(balloon_coord.begin() + i);
			i--;
		}
	}

	// miscorare scaling_factor pentru obiectele lovite si eliminarea lor cand acesta devine negativ
		// animatie de cadere pentru obiectele lovite
	for (int i = 0; i < hit_targets.size(); i++) {
		hit_targets[i].scaling_factor -= deltaTimeSeconds;
		hit_targets[i].y -= 2 * deltaTimeSeconds;
		if (hit_targets[i].scaling_factor < 0) {
			hit_targets.erase(hit_targets.begin() + i);
			i--;
		}
	}
}

void Tema1::GenerateNewObjects() {
	// generare shuriken nou cu coordonata y random la fiecare 3 secunde
	if (interval_tick_1) {
		interval_tick_1 = false;

		point new_shuriken;
		new_shuriken.x = logicSpace.width - 0.25;
		new_shuriken.y = getRandFloat(0.125, logicSpace.height - 0.125);
		new_shuriken.color = glm::vec3(1);
		shuriken_coord.push_back(new_shuriken);
	}

	// generare balon nou cu coordonata x random la fiecare 0.8 secunde
	if (interval_tick_2) {
		interval_tick_2 = false;

		point new_balloon;
		new_balloon.x = getRandFloat(2.3, logicSpace.width);
		new_balloon.y = -0.5;

		// se genereaza baloane rosii si galbene cu probabilitati diferite
		float color_rand = getRandFloat(0, 1);
		if (color_rand < red_balloon_frequency) {
			new_balloon.color = glm::vec3(1, 0, 0);
		}
		else {
			new_balloon.color = glm::vec3(1, 1, 0);
		}

		balloon_coord.push_back(new_balloon);
	}
}

// creste dificultatea jocului pe masura ce scorul jucatorului creste
void Tema1::IncreaseDifficulty() {

	// se mareste viteza shuriken-urilor, frecventa lor de aparitie, si probabilitatea ca baloanele care apar sa fie galbene
	if (score >= 100 && score < 200) {
		shuriken_speed = -1.2;
		interval_1_limit = 2.5;
		red_balloon_frequency = 0.6;
	}
	else if (score >= 200 && score < 300) {
		shuriken_speed = -1.5;
		interval_1_limit = 1.8;
		red_balloon_frequency = 0.55;
	}
	else if (score >= 300 && score < 400) {
		shuriken_speed = -2;
		interval_1_limit = 1.2;
		red_balloon_frequency = 0.5;
	}
	else if (score >= 400 && score < 500) {
		shuriken_speed = -2.5;
		interval_1_limit = 0.7;
		red_balloon_frequency = 0.45;
	}
	else if (score >= 500) {
		shuriken_speed = -3;
		interval_1_limit = 0.3;
		red_balloon_frequency = 0.4;
	}
}

void Tema1::ComputeArrowMovement(float deltaTimeSeconds) {
	// sageata se va misca in functie de unghiul la care se afla cand a fost lansata
	arrow_vel_x = arrow_speed * deltaTimeSeconds * cos(arrow_angle);
	arrow_vel_y = arrow_speed * deltaTimeSeconds * sin(arrow_angle);

	arrow_x += arrow_vel_x;
	arrow_y += arrow_vel_y;

	// cand sageata iese din logicspace, se reseteaza viteza sagetii, se pune inapoi pe arc
	if (!isInLogicSpace(arrow_x, arrow_y)) {
		isArrowFired = false;
		arrow_vel_x = 0;
		arrow_vel_y = 0;
		arrow_speed = 5;
	}

	// calcul varf sageata - pentru detectarea coliziunilor
	arrow_tip_x = arrow_x + 0.7f * cos(arrow_angle);
	arrow_tip_y = arrow_y + 0.02f + 0.7f * sin(arrow_angle);
}


void Tema1::OnInputUpdate(float deltaTime, int mods)
{
	// miscare arc in sus
	if (window->KeyHold(GLFW_KEY_W) == true) {
		
		if (bow_width + bow_edge + bow_y <= logicSpace.height) {
			bow_y += 5*deltaTime;
		}
	}

	// miscare arc in jos
	if (window->KeyHold(GLFW_KEY_S) == true) {
		if (bow_y - bow_edge >= logicSpace.y) {
			bow_y -= 5*deltaTime;
		}
	}
	// resetare joc la apasarea tastei Enter (atunci cand jocul s-a incheiat)
	if (window->KeyHold(GLFW_KEY_ENTER) == true) {
		if (gameOver) {
			gameOver = false;
			Init();
		}
	}
} 

void Tema1::OnKeyPress(int key, int mods)
{

}

void Tema1::OnKeyRelease(int key, int mods)
{
	// add key release event
}

void Tema1::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
	// trecere de la coordonate ale ecranului la coordonate logice pentru mouse
	float sx, sy, tx, ty, smin;
	sx = viewSpace.width / logicSpace.width;
	sy = viewSpace.height / logicSpace.height;
	if (sx < sy)
		smin = sx;
	else
		smin = sy;
	tx = viewSpace.x - smin * logicSpace.x + (viewSpace.width - smin * logicSpace.width) / 2;
	ty = viewSpace.y - smin * logicSpace.y + (viewSpace.height - smin * logicSpace.height) / 2;

	// calcul coordonate logice mouse
	logic_mouseX = ((float)mouseX - tx) / smin;
	logic_mouseY = logicSpace.height - ((float)mouseY - ty) / smin;

	// setare coordonate logice pe 0 daca acestea devin negative (cursorul este inafara spatiului logic)
	if (logic_mouseX <= 0) {
		logic_mouseX = 0.001f;
	}
	if (logic_mouseY <= 0) {
		logic_mouseY = 0.001f;
	}
}

void Tema1::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
	isMousePressed = true;
}

void Tema1::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
	// atunci cand se elibereaza click stanga, se da drumul la sageata
	isArrowFired = true;
	isMousePressed = false;
}

void Tema1::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}