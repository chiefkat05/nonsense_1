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
    TTYPE_SUBTRACTVARIABLE
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
    LCOMM_UI_OBJECT
};

struct level_object
{
    model_primitive visual;
    aabb collider;
    object_type type;
};

struct level_trigger // I think there could be a less complicated way of doing this
{                    // you know you'll have to revamp the trigger system at some point lmao
    bool triggered = false;
    unsigned int objIndex = 0;
    unsigned int uiIndex = 0;
    unsigned int varCheckIndex = 0, varUpdIndex = 0;
    double varValueCompare = 0.0, varUpdValue = 0.0;
    trigger_cause_type ctype = TCAUSE_STARTGAME;
    trigger_type type = TTYPE_MOVEOBJ;
    glm::vec3 pos = glm::vec3(0.0);
    double time = 0.0, timerDown = 0.0;
    std::string newLevel = "";

    level_trigger(unsigned int _oi, unsigned int _ui, unsigned int _vci, unsigned int _vui,
                  double _vvc, double _vuv, trigger_cause_type _ct, trigger_type _tt,
                  glm::vec3 _p, double _t, std::string _nl) : objIndex(_oi), uiIndex(_ui), varCheckIndex(_vci), varUpdIndex(_vui),
                                                              varValueCompare(_vvc), varUpdValue(_vuv), ctype(_ct), type(_tt),
                                                              pos(_p), time(_t), newLevel(_nl) {}

    // lmao

    // check-object trigger-object
    // level_trigger(unsigned int _objIndex, trigger_cause_type _ctype, trigger_type _type, glm::vec3 _pos, double _time)
    //     : objIndex(_objIndex), ctype(_ctype), type(_type), pos(_pos), time(_time) {}
    // // check-object trigger-variable
    // level_trigger(unsigned int _objIndex, trigger_cause_type _ctype, trigger_type _type, unsigned int _varUpdIndex, double _varUpdValue, double _time)
    //     : objIndex(_objIndex), ctype(_ctype), type(_type), varUpdIndex(_varUpdIndex), varUpdValue(_varUpdValue), time(_time) {}
    // // check-object trigger-level
    // level_trigger(unsigned int _objIndex, trigger_cause_type _ctype, trigger_type _type, std::string _newLevel, double _time)
    //     : objIndex(_objIndex), ctype(_ctype), type(_type), newLevel(_newLevel), time(_time) {}

    // // check-variable trigger-object
    // level_trigger(unsigned int _objIndex, unsigned int _varIndex, double _varValue, trigger_cause_type _ctype, trigger_type _type, glm::vec3 _pos, double _time)
    //     : objIndex(_objIndex), varCheckIndex(_varIndex), varValueCompare(_varValue), ctype(_ctype), type(_type), pos(_pos), time(_time) {}
    // // check-variable trigger-variable
    // level_trigger(unsigned int _varIndex, double _varValue, trigger_cause_type _ctype, trigger_type _type, unsigned int _varUpdIndex, double _varUpdValue, double _time)
    //     : varCheckIndex(_varIndex), varValueCompare(_varValue), ctype(_ctype), type(_type), varUpdIndex(_varUpdIndex), varUpdValue(_varUpdValue), time(_time) {}
    // // check-variable trigger-level
    // level_trigger(unsigned int _varIndex, double _varValue, trigger_cause_type _ctype, trigger_type _type, std::string _newLevel, double _time)
    //     : varCheckIndex(_varIndex), varValueCompare(_varValue), ctype(_ctype), type(_type), newLevel(_newLevel), time(_time) {}

    // // check-ui trigger-object
    // level_trigger(unsigned int _uiIndex, unsigned int _objIndex, trigger_cause_type _ctype, trigger_type _type, glm::vec3 _pos, double _time)
    //     : uiIndex(_uiIndex), objIndex(_objIndex), ctype(_ctype), type(_type), pos(_pos), time(_time) {}
    // // check-ui trigger-variable
    // level_trigger(unsigned int _uiIndex, trigger_cause_type _ctype, trigger_type _type, unsigned int _varUpdIndex, double _varUpdValue, double _time)
    //     : uiIndex(_uiIndex), ctype(_ctype), type(_type), varUpdIndex(_varUpdIndex), varUpdValue(_varUpdValue), time(_time) {}
    // // check-ui trigger-level
    // level_trigger(unsigned int _uiIndex, trigger_cause_type _ctype, trigger_type _type, std::string _newLevel, double _time)
    //     : uiIndex(_uiIndex), ctype(_ctype), type(_type), newLevel(_newLevel), time(_time) {}

    // another lmao
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
    unsigned int object_count = 0;
    std::vector<level_variable> variables;
    unsigned int variable_count = 0;
    std::vector<level_trigger> triggers;
    unsigned int trigger_count = 0;
    std::vector<ui_object> ui_objects;
    unsigned int ui_object_count = 0;

    bool triggerGameStartedCheck = false;

    double gravity = -9.81;

public:
    std::string setLevel = "";

    inline unsigned int getObjectCount()
    {
        return object_count;
    }
    inline unsigned int getTriggerCount()
    {
        return trigger_count;
    }
    inline unsigned int getVariableCount()
    {
        return variable_count;
    }
    inline unsigned int getUICount()
    {
        return ui_object_count;
    }
    inline level_variable *getVariableAtIndex(unsigned int index)
    {
        return &variables[index];
    }

    void addObject(model_primitive_type model_type, glm::vec3 pos, glm::vec3 scale, unsigned int texture, object_type type, bool visible = true);
    void addTriggerObject(unsigned int objIndex, trigger_cause_type tct, trigger_type tt, glm::vec3 pos, double time);
    void addTriggerObject(unsigned int objIndex, trigger_cause_type tct, trigger_type tt, unsigned int updvariable_index, double updvariable_value, double time);
    void addTriggerObject(unsigned int objIndex, trigger_cause_type tct, trigger_type tt, std::string trigger_set_level, double time);

    void addVariable(std::string id, double value);
    void addTriggerVariable(unsigned int objIndex, unsigned int varIndex, double varValue, trigger_cause_type tct, trigger_type tt, glm::vec3 pos, double time);
    void addTriggerVariable(unsigned int varIndex, double varValue, trigger_cause_type tct, trigger_type tt, unsigned int updvariable_index, double updvariable_value, double time);
    void addTriggerVariable(unsigned int varIndex, double varValue, trigger_cause_type tct, trigger_type tt, std::string trigger_set_level, double time);

    void addUIObject(glm::vec2 pos, glm::vec2 scale, unsigned int texture);
    void addTriggerUI(unsigned int uiIndex, unsigned int objIndex, trigger_cause_type tct, trigger_type tt, glm::vec3 pos, double time);
    void addTriggerUI(unsigned int uiIndex, trigger_cause_type tct, trigger_type tt, unsigned int updvariable_index, double updvariable_value, double time);
    void addTriggerUI(unsigned int uiIndex, trigger_cause_type tct, trigger_type tt, std::string trigger_set_level, double time);

    void scaleObject(glm::vec3 scale, unsigned int index);

    void reset();
    void drawLevel(shader &shad, shader &shad_ui, double alpha);
    void updatePlayerPhysics(double tick_time, glm::vec3 &player_position, glm::vec3 &player_last_position, glm::vec3 &player_velocity, aabb &player_collider, bool &on_floor);
    void updateTriggerChecks(aabb &playerCollider, glm::vec3 &camPos, glm::vec3 &camDir, glm::vec2 &mousePos, bool &mouseClicked);
    void updateTriggerPhysics(double tick_time);
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