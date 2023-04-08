#pragma once

#include <string>
#include <vector>

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

    struct ProjectSrc{
        std::vector<std::pair<std::string, std::string>> src;
        std::pair<std::string, std::string> build;
        std::vector<std::string> directories;
    };

    struct ProjectTemplate{
        std::string name;

        std::vector<std::string> directories;

        std::vector<std::pair<std::string, std::string>> files;

        std::string makefile;
        std::string bash;

        ProjectTemplate() = default;

        ProjectTemplate(const std::string& path);

        void load(std::ifstream& file, const std::string& seperator);

        std::string to_string() const;
        ProjectSrc compile(const std::string& name, project::build b, bool fast);
    };

    std::vector<ProjectTemplate> load_project_templates(const std::string& p);
}