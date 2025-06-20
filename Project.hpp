//
// Created by vikso on 19/06/2025.
//

#ifndef DAW_PROJECT_HPP
#define DAW_PROJECT_HPP

#include "Track.hpp"
class Project {
private:
    std::vector<Track> tracks;


public:
    Project(double frequency, int trackAmount);


    static Project createNew(const char* name);

    void save(const char*projectName);
    void load(const char& projectName);

};


#endif //DAW_PROJECT_HPP
