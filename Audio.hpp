/**
 * @file Audio.hpp
 * @brief Defines the Audio and AudioCreator classes for audio processing.
 */

#ifndef DAW_AUDIO_HPP
#define DAW_AUDIO_HPP
#pragma once

using sample = double;

#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

/**
 * @brief Abstract base class for Audio objects.
 *
 * This class provides a common interface for different types of audio data.
 * It defines pure virtual functions for accessing and manipulating audio samples.
 */
class Audio {
protected:
//    std::vector<double> samples;
//    TODO string name
    float sampleRate; ///< The sample rate of the audio in Hz.
    double duration;   ///< The duration of the audio in seconds.
    size_t sampleSize; ///< The number of samples in the audio.
    std::string audioName; ///< The name of the audio.

public:
    /**
     * @brief Default constructor for Audio.
     */
    Audio();

    /**
     * @brief Virtual destructor for Audio.
     */
    virtual ~ Audio() noexcept = default;

    /**
     * @brief Sets the duration of the audio.
     *
     * Ensures valid setting of the duration of the Audio.
     * If the new duration is longer than the current duration, the audio will be filled with silence.
     * @param duration The new duration in seconds.
     */
    void setDuration(double duration);

    /**
     * @brief Sets the sample rate of the audio.
     * @param rate The new sample rate in Hz.
     */
    void setSampleRate(float rate);

    /**
     * @brief Sets the sample size (number of samples) of the audio.
     * @param size The new sample size.
     */
    void setSampleSize(size_t size);

    /**
     * @brief Gets the sample rate of the audio.
     * @return The sample rate in Hz.
     */
    float getSampleRate() const;

    /**
     * @brief Gets the duration of the audio.
     * @return The duration in seconds.
     */
    double getDuration() const;

    /**
     * @brief Gets the sample size (number of samples) of the audio.
     * @return The sample size.
     */
    size_t getSampleSize() const;

    /**
     * @brief Checks if a given sample rate is valid.
     * @param rate The sample rate to check.
     * @return True if the sample rate is valid, false otherwise.
     */
    static bool isValidSampleRate(float rate);

    /**
     * @brief Checks if a given sample size is valid.
     * @param size The sample size to check.
     * @return True if the sample size is valid, false otherwise.
     */
    static bool isValidSampleSize(size_t size);

    /**
     * @brief Checks if a given duration is valid.
     * @param duration The duration to check.
     * @return True if the duration is valid, false otherwise.
     */
    static bool isValidDuration(double duration);

    /**
     * @brief Prints information about the audio to the console.
     */
    virtual void print() const;

    /**
     * @brief Prints information about the audio to an output stream.
     * @param out The output stream.
     * @return A reference to the output stream.
     */
    virtual std::ostream &printToStream(std::ostream &out) const = 0;

    /**
     * @brief Gets the audio sample at the given index (const version).
     * @param index The index of the sample.
     * @return The audio sample at the given index.
     */
    virtual double operator[](std::size_t index) const = 0 ;

    /**
     * @brief Gets a reference to the audio sample at the given index.
     * @param index The index of the sample.
     * @return A reference to the audio sample at the given index.
     */
    virtual double &operator[](std::size_t index) = 0;

    /**
     * @brief Creates a clone of the Audio object.
     * @return A pointer to the cloned Audio object.
     */
    virtual Audio *clone() const = 0;
};

//TODO add operator << for all Audio types

/**
 * @brief Abstract base class for creating Audio objects.
 *
 * This class provides an interface for creating different types of Audio objects
 * based on a command string.
 */
class AudioCreator {
protected:
    /// @brief The command string used to identify the type of audio to create.
    std::string createCommand;

public:
    /**
     * @brief Constructor for AudioCreator.
     * @param command The command string that this creator will handle.
     */
    AudioCreator(const char *command);

    /**
     * @brief Virtual destructor for AudioCreator.
     */
    virtual ~AudioCreator() = default;

    /**
     * @brief Checks if this creator supports creating audio from the given string.
     * @param s The string to check (usually a command or file extension).
     * @return True if this creator can handle the given string, false otherwise.
     */
    bool supportsAudio(const std::string &s) const;

    /**
     * @brief Creates an Audio object from an input stream.
     * @param in The input stream to read audio data from.
     * @return A pointer to the created Audio object.
     */
    virtual Audio *createAudio(std::istream &in) const = 0;

};

#endif //DAW_AUDIO_HPP
