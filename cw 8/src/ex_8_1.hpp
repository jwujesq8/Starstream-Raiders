﻿#include "GL/glew.h"
#include <GLFW/glfw3.h>
#include "glm.hpp"
#include "ext.hpp"
#include <iostream>
#include <cmath>
#include <algorithm>

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
#include "Space_Traveler.h"
#include "Timer.h"
#include <gtx/vector_angle.hpp>
#include "Player.h"
#include <ft2build.h>
#include "Characters.h"
#include "Mission.h"




#include FT_FREETYPE_H  
std::vector<Planet> planets = {
};

namespace texture {
	GLuint earth;
	GLuint clouds;
	GLuint moon;
	GLuint ship;
	GLuint spaceshipAlbedo; GLuint spaceshipNormal; GLuint spaceshipMetallic; GLuint spaceshipRoughness;

	GLuint skybox;


	GLuint empty;

	GLuint earthNormal;
	GLuint shipNormal;

	GLuint planetContinentBase;
	GLuint planetContinentNormal;
	GLuint planetContinentRoughness;
	GLuint planetContinentMetallic;
	GLuint planetContinentClouds;
	GLuint planetContinentCloudsNormal;
	GLuint planetContinentCloudsOpacity;

	GLuint planetBarrenBase;
	GLuint planetBarrenNormal;
	GLuint planetBarrenRoughness;
	
	GLuint planetFrozenBase;
	GLuint planetFrozenNormal;
	GLuint planetFrozenRoughness;

	GLuint planetGasBase;
	GLuint planetGasNormal;
	GLuint planetGasClouds;
	GLuint planetGasCloudsNormal;
	GLuint planetGasCloudsOpacity;

	GLuint planetLavaBase;
	GLuint planetLavaNormal;
	GLuint planetLavaRoughness;

	GLuint planetSmacBase;
	GLuint planetSmacNormal;
	GLuint planetSmacRoughness;
	GLuint planetSmacClouds;
	GLuint planetSmacCloudsNormal;
	GLuint planetSmacCloudsOpacity;

	GLuint sun;

	GLuint asteroidBaseColor_0;

	GLuint asteroidBaseColor_2;
	GLuint asteroidMetallic_2;
	GLuint asteroidNormal_2;
	GLuint asteroidRoughness_2;

	GLuint asteroidBaseColor_3;
	GLuint asteroidMetallic_3;
	GLuint asteroidNormal_3;
	std::vector<GLuint> enemyTextures; std::vector<GLuint> enemyNormals; std::vector<GLuint> enemyRoughnesses; std::vector<GLuint> enemyMetallics;


}


GLuint program;
GLuint programSun;
GLuint programTex;
GLuint programSkybox;
GLuint programLaser;
GLuint programText;
Core::Shader_Loader shaderLoader;

Core::RenderContext shipContext;
std::vector<Core::RenderContext> enemyContexts;

Core::RenderContext sphereContext;
Core::RenderContext station;
Core::RenderContext planet;
Core::RenderContext sun;
Core::RenderContext asteroidContext_0;
Core::RenderContext asteroidContext_2;
Core::RenderContext asteroidContext_3;
Core::RenderContext bulletContext;

Core::RenderContext cubeContext;

glm::vec3 cameraPos = glm::vec3(-4.f, 0, 0);
glm::vec3 cameraDir = glm::vec3(1.f, 0.f, 0.f);


glm::mat4 stationPosMatrix = glm::translate(glm::mat4(1.0), glm::vec3(13.0f, 0.f, 19.0f));
std::vector<Mission> missions = {};
Mission currentMission;

// for stable ships score
float lastFrameTime = 0;
float deltaTime;
float missionSuccess = 0.0;
bool win = false;
GLuint VAO,VBO;

float aspectRatio = 1.f;

float exposition = 1.f;

glm::vec3 lightPos = glm::vec3(0.f, 10.f, 0.f);
glm::vec3 lightColor = glm::vec3(0.9, 0.7, 0.8)*2000.0f;

glm::vec3 spotlightPos = glm::vec3(0, 0, 0);
glm::vec3 spotlightConeDir = glm::vec3(0, 0, 0);
glm::vec3 spotlightColor = glm::vec3(0.5, 0.9, 0.8)*10.0f;
float spotlightPhi = 3.14 / 3;


SpaceshipModelList spaceshipModels;

Player player(100, spaceshipModels.getCurrentSpaceshipModel(), 10, glm::vec3(10.0, 1.000000f, -7.124680f), glm::vec3(-0.354510f, 0.000000f, 0.935054f), glm::vec3(1.0), 200);
std::vector<SpaceTraveler> enemies = {};

float scaleModelIndex = 0.1;
glm::vec3 translateModelVec = glm::vec3(0, 0, 0);
float shotDuration = 1.0f;
float maxBoidsSpeed = 0.005f;

unsigned int VAOtext, VBOtext;
glm::mat4 projection = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);

