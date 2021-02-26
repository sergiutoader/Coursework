#pragma once

#include <Component/SimpleScene.h>
#include <string>
#include <Core/Engine.h>

class Tema1 : public SimpleScene
{
public:
	struct ViewportSpace
	{
		ViewportSpace() : x(0), y(0), width(1), height(1) {}
		ViewportSpace(int x, int y, int width, int height)
			: x(x), y(y), width(width), height(height) {}
		int x;
		int y;
		int width;
		int height;
	};

	struct LogicSpace
	{
		LogicSpace() : x(0), y(0), width(1), height(1) {}
		LogicSpace(float x, float y, float width, float height)
			: x(x), y(y), width(width), height(height) {}
		float x;
		float y;
		float width;
		float height;
	};

public:
	Tema1();
	~Tema1();

	void Init() override;

private:
	void FrameStart() override;
	void Update(float deltaTimeSeconds) override;
	void FrameEnd() override;

	void DrawScene(glm::mat3 visMatrix, float deltaTimeSeconds);
	void RenderArrow(glm::mat3 modelMatrix);
	bool isInLogicSpace(float x, float y);
	float getRandFloat(float LO, float HI);

	void OnInputUpdate(float deltaTime, int mods) override;
	void OnKeyPress(int key, int mods) override;
	void OnKeyRelease(int key, int mods) override;
	void OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY) override;
	void OnMouseBtnPress(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods) override;
	void OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY) override;

	void DetectArrowCollisions();
	void DetectBowCollisions();
	void MoveObjects(float deltaTimeSeconds);
	void GenerateNewObjects();
	void ComputeArrowMovement(float deltaTimeSeconds);
	void IncreaseDifficulty();

	glm::mat3 VisualizationTransf2DUnif(const LogicSpace& logicSpace, const ViewportSpace& viewSpace);

	void SetViewportArea(const ViewportSpace& viewSpace, glm::vec3 colorColor = glm::vec3(0), bool clear = true);

protected:

	// structura care retine coordonatele, culoarea, si factorul de scalare pentru un obiect
	struct point {
		float x, y;
		glm::vec3 color;
		float scaling_factor;
	};


	float length;
	ViewportSpace viewSpace;
	LogicSpace logicSpace;
	glm::mat3 modelMatrix, visMatrix;
	/* ARC */
	// coordonatele arcului
	float bow_x, bow_y;
	// latimea arcului
	float bow_width;
	// marginea arcului = folosita pentru a limita corect zonele in care arcul se poate misca
	float bow_edge;
	// unghiul arcului
	float bow_angle;

	/* SAGEATA */
	// coordonatele sagetii
	float arrow_x, arrow_y;
	// viteza sagetii (in momentul in care aceasta este aruncata
	float arrow_vel_x, arrow_vel_y;
	// viteza sagetii (ajustata prin apasare mouse-ului)
	float arrow_speed;
	// unghiul sagetii
	float arrow_angle;
	// variabila care retine daca sageata este lansata
	bool isArrowFired;
	// coordonatele varfului sagetii
	float arrow_tip_x, arrow_tip_y;

	/* SHURIKEN */
	float shuriken_speed;
	// vector de coordonate pentru shuriken-uri
	std::vector<point> shuriken_coord;

	/* BALON */
	float balloon_speed;
	// vector de coordonate pentru baloane
	std::vector<point> balloon_coord;
	// frecventa de aparitie a valoanelor rosii fata de cele galbene
	float red_balloon_frequency;

	// tinte lovite
	std::vector<point> hit_targets;

	/* EROU */
	float hero_x, hero_y;

	/* POWERBAR */
	float powerbar_width, powerbar_height;
	float powerbar_x, powerbar_y;
	float powerbar_scaling_factor;

	/* MOUSE */
	//variabila care retine daca mouse-ul este aparast
	bool isMousePressed;
	// coordonate logice mouse
	float logic_mouseX, logic_mouseY;

	// variabila care creste in timp
	float incr_var;
	// variabile care vor creste asemenea incr_var si vor fi resetate la 0 periodic
	float interval_1, interval_2, interval_3;
	// bool care vor fi setate pe true periodic, pe baza variabilei interval
	bool interval_tick_1, interval_tick_2, interval_tick_3;
	// limita intervalului 1
	float interval_1_limit;

	float score;
	float lives;
	bool gameOver;

};
