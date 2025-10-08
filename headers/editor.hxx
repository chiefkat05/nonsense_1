#ifndef EDITOR_HXX
#define EDITOR_HXX

#include <string>
#include <iostream>
#include <fstream>
#include "../headers/system.hxx"

extern game mainGame;
extern texturegroup allTextures;

class templevel
{
private:
    std::string path, temp_path;
    int editLine = -1;
    std::ifstream input;
    std::ofstream output;

    void copyFile(std::string fileinput, std::string fileoutput)
    {
        input.open(fileinput);
        if (!input.is_open())
        {
            std::cout << "Failed to open " << fileinput << std::endl;
            return;
        }

        output.open(fileoutput);

        if (!output.is_open())
        {
            std::cout << "Failed to open " << fileoutput << std::endl;
            return;
        }

        std::string line;
        while (std::getline(input, line))
        {
            output << line << "\n";
        }

        input.close();
        output.close();
    }

    level_object temp_object;
    level_trigger temp_trigger;
    level_variable temp_variable;
    ui_object temp_ui;

    std::vector<model_primitive> collider_visuals; // yessss

    unsigned int edit_line = 0;
    int updateNum = 0;
    int selectedObj = -1, selectedTrigger = -1, selectedVariable = -1, selectedUI = -1;

public:
    templevel(std::string _p) : path(_p), temp_path(_p + ".temp") {}
    templevel() : path(""), temp_path("") {}

    inline int getSelectedObj()
    {
        return selectedObj;
    }

