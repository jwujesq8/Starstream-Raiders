#pragma once
#ifndef SPACE_TRAVELER_H
#define SPACE_TRAVELER_H

#include "glm.hpp"
#include <vector>
#include <string>
#include "Calculations.h"
#include "SpaceshipModelList.h"

class SpaceTraveler {
private:
    int maxHp;
    SpaceshipModelList spaceshipModelList;
    int hp;
    int damage;
    glm::vec3 size;
    glm::vec3 position;
    glm::vec3 direction;
    bool isAlive;
    float speed;
    float angleSpeed = 0.001f;
    float moveSpeed = 0.001f;

    void getShot(int damage);

public:
    SpaceTraveler(int maxHp, SpaceshipModelList spaceshipModelList, int damage, glm::vec3 position, glm::vec3 direction, glm::vec3 size);

    // Properties
    const glm::vec3 Position() const;
    const glm::vec3 Direction() const;
    const glm::vec3 Size() const;
    SpaceshipModel getSpaceshipModel();

    // Functions
    void shoot(std::vector<SpaceTraveler> targets);
    void forward();
    void backward();
    void turnLeft();
    void turnRight();
    void turnUp();
    void turnDown();
};

#endif // SPACE_TRAVELER_H
