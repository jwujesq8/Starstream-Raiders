#include <iostream>
#include <glm.hpp>
#include <vector>

using namespace glm;

void calculatePlayerBoundingBox(vec3 playerPosition, vec3 playerSize, vec3 playerDirection, vec3 & playerMin, vec3 & playerMax) {
    // Calculate half extents of the player
    vec3 halfExtents = playerSize / 2.0f;

    // Calculate rotation matrix from player direction
    mat3 rotationMatrix = mat3(1.0f); // Identity matrix by default
    if (playerDirection != vec3(0.0f)) {
        vec3 forward = normalize(playerDirection);
        vec3 right = normalize(cross(vec3(0.0f, 1.0f, 0.0f), forward)); // Assume up is (0,1,0)
        vec3 up = cross(forward, right);
        rotationMatrix = mat3(right, up, forward);
    }

    // Calculate the vertices of the player's bounding box
    vec3 vertices[8] = {
        playerPosition + rotationMatrix * vec3(-halfExtents.x, -halfExtents.y, -halfExtents.z),
        playerPosition + rotationMatrix * vec3(halfExtents.x, -halfExtents.y, -halfExtents.z),
        playerPosition + rotationMatrix * vec3(-halfExtents.x, -halfExtents.y, halfExtents.z),
        playerPosition + rotationMatrix * vec3(halfExtents.x, -halfExtents.y, halfExtents.z),
        playerPosition + rotationMatrix * vec3(-halfExtents.x, halfExtents.y, -halfExtents.z),
        playerPosition + rotationMatrix * vec3(halfExtents.x, halfExtents.y, -halfExtents.z),
        playerPosition + rotationMatrix * vec3(-halfExtents.x, halfExtents.y, halfExtents.z),
        playerPosition + rotationMatrix * vec3(halfExtents.x, halfExtents.y, halfExtents.z)
    };

    // Calculate the boundaries of the player's bounding box
    playerMin = playerMax = vertices[0];
    for (int i = 1; i < 8; ++i) {
        playerMin = glm::min(playerMin, vertices[i]);
        playerMax = glm::max(playerMax, vertices[i]);
    }
}

bool rayCubeCollizion(vec3 rayDirection, vec3 rayOrigin, vec3 lb, vec3 rt)
{
    float t; //length of ray until intersection
    
    vec3 dirfrac = vec3(
    1.0f / rayDirection.x,
    1.0f / rayDirection.y,
    1.0f / rayDirection.z);
    // lb is the corner of AABB with minimal coordinates - left bottom, rt is maximal corner
    
    float t1 = (lb.x - rayOrigin.x) * dirfrac.x;
    float t2 = (rt.x - rayOrigin.x) * dirfrac.x;
    float t3 = (lb.y - rayOrigin.y) * dirfrac.y;
    float t4 = (rt.y - rayOrigin.y) * dirfrac.y;
    float t5 = (lb.z - rayOrigin.z) * dirfrac.z;
    float t6 = (rt.z - rayOrigin.z) * dirfrac.z;

    float tmin = max(max(min(t1, t2), min(t3, t4)), min(t5, t6));
    float tmax = min(min(max(t1, t2), max(t3, t4)), max(t5, t6));

    // if tmax < 0, ray (line) is intersecting AABB, but the whole AABB is behind us
    if (tmax < 0)
    {
        t = tmax;
        return false;
    }

    // if tmin > tmax, ray doesn't intersect AABB
    if (tmin > tmax)
    {
        t = tmax;
        return false;
    }

    t = tmin;
    return true;
}