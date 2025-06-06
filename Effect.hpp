//
// Created by vikso on 24/05/2025.
//

#ifndef DAW_EFFECT_HPP
#define DAW_EFFECT_HPP

#include "Audio.hpp"


struct Amplify {
private:
    double factor;
public:
    Amplify(double factor) : factor(factor) {};

    double operator()(double s) const {
        return s * 2.0f;
    }
};

struct Normalize {
    double gain = 1.0;
    double target;

    Normalize(const Audio &a, double targetAmp = 1.0) : target(targetAmp) {
        double maxAmp = 0.0;
        for (std::size_t i = 0; i < a.getSampleSize(); ++i) {
            maxAmp = std::max(maxAmp, std::abs(a[i]));
        }
        gain = (maxAmp > 0.000001) ? (target / maxAmp) : 1.0;
    }

    double operator()(double s) const {
        return s * gain;
    }
};

struct FadeIn {
    double fadeDuration;  // in seconds
    double sampleRate;

    FadeIn(double duration, double rate)
            : fadeDuration(duration), sampleRate(rate) {}

    double operator()(std::size_t i, std::size_t totalSamples) const {
        std::size_t fadeSamples = static_cast<std::size_t>(fadeDuration * sampleRate);
        if (i >= fadeSamples) return 1.0;
        return static_cast<double>(i) / fadeSamples;
    }
};

struct FadeOut {
    double fadeDuration;  // in seconds
    double sampleRate;

    FadeOut(double duration, double rate)
            : fadeDuration(duration), sampleRate(rate) {}

    double operator()(std::size_t i, std::size_t totalSamples) const {
        std::size_t fadeSamples = static_cast<std::size_t>(fadeDuration * sampleRate);
        if (i >= totalSamples) return 0.0;
        if (i + fadeSamples >= totalSamples) {
            return static_cast<double>(totalSamples - i) / fadeSamples;
        }
        return 1.0;
    }
};

template<typename EffectOperation>
class Effect : public Audio {
private:
    Audio *base;
    EffectOperation operation;//copy

public:
    Effect(const Audio *input, EffectOperation op);

    Effect(const Effect &other);

    Effect &operator=(const Effect &other);

    ~Effect() {
        delete base;

    }

    Audio *clone() const override;

    double operator[](std::size_t i) const override;

    double &operator[](std::size_t) override;

    std::ostream &printToStream(std::ostream &out) const override;


};

template<typename EffectOperation>
std::ostream &Effect<EffectOperation>::printToStream(std::ostream &out) const {
    out << "EffectAudio over: ";
    return base->printToStream(out);
}

template<typename EffectOperation>
double &Effect<EffectOperation>::operator[](std::size_t) {
    throw std::logic_error("EffectAudio does not support sample modification.");
}

template<typename EffectOperation>
double Effect<EffectOperation>::operator[](std::size_t i) const {
    return operation((*base)[i]);
}

template<typename EffectOperation>
Audio *Effect<EffectOperation>::clone() const {
    return new Effect<EffectOperation>(*this);
}

template<typename EffectOperation>
Effect<EffectOperation> &Effect<EffectOperation>::operator=(const Effect &other) {
//    TODO check exception safety
    if (this != &other) {
        Audio *temp = other.base->clone();
        delete base;
        base = temp;
        operation = other.operation;
        setSampleRate(base->getSampleRate());
        setDuration(base->getDuration());
        setSampleSize(base->getSampleSize());
    }
    return *this;
}

template<typename EffectOperation>
Effect<EffectOperation>::Effect(const Effect &other) : base(other.base->clone()), operation(other.operation) {
    setSampleRate(base->getSampleRate());
    setDuration(base->getDuration());
    setSampleSize(base->getSampleSize());
}

template<typename EffectOperation>
Effect<EffectOperation>::Effect(const Audio *input, EffectOperation op) :base(input->clone()),
                                                                         operation(op) {
    this->setDuration(base->getDuration());
    this->setSampleRate(base->getSampleRate());
    this->setSampleSize(base->getSampleSize());

}

class EffectCreator : AudioCreator {
public:
    EffectCreator();

    Audio *createAudio(std::istream &in) const override;
};



#endif //DAW_EFFECT_HPP




