#include "../headers/system.hxx"
#include "../headers/audio.hxx"

extern float pixel_scale;
extern float ui_pixel_scale;
extern const glm::vec3 spawnLocation;
extern const unsigned int window_width;
extern const unsigned int window_height;

void game::setup_level(const char *level_path)
{
    audio_player_struct *audio_player = audio_player_struct::getInstance();
    audio_player->load_audio("test", "./snd/land.wav");
    audio_player->play_audio("test");

    level new_level;

    std::ifstream level_file(level_path);
    if (!level_file.is_open())
    {
        std::cout << "File " << level_path << " unable to be opened." << std::endl;
        return;
    }

    level_command_type making = LCOMM_NONE;
    model_primitive_type model_type = MODEL_NONE;
    int step = 0;
    std::string word = "", line = "";
    double pixel_division = 1.0;
    while (std::getline(level_file, line))
    {
        if (line[0] == '/')
            continue;

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
                if (word == "var")
                {
                    making = LCOMM_VARIABLE;
                    continue;
                }
                if (word == "globalvar")
                {
                    making = LCOMM_GLOBAL_VARIABLE; // not complete!!
                    continue;
                }
                if (word == "ui")
                {
                    making = LCOMM_UI_OBJECT;
                    continue;
                }
            }
            if (making == LCOMM_UI_OBJECT)
            {
                static glm::vec2 ui_pos = glm::vec2(0.0);
                static glm::vec2 ui_scale = glm::vec2(0.0);
                unsigned int ui_texture = 0;

                switch (step)
                {
                case 0:
                    ui_pos.x = std::stod(word);
                    break;
                case 1:
                    ui_pos.y = std::stod(word);
                    break;
                case 2:
                    ui_scale.x = std::stod(word);
                    break;
                case 3:
                    ui_scale.y = std::stod(word);
                    break;
                case 4:
                    ui_texture = std::stoi(word);

                    new_level.addUIObject(ui_pos, ui_scale, ui_texture);
                    making = LCOMM_NONE;
                    step = -1;
                    break;
                default:
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
                static bool visible = true;

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
                    if (std::stoi(word) < 0)
                    {
                        new_texture = 0;
                        visible = false;
                    }
                    break;
                case 7:
                    new_type = static_cast<object_type>(std::stoi(word));

                    new_level.addObject(model_type, new_position, new_scale, new_texture, new_type, visible);
                    making = LCOMM_NONE;
                    step = -1;
                    visible = true;
                    break;
                }

                ++step;
            }
            if (making == LCOMM_VARIABLE)
            {
                static std::string sID = "";
                static double value = 0.0;

                switch (step)
                {
                case 0:
                    sID = word;
                    break;
                case 1:
                    value = std::stod(word);
                    new_level.addVariable(sID, value);
                    making = LCOMM_NONE;
                    step = -1;
                    break;
                default:
                    break;
                }
                ++step;
            }
            if (making == LCOMM_TRIGGER)
            {
                static trigger_cause_type tctype = TCAUSE_STARTGAME;
                static trigger_type ttype = TTYPE_MOVEOBJ;
                static glm::vec3 trigger_pos = glm::vec3(0.0);
                static double trigger_time = 0.0;
                static int variable_index = -1, variable_update_index = -1;
                static double variable_value = 0.0, variable_update_value = 0.0;
                static std::string trigger_set_level = level_path;
                static std::string trigger_audio_id = "";

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
                    else if (word == "is")
                    {
                        step = 6; // becomes 7 before next round
                    }
                    else if (word == "hovered")
                    {
                        tctype = TCAUSE_UI_HOVERED;
                    }
                    else if (word == "clicked")
                    {
                        tctype = TCAUSE_UI_CLICKED;
                    }
                    else
                    {
                        std::cout << word << "\tLevel load error: no valid trigger cause found. Please refer to the documentation or check for typos.\n";
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
                    else if (word == "getvar")
                    {
                        step = 9; // becomes 10 before next round
                    }
                    else if (word == "setlevel")
                    {
                        ttype = TTYPE_CHANGELVL;
                        step = 12; // becomes 13 before next round
                    }
                    else
                    {
                        std::cout << "\tLevel load error: no valid trigger response found. Please refer to the documentation or check for typos.\n";
                        goto finish;
                    }
                    break;
                case 2:
                    trigger_pos.x = std::stod(word) / pixel_division;
                    break;
                case 3:
                    trigger_pos.y = std::stod(word) / pixel_division;
                    break;
                case 4:
                    trigger_pos.z = std::stod(word) / pixel_division;
                    break;
                case 5:
                    trigger_time = std::stod(word);
                    goto finish;
                    break;
                case 6: // what even
                    break;
                case 7:
                    for (int i = 0; i < new_level.getVariableCount(); ++i)
                    {
                        if (word != new_level.getVariableAtIndex(i)->strID)
                            continue;
                        variable_index = i;
                    }
                    if (variable_index == -1)
                    {
                        std::cout << "Variable trigger check error: variable " << word << " not found. Please re-check the names of existing variables in your level file and use one of those or make a new one.\n";
                        goto finish;
                    }
                    break;
                case 8:
                    if (word == "equalto")
                    {
                        tctype = TCAUSE_VARIABLEEQUAL;
                    }
                    else if (word == "lessthan")
                    {
                        tctype = TCAUSE_VARIABLELESSER;
                    }
                    else if (word == "greaterthan")
                    {
                        tctype = TCAUSE_VARIABLEGREATER;
                    }
                    else
                    {
                        std::cout << "Variable trigger update error: " << word << " not recognised as valid trigger variable operation. Please check your level file and fix any mistakes.\n";
                        goto finish;
                    }
                    break;
                case 9:
                    variable_value = std::stod(word);
                    step = 0; // becomes 1 before next round
                    break;
                case 10: // variable update after triggered
                    for (int i = 0; i < new_level.getVariableCount(); ++i)
                    {
                        if (word != new_level.getVariableAtIndex(i)->strID)
                            continue;
                        variable_update_index = i;
                    }
                    if (variable_update_index == -1)
                    {
                        std::cout << "Error: variable " << word << " not found. Please re-check the names of existing variables in your level file and use one of those or make a new one.\n";
                        goto finish;
                    }
                    break;
                case 11:
                    if (word == "set")
                    {
                        ttype = TTYPE_SETVARIABLE;
                    }
                    else if (word == "add")
                    {
                        ttype = TTYPE_ADDVARIABLE;
                    }
                    else if (word == "subtract")
                    {
                        ttype = TTYPE_SUBTRACTVARIABLE;
                    }
                    else
                    {
                        std::cout << "Error: " << word << " not a recognised variable manipulation. Please check your level file for mistakes.\n";
                        goto finish;
                    }
                    break;
                case 12:
                    variable_update_value = std::stod(word);
                    step = 4; // will become 5 before next round
                    break;
                case 13:
                    trigger_set_level = word;
                    step = 4; // becomes 5 before next round
                    break;
                default:
                finish:
                    switch (tctype)
                    {
                    case TCAUSE_VARIABLEEQUAL:
                    case TCAUSE_VARIABLEGREATER:
                    case TCAUSE_VARIABLELESSER:
                        new_level.addTriggerVariableCheck(variable_index, variable_value, tctype, ttype);
                        break;
                    case TCAUSE_COLLISION:
                    case TCAUSE_LOOKAT:
                        new_level.addTriggerObjectCheck(new_level.getObjectCount() - 1, tctype, ttype);
                        break;
                    case TCAUSE_UI_CLICKED:
                    case TCAUSE_UI_HOVERED:
                        new_level.addTriggerUICheck(new_level.getUICount() - 1, tctype, ttype);
                        break;
                    case TCAUSE_STARTGAME:
                        new_level.addTrigger(tctype, ttype);
                        break;
                    default:
                        break;
                    }
                    switch (ttype)
                    {
                    case TTYPE_CHANGELVL:
                        new_level.setTriggerLevelResponse(trigger_set_level, trigger_time);
                        break;
                    case TTYPE_ADDVARIABLE:
                    case TTYPE_SUBTRACTVARIABLE:
                    case TTYPE_SETVARIABLE:
                        new_level.setTriggerVariableResponse(variable_update_index, variable_update_value, trigger_time);
                        break;
                    case TTYPE_FADEINSOUND:
                    case TTYPE_FADEOUTSOUND:
                    case TTYPE_PLAYSOUND:
                    case TTYPE_STOPSOUND:
                        new_level.setTriggerAudioResponse(trigger_audio_id, trigger_time);
                        break;
                    case TTYPE_MOVEOBJ:
                    case TTYPE_ROTATEOBJ:
                    case TTYPE_SCALEOBJ:
                        new_level.setTriggerObjectResponse(new_level.getObjectCount() - 1, trigger_pos, trigger_time);
                        break;
                    default:
                        break;
                    }
                    // if (tctype == TCAUSE_VARIABLEEQUAL || tctype == TCAUSE_VARIABLEGREATER || tctype == TCAUSE_VARIABLELESSER)
                    // {
                    //     if (ttype == TTYPE_CHANGELVL)
                    //     {
                    //         new_level.addTriggerVariable(variable_index, variable_value, tctype, ttype, trigger_set_level, trigger_time);
                    //     }
                    //     else if (ttype == TTYPE_SETVARIABLE || ttype == TTYPE_ADDVARIABLE || ttype == TTYPE_SUBTRACTVARIABLE)
                    //     {
                    //         new_level.addTriggerVariable(variable_index, variable_value, tctype, ttype, variable_update_index, variable_update_value, trigger_time);
                    //     }
                    //     else
                    //     {
                    //         new_level.addTriggerVariable(new_level.getObjectCount() - 1, variable_index, variable_value, tctype, ttype, trigger_pos, trigger_time);
                    //     }
                    // }
                    // else if (tctype == TCAUSE_UI_HOVERED || tctype == TCAUSE_UI_CLICKED)
                    // {
                    //     if (ttype == TTYPE_CHANGELVL)
                    //     {
                    //         new_level.addTriggerUI(new_level.getUICount() - 1, tctype, ttype, trigger_set_level, trigger_time);
                    //     }
                    //     else if (ttype == TTYPE_SETVARIABLE || ttype == TTYPE_ADDVARIABLE || ttype == TTYPE_SUBTRACTVARIABLE)
                    //     {
                    //         new_level.addTriggerUI(new_level.getUICount() - 1, tctype, ttype, variable_update_index, variable_update_value, trigger_time);
                    //     }
                    //     else
                    //     {
                    //         new_level.addTriggerUI(new_level.getUICount() - 1, new_level.getObjectCount() - 1, tctype, ttype, trigger_pos, trigger_time);
                    //     }
                    // }
                    // else
                    // {
                    //     if (ttype == TTYPE_CHANGELVL)
                    //     {
                    //         new_level.addTriggerObject(new_level.getObjectCount() - 1, tctype, ttype, trigger_set_level, trigger_time);
                    //     }
                    //     else if (ttype == TTYPE_SETVARIABLE || ttype == TTYPE_ADDVARIABLE || ttype == TTYPE_SUBTRACTVARIABLE)
                    //     {
                    //         new_level.addTriggerObject(new_level.getObjectCount() - 1, tctype, ttype, variable_update_index, variable_update_value, trigger_time);
                    //     }
                    //     else
                    //     {
                    //         new_level.addTriggerObject(new_level.getObjectCount() - 1, tctype, ttype, trigger_pos, trigger_time);
                    //     }
                    // }
                    making = LCOMM_NONE;
                    step = -1;
                    break;
                }

                ++step;
            }
        }
    }
    current_level = new_level;
}

