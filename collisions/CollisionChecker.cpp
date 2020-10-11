#include "CollisionChecker.h"

collisions::CollisionChecker::CollisionChecker() {}
collisions::CollisionChecker::~CollisionChecker() {}

void collisions::CollisionChecker::AddCollidable(Collidable * col) {
	entities.push_back(col);
}

void collisions::CollisionChecker::CheckCollisions() {
	int currentEntityIdx = 0;
	do {
		for (int i = 0; i < entities.size(); i++) {
			if (currentEntityIdx == i) continue; // don't check an entity against itself.

			if (entities[i]->Collided(entities[currentEntityIdx])) {
				entities[i]->CollisionResponse(entities[currentEntityIdx]->getType());
				entities[currentEntityIdx]->CollisionResponse(entities[i]->getType());
				return;
			}
		}
		++currentEntityIdx;
	} while (currentEntityIdx < entities.size());
}
