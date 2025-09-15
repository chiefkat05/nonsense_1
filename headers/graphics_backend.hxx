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

enum transform_order_type
{
    TRANSFORM_DEFAULT,
    TRANSFORM_ROTATION_FIRST,
    TRANSFORM_SCALE_FIRST,
    TRANSFORM_ROTATION_AND_SCALE_FIRST,
    TRANSFORM_SCALE_AND_ROTATION_FIRST,
    TRANSFORM_SCALE_BEFORE_ROTATION
};

class cube
{
private:
    float vertices[288] = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f};

    unsigned int VBO, VAO;
    unsigned int textureIndex = 0;
    unsigned int *textureID = nullptr;

    glm::vec3 position = glm::vec3(0.0), scale = glm::vec3(1.0), rotation = glm::vec3(0.0);

public:
    glm::vec3 getPos()
    {
        return position;
    }
    glm::vec3 getScale()
    {
        return scale;
    }

    void Put(glm::vec3 pos);
    void Put(double x = 0.0, double y = 0.0, double z = 0.0);
    void Scale(glm::vec3 scl);
    void Scale(double x = 0.0, double y = 0.0, double z = 0.0);
    void Rotate(glm::vec3 rot);
    void Rotate(double x = 0.0, double y = 0.0, double z = 0.0);

    void Image(unsigned int index);

    void draw(shader &_shader, transform_order_type transformOrder = TRANSFORM_DEFAULT);

    cube();
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