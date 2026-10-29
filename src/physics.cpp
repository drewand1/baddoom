#include <algorithm>
#include "profile.h"
#include "physics.h"

using namespace std;
using namespace kE;

// Helper functions

bool handleCollisions(PhysicsObject& a, PhysicsObject& b, float dt) {
	Vec3f aMin = a.position - a.boundingBoxSize * 0.5;
	Vec3f aMax = a.position + a.boundingBoxSize * 0.5;
	Vec3f bMin = b.position - b.boundingBoxSize * 0.5;
	Vec3f bMax = b.position + b.boundingBoxSize * 0.5;

	bool areColliding = aMin.x < bMax.x && bMin.x < aMax.x
		&& aMin.y < bMax.y && bMin.y < aMax.y
		&& aMin.z < bMax.z && bMin.z < aMax.z;

	if (!areColliding)
		return false;

	// Solving and Bouncing
	float overX = min(aMax.x, bMax.x) - max(aMin.x, bMin.x);
	float overY = min(aMax.y, bMax.y) - max(aMin.y, bMin.y);
	float overZ = min(aMax.z, bMax.z) - max(aMin.z, bMin.z);
	float frictionCoeff = (a.friction + b.friction) * 0.5;
	float restCoeff = (a.elasticity + b.elasticity) * 0.5;
	Vec3f aInit = a.velocity;
	Vec3f bInit = b.velocity;


	if (overX < overY && overX < overZ) {
		if (aMax.x < bMax.x)
			a.position.x -= overX;
		else
			a.position.x += overX;
		
		if (a.flags & KE_PHYSFLAG_MOVES) {
			a.velocity.x = restCoeff * ((a.mass - b.mass) * aInit.x + 2 * b.mass * bInit.x) / (a.mass + b.mass);
			a.velocity.y *= 1 - frictionCoeff * dt; // TODO: Ticking timebomb for when friction * dt > 1. Lag can cause shit to go backwards.
			a.velocity.z *= 1 - frictionCoeff * dt;
		}

		if (b.flags & KE_PHYSFLAG_MOVES) {
			b.velocity.x = restCoeff * (2 * a.mass * aInit.x + (b.mass - a.mass) * bInit.x) / (a.mass + b.mass);
			b.velocity.y *= 1 - frictionCoeff * dt;
			b.velocity.z *= 1 - frictionCoeff * dt;
		}

	} else if (overY < overX && overY < overZ) {
		if (aMax.y < bMax.y)
			a.position.y -= overY;
		else
			a.position.y += overY;
		
		if (a.flags & KE_PHYSFLAG_MOVES) {
			a.velocity.y = restCoeff * ((a.mass - b.mass) * aInit.y + 2 * b.mass * bInit.y) / (a.mass + b.mass);
			a.velocity.x *= 1 - frictionCoeff * dt;
			a.velocity.z *= 1 - frictionCoeff * dt;
		}

		if (b.flags & KE_PHYSFLAG_MOVES) {
			b.velocity.y = restCoeff * (2 * a.mass * aInit.y + (b.mass - a.mass) * bInit.y) / (a.mass + b.mass);
			b.velocity.x *= 1 - frictionCoeff * dt;
			b.velocity.z *= 1 - frictionCoeff * dt;
		}

	} else {
		if (aMax.z < bMax.z)
			a.position.z -= overZ;
		else
			a.position.z += overZ;
		
		if (a.flags & KE_PHYSFLAG_MOVES) {
			a.velocity.z = restCoeff * ((a.mass - b.mass) * aInit.z + 2 * b.mass * bInit.z) / (a.mass + b.mass);
			a.velocity.x *= 1 - frictionCoeff * dt;
			a.velocity.y *= 1 - frictionCoeff * dt;
		}

		if (b.flags & KE_PHYSFLAG_MOVES) {
			b.velocity.z = restCoeff * (2 * a.mass * aInit.z + (b.mass - a.mass) * bInit.z) / (a.mass + b.mass);
			b.velocity.x *= 1 - frictionCoeff * dt;
			b.velocity.y *= 1 - frictionCoeff * dt;
		}
	}

	return true;
}

// Library functions

bool kE::makePhysicsObject(int id, int modelId, SparseSet<PhysicsObject>& pPool, SparseSet<Model>& mPool) {
	if (mPool.idToIndex(modelId) < 0)
		return false;

	Model& model = mPool.getFromId(modelId);

	pPool.add(id, PhysicsObject());
	pPool.getFromId(id).modelId = modelId;
	model.instanceData.add(id, Instance());
	return true;
}

