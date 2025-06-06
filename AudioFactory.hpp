//
// Created by vikso on 22/05/2025.
//

#ifndef DAW_AUDIOFACTORY_HPP
#define DAW_AUDIOFACTORY_HPP

#include "Audio.hpp"

/// Singleton Factory class
class AudioFactory {
private:

    std::vector<const AudioCreator *> creators;
    size_t size;

    const AudioCreator *getCreator(const std::string &str) const;

    AudioFactory();

    AudioFactory(const AudioFactory &other) = delete;

    AudioFactory &operator=(const AudioFactory &other) = delete;


public:
    static AudioFactory &getInstance();

    void registerAudio(const AudioCreator *creator);

    Audio *createAudio(std::istream &input);

};


#endif //DAW_AUDIOFACTORY_HPP
