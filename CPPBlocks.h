#pragma once

#include "resource.h"

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
		const int w = 320;
		const int h = 240;

		int paddle_x, paddle_y, paddle_w, paddle_h;
		int ball_x, ball_y, ball_dx, ball_dy, ball_w, ball_h;

		HDC dc;
		HBITMAP bm;
		HGDIOBJ oldBmp;
		AutoBrush redBrush;

		void Update(mouse & m) {
			bool mouseclicked = m.getAndClearClicked();
			if (m.sx < (paddle_x + (paddle_w >> 1))) {
				if (paddle_x > 0) paddle_x -= 2;
			} else if (m.sx > (paddle_x + (paddle_w >> 1))) {
				if ((paddle_x + paddle_w) < w) paddle_x += 2;
			}
			ball_x += ball_dx;
			ball_y += ball_dy;

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
			paddle_w = static_cast<int>(6.25f * (static_cast<float>(w) / 100.0f));
			paddle_h = static_cast<int>(2.1f * (static_cast<float>(h) / 100.0f));
			paddle_x = (w >> 1) - (paddle_w >> 1);
			paddle_y = h - (paddle_h << 1);

			ball_x = (w >> 1) - 2;
			ball_y = (h >> 1) - 2;
			ball_dx = 1;
			ball_dy = 1;
			ball_w = 4;
			ball_h = 4;

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

			RECT r = {paddle_x, paddle_y, paddle_x + paddle_w, paddle_y + paddle_h};
			FillRect(dc, &r, redBrush.get());

			RECT q = { ball_x, ball_y, ball_x + ball_w, ball_y + ball_h };
			FillRect(dc, &q, redBrush.get());
			// More drawing code here.
			// ...

			StretchBlt(hdc, 0, 0, rect->right - rect->left, rect->bottom - rect->top, dc, 0, 0, w, h, SRCCOPY);
		}
	};
}; // namespace game
