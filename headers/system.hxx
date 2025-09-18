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
class level
{
private:
    std::vector<level_object> objects;
    unsigned int object_count = 0;

    double gravity = -9.81;

public:
    void addObject(model_primitive_type model_type, glm::vec3 pos, glm::vec3 scale, unsigned int texture, object_type type);
    void placeObject(glm::vec3 pos, unsigned int index);
    void moveObject(glm::vec3 distance, unsigned int index);

    void scaleObject(glm::vec3 scale, unsigned int index);

    void drawLevel(shader &shad, double alpha);
    void updatePhysics(double tick_time, glm::vec3 &player_position, glm::vec3 &player_velocity, aabb &player_collider, bool &on_floor);
};

enum update_command_types
{
    LVLUPD_MOVEOBJ,
    LVLUPD_SCALEOBJ,
    LVLUPD_ROTATEOBJ
};
enum level_command_types
{
    LCOMM_NONE,
    LCOMM_PRIMITIVE,
    LCOMM_PRIMITIVE_PIXELPOS,
    LCOMM_MOVE,
    LCOMM_SCALE,
    LCOMM_ROTATE
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

    void update(double tick_time, glm::vec3 &plPos, glm::vec3 &plVel, aabb &plCol, bool &onG);
    void draw(shader &shad, double alpha);
};

#endif