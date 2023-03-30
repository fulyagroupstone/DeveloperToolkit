#pragma once

#include <string>

#include "defs.hpp"

namespace fdata{
    struct ProjectFile{
        std::string name;
        project::type type;
        project::build build;
        int enabled = 0;

        std::string path;

        ProjectFile(const std::string& path);
        void update();
    };

    struct Solution{
        std::string name;
        project::type type;
    };
}