#include <common.hpp>

namespace common{
    void str_replase_all(std::string& str, const std::string& from, const std::string& to){
        if(str.empty() || from.empty())
            return;
        
        std::size_t pos = 0;

        while((pos = str.find(from, pos)) != std::string::npos){
            str.replace(pos, from.length(), to);
            pos += to.length();
        }
    }
}