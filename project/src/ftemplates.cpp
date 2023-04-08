#include <ftemplates.hpp>

#include <filesystem>
#include <fstream>

#include <liblog.hpp>

namespace fs = std::filesystem;

#define CREATE_DIR(name) if(!fs::create_directory(name)) dtk::log::error("Failed to create directory.", 111); //EACCES
#define CREATE_FILE(name) { std::ofstream file(name); if(!file.good()) dtk::log::error("Failed to create file.", 111); /*EACCES*/ file.close(); }
#define CREATE_SCRIPT(name) { std::ofstream file(name); if(!file.good()) dtk::log::error("Failed to create script file.", 111); /*EACCES*/ file << "#!/bin/bash" << std::endl; file.close(); }
#define FILE_OK(var) if(!var.good()) dtk::log::error("Failed to open file.", 111); //EACCES

namespace ftemplates{
    void create_project(const std::string& name, project::type t, project::build b){
        bool fast = t == project::PROJECT_TYPE_SOLUTION;

        //Create project directories
        CREATE_DIR(name);
        CREATE_DIR(name + "/.project");
        CREATE_DIR(name + "/.scripts");

        if(!fast){
            CREATE_DIR(name + "/include");
            CREATE_DIR(name + "/lib");
            CREATE_DIR(name + "/bin");
        }

        //Create .project/project file
        std::ofstream project(name + "/.project/project");
        FILE_OK(project)
        
        project << "#Name: " << name << std::endl;
        project << "#Type: " << t << std::endl;
        project << "#Build: " << b << std::endl;
        
        project.close();

        //Create .project/templates file (with no data)
        CREATE_FILE(name + "/.project/templates");

        //Create scripts (and .project/solutions file in non-fast case)
        if(!fast){
            CREATE_FILE(name + "/.project/solutions");

            CREATE_SCRIPT(name + "/.scripts/clean.sh");
            CREATE_SCRIPT(name + "/.scripts/make.sh");
            CREATE_SCRIPT(name + "/.scripts/test.sh");
        } else{
            std::ofstream script_test(name + "/.scripts/test.sh");
            FILE_OK(script_test);

            script_test << "#!/bin/bash" << std::endl;
            script_test << std::endl;

            switch(b){
                case project::PROJECT_BUILD_MAKE:
                    script_test << "make test" << std::endl;
                    script_test << std::endl;
                    break;
                case project::PROJECT_BUILD_BASH:
                    script_test << "./" << name << std::endl;
                    break;
            }

            script_test.close();
        }

        //Create run script
        update_run_script(name + "/run", b, fast);
    }

    void update_run_script(const std::string& p, project::build b, bool fast, unsigned int enabled){
        bool log = enabled & project::PROJECT_ENABLE_LOG;
        bool tests = enabled & project::PROJECT_ENABLE_TESTS;

        //Open run script
        std::ofstream run(p);// name + "/run"
        FILE_OK(run);

        //Get make command
        std::string make_cmd = "./.scripts/make.sh";
        if(fast && b != project::PROJECT_BUILD_BASH)
            make_cmd = "make";
        
        //Begin as bash script
        run << "#!/bin/bash" << std::endl;
        run << std::endl;

        if(log){ //log case
            run << "#Get the log file name" << std::endl;
            run << "log=$(date +'log/log%Y.%j.txt')" << std::endl;
            run << "mkdir log 2>/dev/null" << std::endl;
            run << std::endl;
            run << "echo $(date +'#Date: %F %T:%N') >> $log" << std::endl;
            run << "echo \"\" >> $log" << std::endl;
            run << std::endl;

            if(!fast){
                run << "#Clean" << std::endl;
                run << "echo \"#Clean:\" >> $log" << std::endl;
                run << "./.scripts/clean.sh >> $log" << std::endl;
                run << "echo \"\" >> $log" << std::endl;
                run << std::endl;
            }

            run << "#Make" << std::endl;
            run << "echo \"#Make:\" >> $log" << std::endl;
            run << make_cmd << " >> $log" << std::endl;
            run << "echo \"\" >> $log" << std::endl;
            run << std::endl;

            if(tests){
                run << "#Test" << std::endl;
                run << "echo \"#Test:\" >> $log" << std::endl;
                run << "./.scripts/test.sh >> $log" << std::endl;
                run << "echo \"\" >> $log" << std::endl;
            }
        } else{ //normal case
            if(!fast){
                run << "#Clean" << std::endl;
                run << "./.scripts/clean.sh" << std::endl;
                run << std::endl;
            }

            run << "#Make" << std::endl;
            run << make_cmd << std::endl;
            run << std::endl;

            if(tests){
                run << "#Test" << std::endl;
                run << "./.scripts/test.sh" << std::endl;
            }
        }

        //Close the file
        run.close();
    }

    void update_templates_file(const std::string& p, const std::vector<fdata::ProjectTemplate>& templates){
        //Get already added templates
        auto added = fdata::load_project_templates(p);
        
        //Strore seen names
        std::vector<std::string> names;

        //Open temlates file
        std::ofstream templates_file(p);
        FILE_OK(templates_file);

        //Add already known templates and store its names (validate them)
        for(auto& tmpl: added){
            bool found = false;

            for(auto& name: names){
                if(name == tmpl.name){
                    found = true;
                    break;
                }
            }

            if(!found){
                names.push_back(tmpl.name);

                templates_file << tmpl.to_string();
                templates_file << "#!END" << std::endl;
                templates_file << std::endl;
            }
        }

        //Do the same for new templates
        for(auto& tmpl: templates){
            bool found = false;

            for(auto& name: names){
                if(name == tmpl.name){
                    found = true;
                    break;
                }
            }

            if(!found){
                names.push_back(tmpl.name);

                templates_file << "#Begin" << std::endl;
                templates_file << tmpl.to_string();
                templates_file << "#End" << std::endl;
                templates_file << std::endl;
            }
        }

        //Close the file
        templates_file.close();
    }

    void build_template(const std::string& name, const fdata::ProjectSrc& t){
        //Create directories
        for(auto& dir: t.directories){
            CREATE_DIR(dir);
        }

        //Create files
        for(auto& file: t.src){
            std::ofstream f(file.first);
            FILE_OK(f);

            f << file.second;

            f.close();
        }

        //Create build file
        std::ofstream build(t.build.first);
        FILE_OK(build);

        build << t.build.second;

        build.close();
    }
}