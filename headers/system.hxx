#ifndef SYSTEM_HXX
#define SYSTEM_HXX

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "graphics_backend.hxx"
#include "collisions.hxx"

const unsigned int level_cap = 40;

enum object_type
{
    OBJ_SOLID,
    OBJ_PASSTHROUGH,
    OBJ_DANGER,
    OBJ_GATE,
    OBJ_EFFECT,
    OBJ_NONE
};
struct level_object
{
    cube visual;
    aabb collider;
    object_type type;
};
class level
{
private:
    std::vector<level_object> objects;
    unsigned int object_count = 0;

    double gravity = -9.81;

public:
    void addObject(glm::vec3 pos, glm::vec3 scale, unsigned int texture, object_type type)
    {
        cube c;
        c.Put(pos);
        c.Scale(scale);
        c.Image(texture);
        aabb col = makeAABB(pos, scale);
        objects.push_back({c, col, type});
        ++object_count;
    }
    void placeObject(glm::vec3 pos, unsigned int index)
    {
        if (index > object_count)
        {
            std::cout << "That object doesn't or shouldn't exist, there aren't that many in the scene!\n";
            return;
        }

        objects[index].visual.Put(pos);
        putAABB(&objects[index].collider, pos);
    }
    void moveObject(glm::vec3 distance, unsigned int index)
    {
        if (index > object_count)
        {
            std::cout << "That object doesn't or shouldn't exist, there aren't that many in the scene!\n";
            return;
        }

        objects[index].visual.Put(objects[index].visual.getPos() + distance);
        putAABB(&objects[index].collider, objects[index].collider.pos + distance);
    }

    void scaleObject(glm::vec3 scale, unsigned int index)
    {
        if (index > object_count)
        {
            std::cout << "That object doesn't or shouldn't exist, there aren't that many in the scene!\n";
            return;
        }

        objects[index].visual.Scale(scale);
        objects[index].collider.scale = scale; // lol
    }

    void drawLevel(shader &shad) // please add multi-shader support
    {
        for (int i = 0; i < object_count; ++i)
        {
            objects[i].visual.draw(shad);
        }
    }
    // add tick system?
    void updatePhysics(double delta_time, glm::vec3 &player_position, glm::vec3 &player_velocity, aabb &player_collider, bool &on_floor)
    {
        if (object_count == 0)
        {
            std::cout << "no object levels.\n";
            return;
        }
        if (player_position.y < -25.0) // needs to be taken out in favor of a real looping mechanic??
        {
            player_position = glm::vec3(0.0, 25.0, -3.0);
        }

        on_floor = false;

        putAABB(&player_collider, player_position);
        for (int i = 0; i < objects.size(); ++i)
        {
            objects[i].collider.pos = objects[i].visual.getPos(); // inefficient...
            objects[i].collider.scale = objects[i].visual.getScale();
            collision col = normal_collision(&player_collider, &objects[i].collider, player_velocity * static_cast<float>(delta_time), glm::vec3(0.0));
            if (col.hit)
            {
                player_position = col.hitLocation;
                putAABB(&player_collider, player_position);
                for (int j = 0; j < 3; ++j)
                {
                    if (j == 1 && col.normal[j] > 0)
                    {
                        // if (player_velocity.y < 0.0)
                        // {
                        //     ma_sound_start(&landsfx);
                        // }
                        on_floor = true;
                    }
                    if (col.normal[j] < 0 && player_velocity[j] > 0.0)
                    {
                        player_velocity[j] = 0.0;
                    }
                    if (col.normal[j] > 0 && player_velocity[j] < 0.0)
                    {
                        player_velocity[j] = 0.0;
                    }
                }
            }
        }
        if (!on_floor)
            player_velocity.y += gravity * delta_time;

        player_position.y += player_velocity.y * delta_time;
        player_position.x += player_velocity.x * delta_time;
        player_position.z += player_velocity.z * delta_time;
        player_velocity.x = 0.0;
        player_velocity.z = 0.0;
    }
};

enum update_command_types
{
    LVLUPD_MOVEOBJ,
    LVLUPD_SCALEOBJ
};
class game
{
private:
    unsigned int level_id = 0;
    std::vector<level> levels;
    unsigned int level_count = 0;

public:
    void goto_level(unsigned int id)
    {
        level_id = id;
    }
    void setup_level(const char *level_path)
    {
        if (level_count >= level_cap)
        {
            std::cout << "Level count is over level cap! Please increase the level limit or delete a different level to make room.\n";
            return;
        }
        level new_level;

        std::ifstream level_file(level_path);
        if (!level_file.is_open())
        {
            std::cout << "File " << level_path << " unable to be opened." << std::endl;
            return;
        }

        glm::vec3 new_position = glm::vec3(0.0), new_scale = glm::vec3(1.0);
        object_type new_type = OBJ_SOLID;
        unsigned int new_texture = 0;

        bool makingcube = false;
        int step = 0;
        std::string word, line;
        while (std::getline(level_file, line))
        {
            std::stringstream ss(line);
            while (std::getline(ss, word, ' '))
            {
                if (word == "cube")
                {
                    makingcube = true;
                    continue;
                }
                if (!makingcube)
                    continue;

                switch (step)
                {
                case 0:
                    new_position.x = std::stod(word);
                    break;
                case 1:
                    new_position.y = std::stod(word);
                    break;
                case 2:
                    new_position.z = std::stod(word);
                    break;
                case 3:
                    new_scale.x = std::stod(word);
                    break;
                case 4:
                    new_scale.y = std::stod(word);
                    break;
                case 5:
                    new_scale.z = std::stod(word);
                    break;
                case 6:
                    new_texture = std::stoi(word);
                    break;
                case 7:
                    new_type = static_cast<object_type>(std::stoi(word));

                    // make object and add to level here
                    new_level.addObject(new_position, new_scale, new_texture, new_type);
                    makingcube = false;
                    step = -1;
                    break;
                }

                ++step;
            }
        }
        levels.push_back(new_level);
        ++level_count;
    }

    void update(shader &shad, double delta_time, glm::vec3 &plPos, glm::vec3 &plVel, aabb &plCol, bool &onG)
    {
        if (level_count <= level_id)
        {
            std::cout << "level at index " << level_id << " does not exist.\n";
            return;
        }
        levels[level_id].updatePhysics(delta_time, plPos, plVel, plCol, onG);

        levels[level_id].drawLevel(shad);
    }
};

#endif