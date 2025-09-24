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
    model_primitive visual;
    aabb collider;
    object_type type;
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
    TCAUSE_LOOKAT
};
enum level_command_type
{
    LCOMM_NONE,
    LCOMM_PRIMITIVE,
    LCOMM_PRIMITIVE_PIXELPOS,
    LCOMM_TRIGGER,
    LCOMM_VARIABLE
};

struct level_trigger // I think there could be a less complicated way of doing this
{
    bool triggered = false;
    unsigned int objIndex = 0, varCheckIndex = 0, varUpdIndex = 0;
    double varValueCompare = 0.0, varUpdValue = 0.0;
    trigger_cause_type ctype = TCAUSE_STARTGAME;
    trigger_type type = TTYPE_MOVEOBJ;
    glm::vec3 pos = glm::vec3(0.0);
    double time = 0.0, timerDown = 0.0;

    // lmao

    // check-object trigger-object
    level_trigger(unsigned int _objIndex, trigger_cause_type _ctype, trigger_type _type, glm::vec3 _pos, double _time)
        : objIndex(_objIndex), ctype(_ctype), type(_type), pos(_pos), time(_time) {}
    // check-object trigger-variable
    level_trigger(unsigned int _objIndex, trigger_cause_type _ctype, trigger_type _type, unsigned int _varUpdIndex, double _varUpdValue, double _time)
        : objIndex(_objIndex), ctype(_ctype), type(_type), varUpdIndex(_varUpdIndex), varUpdValue(_varUpdValue), time(_time) {}

    // check-variable trigger-object
    level_trigger(unsigned int _objIndex, unsigned int _varIndex, double _varValue, trigger_cause_type _ctype, trigger_type _type, glm::vec3 _pos, double _time)
        : objIndex(_objIndex), varCheckIndex(_varIndex), varValueCompare(_varValue), ctype(_ctype), type(_type), pos(_pos), time(_time) {}
    // check-variable trigger-variable (incomplete)
    level_trigger(unsigned int _objIndex, unsigned int _varIndex, double _varValue, trigger_cause_type _ctype, trigger_type _type, unsigned int _varUpdIndex, double _varUpdValue, double _time)
        : objIndex(_objIndex), varCheckIndex(_varIndex), varValueCompare(_varValue), ctype(_ctype), type(_type), varUpdIndex(_varUpdIndex), varUpdValue(_varUpdValue), time(_time) {}
};

struct level_variable
{
    std::string strID = "null_variable";
    double value = 0.0;
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

    bool triggerGameStartedCheck = false;

    double gravity = -9.81;

public:
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

    void addObject(model_primitive_type model_type, glm::vec3 pos, glm::vec3 scale, unsigned int texture, object_type type, bool visible = true);
    void addTriggerObject(unsigned int objIndex, trigger_cause_type tct, trigger_type tt, glm::vec3 pos, double time);
    void addTriggerObject(unsigned int objIndex, trigger_cause_type tct, trigger_type tt, unsigned int updvariable_index, double updvariable_value, double time);
    void addVariable(std::string id, double value);
    void addTriggerVariable(unsigned int objIndex, unsigned int varIndex, double varValue, trigger_cause_type tct, trigger_type tt, glm::vec3 pos, double time);
    void addTriggerVariable(unsigned int objIndex, unsigned int varIndex, double varValue, trigger_cause_type tct, trigger_type tt, unsigned int updvariable_index, double updvariable_value, double time);

    void scaleObject(glm::vec3 scale, unsigned int index);

    void drawLevel(shader &shad, double alpha);
    void updatePlayerPhysics(double tick_time, glm::vec3 &player_position, glm::vec3 &player_velocity, aabb &player_collider, bool &on_floor);
    void updateTriggerChecks(aabb &playerCollider, glm::vec3 &camPos, glm::vec3 &camDir);
    void updateTriggerPhysics(double tick_time);
};

class game
{
private:
    unsigned int level_id = 0;
    std::vector<level> levels;
    unsigned int level_count = 0;

public:
    void goto_level(unsigned int id);
    void setup_level(const char *level_path);

    void update(double tick_time, glm::vec3 &plPos, glm::vec3 &plVel, aabb &plCol, glm::vec3 camDir, bool &onG);
    void draw(shader &shad, double alpha);
};

#endif