//
// Created by vikso on 21/05/2025.
//

#include "Audio.hpp"
#include "AudioFactory.hpp"

Audio::Audio() : sampleRate(0.0f), duration(0.0), sampleSize(0) {

}

float Audio::getSampleRate() const {
    return this->sampleRate;
}

double Audio::getDuration() const {
    return this->duration;
}

size_t Audio::getSampleSize() const {
    return this->sampleSize;
}

void Audio::print() const {
    std::cout << this->duration << '\n';
    std::cout << this->sampleRate << '\n';
    std::cout << this->sampleSize << '\n';
}


void Audio::setDuration(double duration) {
    if (!isValidDuration(duration)) {
        throw std::invalid_argument("Invalid Duration");
    }
    this->duration = duration;
}

void Audio::setSampleRate(float rate) {
    if (!isValidSampleRate(rate)) {
        throw std::invalid_argument("Invalid Sample Rate");
    }
    this->sampleRate = rate;
}

void Audio::setSampleSize(size_t size) {
    if (!isValidSampleSize(size)) {
        throw std::invalid_argument("Invalid Sample Size");
    }
    this->sampleSize = size;
}

bool Audio::isValidSampleRate(float rate) {
    return rate > 0;
}

bool Audio::isValidDuration(double duration) {
    return duration > 0;
}

bool Audio::isValidSampleSize(size_t size) {
    return size > 0;
}

//Creator class
bool AudioCreator::supportsAudio(const std::string &cmd) const {
    return this->createCommand == cmd;
}

AudioCreator::AudioCreator(const char *command) : createCommand(command) {
    AudioFactory::getInstance().registerAudio(this);
}
