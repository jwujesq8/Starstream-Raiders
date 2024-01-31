#include "Space_Traveler.h"
#include "Calculations.h" // Include for rayCubeCollision function
#include "SpaceshipModelList.h" // Include for SpaceshipModelList class definition
#include <gtx/euler_angles.hpp>

SpaceTraveler::SpaceTraveler(int maxHp, SpaceshipModelList spaceshipModelList, int damage, glm::vec3 position, glm::vec3 direction, glm::vec3 size) :
    maxHp(maxHp), spaceshipModelList(spaceshipModelList), hp(maxHp), damage(damage), position(position), direction(direction), size(size),
    isAlive(true), speed(0.1f), angleSpeed(0.001f), moveSpeed(0.001f) {}

// Private member function
void SpaceTraveler::getShot(int damage) {
    hp -= damage;
    if (hp < 0)
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

SpaceshipModel SpaceTraveler::getSpaceshipModel() {
    return spaceshipModelList.getCurrentSpaceshipModel();
}

// Functions
void SpaceTraveler::shoot(std::vector<SpaceTraveler> targets) {
    glm::vec3 targetsMin;
    glm::vec3 targetsMax;

    for (int i = 0; i < targets.size(); i++) {
        Calculations::calculatePlayerBoundingBox(targets[i].Position(), targets[i].Size(), targets[i].Direction(), targetsMin, targetsMax);
        if (Calculations::rayCubeCollision(direction, position, targetsMin, targetsMax))
            targets[i].getShot(damage);
    }
}

void SpaceTraveler::forward() {
    position += direction * moveSpeed;
}

void SpaceTraveler::backward() {
    position -= direction * moveSpeed;
}

void SpaceTraveler::turnLeft() {
    direction = glm::vec3(glm::eulerAngleY(angleSpeed) * glm::vec4(direction, 0));
}

void SpaceTraveler::turnRight() {
    direction = glm::vec3(glm::eulerAngleY(-angleSpeed) * glm::vec4(direction, 0));
}

void SpaceTraveler::turnUp() {
    direction = glm::vec3(glm::eulerAngleX(angleSpeed) * glm::vec4(direction, 0));
}

void SpaceTraveler::turnDown() {
    direction = glm::vec3(glm::eulerAngleX(-angleSpeed) * glm::vec4(direction, 0));
}
