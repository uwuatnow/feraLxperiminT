#include "Model/ObjLoader.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include "Game/Util.h"

namespace nyaa {

bool ObjLoader::Load(const std::string& path, Model& outModel) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Failed to open OBJ file: " << path << std::endl;
        return false;
    }

    outModel.clear();

    std::vector<float> positions;
    std::vector<float> texCoords;
    std::vector<float> normals;

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        std::stringstream ss(line);
        std::string type;
        ss >> type;

        if (type == "v") {
            float x, y, z;
            ss >> x >> y >> z;
            positions.push_back(x);
            positions.push_back(y);
            positions.push_back(z);
        } else if (type == "vt") {
            float u, v;
            ss >> u >> v;
            texCoords.push_back(u);
            texCoords.push_back(v);
        } else if (type == "vn") {
            float nx, ny, nz;
            ss >> nx >> ny >> nz;
            normals.push_back(nx);
            normals.push_back(ny);
            normals.push_back(nz);
        } else if (type == "f") {
            std::string vertexStr;
            std::vector<Vertex> faceVertices;

            while (ss >> vertexStr) {
                int vIdx = 0, vtIdx = 0, vnIdx = 0;
                
                size_t firstSlash = vertexStr.find('/');
                
                if (firstSlash != std::string::npos) {
                    // Has slashes - parse v/vt/vn format
                    try {
                        vIdx = std::stoi(vertexStr.substr(0, firstSlash));
                    } catch (const std::invalid_argument& e) {
                        std::cerr << "Invalid vertex index in face: " << vertexStr.substr(0, firstSlash) << std::endl;
                        continue;
                    }
                    
                    size_t secondSlash = vertexStr.find('/', firstSlash + 1);
                    
                    if (secondSlash != std::string::npos) {
                        // v/vt/vn or v//vn
                        if (secondSlash > firstSlash + 1) {
                            try {
                                vtIdx = std::stoi(vertexStr.substr(firstSlash + 1, secondSlash - firstSlash - 1));
                            } catch (const std::invalid_argument& e) {
                                std::cerr << "Invalid texture coordinate index in face: " << vertexStr.substr(firstSlash + 1, secondSlash - firstSlash - 1) << std::endl;
                            }
                        }
                        try {
                            vnIdx = std::stoi(vertexStr.substr(secondSlash + 1));
                        } catch (const std::invalid_argument& e) {
                            std::cerr << "Invalid normal index in face: " << vertexStr.substr(secondSlash + 1) << std::endl;
                        }
                    } else {
                        // v/vt
                        try {
                            vtIdx = std::stoi(vertexStr.substr(firstSlash + 1));
                        } catch (const std::invalid_argument& e) {
                            std::cerr << "Invalid texture coordinate index in face: " << vertexStr.substr(firstSlash + 1) << std::endl;
                        }
                    }
                } else {
                    // No slashes - just vertex index
                    try {
                        vIdx = std::stoi(vertexStr);
                    } catch (const std::invalid_argument& e) {
                        std::cerr << "Invalid vertex index in face: " << vertexStr << std::endl;
                        continue;
                    }
                }
                
                // Adjust 1-based OBJ indices to 0-based
                vIdx = (vIdx > 0) ? vIdx - 1 : (int)positions.size() / 3 + vIdx;
                vtIdx = (vtIdx > 0) ? vtIdx - 1 : (int)texCoords.size() / 2 + vtIdx;
                vnIdx = (vnIdx > 0) ? vnIdx - 1 : (int)normals.size() / 3 + vnIdx;
                
                // Bounds checking
                int maxVIdx = (int)positions.size() / 3 - 1;
                int maxVtIdx = (int)texCoords.size() / 2 - 1;
                int maxVnIdx = (int)normals.size() / 3 - 1;
                
                if (vIdx < 0 || vIdx > maxVIdx) {
                    std::cerr << "Vertex index " << vIdx << " out of bounds (max: " << maxVIdx << ")" << std::endl;
                    continue;
                }
                
                Vertex v;
                v.x = positions[vIdx * 3];
                v.y = positions[vIdx * 3 + 1];
                v.z = positions[vIdx * 3 + 2];
                
                if (vtIdx >= 0 && vtIdx <= maxVtIdx) {
                    v.u = texCoords[vtIdx * 2];
                    v.v = texCoords[vtIdx * 2 + 1];
                } else {
                    v.u = 0; v.v = 0;
                }
                
                if (vnIdx >= 0 && vnIdx <= maxVnIdx) {
                    v.nx = normals[vnIdx * 3];
                    v.ny = normals[vnIdx * 3 + 1];
                    v.nz = normals[vnIdx * 3 + 2];
                } else {
                    v.nx = 0; v.ny = 1; v.nz = 0;
                }
                
                faceVertices.push_back(v);
            }
            
            // Triangulate (Fan)
            if (faceVertices.size() >= 3) {
                for (size_t i = 1; i < faceVertices.size() - 1; ++i) {
                    outModel.vertices.push_back(faceVertices[0]);
                    outModel.vertices.push_back(faceVertices[i]);
                    outModel.vertices.push_back(faceVertices[i + 1]);
                }
            }
        }
    }

    return true;
}

}
