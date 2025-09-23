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

    level_command_type making = LCOMM_NONE;
    model_primitive_type model_type = MODEL_NONE;
    bool lastMadeWasVariable = false;
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
                if (word == "cube")
                {
                    making = LCOMM_PRIMITIVE;
                    pixel_division = 1.0;
                    model_type = MODEL_CUBE;
                    continue;
                }
                if (word == "pcube")
                {
                    making = LCOMM_PRIMITIVE_PIXELPOS;
                    pixel_division = pixel_scale;
                    model_type = MODEL_CUBE;
                    continue;
                }
                if (word == "quad")
                {
                    making = LCOMM_PRIMITIVE;
                    pixel_division = 1.0;
                    model_type = MODEL_QUAD;
                    continue;
                }
                if (word == "pquad")
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
                if (word == "ppyramid")
                {
                    making = LCOMM_PRIMITIVE;
                    pixel_division = pixel_scale;
                    model_type = MODEL_PYRAMID;
                    continue;
                }
                if (word == "tri")
                {
                    making = LCOMM_PRIMITIVE;
                    pixel_division = 1.0;
                    model_type = MODEL_TRI;
                    continue;
                }
                if (word == "ptri")
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
            // in the ifmakingvariable thing, set lastMadeWasVariable to true at the end
            if (making == LCOMM_TRIGGER) // after finishing this put it below LCOMM_PRIMITIVE creation for organization
            {
                // insert data here
                static trigger_cause_type tctype = TCAUSE_STARTGAME;
                static trigger_type ttype = TTYPE_MOVEOBJ;
                static glm::vec3 pos = glm::vec3(0.0);
                static double time = 0.0;

                switch (step)
                {
                case 0:
                    if (word == "start")
                    {
                        tctype = TCAUSE_STARTGAME;
                    }
                    else if (word == "touch")
                    {
                        tctype = TCAUSE_COLLISION;
                    }
                    else if (word == "seen")
                    {
                        tctype = TCAUSE_LOOKAT;
                    }
                    else if (word == "vequal")
                    {
                        tctype = TCAUSE_VARIABLEEQUAL;
                        step = 7;
                    }
                    else if (word == "vgreater")
                    {
                        tctype = TCAUSE_VARIABLEGREATER;
                        step = 7;
                    }
                    else if (word == "vlesser")
                    {
                        tctype = TCAUSE_VARIABLELESSER;
                        step = 7;
                    }
                    else
                    {
                        std::cout << "\tLevel load error: no valid trigger cause found. Please refer to the documentation or check for typos.\n";
                        goto finish;
                    }
                    break;
                case 1:
                    if (word == "move")
                    {
                        ttype = TTYPE_MOVEOBJ;
                        pixel_division = 1.0;
                    }
                    else if (word == "pmove")
                    {
                        ttype = TTYPE_MOVEOBJ;
                        pixel_division = pixel_scale;
                    }
                    else if (word == "scale")
                    {
                        ttype = TTYPE_SCALEOBJ;
                        pixel_division = 1.0;
                    }
                    else if (word == "pscale")
                    {
                        ttype = TTYPE_SCALEOBJ;
                        pixel_division = pixel_scale;
                    }
                    else
                    {
                        std::cout << "\tLevel load error: no valid trigger response found. Please refer to the documentation or check for typos.\n";
                        goto finish;
                    }
                    break;
                case 2:
                    pos.x = std::stod(word) / pixel_division;
                    break;
                case 3:
                    pos.y = std::stod(word) / pixel_division;
                    break;
                case 4:
                    pos.z = std::stod(word) / pixel_division;
                    break;
                case 5:
                    time = std::stod(word);
                    break;
                case 6:
                    goto finish;
                    break;
                default:
                finish:
                    if (lastMadeWasVariable)
                    {
                        new_level.addTrigger(new_level.getTriggerCount() - 1, tctype, ttype, pos, time);
                    }
                    else
                    {
                        new_level.addTrigger(new_level.getObjectCount() - 1, tctype, ttype, pos, time);
                    }
                    making = LCOMM_NONE;
                    step = -1;
                    break;
                }

                ++step;
            }

            if (making == LCOMM_PRIMITIVE || making == LCOMM_PRIMITIVE_PIXELPOS)
            {
                static glm::vec3 new_position = glm::vec3(0.0);
                static glm::vec3 new_scale = glm::vec3(1.0);
                static object_type new_type = OBJ_SOLID;
                static unsigned int new_texture = 0;

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
                    lastMadeWasVariable = false;
                    break;
                }

                ++step;
            }
        }
    }
    levels.push_back(new_level);
    ++level_count;
}

void game::update(double tick_time, glm::vec3 &plPos, glm::vec3 &plVel, aabb &plCol, glm::vec3 camDir, bool &onG)
{
    if (level_count <= level_id)
    {
        std::cout << "level at index " << level_id << " does not exist.\n";
        return;
    }
    levels[level_id].updateTriggerChecks(plCol, plPos, camDir);
    levels[level_id].updateTriggerPhysics(tick_time);
    levels[level_id].updatePlayerPhysics(tick_time, plPos, plVel, plCol, onG);
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
void level::addTrigger(unsigned int objIndex, trigger_cause_type tct, trigger_type tt, glm::vec3 pos, double time)
{
    triggers.push_back({false, (time != 0.0), objIndex, tct, tt, pos, time});
    objects[objIndex].visual.makeDynamic();
    ++trigger_count;
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
void level::updatePlayerPhysics(double tick_time, glm::vec3 &player_position, glm::vec3 &player_velocity, aabb &player_collider, bool &on_floor)
{
    if (object_count == 0)
    {
        std::cout << "no object levels.\n";
        return;
    }
    if (player_position.y < -25.0) // needs to be taken out in favor of a real looping mechanic??
    {
        player_position = glm::vec3(0.0, 25.0, 0.0);
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
void level::updateTriggerChecks(aabb &playerCollider, glm::vec3 &camPos, glm::vec3 &camDir)
{
    for (int i = 0; i < trigger_count; ++i)
    {
        switch (triggers[i].ctype)
        {
        case TCAUSE_STARTGAME:
            if (triggerGameStartedCheck)
                break;
            triggers[i].timerDown = triggers[i].time;
            triggers[i].triggered = true;
            break;
        case TCAUSE_COLLISION:
            if (!triggers[i].triggered && colliding(playerCollider, objects[triggers[i].objIndex].collider))
            {
                triggers[i].timerDown = triggers[i].time;
                triggers[i].triggered = true;
            }
            break;
        case TCAUSE_LOOKAT:
        {
            raycast ray = {camPos, camDir};
            bool raycasthit = colliding(ray, objects[triggers[i].objIndex].collider);

            if (raycasthit)
            {
                std::cout << "WHooooo!\n";
            }
        }
        break;
        default:
            std::cout << "Trigger update check error: Undefined trigger cause type.\n";
            triggers[i].triggered = false;
            break;
        }
    }

    triggerGameStartedCheck = true;
}
void level::updateTriggerPhysics(double tick_time)
{
    for (int i = 0; i < trigger_count; ++i)
    {
        if (!triggers[i].triggered)
            continue;

        if (triggers[i].time < 0.0 && triggers[i].timed)
        {
            triggers[i].triggered = false;
        }

        switch (triggers[i].type)
        {
        case TTYPE_MOVEOBJ:
            objects[triggers[i].objIndex].visual.Move(triggers[i].pos * static_cast<float>(tick_time));
            break;
        default:
            break;
        }
        if (triggers[i].timed)
        {
            triggers[i].time -= 1.0 * tick_time;
        }
    }
}