#include "../headers/system.hxx"

extern float pixel_scale;

void game::goto_level(unsigned int id)
{
    level_id = id;
}
void game::setup_level(const char *level_path)
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

    level_command_types making = LCOMM_NONE;
    model_primitive_type model_type = MODEL_NONE;
    int step = 0;
    std::string word, line;
    double pixel_division = 1.0;
    while (std::getline(level_file, line))
    {
        std::stringstream ss(line);
        while (std::getline(ss, word, ' '))
        {
            if (making == LCOMM_NONE)
            {
                if (word == "cube" || word == "box")
                {
                    making = LCOMM_PRIMITIVE;
                    pixel_division = 1.0;
                    model_type = MODEL_CUBE;
                    continue;
                }
                if (word == "pcube" || word == "pbox" || word == "pixelcube" || word == "pixelbox")
                {
                    making = LCOMM_PRIMITIVE_PIXELPOS;
                    pixel_division = pixel_scale;
                    model_type = MODEL_CUBE;
                    continue;
                }
                if (word == "quad" || word == "plane")
                {
                    making = LCOMM_PRIMITIVE;
                    pixel_division = 1.0;
                    model_type = MODEL_QUAD;
                    continue;
                }
                if (word == "pquad" || word == "pplane" || word == "pixelquad" || word == "pixelplane")
                {
                    making = LCOMM_PRIMITIVE;
                    pixel_division = pixel_scale;
                    model_type = MODEL_QUAD;
                    continue;
                }
                if (word == "pyramid")
                {
                    making = LCOMM_PRIMITIVE;
                    pixel_division = 1.0;
                    model_type = MODEL_PYRAMID;
                    continue;
                }
                if (word == "ppyramid" || word == "pixelpyramid")
                {
                    making = LCOMM_PRIMITIVE;
                    pixel_division = pixel_scale;
                    model_type = MODEL_PYRAMID;
                    continue;
                }
                if (word == "tri" || word == "triangle")
                {
                    making = LCOMM_PRIMITIVE;
                    pixel_division = 1.0;
                    model_type = MODEL_TRI;
                    continue;
                }
                if (word == "ptri" || word == "ptriangle" || word == "pixeltri" || word == "pixeltriangle")
                {
                    making = LCOMM_PRIMITIVE;
                    pixel_division = pixel_scale;
                    model_type = MODEL_TRI;
                    continue;
                }
                if (word == "trigger")
                {
                    making = LCOMM_TRIGGER;
                    continue;
                }
            }
            if (making == LCOMM_TRIGGER) // after finishing this put it below LCOMM_PRIMITIVE creation for organization
            {
                // insert data here

                switch (step)
                {
                }
            }

            if (making == LCOMM_PRIMITIVE || making == LCOMM_PRIMITIVE_PIXELPOS)
            {
                glm::vec3 new_position = glm::vec3(0.0), new_scale = glm::vec3(1.0);
                object_type new_type = OBJ_SOLID;
                unsigned int new_texture = 0;

                switch (step)
                {
                case 0:
                    new_position.x = std::stod(word) / pixel_division;
                    break;
                case 1:
                    new_position.y = std::stod(word) / pixel_division;
                    break;
                case 2:
                    new_position.z = std::stod(word) / pixel_division;
                    break;
                case 3:
                    new_scale.x = std::stod(word) / pixel_division;
                    break;
                case 4:
                    new_scale.y = std::stod(word) / pixel_division;
                    break;
                case 5:
                    new_scale.z = std::stod(word) / pixel_division;
                    break;
                case 6:
                    new_texture = std::stoi(word);
                    break;
                case 7:
                    new_type = static_cast<object_type>(std::stoi(word));

                    new_level.addObject(model_type, new_position, new_scale, new_texture, new_type);
                    making = LCOMM_NONE;
                    step = -1;
                    break;
                }

                ++step;
            }
        }
    }
    levels.push_back(new_level);
    ++level_count;
}

void game::update(double tick_time, glm::vec3 &plPos, glm::vec3 &plVel, aabb &plCol, bool &onG)
{
    if (level_count <= level_id)
    {
        std::cout << "level at index " << level_id << " does not exist.\n";
        return;
    }
    levels[level_id].updatePhysics(tick_time, plPos, plVel, plCol, onG);
}
void game::draw(shader &shad, double alpha)
{
    levels[level_id].drawLevel(shad, alpha);
}

void level::addObject(model_primitive_type model_type, glm::vec3 pos, glm::vec3 scale, unsigned int texture, object_type type)
{
    model_primitive c(model_type, false);
    c.Put(pos);
    c.Scale(scale);
    c.Image(texture);
    aabb col = makeAABB(pos, scale);
    objects.push_back({c, col, type});
    ++object_count;
}
void level::placeObject(glm::vec3 pos, unsigned int index)
{
    if (index > object_count)
    {
        std::cout << "That object doesn't or shouldn't exist, there aren't that many in the scene!\n";
        return;
    }

    objects[index].visual.Put(pos);
    putAABB(&objects[index].collider, pos);
}
void level::moveObject(glm::vec3 distance, unsigned int index)
{
    if (index > object_count)
    {
        std::cout << "That object doesn't or shouldn't exist, there aren't that many in the scene!\n";
        return;
    }

    objects[index].visual.Put(objects[index].visual.getPos() + distance);
    putAABB(&objects[index].collider, objects[index].collider.pos + distance);
}

void level::scaleObject(glm::vec3 scale, unsigned int index)
{
    if (index > object_count)
    {
        std::cout << "That object doesn't or shouldn't exist, there aren't that many in the scene!\n";
        return;
    }

    objects[index].visual.Scale(scale);
    objects[index].collider.scale = scale; // lol
}

void level::drawLevel(shader &shad, double alpha) // please add multi-shader support
{
    for (int i = 0; i < object_count; ++i)
    {
        objects[i].visual.draw(shad, alpha);
    }
}
// add tick system?
void level::updatePhysics(double tick_time, glm::vec3 &player_position, glm::vec3 &player_velocity, aabb &player_collider, bool &on_floor)
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
        if (objects[i].type == OBJ_PASSTHROUGH)
            continue;

        putAABB(&objects[i].collider, objects[i].visual.getPos());
        objects[i].collider.scale = objects[i].visual.getScale();
        collision col = normal_collision(&player_collider, &objects[i].collider, player_velocity * static_cast<float>(tick_time), glm::vec3(0.0));
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
        player_velocity.y += 0.5 * gravity * tick_time;

    player_position.y += player_velocity.y * tick_time;
    player_position.x += player_velocity.x * tick_time;
    player_position.z += player_velocity.z * tick_time;

    player_velocity.x = 0.0;
    player_velocity.z = 0.0;
    if (!on_floor)
        player_velocity.y += 0.5 * gravity * tick_time;
}