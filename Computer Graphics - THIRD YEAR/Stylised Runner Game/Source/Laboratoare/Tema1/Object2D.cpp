#include "Object2D.h"
#include <Core/Engine.h>
#include <iostream>

using namespace std;

// ********************************************* SAGEATA *******************************************************
// constructor Arrow - apeleaza metoda setArrow
Object2D::Arrow::Arrow() {
	setArrow();
}

// functie care construieste un obiect alcatuit din 2 obiecte (corp sageata + varf)
void Object2D::Arrow::setArrow() {
	// creare corp sageata
	glm::vec3 corner = glm::vec3(0.001, 0.001, 0);
	glm::vec3 color = glm::vec3(1, 1, 1);

	body = Object2D::CreateRectangle("arrow_body", corner, 0.5, 0.04f, color, true);

	// creare varf sageata
	std::vector<VertexFormat> vertices = {
		VertexFormat(glm::vec3(0.5, 0.16, 0), color),
		VertexFormat(glm::vec3(0.5, -0.08, 0), color),
		VertexFormat(glm::vec3(0.7, 0.02, 0), color)
	};
	std::vector<unsigned short> indices = { 0, 1, 2 };

	tip = new Mesh("arrow_tip");
	tip->SetDrawMode(GL_TRIANGLES);
	tip->InitFromData(vertices, indices);
}
// ********************************************* BALON *******************************************************

// constructor Balloon - apeleaza metoda setBalloon
Object2D::Balloon::Balloon(glm::vec3 color) {
	setBalloon(color);
}

// functie care construieste un obiect alcatuit din 2 obiecte (corp balon + ata)
void Object2D::Balloon::setBalloon(glm::vec3 color) {
	// mesh pentru corpul balonului
	glm::vec3 center = glm::vec3(0);

	std::vector<VertexFormat> body_vertices = CreateEllipseCurveArray(-PI/50, 2*PI, 0, 0, 3, 2, 100, color, 0, 1);
	std::vector<unsigned short> body_indices = {};

	for (int i = 0; i < 2 * (100); i++) {
		body_indices.push_back(i);
	}

	if (color == glm::vec3(1, 0, 0)) {
		body = new Mesh("balloon_body_red");
		triangle = CreateTriangle("balloon_triangle_red", center - glm::vec3(0, 0.4f, 0), 0.2f, glm::vec3(1, 0, 0));

	}
	else if (color == glm::vec3(1, 1, 0)) {
		body = new Mesh("balloon_body_yellow");
		triangle = CreateTriangle("balloon_triangle_yellow", center - glm::vec3(0, 0.4f, 0), 0.2f, glm::vec3(1, 1, 0));

	}

	// setare mod desenare
	body->SetDrawMode(GL_TRIANGLE_STRIP);
	body->InitFromData(body_vertices, body_indices);
	
	// creare fir balon
	glm::vec3 color_str = glm::vec3(1, 1, 1);
	std:vector<VertexFormat> str_vertices = {
		VertexFormat(glm::vec3(-0, -0.6, 0), color_str),
		VertexFormat(glm::vec3(-0.1, -0.7, 0), color_str),
		VertexFormat(glm::vec3(0.1, -0.8, 0), color_str),
		VertexFormat(glm::vec3(-0.08, -0.9, 0), color_str),
		VertexFormat(glm::vec3(0.1, -1.0, 0), color_str)
	};
	std::vector<unsigned short> str_indices = {0, 1, 2, 3, 4};

	string = new Mesh("balloon_string");
	string->SetDrawMode(GL_LINE_STRIP);
	string->InitFromData(str_vertices, str_indices);

}

// ********************************************* HERO *******************************************************

Object2D::Hero::Hero() {
	setHero();
}

void Object2D::Hero::setHero() {
	body = CreateHeroBody();

	std::vector<VertexFormat> head_vertices = CreateEllipseCurveArray(- PI / 50, 2 * PI, 0.125, 0.65, 1, 1, 100, glm::vec3(0, 0, 1), 0.1, 0);
	std::vector<unsigned short> head_indices = {};

	for (unsigned short i = 0; i < 2 * (100); i++) {
		head_indices.push_back(i);
	}

	head = new Mesh("hero_head");
	head->SetDrawMode(GL_TRIANGLE_STRIP);
	head->InitFromData(head_vertices, head_indices);
}