void game::update_level(double tick_time, glm::vec3 &plPos, glm::vec3 &plLastPos, glm::vec3 &plVel, glm::vec2 &mousePos, bool &mouseClicked, aabb &plCol, glm::vec3 camDir, bool &onG)
{
    if (current_level.setLevel != "")
    {
        std::string new_level_path = current_level.setLevel;
        current_level.reset();
        setup_level(new_level_path.c_str());
        plPos = spawnLocation;
        return;
    }
    current_level.updateTriggerChecks(plCol, plPos, camDir, mousePos, mouseClicked);
    current_level.updateTriggerResponses(tick_time);
    current_level.updatePlayerPhysics(tick_time, plPos, plLastPos, plVel, plCol, onG);
}
void game::draw_level(shader &shad, shader &shad_ui, double alpha)
{
    current_level.drawLevel(shad, shad_ui, alpha);
}

void level::addObject(model_primitive_type model_type, glm::vec3 pos, glm::vec3 scale, unsigned int texture, object_type type, bool visible)
{
    model_primitive c(model_type, false, visible);
    c.Put(pos);
    c.Put(pos); // second call to set last_position as well
    c.Scale(scale);
    c.Image(texture);
    aabb col = makeAABB(pos, scale);
    objects.push_back({c, col, type});
}
void level::addVariable(std::string id, double value)
{
    variables.push_back({id, value});
}
void level::addUIObject(glm::vec2 pos, glm::vec2 scale, unsigned int texture)
{
    model_primitive quad(MODEL_QUAD);
    quad.Image(texture);
    quad.Put(-pos.x / ui_pixel_scale, pos.y / ui_pixel_scale, 0.0);
    double pixel_multi = 1.0 / ui_pixel_scale;
    quad.Scale(scale.x * pixel_multi, scale.y * pixel_multi, 1.0);
    aabb2d ui_collider({pos * ((float)window_height / ui_pixel_scale), glm::vec2(scale.x * (float)window_height * pixel_multi, scale.y * (float)window_height * pixel_multi)});

    ui_objects.push_back({quad, ui_collider});
}
void level::addTriggerObjectCheck(unsigned int objIndex, trigger_cause_type tct, trigger_type tt)
{
    triggers.push_back(level_trigger(objIndex, 0, 0, 0.0, tct, tt));
}
void level::addTriggerVariableCheck(unsigned int varIndex, double varValue, trigger_cause_type tct, trigger_type tt)
{
    triggers.push_back(level_trigger(0, 0, varIndex, varValue, tct, tt));
}
void level::addTriggerUICheck(unsigned int uiIndex, trigger_cause_type tct, trigger_type tt)
{
    triggers.push_back(level_trigger(0, uiIndex, 0, 0.0, tct, tt));
}
void level::addTrigger(trigger_cause_type tct, trigger_type tt)
{
    triggers.push_back(level_trigger(0, 0, 0, 0.0, tct, tt));
}
void level::setTriggerObjectResponse(unsigned int objIndex, glm::vec3 pos, double time)
{
    objects[objIndex].visual.makeDynamic();
    triggers[triggers.size() - 1].setObjResponse(objIndex, pos, time);
}
void level::setTriggerVariableResponse(unsigned int varUpdIndex, double varUpdValue, double time)
{
    triggers[triggers.size() - 1].setVariableResponse(varUpdIndex, varUpdValue, time);
}
void level::setTriggerAudioResponse(std::string audioID, double time)
{
    triggers[triggers.size() - 1].setAudioResponse(audioID, time);
}
void level::setTriggerLevelResponse(std::string levelStr, double time)
{
    triggers[triggers.size() - 1].setLevelResponse(levelStr, time);
}

