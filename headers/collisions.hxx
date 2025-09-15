#ifndef COLLISIONS_H
#define COLLISIONS_H

#include "/usr/include/glm/glm.hpp"
#include "/usr/include/glm/gtc/matrix_transform.hpp"
#include "/usr/include/glm/gtc/type_ptr.hpp"

// attempt at c portability

enum collision_id_type
{
    COL_ID_NONE,
    COL_ID_OBJECT,
    COL_ID_GROUND,
    COL_ID_PLAYER,
    COL_ID_DANGER,
};
typedef struct _aabb
{
    collision_id_type id = COL_ID_NONE;
    glm::vec3 pos, scale;
} aabb;

bool colliding(aabb first, aabb second)
{
    for (int i = 0; i < 3; ++i)
    {
        if (std::abs(first.pos[i] - second.pos[i]) > (first.scale[i] + second.scale[i]) * 0.5)
            return false;
    }

    return true;
}
aabb makeAABB(glm::vec3 pos = glm::vec3(0.0), glm::vec3 scale = glm::vec3(1.0), collision_id_type id = COL_ID_OBJECT)
{
    aabb temp;
    temp.pos = pos;
    temp.scale = scale;

    return temp;
}

typedef struct _collision
{
    bool hit = false;
    glm::vec3 hitLocation = glm::vec3(0.0);
    glm::ivec3 normal = glm::ivec3(0);
} collision;

collision normal_collision(aabb *first, aabb *second, glm::vec3 vel_a, glm::vec3 vel_b)
{

    // aabb testBoxExpanded = aabb(test.min_x - (max_x - xPos),
    //                             test.min_y - (max_y - yPos),
    //                             test.max_x + (xPos - min_x),
    //                             test.max_y + (yPos - min_y));

    // double maxDistance = -std::numeric_limits<double>::infinity();
    // if (maxDistance <= testBoxExpanded.min_x - xPos)
    // {
    //     maxDistance = testBoxExpanded.min_x - xPos;
    //     xNormal = -1;
    //     yNormal = 0;
    // }
    // if (maxDistance <= xPos - testBoxExpanded.max_x)
    // {
    //     maxDistance = xPos - testBoxExpanded.max_x;
    //     xNormal = 1;
    //     yNormal = 0;
    // }
    // if (maxDistance <= testBoxExpanded.min_y - yPos)
    // {
    //     maxDistance = testBoxExpanded.min_y - yPos;
    //     xNormal = 0;
    //     yNormal = -1;
    // }
    // if (maxDistance <= yPos - testBoxExpanded.max_y)
    // {
    //     maxDistance = yPos - testBoxExpanded.max_y;
    //     xNormal = 0;
    //     yNormal = 1;
    // }
    // distanceToSide = maxDistance;

    // if (xNormal == -1 && xV > 0.0)
    // {
    //     return testBoxExpanded.min_x;
    // }
    // if (xNormal == 1 && xV < 0.0)
    // {
    //     return testBoxExpanded.max_x;
    // }
    // if (yNormal == -1 && yV > 0.0)
    // {
    //     return testBoxExpanded.min_y;
    // }
    // if (yNormal == 1 && yV < 0.0)
    // {
    //     return testBoxExpanded.max_y;
    // }

    // if (xNormal != 0.0)
    //     return xPos;
    // if (yNormal != 0.0)
    //     return yPos;

    // insideCollision = false;
    // // here to avoid compiler complaints
    // return 0.0;
    collision c;
    c.hitLocation = first->pos;
    aabb boundingA = makeAABB(first->pos + vel_a * 0.5f, first->scale + glm::vec3(std::abs(vel_a.x), std::abs(vel_a.y), std::abs(vel_a.z)), COL_ID_NONE);
    aabb boundingB = makeAABB(second->pos + vel_b * 0.5f, second->scale + glm::vec3(std::abs(vel_b.x), std::abs(vel_b.y), std::abs(vel_b.z)), COL_ID_NONE);
    if (!colliding(boundingA, boundingB))
    {
        c.hit = false;
        return c;
    }
    c.hit = true;

    aabb expandedBox = makeAABB(second->pos, second->scale + first->scale);

    double maxDistance = -std::numeric_limits<double>::infinity();
    for (int i = 0; i < 3; ++i)
    {
        if (maxDistance <= (expandedBox.pos[i] - expandedBox.scale[i] * 0.5) - first->pos[i])
        {
            maxDistance = (expandedBox.pos[i] - expandedBox.scale[i] * 0.5) - first->pos[i];
            c.normal = glm::ivec3(0);
            c.normal[i] = -1;
        }
        if (maxDistance <= first->pos[i] - (expandedBox.pos[i] + expandedBox.scale[i] * 0.5))
        {
            maxDistance = first->pos[i] - (expandedBox.pos[i] + expandedBox.scale[i] * 0.5);
            c.normal = glm::ivec3(0);
            c.normal[i] = 1;
        }
    }

    for (int i = 0; i < 3; ++i)
    {
        if (c.normal[i] != 0)
        {
            c.hitLocation[i] = expandedBox.pos[i] + (expandedBox.scale[i] * 0.5 * c.normal[i]);
        }
    }
    if (c.normal == glm::ivec3(0))
    {
        c.hit = false;
    }
    return c;
}

void putCollision(aabb *box, glm::vec3 new_pos = glm::vec3(0.0))
{
    box->pos = new_pos;
}

#endif