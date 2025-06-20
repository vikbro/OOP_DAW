//
// Created by vikso on 21/05/2025.
//

#ifndef DAW_AUDIO_HPP
#define DAW_AUDIO_HPP
#pragma once

using sample = double;

#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

/// Abstract base class for Audio
class Audio {
protected:
//    std::vector<double> samples;
//    TODO string name
    float sampleRate;
    double duration;
    size_t sampleSize;
    std::string audioName;

public:
    Audio();

    virtual ~ Audio() noexcept = default;

/// Getter for Duration
///
/// Ensures valid setting of the duration of the Audio if you set it bigger then it is it will fill with silence
/// \param duration
    void setDuration(double duration);
    void setSampleRate(float rate);
    void setSampleSize(size_t size);

    float getSampleRate() const;
    double getDuration() const;
    size_t getSampleSize() const;

    static bool isValidSampleRate(float rate);
    static bool isValidSampleSize(size_t size);
    static bool isValidDuration(double duration);

    virtual void print() const;
    virtual std::ostream &printToStream(std::ostream &out) const = 0;

    virtual double operator[](std::size_t index) const = 0 ;
    virtual double &operator[](std::size_t index) = 0;

    virtual Audio *clone() const = 0;
};
//TODO add operator << for all Audio types
class AudioCreator {
protected:
//    command that will be read from file/console
    std::string createCommand;

public:

    AudioCreator(const char *command);

    virtual ~AudioCreator() = default;

    bool supportsAudio(const std::string &s) const;

    virtual Audio *createAudio(std::istream &in) const = 0;

};

#endif //DAW_AUDIO_HPP
