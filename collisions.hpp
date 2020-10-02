#pragma once

#include <vector>

namespace game {
	class Collidable;
	class CollisionChecker;

	enum CollisionType {
		WALL = 0,
		PLAYER,
		BALL,
		BRICK
	};

	class ICollisionChecker {
	public:
		virtual void AddCollidable(Collidable * col) = 0;
		virtual void CheckCollisions() = 0;
	};

	class Collidable {
	public:
		int x, y, w, h, dx, dy;
		
		Collidable(ICollisionChecker * collMan): x(0), y(0), w(0), h(0), dx(0), dy(0) {
			collMan->AddCollidable(this);
		}
		virtual void CollisionResponse(CollisionType) = 0;
		virtual CollisionType getType() = 0;

	protected:
		friend CollisionChecker;

		bool Collided(Collidable * other) {
			if (((y + h) > other->y) && (y < (other->y + other->h))) { // AABB (Axis-Aligned Bounding-Boxes)
				if ((x >= other->x) && ((x + w) <= (other->x + other->w))) {
					return true;
				}
			}
			return false;
		}
	};

	class CollisionChecker : public ICollisionChecker {
		std::vector<Collidable *> entities;
	public:
		CollisionChecker() {}
		~CollisionChecker() {}

		virtual void AddCollidable(Collidable * col) override {
			entities.push_back(col);
		}

		virtual void CheckCollisions() override {
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
	};

	class Wall : public Collidable {
	public:

		Wall(CollisionChecker * collMan) : Collidable(collMan) {}
		~Wall() {}

		virtual void CollisionResponse(CollisionType t) override {}
		virtual CollisionType getType() override { return WALL; }
	};

	class Player : public Collidable {
	public:

		Player(CollisionChecker * collMan) : Collidable(collMan) {}

		~Player() {}

		virtual void CollisionResponse(CollisionType t) override {
			// Do something.
		}

		virtual CollisionType getType() override { return PLAYER; }
	};

	class Ball : public Collidable {
	public:

		Ball(CollisionChecker * collMan) : Collidable(collMan) {}

		~Ball() {}

		virtual void CollisionResponse(CollisionType t) override {
			// Do something.
		}

		virtual CollisionType getType() override { return BALL; }
	};

	class Brick : public Collidable {
	public:

		Brick(CollisionChecker * collMan) : Collidable(collMan) {}

		~Brick() {}

		virtual void CollisionResponse(CollisionType t) override {
			// Do something.
		}

		virtual CollisionType getType() override { return BRICK; }
	};
}; // namespace gamee