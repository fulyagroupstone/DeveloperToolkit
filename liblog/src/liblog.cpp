#include "../headers/liblog.hpp"

#include <iostream>

#include <cerrno>
#include <cstring>

namespace dtk::log{
    void terminate(int code){
        std::cerr << "Terminated." << std::endl;
        exit(code);
    }

    void fatal_error(const std::string& msg, int code){
        fatal_error(msg.c_str(), code);
    }

    void fatal_error(const char* msg, int code){
        std::cerr << "Fatal ";
        error(msg, code);
    }

    void error(const std::string& msg, int code){
        error(msg.c_str(), code);
    }

    void error(const char* msg, int code){
        std::cerr << "Error: " << msg << std::endl;

        if(code != 0)
            terminate(code);
    }

    void warning(const std::string& msg){
        warning(msg.c_str());
    }

    void warning(const char* msg){
        std::cerr << "Warning: " << msg << std::endl;
    }

    void info(const std::string& msg){
        info(msg.c_str());
    }

    void info(const char* msg){
        std::cout << "Info: " << msg << std::endl;
    }

    void _assert(bool b, const char* msg, int code){
        if(b){
            std::cerr << "Assertion: " << msg << std::endl;
            
            if(code != 0)
                terminate(code);
        }
    }

    void _assert_errno(){
        _assert(errno != 0, strerror(errno), errno);
    }
}