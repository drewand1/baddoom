#define MOVEMENT_SPEED 20.0f
#define CAM_SENSITIVITY 0.2f;

#define GLFW_INCLUDE_NONE
#define STB_IMAGE_IMPLEMENTATION
#include <vector>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <string>
#include "profile.h"
#include "kgbrand.h"
#include "vec.hpp"
#include "sparse.h"
#include "shaders/basicshaders.h"
#include "glboilerplate.h"
#include "model.h"
#include "physics.h"

using namespace std;
using namespace glm;
using namespace kE;

#ifdef NDEBUG
const bool DEBUG = false;
#else
const bool DEBUG = true;
#endif

GLFWwindow* window = nullptr;
unsigned int vao = 0, vbo = 0, ebo = 0, program = 0, noTex = 0;
int catModel = -1, cubeModel = -1;

glm::vec3 camPos(10.0f, 0, 50.0f);
glm::vec3 camFront(0, 0, -1.0f);
glm::vec3 camUp(0, 1.0f, 0);
glm::vec3 camRot(0.0f, 0.0f, 0.0f);

mat4 view = mat4(1.0f);
mat4 projection;

float lastFrame, deltaTime = 0, lastMX = 400, lastMY = 300;

SparseSet<Model> modelPool(10000);
SparseSet<PhysicsObject> physicsObjectPool(10000);

// FUNCTIONS

int newId() {
	static int id = 0;
	id++;
	return id;
}

void mouseCallback(GLFWwindow* w, double mx, double my) {
	float dx = mx - lastMX, dy = my - lastMY;
	lastMX = mx;
	lastMY = my;

	camRot.x -= dy * CAM_SENSITIVITY;
	camRot.y += dx * CAM_SENSITIVITY;

	if (camRot.x > 89.0f)
		camRot.x = 89.0f;
	if (camRot.x < -89.0f)
		camRot.x = -89.0f;

	camFront.x = cos(glm::radians(camRot.y)) * cos(glm::radians(camRot.x));
	camFront.y = sin(glm::radians(camRot.x));
	camFront.z = sin(glm::radians(camRot.y)) * cos(glm::radians(camRot.x));
	camFront = glm::normalize(camFront);

	
}

void drawModel(Model& model) {
	if (model.instanceData.getSize() == 0)
		return;

	if (model.reuploadToGPUOnDraw) {
		glBindBuffer(GL_ARRAY_BUFFER, model.ibo);
		glBufferData(GL_ARRAY_BUFFER, model.instanceData.getSize() * sizeof(Instance), &(model.instanceData.getFromIndex(0)), GL_DYNAMIC_DRAW);
	}

	for (int i = 0; i < model.meshes.size(); i++) {
		glBindVertexArray(model.meshes[i].vao);
		glBindTexture(GL_TEXTURE_2D, model.meshes[i].texture);
		glDrawElementsInstanced(GL_TRIANGLES, model.meshes[i].nIndices, GL_UNSIGNED_INT, 0, model.instanceData.getSize());
	}
}

void processMovementInput(GLFWwindow* w) {
	if (glfwGetKey(w, GLFW_KEY_W)) {
		cout << camPos.x << " " << camPos.y << " " << camPos.z << "\n";
		camPos += camFront * MOVEMENT_SPEED * deltaTime;
	}

	if (glfwGetKey(w, GLFW_KEY_S))
		camPos -= camFront * MOVEMENT_SPEED * deltaTime;

	if (glfwGetKey(w, GLFW_KEY_A))
		camPos -= glm::normalize(glm::cross(camFront, camUp)) * MOVEMENT_SPEED * deltaTime;
	
	if (glfwGetKey(w, GLFW_KEY_D))
		camPos += glm::normalize(glm::cross(camFront, camUp)) * MOVEMENT_SPEED * deltaTime;
}

// MAIN