bool isShipOnStation(SpaceTraveler ship) {
	glm::vec3 stationPosition = glm::vec3(stationPosMatrix[3]);

	float threshold = 1.5f;
	float shipStationdistance = glm::distance(ship.Position(), stationPosition);

	return shipStationdistance < threshold;
}

bool isShipNearDestination(SpaceTraveler ship, Mission mission) {
	float threshold = 5.0f;
	glm::vec3 shipPos = ship.Position();
	glm::vec3 planetPosBeforeRotation = mission.TranslationVector();
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0), (float)glfwGetTime() * mission.RotationSpeed(), glm::vec3(0, 1, 0));
	glm::vec4 planetPosAfterRotation = rotationMatrix * glm::vec4(planetPosBeforeRotation, 1.0);
	float distance = glm::length(shipPos - glm::vec3(planetPosAfterRotation));
	return distance < threshold;
}

glm::mat4 createCameraMatrix()
{
	glm::vec3 cameraSide = glm::normalize(glm::cross(cameraDir,glm::vec3(0.f,1.f,0.f)));
	glm::vec3 cameraUp = glm::normalize(glm::cross(cameraSide,cameraDir));
	glm::mat4 cameraRotrationMatrix = glm::mat4({
		cameraSide.x,cameraSide.y,cameraSide.z,0,
		cameraUp.x,cameraUp.y,cameraUp.z ,0,
		-cameraDir.x,-cameraDir.y,-cameraDir.z,0,
		0.,0.,0.,1.,
		});
	cameraRotrationMatrix = glm::transpose(cameraRotrationMatrix);
	glm::mat4 cameraMatrix = cameraRotrationMatrix * glm::translate(glm::mat4(1.0), -cameraPos);

	return cameraMatrix;
}

glm::mat4 createPerspectiveMatrix()
{
	
	glm::mat4 perspectiveMatrix;
	float n = 0.05;
	float f = 2000.f;
	float a1 = glm::min(aspectRatio, 1.f);
	float a2 = glm::min(1 / aspectRatio, 1.f);
	perspectiveMatrix = glm::mat4({
		1,0.,0.,0.,
		0.,aspectRatio,0.,0.,
		0.,0.,(f+n) / (n - f),2*f * n / (n - f),
		0.,0.,-1.,0.,
		});

	
	perspectiveMatrix=glm::transpose(perspectiveMatrix);

	return perspectiveMatrix;
}