// void level::addTriggerObject(unsigned int objIndex, trigger_cause_type tct, trigger_type tt, glm::vec3 pos, double time)
// {
//     triggers.push_back(level_trigger(objIndex, 0, 0, 0, 0.0, 0.0, tct, tt, pos, time, "", ""));
//     objects[objIndex].visual.makeDynamic();
//     ++trigger_count;
// }
// void level::addTriggerObject(unsigned int objIndex, trigger_cause_type tct, trigger_type tt, unsigned int updvariable_index, double updvariable_value, double time)
// {
//     triggers.push_back(level_trigger(objIndex, 0, 0, updvariable_index, 0.0, updvariable_value, tct, tt, glm::vec3(0.0), time, "", ""));
//     ++trigger_count;
// }
// void level::addTriggerObject(unsigned int objIndex, trigger_cause_type tct, trigger_type tt, std::string trigger_set_level, double time)
// {
//     triggers.push_back(level_trigger(objIndex, 0, 0, 0, 0.0, 0.0, tct, tt, glm::vec3(0.0), time, trigger_set_level, ""));
//     ++trigger_count;
// }
// void level::addTriggerObject(unsigned int objIndex, trigger_cause_type tct, trigger_type tt, std::string audio_id, double time)
// {
//     triggers.push_back(level_trigger(objIndex, 0, 0, 0, 0.0, 0.0, tct, tt, glm::vec3(0.0), time, "", audio_id));
//     ++trigger_count;
// }
// void level::addTriggerVariable(unsigned int objIndex, unsigned int varIndex, double varValue, trigger_cause_type tct, trigger_type tt, glm::vec3 pos, double time)
// {
//     objects[objIndex].visual.makeDynamic();
//     triggers.push_back(level_trigger(objIndex, 0, varIndex, 0, varValue, 0.0, tct, tt, pos, time, "", ""));
//     ++trigger_count;
// }
// void level::addTriggerVariable(unsigned int varIndex, double varValue, trigger_cause_type tct, trigger_type tt, unsigned int updvariable_index, double updvariable_value, double time)
// {
//     triggers.push_back(level_trigger(0, 0, varIndex, updvariable_index, varValue, updvariable_value, tct, tt, glm::vec3(0.0), time, "", ""));
//     ++trigger_count;
// }
// void level::addTriggerVariable(unsigned int varIndex, double varValue, trigger_cause_type tct, trigger_type tt, std::string trigger_set_level, double time)
// {
//     triggers.push_back(level_trigger(0, 0, varIndex, 0, varValue, 0.0, tct, tt, glm::vec3(0.0), time, trigger_set_level, ""));
//     ++trigger_count;
// }
// void level::addTriggerUI(unsigned int uiIndex, unsigned int objIndex, trigger_cause_type tct, trigger_type tt, glm::vec3 pos, double time)
// {
//     triggers.push_back(level_trigger(objIndex, uiIndex, 0, 0, 0.0, 0.0, tct, tt, pos, time, "", ""));
//     ++trigger_count;
// }
// void level::addTriggerUI(unsigned int uiIndex, trigger_cause_type tct, trigger_type tt, unsigned int updvariable_index, double updvariable_value, double time)
// {
//     triggers.push_back(level_trigger(0, uiIndex, 0, updvariable_index, 0.0, updvariable_value, tct, tt, glm::vec3(0.0), time, "", ""));
//     ++trigger_count;
// }
// void level::addTriggerUI(unsigned int uiIndex, trigger_cause_type tct, trigger_type tt, std::string trigger_set_level, double time)
// {
//     triggers.push_back(level_trigger(0, uiIndex, 0, 0, 0.0, 0.0, tct, tt, glm::vec3(0.0), time, trigger_set_level, ""));
//     ++trigger_count;
// } // add audio addTrigger funcs here don't forget in the loadLevel func too