// functie care genereaza un arc de elipsa
// primeste unghiul descris de arc, coordonatele initiale, coeficientii ecuatiei, numarul de fragmente, culoarea si 2 raze a caror diferenta o reprezinta grosimea arcului
std::vector<VertexFormat> Object2D::CreateEllipseCurveArray(float startAngle, float endAngle, float x, float y, float x_param, float y_param,
	int fragments, glm::vec3 color, float r1, float r2)
{
	std::vector<VertexFormat> result;

	float increment = (endAngle - startAngle) / fragments;

	for (float currAngle = startAngle; currAngle <= endAngle; currAngle += increment)
	{
		result.push_back(VertexFormat(glm::vec3(r1 * cos(currAngle) / x_param + x, r1 * sin(currAngle) / y_param + y, 0), color));
		result.push_back(VertexFormat(glm::vec3(r2 * cos(currAngle) / x_param + x, r2 * sin(currAngle) / y_param + y, 0), color));
	}

	return result;
}

// intoarce un mesh in forma de inima
Mesh* Object2D::CreateHeart(std::string name, glm::vec3 color) {
	float startAngle = 0;
	float endAngle = 2 * PI;
	int fragments = 100;

	float r = 0.01f;
	float increment = (endAngle - startAngle) / fragments;

	std::vector<VertexFormat> vertices = {};
	for (float currAngle = startAngle; currAngle <= endAngle; currAngle += increment) {
		float x = r * 16 * sin(currAngle) * sin(currAngle) * sin(currAngle);
		float y = r * (13 * cos(currAngle) - 5 * cos(2 * currAngle) - 2 * cos(3 * currAngle) - cos(4 * currAngle));
		vertices.push_back(VertexFormat(glm::vec3(x, y, 0), color));
	}

	std::vector<unsigned short> indices = {};
	for (int i = 0; i <= fragments; i++) {
		indices.push_back(i);
	}

	Mesh* heart = new Mesh(name);
	heart->SetDrawMode(GL_TRIANGLE_FAN);
	heart->InitFromData(vertices, indices);

	return heart;
}

// functie care intoarce mesh-ul arcului
Mesh* Object2D::CreateBow(float bow_width) {
	// setare colt stanga-jos
	glm::vec3 corner = glm::vec3(0.001, 0.001, 0);

	// setare culoare
	glm::vec3 color = glm::vec3(0, 0, 1);

	// numar de fragmente (puncte de pe curba arcului)
	int fragments = 100;

	// arc de elipsa reprezentand partea curba a arcului
	std::vector<VertexFormat> vertices = CreateEllipseCurveArray(-PI/2, PI/2, 0, bow_width/2, 1.25, 1, fragments, color, bow_width, 1.1 * bow_width);

	// "ata" arcului (partea dreptunghiulara)
	vertices.push_back(VertexFormat(glm::vec3(-0.01, bow_width + bow_width / 2, 0), color));
	vertices.push_back(VertexFormat(glm::vec3(-0.01, -bow_width / 2, 0), color));
	vertices.push_back(VertexFormat(glm::vec3(0, bow_width, 0), color));
	vertices.push_back(VertexFormat(glm::vec3(0, -bow_width / 2, 0), color));

	// asignare indici
	std::vector<unsigned short> indices = {};
	Mesh* bow = new Mesh("bow");
	for (int i = 0; i < 2*(fragments + 2); i++) {
		indices.push_back(i);
	}
	
	// setare mod desenare
	bow->SetDrawMode(GL_TRIANGLE_STRIP);
	bow->InitFromData(vertices, indices);

	return bow;
}

// creaza un mesh format din 4 triunghiuri in forma de shuriken
Mesh* Object2D::CreateShuriken(std::string name, glm::vec3 center, float edge, glm::vec3 color) {
	std::vector<VertexFormat> vertices = {
		VertexFormat(center, color),
		VertexFormat(center + glm::vec3(0, -edge, 0), color),
		VertexFormat(center + glm::vec3(edge, -edge, 0), color),
		VertexFormat(center + glm::vec3(edge, 0, 0), color),
		VertexFormat(center + glm::vec3(edge, edge, 0), color),
		VertexFormat(center + glm::vec3(0, edge, 0), color),
		VertexFormat(center + glm::vec3(-edge, edge, 0), color),
		VertexFormat(center + glm::vec3(-edge, 0, 0), color),
		VertexFormat(center + glm::vec3(-edge, -edge, 0), color)

	};

	std::vector<unsigned short> indices = {
		0, 1, 2,
		0, 3, 4,
		0, 5, 6,
		0, 7, 8
	};

	Mesh* shuriken = new Mesh(name);
	shuriken->SetDrawMode(GL_TRIANGLES);
	shuriken->InitFromData(vertices, indices);
	return shuriken;
}

