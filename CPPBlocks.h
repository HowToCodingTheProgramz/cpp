#pragma once

#include "resource.h"

namespace game {
	struct MyPaintStuff {
	private:
		const int w = 320;
		const int h = 240;

		int paddle_x, paddle_y, paddle_w, paddle_h;
		int ball_x, ball_y, ball_w, ball_h;

		HDC dc;
		HBITMAP bm;
		HGDIOBJ oldBmp;

	public:

		MyPaintStuff(HWND hWnd) {
			paddle_w = static_cast<int>(6.25f * (static_cast<float>(w) / 100.0f));
			paddle_h = static_cast<int>(2.1f * (static_cast<float>(h) / 100.0f));
			paddle_x = (w >> 1) - (paddle_w >> 1);
			paddle_y = h - (paddle_h << 1);

			HDC hdc = GetDC(hWnd);
			dc = CreateCompatibleDC(hdc);
			bm = CreateCompatibleBitmap(dc, w, h);
			ReleaseDC(hWnd, hdc);

			oldBmp = SelectObject(dc, bm);
		}

		~MyPaintStuff() {
			SelectObject(dc, oldBmp);
			DeleteDC(dc);
			DeleteObject(bm);
		}

		void UpdateAndDraw(LPRECT rect, HDC hdc) {
			RECT rc = *rect;
			rc.right = rc.left + max(w, rc.right);
			rc.bottom = rc.top + max(h, rc.bottom);

			RECT myRect = { 0, 0, w, h };
			FillRect(dc, &myRect, (HBRUSH)GetStockObject(BLACK_BRUSH));

			RECT r = {paddle_x, paddle_y, paddle_x + paddle_w, paddle_y + paddle_h};
			FillRect(dc, &r, (HBRUSH)GetStockObject(WHITE_BRUSH));

			// More drawing code here.
			// ...

			StretchBlt(hdc, 0, 0, rect->right - rect->left, rect->bottom - rect->top, dc, 0, 0, w, h, SRCCOPY);
		}
	};
}; // namespace game