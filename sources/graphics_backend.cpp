#include "../headers/graphics_backend.hxx"

#define STB_IMAGE_IMPLEMENTATION
#include "../stb/stb_image.h"

extern texturegroup allTextures;
float pixel_scale = 16.0;

float cube_vertices[288] = {
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

float quad_vertices[48] = {
    // vert.x y z       tex.x tex.y normals.x y z
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
float tri_vertices[24] = {
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f};
float pyramid_vertices[144] = {
    // floor
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,

    // edges
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, -1.0f,
    0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, -1.0f,
    0.0f, 0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 1.0f, -1.0f,

    0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
    0.0f, 0.5f, 0.0f, 0.5f, 1.0f, 1.0f, 1.0f, 0.0f,

    0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    -0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
    0.0f, 0.5f, 0.0f, 0.5f, 1.0f, 0.0f, 1.0f, 1.0f,

    -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, 0.0f, -1.0f, 1.0f, 0.0f,
    0.0f, 0.5f, 0.0f, 0.5f, 1.0f, -1.0f, 1.0f, 0.0f};

void graphics_init()
{
    stbi_set_flip_vertically_on_load(true);
}

std::string readFile(std::string path)
{
    std::ifstream input(path);
    if (!input.is_open())
    {
        std::cout << "File error: couldn't read path \"" << path << "\" ." << std::endl;
        return "";
    }

    std::string line = "";
    std::string output = "";
    while (std::getline(input, line))
    {
        output.append(line);
        output.append("\n");
    }
    return output;
}

shader::shader(std::string vPath, std::string fPath)
{
    std::string vS = readFile(vPath);
    std::string fS = readFile(fPath);
    const char *vSource = vS.c_str();
    const char *fSource = fS.c_str();
    unsigned int vertexShader, fragmentShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vSource, NULL);
    glCompileShader(vertexShader);

    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "vertex shader error: " << infoLog << std::endl;
        return;
    }
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fSource, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "fragment shader error: " << infoLog << std::endl;
        return;
    }

    id = glCreateProgram();
    glAttachShader(id, vertexShader);
    glAttachShader(id, fragmentShader);
    glLinkProgram(id);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void shader::use()
{
    glUseProgram(id);
}

void shader::setMat4(const char *name, glm::mat4 v)
{
    glUseProgram(id);
    int loc = glGetUniformLocation(id, name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(v));
}
void shader::setVec4(const char *name, glm::vec4 v)
{
    glUseProgram(id);
    int loc = glGetUniformLocation(id, name);
    glUniform4f(loc, v.x, v.y, v.z, v.w);
}
void shader::setVec3(const char *name, glm::vec3 v)
{
    glUseProgram(id);
    int loc = glGetUniformLocation(id, name);
    glUniform3f(loc, v.x, v.y, v.z);
}
void shader::setVec2(const char *name, glm::vec2 v)
{
    glUseProgram(id);
    int loc = glGetUniformLocation(id, name);
    glUniform2f(loc, v.x, v.y);
}
void shader::setDouble(const char *name, double v)
{
    glUseProgram(id);
    int loc = glGetUniformLocation(id, name);
    glUniform1f(loc, v);
}
void shader::setInt(const char *name, int v)
{
    glUseProgram(id);
    int loc = glGetUniformLocation(id, name);
    glUniform1i(loc, v);
}

void model_primitive::Put(glm::vec3 pos)
{
    position = pos;
}
void model_primitive::Put(double x, double y, double z)
{
    position = glm::vec3(x, y, z);
}
void model_primitive::Scale(glm::vec3 scl)
{
    scale = scl;
}
void model_primitive::Scale(double x, double y, double z)
{
    scale = glm::vec3(x, y, z);
}
void model_primitive::Rotate(glm::vec3 rot)
{
    rotation = rot;
}
void model_primitive::Rotate(double x, double y, double z)
{
    rotation = glm::vec3(x, y, z);
}

void model_primitive::Image(unsigned int index)
{
    textureIndex = index;
}

