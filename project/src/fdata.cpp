#include <fdata.hpp>

#include <fstream>
#include <sstream>

#include <common.hpp>

#include <liblog.hpp>

namespace fdata{
    ProjectFile::ProjectFile(const std::string& path){
        this->path = path;

        bool name_set = false;
        bool type_set = false;
        bool build_set = false;
        bool enable_set = false;

        //Open the project file
        std::ifstream project(path);
        if(!project.good())
            dtk::log::error("Failed to open project file.", 83); //ELOAD

        //Read the file line-by-line
        std::string line;
        while(getline(project, line)){
            if(line.empty() || line[0] != '#') //Everythink witch does not start by the '#' is a comment
                continue;
            
            if(line.starts_with("#Name: ")){ //Load the name, return an error if redefinition
                if(name_set)
                    dtk::log::error("Project file internal error: Name redefinition.", 83); //ELOAD

                name = line.substr(7);
                name_set = true;
            } else if(line.starts_with("#Type: ")){ //Load the type, return an error if redefinition
                if(type_set)
                    dtk::log::error("Project file internal error: Type redefinition.", 83); //ELOAD

                type = (project::type) std::stoi(line.substr(7));
                type_set = true;
            } else if(line.starts_with("#Build: ")){ //Load the build system, return an error if redefinition
                if(build_set)
                    dtk::log::error("Project file internal error: Type redefinition.", 83); //ELOAD

                build = (project::build) std::stoi(line.substr(8));
                build_set = true;
            } else if(line.starts_with("#Enable: ")){ //Load the enabled functionality, return an error if redefinition
                if(enable_set)
                    dtk::log::error("Project file internal error: Enable value redefinition.", 83); //ELOAD

                enabled = std::stoi(line.substr(9));
                enable_set = true;
            } else{
                dtk::log::error("Project file internal error: Unknown \"" + line + "\"", 83); //ELOAD
            }
        }

        project.close();

        //Validate the data

        if(!name_set) //Name is required
            dtk::log::error("Project file internal error: No name definition.", 83); //ELOAD
        
        //The type and the build system have default values
        if(!type_set){
            dtk::log::warning("Project file internal error: No type definition. Default: PROJECT");
            type = project::PROJECT_TYPE_PROJECT;
        }

        if(!build_set){
            dtk::log::warning("Project file internal error: No build system definition. Default: MAKE");
            build = project::PROJECT_BUILD_MAKE;
        }
    }

    void ProjectFile::update(){
        //Update the project file
        std::ofstream project(path);
        if(!project.good())
            dtk::log::error("Failed to open project file.", 83); //ELOAD
        
        project << "#Name: " << name << std::endl;
        project << "#Type: " << type << std::endl;
        project << "#Build: " << build << std::endl;
        project << "#Enable: " << enabled << std::endl;

        project.close();
    }

    ProjectTemplate::ProjectTemplate(const std::string& path){
        //Open template file
        std::ifstream file(path);
        if(!file.good())
            dtk::log::error("Failed to open template file.", 83); //ELOAD

        //Load the file to EOF or '#End' (Unused in this case)
        load(file, "End");

        file.close();
    }

    void ProjectTemplate::load(std::ifstream& file, const std::string& seperator){
        //Loads template data

        bool name_set = false;

        bool makefile_set = false;
        bool bash_set = false;

        std::string line;

        //Load data
        while(getline(file, line)){
            if(line.empty() || !line.starts_with("#")) //Everything witch doesn't start with '#' is a comment in this case
                continue;

            if(line.starts_with("#" + seperator)) //Read only to seperator
                break;
            
            if(line.starts_with("#Name: ")){ //Get name, return an error if redefinition
                if(name_set)
                    dtk::log::error("Template file internal error: Name redefinition.", 83); //ELOAD

                name = line.substr(7);
                name_set = true;
            } else if(line.starts_with("#!DIR: ")){ //Add a directory, return a warning if already exists
                std::string dir = line.substr(7);

                for(auto& d: directories){
                    if(d == dir)
                        dtk::log::warning("Template file internal error: DIR already exists.");
                }

                directories.push_back(dir);
            } else if(line.starts_with("#!FILE: ")){ //Get text to '#!END' and interpret as file, if filename == makefile or bash get it as special case
                std::string file_name = line.substr(8);

                std::stringstream ss;

                while(getline(file, line)){
                    if(line.starts_with("#!END"))
                        break;
                    
                    ss << line << std::endl;
                }

                if(file_name == "makefile"){ //makefile case
                    if(makefile_set)
                        dtk::log::error("Template file internal error: makefile redefinition.", 83); //ELOAD

                    makefile = ss.str();
                    makefile_set = true;
                } else if(file_name == "bash"){ //bash case
                    if(bash_set)
                        dtk::log::error("Template file internal error: bash redefinition.", 83); //ELOAD
                    
                    bash = ss.str();
                    bash_set = true;
                } else{ //file case
                    files.push_back(std::pair<std::string, std::string>(file_name, ss.str()));
                }
            } else{
                dtk::log::error("Template file internal error: Unknown \"" + line + "\".", 83); //ELOAD
            }
        }

        //Validate the data

        if(files.size() <= 0)
            dtk::log::error("Template file internal error: Excepted file.", 83); //ELOAD
        
        if(!makefile_set)
            dtk::log::error("Template file internal error: makefile undefined.", 83); //ELOAD

        if(!bash_set)
            dtk::log::error("Template file internal error: bash undefined.", 83); //ELOAD
    }
    
