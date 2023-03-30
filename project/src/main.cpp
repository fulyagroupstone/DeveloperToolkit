#include <iostream>
#include <fstream>

#include <cstring>

#define STRCMP_EQ(a, b) std::strcmp(a, b) == 0

#include <liblog.hpp>

#include <defs.hpp>
#include <fdata.hpp>
#include <ftemplates.hpp>

namespace pr = project;
namespace fd = fdata;
namespace ft = ftemplates;

int main(int argc, char** argv){
    //Skipp execution
    argc--; argv++;

    if(argc == 0)
        dtk::log::fatal_error("Excepted arguments.", 130); //ENOEXEC

    //Choose task
    char* task = argv[0]; argc--; argv++; //Get a task name and pop the argument
    if(STRCMP_EQ(task, "init")){
        std::string name;
        pr::type type = pr::PROJECT_TYPE_PROJECT;
        pr::build build = pr::PROJECT_BUILD_MAKE;

        //Load properties
        for(int i = 0; i < argc; i++){
            if(STRCMP_EQ(argv[i], "-n") || STRCMP_EQ(argv[i], "--name")){ //Load a name, return an error if already exists or there is a syntax error
                if(++i >= argc)
                    dtk::log::fatal_error("Excepted argument after '-n'.", 83); //ELOAD
                
                if(!name.empty())
                    dtk::log::fatal_error("Name already has been set.");
                
                name = argv[i];
            } else if(STRCMP_EQ(argv[i], "--exe")){ //Set the project as "fast" which output is an executable file, return an error if project type was aready seted
                if(type != pr::PROJECT_TYPE_PROJECT)
                    dtk::log::fatal_error("Project type already has been set.");
                
                type = pr::PROJECT_TYPE_EXE;
            } else if(STRCMP_EQ(argv[i], "--lib")){ //Set project as "fast" which output is a liblary, return an error if the project type was aready seted
                if(type != pr::PROJECT_TYPE_PROJECT)
                    dtk::log::fatal_error("Project type already has been set.");
                
                type = pr::PROJECT_TYPE_LIB;
            } else if(STRCMP_EQ(argv[i], "-b") || STRCMP_EQ(argv[i], "--build")){ //Load a build system, return an error if was already seted or there is a syntax error
                if(build != pr::PROJECT_BUILD_MAKE)
                    dtk::log::fatal_error("Project build system already has been set.");
                
                if(++i >= argc)
                    dtk::log::fatal_error("Excepted argument after '-b'.", 83); //ELOAD
                
                if(STRCMP_EQ(argv[i], "make") || STRCMP_EQ(argv[i], "makefile")) //Set to MAKE
                    build = pr::PROJECT_BUILD_MAKE;
                else if(STRCMP_EQ(argv[i], "bash")) //Set to BASH
                    build = pr::PROJECT_BUILD_BASH;
                else
                    dtk::log::fatal_error("Undefined build system.");
            } else{
                dtk::log::fatal_error(std::string("Unknown argument \"") + argv[i] + "\"", 158); //EMVSPARM
            }
        }

        //Create the project file and the scripts
        ft::create_project(name, type, build);

        //Create the build system files
        if(type == pr::PROJECT_TYPE_EXE || type == pr::PROJECT_TYPE_LIB){
            ft::create_src_file(name, type);
            switch(build){
                case pr::PROJECT_BUILD_MAKE:
                    ft::create_makefile(name, type);
                    break;
                case pr::PROJECT_BUILD_BASH:
                    ft::create_bash(name, type);
                    break;
            }
        }
    } else if(STRCMP_EQ(task, "info")){
        //Load project data, validate and output them
        fd::ProjectFile project("./.project/project");
        
        dtk::log::info("Name: " + project.name);
        
        switch(project.type){
            case pr::PROJECT_TYPE_PROJECT:
                dtk::log::info("Type: PROJECT");
                break;
            case pr::PROJECT_TYPE_EXE:
                dtk::log::info("Type: EXE");
                break;
            case pr::PROJECT_TYPE_LIB:
                dtk::log::info("Type: LIB");
                break;
        }

        if(project.type != pr::PROJECT_TYPE_PROJECT)
            dtk::log::info("Project is non-modular.");
        
        switch(project.build){
            case pr::PROJECT_BUILD_MAKE:
                dtk::log::info("Build system: MAKE");
                break;
            case pr::PROJECT_BUILD_BASH:
                dtk::log::info("Build system: BASH");
                break;
        }

        if(project.enabled){
            std::string enabled = "";

            if(project.enabled & project::PROJECT_ENABLE_LOG)
                enabled += "\"log\" ";

            dtk::log::info("Enabled: " + enabled);
        }
    } else if(STRCMP_EQ(task, "enable")){
        //Load project data and try to enable the functionality
        fd::ProjectFile project("./.project/project");
        bool fast = project.type == pr::PROJECT_TYPE_EXE || project.type == pr::PROJECT_TYPE_LIB;

        //Get functionality and pop it
        if(argc <= 0)
            dtk::log::fatal_error("Excepted the functionality name.", 83); //ELOAD
        
        char* functionality = argv[0];
        argv++;argc--;

        //Select the functionality and enable it (if hadn't been yet)
        if(STRCMP_EQ(functionality, "log")){
            if(project.enabled & pr::PROJECT_ENABLE_LOG){
                dtk::log::warning("\"log\" already enabled.");
                return 0;
            }

            ft::update_run_script("run", project.build, fast, true);
            project.enabled |= project::PROJECT_ENABLE_LOG;
        } else{
            dtk::log::fatal_error(std::string("Invalid functionality name\"") + functionality + "\"", 130); //ENOEXEC
        }

        project.update();
    } else if(STRCMP_EQ(task, "disable")){
        //Load project data and try to disable the functionality
        fd::ProjectFile project("./.project/project");
        bool fast = project.type == pr::PROJECT_TYPE_EXE || project.type == pr::PROJECT_TYPE_LIB;

        //Get functionality and pop it
        if(argc <= 0)
            dtk::log::fatal_error("Excepted the functionality name.", 83); //ELOAD
        
        char* functionality = argv[0];
        argv++;argc--;

        //Select the functionality and disable it (if hadn't been yet)
        if(STRCMP_EQ(functionality, "log")){
            if(project.enabled & pr::PROJECT_ENABLE_LOG == 0){
                dtk::log::warning("\"log\" already disable.");
                return 0;
            }

            ft::update_run_script("run", project.build, fast, false);
            project.enabled &= ~project::PROJECT_ENABLE_LOG;
        } else{
            dtk::log::fatal_error(std::string("Invalid functionality name\"") + functionality + "\"", 130); //ENOEXEC
        }

        //Save changes
        project.update();
    } else{
        dtk::log::fatal_error(std::string("Invalid task name \"") + task + "\"", 130); //ENOEXEC
    }
    return 0;
}