void drawObjectColor(Core::RenderContext& context, glm::mat4 modelMatrix, glm::vec3 color, float roughness, float metallic) {
	glUseProgram(program);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

	glUniform1f(glGetUniformLocation(program, "exposition"), exposition);

	glUniform3f(glGetUniformLocation(program, "baseColor"), color.x, color.y, color.z);

	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	glUniform3f(glGetUniformLocation(program, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
	glUniform3f(glGetUniformLocation(program, "lightColor"), lightColor.x, lightColor.y, lightColor.z);

	glUniform1f(glGetUniformLocation(program, "additionalMetallic"), metallic);
	glUniform1f(glGetUniformLocation(program, "additionalRoughness"), roughness);



	/*Core::SetActiveTexture(texture::textureAlbedo, "textureAlbedo", program, 0);
	Core::SetActiveTexture(texture::textureNormal, "textureNormal", program, 1);
	Core::SetActiveTexture(texture::textureMetallic, "textureMetallic", program, 2);
	Core::SetActiveTexture(texture::textureRoughness, "textureRoughness", program, 3);*/
	//Core::SetActiveTexture(texture::textureAO, "textureAO", program, 4);




	Core::DrawContext(context);

}

void drawSkybox(glm::mat4 modelMatrix) {
	glUseProgram(programSkybox);
	glDisable(GL_DEPTH_TEST);

	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(programSkybox, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture::skybox);
	Core::DrawContext(cubeContext);

	glEnable(GL_DEPTH_TEST);
}

void drawStar(Core::RenderContext& context, glm::mat4 modelMatrix, GLuint texture)
{
	glUseProgram(programSun);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(programSun, "transformation"), 1, GL_FALSE, (float*)&transformation);

	Core::SetActiveTexture(texture, "texture1", programSun, 0);

	Core::DrawContext(context);
}

void drawObjectTexture(Core::RenderContext& context, glm::mat4 modelMatrix, GLuint textureID, GLuint normal, GLuint roughness, GLuint metallic) {
	glUseProgram(programTex);
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(programTex, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(programTex, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniform3f(glGetUniformLocation(programTex, "lightPos"), 0, 0, 0);
	glUniform3f(glGetUniformLocation(program, "lightColor"), lightColor.x, lightColor.y, lightColor.z);

	glUniform3f(glGetUniformLocation(program, "cameraPos"), cameraPos.x, cameraPos.y, cameraPos.z);

	Core::SetActiveTexture(textureID, "baseColor", programTex, 0);
	Core::SetActiveTexture(normal, "normalMap", programTex, 1);
	Core::SetActiveTexture(roughness, "rougMap", programTex, 2);
	Core::SetActiveTexture(metallic, "metalMap", programTex, 3);

	Core::DrawContext(context);

}

void drawLaser(SpaceTraveler entity) {

	auto getLaserPower = [](float x)
		{
			return (-4.0f * x * x) + (4.0f * x);
		};

	float time = glfwGetTime();
	float timeDiff = time - entity.LastTimeShot();
	if (timeDiff < shotDuration) {
		float power = getLaserPower(timeDiff);
		glm::vec3 laserPos = entity.Position() + entity.Direction() * entity.Size();
		glm::vec3 laserDir = entity.Direction();
		glm::vec3 laserColor = glm::vec3(1.0, 0.0, 1.0);

		glm::vec3 spaceshipSide = glm::normalize(glm::cross(player.Direction(), glm::vec3(0.f, 1.f, 0.f)));
		glm::vec3 spaceshipUp = glm::normalize(glm::cross(spaceshipSide, player.Direction()));
		glm::mat4 spaceshipCameraRotrationMatrix = glm::mat4({
			spaceshipSide.x,spaceshipSide.y,spaceshipSide.z,0,
			spaceshipUp.x,spaceshipUp.y,spaceshipUp.z ,0,
			-player.Direction().x,-player.Direction().y,-player.Direction().z,0,
			0.,0.,0.,1.,
			});

		glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0), player.Direction()*75.3f) * glm::translate(glm::mat4(1.0), player.Position()) * spaceshipCameraRotrationMatrix * glm::eulerAngleY(glm::pi<float>())
			* glm::rotate(glm::mat4(1.0), glm::pi<float>(), glm::vec3(0, 1, 0)) * glm::scale(glm::mat4(1.0), glm::vec3(0.002f * power, 0.002f * power, 1.0f));

		glUseProgram(programLaser);
		glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
		glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
		glUniformMatrix4fv(glGetUniformLocation(programLaser, "transformation"), 1, GL_FALSE, (float*)&transformation);
		//glUniformMatrix4fv(glGetUniformLocation(programLaser, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);

		glUniform1f(glGetUniformLocation(programLaser, "shotPower"), power);

		glUniform3f(glGetUniformLocation(programLaser, "color"), laserColor.x, laserColor.y, laserColor.z);


		Core::DrawContext(bulletContext);

	}
}

void drawText(std::string text, glm::vec3 color, float x, float y, float scale)
{
	glUseProgram(programText);
	glUniform3f(glGetUniformLocation(programText, "textColor"), color.x, color.y, color.z);
	glUniformMatrix4fv(glGetUniformLocation(programText, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAOtext);

	// iterate through all characters
	std::string::const_iterator c;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		float xpos = x + ch.Bearing.x * scale;
		float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		float w = ch.Size.x * scale;
		float h = ch.Size.y * scale;
		// update VBO for each character
		float vertices[6][4] = {
			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos,     ypos,       0.0f, 1.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },

			{ xpos,     ypos + h,   0.0f, 0.0f },
			{ xpos + w, ypos,       1.0f, 1.0f },
			{ xpos + w, ypos + h,   1.0f, 0.0f }
		};
		// render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBOtext);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

}

void drawDestinationPoint(glm::vec3 translation, float rotationSpeed, Mission mission) {
	glUseProgram(program);
	float time = glfwGetTime();
	glm::mat4 viewProjectionMatrix = createPerspectiveMatrix() * createCameraMatrix();
	glm::vec3 planetPosBeforeRotation = mission.TranslationVector();
	glm::mat4 rotationMatrix = glm::rotate(glm::mat4(1.0), time * mission.RotationSpeed(), glm::vec3(0, 1, 0));
	glm::vec3 planetPosAfterRotation = glm::vec3(rotationMatrix * glm::vec4(planetPosBeforeRotation, 1.0));
	glm::mat4 modelMatrix = glm::translate(glm::mat4(1.0), planetPosAfterRotation + glm::vec3(0.0, 5.f + sin(time) * 0.5, 0.0));
	glm::mat4 transformation = viewProjectionMatrix * modelMatrix;
	glUniformMatrix4fv(glGetUniformLocation(program, "transformation"), 1, GL_FALSE, (float*)&transformation);
	glUniformMatrix4fv(glGetUniformLocation(program, "modelMatrix"), 1, GL_FALSE, (float*)&modelMatrix);
	glUniform3f(glGetUniformLocation(program, "baseColor"), 0.8, 0.1, 0.1);
	Core::DrawContext(sphereContext);
}


glm::vec3 rule1(const SpaceTraveler& enemy, int index) {
	glm::vec3 distance = glm::vec3(0, 0, 0);

	for (int i = 0; i < enemies.size(); i++) {
		if (enemies[i].IsAlive() && i != index) {
			if (glm::distance(enemies[i].Position(), enemy.Position()) < 3.0f) {
				distance = distance - (enemies[i].Position() - enemy.Position());
			}
		}
	}
	return distance;
}

glm::vec3 rule2(const SpaceTraveler& enemy, int index) {
	glm::vec3 pv = glm::vec3(0, 0, 0);

	for (int i = 0; i < enemies.size(); i++) {
		if (enemies[i].IsAlive() && i != index) {
			pv = pv + enemies[i].Direction();
		}
	}
	if (enemies.size() > 1) {
		pv /= static_cast<float>(enemies.size() - 1);
		glm::vec3 enemy_pv = (pv - enemy.Direction()) / static_cast<float>(8);
		return enemy_pv;
	}
	return glm::vec3(0, 0, 0);
}

glm::vec3 tendToTarget(const SpaceTraveler& enemy, const glm::vec3& targetPosition) {
	glm::vec3 desired = targetPosition - enemy.Position();
	glm::vec3 steer = desired - enemy.Direction();
	return steer;
}

//void attackThePlayer(SpaceTraveler& enemy) {
//	if (3.0f < glm::distance(enemy.Position(), player.Position()) < 7.0f) {
//		if (enemy.IsAlive()) {
//			enemy.shoot(player);
//		}
//	}
//}

void enemyMove(SpaceTraveler& enemy, int enemies_index) {
	glm::vec3 v1 = rule1(enemy, enemies_index);
	glm::vec3 v2 = rule2(enemy, enemies_index);
	glm::vec3 v3 = tendToTarget(enemy, player.Position());

	glm::vec3 acceleration = v1 + v2 + v3;
	if (glm::length(acceleration) > maxBoidsSpeed) {
        acceleration = glm::normalize(acceleration) * maxBoidsSpeed;
    }

	// Update direction and position
	glm::vec3 dir = enemy.Direction() + acceleration;
	glm::vec3 pos = enemy.Position() + enemy.Direction() * maxBoidsSpeed;
	enemy.move(pos, dir);

	// Call move function if necessary
	// enemy.move(enemy.Position(), enemy.Direction());
}



void renderScene(GLFWwindow* window)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 transformation;
	float time = glfwGetTime();
	deltaTime = std::min(time - lastFrameTime, 0.1f);
	lastFrameTime = time;

	drawSkybox(glm::translate(glm::mat4(1.0), player.Position()));


	
	glUseProgram(program);

	#pragma region planets
	for (int i = 0; i < planets.size(); i++)
	{
		drawObjectTexture(planet, glm::rotate(glm::mat4(1.0), time * planets[i].rotationSpeed, glm::vec3(0, 1, 0)) * glm::translate(glm::mat4(1.0), planets[i].translation) * glm::scale(glm::mat4(1.0), glm::vec3(planets[i].scaleFactor)),
			planets[i].albedo, planets[i].normal, planets[i].roughness, planets[i].metallic);
	}
	//drawStar(sphereContext, glm::mat4(1.0), texture::sun);
	drawObjectColor(station, glm::translate(glm::mat4(1.0), glm::vec3(13.0f, 0.f, 19.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(0.001f)) * glm::rotate(glm::mat4(1.0), 1.57f, glm::vec3(0, 0, 1)), glm::vec3(0.2), 0.3, 0.6);
	drawObjectTexture(asteroidContext_2, glm::rotate(glm::mat4(1.0), time * 0.3f, glm::vec3(0, 1, 0)) * glm::translate(glm::mat4(1.0), glm::vec3(13.0, 2., 0)) *
		glm::scale(glm::mat4(1.0), glm::vec3(0.0002f)),
		texture::asteroidBaseColor_2, texture::asteroidNormal_2, texture::asteroidRoughness_2, texture::asteroidMetallic_2);
	drawObjectTexture(asteroidContext_0, glm::rotate(glm::mat4(1.0), time * 0.000813f, glm::vec3(0, 1, 0)) * glm::translate(glm::mat4(1.0), glm::vec3(33.0, 0, 0)),
		texture::asteroidBaseColor_0, texture::empty, texture::empty, texture::empty);
	drawObjectTexture(asteroidContext_3, glm::rotate(glm::mat4(1.0), time * 0.0013f, glm::vec3(0, 1, 0)) * glm::translate(glm::mat4(1.0), glm::vec3(50.0, 0, 0)),
		texture::asteroidBaseColor_3, texture::asteroidNormal_3, texture::empty, texture::asteroidMetallic_3);
	drawObjectTexture(asteroidContext_2, glm::rotate(glm::mat4(1.0), time * 0.0017f, glm::vec3(0, 1, 0)) * glm::translate(glm::mat4(1.0), glm::vec3(57., -0.5, 0)) *
		glm::scale(glm::mat4(1.0), glm::vec3(0.001f)),
		texture::asteroidBaseColor_2, texture::asteroidNormal_2, texture::asteroidRoughness_2, texture::asteroidMetallic_2);
	drawObjectTexture(asteroidContext_2, glm::rotate(glm::mat4(1.0), time * 0.003f, glm::vec3(0, 1, 0)) * glm::translate(glm::mat4(1.0), glm::vec3(64., 0.5, 0)) *
		glm::scale(glm::mat4(1.0), glm::vec3(0.0014f)),
		texture::asteroidBaseColor_2, texture::asteroidNormal_2, texture::asteroidRoughness_2, texture::asteroidMetallic_2);
	drawStar(sun, glm::translate(glm::mat4(1.0), glm::vec3(0, 0, 0)) * glm::scale(glm::mat4(1.0), glm::vec3(0.3)), texture::sun);
	
	
	#pragma endregion
	#pragma region enemies
	for (int i = 0; i < enemies.size(); i++) {
		//enemy.move();
		if (enemies[i].IsAlive()) {
			enemyMove(enemies[i], i);
			//enemy.move(glm::vec3(4.5, 0.0, 4.7), glm::vec3(0.0));
			std::cout << "index: " << i << ", " << enemies[i].Position().x << "   " << enemies[i].Direction().x << std::endl;
			drawObjectTexture(enemyContexts[i],
				glm::translate(glm::mat4(1.0), enemies[i].Position()) * glm::eulerAngleY(glm::pi<float>()) * glm::scale(glm::mat4(1.0), glm::vec3(1.0)),
				texture::enemyTextures[i], texture::enemyNormals[i], texture::enemyRoughnesses[i], texture::enemyMetallics[i]
			);
			
		}
		drawLaser(enemies[i]);
	}
	#pragma endregion
	#pragma region spaceship

	glm::vec3 spaceshipSide = glm::normalize(glm::cross(player.Direction(), glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 spaceshipUp = glm::normalize(glm::cross(spaceshipSide, player.Direction()));
	glm::mat4 spaceshipCameraRotrationMatrix = glm::mat4({
		spaceshipSide.x,spaceshipSide.y,spaceshipSide.z,0,
		spaceshipUp.x,spaceshipUp.y,spaceshipUp.z ,0,
		-player.Direction().x,-player.Direction().y,-player.Direction().z,0,
		0.,0.,0.,1.,
		});


	drawObjectTexture(shipContext,
		glm::translate(glm::mat4(1.0), player.Position()) * spaceshipCameraRotrationMatrix * glm::eulerAngleY(glm::pi<float>()) * glm::scale(glm::mat4(1.0), glm::vec3(scaleModelIndex)),
		texture::spaceshipAlbedo, texture::spaceshipNormal, texture::spaceshipRoughness, texture::spaceshipMetallic
	);
	//glm::mat4 shipRotationMatrix = glm::lookAt(player.Position(), player.Position() + player.Direction(), glm::vec3(0.0f, 1.0f, 0.0f));

	//// Apply the rotation matrix to the model matrix
	//drawObjectTexture(shipContext,
	//	glm::translate(glm::mat4(1.0), player.Position()) * spaceshipCameraRotrationMatrix * glm::eulerAngleY(glm::pi<float>())
	//	* shipRotationMatrix
	//	* glm::scale(glm::mat4(1.0), glm::vec3(scaleModelIndex)),
	//	texture::spaceshipAlbedo, texture::spaceshipNormal, texture::spaceshipRoughness, texture::spaceshipMetallic
	//);
	drawLaser(player);
	spotlightPos = player.Position() + 0.5f * player.Direction();
	spotlightConeDir = player.Direction();

#pragma endregion
	#pragma region mission
	if (currentMission.IsActive()) {
		drawText(std::to_string(currentMission.TimeLeft()), glm::vec3(0.8f, 0.1f, 0.1f), 250.0f, 570.0f, 0.8f);
		
		drawDestinationPoint(currentMission.TranslationVector(), currentMission.RotationSpeed(), currentMission);
		drawObjectTexture(currentMission.CargoContext(),
			 glm::translate(glm::mat4(1.0), player.Position() + glm::vec3(0.0, 2.0, 0.0)) * spaceshipCameraRotrationMatrix * glm::scale(glm::mat4(1.0), glm::vec3(0.01)),
			currentMission.Albedo(), currentMission.Normal(), currentMission.Roughness(), currentMission.Metallic());
	}
	#pragma endregion
	#pragma region text
	drawText(std::to_string((int)round(player.BatteryLeft() / player.BatteryCapacity() * 100.f)), glm::vec3(0.5, 0.8f, 0.2f), 25.0f, 570.0f, 1.0f);
	if (isShipOnStation(player)) {
		drawText("Press E to recharge", glm::vec3(0.5, 0.8f, 0.2f), 25.0f, 520.0f, 0.5f);
	}
	if (isShipOnStation(player) && !currentMission.IsActive()) {
		drawText("Press F to start mission", glm::vec3(0.5, 0.8f, 0.2f), 25.0f, 470.0f, 0.5f);
	}
	if (currentMission.IsActive() && isShipNearDestination(player, currentMission)) {
		drawText("Press F to end mission", glm::vec3(0.5, 0.8f, 0.2f), 25.0f, 470.0f, 0.5f);
	}
	if (isShipOnStation(player))
	{
		drawText("Press M to change your spaceship", glm::vec3(0.5, 0.8f, 0.2f), 25.0f, 420.0f, 0.5f);

	}
	if(time - currentMission.MissionFailTime() < 3.f)
	{
		drawText("Mission failed", glm::vec3(0.8f, 0.1f, 0.1f), 250.0f, 250.0f, 1.0f);
	}
	if(time - missionSuccess < 3.f)
	{
		drawText("Mission success", glm::vec3(0.1f, 0.8f, 0.1f), 250.0f, 250.0f, 1.0f);
	}
	if(win)
	{
		drawText("You win", glm::vec3(0.1f, 0.8f, 0.1f), 250.0f, 200.0f, 1.0f);
	}

	#pragma endregion


	glUseProgram(0);
	glfwSwapBuffers(window);
}
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	aspectRatio = width / float(height);
	glViewport(0, 0, width, height);
}
void loadModelToContext(std::string path, Core::RenderContext& context)
{
	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
		return;
	}
	context.initFromAssimpMesh(scene->mMeshes[0]);
}
void init(GLFWwindow* window)
{
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	std::vector<SpaceshipModel> ufoModelShip = {
		SpaceshipModel{"./models/ufo/UFO.obj",
			"./models/ufo/textures/ufo_base.png",
			"./models/ufo/textures/ufo_normal.png",
			"./models/ufo/textures/ufo_metal.png",
			"./models/ufo/textures/ufo_roughness.png"}
	};
	enemies = {
		SpaceTraveler(20, ufoModelShip[0], 10, glm::vec3(-5.0f, 1.500000f, 2.124680f), glm::vec3(-0.354510f, 0.000000f, 0.935054f), glm::vec3(1.0)),
		SpaceTraveler(20, ufoModelShip[0], 10, glm::vec3(9.0f, 0.0f, -4.0f), glm::vec3(-0.354510f, 0.000000f, 0.935054f), glm::vec3(1.0)),
	};

	glEnable(GL_DEPTH_TEST);
	program = shaderLoader.CreateProgram("shaders/shader_color.vert", "shaders/shader_color.frag");
	programSun = shaderLoader.CreateProgram("shaders/shader_sun.vert", "shaders/shader_sun.frag");
	programSkybox = shaderLoader.CreateProgram("shaders/shader_skybox.vert", "shaders/shader_skybox.frag");
	programLaser = shaderLoader.CreateProgram("shaders/shader_laser.vert", "shaders/shader_laser.frag");
	programText = shaderLoader.CreateProgram("shaders/shader_text.vert", "shaders/shader_text.frag");

	glGenTextures(1, &texture::skybox);
	
	programTex = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");

	generateCharacters();
	
	//spaceshipModelList.fillList();
	//TODO LENA: replace last argument with the actual size
	//currentSpaceship = spaceshipModelList.getNextModel();
	loadModelToContext(player.getSpaceshipModel().mainModelPath, shipContext);
	texture::spaceshipAlbedo = Core::LoadTexture(spaceshipModels.getCurrentSpaceshipModel().textureBaseColorPath.data());
	texture::spaceshipNormal = Core::LoadTexture(spaceshipModels.getCurrentSpaceshipModel().textureNormalPath.data());
	texture::spaceshipMetallic = Core::LoadTexture(spaceshipModels.getCurrentSpaceshipModel().textureMetallicPath.data());
	texture::spaceshipRoughness = Core::LoadTexture(spaceshipModels.getCurrentSpaceshipModel().textureRoughnessPath.data());

	loadModelToContext("./models/sphere.obj", sphereContext);
	loadModelToContext("./models/cube.obj", cubeContext);
	loadModelToContext("./models/Station.obj", station);
	loadModelToContext("./models/planet.obj", planet);
	loadModelToContext("./models/asteroids/0/asteroid_0.obj", asteroidContext_0);
	loadModelToContext("./models/asteroids/2/asteroid_2.obj", asteroidContext_2);
	loadModelToContext("./models/asteroids/3/asteroid_3.obj", asteroidContext_3);
	loadModelToContext("./models/lazer_bullet.obj", bulletContext);
	loadModelToContext("./models/sun.obj", sun);
	for (int i = 0; i < enemies.size(); i++) {
		Core::RenderContext enemyContext;
		loadModelToContext(enemies[i].getSpaceshipModel().mainModelPath, enemyContext);
		enemyContexts.push_back(enemyContext);

		texture::enemyTextures.push_back(Core::LoadTexture(enemies[i].getSpaceshipModel().textureBaseColorPath.data()));
		texture::enemyNormals.push_back(Core::LoadTexture(enemies[i].getSpaceshipModel().textureNormalPath.data()));
		texture::enemyRoughnesses.push_back(Core::LoadTexture(enemies[i].getSpaceshipModel().textureRoughnessPath.data()));
		texture::enemyMetallics.push_back(Core::LoadTexture(enemies[i].getSpaceshipModel().textureMetallicPath.data()));
	}
	std::vector<std::string> skyboxPaths = {
			"textures/skybox-right.png",
			"textures/skybox-left.png",
			"textures/skybox-top.png",
			"textures/skybox-bottom.png",
			"textures/skybox-back.png",
			"textures/skybox-front.png"
	};
	texture::skybox = Core::LoadSkybox(skyboxPaths);

	#pragma region planets
	texture::sun = Core::LoadTexture("./textures/sun.jpg");
	texture::planetContinentBase = Core::LoadTexture("./textures/Planets/planet_continental_Base_Color.jpg");
	texture::planetContinentNormal = Core::LoadTexture("./textures/Planets/planet_continental_Normal_OpenGL.png");
	texture::planetContinentRoughness = Core::LoadTexture("./textures/Planets/planet_continental_Roughness.png");
	texture::planetContinentMetallic = Core::LoadTexture("./textures/Planets/planet_continental_Metallic.png");

	texture::planetBarrenBase = Core::LoadTexture("./textures/Planets/planet_barren_Base_Color.jpg");
	texture::planetBarrenNormal = Core::LoadTexture("./textures/Planets/planet_barren_Normal_OpenGL.png");
	texture::planetBarrenRoughness = Core::LoadTexture("./textures/Planets/planet_barren_Roughness.png");

	texture::planetFrozenBase = Core::LoadTexture("./textures/Planets/planet_frozen_Base_Color.jpg");
	texture::planetFrozenNormal = Core::LoadTexture("./textures/Planets/planet_frozen_Normal_OpenGL.png");
	texture::planetFrozenRoughness = Core::LoadTexture("./textures/Planets/planet_frozen_Roughness.png");

	texture::planetGasBase = Core::LoadTexture("./textures/Planets/planet_gas_Base_Color.jpg");
	texture::planetGasNormal = Core::LoadTexture("./textures/Planets/planet_gas_Normal_OpenGL.png");
	texture::planetGasClouds = Core::LoadTexture("./textures/Planets/planet_gas_cloud_01_Base_Color.jpg");
	texture::planetGasCloudsNormal = Core::LoadTexture("./textures/Planets/planet_gas_cloud_01_Normal_OpenGL.png");
	texture::planetGasCloudsOpacity = Core::LoadTexture("./textures/Planets/planet_gas_cloud_01_Opacity.png");

	texture::planetLavaBase = Core::LoadTexture("./textures/Planets/planet_lava_Base_Color.jpg");;
	texture::planetLavaNormal = Core::LoadTexture("./textures/Planets/planet_lava_Normal_OpenGL.png");
	texture::planetLavaRoughness= Core::LoadTexture("./textures/Planets/planet_lava_Roughness.png");;

	texture::planetSmacBase = Core::LoadTexture("./textures/Planets/planet_smac_Base_Color.png");;
	texture::planetSmacNormal = Core::LoadTexture("./textures/Planets/planet_smac_Normal_OpenGL.png");
	texture::planetSmacRoughness = Core::LoadTexture("./textures/Planets/planet_smac_Roughness.png");
	texture::planetSmacClouds = Core::LoadTexture("./textures/Planets/planet_smac_clouds_Base_Color.jpg");
	texture::planetSmacCloudsNormal = Core::LoadTexture("./textures/Planets/planet_smac_clouds_Normal_OpenGL.png");
	texture::planetSmacCloudsOpacity = Core::LoadTexture("./textures/Planets/planet_smac_clouds_Opacity.png");
	
	texture::asteroidBaseColor_0 = Core::LoadTexture("./models/asteroids/0/textures/Lunar_Target.png");
	texture::asteroidBaseColor_2 = Core::LoadTexture("./models/asteroids/2/textures/asteroid_low_1_BaseColor.png");
	texture::asteroidMetallic_2 = Core::LoadTexture("./models/asteroids/2/textures/asteroid_low_1_Metallic.png");
	texture::asteroidNormal_2 = Core::LoadTexture("./models/asteroids/2/textures/asteroid_low_1_Normal.png");
	texture::asteroidRoughness_2 = Core::LoadTexture("./models/asteroids/2/textures/asteroid_low_1_Roughness.png");
	
	texture::asteroidBaseColor_3 = Core::LoadTexture("./models/asteroids/3/textures/Displacement.png");
	texture::asteroidMetallic_3 = Core::LoadTexture("./models/asteroids/3/textures/Metalness.png");
	texture::asteroidNormal_3 = Core::LoadTexture("./models/asteroids/3/textures/Normal.png");

	planets = {
		Planet(glm::vec3(16.0f, 0.f, 10.0f), 2.5f, 0.02f, texture::planetContinentBase, texture::planetContinentNormal, texture::planetContinentRoughness, texture::planetContinentMetallic),
		Planet(glm::vec3(-7.0f, 0.f, 6.0f), 3.f, 0.035f, texture::planetBarrenBase, texture::planetBarrenNormal, texture::planetBarrenRoughness, texture::empty),
		Planet(glm::vec3(26.0f, 0.f, -15.0f), 3.f, 0.014f, texture::planetFrozenBase, texture::planetFrozenNormal, texture::planetFrozenRoughness, texture::empty),
		Planet(glm::vec3(-38.0f, 0.f, -3.0f), 3.f, 0.007f, texture::planetGasBase, texture::planetGasNormal, texture::empty, texture::empty),
		Planet(glm::vec3(43.0, 0.0, 0.0), 5.f, 0.009, texture::planetLavaBase, texture::planetLavaNormal, texture::planetLavaRoughness, texture::empty),
		Planet(glm::vec3(-53.0, 25.0, 0.0), 5.f, 0.022, texture::planetSmacBase, texture::planetSmacNormal, texture::planetSmacRoughness, texture::empty)
	};

	#pragma endregion

	missions = {
	Mission(30.f, planets[2], "textures/containers/cargocrate1_BaseColor.png", "textures/containers/cargocrate1_Normal.png", "textures/containers/cargocrate1_Roughness.png", "textures/containers/cargocrate2_Metallic.png"),
	Mission(30.f, planets[3], "textures/containers/cargocrate2_BaseColor.png", "textures/containers/cargocrate2_Normal.png", "textures/containers/cargocrate2_Roughness.png", "textures/containers/cargocrate2_Metallic.png"),
	Mission(30.f, planets[5], "textures/containers/cargocrate2_BaseColor.png", "textures/containers/cargocrate2_Normal.png", "textures/containers/cargocrate2_Roughness.png", "textures/containers/cargocrate2_Metallic.png"),
	};

	currentMission = missions[0];

	std::cout << "textures loaded" << std::endl;
	//texture::ao = Core::LoadTexture("./textures/water/rustediron1-alt2-bl/Pool_Water_Texture_ao.jpg");

#pragma region text
	glGenVertexArrays(1, &VAOtext);
	glGenBuffers(1, &VBOtext);
	glBindVertexArray(VAOtext);
	glBindBuffer(GL_ARRAY_BUFFER, VBOtext);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#pragma endregion

	
	
}

void shutdown(GLFWwindow* window)
{
	shaderLoader.DeleteProgram(program);
}


void processInput(GLFWwindow* window)
{
	glm::vec3 spaceshipSide = glm::normalize(glm::cross(player.Direction(), glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 spaceshipUp = glm::vec3(0.f, 1.f, 0.f);
	float angleSpeed = deltaTime * 0.44f;
	float moveSpeed = deltaTime * 4.;
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		player.forward(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		player.backward(glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS); 
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		player.turnLeft();
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		player.turnRight();
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		player.turnUp();
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		player.turnDown();
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && isShipOnStation(player))
		player.charge();
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && isShipOnStation(player) && !currentMission.IsActive()) {
		currentMission.StartMission();
	}
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && currentMission.IsActive() && isShipNearDestination(player, currentMission)) {
		currentMission.EndMission();
		missionSuccess = glfwGetTime();
		missions.erase(missions.begin());
		if (missions.size() > 0)
			currentMission = missions[0];
		else
			win = true;
	}

	cameraPos = player.Position() - 1.5f * player.Direction() + glm::vec3(0, 1, 0) * 0.5f;
	cameraDir = player.Direction();
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		exposition -= 0.05;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		exposition += 0.05;

	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS && isShipOnStation(player)) {
		
		spaceshipModels.setNextModel();

		if (spaceshipModels.getCurrentSpaceshipModel().mainModelPath.find("_0") != std::string::npos){
			scaleModelIndex = 0.1;
			translateModelVec = glm::vec3(0, 0, 0);
		}
		else if (spaceshipModels.getCurrentSpaceshipModel().mainModelPath.find("_1") != std::string::npos) {
			scaleModelIndex = 0.04;
			translateModelVec = glm::vec3(0, -0.11, 0);
		}
		else if (spaceshipModels.getCurrentSpaceshipModel().mainModelPath.find("_2") != std::string::npos) {
			scaleModelIndex = 0.07;
			translateModelVec = glm::vec3(0, -0.205, 0);
		}
		else if (spaceshipModels.getCurrentSpaceshipModel().mainModelPath.find("_3") != std::string::npos) {
			scaleModelIndex = 0.0063;
			translateModelVec = glm::vec3(0, -0.295, 0);
		}

		loadModelToContext(spaceshipModels.getCurrentSpaceshipModel().mainModelPath, shipContext);
		for (int i = 0; i < enemies.size(); i++) {
			
		}
		texture::spaceshipAlbedo = Core::LoadTexture(spaceshipModels.getCurrentSpaceshipModel().textureBaseColorPath.data());
		texture::spaceshipNormal = Core::LoadTexture(spaceshipModels.getCurrentSpaceshipModel().textureNormalPath.data());
		texture::spaceshipMetallic = Core::LoadTexture(spaceshipModels.getCurrentSpaceshipModel().textureMetallicPath.data());
		texture::spaceshipRoughness = Core::LoadTexture(spaceshipModels.getCurrentSpaceshipModel().textureRoughnessPath.data());
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		if(player.LastTimeShot() + player.Cooldown() < glfwGetTime())
		ShotTimer t1{ (size_t) (shotDuration / 2.0f), player, enemies };
	}

}

void setNextMission()
{
	missions.erase(missions.begin());
	currentMission = missions[0];
}


void renderLoop(GLFWwindow* window) {
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		renderScene(window);
		glfwPollEvents();
	}
}