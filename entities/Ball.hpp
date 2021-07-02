#pragma once
#include "../collisions/Collidable.h"

class Ball : public collisions::Collidable {
public:

	Ball() : Collidable(collisions::BALL) {}

	~Ball() {}

	virtual void CollisionResponse(collisions::CollisionType t, collisions::CollisionEdges e) override {
		if (dy > 0) { // heading down
			if (e.BOTTOM) {
				dy = -1;
				y += dy;
				return;
			}
		} else {
			if (e.TOP) {
				dy = 1;
				y += dy;
				return;
			}
		}
		if (dx > 0) { // heading right
			if (e.RIGHT) {
				dx = -1;
				x += dx;
				return;
			}
		} else {
			if (e.LEFT) {
				dx = 1;
				x += dx;
				return;
			}
		}
	}
};