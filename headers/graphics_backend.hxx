#ifndef SHADER_HXX
#define SHADER_HXX

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#ifdef __EMSCRIPTEN__
#include <emscripten/emscripten.h>
#include <GL/glew.h>
// #include <GL/gl.h>
#else
#include "../glad/include/glad/gl.h"
#endif

#include <GLFW/glfw3.h>

#include "/usr/include/glm/glm.hpp"
#include "/usr/include/glm/gtc/matrix_transform.hpp"
#include "/usr/include/glm/gtc/type_ptr.hpp"
#include "/usr/include/glm/gtc/quaternion.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "/usr/include/glm/gtx/quaternion.hpp"
#include "/usr/include/glm/gtx/euler_angles.hpp"

void graphics_init();

class shader
{
private:
    unsigned int id;

public:
    shader(std::string vPath, std::string fPath);

    void use();

    void setMat4(const char *name, glm::mat4 v);
    void setVec4(const char *name, glm::vec4 v);
    void setVec3(const char *name, glm::vec3 v);
    void setVec2(const char *name, glm::vec2 v);
    void setDouble(const char *name, double v);
    void setInt(const char *name, int v);
};

// Okay you're not actually going to implement all this are you
enum model_primitive_type
{
    MODEL_NONE,
    MODEL_TRI,
    MODEL_QUAD,
    MODEL_CUBE,
    MODEL_SPHERE,
    MODEL_CONE,
    MODEL_CYLINDER,
    MODEL_CAPSULE,
    MODEL_PYRAMID
};
class model_primitive
{
private:
    unsigned int VBO, VAO;
    unsigned int textureIndex = 0;
    unsigned int *textureID = nullptr;
    model_primitive_type mtype = MODEL_NONE;
    unsigned int vertex_count = 0;
    bool dynamic_model = false, static_model_set = false, is_visible = true;
    glm::mat4 model = glm::mat4(1.0);

    glm::vec3 position = glm::vec3(0.0), scale = glm::vec3(1.0);
    glm::vec3 last_position = glm::vec3(0.0), last_scale = glm::vec3(1.0);
    glm::quat rotation = glm::quat(glm::vec3(0.0));
    glm::quat last_rotation = glm::quat(glm::vec3(0.0));

public:
    glm::vec3 rotation_direction = glm::vec3(0.0);
    glm::vec3 getPos()
    {
        return position;
    }
    glm::vec3 getScale()
    {
        return scale;
    }
    inline glm::quat getRotation()
    {
        return rotation;
    }
    inline void makeDynamic()
    {
        dynamic_model = true;
    }
    inline void makeStatic()
    {
        dynamic_model = false;
    }
    inline bool isDynamic()
    {
        return dynamic_model;
    }
    inline glm::vec3 getLastPosition()
    {
        return last_position;
    }

    void Put(glm::vec3 pos = glm::vec3(0.0));
    void Put(double x = 0.0, double y = 0.0, double z = 0.0);
    void Move(glm::vec3 pos = glm::vec3(0.0));
    void Move(double x = 0.0, double y = 0.0, double z = 0.0);
    void Scale(glm::vec3 scl = glm::vec3(1.0));
    void Scale(double x = 1.0, double y = 1.0, double z = 1.0);
    void Rotate(glm::vec3 rot = glm::vec3(0.0));
    void Rotate(double x = 0.0, double y = 0.0, double z = 0.0);

    void Image(unsigned int index);

    void draw(shader &_shader, double p_scale, double alpha);
    void static_draw(shader &_shader, double p_scale);

    model_primitive(model_primitive_type type, bool dyn = false, bool visible = true);
};

class texture
{
private:
    unsigned int id;
    unsigned int pixel_width, pixel_height;

public:
    void Set(unsigned int index, const char *path);
    unsigned int getWidth();
    unsigned int getHeight();
    glm::ivec2 getSize();
};

class texturegroup
{
private:
    std::vector<texture> textures;
    unsigned int texture_count = 0;

public:
    texture *getTextureAtIndex(unsigned int index);
    void addTexture(unsigned int index, const char *path);
};

#endif