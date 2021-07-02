#pragma once
#include "../collisions/Collidable.h"

class Paddle : public collisions::Collidable {
public:

	Paddle() : collisions::Collidable(collisions::PADDLE) {}

	~Paddle() {}

	virtual void CollisionResponse(collisions::CollisionType t, collisions::CollisionEdges e) override {
		// Do something.
	}
};