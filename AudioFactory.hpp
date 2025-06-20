/**
 * @file AudioFactory.hpp
 * @brief Defines the AudioFactory class for creating Audio objects.
 */

#ifndef DAW_AUDIOFACTORY_HPP
#define DAW_AUDIOFACTORY_HPP

#include "Audio.hpp"

/**
 * @brief A singleton factory class for creating Audio objects.
 *
 * This class manages a collection of AudioCreator objects and uses them
 * to create instances of different Audio types.
 */
class AudioFactory {
private:
    /// @brief A vector to store registered AudioCreator objects.
    std::vector<const AudioCreator *> creators;
    /// @brief The number of registered AudioCreator objects.
    size_t size;

    /**
     * @brief Gets an appropriate AudioCreator for the given string.
     * @param str The string identifier for the desired Audio type (e.g., command or file extension).
     * @return A pointer to the AudioCreator if found, otherwise nullptr.
     */
    const AudioCreator *getCreator(const std::string &str) const;

    /**
     * @brief Private constructor to enforce singleton pattern.
     */
    AudioFactory();

    /**
     * @brief Deleted copy constructor to prevent copying.
     */
    AudioFactory(const AudioFactory &other) = delete;

    /**
     * @brief Deleted assignment operator to prevent assignment.
     */
    AudioFactory &operator=(const AudioFactory &other) = delete;


public:
    /**
     * @brief Gets the singleton instance of the AudioFactory.
     * @return A reference to the AudioFactory instance.
     */
    static AudioFactory &getInstance();

    /**
     * @brief Registers an AudioCreator with the factory.
     * @param creator A pointer to the AudioCreator object to register.
     */
    void registerAudio(const AudioCreator *creator);

    /**
     * @brief Creates an Audio object from an input stream.
     *
     * The factory determines the appropriate AudioCreator based on the input stream
     * (e.g., by reading a command or file header).
     * @param input The input stream to read audio data from.
     * @return A pointer to the created Audio object, or nullptr if creation fails.
     */
    Audio *createAudio(std::istream &input);

};


#endif //DAW_AUDIOFACTORY_HPP
