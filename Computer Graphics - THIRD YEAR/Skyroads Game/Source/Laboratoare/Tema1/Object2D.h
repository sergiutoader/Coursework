#pragma once

#include <string>

#include <include/glm.h>
#include <Core/GPU/Mesh.h>

namespace Object2D
{
	
	// wrapper pentru 2 mesh-uri care construiesc o sageata
	class Arrow {
		
	public:
		Mesh* body;
		Mesh* tip;
		Arrow::Arrow();
	private:
		void setArrow(void);
	};

	// wrapper pentru 3 mesh-uri care construiesc un balon
	class Balloon {
	public:
		Mesh* body;
		Mesh* triangle;
		Mesh* string;
		Balloon::Balloon(glm::vec3 color);
	private:
		void setBalloon(glm::vec3 color);
	};

	// wrapper pentru 2 mesh-uri care construiesc eroul
	class Hero {
	public:
		Mesh* body;
		Mesh* head;
		Hero::Hero();
	private:
		void setHero();
	};

	const float PI = 3.1415926f;
	std::vector<VertexFormat> CreateEllipseCurveArray(float startAngle, float endAngle, float x, float y, float x_param, float y_param, int fragments, glm::vec3 color, float r1, float r2);
	Mesh* CreateRectangle(std::string name, glm::vec3 leftBottomCorner, float width, float height, glm::vec3 color, bool fill);
	Mesh* CreateBow(float bow_width);
	Mesh* CreateShuriken(std::string name, glm::vec3 center, float edge, glm::vec3 color);
	Mesh* CreateTriangle(std::string name, glm::vec3 upVertex, float height, glm::vec3 color);
	Mesh* CreateHeart(std::string name, glm::vec3 color);
	Mesh* CreateHeroBody();
}

