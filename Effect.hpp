/**
 * @file Effect.hpp
 * @brief Defines various audio effect operations and the Effect class template.
 */

#ifndef DAW_EFFECT_HPP
#define DAW_EFFECT_HPP

#include "Audio.hpp"

/**
 * @brief Functor to amplify an audio sample by a given factor.
 */
struct Amplify {
private:
    double factor; ///< The amplification factor.
public:
    /**
     * @brief Constructs an Amplify effect.
     * @param factor The amplification factor.
     */
    Amplify(double factor) : factor(factor) {};

    /**
     * @brief Applies the amplification.
     * @param s The input sample.
     * @return The amplified sample.
     */
    double operator()(double s) const {
        return s * factor;
    }
};

/**
 * @brief Functor to normalize an audio signal to a target amplitude.
 */
struct Normalize {
    double gain = 1.0; ///< The gain to apply to each sample.
    double target;     ///< The target peak amplitude.

    /**
     * @brief Constructs a Normalize effect.
     *
     * Calculates the gain needed to bring the peak amplitude of the audio
     * to the target amplitude.
     * @param a The input Audio object to analyze for normalization.
     * @param targetAmp The target peak amplitude (default is 1.0).
     */
    Normalize(const Audio &a, double targetAmp = 1.0) : target(targetAmp) {
        double maxAmp = 0.0;
        for (std::size_t i = 0; i < a.getSampleSize(); ++i) {
            maxAmp = std::max(maxAmp, std::abs(a[i]));
        }
        gain = (maxAmp > 0.000001) ? (target / maxAmp) : 1.0; // Avoid division by zero or very small numbers
    }

    /**
     * @brief Applies the normalization gain.
     * @param s The input sample.
     * @return The normalized sample.
     */
    double operator()(double s) const {
        return s * gain;
    }
};

/**
 * @brief Functor to apply a fade-in effect.
 */
struct FadeIn {
    double fadeDuration;  ///< Duration of the fade in seconds.
    double sampleRate;    ///< Sample rate of the audio.

    /**
     * @brief Constructs a FadeIn effect.
     * @param duration Duration of the fade in seconds.
     * @param rate Sample rate of the audio.
     */
    FadeIn(double duration, double rate)
            : fadeDuration(duration), sampleRate(rate) {}

    /**
     * @brief Calculates the fade-in multiplier for a given sample.
     * @param i The current sample index.
     * @param totalSamples The total number of samples in the audio (unused in current logic but kept for potential future use).
     * @return The fade-in multiplier (0.0 to 1.0).
     * @note The logic for fade-in depends on the current sample index `i` and the calculated `fadeSamples`.
     *       If `i` is beyond `fadeSamples`, the multiplier is 1.0 (full volume).
     *       Otherwise, it's a linear ramp from 0.0 to 1.0.
     */
    double operator()(std::size_t i, [[maybe_unused]] std::size_t totalSamples) const {
        // TODO: Verify the logic of this operation, especially if totalSamples should be used.
        // Currently, totalSamples is unused. The fade is based on fadeDuration and sampleRate.
        std::size_t fadeSamples = static_cast<std::size_t>(fadeDuration * sampleRate);
        if (fadeSamples == 0) return 1.0; // Avoid division by zero if fadeDuration is too small
        if (i >= fadeSamples) return 1.0;
        return static_cast<double>(i) / fadeSamples;
    }
};

/**
 * @brief Functor to apply a fade-out effect.
 */
struct FadeOut {
    double fadeDuration;  ///< Duration of the fade in seconds.
    double sampleRate;    ///< Sample rate of the audio.

    /**
     * @brief Constructs a FadeOut effect.
     * @param duration Duration of the fade in seconds.
     * @param rate Sample rate of the audio.
     */
    FadeOut(double duration, double rate)
            : fadeDuration(duration), sampleRate(rate) {}

    /**
     * @brief Calculates the fade-out multiplier for a given sample.
     * @param i The current sample index.
     * @param totalSamples The total number of samples in the audio.
     * @return The fade-out multiplier (0.0 to 1.0).
     */
    double operator()(std::size_t i, std::size_t totalSamples) const {
        std::size_t fadeSamples = static_cast<std::size_t>(fadeDuration * sampleRate);
        if (fadeSamples == 0) return 1.0; // No fade if duration is zero
        if (i >= totalSamples) return 0.0; // Should not happen if iterating up to totalSamples - 1
        // Start fading when (totalSamples - i) <= fadeSamples
        if (totalSamples - i <= fadeSamples) {
            return static_cast<double>(totalSamples - i) / fadeSamples;
        }
        return 1.0; // Before fade-out period, full volume
    }
};

