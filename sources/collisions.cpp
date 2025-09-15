#include "../headers/collisions.hxx"

bool colliding(aabb first, aabb second)
{
    for (int i = 0; i < 3; ++i)
    {
        if (std::abs(first.pos[i] - second.pos[i]) > (first.scale[i] + second.scale[i]) * 0.5)
            return false;
    }

    return true;
}
aabb makeAABB(glm::vec3 pos, glm::vec3 scale)
{
    aabb temp;
    temp.pos = pos;
    temp.scale = scale;

    return temp;
}

collision normal_collision(aabb *first, aabb *second, glm::vec3 vel_a, glm::vec3 vel_b)
{
    collision c;
    c.hitLocation = first->pos;
    aabb boundingA = makeAABB(first->pos + vel_a * 0.5f, first->scale + glm::vec3(std::abs(vel_a.x), std::abs(vel_a.y), std::abs(vel_a.z)));
    aabb boundingB = makeAABB(second->pos + vel_b * 0.5f, second->scale + glm::vec3(std::abs(vel_b.x), std::abs(vel_b.y), std::abs(vel_b.z)));
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

void putAABB(aabb *box, glm::vec3 new_pos)
{
    box->pos = new_pos;
}