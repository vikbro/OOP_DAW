/**
 * @file Silence.hpp
 * @brief Defines the Silence class for representing silent audio segments and its creator.
 */

#ifndef DAW_SILENCE_HPP
#define DAW_SILENCE_HPP

#include "Audio.hpp"

/**
 * @brief Represents a segment of silent audio.
 *
 * This class inherits from `Audio` and provides an implementation
 * where all samples have a value of 0.0.
 */
class Silence : public Audio {
public:
    /**
     * @brief Constructs a Silence object.
     * @param duration The duration of the silence in seconds.
     * @param sampleRate The sample rate of the audio in Hz.
     */
    Silence(double duration, float sampleRate);

    /**
     * @brief Accesses a sample at the given index (const version).
     *
     * For Silence objects, this always returns 0.0, provided the index is valid.
     * @param index The index of the sample.
     * @return Always 0.0 for silence.
     * @throws std::out_of_range if index is out of bounds (behavior inherited from Audio or should be checked).
     */
    double operator[](size_t index) const override;

    /**
     * @brief Accesses a sample at the given index (non-const version).
     * @throws std::logic_error as silence is immutable.
     * @param index The sample index (unused).
     * @return A reference to a sample (never actually returns due to exception).
     */
    double &operator[](size_t index) override;

    /**
     * @brief Clones the Silence object.
     * @return A pointer to a new Silence object with the same duration and sample rate.
     */
    Silence *clone() const override;

    /**
     * @brief Prints the silence data to an output stream.
     *
     * This typically involves printing metadata indicating it's silence,
     * its duration, and sample rate.
     * @param out The output stream.
     * @return A reference to the output stream.
     */
    std::ostream &printToStream(std::ostream &out) const override;

    /**
     * @brief Prints information about the silence object to the standard output.
     */
    void print() const override;
};

/**
 * @brief Creator class for Silence objects.
 *
 * Inherits from `AudioCreator` to allow `Silence` instantiation via the `AudioFactory`.
 */
class SilenceCreator : public AudioCreator { // Explicitly public inheritance
public:
    /**
     * @brief Constructs a SilenceCreator.
     *
     * Typically registers itself with a factory or sets up the command it responds to (e.g., "SILENCE").
     */
    SilenceCreator();

    /**
     * @brief Creates a Silence object from an input stream.
     *
     * This method would read parameters like duration and sample rate from the stream.
     * @param in The input stream (e.g., representing console input or a script).
     * @return A pointer to the created Silence object.
     */
    Audio *createAudio(std::istream &in) const override;
};

#endif //DAW_SILENCE_HPP
