#pragma once

#include "resource.h"
#include "entities/Ball.hpp"
#include "entities/Brick.hpp"
#include "entities/Wall.hpp"
#include "entities/Paddle.hpp"
#include "collisions/CollisionChecker.h"

#define NUM_WALLS 4
#define NUM_BRICKS 200

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
	struct Simulation {
	private:
		const int w = 320; // rookie numbers.
		const int h = 240;

		Wall walls[NUM_WALLS];
		Brick bricks[NUM_BRICKS];
		Paddle paddle;
		Ball ball;

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
			/*
			if (((ball_y + ball_h) > paddle_y) && (ball_y < (paddle_y + paddle_h))) {
				if ((ball_x >= paddle_x) && ((ball_x + ball_w) <= (paddle_x + paddle_w))) {
					ball_dy = -1;
					ball_y += ball_dy;
					//score += 10;
					return;
				}
			}
			if ((ball_x + ball_w) > w) {
				ball_dx = -1;
				ball_x += ball_dx;
				return;
			}
			if ((ball_y + ball_h) > h) {
				ball_dy = -1;
				ball_y += ball_dy;
				// you missed the ball . . . do something.
				// end screen or something?
				//--lives;
				//resetBall();
				//if (lives < 0) {
				//	currentState = states.DIED;
				//}
				return;
			}
			if ((ball_x) < 0) {
				ball_dx = 1;
				ball_x += ball_dx;
				return;
			}
			if ((ball_y) < 0) {
				ball_dy = 1;
				ball_y += ball_dy;
				return;
			}
			/*
			var intersectingEdge = playingField.findIntersectingEdgeOf(ball);
			if (intersectingEdge > -1) {
				// change direction based on the intersection
				score += 100;
				switch (intersectingEdge) {
				case 0:
					ball_dy = 1;
					ball_y += ball_dy;
					break;
				case 1:
					ball_dx = -1;
					ball_x += ball_dx;
					break;
				case 2:
					ball_dy = -1;
					ball_y += ball_dy;
					break;
				case 3:
					ball_dx = 1;
					ball_x += ball_dx;
					break;
				default:
					break;
				}
			}
			*/
		}

	public:

		Simulation(HWND hWnd) : redBrush(RGB(255, 32, 32)) {
			int paddle_w = static_cast<int>(6.25f * (static_cast<float>(w) / 100.0f));
			int paddle_h = static_cast<int>(2.1f * (static_cast<float>(h) / 100.0f));

			paddle.SetValues((w >> 1) - (paddle_w >> 1), h - (paddle_h << 1), paddle_w, paddle_h, 0, 0);
			ball.SetValues((w >> 1) - 2, (h >> 1) - 2, 4, 4, 1, 1);

			walls[0].SetValues(0, 0, 2, h, 0, 0); // left wall
			walls[1].SetValues(0, 0, w, 2, 0, 0); // top wall
			walls[2].SetValues(w - 2, 0, 2, h, 0, 0); // right wall
			walls[3].SetValues(0, h - 2, w, 2, 0, 0); // bottom wall

			HDC hdc = GetDC(hWnd);
			dc = CreateCompatibleDC(hdc);
			bm = CreateCompatibleBitmap(hdc, w, h); // This is literal magic in here.
			ReleaseDC(hWnd, hdc);

			oldBmp = SelectObject(dc, bm);
		}

		~Simulation() {
			SelectObject(dc, oldBmp);
			DeleteDC(dc);
			DeleteObject(bm);
		}

		void UpdateAndDraw(LPRECT rect, HDC hdc, mouse & m) {
			RECT rc = *rect;
			rc.right = rc.left + max(w, rc.right);
			rc.bottom = rc.top + max(h, rc.bottom);
			m.scaleCoords(w, h, max(w, rc.right), max(h, rc.bottom));

			Update(m);

			RECT myRect = { 0, 0, w, h };
			FillRect(dc, &myRect, (HBRUSH)GetStockObject(BLACK_BRUSH));

			for (int i = 0; i < NUM_WALLS; i++) {
				RECT w = { walls[i].x, walls[i].y, walls[i].x + walls[i].w, walls[i].y + walls[i].h };
				FillRect(dc, &w, redBrush.get());
			}

			RECT r = { paddle.x, paddle.y, paddle.x + paddle.w, paddle.y + paddle.h };
			FillRect(dc, &r, redBrush.get());

			RECT q = { ball.x, ball.y, ball.x + ball.w, ball.y + ball.h };
			FillRect(dc, &q, redBrush.get());
			// More drawing code here.
			// ...

			StretchBlt(hdc, 0, 0, rect->right - rect->left, rect->bottom - rect->top, dc, 0, 0, w, h, SRCCOPY);
		}
	};
}; // namespace game
