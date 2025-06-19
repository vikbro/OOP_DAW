//
// Created by vikso on 24/05/2025.
//

#include "Effect.hpp"
#include "AudioFactory.hpp" // For AudioFactory::getInstance()
// "Audio.hpp" is included by "Effect.hpp", so not strictly needed again here
#include <string>           // For std::string
#include <stdexcept>        // For std::runtime_error
#include <istream>          // For std::istream
#include <limits>           // For std::numeric_limits (for consuming line)

EffectCreator::EffectCreator(const char* command) : AudioCreator(command) {
    // The base class AudioCreator(command) constructor handles registration
    // with the AudioFactory. No additional code needed here for registration.
    // If the command string "effect" is not passed, it will use the default from the header.
}

Audio* EffectCreator::createAudio(std::istream& in) const {
    std::string effectType;
    if (!(in >> effectType)) {
        if (in.eof()) { /* stream ended, nothing to do */ }
        else if (in.fail() || in.bad()) { // If it's a formatting error or worse
            in.clear(); // Clear error flags
            in.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // Consume the problematic line
        }
        throw std::runtime_error("EffectCreator: Could not read effect type.");
    }

    Audio* baseAudio = nullptr;

    try {
        if (effectType == "AMPL") {
            double factor;
            // Note: The Amplify struct in Effect.hpp has an issue:
            // Its operator() uses a hardcoded 2.0f instead of the factor.
            // This creator correctly reads and passes the factor, but the
            // Amplify struct itself needs to be fixed for it to work as intended.
            if (!(in >> factor)) {
                in.clear(); in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                throw std::runtime_error("EffectCreator: Missing or invalid amplify factor.");
            }
            baseAudio = AudioFactory::getInstance().createAudio(in);
            if (!baseAudio) throw std::runtime_error("EffectCreator: Base audio creation failed for amplify effect.");
            Amplify op(factor);
            Effect<Amplify>* effect = new Effect<Amplify>(baseAudio, op);
            baseAudio = nullptr; // Ownership transferred
            return effect;
        } else if (effectType == "NORM") {
            double targetAmplitude;
            if (!(in >> targetAmplitude)) {
                in.clear(); in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                throw std::runtime_error("EffectCreator: Missing or invalid normalize target amplitude.");
            }
            baseAudio = AudioFactory::getInstance().createAudio(in);
            if (!baseAudio) throw std::runtime_error("EffectCreator: Base audio creation failed for normalize effect.");
            Normalize op(*baseAudio, targetAmplitude); // Normalize op constructor needs const Audio&
            Effect<Normalize>* effect = new Effect<Normalize>(baseAudio, op);
            baseAudio = nullptr; // Ownership transferred
            return effect;
        } else if (effectType == "FDIN") {
            double durationSeconds, configuredSampleRate;
            if (!(in >> durationSeconds >> configuredSampleRate)) {
                in.clear(); in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                throw std::runtime_error("EffectCreator: Missing or invalid fadeIn parameters (duration, sampleRate).");
            }
            baseAudio = AudioFactory::getInstance().createAudio(in);
            if (!baseAudio) throw std::runtime_error("EffectCreator: Base audio creation failed for fadeIn effect.");
            FadeIn op(durationSeconds, configuredSampleRate);
            Effect<FadeIn>* effect = new Effect<FadeIn>(baseAudio, op);
            baseAudio = nullptr; // Ownership transferred
            return effect;
        } else if (effectType == "FOUT") {
            double durationSeconds, configuredSampleRate;
            if (!(in >> durationSeconds >> configuredSampleRate)) {
                in.clear(); in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                throw std::runtime_error("EffectCreator: Missing or invalid fadeOut parameters (duration, sampleRate).");
            }
            baseAudio = AudioFactory::getInstance().createAudio(in);
            if (!baseAudio) throw std::runtime_error("EffectCreator: Base audio creation failed for fadeOut effect.");
            FadeOut op(durationSeconds, configuredSampleRate);
            Effect<FadeOut>* effect = new Effect<FadeOut>(baseAudio, op);
            baseAudio = nullptr; // Ownership transferred
            return effect;
        }
            // Add more 'else if' blocks here for other effects like LowPass, HighPass, etc.
            // when their operation structs and Effect specializations (if needed) are defined.
        else {
            // Consume the rest of the line for an unknown effect type to avoid parsing errors later.
            in.clear(); // Clear error flags in case effectType was valid but unrecognised, then ignore.
            in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            throw std::runtime_error("EffectCreator: Unknown effect type: " + effectType);
        }
    } catch (...) {
        delete baseAudio; // IMPORTANT: Clean up baseAudio if it was allocated but not successfully passed to an Effect object.
        throw; // Re-throw the exception.
    }
}

// Static instance of EffectCreator to trigger self-registration with AudioFactory.
// The EffectCreator constructor calls the AudioCreator base constructor,
// which in turn calls AudioFactory::getInstance().registerAudio(this).
// The default command "effect" will be used from EffectCreator's constructor.
static EffectCreator globalEffectCreatorInstance;
