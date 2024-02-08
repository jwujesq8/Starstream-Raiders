#include <GLFW/glfw3.h>
#include "Space_Traveler.h"
#include "Calculations.h" // Include for rayCubeCollision function
#include "SpaceshipModelList.h" // Include for SpaceshipModelList class definition
#include <gtx/euler_angles.hpp>


SpaceTraveler::SpaceTraveler(int maxHp, SpaceshipModel spaceshipModel, int damage, glm::vec3 position, glm::vec3 direction, glm::vec3 size) :
    maxHp(maxHp), spaceshipModel(spaceshipModel), hp(maxHp), damage(damage), position(position), direction(direction), size(size), isAlive(true) {}

// Private member function
void SpaceTraveler::getShot(int damage) {
    hp -= damage;
    if (hp <= 0)
        isAlive = false;
}

// Properties
const glm::vec3 SpaceTraveler::Position() const {
    return position;
}

const glm::vec3 SpaceTraveler::Direction() const {
    return direction;
}

const glm::vec3 SpaceTraveler::Size() const {
    return size;
}

const float SpaceTraveler::LastTimeShot() const {
	return lastTimeShot;
}

const float SpaceTraveler::Cooldown() const
{
    return cooldown;
}

SpaceshipModel SpaceTraveler::getSpaceshipModel() {
    return spaceshipModel;
}
void SpaceTraveler::setSpaceshipModel(SpaceshipModel spaceshipModel) {
	this->spaceshipModel = spaceshipModel;
}

bool SpaceTraveler::IsAlive() {
	return isAlive;
}

// Functions
void SpaceTraveler::shoot(std::vector<SpaceTraveler>& targets) {
    glm::vec3 targetsMin;
    glm::vec3 targetsMax;

    lastTimeShot = glfwGetTime();

    for (int i = 0; i < targets.size(); i++) {
        Calculations::calculatePlayerBoundingBox(targets[i].Position(), targets[i].Size(), targets[i].Direction(), targetsMin, targetsMax);
        if (Calculations::rayCubeCollision(direction, position, targetsMin, targetsMax))
            targets[i].getShot(damage);
    }
}


void SpaceTraveler::forward(bool doubleSpeed) {
    float s = moveSpeed;
    if (doubleSpeed) s = moveSpeed * 2.7;
    position += direction * s;
}

void SpaceTraveler::backward(bool doubleSpeed) {
    float s = moveSpeed;
    if (doubleSpeed) s = moveSpeed * 2.7;
    position -= direction * s;
}

void SpaceTraveler::turnLeft() {
    direction = glm::vec3(glm::eulerAngleY(angleSpeed) * glm::vec4(direction, 0));
}

void SpaceTraveler::turnRight() {
    direction = glm::vec3(glm::eulerAngleY(-angleSpeed) * glm::vec4(direction, 0));
}

void SpaceTraveler::turnUp() {
    glm::vec3 right = glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f)));
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angleSpeed, right);
    direction = glm::vec3(rotation * glm::vec4(direction, 0));
}

void SpaceTraveler::turnDown() {
    glm::vec3 right = glm::normalize(glm::cross(direction, glm::vec3(0.0f, 1.0f, 0.0f)));
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), -angleSpeed, right);
    direction = glm::vec3(rotation * glm::vec4(direction, 0));
}

void SpaceTraveler::move(glm::vec3 pos, glm::vec3 dir) {
    this->position = pos;
    this->direction = dir;
}
