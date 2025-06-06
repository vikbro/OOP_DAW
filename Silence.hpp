//
// Created by vikso on 22/05/2025.
//

#ifndef DAW_SILENCE_HPP
#define DAW_SILENCE_HPP

#include "Audio.hpp"

class Silence : public Audio {
public:
    Silence(double duration, float sampleRate);

    double operator[](size_t index) const override;

    double &operator[](size_t index) override;

    Silence *clone() const override;

    std::ostream &printToStream(std::ostream &out) const override;

    void print() const override;
};

class SilenceCreator : AudioCreator {
public:
    SilenceCreator();

    Audio *createAudio(std::istream &in) const override;
};

#endif //DAW_SILENCE_HPP
