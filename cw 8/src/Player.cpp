#include "Player.h"

Player::Player(int maxHp, SpaceshipModel spaceshipModelList, int damage, glm::vec3 position, glm::vec3 direction, glm::vec3 size, float batteryCapacity) : SpaceTraveler(maxHp, spaceshipModelList, damage, position, direction, size, 1.5)
{
	this->batteryCapacity = batteryCapacity;
	this->batteryLeft = batteryCapacity;
}

float Player::BatteryLeft() const
{
	return batteryLeft;
}

float Player::BatteryCapacity() const
{
	return batteryCapacity;
}

void Player::forward(bool doubleSpeed)
{
	if (batteryLeft > 0) {
		SpaceTraveler::forward(doubleSpeed);
		batteryLeft -= batteryDrainSpeed;
	}
	else {
		std::cout << "Your ship needs to be charged" << std::endl;
	}
}

void Player::backward(bool doubleSpeed)
{
	if (batteryLeft > 0) {
		SpaceTraveler::backward(doubleSpeed);
		batteryLeft -= batteryDrainSpeed;
	}
	else {
		std::cout << "Your ship needs to be charged" << std::endl;
	}
}

void Player::shoot(std::vector<SpaceTraveler>& targets)
{
	if (batteryLeft > 0) {
		SpaceTraveler::shoot(targets);
		batteryLeft -= batteryDrainSpeed * 100;
	}
	else {
		std::cout << "Your ship needs to be charged" << std::endl;
	}
}

void Player::charge()
{
	batteryLeft = batteryCapacity;
	std::cout << "battery condition: " << batteryLeft << std::endl;
}


