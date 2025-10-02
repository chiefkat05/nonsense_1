#include "../headers/system.hxx"
#include "../headers/audio.hxx"

extern float pixel_scale;
extern float ui_pixel_scale;
extern const glm::vec3 spawnLocation;
extern const unsigned int current_window_width;
extern const unsigned int current_window_height;
extern const unsigned int window_width;
extern const unsigned int window_height;
extern bool window_resize;
extern bool mousePressed;
extern glm::vec2 mousePos;

void game::setup_level(const char *level_path)
{
    current_level_path = level_path;
    level new_level;

    for (int i = 0; i < global_variables.size(); ++i)
    {
        new_level.addVariable(global_variables[i].strID, global_variables[i].value, true);
    }

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
    unsigned int lineNum = 0;
    while (std::getline(level_file, line))
    {
        if (line[0] == '/')
        {
            new_level.addLine();
            ++lineNum;
            continue;
        }

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
                if (word == "sound")
                {
                    making = LCOMM_AUDIO;
                    continue;
                }
            }
            if (making == LCOMM_AUDIO)
            {
                static std::string id = "", path = "";
                switch (step)
                {
                case 0:
                    id = word;
                    break;
                case 1:
                    path = word;
                    new_level.addAudio(id, path);
                    step = -1;
                    making = LCOMM_NONE;
                    break;
                }
                ++step;
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
                    new_level.getUIAtIndex(new_level.getUICount() - 1)->lineIndex = lineNum - 1;
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
                    new_level.getObjectAtIndex(new_level.getObjectCount() - 1)->lineIndex = lineNum;
                    making = LCOMM_NONE;
                    step = -1;
                    visible = true;
                    break;
                }

                ++step;
            }
            if (making == LCOMM_VARIABLE || making == LCOMM_GLOBAL_VARIABLE)
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
                    new_level.addVariable(sID, value, (making == LCOMM_GLOBAL_VARIABLE));
                    new_level.getVariableAtIndex(new_level.getVariableCount() - 1)->lineIndex = lineNum - 1;
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
                    else if (word == "playsound")
                    {
                        ttype = TTYPE_PLAYSOUND;
                        step = 5; // becomes 6 before next round
                    }
                    else if (word == "stopsound")
                    {
                        ttype = TTYPE_STOPSOUND;
                        step = 5; // becomes 6 before next round
                    }
                    else if (word == "fadeinsound")
                    {
                        ttype = TTYPE_FADEINSOUND;
                        step = 5; // becomes 6 before next round
                    }
                    else if (word == "fadeoutsound")
                    {
                        ttype = TTYPE_FADEOUTSOUND;
                        step = 5; // becomes 6 before next round
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
                case 6:
                    trigger_audio_id = word;
                    step = 4; // becomes 5 before next round
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
                    new_level.getTriggerAtIndex(new_level.getTriggerCount() - 1)->lineIndex = lineNum - 1;
                    making = LCOMM_NONE;
                    step = -1;
                    break;
                }

                ++step;
            }
        }
        new_level.addLine();
        ++lineNum;
    }

    level_file.close();
    current_level.deleteOctree();
    current_level = new_level;
}

