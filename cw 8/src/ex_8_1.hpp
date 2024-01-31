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
#include "Space_Traveler.h"


namespace texture {
	GLuint earth;
	GLuint clouds;
	GLuint moon;
	GLuint ship;
	GLuint textureAlbedo; GLuint textureNormal; GLuint textureMetallic; GLuint textureRoughness; GLuint textureAO;

	GLuint skybox;


	GLuint empty;

	GLuint earthNormal;
	GLuint asteroidNormal;
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



}


GLuint program;
GLuint programSun;
GLuint programTex;
GLuint programSkybox;
Core::Shader_Loader shaderLoader;

Core::RenderContext shipContext;
Core::RenderContext sphereContext;
Core::RenderContext station;
Core::RenderContext planet;
Core::RenderContext sun;

Core::RenderContext cubeContext;

glm::vec3 cameraPos = glm::vec3(-4.f, 0, 0);
glm::vec3 cameraDir = glm::vec3(1.f, 0.f, 0.f);


//glm::vec3 spaceshipPos = glm::vec3(0.479490f, 1.000000f, -2.124680f);
//glm::vec3 spaceshipDir = glm::vec3(-0.354510f, 0.000000f, 0.935054f);

GLuint VAO,VBO;

float aspectRatio = 1.f;

float exposition = 1.f;

glm::vec3 lightPos = glm::vec3(0.f, 10.f, 0.f);
glm::vec3 lightColor = glm::vec3(0.9, 0.7, 0.8)*100.0f;

glm::vec3 spotlightPos = glm::vec3(0, 0, 0);
glm::vec3 spotlightConeDir = glm::vec3(0, 0, 0);
glm::vec3 spotlightColor = glm::vec3(0.5, 0.9, 0.8)*10.0f;
float spotlightPhi = 3.14 / 3;


SpaceshipModelList spaceshipModelList;

SpaceTraveler player(100, spaceshipModelList, 10, glm::vec3(0.479490f, 1.000000f, -2.124680f), glm::vec3(-0.354510f, 0.000000f, 0.935054f), glm::vec3(1.0));
std::vector<SpaceTraveler> enemies;

float scaleModelIndex = 0.1;


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
	float f = 2000.;
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



	Core::SetActiveTexture(texture::textureAlbedo, "textureAlbedo", program, 0);
	Core::SetActiveTexture(texture::textureNormal, "textureNormal", program, 1);
	Core::SetActiveTexture(texture::textureMetallic, "textureMetallic", program, 2);
	Core::SetActiveTexture(texture::textureRoughness, "textureRoughness", program, 3);
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
	glUniformMatrix4fv(glGetUniformLocation(programTex, "transformation"), 1, GL_FALSE, (float*)&transformation);

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

void renderScene(GLFWwindow* window)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 transformation;
	float time = glfwGetTime();

	drawSkybox(glm::translate(glm::mat4(1.0), player.Position()));


	
	glUseProgram(program);

	#pragma region planets
	drawStar(sphereContext, glm::mat4(1.0), texture::sun);
	drawObjectColor(station, glm::translate(glm::mat4(1.0), glm::vec3(15.0f, 0.f, 19.0f)) * glm::scale(glm::mat4(1.0), glm::vec3(0.001f)), glm::vec3(0.2), 0.3, 0.6);
	drawObjectTexture(planet, glm::rotate(glm::mat4(1.0), time * 0.15f, glm::vec3(0, 1, 0)) * glm::translate(glm::mat4(1.0), glm::vec3(16.0, 0, 0)),
		texture::planetContinentBase, texture::planetContinentNormal, texture::planetContinentRoughness, texture::planetContinentMetallic);
	drawObjectTexture(planet, glm::rotate(glm::mat4(1.0), time * 0.35f, glm::vec3(0, 1, 0)) * glm::translate(glm::mat4(1.0), glm::vec3(7.0, 0, 0)),
		texture::planetBarrenBase, texture::planetBarrenNormal, texture::planetBarrenRoughness, texture::empty);
	drawObjectTexture(planet, glm::rotate(glm::mat4(1.0), time * 0.07f, glm::vec3(0, 1, 0)) * glm::translate(glm::mat4(1.0), glm::vec3(26.0, 0, 0)),
		texture::planetFrozenBase, texture::planetFrozenNormal, texture::planetFrozenRoughness, texture::empty);
	drawObjectTexture(planet, glm::rotate(glm::mat4(1.0), time * 0.045f, glm::vec3(0, 1, 0)) * glm::translate(glm::mat4(1.0), glm::vec3(38.0, 0, 0)),
		texture::planetGasBase, texture::planetGasNormal, texture::empty, texture::empty);
	drawObjectTexture(planet, glm::rotate(glm::mat4(1.0), time * 0.030f, glm::vec3(0, 1, 0)) * glm::translate(glm::mat4(1.0), glm::vec3(43.0, 0, 0)),
		texture::planetLavaBase, texture::planetLavaNormal, texture::planetLavaRoughness, texture::empty);
	drawObjectTexture(planet, glm::rotate(glm::mat4(1.0), time * 0.022f, glm::vec3(0, 1, 0)) * glm::translate(glm::mat4(1.0), glm::vec3(50.0, 0, 0)),
		texture::planetSmacBase, texture::planetSmacNormal, texture::planetSmacRoughness, texture::empty);
	//drawStar(sun, glm::mat4(1.0), texture::sun);
	


	//drawObjectColor(sphereContext, glm::translate(glm::mat4(1.0), glm::vec3(2.0)), glm::vec3(0.5), 0.5, 0.5);
