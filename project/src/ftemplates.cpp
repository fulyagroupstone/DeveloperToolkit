#include <ftemplates.hpp>

#include <filesystem>
#include <fstream>

#include <liblog.hpp>

namespace fs = std::filesystem;

#define CREATE_DIR(name) if(!fs::create_directory(name)) dtk::log::error("Failed to create directory.", 111); //EACCES
#define CREATE_FILE(name) { std::ofstream file(name); if(!file.good()) dtk::log::error("Failed to create file.", 111); /*EACCES*/ file.close(); }
#define CREATE_SCRIPT(name) { std::ofstream file(name); if(!file.good()) dtk::log::error("Failed to create script file.", 111); /*EACCES*/ file << "#!/bin/bash" << std::endl; file.close(); }
#define FILE_OK(var) if(!var.good()) dtk::log::error("Failed to create file.", 111); //EACCES

namespace ftemplates{
    void create_project(const std::string& name, project::type t, project::build b){
        bool fast = t == project::PROJECT_TYPE_EXE || t == project::PROJECT_TYPE_LIB;

        CREATE_DIR(name);
        CREATE_DIR(name + "/.project");
        CREATE_DIR(name + "/.scripts");

        if(!fast){
            CREATE_DIR(name + "/include");
            CREATE_DIR(name + "/lib");
            CREATE_DIR(name + "/bin");
        }

        std::ofstream project(name + "/.project/project");
        FILE_OK(project)
        
        project << "#Name: " << name << std::endl;
        project << "#Type: " << t << std::endl;
        project << "#Build: " << b << std::endl;
        
        project.close();

        CREATE_FILE(name + "/.project/solutions");

        if(!fast){
            CREATE_SCRIPT(name + "/.scripts/clean.sh");
            CREATE_SCRIPT(name + "/.scripts/make.sh");
        }

        if(t == project::PROJECT_TYPE_LIB){
            std::ofstream script_test(name + "/.scripts/test.sh");
            FILE_OK(script_test);

            script_test << "#!/bin/bash" << std::endl;
            script_test << std::endl;
            script_test << "make test/test" << std::endl;

            script_test.close();
        } else{
            CREATE_SCRIPT(name + "/.scripts/test.sh");
        }

        update_run_script(name + "/run", b, fast);
    }

    void update_run_script(const std::string& p, project::build b, bool fast, bool log){
        std::ofstream run(p);// name + "/run"
        FILE_OK(run);

        run << "#!/bin/bash" << std::endl;
        run << std::endl;

        if(log){
            run << "#Get the log file name" << std::endl;
            run << "log=$(date +'log/log%d.%m.%Y.txt')" << std::endl;
            run << "mkdir log 2>/dev/null" << std::endl;
            run << std::endl;
        }

        if(!fast){
            run << "#Clean" << std::endl;
            if(log)
                run << "echo \"#Clean:\" >> $log" << std::endl;
            run << "./.scripts/clean.sh" << (log?" >> $log":"") << std::endl;
            if(log)
                run << "echo \"\" >> $log" << std::endl;
            run << std::endl;
        }

        run << "#Make" << std::endl;
        if(log)
            run << "echo \"#Make:\" >> $log" << std::endl;

        if(fast && b != project::PROJECT_BUILD_BASH)
            run << "make" << (log?" >> $log":"") << std::endl;
        else
            run << "./.scripts/make.sh" << (log?" >> $log":"") << std::endl;
        
        if(log)
            run << "echo \"\" >> $log" << std::endl;
        
        run << std::endl;
        run << "#Test" << std::endl;
        if(log)
            run << "echo \"#Test:\" >> $log" << std::endl;
        run << "./.scripts/test.sh" << (log?" >> $log":"") << std::endl;

        run << std::endl;

        run.close();
    }

