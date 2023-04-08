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
        std::string templ;
        std::vector<std::string> templ_p;
        pr::type type = pr::PROJECT_TYPE_PROJECT;
        pr::build build = pr::PROJECT_BUILD_MAKE;

        bool build_set = false;

        //Load properties
        for(int i = 0; i < argc; i++){
            if(STRCMP_EQ(argv[i], "-n") || STRCMP_EQ(argv[i], "--name")){ //Load a name, return an error if already exists or there is a syntax error
                if(++i >= argc)
                    dtk::log::fatal_error("Excepted argument after '-n'.", 83); //ELOAD
                
                if(!name.empty())
                    dtk::log::fatal_error("Name already has been set.");
                
                name = argv[i];
            } else if(STRCMP_EQ(argv[i], "-b") || STRCMP_EQ(argv[i], "--build")){ //Load a build system, return an error if was already seted or there is a syntax error
                if(build_set)
                    dtk::log::fatal_error("Project build system already has been set.");
                
                if(++i >= argc)
                    dtk::log::fatal_error("Excepted argument after '-b'.", 83); //ELOAD
                
                if(STRCMP_EQ(argv[i], "make") || STRCMP_EQ(argv[i], "makefile")) //Set to MAKE
                    build = pr::PROJECT_BUILD_MAKE;
                else if(STRCMP_EQ(argv[i], "bash")) //Set to BASH
                    build = pr::PROJECT_BUILD_BASH;
                else
                    dtk::log::fatal_error("Unsuported build system.");
                
                build_set = true;
            } else if(STRCMP_EQ(argv[i], "-t") || STRCMP_EQ(argv[i], "--template")){ //Load a template name, return an error if was already seted or there is a syntax error, set project to fast
                if(!templ.empty())
                    dtk::log::fatal_error("Project template already has been set.");
                
                if(++i >= argc)
                    dtk::log::fatal_error("Excepted argument after '-t'.", 83); //ELOAD
                
                templ = argv[i];
                type = pr::PROJECT_TYPE_SOLUTION;
            } else if(STRCMP_EQ(argv[i], "-T") || STRCMP_EQ(argv[i], "--template-file")){ //Add a template file, return an error if there is a syntax error
                if(++i >= argc)
                    dtk::log::fatal_error("Excepted argument after '-T'.", 83); //ELOAD
                
                templ_p.emplace_back(argv[i]);
            } else{
                dtk::log::fatal_error(std::string("Unknown argument \"") + argv[i] + "\"", 158); //EMVSPARM
            }
        }

        //Load templates
        std::vector<fd::ProjectTemplate> templates;

        //Default template
        fd::ProjectTemplate c_cpp;
        c_cpp.name = "c_cpp";
        c_cpp.directories = {"src", "headers"};
        c_cpp.files.emplace_back("src/main.cpp", "#include <iostream>\n\nint main(int argc, char** argv){\n\tstd::cout << \"Hello World!\" << std::endl;\n\treturn 0;\n}");
        c_cpp.makefile = "name := #!NAME!#\n\nincludes := #!INCLUDES!# -I headers\n\ncflags := #!C_FLAGS!# $(includes)\ncppflags := #!CXX_FLAGS!# $(includes)\nlinkerflags := #!LD_FLAGS!#\n\nbin := $(patsubst src/%,bin/%.o,$(wildcard src/*.c*))\n\n.PHONY: all\nall: clean bin $(name)\n\nclean:\n\t@echo Clean $(name)\n\t@rm -f bin/*\n\t@rm -f $(name)\n\nbin:\n\t@mkdir $@\n\nbin/%.c.o:src/%.c\n\t@echo Compiling $^\n\t@gcc -c -o $@ $^ $(cflags)\n\nbin/%.cpp.o:src/%.cpp\n\t@echo Compiling $^\n\t@g++ -c -o $@ $^ $(cppflags)\n\n$(name): $(bin)\n\t@echo Linking $(name)\n\t@g++ -o $@ $^ $(linkerflags)\n\ntest: $(name)\n\t@echo Test $(name)\n\t./$(name)\n";
        c_cpp.bash = "name='#!NAME!#'\n\nincludes='-I headers #!INCLUDES!#'\n\ncflags=('#!C_FLAGS!#' $includes)\ncppflags=('#!CXX_FLAGS!#' $includes)\nlinkerflags='#!LD_FLAGS!#'\n\n#Clean:\necho Cleaning $name\nrm -fr bin/*\nrm -f $name*\n\n#Create bin:\nmkdir bin 2>/dev/null\n\n#Compile sources\nfor src in ./src/*\ndo\n\techo Compiling $src\n\tif [[ $src == *.c ]]\n\tthen\n\t\tgcc -c -o bin/$( echo $src | cut -d '/' -f 2).o $src $cflags\n\telif [[ $src == *.cpp ]]\n\tthen\n\t\tg++ -c -o bin/$( echo $src | cut -d '/' -f 2).o $src $cppflags\n\telse\n\t\techo Error: No rule to compile $src\n\tfi\ndone\n\n#Link:\necho Linking $name\ng++ -o $name bin/*.o $linkerflags\n";
        templates.push_back(c_cpp);

        for(auto& t: templ_p){
            templates.emplace_back(t);
        }

        //Create the project files and scripts
        ft::create_project(name, type, build);

        ft::update_templates_file(name + "/.project/templates", templates);

        if(type == pr::PROJECT_TYPE_SOLUTION){ //Use template if project is fast
            fd::ProjectTemplate& used_templ = templates[0];
            
            //Find choosen template and return an error if not found
            bool found = false;
            for(auto& t: templates){
                if(t.name == templ){
                    used_templ = t;
                    found = true;
                    break;
                }
            }

            if(!found)
                dtk::log::fatal_error("Template not found \"" + templ + "\".", 83); //ELOAD

            //Build files
            ft::build_template(name, used_templ.compile(name, build, true));
        }
    } else if(STRCMP_EQ(task, "info")){
        //Load project data, validate and output them
        fd::ProjectFile project("./.project/project");
        
        dtk::log::info("Name: " + project.name);
        
        switch(project.type){
            case pr::PROJECT_TYPE_PROJECT:
                dtk::log::info("Type: PROJECT");
                break;
            case pr::PROJECT_TYPE_SOLUTION:
                dtk::log::info("Type: SOLUTION");
                break;
        }

        if(project.type != pr::PROJECT_TYPE_PROJECT)
            dtk::log::warning("Project is non-modular.");
        
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

            if(project.enabled & pr::PROJECT_ENABLE_LOG)
                enabled += "\"silent\" ";
            if(project.enabled & pr::PROJECT_ENABLE_TESTS)
                enabled += "\"tests\" ";

            dtk::log::info("Enabled: " + enabled);
        }
    } else if(STRCMP_EQ(task, "enable")){
        //Load project data and try to enable the functionality
        fd::ProjectFile project("./.project/project");
        bool fast = project.type == pr::PROJECT_TYPE_SOLUTION;

        //Get functionality and pop it
        if(argc <= 0)
            dtk::log::fatal_error("Excepted the functionality name.", 83); //ELOAD
        
        char* functionality = argv[0];
        argv++;argc--;

        //Select the functionality and enable it (if hadn't been yet)
        if(STRCMP_EQ(functionality, "log") || STRCMP_EQ(functionality, "silent")){
            if(project.enabled & pr::PROJECT_ENABLE_LOG){
                dtk::log::warning("\"silent\" already enabled.");
                return 0;
            }

            project.enabled |= pr::PROJECT_ENABLE_LOG;
            ft::update_run_script("run", project.build, fast, project.enabled);
        } else if(STRCMP_EQ(functionality, "test") || STRCMP_EQ(functionality, "tests")){
            if(project.enabled & pr::PROJECT_ENABLE_TESTS){
                dtk::log::warning("\"tests\" already enabled.");
                return 0;
            }

            project.enabled |= pr::PROJECT_ENABLE_TESTS;
            ft::update_run_script("run", project.build, fast, project.enabled);
        } else{
            dtk::log::fatal_error(std::string("Invalid functionality name\"") + functionality + "\"", 130); //ENOEXEC
        }

        project.update();
    } else if(STRCMP_EQ(task, "disable")){
        //Load project data and try to disable the functionality
        fd::ProjectFile project("./.project/project");
        bool fast = project.type == pr::PROJECT_TYPE_SOLUTION;

        //Get functionality and pop it
        if(argc <= 0)
            dtk::log::fatal_error("Excepted the functionality name.", 83); //ELOAD
        
        char* functionality = argv[0];
        argv++;argc--;

        //Select the functionality and disable it (if hadn't been yet)
        if(STRCMP_EQ(functionality, "log")  || STRCMP_EQ(functionality, "silent")){
            if(project.enabled & pr::PROJECT_ENABLE_LOG == 0){
                dtk::log::warning("\"silent\" already disabled.");
                return 0;
            }

            project.enabled &= ~pr::PROJECT_ENABLE_LOG;
            ft::update_run_script("run", project.build, fast, project.enabled);
        } else if(STRCMP_EQ(functionality, "test") || STRCMP_EQ(functionality, "tests")){
            if(project.enabled & pr::PROJECT_ENABLE_TESTS == 0){
                dtk::log::warning("\"tests\" already disabled.");
                return 0;
            }

            project.enabled &= ~pr::PROJECT_ENABLE_TESTS;
            ft::update_run_script("run", project.build, fast, project.enabled);
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