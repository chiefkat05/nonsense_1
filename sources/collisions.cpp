#include "../headers/collisions.hxx"
#include "../headers/graphics_backend.hxx"
#include <iostream>

bool colliding(aabb first, aabb second)
{
    for (int i = 0; i < 3; ++i)
    {
        if (std::abs(first.pos[i] - second.pos[i]) > (first.scale[i] + second.scale[i]) * 0.5)
            return false;
    }

    return true;
}
bool colliding(aabb2d box, glm::vec2 point)
{
    if (std::abs(box.pos.x - point.x) > box.scale.x * 0.5 || std::abs(box.pos.y - point.y) > box.scale.y * 0.5)
        return false;

    return true;
}
aabb makeAABB(glm::vec3 pos, glm::vec3 scale)
{
    aabb temp;
    temp.pos = pos;
    temp.scale = scale;

    return temp;
}

// void deleteAABBOctree(aabb_octree *octree)
// {
//     if (octree == nullptr)
//         return;

//     for (int i = 0; i < 8; ++i)
//     {
//         deleteAABBOctree(octree->pChild[i]);
//     }

//     delete octree;
// }

// int insertDepth = 0;
// void insertObjectIntoAABBOctree(aabb_octree *octree, aabb *obj)
// {
//     int index = 0;
//     bool straddle = false;

//     for (int i = 0; i < 3; ++i)
//     {
//         double delta = obj->pos[i] - octree->center[i];
//         if (std::abs(delta) <= obj->scale[i])
//         {
//             straddle = true;
//             break;
//         }
//         if (delta > 0.0)
//             index |= (1 << i);
//     }
//     if (!straddle)
//     {
//         if (octree->pChild[index] == nullptr)
//         {
//             // octree->pChild[index] = new aabb_octree;
//             // double step = octree->halfwidth * 0.5;
//             // glm::vec3 offset = glm::vec3(0.0);
//             // offset.x = ((index & 1) ? step : -step);
//             // offset.y = ((index & 2) ? step : -step);
//             // offset.z = ((index & 4) ? step : -step);
//             // octree->pChild[index]->center = octree->center + offset;
//             // octree->pChild[index]->halfwidth = step;
//         }
//         // insertObjectIntoAABBOctree(octree->pChild[index], obj);
//         // return;
//     }
//     obj->nextAABB = octree->pObjectList;
//     octree->pObjectList = obj;
// }

// typedef<typename T> thing here for the parent_object handling
// void AABBOctreeCollisionHandling(aabb_octree *octree)
// {
//     std::cout << (octree != nullptr) << " depth start\n";
//     const int depth_limit = 8;
//     static aabb_octree *ancestorStack[depth_limit]; // objective is to get it to not crash lol
//     static int depth = 0;

//     ancestorStack[depth++] = octree;

//     std::cout << depth << ", " << (octree != nullptr) << " huh\n";
//     // if (depth >= depth_limit || octree == nullptr)
//     // {
//     //     --depth;
//     //     return;
//     // }

//     for (int i = 0; i < depth; ++i)
//     {
//         aabb *objA, *objB;
//         for (objA = ancestorStack[i]->pObjectList; objA; objA = objA->nextAABB)
//         {
//             for (objB = octree->pObjectList; objB; objB = objB->nextAABB)
//             {
//                 if (objA == objB)
//                 {
//                     continue; // make this break; if you make responses correctly act for both sides
//                 }
//                 collision c = normal_collision(objA, objB, glm::vec3(0.0), glm::vec3(0.0)); // if it's more interesting then you could start over and design your own in system I think take *model_primitives instead of colliders
//                 if (c.hit && objA->parent_object != nullptr)
//                 {
//                     std::cout << "before\n"; // idk figure out plan for how to handle thing
//                     // static_cast<model_primitive *>(objA->parent_object)->Put(c.hitLocation); // yes?
//                     // std::cout << static_cast<model_primitive *>(objA->parent_object)->getImage() << " strange\n";
//                     std::cout << c.normal.x << ", " << c.normal.y << ", " << c.normal.z << " whoa\n";
//                 }
//             }
//         }
//     }
//     for (int i = 0; i < 8; ++i)
//     {
//         // std::cout << i << " n " << (octree != nullptr) << " midpoint\n";
//         if (octree->pChild[i] != nullptr)
//             AABBOctreeCollisionHandling(octree->pChild[i]);
//         // std::cout << "???\n";
//     }

//     // std::cout << "end loop\n";
//     --depth;
// }

collision normal_collision(aabb *first, aabb *second, glm::vec3 vel_a, glm::vec3 vel_b)
{
    collision c;
    c.hitLocation = first->pos;
    if (!colliding(*first, *second))
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

bool colliding(glm::vec3 point, aabb box)
{
    for (int i = 0; i < 3; ++i)
    {
        if (point[i] < box.pos[i] - (box.scale[i] * 0.5f) || point[i] > box.pos[i] + (box.scale[i] * 0.5f))
            return false;
    }
    return true;
}
bool colliding(raycast ray, aabb box, glm::vec3 &point)
{
    double tmin = 0.0;
    double tmax = std::numeric_limits<double>::infinity();

    glm::vec3 box_min = box.pos - box.scale * 0.5f;
    glm::vec3 box_max = box.pos + box.scale * 0.5f;
    for (int i = 0; i < 3; ++i)
    {
        if (std::abs(ray.dir[i]) == 0.0)
        {
            if (!colliding(ray.pos, box))
                return false;

            return true;
        }

        double t1 = (box_min[i] - ray.pos[i]) / ray.dir[i];
        double t2 = (box_max[i] - ray.pos[i]) / ray.dir[i];
        // double t1 = box_min[i] / ray.dir[i]; // 0,0,0 position
        // double t2 = box_max[i] / ray.dir[i];

        if (t1 > t2)
        {
            double ttemp = t2;
            t2 = t1;
            t1 = ttemp;
        }

        tmin = std::max(tmin, t1);
        tmax = std::min(tmax, t2);

        if (tmin > tmax)
            return false;
    }

    point = ray.pos + ray.dir * static_cast<float>(tmin);
    // point = ray.pos + ray.dir;

    return true;
}
void putAABB(aabb *box, glm::vec3 new_pos)
{
    box->pos = new_pos;
}