    void create_src_file(const std::string& name, project::type t){
        CREATE_DIR(name + "/src");
        CREATE_DIR(name + "/headers");

        std::string file_path = name + "/src/";
        switch(t){
            case project::PROJECT_MODULE_TYPE_EXE:
            case project::PROJECT_TYPE_EXE:
                file_path += "main";
                break;
            case project::PROJECT_MODULE_TYPE_LIB:
            case project::PROJECT_TYPE_LIB:
                file_path += name;
                break;
            default:
                file_path += "unnamed";
                break;
        }
        file_path += ".cpp";

        std::ofstream file(file_path);
        FILE_OK(file);

        if(t == project::PROJECT_MODULE_TYPE_LIB || t == project::PROJECT_TYPE_LIB){
            file << "#include \"../headers/" << name << ".hpp\"" << std::endl;
            file << std::endl;
        }

        file << "#include <iostream>" << std::endl;
        file << std::endl;

        switch(t){
            case project::PROJECT_MODULE_TYPE_EXE:
            case project::PROJECT_TYPE_EXE:
                file << "int main(int argc, char** argv){" << std::endl;
                break;
            case project::PROJECT_MODULE_TYPE_LIB:
            case project::PROJECT_TYPE_LIB:
                file << "extern \"C\"{" << std::endl;
                file << "\tvoid hello(){" << std::endl << "\t";
                break;
        }
        
        file << "\tstd::cout << \"Hello World\" << std::endl;" << std::endl << "\t";

        switch(t){
            case project::PROJECT_MODULE_TYPE_EXE:
            case project::PROJECT_TYPE_EXE:
                file << "return 0;" << std::endl;
                break;
            case project::PROJECT_MODULE_TYPE_LIB:
            case project::PROJECT_TYPE_LIB:
                file << "\t}" << std::endl;
                break;
        }
        
        file << "}";

        file.close();

        if(t == project::PROJECT_MODULE_TYPE_LIB || t == project::PROJECT_TYPE_LIB){
            std::ofstream header(name + "/headers/" + name + ".hpp");
            FILE_OK(header);

            header << "#pragma once" << std::endl;
            header << std::endl;
            header << "extern \"C\"{" << std::endl;
            header << "\tvoid hello();" << std::endl;
            header << "}";

            header.close();
        }

        if(t == project::PROJECT_TYPE_LIB){
            CREATE_DIR(name + "/test");

            std::ofstream test(name + "/test/test.cpp");
            FILE_OK(test);

            test << "#include <iostream>" << std::endl;
            test << std::endl;
            test << "#include <" << name << ".hpp>" << std::endl;
            test << std::endl;
            test << "int main(){" << std::endl;
            test << "\thello();" << std::endl;
            test << "\treturn 0;" << std::endl;
            test << "}" << std::endl;

            test.close();
        }
    }

    void create_makefile(const std::string& name, project::type t){
        bool fast = t == project::PROJECT_TYPE_EXE || t == project::PROJECT_TYPE_LIB;

        std::ofstream makefile(name + "/makefile");
        FILE_OK(makefile);

        makefile << "name := " << name << std::endl;
        makefile << "flags := -std=c++2b " << (t == project::PROJECT_MODULE_TYPE_LIB || t == project::PROJECT_TYPE_LIB?"-fPIC":"") << std::endl;
        makefile << std::endl;
        makefile << "includes := headers " << (fast?"":"../include") << std::endl;

        if(t != project::PROJECT_MODULE_TYPE_LIB && t != project::PROJECT_TYPE_LIB) 
            makefile << "libs := " << (fast?"":"-L ../lib") << std::endl;
            
        makefile << std::endl;
        makefile << "bin := $(patsubst src/%,bin/%.o,$(wildcard src/*))" << std::endl;
        makefile << std::endl;
        makefile << ".PHONY: all" << std::endl;

        makefile << "all: clean bin ";
        switch(t){
            case project::PROJECT_MODULE_TYPE_EXE:
            case project::PROJECT_TYPE_EXE:
                makefile << "$(name)";
                break;
            case project::PROJECT_MODULE_TYPE_LIB:
            case project::PROJECT_TYPE_LIB:
                makefile << "$(name).a $(name).so";
        }
        makefile << std::endl;

        makefile << std::endl;
        makefile << "clean:" << std::endl;
        makefile << "\t@echo Cleaning $(name)" << std::endl;
        makefile << "\t@rm -fr bin/*" << std::endl;
        makefile << "\t@rm -f $(name)*" << std::endl;
        makefile << std::endl;
        makefile << "bin:" << std::endl;
        makefile << "\t@mkdir $@" << std::endl;
        makefile << std::endl;
        makefile << "$(bin):bin/%.o:src/%" << std::endl;
        makefile << "\t@echo Compiling $^" << std::endl;
        makefile << "\t@g++ -c -o $@ $^ $(foreach x,$(includes), -I $(x)) $(flags)" << std::endl;
        makefile << std::endl;

        switch(t){
            case project::PROJECT_MODULE_TYPE_EXE:
            case project::PROJECT_TYPE_EXE:
                makefile << "$(name): $(bin)" << std::endl;
                makefile << "\t@echo Linking $@" << std::endl;
                makefile << "\t@g++ -o $@ $^ $(libs) $(flags)" << std::endl;
                break;
            case project::PROJECT_MODULE_TYPE_LIB:
            case project::PROJECT_TYPE_LIB:
                makefile << "$(name).a: $(bin)" << std::endl;
                makefile << "\t@echo Linking $@" << std::endl;
                makefile << "\t@ar cvr $@ $^" << std::endl;
                makefile << std::endl;
                makefile << "$(name).so: $(bin)" << std::endl;
                makefile << "\t@echo Linking $@" << std::endl;
                makefile << "\t@g++ -shared -o $@ $^ $(flags)" << std::endl;
                break;
        }

        if(t == project::PROJECT_TYPE_LIB){
            makefile << std::endl;
            makefile << "test/test:test/test.cpp" << std::endl;
            makefile << "\t@echo Test of $(name)" << std::endl;
            makefile << "\t@g++ -o $@ $^ $(foreach x,$(includes), -I $(x)) $(flags) $(name).a" << std::endl;
        }

        makefile.close();
    }


