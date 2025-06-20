#include "FadeInOperation.hpp"

//TODO not sure this works correctly
double FadeInOperation::operator()(double sample, std::size_t index) const {
    double time = index / sampleRate;
    double gain = (time >= fadeDuration) ? 1.0 : time / fadeDuration;
    return sample * gain;
}

EffectOperation *FadeInOperation::clone() const {
    return new FadeInOperation(*this);
}
