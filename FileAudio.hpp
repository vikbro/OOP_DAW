/**
 * @file FileAudio.hpp
 * @brief Defines the FileAudio class for handling audio data from files and its creator.
 */

#ifndef DAW_FILEAUDIO_HPP
#define DAW_FILEAUDIO_HPP
#include "Audio.hpp"
#include <fstream>

/**
 * @brief Represents an audio object whose data is primarily sourced from or destined for a file.
 *
 * This class extends the base `Audio` class to include a buffer for audio samples
 * and methods for reading from and writing to various file formats (TXT, WAV).
 */
class FileAudio : public Audio {
private:
    std::vector<sample> samples; ///< Buffer storing the audio samples.
    size_t currentSize;          ///< The current number of samples stored in the buffer.
    const char* fileName;        ///< The name of the file associated with this audio object.

    /**
     * @brief Writes an integer value to an output stream as a sequence of bytes.
     * @param file The output stream to write to.
     * @param value The integer value to write.
     * @param byteSize The number of bytes to represent the integer.
     */
    static void writeAsBytes(std::ostream& file, int value, int byteSize);

public:
    /**
     * @brief Default constructor. Initializes an empty FileAudio object.
     */
    FileAudio();

    /**
     * @brief Constructs a FileAudio object and loads audio data from the specified file.
     *
     * The file type is typically inferred from the extension or by attempting to read it.
     * @param fileName The path to the audio file.
     */
    FileAudio(const char* fileName);

    /**
     * @brief Constructs a FileAudio object from an existing Audio object.
     *
     * This effectively converts any Audio object into a FileAudio object,
     * copying its samples and properties.
     * @param existingAudio The Audio object to copy from.
     */
    FileAudio(const Audio& existingAudio);
//    FileAudio(const std::string& fileName); // Commented out constructor, potentially for future use or replaced by char* version.

    /**
     * @brief Accesses a sample at the given index (const version).
     * @param index The index of the sample.
     * @return The value of the sample at the specified index.
     * @throws std::out_of_range if the index is invalid.
     */
    double operator[](size_t index) const override;

    /**
     * @brief Accesses a sample at the given index (non-const version).
     * @param index The index of the sample.
     * @return A reference to the sample at the specified index.
     * @throws std::out_of_range if the index is invalid.
     */
    double &operator[](size_t index) override;

    /**
     * @brief Clones the FileAudio object.
     * @return A pointer to a new FileAudio object, which is a deep copy of this one.
     */
    FileAudio* clone() const override;

    /**
     * @brief Reads audio data from a text file.
     *
     * The text file is expected to contain sample values, typically one per line or space-separated.
     * Metadata like sample rate and duration might need to be set separately or inferred.
     * @param fileName The path to the text file.
     */
    void readTXT(const char* fileName);

    /**
     * @brief Reads audio data from a WAV file.
     *
     * Parses the WAV file header to set sample rate, duration, etc., and then loads the sample data.
     * @param fileName The path to the WAV file.
     */
    void readWAV(const char* fileName);

    /**
     * @brief Writes the audio data to a text file.
     *
     * Samples are typically written one per line or space-separated.
     * @param fileName The path to the text file to create/overwrite.
     */
    void writeTXT(const char* fileName) const;

    /**
     * @brief Writes the audio data to a WAV file.
     *
     * Creates a WAV file with the appropriate header based on the audio properties
     * and writes out the sample data.
     * @param fileName The path to the WAV file to create/overwrite.
     */
    void writeWAV(const char* fileName) const;

    /**
     * @brief Prints the audio data to an output stream.
     *
     * This typically involves printing metadata and then the sample values.
     * @param out The output stream.
     * @return A reference to the output stream.
     */
    std::ostream &printToStream(std::ostream &out) const override;

    /**
     * @brief Prints information about the audio object to the standard output.
     *
     * This may include file name, duration, sample rate, etc.
     */
    void print() const override;
};

/**
 * @brief Creator class for FileAudio objects.
 *
 * Inherits from `AudioCreator` to allow `FileAudio` instantiation via the `AudioFactory`.
 */
class FileAudioCreator : public AudioCreator {
public:
    /**
     * @brief Constructs a FileAudioCreator.
     *
     * Typically registers itself with a factory or sets up the command it responds to.
     * The default command is often related to file-based audio, e.g., "FILE".
     */
    FileAudioCreator();

    /**
     * @brief Creates a FileAudio object from an input stream.
     *
     * This method might read a filename or initial command from the stream
     * to determine how to load the FileAudio object.
     * @param in The input stream (e.g., representing console input or a script).
     * @return A pointer to the created FileAudio object.
     */
    Audio *createAudio(std::istream &in) const override;
};

#endif //DAW_FILEAUDIO_HPP
