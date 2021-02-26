#pragma once

#include <include/glm.h>
#include <Core/GPU/Mesh.h>

using namespace std;

namespace Objects {
	
	Mesh* CreateDisplaySegment(glm::vec3 baseVertex);
	Mesh* EmeraldMesh();
	Mesh* RingMesh();

	std::vector<VertexFormat> CreateEllipseCurveArray(float startAngle, float endAngle, float x, float y, float x_param, float y_param,
		int fragments, glm::vec3 color, float r1, float r2);

	class Segment {
	public:
		bool on;

		glm::vec3 position;
		bool vertical;
	};

	class Digit {
	public:
		Digit::Digit();
		void DisplayDigit(int);

		Mesh* mesh;

		std::vector<Segment *> segments;

		Segment a;
		Segment b;
		Segment c;
		Segment d;
		Segment e;
		Segment f;
		Segment g;
	};
}