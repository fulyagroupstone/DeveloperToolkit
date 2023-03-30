#pragma once

#include <string>

namespace dtk::log{
    void terminate(int code);
    
    void error(const std::string& msg, int code = 0);
    void error(const char* msg, int code = 0);

    void fatal_error(const std::string& msg, int code = 0);
    void fatal_error(const char* msg, int code = 0);

    void warning(const std::string& msg);
    void warning(const char* msg);

    void info(const std::string& msg);
    void info(const char* msg);

    void _assert(bool b, const char* msg, int code);
    void _assert_errno();

    inline void assert(bool b, const std::string& msg, int code = 0){
        #ifdef DTK_LOG_ENABLE_ASSERTION
        _assert(b, msg.c_str(), code);
        #endif
    }

    inline void assert(bool b, const char* msg, int code = 0){
        #ifdef DTK_LOG_ENABLE_ASSERTION
        _assert(b, msg, code);
        #endif
    }

    inline void assert_errno(){
        #ifdef DTK_LOG_ENABLE_ASSERTION
        _assert_errno();
        #endif
    }


}