// functie care creaza un dreptunghi
Mesh* Object2D::CreateRectangle(std::string name, glm::vec3 leftBottomCorner, float width, float height, glm::vec3 color, bool fill) {
	glm::vec3 corner = leftBottomCorner;
	std::vector<VertexFormat> vertices = {
		VertexFormat(corner, color),
		VertexFormat(corner + glm::vec3(width, 0, 0), color),
		VertexFormat(corner + glm::vec3(width, height, 0), color),
		VertexFormat(corner + glm::vec3(0, height, 0), color)
	};

	Mesh* rectangle = new Mesh(name);
	std::vector<unsigned short> indices = { 0, 1, 2, 3 };

	if (!fill) {
		rectangle->SetDrawMode(GL_LINE_LOOP);
	}
	else {
		indices.push_back(0);
		indices.push_back(2);
	}
	rectangle->InitFromData(vertices, indices);
	return rectangle;
}

// functie care creaza un triunghi echilateral
Mesh* Object2D::CreateTriangle(std::string name, glm::vec3 upVertex, float height, glm::vec3 color) {
	Mesh* triangle = new Mesh(name);
	std:vector<VertexFormat> vertices = {
		VertexFormat(upVertex, color),
		VertexFormat(upVertex + glm::vec3(-height / 2, -height, 0), color),
		VertexFormat(upVertex + glm::vec3(height / 2, -height, 0), color)
	};

	std::vector<unsigned short> indices = {0, 1, 2, 0, 2};
	triangle->InitFromData(vertices, indices);
	return triangle;
}

// creaza un mesh cu corpul eroului
Mesh* Object2D::CreateHeroBody() {
	Mesh* body = new Mesh("hero_body");

	glm::vec3 color = glm::vec3(0, 0, 1);
	
	std::vector<VertexFormat> vertices = {
		// picior stang
		VertexFormat(glm::vec3(0, 0, 0), color),
		VertexFormat(glm::vec3(0.1, 0, 0), color),
		VertexFormat(glm::vec3(0, 0.2, 0), color),

		VertexFormat(glm::vec3(0.1, 0, 0), color),
		VertexFormat(glm::vec3(0.1, 0.2, 0), color),
		VertexFormat(glm::vec3(0, 0.2, 0), color),

		// picior drept
		VertexFormat(glm::vec3(0.15, 0, 0), color),
		VertexFormat(glm::vec3(0.25, 0, 0), color),
		VertexFormat(glm::vec3(0.15, 0.2, 0), color),

		VertexFormat(glm::vec3(0.25, 0, 0), color),
		VertexFormat(glm::vec3(0.25, 0.2, 0), color),
		VertexFormat(glm::vec3(0.15, 0.2, 0), color),

		// corp
		VertexFormat(glm::vec3(0, 0.2, 0), color),
		VertexFormat(glm::vec3(0.25, 0.2, 0), color),
		VertexFormat(glm::vec3(0, 0.5, 0), color),

		VertexFormat(glm::vec3(0, 0.5, 0), color),
		VertexFormat(glm::vec3(0.25, 0.2, 0), color),
		VertexFormat(glm::vec3(0.25, 0.5, 0), color),

		// mana dreapta
		VertexFormat(glm::vec3(0.27, 0.5, 0), color),
		VertexFormat(glm::vec3(0.27, 0.3, 0), color),
		VertexFormat(glm::vec3(0.33, 0.3, 0), color),

		VertexFormat(glm::vec3(0.33, 0.3, 0), color),
		VertexFormat(glm::vec3(0.27, 0.5, 0), color),
		VertexFormat(glm::vec3(0.33, 0.5, 0), color),


		// mana stanga
		VertexFormat(glm::vec3(-0.02, 0.5, 0), color),
		VertexFormat(glm::vec3(-0.02, 0.3, 0), color),
		VertexFormat(glm::vec3(-0.08, 0.3, 0), color),

		VertexFormat(glm::vec3(-0.08, 0.3, 0), color),
		VertexFormat(glm::vec3(-0.02, 0.5, 0), color),
		VertexFormat(glm::vec3(-0.08, 0.5, 0), color),

	};
	std::vector<unsigned short> indices = {};
	for (unsigned short i = 0; i < 30; i++) {
		indices.push_back(i);
	}


	body->SetDrawMode(GL_TRIANGLES);
	body->InitFromData(vertices, indices);

	return body;
}