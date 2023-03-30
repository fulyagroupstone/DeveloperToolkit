#pragma once

#include <string>

#include "defs.hpp"

namespace ftemplates{
    void create_project(const std::string& name, project::type t, project::build b);
    void update_run_script(const std::string& p, project::build b, bool fast, bool log = false);

    void create_src_file(const std::string& name, project::type t);
    void create_makefile(const std::string& name, project::type t);
    void create_bash(const std::string& name, project::type t);
}