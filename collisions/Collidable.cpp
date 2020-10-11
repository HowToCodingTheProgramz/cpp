#pragma once

#include "Collidable.h"
#include "CollisionChecker.h"

collisions::Collidable::Collidable(CollisionType type) : x(0), y(0), w(0), h(0), dx(0), dy(0), type(type) {
	//collMan.AddCollidable(this);
}

collisions::Collidable::~Collidable() {
	//collMan->RemoveCollidable(this);
}

bool collisions::Collidable::Collided(Collidable * other) {
	if (((y + h) > other->y) && (y < (other->y + other->h))) { // AABB (Axis-Aligned Bounding-Boxes)
		if ((x >= other->x) && ((x + w) <= (other->x + other->w))) {
			return true;
		}
	}
	return false;
}