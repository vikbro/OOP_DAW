//
// Created by vikso on 06/06/2025.
//

#ifndef DAW_TRACK_HPP
#define DAW_TRACK_HPP

#include "Audio.hpp"

//TODO maybe Track is also Audio
//TODO Check Track doesn't contain Track
class Track {
private:
    std::vector<Audio *> audio;
    size_t audioSize;

    double frequency;


public:
    void applyEffect();

};


#endif //DAW_TRACK_HPP