void model_primitive::draw(shader &_shader, transform_order_type transformOrder)
{
    _shader.use();

    _shader.setInt("texture1", textureIndex);

    glm::mat4 model = glm::mat4(1.0f);
    switch (transformOrder)
    {
    case TRANSFORM_ROTATION_FIRST:
        model = glm::rotate(model, rotation.x, glm::vec3(1.0, 0.0, 0.0));
        model = glm::rotate(model, rotation.y, glm::vec3(0.0, 1.0, 0.0));
        model = glm::rotate(model, rotation.z, glm::vec3(0.0, 0.0, 1.0));
        model = glm::translate(model, position);
        model = glm::scale(model, scale);
        break;
    case TRANSFORM_SCALE_FIRST:
        model = glm::scale(model, scale);
        model = glm::translate(model, position);
        model = glm::rotate(model, rotation.x, glm::vec3(1.0, 0.0, 0.0));
        model = glm::rotate(model, rotation.y, glm::vec3(0.0, 1.0, 0.0));
        model = glm::rotate(model, rotation.z, glm::vec3(0.0, 0.0, 1.0));
        break;
    case TRANSFORM_ROTATION_AND_SCALE_FIRST:
        model = glm::rotate(model, rotation.x, glm::vec3(1.0, 0.0, 0.0));
        model = glm::rotate(model, rotation.y, glm::vec3(0.0, 1.0, 0.0));
        model = glm::rotate(model, rotation.z, glm::vec3(0.0, 0.0, 1.0));
        model = glm::scale(model, scale);
        model = glm::translate(model, position);
        break;
    case TRANSFORM_SCALE_AND_ROTATION_FIRST:
        model = glm::scale(model, scale);
        model = glm::rotate(model, rotation.x, glm::vec3(1.0, 0.0, 0.0));
        model = glm::rotate(model, rotation.y, glm::vec3(0.0, 1.0, 0.0));
        model = glm::rotate(model, rotation.z, glm::vec3(0.0, 0.0, 1.0));
        model = glm::translate(model, position);
        break;
    case TRANSFORM_SCALE_BEFORE_ROTATION:
        model = glm::translate(model, position);
        model = glm::scale(model, scale);
        model = glm::rotate(model, rotation.x, glm::vec3(1.0, 0.0, 0.0));
        model = glm::rotate(model, rotation.y, glm::vec3(0.0, 1.0, 0.0));
        model = glm::rotate(model, rotation.z, glm::vec3(0.0, 0.0, 1.0));
        break;
    default:
        model = glm::translate(model, position);
        model = glm::rotate(model, rotation.x, glm::vec3(1.0, 0.0, 0.0));
        model = glm::rotate(model, rotation.y, glm::vec3(0.0, 1.0, 0.0));
        model = glm::rotate(model, rotation.z, glm::vec3(0.0, 0.0, 1.0));
        model = glm::scale(model, scale);
        break;
    }
    // _shader.setDouble("texture_scale", std::max(std::max(scale.x, scale.y), scale.z));
    // to do pixel size you need texture x and y
    _shader.setVec3("texture_scale", scale * pixel_scale);
    _shader.setVec2("texture_pixel_scale", glm::vec2(1.0) / glm::vec2(allTextures.getTextureAtIndex(textureIndex)->getSize()));
    _shader.setMat4("model", model);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

model_primitive::model_primitive(model_primitive_type type)
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    switch (type)
    {
    case MODEL_NONE:
        std::cout << "What\n";
        break;
    case MODEL_QUAD:
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vertices), quad_vertices, GL_STATIC_DRAW);
        break;
    case MODEL_CUBE:
        glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);
        break;
    case MODEL_PYRAMID:
        glBufferData(GL_ARRAY_BUFFER, sizeof(pyramid_vertices), pyramid_vertices, GL_STATIC_DRAW);
        break;
    case MODEL_TRI:
        glBufferData(GL_ARRAY_BUFFER, sizeof(tri_vertices), tri_vertices, GL_STATIC_DRAW);
        break;
    default:
        std::cout << "Oh... sorry, that hasn't been implemented yet. Please try a different shape!\n";
    }

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
}

void texture::Set(unsigned int index, const char *path)
{
    glGenTextures(1, &id);
    glActiveTexture(GL_TEXTURE0 + index);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    int width, height, nrChannels;

    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture at path \"" << path << "\"." << std::endl;
    }
    stbi_image_free(data);

    pixel_width = width;
    pixel_height = height;
}
unsigned int texture::getWidth()
{
    return pixel_width;
}
unsigned int texture::getHeight()
{
    return pixel_height;
}
glm::ivec2 texture::getSize()
{
    return glm::ivec2(pixel_width, pixel_height);
}
texture *texturegroup::getTextureAtIndex(unsigned int index)
{
    if (index >= texture_count)
    {
        std::cout << "No texture at index " << index << "!" << std::endl;
        return nullptr;
    }

    return &textures[index];
}
void texturegroup::addTexture(unsigned int index, const char *path)
{
    texture newTexture;
    newTexture.Set(index, path);
    textures.push_back(newTexture);
    ++texture_count;
}