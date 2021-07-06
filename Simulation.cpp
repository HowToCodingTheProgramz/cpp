#include "Simulation.h"

game::mouse::mouse() : clicked(false), x(0), y(0), sx(0), sy(0) {}

bool game::mouse::getAndClearClicked() {
	bool c = clicked;
	clicked = false;
	return c;
}

void game::mouse::scaleCoords(int w1, int h1, int w2, int h2) {
	float scalex = static_cast<float>(w1) / static_cast<float>(w2);
	float scaley = static_cast<float>(h1) / static_cast<float>(h2);
	sx = static_cast<unsigned int>(static_cast<float>(x) * scalex);
	sy = static_cast<unsigned int>(static_cast<float>(y) * scaley);
}

game::AutoBrush::AutoBrush(COLORREF col) {
	brush = CreateSolidBrush(col);
}
game::AutoBrush::~AutoBrush() {
	DeleteObject(brush);
}

HBRUSH game::AutoBrush::get() const {
	return brush;
}

game::Simulation::Simulation(HWND hWnd) : redBrush(RGB(255, 32, 32)) {
	int paddle_w = static_cast<int>(10.25f * (static_cast<float>(SIMULATION_W) / 100.0f));
	int paddle_h = static_cast<int>(3.1f * (static_cast<float>(SIMULATION_H) / 100.0f));

	paddle.SetValues((SIMULATION_W >> 1) - (paddle_w >> 1), SIMULATION_H - (paddle_h << 1), paddle_w, paddle_h, 0, 0);
	ball.SetValues((SIMULATION_W >> 1) - 2, (SIMULATION_H >> 1) - 2, 4, 4, 1, 1);

	walls[0].SetValues(0, 0, WALL_W, SIMULATION_H, 0, 0); // left wall
	walls[1].SetValues(0, 0, SIMULATION_W, WALL_H, 0, 0); // top wall
	walls[2].SetValues(SIMULATION_W - WALL_W, 0, WALL_W, SIMULATION_H, 0, 0); // right wall
	// walls[3].SetValues(0, SIMULATION_H - WALL_H, SIMULATION_W, WALL_W, 0, 0); // bottom wall

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

	currentState = STATE_INITIAL;
}

game::Simulation::~Simulation() {
	SelectObject(dc, oldBmp);
	DeleteDC(dc);
	DeleteObject(bm);
}

void game::Simulation::UpdateAndDraw(LPRECT rect, HDC hdc, game::mouse & m) {
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

	RECT scoreRect = { WALL_W << 1, SIMULATION_H - 40, 200, SIMULATION_H };
	DrawText(dc, scoreStr.c_str(), scoreStr.size(), &scoreRect, DT_LEFT);

	DrawStateText();

	StretchBlt(hdc, 0, 0, rect->right - rect->left, rect->bottom - rect->top, dc, 0, 0, SIMULATION_W, SIMULATION_H, SRCCOPY);
}
void game::Simulation::UpdateScore() {
	std::stringstream ss;
	ss << "SCORE: " << score;
	scoreStr = ss.str();
}
void game::Simulation::Update(game::mouse & m) {
	bool mouseclicked = m.getAndClearClicked();

	switch (currentState) {
	case STATE_INITIAL:
		if (mouseclicked)
			BeginCountdown();
		return;
	case STATE_COUNTDOWN: {
		if (GetTickCount() - startTime > COUNTDOWN_PERIOD)
			currentState = STATE_GAMEPLAY;
		return;
	} break;
	case STATE_GAMEPLAY: {
		ball.x += ball.dx;
		ball.y += ball.dy;
		collisions::CollisionChecker::getCollisionChecker()->CheckCollisions();
		if (mouseclicked)
			currentState = STATE_PAUSED;

		if (ball.y > SIMULATION_H)
			currentState = STATE_LOSE;
	} break;
	case STATE_PAUSED:
		if (mouseclicked)
			BeginCountdown();
		break;
	case STATE_WIN:
	case STATE_LOSE:
		ball.SetValues((SIMULATION_W >> 1) - 2, (SIMULATION_H >> 1) - 2, 4, 4, 1, 1);
		if (mouseclicked) {
			score = 0;
			UpdateScore();
			for (int i = 0; i < NUM_BRICKS; i++) {
				bricks[i]->SetVisible(true);
			}
			currentState = STATE_INITIAL;
		}
		break;
	default:
		break;
	}

	paddle.x = m.sx - (paddle.w >> 1);
}
void game::Simulation::BeginCountdown() {
	startTime = GetTickCount();
	currentState = STATE_COUNTDOWN;
}
void game::Simulation::DrawStateText() {
	RECT textRect = { WALL_W << 1, SIMULATION_H >> 1, SIMULATION_W - (WALL_W << 1), SIMULATION_H };
	int lineHeight = 14;
	switch (currentState) {
	case STATE_INITIAL: {
		std::string l1 = "Move the mouse to control the paddle.";
		std::string l2 = "Click the mouse to toggle pause.";
		std::string l3 = "Click the mouse to start.";
		DrawText(dc, l1.c_str(), l1.size(), &textRect, DT_LEFT);
		textRect.top += lineHeight;
		DrawText(dc, l2.c_str(), l2.size(), &textRect, DT_LEFT);
		textRect.top += lineHeight;
		DrawText(dc, l3.c_str(), l3.size(), &textRect, DT_LEFT);
	} break;
	case STATE_COUNTDOWN: {
		std::string l1 = "Game Starts in:";
		DrawText(dc, l1.c_str(), l1.size(), &textRect, DT_LEFT);
		std::stringstream ss;
		ss << ((COUNTDOWN_PERIOD - (GetTickCount() - startTime)) / 1000) << " Seconds.";
		textRect.top += lineHeight;
		DrawText(dc, ss.str().c_str(), ss.str().size(), &textRect, DT_LEFT);
	} break;
	case STATE_GAMEPLAY:
	break;
	case STATE_PAUSED: {
		std::string l1 = "Paused - Click mouse to play.";
		DrawText(dc, l1.c_str(), l1.size(), &textRect, DT_LEFT);
	} break;
	case STATE_WIN: {
		std::string l1 = "Congratulations you have won!";
		std::string l2 = "Click mouse to start over.";
		DrawText(dc, l1.c_str(), l1.size(), &textRect, DT_LEFT);
		textRect.top += lineHeight;
		DrawText(dc, l2.c_str(), l2.size(), &textRect, DT_LEFT);
	} break;
	case STATE_LOSE: {
		std::string l1 = "Sorry, you've lost - game over!";
		std::string l2 = "Click mouse to start over.";
		DrawText(dc, l1.c_str(), l1.size(), &textRect, DT_LEFT);
		textRect.top += lineHeight;
		DrawText(dc, l2.c_str(), l2.size(), &textRect, DT_LEFT);
	} break;
	default:
		break;
	}
}
void game::Simulation::OnBrickBroken(Brick* b, collisions::CollisionType t, collisions::CollisionEdges e) {
	b->SetVisible(false);
	b->interactive = false;
	score += 10;
	UpdateScore();

	// Check - are all the bricks broken? Yes? New Stage!
	for (int i = 0; i < NUM_BRICKS; i++) {
		if (bricks[i]->IsVisible()) return;
	}
	currentState = STATE_WIN;
}