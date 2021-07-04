#pragma once

namespace collisions {
	class CollisionChecker;

	enum CollisionType {
		BALL = 0,
		BRICK,
		PADDLE,
		WALL
	};

	struct CollisionEdges {
		bool TOP;
		bool RIGHT;
		bool BOTTOM;
		bool LEFT;
	};

	class Collidable {
	private:
		CollisionType type;

		bool topWithinRange(Collidable* other);
		bool rightWithinRange(Collidable* other);
		bool bottomWithinRange(Collidable* other);
		bool leftWithinRange(Collidable* other);
	public:
		int x, y, w, h, dx, dy;
		bool interactive;

		Collidable(CollisionType type);
		virtual ~Collidable();

		virtual void CollisionResponse(CollisionType, CollisionEdges) = 0;

		void SetValues(int x, int y, int w, int h, int dx, int dy);

	protected:
		friend CollisionChecker;

		CollisionEdges Collided(Collidable * other);
		CollisionType getType() { return type; }
	};

}; // namespace collisions