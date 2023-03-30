#pragma once 

namespace project{
    typedef enum{
        PROJECT_TYPE_PROJECT,
        PROJECT_TYPE_EXE,
        PROJECT_TYPE_LIB,
        PROJECT_MODULE_TYPE_EXE,
        PROJECT_MODULE_TYPE_LIB
    } type;

    typedef enum{
        PROJECT_BUILD_MAKE,
        PROJECT_BUILD_BASH
    } build;

    typedef enum{
        PROJECT_ENABLE_LOG = 1
    } enable;
}