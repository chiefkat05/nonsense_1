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
    LCOMM_OBJECT,
    LCOMM_TRIGGER,
    LCOMM_VARIABLE,
    LCOMM_GLOBAL_VARIABLE,
    LCOMM_UI_OBJECT,
    LCOMM_AUDIO
};

struct octree;

struct level_object
{
    model_primitive visual;
    // std::vector<aabb> colliders;
    aabb collider;
    object_type type;
    unsigned int lineIndex = 0;
    glm::vec3 velocity = glm::vec3(0.0);
    level_object *pNextObject = nullptr;
    octree *pOctree = nullptr;

    void Put(glm::vec3 new_pos)
    {
        visual.Put(new_pos);
        collider.pos = new_pos;
    }
    void Put(double x, double y, double z)
    {
        visual.Put(x, y, z);
        collider.pos = glm::vec3(x, y, z);
    }
    void SetScale(glm::vec3 new_scale)
    {
        visual.Scale(new_scale);
        collider.scale = new_scale;
    }
};

constexpr int max_octree_depth = 40;
struct octree
{
    glm::vec3 center = glm::vec3(0.0);
    double halfwidth = 1000.0;
    octree *pChild[8] = {nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr};
    level_object *pObjList = nullptr;
    model_primitive visual;
    unsigned int depth = 0;
    bool leafnode = true;

    void clear()
    {
        for (int i = 0; i < 8; ++i)
        {
            if (pChild[i] == nullptr)
                continue;

            pChild[i]->clear();
            delete pChild[i];
            pChild[i] = nullptr;
        }
    }

    octree() {}
    octree(glm::vec3 pos, double hw) : center(pos), halfwidth(hw)
    {
        visual = model_primitive(MODEL_CUBE, true, true);
        visual.Put(pos);
        visual.Scale(hw, hw, hw);
        visual.Image(2);
        visual.SetColor(1.0, 1.0, 1.0, 1.0);
        visual.makeDynamic();
    }
    void draw(shader &shad, double alpha_time);
    void buildChildAtIndex(unsigned int index)
    {
        double step = halfwidth * 0.5;
        glm::vec3 offset = glm::vec3(-step, -step, -step);
        if (index & 1)
            offset.x = step;
        if (index & 2)
            offset.y = step;
        if (index & 4)
            offset.z = step;

        pChild[index] = new octree(center + offset * 0.5f, step);
        leafnode = false;
    }
    void insert(level_object *obj)
    {
        if (!colliding(obj->collider, aabb({center, glm::vec3(halfwidth)})))
        {
            return;
        }
        unsigned int index = 0;
        bool straddle = false;
        // ++depth;
        // if (depth >= max_octree_depth) // tis is just defense against low max_depth levels. Please lower the max depth and re-implement this when you're done testing!
        // {
        //     straddle = true;
        //     obj->pNextObject = pObjList;
        //     pObjList = obj;
        //     --depth;
        //     return;
        // }

        glm::vec3 offset = glm::vec3(0.0);
        for (int i = 0; i < 3; ++i)
        {
            double delta = obj->visual.getPos()[i] - center[i];
            if (std::abs(delta) <= obj->visual.getScale()[i])
            {
                straddle = true;
                break;
            }
            if (delta > 0.0)
                index |= (1 << i);
        }
        if (!straddle)
        {
            if (pChild[index] == nullptr)
            {
                buildChildAtIndex(index);
            }
            pChild[index]->insert(obj);
        }
        else
        {
            obj->pNextObject = pObjList;
            pObjList = obj;
            obj->pOctree = this;
            // --depth;
        }
    }
    void collisionTest(level_object *pPlayer, bool &on_floor);

    bool deleteObject(level_object *deletionObject)
    {
        // first case
        if (pObjList == deletionObject)
        {
            pObjList = pObjList->pNextObject;
            return true;
        }
        return false;
    }

