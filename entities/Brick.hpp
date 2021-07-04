#pragma once
#include "../collisions/Collidable.h"

class Brick;

class BrickBrokenListener {
public:
	virtual void OnBrickBroken(Brick* b, collisions::CollisionType t, collisions::CollisionEdges e) = 0;
};

class Brick : public collisions::Collidable {
	BrickBrokenListener * listener;
	bool visible;
public:

	Brick(BrickBrokenListener * listener, bool visibility) : collisions::Collidable(collisions::BRICK), listener(listener),
	visible(visibility) {}

	~Brick() {}

	void SetVisible(bool visible) {
		this->visible = visible;
	}
	bool IsVisible() const {
		return visible;
	}

	virtual void CollisionResponse(collisions::CollisionType t, collisions::CollisionEdges e) override {
		if (t != collisions::BALL) return;
		if (listener) {
			listener->OnBrickBroken(this, t, e);
		}
	}
};