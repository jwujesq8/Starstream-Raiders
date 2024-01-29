#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>

#include "Shader_Loader.h"
#include "Render_Utils.h"
#include "Texture.h"

#include "Box.cpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <gtx/euler_angles.hpp>
#include <GLFW/glfw3.h>
#include <list>
#include "SpaceshipModelList.h"

class Alien {
	const int maxHp;
	Core::RenderContext model;
	int hp;
	int damage;
	GLuint texture;
	glm::vec3 position;
	glm::vec3 direction;

	bool shoot(glm::vec3 playerPosition)
	{

	}

};