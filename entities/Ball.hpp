#pragma once
#include "../collisions/Collidable.h"

class Ball : public collisions::Collidable {
public:

	Ball() : Collidable(collisions::BALL) {}

	~Ball() {}

	virtual void CollisionResponse(collisions::CollisionType t) override {
		// Do something.
	}
};