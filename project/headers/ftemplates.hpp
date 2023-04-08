#pragma once

#include <string>
#include <vector>

#include "defs.hpp"
#include "fdata.hpp"

namespace ftemplates{
    void create_project(const std::string& name, project::type t, project::build b);
    void update_run_script(const std::string& p, project::build b, bool fast, unsigned int enabled = 0);
    void update_templates_file(const std::string& p, const std::vector<fdata::ProjectTemplate>& templates);

    void build_template(const std::string& name, const fdata::ProjectSrc& t);
}