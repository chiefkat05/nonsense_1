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
enum trigger_type
{
    TTYPE_MOVEOBJ,
    TTYPE_SCALEOBJ,
    TTYPE_ROTATEOBJ,
    TTYPE_CHANGELVL,
    TTYPE_SETVARIABLE,
    TTYPE_ADDVARIABLE,
    TTYPE_SUBTRACTVARIABLE,
    TTYPE_PLAYSOUND,
    TTYPE_STOPSOUND,
    TTYPE_FADEINSOUND,
    TTYPE_FADEOUTSOUND
};
enum trigger_cause_type
{
    TCAUSE_STARTGAME,
    TCAUSE_COLLISION,
    TCAUSE_VARIABLEEQUAL,
    TCAUSE_VARIABLEGREATER,
    TCAUSE_VARIABLELESSER,
    TCAUSE_LOOKAT,
    TCAUSE_UI_CLICKED,
    TCAUSE_UI_HOVERED
};
enum level_command_type
{
    LCOMM_NONE,
    LCOMM_PRIMITIVE,
    LCOMM_PRIMITIVE_PIXELPOS,
    LCOMM_TRIGGER,
    LCOMM_VARIABLE,
    LCOMM_GLOBAL_VARIABLE,
    LCOMM_UI_OBJECT,
    LCOMM_AUDIO
};

struct level_object
{
    model_primitive visual;
    aabb collider;
    object_type type;
};

// struct trigger_obj_response
// {
//     unsigned int obj_index = 0;
//     glm::vec3 pos
// };

struct level_trigger // I think there could be a less complicated way of doing this
{                    // you know you'll have to revamp the trigger system at some point lmao
    bool triggered = false;
    unsigned int objIndex = 0;
    unsigned int uiIndex = 0;
    unsigned int varCheckIndex = 0, varUpdIndex = 0;
    std::string audioID = "";
    double varValueCompare = 0.0, varUpdValue = 0.0;
    trigger_cause_type ctype = TCAUSE_STARTGAME;
    trigger_type type = TTYPE_MOVEOBJ;
    glm::vec3 pos = glm::vec3(0.0);
    double time = 0.0, timerDown = 0.0;
    std::string newLevel = "";

    level_trigger(unsigned int _oi, unsigned int _ui, unsigned int _vci, double _vvc, trigger_cause_type _ct, trigger_type _t)
        : objIndex(_oi), uiIndex(_ui), varCheckIndex(_vci), varValueCompare(_vvc), ctype(_ct), type(_t) {}

    void setObjResponse(unsigned int _oi, glm::vec3 _p, double _t)
    {
        objIndex = _oi;
        pos = _p;
        time = _t;
    }
    void setVariableResponse(unsigned int _vui, double _vuv, double _t)
    {
        varUpdIndex = _vui;
        varUpdValue = _vuv;
        time = _t;
    }
    void setLevelResponse(std::string _l, double _t)
    {
        newLevel = _l;
        time = _t;
    }
    void setAudioResponse(std::string _a, double _t)
    {
        audioID = _a;
        time = _t;
    }
};

struct level_variable
{
    std::string strID = "null_variable";
    double value = 0.0;
};

struct ui_object
{
    model_primitive visual;
    aabb2d collider;
};

class level
{
private:
    std::vector<level_object> objects;
    std::vector<level_variable> variables;
    std::vector<level_trigger> triggers;
    std::vector<ui_object> ui_objects;

    bool triggerGameStartedCheck = false;

    double gravity = -9.81;

public:
    std::string setLevel = "";

    inline unsigned int getObjectCount()
    {
        return objects.size();
    }
    inline unsigned int getTriggerCount()
    {
        return triggers.size();
    }
    inline unsigned int getVariableCount()
    {
        return variables.size();
    }
    inline unsigned int getUICount()
    {
        return ui_objects.size();
    }
    inline level_variable *getVariableAtIndex(unsigned int index)
    {
        return &variables[index];
    }

    void addObject(model_primitive_type model_type, glm::vec3 pos, glm::vec3 scale, unsigned int texture, object_type type, bool visible = true);
    // void addTriggerObject(unsigned int objIndex, trigger_cause_type tct, trigger_type tt, glm::vec3 pos, double time);
    // void addTriggerObject(unsigned int objIndex, trigger_cause_type tct, trigger_type tt, unsigned int updvariable_index, double updvariable_value, double time);
    // void addTriggerObject(unsigned int objIndex, trigger_cause_type tct, trigger_type tt, std::string trigger_set_level, double time);
    // void addTriggerObject(unsigned int objIndex, trigger_cause_type tct, trigger_type tt, std::string audio_id, double time);
    void addTriggerObjectCheck(unsigned int objIndex, trigger_cause_type tct, trigger_type tt);
    void addTriggerVariableCheck(unsigned int varIndex, double varValue, trigger_cause_type tct, trigger_type tt);
    void addTriggerUICheck(unsigned int uiIndex, trigger_cause_type tct, trigger_type tt);
    void addTrigger(trigger_cause_type tct, trigger_type tt);
    void setTriggerObjectResponse(unsigned int objIndex, glm::vec3 pos, double time);
    void setTriggerVariableResponse(unsigned int varUpdIndex, double varUpdValue, double time);
    void setTriggerAudioResponse(std::string audioID, double time);
    void setTriggerLevelResponse(std::string newLevel, double time);

    void addVariable(std::string id, double value);
    // void addTriggerVariable(unsigned int objIndex, unsigned int varIndex, double varValue, trigger_cause_type tct, trigger_type tt, glm::vec3 pos, double time);
    // void addTriggerVariable(unsigned int varIndex, double varValue, trigger_cause_type tct, trigger_type tt, unsigned int updvariable_index, double updvariable_value, double time);
    // void addTriggerVariable(unsigned int varIndex, double varValue, trigger_cause_type tct, trigger_type tt, std::string trigger_set_level, double time);

    void addUIObject(glm::vec2 pos, glm::vec2 scale, unsigned int texture);
    // void addTriggerUI(unsigned int uiIndex, unsigned int objIndex, trigger_cause_type tct, trigger_type tt, glm::vec3 pos, double time);
    // void addTriggerUI(unsigned int uiIndex, trigger_cause_type tct, trigger_type tt, unsigned int updvariable_index, double updvariable_value, double time);
    // void addTriggerUI(unsigned int uiIndex, trigger_cause_type tct, trigger_type tt, std::string trigger_set_level, double time);

    void scaleObject(glm::vec3 scale, unsigned int index);

    void reset();
    void drawLevel(shader &shad, shader &shad_ui, double alpha);
    void updatePlayerPhysics(double tick_time, glm::vec3 &player_position, glm::vec3 &player_last_position, glm::vec3 &player_velocity, aabb &player_collider, bool &on_floor);
    void updateTriggerChecks(aabb &playerCollider, glm::vec3 &camPos, glm::vec3 &camDir, glm::vec2 &mousePos, bool &mouseClicked);
    void updateTriggerResponses(double tick_time);
};

class game
{
private:
    level current_level;
    std::vector<level_variable> global_variables; // will maybe need to be inserted and taken out of level whenever level changed so level can access the variables too
    unsigned int global_variable_count = 0;

public:
    void setup_level(const char *level_path);

    void update_level(double tick_time, glm::vec3 &plPos, glm::vec3 &plLastPos, glm::vec3 &plVel, glm::vec2 &mousePos, bool &mouseClicked, aabb &plCol, glm::vec3 camDir, bool &onG);
    void draw_level(shader &shad, shader &shad_ui, double alpha);
};

#endif