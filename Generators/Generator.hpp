/**
 * @file Generator.hpp
 * @brief Defines various audio generator operations and the GeneratorAudio template class.
 */

#ifndef DAW_GENERATOR_HPP
#define DAW_GENERATOR_HPP

#include "../Audio.hpp"
#include <cmath> // For std::sin
#include <stdexcept> // For std::logic_error

/**
 * @brief Placeholder for a mix generator operation.
 * @note This struct is currently empty and needs implementation.
 */
struct MixGenerator{
    // TODO: Implement MixGenerator functionality.
    // It should likely take two or more audio sources (or generators)
    // and combine their output sample by sample.
};

/**
 * @brief Placeholder for a repeat generator operation.
 * @note This struct is currently empty and needs implementation.
 */
struct RepeatGenerator{
    // TODO: Implement RepeatGenerator functionality.
    // It should likely take an audio segment (or another generator's output for a duration)
    // and repeat it a certain number of times or for a total duration.
};

/**
 * @brief Functor to generate a sine wave.
 *
 * This struct, when called with a sample index, produces the corresponding
 * sample value for a sine wave of a given frequency and sample rate.
 */
struct SineGenerator {
    float frequency = 440.0; ///< Frequency of the sine wave in Hz.
    float rate = 44100.0;    ///< Sample rate in Hz.

    /**
     * @brief Generates the sine wave sample at the given index.
     * @param i The sample index.
     * @return The value of the sine wave sample at index `i`.
     *         The value is in the range [-1.0, 1.0].
     * @note Uses a common approximation for PI.
     */
    sample operator()(std::size_t i) const {
        return std::sin(2 * 3.14159265358979323846 * frequency * static_cast<double>(i) / rate);
    }
};

/**
 * @brief Template class for audio generated by a specific generator operation.
 *
 * This class derives from `Audio` and uses a `Generator` functor (like `SineGenerator`)
 * to produce audio samples on the fly. The audio data is not stored in a buffer
 * but generated when requested via the `operator[]`.
 *
 * @tparam Generator The type of generator functor. This functor must implement
 *         `sample operator()(std::size_t index) const` to produce a sample
 *         at a given index.
 */
template<typename Generator>
class GeneratorAudio : public Audio {
private:
    Generator generator; ///< The generator functor instance.

public:
    /**
     * @brief Constructs a GeneratorAudio object.
     * @param rate The sample rate in Hz.
     * @param dur The duration of the audio in seconds.
     * @param gen The generator functor instance.
     */
    GeneratorAudio(float rate, double dur, const Generator &gen);

    /**
     * @brief Clones the GeneratorAudio object.
     * @return A pointer to a new GeneratorAudio object, which is a copy of this one.
     */
    Audio *clone() const override;

    /**
     * @brief Accesses a generated sample at the given index (const version).
     *
     * If the index is within the defined sample size, it calls the generator functor.
     * Otherwise, it returns 0.0.
     * @param i The sample index.
     * @return The value of the generated sample at index `i`.
     */
    double operator[](std::size_t i) const override;

    /**
     * @brief Accesses a sample (non-const version).
     * @throws std::logic_error as GeneratorAudio is non-modifiable once created.
     * @param i The sample index (unused).
     * @return A reference to a sample (never actually returns due to exception).
     */
    double &operator[](std::size_t i) override;

    /**
     * @brief Prints information about the generated audio to an output stream.
     * @param out The output stream.
     * @return A reference to the output stream.
     */
    std::ostream &printToStream(std::ostream &out) const override;
};

/**
 * @brief Implementation of printToStream for GeneratorAudio.
 * @tparam Generator The type of the generator functor.
 * @param out The output stream.
 * @return A reference to the output stream.
 */
template<typename Generator>
std::ostream &GeneratorAudio<Generator>::printToStream(std::ostream &out) const {
    // Accessing protected members sampleSize and sampleRate from base class Audio
    out << "GeneratorAudio: " << this->getSampleSize() << " samples @ " << this->getSampleRate() << "Hz, Generated by: " << typeid(Generator).name() << "\n";
    return out;
}

/**
 * @brief Implementation of the non-const array access operator for GeneratorAudio.
 * @tparam Generator The type of the generator functor.
 * @param i The sample index (unused).
 * @throws std::logic_error Always, as generated audio is immutable post-creation.
 * @return A reference to a double (never actually returns due to exception).
 */
template<typename Generator>
double &GeneratorAudio<Generator>::operator[](std::size_t /*i*/) { // Marked i as unused
    throw std::logic_error("GeneratorAudio does not support sample modification.");
}

/**
 * @brief Implementation of the const array access operator for GeneratorAudio.
 *
 * Generates the sample value using the provided generator functor if the index
 * is within bounds.
 * @tparam Generator The type of the generator functor.
 * @param i The sample index.
 * @return The generated sample value, or 0.0 if out of bounds.
 */
template<typename Generator>
double GeneratorAudio<Generator>::operator[](std::size_t i) const {
    // Accessing protected member sampleSize from base class Audio
    return (i < this->getSampleSize()) ? generator(i) : 0.0;
}

/**
 * @brief Constructor implementation for GeneratorAudio.
 *
 * Initializes the base Audio properties (sample rate, duration, sample size)
 * and stores the generator functor.
 * @tparam Generator The type of the generator functor.
 * @param rate The sample rate in Hz.
 * @param dur The duration in seconds.
 * @param gen The generator functor instance.
 */
template<typename Generator>
GeneratorAudio<Generator>::GeneratorAudio(float rate, double dur, const Generator &gen) : generator(gen) {
    setSampleRate(rate);
    setDuration(dur);
    // Calculate sampleSize based on rate and duration
    setSampleSize(static_cast<size_t>(rate * dur));
}

/**
 * @brief Clone implementation for GeneratorAudio.
 * @tparam Generator The type of the generator functor.
 * @return A pointer to a new, dynamically allocated copy of this GeneratorAudio object.
 */
template<typename Generator>
Audio *GeneratorAudio<Generator>::clone() const {
    return new GeneratorAudio<Generator>(*this); // Uses copy constructor
}

#endif //DAW_GENERATOR_HPP
