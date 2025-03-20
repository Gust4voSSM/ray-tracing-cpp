#ifndef MATERIAL_READER_HPP
#define MATERIAL_READER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>
#include "Object.hpp"   // Presume que aqui está definida a struct Material dentro de Object
#include "Color.hpp"

class MaterialReader {
public:
    // Mapa que relaciona o nome do material com as suas propriedades
    std::map<std::string, Object::Material> materials;

    MaterialReader() {}

    // Construtor que lê o arquivo .mtl passado em 'filepath'
    MaterialReader(const std::string &filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Erro ao abrir o arquivo .mtl: " << filepath << std::endl;
            return;
        }

        std::string line;
        std::string currentMaterial;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string keyword;
            iss >> keyword;

            if (keyword == "newmtl") {
                iss >> currentMaterial;
                if (!currentMaterial.empty()) {
                    materials[currentMaterial] = Object::Material();
                }
            }
            else if (keyword == "Kd") {
                double r, g, b;
                iss >> r >> g >> b;
                if (!currentMaterial.empty()) {
                    // Kd = Difuso
                    materials[currentMaterial].diffuse = Color(r, g, b);
                }
            }
            else if (keyword == "Ks") {
                double r, g, b;
                iss >> r >> g >> b;
                if (!currentMaterial.empty()) {
                    // Ks = Specular
                    materials[currentMaterial].specular = Color(r, g, b);
                }
            }
            else if (keyword == "Ke") {
                double r, g, b;
                iss >> r >> g >> b;
                if (!currentMaterial.empty()) {
                    // Ke = Emissive
                    materials[currentMaterial].emissive = Color(r, g, b);
                }
            }
            else if (keyword == "Ka") {
                double r, g, b;
                iss >> r >> g >> b;
                if (!currentMaterial.empty()) {
                    // Ka = Ambiente
                    materials[currentMaterial].ambient = Color(r, g, b);
                }
            }
            else if (keyword == "Ns") {
                double ns;
                iss >> ns;
                if (!currentMaterial.empty()) {
                    materials[currentMaterial].ns = ns;
                }
            }
            else if (keyword == "Ni") {
                double ni;
                iss >> ni;
                if (!currentMaterial.empty()) {
                    materials[currentMaterial].ni = ni;
                }
            }
            else if (keyword == "d") {
                double d;
                iss >> d;
                if (!currentMaterial.empty()) {
                    materials[currentMaterial].opacity = d;
                }
            }
        }
        file.close();
    }

    // Retorna um ponteiro para o material correspondente ou nullptr se não encontrado
    Object::Material* getMaterial(const std::string &materialName) {
        auto it = materials.find(materialName);
        if (it != materials.end()) {
            return &(it->second);
        }
        std::cerr << "Erro: material '" << materialName << "' não definido no arquivo .mtl." << std::endl;
        return nullptr;
    }
};

#endif
