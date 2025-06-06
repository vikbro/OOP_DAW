//
// Created by vikso on 06/06/2025.
//

#ifndef DAW_GENERATOR_HPP
#define DAW_GENERATOR_HPP

#include "../Audio.hpp"
#include <cmath>

struct SineGenerator {
    float frequency = 440.0;
    float rate = 44100.0;

    sample operator()(std::size_t i) const {
        return sin(2 * 3.14159265 * frequency * i / rate);
    }
};

template<typename Generator>
class GeneratorAudio : public Audio {
private:
    Generator generator;

public:

    GeneratorAudio(float rate, double dur, const Generator &gen);

    Audio *clone() const override;

    double operator[](std::size_t i) const override;

    double &operator[](std::size_t) override;

    std::ostream &printToStream(std::ostream &out) const override;

};

template<typename Generator>
std::ostream &GeneratorAudio<Generator>::printToStream(std::ostream &out) const {
    out << "GeneratorAudio: " << sampleSize << " samples @ " << sampleRate << "Hz\n";
    return out;
}

template<typename Generator>
double &GeneratorAudio<Generator>::operator[](std::size_t) {
    throw std::logic_error("GeneratorAudio does not support sample modification.");
}

template<typename Generator>
double GeneratorAudio<Generator>::operator[](std::size_t i) const {
    return (i < sampleSize) ? generator(i) : 0.0;
}

template<typename Generator>
GeneratorAudio<Generator>::GeneratorAudio(float rate, double dur, const Generator &gen) : generator(gen) {
    setSampleRate(rate);
    setDuration(dur);
    setSampleSize(static_cast<size_t>(rate * dur));
}

template<typename Generator>
Audio *GeneratorAudio<Generator>::clone() const {
    return new GeneratorAudio<Generator>(*this);
}

#endif //DAW_GENERATOR_HPP
