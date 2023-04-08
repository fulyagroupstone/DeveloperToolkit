#pragma once 

namespace project{
    typedef enum{
        PROJECT_TYPE_PROJECT,
        PROJECT_TYPE_SOLUTION
    } type;

    typedef enum{
        PROJECT_BUILD_MAKE,
        PROJECT_BUILD_BASH
    } build;

    typedef enum{
        PROJECT_ENABLE_LOG = 1,
        PROJECT_ENABLE_TESTS = 2
    } enable;
}