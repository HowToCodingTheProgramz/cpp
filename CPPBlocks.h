#pragma once

#include "resource.h"
#include "entities/Ball.hpp"
#include "entities/Brick.hpp"
#include "entities/Wall.hpp"
#include "entities/Paddle.hpp"
#include "collisions/CollisionChecker.h"

#include <string>
#include <sstream>

#define SIMULATION_W 320
#define SIMULATION_H 240
#define NUM_WALLS 4
#define NUM_BRICKS 105
#define NUM_BRICK_ROWS 5
#define WALL_W 2
#define WALL_H 2

namespace game {
	struct mouse {
		bool clicked;
		unsigned int x;
		unsigned int y;

		unsigned int sx;
		unsigned int sy;

		mouse() : clicked(false), x(0), y(0), sx(0), sy(0) {}

		bool getAndClearClicked() {
			bool c = clicked;
			clicked = false;
			return c;
		}

		void scaleCoords(int w1, int h1, int w2, int h2) {
			float scalex = static_cast<float>(w1) / static_cast<float>(w2);
			float scaley = static_cast<float>(h1) / static_cast<float>(h2);
			sx = static_cast<unsigned int>(static_cast<float>(x) * scalex);
			sy = static_cast<unsigned int>(static_cast<float>(y) * scaley);
		}
	};

	struct AutoBrush {
	private:
		HBRUSH brush;

	public:
		AutoBrush(COLORREF col) {
			brush = CreateSolidBrush(col);
		}
		~AutoBrush() {
			DeleteObject(brush);
		}

		HBRUSH get() const {
			return brush;
		}
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

		HDC dc;
		HBITMAP bm;
		HGDIOBJ oldBmp;
		AutoBrush redBrush;

		void Update(mouse & m) {
			bool mouseclicked = m.getAndClearClicked();
			if (m.sx < (paddle.x + (paddle.w >> 1))) {
				paddle.x -= 2;
			} else if (m.sx > (paddle.x + (paddle.w >> 1))) {
				paddle.x += 2;
			}

			ball.x += ball.dx;
			ball.y += ball.dy;

			collisions::CollisionChecker::getCollisionChecker()->CheckCollisions();
		}

	public:

		Simulation(HWND hWnd) : redBrush(RGB(255, 32, 32)) {
			int paddle_w = static_cast<int>(6.25f * (static_cast<float>(SIMULATION_W) / 100.0f));
			int paddle_h = static_cast<int>(2.1f * (static_cast<float>(SIMULATION_H) / 100.0f));

			paddle.SetValues((SIMULATION_W >> 1) - (paddle_w >> 1), SIMULATION_H - (paddle_h << 1), paddle_w, paddle_h, 0, 0);
			ball.SetValues((SIMULATION_W >> 1) - 2, (SIMULATION_H >> 1) - 2, 4, 4, 1, 1);

			walls[0].SetValues(0, 0, WALL_W, SIMULATION_H, 0, 0); // left wall
			walls[1].SetValues(0, 0, SIMULATION_W, WALL_H, 0, 0); // top wall
			walls[2].SetValues(SIMULATION_W - WALL_W, 0, WALL_W, SIMULATION_H, 0, 0); // right wall
			walls[3].SetValues(0, SIMULATION_H - WALL_H, SIMULATION_W, WALL_W, 0, 0); // bottom wall

			// Build a wall.
			int num_bricks_per_row = NUM_BRICKS / NUM_BRICK_ROWS;
			int brick_w = ((SIMULATION_W - (WALL_W << 1)) / num_bricks_per_row);
			float bw = (static_cast<float>(brick_w) * 3.0f) / 4.0f;
			int brick_h = static_cast<int>(bw);

			int current_brick = 0;
			int x = WALL_W + 1;
			int y = WALL_H;
			for (int i = 0; i < NUM_BRICKS; i++) {
				bricks[i] = new Brick(this, true);
				bricks[i]->SetValues(x, y, brick_w, brick_h, 0, 0);

				current_brick++;
				x += brick_w;
				if (current_brick >= num_bricks_per_row) {
					current_brick = 0;
					x = WALL_W + 1;
					y += brick_h;
				}
			}

			HDC hdc = GetDC(hWnd);
			dc = CreateCompatibleDC(hdc);
			bm = CreateCompatibleBitmap(hdc, SIMULATION_W, SIMULATION_H); // This is literal magic in here.
			ReleaseDC(hWnd, hdc);
			SetTextColor(dc, RGB(192, 64, 92));
			SetBkMode(dc, TRANSPARENT);

			oldBmp = SelectObject(dc, bm);

			score = 0;
			UpdateScore();
		}

		~Simulation() {
			SelectObject(dc, oldBmp);
			DeleteDC(dc);
			DeleteObject(bm);
		}

		void UpdateAndDraw(LPRECT rect, HDC hdc, mouse & m) {
			RECT rc = *rect;
			rc.right = rc.left + max(SIMULATION_W, rc.right);
			rc.bottom = rc.top + max(SIMULATION_H, rc.bottom);
			m.scaleCoords(SIMULATION_W, SIMULATION_H, max(SIMULATION_W, rc.right), max(SIMULATION_H, rc.bottom));

			Update(m);

			RECT myRect = { 0, 0, SIMULATION_W, SIMULATION_H };
			FillRect(dc, &myRect, (HBRUSH)GetStockObject(BLACK_BRUSH));

			for (int i = 0; i < NUM_WALLS; i++) {
				RECT w = { walls[i].x, walls[i].y, walls[i].x + walls[i].w, walls[i].y + walls[i].h };
				FillRect(dc, &w, redBrush.get());
			}

			for (int i = 0; i < NUM_BRICKS; i++) {
				if (!bricks[i]->IsVisible()) continue;
				RECT b = { bricks[i]->x, bricks[i]->y, bricks[i]->x + bricks[i]->w, bricks[i]->y + bricks[i]->h };
				FillRect(dc, &b, brickBrushes[i % 4].get());
			}

			RECT r = { paddle.x, paddle.y, paddle.x + paddle.w, paddle.y + paddle.h };
			FillRect(dc, &r, redBrush.get());

			RECT q = { ball.x, ball.y, ball.x + ball.w, ball.y + ball.h };
			FillRect(dc, &q, redBrush.get());
			// More drawing code here.
			// ...
			RECT scoreRect = { WALL_W << 1, SIMULATION_H - 20, 200, SIMULATION_H };
			DrawText(dc, scoreStr.c_str(), scoreStr.size(), &scoreRect, DT_LEFT);
			StretchBlt(hdc, 0, 0, rect->right - rect->left, rect->bottom - rect->top, dc, 0, 0, SIMULATION_W, SIMULATION_H, SRCCOPY);
		}
		void UpdateScore() {
			std::stringstream ss;
			ss << "SCORE: " << score;
			scoreStr = ss.str();
		}
		void OnBrickBroken(Brick* b, collisions::CollisionType t, collisions::CollisionEdges e) override {
			b->SetVisible(false);
			b->interactive = false;
			score += 10;
			UpdateScore();

			// Check - are all the bricks broken? Yes? New Stage!
		}
	};
}; // namespace game
