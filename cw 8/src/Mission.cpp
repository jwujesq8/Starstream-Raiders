#include "Mission.h"



Mission::Mission()
{
}

Mission::Mission(float duration, glm::vec3 destination, float rotationSpeed, std::string albedoPath, std::string normalPath, std::string roughnessPath, std::string metallicPath) :
	duration(duration), destination(destination), rotationSpeed(rotationSpeed)
{
	containerTextures.albedo = Core::LoadTexture(albedoPath.data());
	containerTextures.normal = Core::LoadTexture(normalPath.data());
	containerTextures.roughness = Core::LoadTexture(roughnessPath.data());
	containerTextures.metallic = Core::LoadTexture(metallicPath.data());
	missionStartTime = 0.0f;
	missionActive = false;
	missionFailTime = 0.0f;
	missionSuccessTime = 0.0f;
}
Mission::Mission(float duration, Planet planet, std::string albedoPath, std::string normalPath, std::string roughnessPath, std::string metallicPath) : duration(duration)
{
	destination = planet.translation;
	rotationSpeed = planet.rotationSpeed;
	containerTextures.albedo = Core::LoadTexture(albedoPath.data());
	containerTextures.normal = Core::LoadTexture(normalPath.data());
	containerTextures.roughness = Core::LoadTexture(roughnessPath.data());
	containerTextures.metallic = Core::LoadTexture(metallicPath.data());
	missionStartTime = 0.0f;
	missionActive = false;
	missionFailTime = 0.0f;
	missionSuccessTime = 0.0f;
}
void Mission::StartMission()
{
	missionStartTime = glfwGetTime();
	missionActive = true;
}
float Mission::TimeLeft()
{
	float timeLeft = missionStartTime + duration - glfwGetTime();
	if (timeLeft < 0)
	{
		missionActive = false;
		missionFailTime = glfwGetTime();
	}
	return round(timeLeft);
}

Core::RenderContext Mission::CargoContext()
{
	return cargoContext;
}

bool Mission::IsActive()
{
	return missionActive;
}

GLuint Mission::Albedo()
{
	return containerTextures.albedo;
}
GLuint Mission::Normal()
{
	return containerTextures.normal;
}
GLuint Mission::Roughness()
{
	return containerTextures.roughness;
}
GLuint Mission::Metallic()
{
	return containerTextures.metallic;
}

glm::vec3 Mission::TranslationVector()
{
	return destination;
}
float Mission::RotationSpeed()
{
	return rotationSpeed;
}

float Mission::MissionFailTime()
{
	return missionFailTime;
}

float Mission::MissionSuccessTime()
{
	return missionSuccessTime;
}

void Mission::EndMission()
{
	missionActive = false;
	missionSuccessTime = glfwGetTime();
}


