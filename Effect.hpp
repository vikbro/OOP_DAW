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
        return s * factor;
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
//        TODO check the logic of this operation
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
    //TODO. Should I use a reference to Audio instead of a pointer? Expensive copy
    const Audio *base;
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
    out << this->getDuration() << '\t' << this->getSampleRate() << '\t' << this->getSampleSize() << '\t';
    for (size_t i = 0; i < this->getSampleSize(); ++i) {
        out << (*this)[i] << ' '; // Use the effect's own operator[] const
    }
    out << std::endl;
    return out;
}

template<typename EffectOperation>
double &Effect<EffectOperation>::operator[](std::size_t i) {
    throw std::logic_error("Effect does not support sample modification.");
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

// Specialization for FadeIn
//TODO I might not need this specializatoins
template<>
inline double Effect<FadeIn>::operator[](std::size_t i) const {
    // The FadeIn operation needs current index and total samples.
    // (*base)[i] gives the sample value.
    // operation(i, base->getSampleSize()) gives the fade multiplier.
    // Note: The FadeIn struct uses the sampleRate passed to its constructor for fadeSamples calculation.
    // The base->getSampleSize() is used here as totalSamples for the multiplier logic.
    return (*base)[i] * operation(i, base->getSampleSize());
}

// Specialization for FadeOut
//TODO I might not need this specializatoins
template<>
inline double Effect<FadeOut>::operator[](std::size_t i) const {
    // Similar to FadeIn, provides current index and total samples to the operation.
    return (*base)[i] * operation(i, base->getSampleSize());
}

class EffectCreator : public AudioCreator { // Made inheritance public
public:
    explicit EffectCreator(const char* command = "EFCT"); // Added explicit and default argument
    Audio* createAudio(std::istream& in) const override;
};



#endif //DAW_EFFECT_HPP




