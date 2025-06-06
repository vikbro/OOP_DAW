//
// Created by vikso on 30/05/2025.
//

#ifndef DAW_FADEINOPERATION_HPP
#define DAW_FADEINOPERATION_HPP
#include "EffectOpeation.hpp"
#include "../Utils.hpp" // std iostream

class FadeInOperation : EffectOperation {
private:
    double fadeDuration;
    float sampleRate;

public:
    FadeInOperation(double fadeDuration, float sampleRate)
            : fadeDuration(fadeDuration), sampleRate(sampleRate) {}
//Functor class
    double operator()(double sample, std::size_t index) const;

    EffectOperation* clone() const override ;
};


#endif //DAW_FADEINOPERATION_HPP
