#ifndef TRIANGLE_MESH
#define TRIANGLE_MESH
#include "Object.hpp"
#include "MaterialReader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <cstdlib> // para std::stoi

class TriangleMesh: public Object {
    public:
        std::vector<Triangle> triangles;
        std::vector<Vector3> vertices;

        TriangleMesh(int vertex_count, int triangle_count, Vector3* vertex_array, int** triangle_array) {
            // Copia os vértices para o container interno
            for (int i = 0; i < vertex_count; i++) {
                vertices.push_back(vertex_array[i]);
            }
            // Cria os triângulos utilizando os endereços dos vértices no vetor 'vertices'
            for (int i = 0; i < triangle_count; i++) {
                int* triangle_indices = triangle_array[i];
                triangles.push_back(Triangle(&vertices[triangle_indices[0]],
                                               &vertices[triangle_indices[1]],
                                               &vertices[triangle_indices[2]]));
            }
        }

        #include "MaterialReader.hpp"

        // Construtor de TriangleMesh que recebe apenas o caminho do arquivo .obj.
        TriangleMesh(std::string obj_filepath) {
            // Deriva o caminho do arquivo .mtl a partir do caminho do .obj
            std::string mtl_filepath = obj_filepath.substr(0, obj_filepath.find_last_of('.')) + ".mtl";
            
            // Instancia o MaterialReader com o caminho derivado
            MaterialReader materialReader(mtl_filepath);
            
            std::ifstream inFile(obj_filepath);
            if (!inFile.is_open()) {
                std::cerr << "Erro ao abrir o arquivo: " << obj_filepath << std::endl;
                return;
            }
            
            std::string line;
            // Ponteiro para o material corrente, obtido via MaterialReader
            Object::Material* current_material = nullptr;
            
            // Primeiro, lê os vértices
            while (std::getline(inFile, line)) {
                if (line.size() > 2 && line.substr(0, 2) == "v ") {
                    std::istringstream iss(line.substr(2));
                    double x, y, z;
                    if (iss >> x >> y >> z) {
                        vertices.push_back(Vector3(x, y, z));
                    }
                }
            }
            
            // Reinicia o stream para ler faces e diretivas de material
            inFile.clear();
            inFile.seekg(0, std::ios::beg);
            
            while (std::getline(inFile, line)) {
                // Processa a diretiva de material
                if (line.size() > 6 && line.substr(0, 6) == "usemtl") {
                    std::istringstream iss(line.substr(7)); // pula "usemtl "
                    std::string materialName;
                    iss >> materialName;
                    current_material = materialReader.getMaterial(materialName);
                }
                // Processa as faces
                if (line.size() > 2 && line.substr(0, 2) == "f ") {
                    std::istringstream iss(line.substr(2));
                    std::string vertexStr;
                    std::vector<int> faceIndices;
                    while (iss >> vertexStr) {
                        std::istringstream tokenStream(vertexStr);
                        std::string indexStr;
                        std::getline(tokenStream, indexStr, '/');
                        try {
                            int idx = std::stoi(indexStr);
                            // Converte de 1-indexado (.obj) para 0-indexado
                            faceIndices.push_back(idx - 1);
                        } catch (std::invalid_argument&) {
                            std::cerr << "Formato de face inválido: " << vertexStr << std::endl;
                        }
                    }
                    // Cria os triângulos e atribui o material (por ponteiro)
                    if (faceIndices.size() == 3) {
                        Triangle t(&vertices[faceIndices[0]],
                                &vertices[faceIndices[1]],
                                &vertices[faceIndices[2]]);
                        t.material = current_material;  // Atribuição completa do material
                        triangles.push_back(t);
                    }
                    else if (faceIndices.size() > 3) {
                        // Triangulação em fan para faces com mais de 3 vértices
                        for (size_t i = 1; i < faceIndices.size() - 1; i++) {
                            Triangle t(&vertices[faceIndices[0]],
                                    &vertices[faceIndices[i]],
                                    &vertices[faceIndices[i + 1]]);
                            t.material = current_material;
                            triangles.push_back(t);
                        }
                    }
                }
            }
            inFile.close();
        }
        
        std::string to_string() {
            return "triangle mesh";
        }

