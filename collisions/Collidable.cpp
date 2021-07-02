#pragma once

#include "Collidable.h"
#include "CollisionChecker.h"

collisions::Collidable::Collidable(CollisionType type) : x(0), y(0), w(0), h(0), dx(0), dy(0), type(type) {
	collisions::CollisionChecker::getCollisionChecker()->AddCollidable(this);
}

collisions::Collidable::~Collidable() {
	collisions::CollisionChecker::getCollisionChecker()->RemoveCollidable(this);
}

bool collisions::Collidable::topWithinRange(collisions::Collidable* other) {
	return (y >= other->y) && (y <= (other->y + other->h));
}
bool collisions::Collidable::rightWithinRange(collisions::Collidable* other) {
	return (x + w >= other->x) && (x + w <= other->x + other->w);
}
bool collisions::Collidable::bottomWithinRange(collisions::Collidable* other) {
	return (y + h >= other->y) && (y + h <= other->y + other->h);
}
bool collisions::Collidable::leftWithinRange(collisions::Collidable* other) {
	return (x >= other->x) && (x <= other->x + other->w);
}

collisions::CollisionEdges collisions::Collidable::Collided(Collidable * other) {
	collisions::CollisionEdges ret = { false, false, false, false };

	if (leftWithinRange(other) && rightWithinRange(other)) {
		if (topWithinRange(other)) ret.TOP = true;
		if (bottomWithinRange(other)) ret.BOTTOM = true;
	}

	if (topWithinRange(other) && bottomWithinRange(other)) {
		if (leftWithinRange(other)) ret.LEFT = true;
		if (rightWithinRange(other)) ret.RIGHT = true;
	}

	return ret;
}

void collisions::Collidable::SetValues(int _x, int _y, int _w, int _h, int _dx, int _dy) {
	x = _x;
	y = _y;
	w = _w;
	h = _h;
	dx = _dx;
	dy = _dy;
}