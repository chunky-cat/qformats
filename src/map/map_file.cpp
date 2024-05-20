#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <regex>

#include <glm/glm.hpp>
#include "../../include/qformats/map/map_file.h"

namespace qformats::map
{
    static std::vector<std::string> rexec_vec(std::string line, std::string regexstr)
    {
        std::stringstream results;
        std::regex re(regexstr, std::regex::icase);

        std::vector<std::string> matches;
        const std::sregex_token_iterator end;
        for (std::sregex_token_iterator it(line.begin(), line.end(), re, 1); it != end; it++)
        {
            matches.push_back(*it);
        }

        return matches;
    }

    void QMapFile::Parse(const std::string &filename)
    {
        struct parsedObject
        {
            std::stringstream lines;
            parsedObject *parent = nullptr;
            std::vector<parsedObject *> children;
        };

        auto strstr = std::fstream(filename);
        std::vector<parsedObject *> objects;
        parsedObject *current = nullptr;
        for (std::string line; std::getline(strstr, line);)
        {
            if (line.starts_with("//"))
            {
                continue;
            }
            if (line == "{")
            {
                parsedObject *newobj = new parsedObject;
                if (current == nullptr)
                {
                    objects.push_back(newobj);
                    current = newobj;
                }
                else
                {
                    newobj->parent = current;
                    current->children.push_back(newobj);
                    current = newobj;
                }
                continue;
            }
            if (line == "}")
            {
                if (current != nullptr)
                {
                    current = current->parent;
                }
                continue;
            }
            if (current != nullptr)
            {
                current->lines << line << std::endl;
            }
        }

        for (auto *obj : objects)
        {
            QEntity *ent = nullptr;
            if (obj->children.size() > 0)
            {
                ent = new QBrushEntity();
                brushEntities.push_back(static_cast<QBrushEntity *>(ent));
            }
            else
            {
                ent = new QPointEntity();
                pointEntities.push_back(static_cast<QPointEntity *>(ent));
            }
            for (std::string line; std::getline(obj->lines, line);)
            {
                parse_entity_attributes(line, ent);
            }
            for (auto *subObj : obj->children)
            {
                parse_entity_planes(subObj->lines, static_cast<QBrushEntity *>(ent));
            }
        }
    }

    void QMapFile::parse_wad_string(const std::string &wstr)
    {
        std::istringstream ss(wstr);

        for (std::string item; std::getline(ss, item, ';');)
        {
            wads.push_back(item.substr(item.find_last_of("/\\") + 1));
        }
    }

    void QMapFile::parse_entity_attributes(std::string l, QEntity *ent)
    {
        auto matches = rexec_vec(l, R"(\"([\s\S]*?)\")");
        if (matches[0] == "mapversion")
        {
            mapVersion = stoi(matches[1]);
            return;
        }
        if (matches[0] == "wad")
        {
            parse_wad_string(matches[1]);
            return;
        }
        if (matches[0] == "classname")
        {
            ent->classname = matches[1];
            if (ent->classname == "worldspawn")
            {
                worldSpawn = reinterpret_cast<QBrushEntity *>(ent);
            }
            return;
        }
        if (matches[0] == "origin")
        {
            std::stringstream stream(matches[1]);
            auto pent = reinterpret_cast<QPointEntity *>(ent);
            stream >> pent->origin.x >> pent->origin.y >> pent->origin.z;
            return;
        }
        if (matches[0] == "_tb_name")
        {
            auto bent = reinterpret_cast<QBrushEntity *>(ent);
            bent->tbName = matches[1];
            return;
        }
        if (matches[0] == "_tb_type")
        {
            auto bent = reinterpret_cast<QBrushEntity *>(ent);
            bent->tbType = matches[1];
            return;
        }
        if (matches[0] == "_phong")
        {
            auto bent = reinterpret_cast<QBrushEntity *>(ent);
            bent->hasPhongShading = matches[1].compare("1") == 0;
            return;
        }
        ent->attributes.emplace(matches[0], matches[1]);
    }

    void QMapFile::parse_entity_planes(std::stringstream &lines, QBrushEntity *ent)
    {
        Brush brush;
        for (std::string line; std::getline(lines, line);)
        {
            MapFileFace face;
            std::string chars = "()[]";
            for (unsigned int i = 0; i < chars.length(); ++i)
            {
                line.erase(std::remove(line.begin(), line.end(), chars[i]), line.end());
            }

            std::stringstream l(line);
            l >> face.planePoints[0].x >> face.planePoints[0].y >> face.planePoints[0].z;
            l >> face.planePoints[1].x >> face.planePoints[1].y >> face.planePoints[1].z;
            l >> face.planePoints[2].x >> face.planePoints[2].y >> face.planePoints[2].z;
            std::string texture;
            l >> texture;
            switch (mapVersion)
            {
            case VALVE_VERSION:
                l >> face.valveUV.u.x >> face.valveUV.u.y >> face.valveUV.u.z >> face.valveUV.u.w;
                l >> face.valveUV.v.x >> face.valveUV.v.y >> face.valveUV.v.z >> face.valveUV.v.w;
                break;
            default:
                l >> face.standardUv.u >> face.standardUv.v;
                break;
            }
            l >> face.rotation >> face.scaleX >> face.scaleY;

            glm::dvec3 v0v1 = face.planePoints[1] - face.planePoints[0];
            glm::dvec3 v1v2 = face.planePoints[2] - face.planePoints[1];
            face.planeNormal = glm::normalize(glm::cross(v1v2, v0v1));
            face.planeDist = glm::dot(face.planeNormal, face.planePoints[0]);
            brush.hasValveUV = mapVersion == VALVE_VERSION;
            face.textureID = getOrAddTexture(texture);
            brush.faces.push_back(face);
        }
        ent->brushes.push_back(brush);
    }

    int QMapFile::getOrAddTexture(std::string texture)
    {
        for (int i = 0; i < textures.size(); i++)
        {
            if (textures[i] == texture)
                return i;
        }
        textures.push_back(texture);
        int ret = textures.size() - 1;
        return ret;
    }
}