        Vector3 get_normal(const Vector3 &p) {
            std::cerr << "Error: Normal for mesh not implemented\n";
            return Vector3();
        }
        Intersection raycast(Vector3 p, Vector3 v) {
            Sphere bounding_sphere = Sphere(position, (scale).length()/2);      
            if (bounding_sphere.raycast(p, v).distance == INFINITY) return INFINITY;
            double min_dist = INFINITY;
            Vector3 n;
            Material *mat;
            for (Triangle &t : triangles) {
                Intersection try_hit = t.raycast(p, v);
                double dist = try_hit.distance;
                if (dist < min_dist && dist > epsilon) {
                    mat = t.material;
                    min_dist = dist;
                    n = t.get_normal();
                }
            }
            return Intersection(min_dist, mat, n);
        }

    private:

        // Operadores LINEARES (requer mesh na origem)

        void translate(const Vector3& t) {
            for (auto& v : vertices) {
                v += t;
            }
        }

        void apply_scale(Vector3 s) {
            for (auto& v : vertices) {
                v *= s;
            }
        }

        void rotateX(double angle) {
            double cosA = cos(angle);
            double sinA = sin(angle);
            for (auto& v : vertices) {
                double y = v.y() * cosA - v.z() * sinA;
                double z = v.y() * sinA + v.z() * cosA;
                v[1] = y;
                v[2] = z;
            }
        }

        void rotateY(double angle) {
            double cosA = cos(angle);
            double sinA = sin(angle);
            for (auto& v : vertices) {
                double x = v.x() * cosA + v.z() * sinA;
                double z = -v.x() * sinA + v.z() * cosA;
                v[0] = x;
                v[2] = z;
            }
        }

        void rotate(Vector3 v) {
            return rotate(v.x(), v.y(), v.z());
        }
        void rotate(double angleX, double angleY, double angleZ) {
            double cosX = cos(angleX), sinX = sin(angleX);
            double cosY = cos(angleY), sinY = sin(angleY);
            double cosZ = cos(angleZ), sinZ = sin(angleZ);
            
            for (auto& v : vertices) {
                double x = v.x(), y = v.y(), z = v.z();
                
                // Rotação em X
                if (angleX != 0) {
                    double newY = y * cosX - z * sinX;
                    double newZ = y * sinX + z * cosX;
                    y = newY;
                    z = newZ;
                }
                
                // Rotação em Y
                if (angleY != 0) {
                    double newX = x * cosY + z * sinY;
                    double newZ = -x * sinY + z * cosY;
                    x = newX;
                    z = newZ;
                }
                
                // Rotação em Z
                if (angleZ != 0) {
                    double newX = x * cosZ - y * sinZ;
                    double newY = x * sinZ + y * cosZ;
                    x = newX;
                    y = newY;
                }
                
                v[0] = x;
                v[1] = y;
                v[2] = z;
            }
        }

        void rotateZ(double angle) {
            double cosA = cos(angle);
            double sinA = sin(angle);
            for (auto& v : vertices) {
                double x = v.x() * cosA - v.y() * sinA;
                double y = v.x() * sinA + v.y() * cosA;
                v[0] = x;
                v[1] = y;
            }
        }

        // Ponto Medio
        struct Bounds {Vector3 max, min;};
        Bounds computeBounds() {
            Bounds b;
            b.min = Vector3(INFINITY, INFINITY, INFINITY);
            for (const auto& v : vertices) {
                b.max = max(b.max, v);
                b.min = min(b.min, v);
            }
            return b;
        }

        // Transformação Afim
        void apply_transformations() {
            auto [max, min] = computeBounds();
            Vector3 c = (max + min) / 2;
            double size = (max - min).length();
            translate(-c);
            Vector3 radians = rotation * (180.0/3.141592653589793238463);
            rotate(radians);
            apply_scale(scale/size);
            translate(position);
        }
};

inline std::ostream& operator<<(std::ostream &os, const TriangleMesh &m) {
    os << "Mesh:\n\tVertices:\n";
    for (Vector3 v : m.vertices) {
        os << "\t\t" << v << "\n";
    }
    os << "Numero de faces: " << m.triangles.size() << "\n";
    return os;
}

#endif