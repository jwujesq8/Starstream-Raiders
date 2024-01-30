#include "GL/glew.h"
#include "glm.hpp"
#include <iostream>
#include <cmath>

#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Texture.h"

#include <string>
#include <gtx/euler_angles.hpp>
#include <GLFW/glfw3.h>
#include <list>
#include "Calculations.cpp"
#include "SpaceshipModelList.h"

class SpaceTraveler {
private:
	int maxHp;
	SpaceshipModel spaceshipModel;
	int hp;
	int damage;
	vec3 size;
	glm::vec3 position;
	glm::vec3 direction;
	bool isAlive = true;

	void getShot(int damage)
	{
		hp -= damage;
		if (hp < 0)
			isAlive = false;
	}

public:
	SpaceTraveler(int maxHp, SpaceshipModel spaceshipModel, int damage,	GLuint texture,	glm::vec3 position,	glm::vec3 direction, glm::vec3 size) {
		this->maxHp = maxHp;
		this->spaceshipModel = spaceshipModel;
		this->hp = maxHp;
		this->damage = damage;
		this->position = position;
		this->direction = direction;
		this->size = size;
	}
	//Properties
	const vec3 Position() const{
		return position;
	}
	const vec3 Direction() const {
		return direction;
	}

	//Functions
	void shoot(std::vector<SpaceTraveler> targets)
	{

		vec3 targetsMin;
		vec3 targetsMax;

		for (int i = 0; i < targets.size(); i++) {
			calculatePlayerBoundingBox(position, size, direction, targetsMin, targetsMax);
			if (rayCubeCollizion(direction, position, targetsMin, targetsMax))
				targets[i].getShot(damage);
		}
	}
};