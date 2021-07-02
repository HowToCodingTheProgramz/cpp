#pragma once
#include <vector>
#include "Collidable.h"

namespace collisions {
	class CollisionChecker {
		std::vector<Collidable *> entities;
		CollisionChecker();
		~CollisionChecker();
	public:

		void AddCollidable(Collidable * col);
		void RemoveCollidable(Collidable * col);
		void CheckCollisions();

		static CollisionChecker* getCollisionChecker();
	};
}; // namespace collisions