    std::string ProjectTemplate::to_string() const{
        //Restore the template to file-form

        std::stringstream ss;
        ss << "#Name: " << name << std::endl;
        ss << std::endl;

        for(auto& dir: directories)
            ss << "#!DIR: " << dir << std::endl;
        
        ss << std::endl;
        
        for(auto& file: files){
            ss << "#!FILE: " << file.first << std::endl;
            ss << file.second << std::endl;
            ss << "#!END" << std::endl;
        }

        ss << std::endl;

        ss << "#!FILE: makefile" << std::endl << makefile << std::endl << "#!END" << std::endl;
        ss << std::endl;

        ss << "#!FILE: bash" << std::endl << bash << std::endl << "#!END" << std::endl;
        ss << std::endl;

        return ss.str();
    }

    ProjectSrc ProjectTemplate::compile(const std::string& name, project::build b, bool fast){
        //Create ProjectSrc

        std::vector<std::pair<std::string, std::string>> src;
        std::pair<std::string, std::string> build;
        std::vector<std::string> dir;

        std::string prefix = name + "/";

        //Varibles
        std::string includes = (!fast)?"-I ../includes":"";
        std::string c_flags = "-Wall";
        std::string cxx_flags = "-Wall";
        std::string ld_flags = "";

        //For each file replase predefined varibles and push it as src file
        for(auto file: files){
            std::string _name = prefix + file.first;
            std::string _cnt = file.second;

            common::str_replase_all(_name, "#!NAME!#", name);

            common::str_replase_all(_cnt, "#!INCLUDES!#", includes);
            common::str_replase_all(_cnt, "#!C_FLAGS!#", c_flags);
            common::str_replase_all(_cnt, "#!CXX_FLAGS!#", cxx_flags);
            common::str_replase_all(_cnt, "#!LD_FLAGS!#", ld_flags);
            common::str_replase_all(_cnt, "#!NAME!#", name);
            common::str_replase_all(_cnt, "    ", "\t");

            src.emplace_back(_name, _cnt);
        }

        //Choose and prepare build file, add it to src
        std::string build_name = prefix;
        std::string build_cnt;
        switch(b){
            case project::PROJECT_BUILD_MAKE:
                build_name += "makefile";
                build_cnt = makefile;
                break;
            case project::PROJECT_BUILD_BASH:
                if(fast)
                    build_name += ".scripts/make.sh";
                else
                    build_name += "make.sh";
                build_cnt = bash;
                break;
        }

        common::str_replase_all(build_cnt, "#!NAME!#", name);
        common::str_replase_all(build_cnt, "#!INCLUDES!#", includes);
        common::str_replase_all(build_cnt, "#!C_FLAGS!#", c_flags);
        common::str_replase_all(build_cnt, "#!CXX_FLAGS!#", cxx_flags);
        common::str_replase_all(build_cnt, "#!LD_FLAGS!#", ld_flags);
        common::str_replase_all(build_cnt, "    ", "\t");
        
        build = std::pair<std::string, std::string>(build_name, build_cnt);

        //Prepare dirs
        for(auto& d: directories)
            dir.push_back(prefix + d);
        
        return {src, build, dir}; //Return new ProjectSrc
    }

    std::vector<ProjectTemplate> load_project_templates(const std::string& p){
        //Loads templates from multi-definition file (or something like this)

        std::vector<ProjectTemplate> templates;

        //Open templates file
        std::ifstream file(p); //p = ".project/templates"
        if(!file.good())
            dtk::log::error("Failed to open file.", 111); //EACCES

        //Read the file line-by-line
        std::string line;
        while(getline(file, line)){
            if(line.empty() || !line.starts_with("#")) //Every line witch don't start by '#' is a comment
                continue;
            
            if(line.starts_with("#Begin")){ //If starts by '#Begin' begin loading new ProjectTemplate to '#End'
                ProjectTemplate p;
                p.load(file, "End");
                templates.push_back(p);
            } else{
                dtk::log::error("Multi-template file internal error: Unknown \"" + line + "\".", 83); //ELOAD
            }
        }
        
        //Close the file
        file.close();

        return templates;
    }
}