void level::scaleObject(glm::vec3 scale, unsigned int index)
{
    if (index >= getObjectCount())
    {
        std::cout << "That object doesn't or shouldn't exist, there aren't that many in the scene!\n";
        return;
    }

    objects[index].visual.Scale(scale);
    objects[index].collider.scale = scale; // lol
}

void level::reset()
{
    // insert any existing level global variables back to game list here

    objects.clear();
    ui_objects.clear();
    variables.clear();
    triggers.clear();
    triggerGameStartedCheck = false;
    setLevel = "";
}
void level::drawLevel(shader &shad, shader &shad_ui, double alpha) // please add multi-shader support
{
    for (int i = 0; i < getObjectCount(); ++i)
    {
        objects[i].visual.draw(shad, pixel_scale, alpha);
    }
    for (int i = 0; i < getUICount(); ++i)
    {
        ui_objects[i].visual.draw(shad_ui, ui_pixel_scale, alpha);
    }
}

void level::updatePlayerPhysics(double tick_time, glm::vec3 &player_position, glm::vec3 &player_last_position, glm::vec3 &player_velocity, aabb &player_collider, bool &on_floor)
{
    if (getObjectCount() == 0)
    {
        // std::cout << "no object levels.\n";
        return;
    }
    if (player_position.y < -25.0) // needs to be taken out in favor of a real looping mechanic??
    {
        player_position = glm::vec3(0.0, 25.0, 0.0);
    }

    on_floor = false;

    putAABB(&player_collider, player_position + player_velocity * (float)tick_time); // future collision
    for (int i = 0; i < objects.size(); ++i)
    {
        if (objects[i].type == OBJ_PASSTHROUGH)
            continue;

        objects[i].collider.pos = objects[i].visual.getPos();
        objects[i].collider.scale = objects[i].visual.getScale();
        collision col = normal_collision(&player_collider, &objects[i].collider, player_velocity * static_cast<float>(tick_time),
                                         (objects[i].visual.getPos() - objects[i].visual.getLastPosition()) * static_cast<float>(tick_time));
        if (col.hit)
        {
            for (int i = 0; i < 3; ++i)
            {
                if (col.hitLocation[i] != player_collider.pos[i])
                {
                    player_position[i] = col.hitLocation[i];
                }
            }
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
                    player_position += (objects[i].visual.getPos() - objects[i].visual.getLastPosition());
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

    player_position.x += player_velocity.x * tick_time;
    player_position.y += player_velocity.y * tick_time;
    player_position.z += player_velocity.z * tick_time;

    player_velocity.x = 0.0;
    player_velocity.z = 0.0;
    if (!on_floor)
        player_velocity.y += 0.5 * gravity * tick_time;
}

void level::updateTriggerChecks(aabb &playerCollider, glm::vec3 &camPos, glm::vec3 &camDir, glm::vec2 &mousePos, bool &mouseClicked)
{
    for (int i = 0; i < getTriggerCount(); ++i)
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
        case TCAUSE_UI_HOVERED:
            if (!triggers[i].triggered && colliding(ui_objects[triggers[i].uiIndex].collider, mousePos))
            {
                triggers[i].timerDown = triggers[i].time;
                triggers[i].triggered = true;
            }
            break;
        case TCAUSE_UI_CLICKED:
            if (!triggers[i].triggered && colliding(ui_objects[triggers[i].uiIndex].collider, mousePos) && mouseClicked)
            {
                triggers[i].timerDown = triggers[i].time;
                triggers[i].triggered = true;
            }
            break;
        case TCAUSE_LOOKAT:
        {
            raycast ray = {camPos + glm::vec3(0.0, 0.5, 0.0), glm::normalize(camDir)};
            glm::vec3 hitPos = glm::vec3(0.0);
            bool raycasthit = colliding(ray, objects[triggers[i].objIndex].collider, hitPos);

            glm::vec3 boxmin = objects[triggers[i].objIndex].collider.pos - objects[triggers[i].objIndex].collider.scale;
            glm::vec3 boxmax = objects[triggers[i].objIndex].collider.pos + objects[triggers[i].objIndex].collider.scale;
            if (raycasthit && !triggers[i].triggered)
            {
                triggers[i].triggered = true;
                triggers[i].timerDown = triggers[i].time;
            }
        }
        break;
        case TCAUSE_VARIABLEEQUAL:
            if (!triggers[i].triggered && variables[triggers[i].varCheckIndex].value == triggers[i].varValueCompare)
            {
                triggers[i].triggered = true;
                triggers[i].timerDown = triggers[i].time;
            }
            if (variables[triggers[i].varCheckIndex].value != triggers[i].varValueCompare && triggers[i].time == 0.0)
                triggers[i].triggered = false;
            break;
        case TCAUSE_VARIABLELESSER:
            if (!triggers[i].triggered && variables[triggers[i].varCheckIndex].value < triggers[i].varValueCompare)
            {
                if (triggers[i].time != 0.0 || triggers[i].time == triggers[i].timerDown)
                    triggers[i].triggered = true;
                triggers[i].timerDown = triggers[i].time;
            }
            if (variables[triggers[i].varCheckIndex].value >= triggers[i].varValueCompare && triggers[i].time == 0.0)
                triggers[i].triggered = false;
            break;
        case TCAUSE_VARIABLEGREATER:
            if (!triggers[i].triggered && variables[triggers[i].varCheckIndex].value > triggers[i].varValueCompare)
            {
                if (triggers[i].time != 0.0 || triggers[i].time == triggers[i].timerDown)
                    triggers[i].triggered = true;
                triggers[i].timerDown = triggers[i].time;
            }
            if (variables[triggers[i].varCheckIndex].value <= triggers[i].varValueCompare && triggers[i].time == 0.0)
                triggers[i].triggered = false;
            break;
        default:
            std::cout << "Trigger update check error: Undefined trigger cause type.\n";
            triggers[i].triggered = false;
            break;
        }
    }

    triggerGameStartedCheck = true;
}
void level::updateTriggerResponses(double tick_time)
{
    audio_player_struct *audio_player = audio_player_struct::getInstance();

    for (int i = 0; i < getTriggerCount(); ++i)
    {
        if (!triggers[i].triggered)
            continue;

        if (triggers[i].timerDown < 0.0 && triggers[i].time != 0.0)
        {
            triggers[i].triggered = false;
        }

        switch (triggers[i].type)
        {
        case TTYPE_MOVEOBJ:
            objects[triggers[i].objIndex].visual.Move(triggers[i].pos * static_cast<float>(tick_time));
            objects[triggers[i].objIndex].collider.pos = objects[triggers[i].objIndex].visual.getPos();
            break;
        case TTYPE_SCALEOBJ:
            objects[triggers[i].objIndex].visual.Scale(objects[triggers[i].objIndex].visual.getScale() + triggers[i].pos * static_cast<float>(tick_time));
            objects[triggers[i].objIndex].collider.scale = objects[triggers[i].objIndex].visual.getScale();
            break;
        case TTYPE_ROTATEOBJ:
            std::cout << "rotating objects is not implemented yet :(\n";
            break;
        case TTYPE_SETVARIABLE:
            if (triggers[i].time == triggers[i].timerDown)
            {
                variables[triggers[i].varUpdIndex].value = triggers[i].varUpdValue;
                triggers[i].triggered = false;
            }
            break;
        case TTYPE_ADDVARIABLE:
            if (triggers[i].time == triggers[i].timerDown)
            {
                variables[triggers[i].varUpdIndex].value += triggers[i].varUpdValue;
            }
            break;
        case TTYPE_SUBTRACTVARIABLE:
            if (triggers[i].time == triggers[i].timerDown)
                variables[triggers[i].varUpdIndex].value -= triggers[i].varUpdValue;
            break;
        case TTYPE_CHANGELVL:
            if (triggers[i].time == triggers[i].timerDown || triggers[i].time == 0.0)
            {
                setLevel = triggers[i].newLevel;
                triggers[i].triggered = false;
            }
            break;
        case TTYPE_PLAYSOUND:
            if (triggers[i].time == triggers[i].timerDown || triggers[i].time == 0.0)
            {
                audio_player->play_audio(triggers[i].audioID);
            }
            break;
        case TTYPE_STOPSOUND:
            if (triggers[i].time == triggers[i].timerDown || triggers[i].time == 0.0)
            {
                audio_player->stop_audio(triggers[i].audioID);
            }
            break;
        default:
            break;
        }
        if (triggers[i].time != 0.0)
        {
            triggers[i].timerDown -= 1.0 * tick_time;
        }
    }
}