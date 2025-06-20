#include <limits>
#include "AudioFactory.hpp"

AudioFactory::AudioFactory() {
    std::clog << "Created Audio factory" << std::endl;
}

AudioFactory &AudioFactory::getInstance() {
    static AudioFactory audioFactory;
    return audioFactory;
}

void AudioFactory::registerAudio(const AudioCreator *creator) {
    creators.push_back(creator);
    size++;
}

Audio *AudioFactory::createAudio(std::istream &input) {
    std::string command;
    input >> command;
    const AudioCreator *creator = getCreator(command);
    if (creator) {
        return creator->createAudio(input);
    } else {
        input.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        throw std::runtime_error("Could not find appropriate Audio creator");
        return nullptr;
    }
}

const AudioCreator *AudioFactory::getCreator(const std::string &str) const {
    for (int i = 0; i < size; ++i) {
        if (creators[i]->supportsAudio(str))
            return creators[i];
    }
    return nullptr;
}