    void cleanUpEmptyChildren()
    {
        bool anyChildren = false;
        for (int i = 0; i < 8; ++i)
        {
            if (pChild[i] != nullptr) // only delete if leafnode and no objlst
            {
                anyChildren = true;
                pChild[i]->cleanUpEmptyChildren();
                if (pChild[i]->leafnode && pChild[i]->pObjList == nullptr)
                {
                    delete pChild[i];
                    pChild[i] = nullptr;
                }
            }
        }
        if (!anyChildren)
        {
            leafnode = true;
        }
    }
};

struct level_trigger // I think there could be a less complicated way of doing this
{
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
    unsigned int lineIndex = 0;

    level_trigger(unsigned int _oi, unsigned int _ui, unsigned int _vci, double _vvc, trigger_cause_type _ct, trigger_type _t)
        : objIndex(_oi), uiIndex(_ui), varCheckIndex(_vci), varValueCompare(_vvc), ctype(_ct), type(_t) {}
    level_trigger() {}

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
    unsigned int lineIndex = 0;
    bool is_global = false;
};

struct ui_object
{
    model_primitive visual;
    aabb2d collider;
    unsigned int lineIndex = 0;
    glm::vec2 truepos = glm::vec2(0.0), truescale = glm::vec2(1.0);
    bool held = false, released = false, clicked = false, hovered;

    void updateSize(bool window_resized);
    void updateButtonState();
};

class level
{
private:
    std::vector<level_object> objects;
    std::vector<level_variable> variables;
    std::vector<level_trigger> triggers;
    std::vector<ui_object> ui_objects;

    bool triggerGameStartedCheck = false;
    unsigned int lineCount = 0;

    double gravity = -3.1;
    octree *tree = nullptr;
    bool treeMade = false;
    // octree tree;

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
    inline level_object *getObjectAtIndex(unsigned int index)
    {
        return &objects[index];
    }
    inline ui_object *getUIAtIndex(unsigned int index)
    {
        return &ui_objects[index];
    }
    inline level_trigger *getTriggerAtIndex(unsigned int index)
    {
        return &triggers[index];
    }
    inline unsigned int getLineCount()
    {
        return lineCount;
    }
    void addLine()
    {
        ++lineCount;
    }
    void deleteOctree()
    {
        if (tree == nullptr)
            return;
        tree->clear();
        delete tree;
        tree = nullptr;
    }

    void addObject(model_primitive_type model_type, std::string mpath, std::string matdir, glm::vec3 pos, glm::vec3 scale, unsigned int texture, object_type type, bool visible = true);
    void removeObjectAtIndex(unsigned int index);
    void addVariable(std::string id, double value, bool global);
    void addUIObject(glm::vec2 pos, glm::vec2 scale, unsigned int texture);
    void addAudio(std::string id, std::string path);

    void addTriggerObjectCheck(unsigned int objIndex, trigger_cause_type tct, trigger_type tt);
    void addTriggerVariableCheck(unsigned int varIndex, double varValue, trigger_cause_type tct, trigger_type tt);
    void addTriggerUICheck(unsigned int uiIndex, trigger_cause_type tct, trigger_type tt);
    void addTrigger(trigger_cause_type tct, trigger_type tt);
    void setTriggerObjectResponse(unsigned int objIndex, glm::vec3 pos, double time);
    void setTriggerVariableResponse(unsigned int varUpdIndex, double varUpdValue, double time);
    void setTriggerAudioResponse(std::string audioID, double time);
    void setTriggerLevelResponse(std::string newLevel, double time);

    void reset();
    void drawLevel(shader &shad, shader &shad_ui, bool debugMode, double alpha);
    void updatePlayerPhysics(double tick_time, level_object &plObject, bool &on_floor);
    void updateTriggerChecks(level_object &plObject, glm::vec3 &camDir);
    void updateTriggerResponses(level_object &plObject, double tick_time);
};

class game
{
private:
    level current_level;
    std::vector<level_variable> global_variables; // will maybe need to be inserted and taken out of level whenever level changed so level can access the variables to

public:
    void setup_level(const char *level_path);

    void update_level(double tick_time, level_object &plObject, glm::vec3 camDir, bool &onG, bool debug = false);
    void draw_level(shader &shad, shader &shad_ui, bool debugMode, double alpha);
    std::string current_level_path = "";

    inline level *getCurrentLevel()
    {
        return &current_level;
    }
};

#endif