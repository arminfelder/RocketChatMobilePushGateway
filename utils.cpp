//
// Created by armin on 16.11.17.
//

#include "utils.h"

namespace utils{
    std::vector<std::string> getSegments(const std::string &pPath){
        size_t current = 0;
        size_t next = -1;
        std::vector<std::string> segments;
        do{
            current = next +1;
            next = pPath.find_first_of("/",current);
            std::string segment = pPath.substr(current, next-current);
            if(segment != "") {
                segments.push_back(segment);
                std::cout << segment << std::endl;
            }
        }while(next != std::string::npos);
        return segments;
    }
}