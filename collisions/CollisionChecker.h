#pragma once
#include <vector>
#include "Collidable.h"

namespace collisions {
	class CollisionChecker {
		std::vector<Collidable *> entities;
	public:
		CollisionChecker();
		~CollisionChecker();

		void AddCollidable(Collidable * col);
		void CheckCollisions();
	};
}; // namespace collisions