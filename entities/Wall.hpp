#pragma once
#include "../collisions/Collidable.h"

class Wall : public collisions::Collidable {
public:

	Wall() : collisions::Collidable(collisions::WALL) {}

	~Wall() {}

	virtual void CollisionResponse(collisions::CollisionType t, collisions::CollisionEdges e) override {}
};