    void copyMainToTemp()
    {
        copyFile(path, temp_path);
    }
    void saveTempToMain()
    {
        copyFile(temp_path, path);
    }
    void removeTemp()
    {
        if (temp_path != "" && std::ifstream(temp_path))
            std::remove(temp_path.c_str());
        else
            std::cout << "Error: file at " << temp_path << " cannot be deleted because it doesn't exist!\n";
    }
    void drawCollider(shader shad, double pscale, double alpha)
    {
        for (int i = 0; i < collider_visuals.size(); ++i)
        {
            collider_visuals[i].Put(mainGame.getCurrentLevel()->getObjectAtIndex(i)->collider.pos);
            collider_visuals[i].Scale(mainGame.getCurrentLevel()->getObjectAtIndex(i)->collider.scale);
            collider_visuals[i].SetColor(0.5, 0.0, 0.0, 0.5);
            if (i == selectedObj)
            {
                collider_visuals[i].SetColor(0.5, 0.0, 0.0, 0.9);
            }

            glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
            collider_visuals[i].draw(shad, pscale, alpha);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        if (selectedObj == -1)
            return;

        level_object *obj = mainGame.getCurrentLevel()->getObjectAtIndex(selectedObj);
    }

    void newObject()
    {
        mainGame.getCurrentLevel()->addObject(MODEL_CUBE, "", "", glm::vec3(0.0), glm::vec3(1.0), 0, OBJ_SOLID, true);
        std::ofstream output(temp_path, std::ios::app);
        if (!output.is_open())
        {
            std::cout << "New object error: failed to open " << temp_path << "\n";
            return;
        }
        output << "object cube 0.0 0.0 0.0 1.0 1.0 1.0 0 0 end";
        output.close();
        mainGame.getCurrentLevel()->addLine();

        level_object *obj = mainGame.getCurrentLevel()->getObjectAtIndex(mainGame.getCurrentLevel()->getObjectCount() - 1);
        obj->lineIndex = mainGame.getCurrentLevel()->getLineCount() - 1;
        obj->visual.makeDynamic();
        obj->visual.SetColor(0.5, 1.0, 0.5, 1.0);
    }
    void duplicateSelectedObject()
    {
        level_object *obj = mainGame.getCurrentLevel()->getObjectAtIndex(selectedObj);
        obj->visual.SetColor(1.0, 1.0, 1.0, 1.0);
        std::string holdMPath = obj->visual.getModelPath();
        std::string holdMatDir = obj->visual.getMaterialDir();
        mainGame.getCurrentLevel()->addObject(obj->visual.getType(), obj->visual.getModelPath(), obj->visual.getMaterialDir(), obj->visual.getPos(), obj->visual.getScale(), obj->visual.getImage(), obj->type, true);
        obj->visual.setModelPath(holdMPath);
        obj->visual.setMaterialDir(holdMatDir);
        std::ofstream output(temp_path, std::ios::app);
        if (!output.is_open())
        {
            std::cout << "New object error: failed to open " << temp_path << "\n";
            return;
        }
        if (obj->visual.getType() == MODEL_CUSTOM)
        {
            output << "object " << obj->visual.getModelPath() << " " << obj->visual.getMaterialDir()
                   << " " << std::to_string(obj->visual.getPos().x) << " "
                   << std::to_string(obj->visual.getPos().y) << " "
                   << std::to_string(obj->visual.getPos().z) << " "
                   << std::to_string(obj->visual.getScale().x) << " "
                   << std::to_string(obj->visual.getScale().y) << " "
                   << std::to_string(obj->visual.getScale().z) << " "
                   << std::to_string(obj->visual.getImage()) << " "
                   << std::to_string(obj->type) << " col "
                   << std::to_string(obj->collider.pos.x) << " "
                   << std::to_string(obj->collider.pos.y) << " "
                   << std::to_string(obj->collider.pos.z) << " "
                   << std::to_string(obj->collider.scale.x) << " "
                   << std::to_string(obj->collider.scale.y) << " "
                   << std::to_string(obj->collider.scale.z) << " end";
        }
        else
        {
            std::string model_type_str = "";
            switch (obj->visual.getType())
            {
            default:
                model_type_str = "cube"; // add in support for the other model types
                break;
            }
            output << "object " << model_type_str
                   << " " << std::to_string(obj->visual.getPos().x) << " "
                   << std::to_string(obj->visual.getPos().y) << " "
                   << std::to_string(obj->visual.getPos().z) << " "
                   << std::to_string(obj->visual.getScale().x) << " "
                   << std::to_string(obj->visual.getScale().y) << " "
                   << std::to_string(obj->visual.getScale().z) << " "
                   << std::to_string(obj->visual.getImage()) << " "
                   << std::to_string(obj->type) << " col "
                   << std::to_string(obj->collider.pos.x) << " "
                   << std::to_string(obj->collider.pos.y) << " "
                   << std::to_string(obj->collider.pos.z) << " "
                   << std::to_string(obj->collider.scale.x) << " "
                   << std::to_string(obj->collider.scale.y) << " "
                   << std::to_string(obj->collider.scale.z) << " end";
        }
        output.close();
        mainGame.getCurrentLevel()->addLine();
        level_object *duped_obj = mainGame.getCurrentLevel()->getObjectAtIndex(mainGame.getCurrentLevel()->getObjectCount() - 1);
        duped_obj->lineIndex = mainGame.getCurrentLevel()->getLineCount() - 1;
        duped_obj->visual.makeDynamic();
        duped_obj->visual.SetColor(0.5, 1.0, 0.5, 1.0);
        selectedObj = mainGame.getCurrentLevel()->getObjectCount() - 1;
    }
    void deleteSelectedObject()
    {
        level_object *obj = mainGame.getCurrentLevel()->getObjectAtIndex(selectedObj);
        std::ifstream input(temp_path);
        if (!input.is_open())
        {
            std::cout << "Delete object error: failed to open input file " << temp_path << "\n";
            return;
        }
        std::stringstream ss("");

        std::string line;
        unsigned int lineNum = 0;
        while (std::getline(input, line))
        {
            if (lineNum == obj->lineIndex)
            {
                ++lineNum;
                continue;
            }

            ss << line << "\n";

            ++lineNum;
        }
        input.close();
        std::ofstream output(temp_path);
        if (!output.is_open())
        {
            std::cout << "Delete object error: failed to open output file " << temp_path << "\n";
            return;
        }

        line = "";
        while (std::getline(ss, line))
        {
            output << line << "\n";
        }

        output.close();

        mainGame.getCurrentLevel()->removeObjectAtIndex(selectedObj); // something's off probably with the lineNum variable, please check and get an std::cout << line << " "; in there to check what's going on!
    }

    void selectObj(glm::vec3 camPos, glm::vec3 camDir, int &textureOffset)
    {
        int closestObj = -1;
        double contactDistance = std::numeric_limits<double>::infinity();
        level_object *obj;
        for (int i = 0; i < mainGame.getCurrentLevel()->getObjectCount(); ++i)
        {
            obj = mainGame.getCurrentLevel()->getObjectAtIndex(i);
            obj->visual.SetColor(1.0, 1.0, 1.0, 1.0);
            raycast ray = {camPos + glm::vec3(0.0, 0.5, 0.0), glm::normalize(camDir)};
            glm::vec3 hitPos = glm::vec3(0.0f);
            bool raycasthit = colliding(ray, obj->collider, hitPos);

            glm::vec3 boxmin = obj->collider.pos - obj->collider.scale;
            glm::vec3 boxmax = obj->collider.pos + obj->collider.scale;
            double hitDistance = glm::distance2(camPos, hitPos);
            if (raycasthit && contactDistance > hitDistance)
            {
                contactDistance = hitDistance;
                closestObj = i;
            }
        }
        if (closestObj == -1)
        {
            std::cout << "leaving selection\n";
            selectedObj = -1;
            return;
        }
        if (selectedObj != closestObj)
        {
            selectedObj = closestObj;
            updateNum = 0;
        }
        else if (selectedObj == closestObj)
        {
            ++updateNum;
            if (updateNum > 4)
            {
                updateNum = 0;
                selectedObj = -1;
                return;
            }
        }
        obj = mainGame.getCurrentLevel()->getObjectAtIndex(selectedObj);

        if (!obj->visual.isDynamic())
            obj->visual.makeDynamic();

        // collider_visual = model_primitive(MODEL_CUBE, true, true);

        switch (updateNum)
        {
        case 0:
            obj->visual.SetColor(0.5, 1.0, 0.5, 1.0);
            break;
        case 1:
            obj->visual.SetColor(1.0, 0.5, 1.0, 1.0);
            break;
        case 2:
            obj->visual.SetColor(1.0, 1.0, 1.0, 1.0);
            textureOffset = obj->visual.getImage();
            break;
        case 3:
            obj->visual.SetColor(1.0, 1.0, 1.0, 0.25);
            break;
        case 4:
            obj->visual.SetColor(1.0, 1.0, 1.0, 0.25);
            break;
        default:
            break;
        }

        edit_line = obj->lineIndex;
    }

    void updateObj(glm::vec3 camPos, glm::vec3 prevCamPos, int &textureOffset, bool &textureEditing, bool &createObjCall, bool &deleteObjCall, double tick_time)
    {
        textureEditing = false;
        if (createObjCall && selectedObj == -1)
        {
            newObject();
            selectedObj = mainGame.getCurrentLevel()->getObjectCount() - 1;
            updateNum = 0;
        }
        else if (createObjCall && selectedObj != -1)
        {
            duplicateSelectedObject();
            updateNum = 0;
        }

        if (deleteObjCall && selectedObj != -1)
        {
            deleteSelectedObject();
            updateNum = 0;
            selectedObj = -1;
        }

        if (selectedObj == -1)
            return;

        level_object *obj = mainGame.getCurrentLevel()->getObjectAtIndex(selectedObj);

        switch (updateNum)
        {
        case 0:
            obj->visual.Move((camPos - prevCamPos));
            // obj->collider.pos = obj->visual.getPos();
            break;
        case 1:
            obj->visual.Scale(obj->visual.getScale() + ((camPos - prevCamPos) * 2.0f));
            if (obj->visual.getScale().x < 0.0)
            {
                obj->visual.Scale(0.0, obj->visual.getScale().y, obj->visual.getScale().z);
            }
            if (obj->visual.getScale().y < 0.0)
            {
                obj->visual.Scale(obj->visual.getScale().x, 0.0, obj->visual.getScale().z);
            }
            if (obj->visual.getScale().z < 0.0)
            {
                obj->visual.Scale(obj->visual.getScale().x, obj->visual.getScale().y, 0.0);
            }
            // obj->collider.scale = obj->visual.getScale(); // do collider seperately
            break;
        case 2:
        {
            textureEditing = true;
            if (textureOffset < 0)
            {
                textureOffset = allTextures.getTextureCount() - 1;
            }
            if (textureOffset > allTextures.getTextureCount() - 1)
            {
                textureOffset = 0;
            }
            obj->visual.Image(textureOffset);
        }
        break;
        case 3:
            obj->collider.pos += (camPos - prevCamPos);
            break;
        case 4:
            obj->collider.scale += (camPos - prevCamPos) * 2.0f;
            for (int i = 0; i < 3; ++i)
            {
                obj->collider.scale[i] = std::max(obj->collider.scale[i], 0.0f);
            }
            break;
        default:
            break;
        }
    }
    void updateFile()
    {
        collider_visuals.clear();
        for (int i = 0; i < mainGame.getCurrentLevel()->getObjectCount(); ++i)
        {
            collider_visuals.push_back(model_primitive(MODEL_CUBE, true, true));
            level_object *obj = mainGame.getCurrentLevel()->getObjectAtIndex(i);
            collider_visuals[collider_visuals.size() - 1].Put(obj->collider.pos);
            collider_visuals[collider_visuals.size() - 1].Scale(obj->collider.scale);
        }

        if (selectedObj == -1)
            return;

        level_object *obj = mainGame.getCurrentLevel()->getObjectAtIndex(selectedObj);

        std::ifstream input(temp_path);
        if (!input.is_open())
        {
            std::cout << "Error updating file: " << temp_path << "\n";
            return;
        }

        std::stringstream copy_str("");
        std::string line = "";
        unsigned int lineNum = 0;
        while (getline(input, line))
        {
            if (lineNum == obj->lineIndex)
            {
                bool finishedUpdate = false;
                int step = 0;
                if (obj->visual.getType() != MODEL_CUSTOM)
                {
                    step = 1;
                }
                std::stringstream ss(line);
                std::stringstream newline("");
                std::string word = "";
                while (!finishedUpdate && std::getline(ss, word, ' '))
                {
                    switch (step)
                    {
                    case 3:
                        newline << (obj->visual.getPos().x) << ' ';
                        break;
                    case 4:
                        newline << (obj->visual.getPos().y) << ' ';
                        break;
                    case 5:
                        newline << (obj->visual.getPos().z) << ' ';
                        break;
                    case 6:
                        newline << (obj->visual.getScale().x) << ' ';
                        break;
                    case 7:
                        newline << (obj->visual.getScale().y) << ' ';
                        break;
                    case 8:
                        newline << (obj->visual.getScale().z) << ' ';
                        break;
                    case 9:
                        newline << (obj->visual.getImage()) << ' ';
                        break;
                    case 10:
                        newline << (obj->type) << " col ";
                        break;
                    case 11:
                        newline << (obj->collider.pos.x) << ' ';
                        break;
                    case 12:
                        newline << (obj->collider.pos.y) << ' ';
                        break;
                    case 13:
                        newline << (obj->collider.pos.z) << ' ';
                        break;
                    case 14:
                        newline << (obj->collider.scale.x) << ' ';
                        break;
                    case 15:
                        newline << (obj->collider.scale.y) << ' ';
                        break;
                    case 16:
                        newline << (obj->collider.scale.z) << " end";
                        step = -1;
                        finishedUpdate = true;
                        break;
                    default:
                        newline << word << ' ';
                        break;
                    }
                    ++step;
                }
                copy_str << newline.str() << '\n';
            }
            else
            {
                copy_str << line << '\n';
            }
            ++lineNum;
        }

        input.close();

        std::ofstream output(temp_path);
        if (!output.is_open())
        {
            std::cout << "Update file output failure at path: " << temp_path << "\n";
            return;
        }
        output << copy_str.str();

        output.close();
    }
};

#endif