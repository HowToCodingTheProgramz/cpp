#pragma once

#include "framework.h"
#include "entities/Ball.hpp"
#include "entities/Brick.hpp"
#include "entities/Wall.hpp"
#include "entities/Paddle.hpp"
#include "collisions/CollisionChecker.h"

#include <string>
#include <sstream>

#define SIMULATION_W 320
#define SIMULATION_H 240
#define NUM_WALLS 3
#define NUM_BRICKS 105
#define NUM_BRICK_ROWS 5
#define WALL_W 2
#define WALL_H 2
#define COUNTDOWN_PERIOD 3000

namespace game {
	enum GameStates {
		STATE_INITIAL = 0,
		STATE_COUNTDOWN,
		STATE_GAMEPLAY,
		STATE_PAUSED,
		STATE_WIN,
		STATE_LOSE
	};
	struct mouse {
		bool clicked;
		unsigned int x;
		unsigned int y;

		unsigned int sx;
		unsigned int sy;

		mouse();

		bool getAndClearClicked();
		void scaleCoords(int w1, int h1, int w2, int h2);
	};

	struct AutoBrush {
	private:
		HBRUSH brush;

	public:
		AutoBrush(COLORREF col);
		~AutoBrush();

		HBRUSH get() const;
	};
	struct Simulation : public BrickBrokenListener {
	private:
		Wall walls[NUM_WALLS];
		Brick* bricks[NUM_BRICKS];
		AutoBrush brickBrushes[4] = {
			RGB(32, 255, 32), RGB(32, 32, 255),
			RGB(255, 255, 32), RGB(32, 32, 255)
		};
		Paddle paddle;
		Ball ball;
		int score;
		std::string scoreStr;
		GameStates currentState;
		DWORD startTime;

		HDC dc;
		HBITMAP bm;
		HGDIOBJ oldBmp;
		AutoBrush redBrush;

		void Update(mouse & m);
		void DrawStateText();
		void UpdateScore();
		void BeginCountdown();

	public:

		Simulation(HWND hWnd);
		~Simulation();

		void UpdateAndDraw(LPRECT rect, HDC hdc, mouse & m);
		
		void OnBrickBroken(Brick* b, collisions::CollisionType t, collisions::CollisionEdges e) override;
	};
}; // namespace game
