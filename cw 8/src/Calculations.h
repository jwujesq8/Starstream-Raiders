#ifndef PLAYER_COLLISION_H
#define PLAYER_COLLISION_H

#include <glm.hpp>
#include <vector>

using namespace glm;

class Calculations {
public:
    static void calculatePlayerBoundingBox(vec3 playerPosition, vec3 playerSize, vec3 playerDirection, vec3& playerMin, vec3& playerMax);
    static bool rayCubeCollision(vec3 rayDirection, vec3 rayOrigin, vec3 lb, vec3 rt);
};

#endif // PLAYER_COLLISION_H
