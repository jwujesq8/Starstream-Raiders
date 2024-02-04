#include "Space_Traveler.h"

class Player : public SpaceTraveler {
private:
	float batteryDrainSpeed = 0.005;
	float batteryCapacity;
	float batteryLeft;

public:
	Player(int maxHp, SpaceshipModel spaceshipModelList, int damage, glm::vec3 position, glm::vec3 direction, glm::vec3 size, float batteryCapacity);

	float BatteryLeft() const;

	void forward(bool doubleSpeed);
	void backward(bool doubleSpeed);
	void shoot(std::vector<SpaceTraveler>& targets);
	void charge();
	
};