void game::update_level(double tick_time, level_object &plObject, glm::vec3 camDir, bool &onG, bool debug)
{
    if (current_level.setLevel != "")
    {
        std::string new_level_path = current_level.setLevel;
        for (int i = 0; i < current_level.getVariableCount(); ++i)
        {
            if (!current_level.getVariableAtIndex(i)->is_global)
                return;

            global_variables.push_back(*current_level.getVariableAtIndex(i));
        }
        current_level.reset();
        setup_level(new_level_path.c_str());
        // plObject.Put(spawnLocation);
        return;
    }
    for (int i = 0; i < current_level.getUICount(); ++i)
    {
        current_level.getUIAtIndex(i)->updateButtonState();
    }
    current_level.updateTriggerChecks(plObject, camDir);
    current_level.updateTriggerResponses(plObject, tick_time);
    if (!debug)
        current_level.updatePlayerPhysics(tick_time, plObject, onG);
}
void game::draw_level(shader &shad, shader &shad_ui, bool debugMode, double alpha)
{
    current_level.drawLevel(shad, shad_ui, debugMode, alpha);
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
void level::removeObjectAtIndex(unsigned int index)
{
    for (int i = index; i < objects.size(); ++i)
    {
        objects[i].lineIndex -= 1;
    }
    objects.erase(objects.begin() + index); // does this even work?

    deleteOctree();
    treeMade = false;
}
void level::addVariable(std::string id, double value, bool global)
{
    variables.push_back({id, value, 0, global});
}
void level::addUIObject(glm::vec2 pos, glm::vec2 scale, unsigned int texture)
{
    model_primitive quad(MODEL_QUAD);
    quad.Image(texture);
    quad.Put(-pos.x / ui_pixel_scale * ((double)current_window_width / (double)current_window_height), pos.y / ui_pixel_scale, 0.0);
    quad.Scale(scale.x / ui_pixel_scale, scale.y / ui_pixel_scale, 1.0);
    aabb2d ui_collider({pos * (glm::vec2((float)current_window_width, (float)current_window_height) / ui_pixel_scale),
                        glm::vec2((scale.x * (float)current_window_width / ui_pixel_scale) / ((double)window_width / (double)window_height),
                                  scale.y * (float)current_window_height / ui_pixel_scale)});

    ui_objects.push_back({quad, ui_collider, 0, pos, scale});
}
void level::addAudio(std::string id, std::string path)
{
    audio_player_struct *audio_player = audio_player_struct::getInstance();
    audio_player->load_audio(id, path);
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
void ui_object::updateSize(bool window_resized)
{
    if (!window_resized)
        return;
    double window_aspect = (double)window_width / (double)window_height;
    collider.pos = glm::vec2(truepos.x * ((float)current_window_width / ui_pixel_scale), truepos.y * ((float)current_window_height / ui_pixel_scale));
    collider.scale = glm::vec2((truescale.x * (float)current_window_width / ui_pixel_scale) / window_aspect,
                               truescale.y * (float)current_window_height / ui_pixel_scale);
}
void ui_object::updateButtonState()
{
    clicked = false;
    if (colliding(collider, mousePos)) // mouse over button
    {
        hovered = true;
        if (mousePressed) // mouse over button and pressed
        {
            held = true;
            released = false;
        }
        else // mouse over button, not pressed
        {
            if (held)
            {
                clicked = true;
            }
            held = false;
            released = true;
        }
    }
    else // mouse not over button
    {
        hovered = false;
        held = false;
        released = true;
        clicked = false;
    }
}
void level::drawLevel(shader &shad, shader &shad_ui, bool debugMode, double alpha) // please add multi-shader support
{
    for (int i = 0; i < getObjectCount(); ++i)
    {
        objects[i].visual.draw(shad, pixel_scale, alpha);
    }
    for (int i = 0; i < getUICount(); ++i)
    {
        ui_objects[i].updateSize(window_resize);
        ui_objects[i].visual.draw(shad_ui, ui_pixel_scale, alpha);
    }
    if (tree != nullptr && debugMode)
    {
        tree->draw(shad, alpha);
    }
}

void collisionResolution(level_object *objA, level_object *objB, bool &on_floor)
{
    collision c = normal_collision(&objA->collider, &objB->collider, objA->velocity, objB->velocity);
    if (c.hit)
    {
        // uses collider.pos instead of visual.getPos() because collider.pos is the most updated at this moment (it's using future velocity movement, remember?)
        // if we compare hitLocation to visual.getPos(), then it will always return true for every axis since visualPos is not at that location and
        // the object will be pulled forward to the new hitPosition calculated in relation to the colliderPosition.
        if (c.hitLocation.x != objA->collider.pos.x)
        {
            objA->visual.refPos()->x = c.hitLocation.x;
            objA->visual.refLastPos()->x = c.hitLocation.x;
        }
        if (c.hitLocation.y != objA->collider.pos.y)
        {
            objA->visual.refPos()->y = c.hitLocation.y;
            objA->visual.refLastPos()->y = c.hitLocation.y;
        }
        if (c.hitLocation.z != objA->collider.pos.z)
        {
            objA->visual.refPos()->z = c.hitLocation.z;
            objA->visual.refLastPos()->z = c.hitLocation.z;
        }
        putAABB(&objA->collider, objA->visual.getPos());
        for (int j = 0; j < 3; ++j)
        {
            if (j == 1 && c.normal[j] > 0)
            {
                on_floor = true;
            }
            if (c.normal[j] < 0 && objA->velocity[j] > 0.0)
            {
                objA->velocity[j] = 0.0;
            }
            if (c.normal[j] > 0 && objA->velocity[j] < 0.0)
            {
                objA->velocity[j] = 0.0;
            }
        }
    }
}
void octree::collisionTest(level_object *pPlayer, bool &on_floor)
{
    static octree *ancestor_stack[max_octree_depth];
    static int depth = 0;

    ancestor_stack[depth] = this;
    ++depth;
    // if (depth >= max_octree_depth)
    // {
    //     --depth;
    //     return;
    // }

    for (int i = 0; i < depth; ++i)
    {
        level_object *objA = nullptr, *objB = nullptr;
        for (objA = ancestor_stack[i]->pObjList; objA != nullptr; objA = objA->pNextObject)
        {
            for (objB = pObjList; objB != nullptr; objB = objB->pNextObject)
            {
                if (objA == objB || objA != pPlayer && objB != pPlayer || objB->type == OBJ_PASSTHROUGH || objA->type == OBJ_PASSTHROUGH)
                    continue;
                if (objA == pPlayer)
                {
                    collisionResolution(objA, objB, on_floor);
                }
                if (objB == pPlayer)
                {
                    collisionResolution(objB, objA, on_floor);
                }
            }
        }
    }

    for (int i = 0; i < 8; ++i)
    {
        if (pChild[i] != nullptr)
        {
            pChild[i]->collisionTest(pPlayer, on_floor);
        }
    }
    --depth;
}

void level::updatePlayerPhysics(double tick_time, level_object &plObject, bool &on_floor)
{
    if (getObjectCount() == 0)
    {
        // std::cout << "no object levels.\n";
        return;
    }
    if (plObject.visual.getPos().y < -25.0) // needs to be taken out in favor of a real looping mechanic??
    {
        plObject.Put(0.0, 25.0, -3.0);
        plObject.Put(0.0, 25.0, -3.0);
    }

    on_floor = false;

    putAABB(&plObject.collider, plObject.visual.getPos() + plObject.velocity * (float)tick_time); // future position
    if (tree == nullptr)
    {
        treeMade = false;
    }

    if (!treeMade)
    {
        tree = new octree(glm::vec3(0.0), 1000.0);
        for (int i = 0; i < objects.size(); ++i)
        {
            objects[i].velocity = objects[i].visual.getPos() - objects[i].visual.getLastPosition();
            tree->insert(&objects[i]);
        }
        tree->insert(&plObject);

        treeMade = true;
    }
    for (int i = 0; i < objects.size(); ++i)
    {
        if (!objects[i].visual.isDynamic())
            continue;

        if (objects[i].visual.getPos() != objects[i].visual.getLastPosition() || objects[i].visual.getScale() != objects[i].visual.getLastScale())
        {
            if (objects[i].pOctree->deleteObject(&objects[i]))
                tree->insert(&objects[i]);
        }
    }
    if (plObject.visual.getPos() != plObject.visual.getLastPosition())
    {
        if (plObject.pOctree->deleteObject(&plObject))
        {
            tree->insert(&plObject);
        }
    }
    tree->cleanUpEmptyChildren();

    if (tree != nullptr)
    {
        tree->collisionTest(&plObject, on_floor);
    }

    if (!on_floor)
        plObject.velocity.y += 0.5 * gravity * tick_time;

    plObject.visual.refPos()->x += plObject.velocity.x * tick_time;
    plObject.visual.refPos()->y += plObject.velocity.y * tick_time;
    plObject.visual.refPos()->z += plObject.velocity.z * tick_time;

    plObject.velocity.x = 0.0;
    plObject.velocity.z = 0.0;
    if (!on_floor)
        plObject.velocity.y += 0.5 * gravity * tick_time;
}

void level::updateTriggerChecks(level_object &plObject, glm::vec3 &camDir)
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
            if (!triggers[i].triggered && colliding(plObject.collider, objects[triggers[i].objIndex].collider))
            {
                triggers[i].timerDown = triggers[i].time;
                triggers[i].triggered = true;
            }
            break;
        case TCAUSE_UI_HOVERED:
            if (!triggers[i].triggered && ui_objects[triggers[i].uiIndex].hovered)
            {
                triggers[i].timerDown = triggers[i].time;
                triggers[i].triggered = true;
            }
            break;
        case TCAUSE_UI_CLICKED:
            if (!triggers[i].triggered && ui_objects[triggers[i].uiIndex].clicked)
            {
                triggers[i].timerDown = triggers[i].time;
                triggers[i].triggered = true;
            }
            break;
        case TCAUSE_LOOKAT:
        {
            raycast ray = {plObject.visual.getPos() + glm::vec3(0.0, 0.5, 0.0), glm::normalize(camDir)};
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
void level::updateTriggerResponses(level_object &plObject, double tick_time)
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
                plObject.Put(spawnLocation);
                plObject.Put(spawnLocation);
            }
            break;
        case TTYPE_PLAYSOUND:
            if ((triggers[i].time == triggers[i].timerDown || triggers[i].time == 0.0))
            {
                audio_player->play_audio(triggers[i].audioID);
            }
            triggers[i].triggered = false;
            break;
        case TTYPE_STOPSOUND:
            if ((triggers[i].time == triggers[i].timerDown || triggers[i].time == 0.0))
            {
                audio_player->stop_audio(triggers[i].audioID);
            }
            triggers[i].triggered = false;
            break;
        case TTYPE_FADEINSOUND:
            audio_player->set_fade_in(triggers[i].audioID, triggers[i].time);
            triggers[i].triggered = false;
            break;
        case TTYPE_FADEOUTSOUND:
            audio_player->set_fade_out(triggers[i].audioID, triggers[i].time);
            triggers[i].triggered = false;
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

void octree::draw(shader &shad, double alpha_time)
{
#ifndef __EMSCRIPTEN__
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glLineWidth(12.0);
    visual.draw(shad, pixel_scale, alpha_time);
    for (int i = 0; i < 8; ++i)
    {
        if (pChild[i] != nullptr)
            pChild[i]->draw(shad, alpha_time);
    }
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
#endif
}