//
// Created by vikso on 24/05/2025.
//

#include "Effect.hpp"

EffectCreator::EffectCreator(): AudioCreator("EFCT")  {
    std::clog <<"Created Effect" << std::endl;
}

Audio *EffectCreator::createAudio(std::istream &in) const {
    try {
        std::string effectType;
        in >> effectType;

        if (effectType == "AMPL") {
            double factor;
            in >> factor;

            // След това идва оригиналното аудио, което ще обработваме:
            std::string dummy;
            in >> dummy; // напр. "silence", "sine", и т.н.
            AudioCreator *innerCreator = getAudioCreatorFor(dummy);
            if (!innerCreator) throw std::runtime_error("Unknown inner audio type: " + dummy);

            Audio *base = innerCreator->createAudio(in);
            return new Effect<Amplify>(base, Amplify(factor));
        }

        else if (effectType == "NORM") {
            std::string dummy;
            in >> dummy;
            AudioCreator *innerCreator = getAudioCreatorFor(dummy);
            if (!innerCreator) throw std::runtime_error("Unknown inner audio type: " + dummy);

            Audio *base = innerCreator->createAudio(in);
            return new Effect<Normalize>(base, Normalize(*base));
        }

        else if (effectType == "FDIN") {
            double duration;
            in >> duration;

            std::string dummy;
            in >> dummy;
            AudioCreator *innerCreator = getAudioCreatorFor(dummy);
            if (!innerCreator) throw std::runtime_error("Unknown inner audio type: " + dummy);

            Audio *base = innerCreator->createAudio(in);
            return new Effect<FadeIn>(base, FadeIn(duration, base->getSampleRate()));
        }

        else if (effectType == "FOUT") {
            double duration;
            in >> duration;

            std::string dummy;
            in >> dummy;
            AudioCreator *innerCreator = getAudioCreatorFor(dummy);
            if (!innerCreator) throw std::runtime_error("Unknown inner audio type: " + dummy);

            Audio *base = innerCreator->createAudio(in);
            return new Effect<FadeOut>(base, FadeOut(duration, base->getSampleRate()));
        }

        else {
            throw std::runtime_error("Unsupported effect type: " + effectType);
        }

    } catch (const std::exception &ex) {
        std::cerr << "EffectCreator error: " << ex.what() << '\n';
        return nullptr;
    } catch (...) {
        throw;
    }
}


static EffectCreator __;