#pragma endregion

	#pragma region spaceship

	glm::vec3 spaceshipSide = glm::normalize(glm::cross(player.Direction(), glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 spaceshipUp = glm::normalize(glm::cross(spaceshipSide, player.Direction()));
	glm::mat4 specshipCameraRotrationMatrix = glm::mat4({
		spaceshipSide.x,spaceshipSide.y,spaceshipSide.z,0,
		spaceshipUp.x,spaceshipUp.y,spaceshipUp.z ,0,
		-player.Direction().x,-player.Direction().y,-player.Direction().z,0,
		0.,0.,0.,1.,
		});


	drawObjectColor(shipContext,
		glm::translate(glm::mat4(1.0), player.Position()) * specshipCameraRotrationMatrix * glm::eulerAngleY(glm::pi<float>()) * glm::scale(glm::mat4(1.0), glm::vec3(scaleModelIndex)),
		glm::vec3(0.3, 0.3, 0.5), 0.2, 0.8
	);

	spotlightPos = player.Position() + 0.5f * player.Direction();
	spotlightConeDir = player.Direction();

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

	glEnable(GL_DEPTH_TEST);
	program = shaderLoader.CreateProgram("shaders/shader_color.vert", "shaders/shader_color.frag");
	programSun = shaderLoader.CreateProgram("shaders/shader_sun.vert", "shaders/shader_sun.frag");
	programSkybox = shaderLoader.CreateProgram("shaders/shader_skybox.vert", "shaders/shader_skybox.frag");

	glGenTextures(1, &texture::skybox);
	
	programTex = shaderLoader.CreateProgram("shaders/shader_tex.vert", "shaders/shader_tex.frag");


	
	//spaceshipModelList.fillList();
	//TODO LENA: replace last argument with the actual size
	//currentSpaceship = spaceshipModelList.getNextModel();
	loadModelToContext(player.getSpaceshipModel().mainModelPath, shipContext);
	texture::textureAlbedo = Core::LoadTexture(spaceshipModelList.getCurrentSpaceshipModel().textureBaseColorPath.data());
	texture::textureNormal = Core::LoadTexture(spaceshipModelList.getCurrentSpaceshipModel().textureNormalPath.data());
	texture::textureMetallic = Core::LoadTexture(spaceshipModelList.getCurrentSpaceshipModel().textureMetallicPath.data());
	texture::textureRoughness = Core::LoadTexture(spaceshipModelList.getCurrentSpaceshipModel().textureRoughnessPath.data());

	loadModelToContext("./models/sphere.obj", sphereContext);
	loadModelToContext("./models/cube.obj", cubeContext);
	loadModelToContext("./models/Ring.obj", station);
	loadModelToContext("./models/planet.obj", planet);
	std::vector<std::string> skyboxPaths = {
			"textures/skybox-right.jpg",
			"textures/skybox-left.jpg",
			"textures/skybox-top.jpg",
			"textures/skybox-bottom.jpg",
			"textures/skybox-back.jpg",
			"textures/skybox-front.jpg"
	};
	texture::skybox = Core::LoadSkybox(skyboxPaths);

	texture::textureAlbedo = Core::LoadTexture("./textures/rustediron2_basecolor.png");
	texture::textureNormal = Core::LoadTexture("./textures/rustediron2_normal.png");
	texture::textureMetallic = Core::LoadTexture("./textures/rustediron2_metallic.png");
	texture::textureRoughness = Core::LoadTexture("./textures/rustediron2_roughness.png");

	texture::sun = Core::LoadTexture("./textures/Planets/sun.jpg");
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
	std::cout << "textures loaded" << std::endl;
	//texture::ao = Core::LoadTexture("./textures/water/rustediron1-alt2-bl/Pool_Water_Texture_ao.jpg");


}

void shutdown(GLFWwindow* window)
{
	shaderLoader.DeleteProgram(program);
}



//obsluga wejscia
void processInput(GLFWwindow* window)
{
	glm::vec3 spaceshipSide = glm::normalize(glm::cross(player.Direction(), glm::vec3(0.f, 1.f, 0.f)));
	glm::vec3 spaceshipUp = glm::vec3(0.f, 1.f, 0.f);
	
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		player.forward();//spaceshipPos += player.Direction() * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		player.backward(); //spaceshipPos -= player.Direction() * moveSpeed;
	//if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
	//	player.glideRight();//spaceshipPos += spaceshipSide * moveSpeed;
	//if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
	//	player.glideLeft();//spaceshipPos -= spaceshipSide * moveSpeed;
	//if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	//	player.up();//spaceshipPos += spaceshipUp * moveSpeed;
	//if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
	//	player.down();//spaceshipPos -= spaceshipUp * moveSpeed;
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
		player.turnLeft();//player.Direction() = glm::vec3(glm::eulerAngleY(angleSpeed) * glm::vec4(player.Direction(), 0));
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
		player.turnRight();//player.Direction() = glm::vec3(glm::eulerAngleY(-angleSpeed) * glm::vec4(player.Direction(), 0));
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		player.turnUp();//player.Direction() = glm::vec3(glm::eulerAngleX(angleSpeed) * glm::vec4(player.Direction(), 0));
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		player.turnDown();//player.Direction() = glm::vec3(glm::eulerAngleX(-angleSpeed) * glm::vec4(player.Direction(), 0));

	cameraPos = player.Position() - 1.5f * player.Direction() + glm::vec3(0, 1, 0) * 0.5f;
	cameraDir = player.Direction();
	if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS)
		exposition -= 0.05;
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS)
		exposition += 0.05;

	//change the spaceships model (it's available only on the station but this bound will be added LATER !!)
	if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS){
		spaceshipModelList.setNextModel();

		//change scaleModelIndex
		if (spaceshipModelList.getCurrentSpaceshipModel().mainModelPath.find("_0") != std::string::npos){
			scaleModelIndex = 0.1;
		}
		else if (spaceshipModelList.getCurrentSpaceshipModel().mainModelPath.find("_1") != std::string::npos) {
			scaleModelIndex = 0.05;
		}
		else if (spaceshipModelList.getCurrentSpaceshipModel().mainModelPath.find("_2") != std::string::npos) {
			scaleModelIndex = 0.08;
		}
		else if (spaceshipModelList.getCurrentSpaceshipModel().mainModelPath.find("_3") != std::string::npos) {
			scaleModelIndex = 0.008;
		}

		loadModelToContext(spaceshipModelList.getCurrentSpaceshipModel().mainModelPath, shipContext);
		texture::textureAlbedo = Core::LoadTexture(spaceshipModelList.getCurrentSpaceshipModel().textureBaseColorPath.data());
		texture::textureNormal = Core::LoadTexture(spaceshipModelList.getCurrentSpaceshipModel().textureNormalPath.data());
		texture::textureMetallic = Core::LoadTexture(spaceshipModelList.getCurrentSpaceshipModel().textureMetallicPath.data());
		texture::textureRoughness = Core::LoadTexture(spaceshipModelList.getCurrentSpaceshipModel().textureRoughnessPath.data());
	}

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
		player.shoot(enemies);
	}

	//cameraDir = glm::normalize(-cameraPos);

}

// funkcja jest glowna petla
void renderLoop(GLFWwindow* window) {
	while (!glfwWindowShouldClose(window))
	{
		processInput(window);

		renderScene(window);
		glfwPollEvents();
	}
}
//}