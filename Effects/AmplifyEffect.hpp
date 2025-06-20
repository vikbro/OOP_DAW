#ifndef DAW_AMPLIFYEFFECT_HPP
#define DAW_AMPLIFYEFFECT_HPP
#include "EffectOpeation.hpp"

//TODO MAKE A EFFECT CREATOR LIKE IM AUDIO CREATOR
class AmplifyEffect : EffectOperation {
private:
    double factor;
public:
    AmplifyEffect(double f);
//    double apply(double sample) const override;
//        Implementation of the logic of AmplifyEffect
    double operator()(double factor) const;
    EffectOperation* clone() const override;

};

//static AmplifyEffect __

#endif //DAW_AMPLIFYEFFECT_HPP
