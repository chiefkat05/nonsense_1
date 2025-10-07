#ifndef MODEL_HXX
#define MODEL_HXX

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#define TINYOBJLOADER_IMPLEMENTATION
#include "../deps/tiny_obj_loader.h"

// struct vertex // bring this back? At some point
// {
//     // pos = 3, normals = 3, texture = 2
//     float data[8] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
// };
enum vetrtex_data_type
{
    VERTEX_POSITION,
    VERTEX_NORMAL,
    VERTEX_TEXTURE_POSITION
};

class model_data
{
private:
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string model_error_return, model_warning_return;
    tinyobj::attrib_t attrib;

public:
    std::vector<float> data;
    bool loaded = false;

    model_data(const char *obj_path, const char *material_path)
    {
        tinyobj::LoadObj(&attrib, &shapes, &materials, &model_warning_return, &model_error_return, obj_path, material_path, true, true);

        if (shapes.size() == 0)
        {
            return;
        }
        for (int o = 0; o < shapes.size(); ++o)
        {
            for (int i = 0; i < shapes[o].mesh.indices.size() / 3; ++i)
            {
                tinyobj::index_t tri_index_0 = shapes[o].mesh.indices[i * 3];
                tinyobj::index_t tri_index_1 = shapes[o].mesh.indices[i * 3 + 1];
                tinyobj::index_t tri_index_2 = shapes[o].mesh.indices[i * 3 + 2];

                float textureArray[3][2];

                textureArray[0][0] = attrib.texcoords[2 * tri_index_0.texcoord_index];
                textureArray[0][1] = attrib.texcoords[2 * tri_index_0.texcoord_index + 1];
                textureArray[1][0] = attrib.texcoords[2 * tri_index_1.texcoord_index];
                textureArray[1][1] = attrib.texcoords[2 * tri_index_1.texcoord_index + 1];
                textureArray[2][0] = attrib.texcoords[2 * tri_index_2.texcoord_index];
                textureArray[2][1] = attrib.texcoords[2 * tri_index_2.texcoord_index + 1];

                float vertexArray[3][3];
                for (int j = 0; j < 3; ++j)
                {
                    vertexArray[0][j] = attrib.vertices[3 * tri_index_0.vertex_index + j];
                    vertexArray[1][j] = attrib.vertices[3 * tri_index_1.vertex_index + j];
                    vertexArray[2][j] = attrib.vertices[3 * tri_index_2.vertex_index + j];
                }

                float normalArray[3][3];
                for (int j = 0; j < 3; ++j)
                {
                    normalArray[0][j] = attrib.normals[3 * tri_index_0.normal_index + j];
                    normalArray[1][j] = attrib.normals[3 * tri_index_1.normal_index + j];
                    normalArray[2][j] = attrib.normals[3 * tri_index_2.normal_index + j];
                }
                for (int j = 0; j < 3; ++j)
                {
                    data.push_back(vertexArray[j][0]);
                    data.push_back(vertexArray[j][1]);
                    data.push_back(vertexArray[j][2]);
                    data.push_back(textureArray[j][0]);
                    data.push_back(textureArray[j][1]);
                    data.push_back(normalArray[j][0]);
                    data.push_back(normalArray[j][1]);
                    data.push_back(normalArray[j][2]);
                }
            }
        }

        // std::cout << "wow << " << std::endl;
        // std::cout << model_error_return << ", " << model_warning_return << " wow!\n";

        loaded = true;
    }
};

#endif