    void create_bash(const std::string& name, project::type t){
        bool fast = t == project::PROJECT_TYPE_EXE || t == project::PROJECT_TYPE_LIB;

        std::ofstream bash(name + "/.scripts/make.sh");
        FILE_OK(bash);

        bash << "name='" << name << "'" << std::endl;
        bash << "flags='-std=c++2b " << (t == project::PROJECT_MODULE_TYPE_LIB || t == project::PROJECT_TYPE_LIB?"-fPIC'":"'") << std::endl;
        bash << std::endl;
        bash << "includes='-I headers " << (fast?"'":"-I ../include'") << std::endl;

        if(t != project::PROJECT_MODULE_TYPE_LIB && t != project::PROJECT_TYPE_LIB) 
            bash << "libs='" << (fast?"'":"-L ../lib'") << std::endl;

        bash << std::endl;
        bash << "#Clean:" << std::endl;
        bash << "echo Cleaning $name" << std::endl;
        bash << "rm -fr bin/*" << std::endl;
        bash << "rm -f $name*" << std::endl;
        bash << std::endl;
        bash << "#Create bin:" << std::endl;
        bash << "mkdir bin 2>/dev/null" << std::endl;
        bash << std::endl;
        bash << "#Compile sources" << std::endl;
        bash << "for src in ./src/*" << std::endl;
        bash << "do" << std::endl;
        bash << "\techo Compiling $src" << std::endl;
        bash << "\tg++ -c -o bin/$( echo $src | cut -d '/' -f 2).o $src $includes $flags" << std::endl;
        bash << "done" << std::endl;
        bash << std::endl;

        bash << "#Link:" << std::endl;
        switch(t){
            case project::PROJECT_MODULE_TYPE_EXE:
            case project::PROJECT_TYPE_EXE:
                bash << "echo Linking $name" << std::endl;
                bash << "g++ -o $name bin/*.o $libs $flags" << std::endl;
                break;
            case project::PROJECT_MODULE_TYPE_LIB:
            case project::PROJECT_TYPE_LIB:
                bash << "echo Linking $name.a" << std::endl;
                bash << "ar cvr $name.a bin/*" << std::endl;
                bash << std::endl;
                bash << "echo Linking $name.so" << std::endl;
                bash << "g++ -shared -o $name.so bin/*.o $flags" << std::endl;
                break;
        }

        if(t == project::PROJECT_TYPE_LIB){
            bash << std::endl;
            bash << "#Compile test:" << std::endl;
            bash << "echo Test of $name" << std::endl;
            bash << "g++ -o test/test test/test.cpp $includes $flags $name.a" << std::endl;
        }

        bash.close();
    }
}