int main() {
	cout << "Debug: " << DEBUG << endl;
	if (!glfwInit()) {
		cerr << "No GLFW init" << endl;
		return -1;
	}

	window = glfwCreateWindow(640, 480, "baddoom", nullptr, nullptr);
	if (!window) {
		cerr << "No window" << endl;
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouseCallback);

	gladLoadGL();
	glViewport(0, 0, 640, 480);
	
	program = mkShaderProgram(Shaders::basicVtx, Shaders::basicFrag);
	glUseProgram(program);
	
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vbo);
	glGenBuffers(1, &ebo);

	noTex = mkTexture("..\\rsc\\textures\\kgnotex.png");

	catModel = newId();
	modelPool.add(catModel, Model(10000));
	cubeModel = newId();
	modelPool.add(cubeModel, Model(10000));


	if (!loadCum(modelPool.getFromId(catModel), "..\\rsc\\models\\cat\\cat.cum", "..\\rsc\\models\\cat\\")) {
		cerr << "Cannot load object!" << endl;
		return -1;
	}

	if (!loadCum(modelPool.getFromId(cubeModel), "..\\rsc\\models\\cube\\cube.cum", "..\\rsc\\models\\cube\\")) {
		cerr << "Cannot load object!" << endl;
		return -1;
	}

	Model& cat = modelPool.getFromId(catModel);
	Model& cube = modelPool.getFromId(cubeModel);

	int jim = newId();
	makePhysicsObject(jim, catModel, physicsObjectPool, modelPool);
	setPosition(jim, {0, 8.0f, 0}, physicsObjectPool, modelPool);
	setScale(jim, {0.2f, 0.2f, 0.2f}, physicsObjectPool, modelPool);
	setRotation(jim, {glm::radians(-90.0f), 0, 0}, physicsObjectPool, modelPool);
	PhysicsObject& jimPhys = physicsObjectPool.getFromId(jim);
	jimPhys.flags = KE_PHYSFLAG_GRAVITY | KE_PHYSFLAG_COLLIDES | KE_PHYSFLAG_MOVES;
	jimPhys.velocity = {20.0f, 0, 0};

	for (int i = 0; i < 20; i++) {
		for (int j = 0; j < 10; j++) {
			int thing = newId();
			makePhysicsObject(thing, catModel, physicsObjectPool, modelPool);
			setPosition(thing, {(i - 10.0f) * 2, 50, (j - 5.0f) * 3}, physicsObjectPool, modelPool);
			setScale(thing, {0.1f, 0.1f, 0.1f}, physicsObjectPool, modelPool);
			PhysicsObject& thingPhys = physicsObjectPool.getFromId(thing);
			thingPhys.flags = KE_PHYSFLAG_GRAVITY | KE_PHYSFLAG_COLLIDES | KE_PHYSFLAG_MOVES;
		}
	}

	int floor = newId();
	makePhysicsObject(floor, cubeModel, physicsObjectPool, modelPool);
	setPosition(floor, {0, -6.0f, 0}, physicsObjectPool, modelPool);
	setScale(floor, {100.0f, 0.2f, 100.0f}, physicsObjectPool, modelPool);
	setRotation(floor, {0, 0, 0}, physicsObjectPool, modelPool);
	PhysicsObject& floorPhys = physicsObjectPool.getFromId(floor);
	floorPhys.flags = KE_PHYSFLAG_COLLIDES;
	floorPhys.boundingBoxSize = {100.0f, 0.2f, 100.0f};
	floorPhys.mass = 2.0f;
	
	int wall = newId();
	makePhysicsObject(wall, cubeModel, physicsObjectPool, modelPool);
	setPosition(wall, {10.0f, 5.0f, 0}, physicsObjectPool, modelPool);
	setScale(wall, {1.0f, 5.0f, 10.0f}, physicsObjectPool, modelPool);
	PhysicsObject& wallPhys = physicsObjectPool.getFromId(wall);
	wallPhys.flags = KE_PHYSFLAG_COLLIDES | KE_PHYSFLAG_MOVES | KE_PHYSFLAG_GRAVITY;
	wallPhys.boundingBoxSize = {3.0f, 10.0f, 10.0f};
	wallPhys.mass = 1.5f;

	projection = glm::perspective(glm::radians(45.0f), 640.0f / 480.0f, 0.1f, 1000.0f);
	view = glm::lookAt(
		camPos,
		camPos + camFront,
		camUp
	);

	int viewLoc = glGetUniformLocation(program, "view");
	glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
	int projLoc = glGetUniformLocation(program, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
	int colorLoc = glGetUniformLocation(program, "color");
	glUniform4f(colorLoc, 1.0f, 1.0f, 1.0f, 1.0f);


	glEnable(GL_DEPTH_TEST);

	lastFrame = glfwGetTime();

	while (!glfwWindowShouldClose(window)) {
		float now = glfwGetTime();
		deltaTime = now - lastFrame;
		lastFrame = now;

		processMovementInput(window);
		view = glm::lookAt(
			camPos,
			camPos + camFront,
			camUp
		);
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

		runPhysics(physicsObjectPool, modelPool, deltaTime);

		glClearColor(0.7f, 0.9f, 0.9f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(program);
		for (int i = 0; i < modelPool.getSize(); i++)
			drawModel(modelPool.getFromIndex(i));

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
	glfwTerminate();
}