/**
 * @brief A template class that applies an effect operation to an Audio object.
 *
 * This class takes an `EffectOperation` (a functor like Amplify, Normalize, FadeIn, FadeOut)
 * and applies it to the samples of the base audio.
 *
 * @tparam EffectOperation The type of effect to apply. It must be a callable
 *         object that takes a double (sample) and returns a double (modified sample),
 *         or for specific effects like FadeIn/FadeOut, it might take (sample_index, total_samples)
 *         and return a multiplier.
 */
template<typename EffectOperation>
class Effect : public Audio {
private:
    /**
     * @brief Pointer to the base Audio object.
     * @note Using a pointer to the base Audio object allows for polymorphism
     *       but requires careful memory management (cloning and deletion).
     *       A reference might be considered if ownership semantics were different,
     *       but cloning is essential here to ensure the Effect owns its base audio data.
     */
    const Audio *base;
    EffectOperation operation; ///< The effect operation functor.

public:
    /**
     * @brief Constructs an Effect object.
     * @param input A pointer to the base Audio object. The Effect class will take ownership by cloning this object.
     * @param op The effect operation to apply.
     */
    Effect(const Audio *input, EffectOperation op);

    /**
     * @brief Copy constructor.
     * @param other The Effect object to copy.
     */
    Effect(const Effect &other);

    /**
     * @brief Assignment operator.
     * @param other The Effect object to assign from.
     * @return A reference to this Effect object.
     * @note This operator performs a deep copy of the base audio data.
     *       Consider exception safety: if `other.base->clone()` throws, `base` is not deleted,
     *       which is acceptable as the object state remains valid.
     */
    Effect &operator=(const Effect &other);

    /**
     * @brief Destructor. Deletes the cloned base Audio object.
     */
    ~Effect() {
        delete base;
    }

    /**
     * @brief Clones the Effect object.
     * @return A pointer to a new Effect object, which is a deep copy of this one.
     */
    Audio *clone() const override;

    /**
     * @brief Accesses a sample with the effect applied (const version).
     *
     * For most effects, this applies `operation(base_sample)`.
     * For effects like FadeIn/FadeOut, specializations are provided.
     * @param i The sample index.
     * @return The value of the sample at index `i` after the effect is applied.
     */
    double operator[](std::size_t i) const override;

    /**
     * @brief Accesses a sample (non-const version).
     * @throws std::logic_error as effects are non-modifiable once created.
     * @param i The sample index.
     * @return A reference to the sample at index `i`.
     */
    double &operator[](std::size_t i) override;

    /**
     * @brief Prints the effect's audio data to an output stream.
     * @param out The output stream.
     * @return A reference to the output stream.
     */
    std::ostream &printToStream(std::ostream &out) const override;

};

/**
 * @brief Implementation of printToStream for the Effect class.
 * @tparam EffectOperation The type of the effect operation.
 * @param out The output stream.
 * @return A reference to the output stream.
 */
template<typename EffectOperation>
std::ostream &Effect<EffectOperation>::printToStream(std::ostream &out) const {
    out << this->getDuration() << '\t' << this->getSampleRate() << '\t' << this->getSampleSize() << '\t';
    for (size_t i = 0; i < this->getSampleSize(); ++i) {
        out << (*this)[i] << ' '; // Use the effect's own operator[] const
    }
    out << std::endl;
    return out;
}

/**
 * @brief Implementation of the non-const array access operator.
 * @tparam EffectOperation The type of the effect operation.
 * @param i The sample index (unused).
 * @throws std::logic_error Always, as effects are designed to be immutable post-creation.
 * @return A reference to a double (never actually returns due to exception).
 */
template<typename EffectOperation>
double &Effect<EffectOperation>::operator[](std::size_t /*i*/) { // Marked i as unused
    throw std::logic_error("Effect does not support sample modification.");
}

/**
 * @brief Default implementation of the const array access operator.
 *
 * This version is used for effects that operate on a single sample value
 * (e.g., Amplify, Normalize).
 * @tparam EffectOperation The type of the effect operation.
 * @param i The sample index.
 * @return The processed sample value.
 */
