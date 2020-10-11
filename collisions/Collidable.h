#pragma once

namespace collisions {
	class CollisionChecker;

	enum CollisionType {
		BALL = 0,
		BRICK,
		PADDLE,
		WALL
	};

	class Collidable {
	private:
		CollisionType type;
	public:
		int x, y, w, h, dx, dy;

		Collidable(CollisionType type);
		virtual ~Collidable();

		virtual void CollisionResponse(CollisionType) = 0;

	protected:
		friend CollisionChecker;

		bool Collided(Collidable * other);
		CollisionType getType() { return type; }
	};

}; // namespace collisions