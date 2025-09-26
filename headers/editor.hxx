#ifndef EDITOR_HXX
#define EDITOR_HXX

#include <string>
#include <iostream>
#include <fstream>
#include "../headers/system.hxx"

extern game mainGame;

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

    unsigned int edit_line = 0;
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

    void selectObj(glm::vec3 camPos, glm::vec3 camDir) // now add support for pcubes, variables, triggers, and everything else. There should be a key that locks the cube in place and scales according to player movement instead
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
        }
        else if (selectedObj == closestObj)
        {
            selectedObj = -1;
            return;
        }
        obj = mainGame.getCurrentLevel()->getObjectAtIndex(selectedObj);

        mainGame.getCurrentLevel()->getObjectAtIndex(closestObj)->visual.makeDynamic();
        obj->visual.SetColor(0.5, 1.0, 0.5, 1.0);

        edit_line = obj->lineIndex;
    }

    void updateSelection(glm::vec3 camPos, glm::vec3 prevCamPos)
    {
        if (selectedObj == -1)
            return;

        level_object *obj = mainGame.getCurrentLevel()->getObjectAtIndex(selectedObj);
        obj->visual.Move(camPos - prevCamPos);
        obj->collider.pos = obj->visual.getPos();
    }
    void updateFile()
    {
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
                std::stringstream ss(line);
                std::stringstream newline("");
                std::string word = "";
                while (!finishedUpdate && std::getline(ss, word, ' '))
                {
                    switch (step)
                    {
                    case 1:
                        newline << (obj->visual.getPos().x) << ' ';
                        break;
                    case 2:
                        newline << (obj->visual.getPos().y) << ' ';
                        break;
                    case 3:
                        newline << (obj->visual.getPos().z) << ' ';
                        break;
                    case 8:
                        newline << word;
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