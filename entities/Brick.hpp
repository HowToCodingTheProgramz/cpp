#pragma once
#include "../collisions/Collidable.h"

class Brick : public collisions::Collidable {
public:

	Brick() : collisions::Collidable(collisions::BRICK) {}

	~Brick() {}

	virtual void CollisionResponse(collisions::CollisionType t, collisions::CollisionEdges e) override {
		// Do something.
	}
};