template<typename EffectOperation>
double Effect<EffectOperation>::operator[](std::size_t i) const {
    return operation((*base)[i]);
}

/**
 * @brief Implementation of the clone method.
 * @tparam EffectOperation The type of the effect operation.
 * @return A pointer to a new, dynamically allocated copy of this Effect.
 */
template<typename EffectOperation>
Audio *Effect<EffectOperation>::clone() const {
    return new Effect<EffectOperation>(*this);
}

/**
 * @brief Implementation of the assignment operator.
 * @tparam EffectOperation The type of the effect operation.
 * @param other The Effect object to assign from.
 * @return A reference to this Effect object.
 * @note Regarding exception safety for `other.base->clone()`: If it throws, `this->base`
 *       is not yet deleted, and the object remains in its original valid state.
 *       The old `base` is only deleted after the new `base` (temp) is successfully cloned.
 */
template<typename EffectOperation>
Effect<EffectOperation> &Effect<EffectOperation>::operator=(const Effect &other) {
    if (this != &other) {
        Audio *temp = other.base->clone(); // Potential throw here
        delete base; // Safe to delete old base now
        base = temp;
        operation = other.operation;
        // Update properties from the new base audio
        setSampleRate(base->getSampleRate());
        setDuration(base->getDuration());
        setSampleSize(base->getSampleSize());
    }
    return *this;
}

/**
 * @brief Implementation of the copy constructor.
 * @tparam EffectOperation The type of the effect operation.
 * @param other The Effect object to copy from.
 */
template<typename EffectOperation>
Effect<EffectOperation>::Effect(const Effect &other) : base(other.base->clone()), operation(other.operation) {
    setSampleRate(base->getSampleRate());
    setDuration(base->getDuration());
    setSampleSize(base->getSampleSize());
}

/**
 * @brief Implementation of the constructor taking a base Audio and an operation.
 * @tparam EffectOperation The type of the effect operation.
 * @param input Pointer to the base Audio object. This object will be cloned.
 * @param op The effect operation functor.
 */
template<typename EffectOperation>
Effect<EffectOperation>::Effect(const Audio *input, EffectOperation op) :base(input->clone()),
                                                                         operation(op) {
    this->setDuration(base->getDuration());
    this->setSampleRate(base->getSampleRate());
    this->setSampleSize(base->getSampleSize());
}

/**
 * @brief Specialization of the const array access operator for FadeIn effects.
 *
 * This operator applies the fade-in logic, which depends on the current sample index
 * and the total number of samples.
 * @param i The current sample index.
 * @return The sample value after applying the fade-in effect.
 * @note This specialization is provided because FadeIn::operator() has a different signature
 *       (takes index and total samples) compared to simpler effects like Amplify.
 */
template<>
inline double Effect<FadeIn>::operator[](std::size_t i) const {
    return (*base)[i] * operation(i, base->getSampleSize());
}

/**
 * @brief Specialization of the const array access operator for FadeOut effects.
 *
 * This operator applies the fade-out logic, which depends on the current sample index
 * and the total number of samples.
 * @param i The current sample index.
 * @return The sample value after applying the fade-out effect.
 * @note This specialization is provided because FadeOut::operator() has a different signature
 *       (takes index and total samples) compared to simpler effects like Amplify.
 */
template<>
inline double Effect<FadeOut>::operator[](std::size_t i) const {
    // Similar to FadeIn, provides current index and total samples to the operation.
    return (*base)[i] * operation(i, base->getSampleSize());
}

/**
 * @brief Creator class for Effect objects.
 *
 * Inherits from AudioCreator to allow Effect creation through the AudioFactory.
 */
class EffectCreator : public AudioCreator {
public:
    /**
     * @brief Constructs an EffectCreator.
     * @param command The command string this creator will respond to (default is "EFCT").
     */
    explicit EffectCreator(const char* command = "EFCT");

    /**
     * @brief Creates an Effect Audio object from an input stream.
     *
     * This method is expected to read effect parameters and the base audio
     * information from the stream to construct an appropriate Effect object.
     * @param in The input stream.
     * @return A pointer to the created Audio (Effect) object.
     * @note The actual implementation of how effects are specified and created from
     *       the stream is defined in the corresponding .cpp file.
     */
    Audio* createAudio(std::istream& in) const override;
};

#endif //DAW_EFFECT_HPP




