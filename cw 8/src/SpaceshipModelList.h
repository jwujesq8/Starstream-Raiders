#pragma once

#include <string>
#include <list>
#include <iostream>

struct SpaceshipModel {
    std::string mainModelPath;
    std::string textureBaseColorPath;
    std::string textureNormalPath;
    std::string textureMetallicPath;
    std::string textureRoughnessPath;
};

class SpaceshipModelList {
private:
    std::list<SpaceshipModel> spaceshipModelList;
    std::list<SpaceshipModel>::iterator currentSpaceshipModel;

public:

    SpaceshipModelList() {
        fillList();
        currentSpaceshipModel = spaceshipModelList.begin();
    }

    void addModel(const std::string& mainModelPath, const std::string& textureBaseColorPath, const std::string& textureNormalPath,
        const std::string& textureMetallicPath, const std::string& textureRoughnessPath)
    {
        SpaceshipModel newModel;
        newModel.mainModelPath = mainModelPath;
        newModel.textureBaseColorPath = textureBaseColorPath;
        newModel.textureNormalPath = textureNormalPath;
        newModel.textureMetallicPath = textureMetallicPath;
        newModel.textureRoughnessPath = textureRoughnessPath;
        spaceshipModelList.push_back(newModel);
    };

    void fillList()
    {

        addModel("./models/spaceshipModels/0/spaceship_0.obj",
            "./models/spaceshipModels/0/textures/rustediron2_basecolor.png",
            "./models/spaceshipModels/0/textures/rustediron2_normal.png",
            "./models/spaceshipModels/0/textures/rustediron2_metallic.png",
            "./models/spaceshipModels/0/textures/rustediron2_roughness.png");

        addModel("./models/spaceshipModels/1/spaceship_1.obj",
            "./models/spaceshipModels/1/textures/DeathRow_Low_Cube001_[AlbedoM].png",
            "./models/spaceshipModels/1/textures/DeathRow_Low_Cube001_[Normal].png",
            "./models/spaceshipModels/1/textures/DeathRow_Low_Cube001_[Metalness].png",
            "./models/spaceshipModels/1/textures/DeathRow_Low_Cube001_[Roughness].png");

        addModel("./models/spaceshipModels/2/spaceship_2.obj",
            "./models/spaceshipModels/2/textures/Material.001_Base_color.png",
            "./models/spaceshipModels/2/textures/Material.001_Normal_DirectX.png",
            "./models/spaceshipModels/2/textures/Material.001_Metallic.png",
            "./models/spaceshipModels/2/textures/Material.001_Roughness.png");

        addModel("./models/spaceshipModels/3/spaceship_3.obj",
            "./models/spaceshipModels/3/textures/paint_texture_glow1.png",
            "./models/spaceshipModels/3/textures/paint_texture_normal.png",
            "./models/spaceshipModels/3/textures/paint_texture_diffuse.png",
            "./models/spaceshipModels/3/textures/Material.001_Roughness.png"
        );

    };
    
    SpaceshipModel getCurrentSpaceshipModel() {
        if (spaceshipModelList.empty()) {
            throw std::runtime_error("spaceshipModelList is empty");
        }

        return *currentSpaceshipModel;
    }
    /*
    SpaceshipModel getNextModel() {
        if (spaceshipModelList.empty()) {
            // If the list is empty, return a default item
            return { "", "", "", "" };
        }
        // If currentItem is pointing to the end, reset it to the beginning
        if (currentSpaceshipModel == spaceshipModelList.end()) {
            currentSpaceshipModel = spaceshipModelList.begin();
        }

        return *currentSpaceshipModel++;
    };*/
    void setNextModel() {
        if (spaceshipModelList.empty()) {
            // If the list is empty????
            throw "spaceshipModelList is empty";//{ "", "", "", "" };
        }
        // If currentItem is pointing to the end, reset it to the beginning
        if (currentSpaceshipModel == spaceshipModelList.end()) {
            currentSpaceshipModel = spaceshipModelList.begin();
        }

        *currentSpaceshipModel++;
    };

    void printModelsList() {
        for (const auto& item : spaceshipModelList) {
            std::cout << "Main Model Path: " << item.mainModelPath
                << ", Texture Base Color Path: " << item.textureBaseColorPath
                << ", Texture Normal Path: " << item.textureNormalPath
                << ", Texture Metallic Path: " << item.textureMetallicPath
                << ", Texture Roughness Path: " << item.textureRoughnessPath << std::endl;
        }
    };
};