void kE::destroyPhysicsObject(int id, SparseSet<PhysicsObject>& pPool, SparseSet<Model>& mPool) {
	if (pPool.idToIndex(id) < 0)
		return;

	PhysicsObject& obj = pPool.getFromId(id);

	if (mPool.idToIndex(obj.modelId) > -1) {
		Model& model = mPool.getFromId(obj.modelId);
		model.instanceData.removeId(id);
	}

	pPool.removeId(id);
}

void kE::setPosition(int id, Vec3f pos, SparseSet<PhysicsObject>& pPool, SparseSet<Model>& mPool) {
	if (pPool.idToIndex(id) < 0)
		return;

	PhysicsObject& object = pPool.getFromId(id);
	object.position = pos;

	if (mPool.idToIndex(object.modelId) > -1) {
		Model& model = mPool.getFromId(object.modelId);
		Instance& inst = model.instanceData.getFromId(id);
		inst.transform = makeTransformMatrix(object.position, object.scale, object.rotation);
	}
}

void kE::incrementPosition(int id, Vec3f pos, SparseSet<PhysicsObject>& pPool, SparseSet<Model>& mPool) {
	if (pPool.idToIndex(id) < 0)
		return;

	pPool.getFromId(id).position += pos;
}

void kE::setScale(int id, Vec3f sc, SparseSet<PhysicsObject>& pPool, SparseSet<Model>& mPool) {
	if (pPool.idToIndex(id) < 0)
		return;

	PhysicsObject& object = pPool.getFromId(id);
	object.scale = sc;

	if (mPool.idToIndex(object.modelId) > -1) {
		Model& model = mPool.getFromId(object.modelId);
		Instance& inst = model.instanceData.getFromId(id);
		inst.transform = makeTransformMatrix(object.position, object.scale, object.rotation);
	}
}

void kE::incrementScale(int id, Vec3f sc, SparseSet<PhysicsObject>& pPool, SparseSet<Model>& mPool) {
	if (pPool.idToIndex(id) < 0)
		return;

	pPool.getFromId(id).scale += sc;
}

void kE::setRotation(int id, Vec3f rot, SparseSet<PhysicsObject>& pPool, SparseSet<Model>& mPool) {
	if (pPool.idToIndex(id) < 0)
		return;

	PhysicsObject& object = pPool.getFromId(id);
	object.rotation = rot;

	if (mPool.idToIndex(object.modelId) > -1) {
		Model& model = mPool.getFromId(object.modelId);
		Instance& inst = model.instanceData.getFromId(id);
		inst.transform = makeTransformMatrix(object.position, object.scale, object.rotation);
	}
}

void kE::incrementRotation(int id, Vec3f rot, SparseSet<PhysicsObject>& pPool, SparseSet<Model>& mPool) {
	if (pPool.idToIndex(id) < 0)
		return;

	pPool.getFromId(id).rotation += rot;
}

void kE::runPhysics(SparseSet<PhysicsObject>& pPool, SparseSet<Model>& mPool, float dt) {
	for (int i = 0; i < pPool.getSize(); i++) {
		//profToggle();
		PhysicsObject& obj = pPool.getFromIndex(i);

		if (obj.flags & KE_PHYSFLAG_MOVES) {
			incrementPosition(pPool.indexToId(i), obj.velocity * dt, pPool, mPool);

			if (obj.flags & KE_PHYSFLAG_GRAVITY)
				obj.velocity.y -= KE_PHYSCONFIG_GRAVITY * dt;
		}
		
		// Collision Detection
		//profToggle();
		if ((obj.flags & KE_PHYSFLAG_COLLIDES)) {
			for (int j = 0; j < pPool.getSize(); j++) {
				if (i == j || !(pPool.getFromIndex(j).flags & KE_PHYSFLAG_COLLIDES))
					continue;

				PhysicsObject& other = pPool.getFromIndex(j);

				handleCollisions(obj, other, dt);
			}
		} else {
			obj.velocity = {0, 0, 0};
		}
		//cout << "Individual collision detection: " << profToggle() << "\n";

		// Updating transforms
		if (obj.flags & KE_PHYSFLAG_MOVES && mPool.idToIndex(obj.modelId) > -1) {
			Model& model = mPool.getFromId(obj.modelId);
			Instance& inst = model.instanceData.getFromId(pPool.indexToId(i));
			inst.transform = makeTransformMatrix(obj.position, obj.scale, obj.rotation);
		}
		//cout << profToggle() << "\n";
	}
	//cout << objs << "\n";
}