#include "AmplifyEffect.hpp"

EffectOperation *AmplifyEffect::clone() const {
    return new AmplifyEffect(*this);
}

AmplifyEffect::AmplifyEffect(double f): factor(f) {

}

double AmplifyEffect::operator()(double sample) const{
    return sample * factor;
}
