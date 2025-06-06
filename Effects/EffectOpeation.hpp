//
// Created by vikso on 30/05/2025.
//

#ifndef DAW_EFFECTOPEATION_HPP
#define DAW_EFFECTOPEATION_HPP

//TODO ASK about required operator() overloading
class EffectOperation {
public:
//    virtual  operator() const = 0;
    virtual EffectOperation* clone() const = 0;
    virtual ~EffectOperation() = default;
};

#endif //DAW_EFFECTOPEATION_HPP
