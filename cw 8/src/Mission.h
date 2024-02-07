#pragma once
#include <GL/glew.h>
#include <glm.hpp>
#include <GLFW/glfw3.h>
#include <cmath>
#include "Texture.h"
#include "Timer.h"
#include "Render_Utils.h"

struct Planet {
	glm::vec3 translation;
	float scaleFactor;
	float rotationSpeed;
	GLuint albedo;
	GLuint normal;
	GLuint roughness;
	GLuint metallic;
	Planet(glm::vec3 translation, float scaleFactor, float rotationSpeed, GLuint albedo, GLuint normal, GLuint roughness, GLuint metallic) :
		translation(translation), scaleFactor(scaleFactor), rotationSpeed(rotationSpeed), albedo(albedo), normal(normal), roughness(roughness), metallic(metallic) {};
};


struct ContainerTextures {
	GLuint albedo;
	GLuint normal;
	GLuint roughness;
	GLuint metallic;
};

class Mission {
private:
	float duration;
	glm::vec3 destination;
	float rotationSpeed;
	float missionStartTime;
	bool missionActive;
	float missionFailTime;
	float missionSuccessTime;
	Core::RenderContext cargoContext;
	ContainerTextures containerTextures;

public:
	Mission();
	Mission(float duration, glm::vec3 destination, float rotationSpeed, std::string albedoPath, std::string normalPath, std::string roughnessPath, std::string metallicPath);
	Mission(float duration, Planet planet, std::string albedoPath, std::string normalPath, std::string roughnessPath, std::string metallicPath);
	void StartMission();
	float TimeLeft();
	Core::RenderContext CargoContext();
	GLuint Albedo();
	GLuint Normal();
	GLuint Roughness();
	GLuint Metallic();
	bool IsActive();
	glm::vec3 TranslationVector();
	float RotationSpeed();
	float MissionFailTime();
	float MissionSuccessTime();
	void EndMission();
};

void setNextMission();
