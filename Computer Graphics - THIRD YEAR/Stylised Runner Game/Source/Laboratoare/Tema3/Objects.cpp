#include "Objects.h"
#include <Core/Engine.h>

using namespace std;

// initializare orientare si pozitie segmente
Objects::Digit::Digit() {
	this->mesh = CreateDisplaySegment(glm::vec3(0));

	segments = {};

	for (int i = 0; i < 7; i++) {
		segments.push_back(new Segment());
		segments[i]->on = false;
		segments[i]->vertical = true;
	}

	segments[0]->vertical = false;
	segments[3]->vertical = false;
	segments[6]->vertical = false;

	segments[0]->position = glm::vec3(0);
	segments[1]->position = glm::vec3(1.05, -0.05, 0);
	segments[2]->position = glm::vec3(1.05, -1.15, 0);
	segments[3]->position = glm::vec3(0, -2.2, 0);
	segments[4]->position = glm::vec3(-0.05, -1.15, 0);
	segments[5]->position = glm::vec3(-0.05, -0.05, 0);
	segments[6]->position = glm::vec3(0, -1.1, 0);

}

// aprindere segmente corespunzatoare fiecarei cifre
void Objects::Digit::DisplayDigit(int digit) {
	switch (digit) {
	case(0):
		segments[0]->on = true;
		segments[1]->on = true;
		segments[2]->on = true;
		segments[3]->on = true;
		segments[4]->on = true;
		segments[5]->on = true;
		segments[6]->on = false;
		break;
	case(1):
		segments[0]->on = false;
		segments[1]->on = true;
		segments[2]->on = true;
		segments[3]->on = false;
		segments[4]->on = false;
		segments[5]->on = false;
		segments[6]->on = false;
		break;
	case(2):
		segments[0]->on = true;
		segments[1]->on = true;
		segments[2]->on = false;
		segments[3]->on = true;
		segments[4]->on = true;
		segments[5]->on = false;
		segments[6]->on = true;
		break;
	case(3):
		segments[0]->on = true;
		segments[1]->on = true;
		segments[2]->on = true;
		segments[3]->on = true;
		segments[4]->on = false;
		segments[5]->on = false;
		segments[6]->on = true;
		break;
	case(4):
		segments[0]->on = false;
		segments[1]->on = true;
		segments[2]->on = true;
		segments[3]->on = false;
		segments[4]->on = false;
		segments[5]->on = true;
		segments[6]->on = true;
		break;
	case(5):
		segments[0]->on = true;
		segments[1]->on = false;
		segments[2]->on = true;
		segments[3]->on = true;
		segments[4]->on = false;
		segments[5]->on = true;
		segments[6]->on = true;
		break;
	case(6):
		segments[0]->on = true;
		segments[1]->on = false;
		segments[2]->on = true;
		segments[3]->on = true;
		segments[4]->on = true;
		segments[5]->on = true;
		segments[6]->on = true;
		break;
	case(7):
		segments[0]->on = true;
		segments[1]->on = true;
		segments[2]->on = true;
		segments[3]->on = false;
		segments[4]->on = false;
		segments[5]->on = false;
		segments[6]->on = false;
		break;
	case(8):
		segments[0]->on = true;
		segments[1]->on = true;
		segments[2]->on = true;
		segments[3]->on = true;
		segments[4]->on = true;
		segments[5]->on = true;
		segments[6]->on = true;
		break;
	case(9):
		segments[0]->on = true;
		segments[1]->on = true;
		segments[2]->on = true;
		segments[3]->on = true;
		segments[4]->on = false;
		segments[5]->on = true;
		segments[6]->on = true;
		break;
	}
}

// creeaza un mesh pentru un o cifra
Mesh* Objects::CreateDisplaySegment(glm::vec3 baseVertex) {

	glm::vec3 color = glm::vec3(1);

	Mesh* segment = new Mesh("segment");
	std:vector<VertexFormat> vertices = {

		VertexFormat(baseVertex, color),
		VertexFormat(baseVertex + glm::vec3(0.1, -0.1, 0), color),
		VertexFormat(baseVertex + glm::vec3(0.9, -0.1, 0), color),
		VertexFormat(baseVertex + glm::vec3(1, 0, 0), color),
		VertexFormat(baseVertex + glm::vec3(0.9, 0.1, 0), color),
		VertexFormat(baseVertex + glm::vec3(0.1, 0.1, 0), color),
		VertexFormat(baseVertex, color)

	};

	std::vector<unsigned short> indices = { 0, 1, 2, 3, 4, 5, 6 };
	segment->SetDrawMode(GL_TRIANGLE_FAN);
	segment->InitFromData(vertices, indices);
	return segment;
}

// mesh pentru emerald (element colectabil)
Mesh* Objects::EmeraldMesh() {
	
	glm::vec3 color = glm::vec3(0.314, 0.784, 0.471);

	Mesh* mesh = new Mesh("emerald");

	std::vector<VertexFormat> vertices = {
		VertexFormat(glm::vec3(0), color),
		VertexFormat(glm::vec3(0.5, 1, 0.5), color),
		VertexFormat(glm::vec3(0.5, 1, -0.5), color),
		VertexFormat(glm::vec3(-0.5, 1, -0.5), color),
		VertexFormat(glm::vec3(-0.5, 1, 0.5), color),
		VertexFormat(glm::vec3(0, 2, 0), color),
	};

	std::vector<unsigned short> indices = {
		0, 1, 2,
		0, 2, 3,
		0, 3, 4,
		0, 4, 1,
		5, 2, 1,
		5, 3, 2,
		5, 4, 3,
		5, 1, 4
	};

	mesh->InitFromData(vertices, indices);
	
	return mesh;
}


// functie care genereaza un arc de elipsa
// primeste unghiul descris de arc, coordonatele initiale, coeficientii ecuatiei, numarul de fragmente, culoarea si 2 raze a caror diferenta o reprezinta grosimea arcului
std::vector<VertexFormat> Objects::CreateEllipseCurveArray(float startAngle, float endAngle, float x, float y, float x_param, float y_param,
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

// mesh pentru inelul unei planete
Mesh* Objects::RingMesh() {

	std::vector<VertexFormat> vertices = CreateEllipseCurveArray(0, 6.5, 0, 0, 1, 1, 100, glm::vec3(1), 0.8, 1);

	std::vector<unsigned short> indices = {};
	for (int i = 0; i < 201; i++) {
		indices.push_back(i);
	}

	Mesh* mesh = new Mesh("ring");
	mesh->SetDrawMode(GL_TRIANGLE_STRIP);
	mesh->InitFromData(vertices, indices);

	return mesh;
}