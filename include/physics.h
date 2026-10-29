#ifndef KE_PHYSICS_H
#define KE_PHYSICS_H

#ifndef KE_PHYSCONFIG_GRAVITY
#define KE_PHYSCONFIG_GRAVITY 20.0f
#endif

#define KE_PHYSFLAG_COLLIDES 0x1
#define KE_PHYSFLAG_GRAVITY 0x2
#define KE_PHYSFLAG_MOVES 0x4

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <stdint.h>
#include "vec.hpp"
#include "model.h"
#include "sparse.h"

namespace kE {

struct PhysicsObject {
	Vec3f position = {0, 0, 0};
	Vec3f scale = {1, 1, 1};
	Vec3f boundingBoxSize = {1, 1, 1};
	Vec3f rotation = {0, 0, 0};
	Vec3f velocity = {0, 0, 0};
	float elasticity = 1.0f;
	float friction = 10.0f;
	float mass = 1.0f;
	uint8_t flags = 0;
	int modelId = -1;
};

bool makePhysicsObject(int id, int modelId, SparseSet<PhysicsObject>& pPool, SparseSet<Model>& mPool);
void destroyPhysicsObject(int id, SparseSet<PhysicsObject>& pPool, SparseSet<Model>& mPool);

void setPosition(int id, Vec3f pos, SparseSet<PhysicsObject>& pPool, SparseSet<Model>& mPool);
void incrementPosition(int id, Vec3f pos, SparseSet<PhysicsObject>& pPool, SparseSet<Model>& mPool);

void setScale(int id, Vec3f sc, SparseSet<PhysicsObject>& pPool, SparseSet<Model>& mPool);
void incrementScale(int id, Vec3f sc, SparseSet<PhysicsObject>& pPool, SparseSet<Model>& mPool);

void setRotation(int id, Vec3f rot, SparseSet<PhysicsObject>& pPool, SparseSet<Model>& mPool);
void incrementRotation(int id, Vec3f rot, SparseSet<PhysicsObject>& pPool, SparseSet<Model>& mPool);

void runPhysics(SparseSet<PhysicsObject>& pPool, SparseSet<Model>& mPool, float dt);

}

#endif