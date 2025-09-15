#ifndef COLLISIONS_HXX
#define COLLISIONS_HXX

#include "/usr/include/glm/glm.hpp"
#include "/usr/include/glm/gtc/matrix_transform.hpp"
#include "/usr/include/glm/gtc/type_ptr.hpp"

// attempt at c portability

typedef struct _aabb
{
    glm::vec3 pos = glm::vec3(0.0), scale = glm::vec3(1.0);
} aabb;

bool colliding(aabb first, aabb second);
aabb makeAABB(glm::vec3 pos = glm::vec3(0.0), glm::vec3 scale = glm::vec3(1.0));

typedef struct _collision
{
    bool hit = false;
    glm::vec3 hitLocation = glm::vec3(0.0);
    glm::ivec3 normal = glm::ivec3(0);
} collision;

collision normal_collision(aabb *first, aabb *second, glm::vec3 vel_a, glm::vec3 vel_b);

void putAABB(aabb *box, glm::vec3 new_pos = glm::vec3(0.0));

#endif