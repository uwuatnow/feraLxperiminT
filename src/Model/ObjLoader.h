#pragma once
#include <string>
#include "Model/Model.h"

namespace nyaa {

class ObjLoader {
public:
    static bool Load(const std::string& path, Model& outModel);
};

}
