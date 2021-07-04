#include "CollisionChecker.h"

static collisions::CollisionChecker* collMan = nullptr;
collisions::CollisionChecker* collisions::CollisionChecker::getCollisionChecker() {
	if (collMan == nullptr) collMan = new collisions::CollisionChecker();
	return collMan;
}

collisions::CollisionChecker::CollisionChecker() {}
collisions::CollisionChecker::~CollisionChecker() {}

void collisions::CollisionChecker::AddCollidable(Collidable * col) {
	entities.push_back(col);
}
void collisions::CollisionChecker::RemoveCollidable(Collidable * col) {
	auto iter = entities.begin();
	for (; iter != entities.end(); iter++) {
		if (*iter == col) {
			iter = entities.erase(iter);
			return;
		}
	}
}

void collisions::CollisionChecker::CheckCollisions() {
	int currentEntityIdx = 0;
	do {
		for (int i = 0; i < entities.size(); i++) {
			if (currentEntityIdx == i) continue; // don't check an entity against itself.
			if (!entities[i]->interactive) continue;
			if (!entities[currentEntityIdx]->interactive) continue;

			collisions::CollisionEdges e = entities[i]->Collided(entities[currentEntityIdx]);
			if (e.TOP || e.RIGHT || e.BOTTOM || e.LEFT) {
				entities[i]->CollisionResponse(entities[currentEntityIdx]->getType(), e);

				collisions::CollisionEdges comp = { !e.TOP, !e.RIGHT, !e.BOTTOM, !e.LEFT };
				entities[currentEntityIdx]->CollisionResponse(entities[i]->getType(), comp);
			}
		}
		++currentEntityIdx;
	} while (currentEntityIdx < entities.size());
}
