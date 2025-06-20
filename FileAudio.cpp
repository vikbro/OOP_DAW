#include "FileAudio.hpp"
//#include <fstream>     // For std::ifstream, std::ofstream
//#include <string>      // For std::string

//Helped figuring out the WAV format: https://www.youtube.com/watch?v=rHqkeLxAsTc

// Helper function to get file extension
static std::string getFileExtension(const std::string& filePath) {
    size_t dotPos = filePath.rfind('.');
    if (dotPos == std::string::npos || dotPos == filePath.length() - 1) {
        return ""; // No extension or dot is the last character
    }
    std::string extension = filePath.substr(dotPos + 1);
    // Convert to lowercase manually
    for (char &c : extension) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return extension;
}

FileAudio::FileAudio() : Audio(), fileName(nullptr) {
    // Default constructor: Initializes base Audio and sets fileName to nullptr.
    // No file is loaded by default. Samples vector will be empty.
    // Duration, sampleRate, sampleSize will be 0 as per Audio default constructor.
}

//TODO maybe make a factory and creators for different files/
FileAudio::FileAudio(const char *fileNameParam) : Audio(), fileName(nullptr) { // Initialize fileName to nullptr
    if (!fileNameParam) {
        throw std::runtime_error("File name is null.");
    }
    std::string filePathStr = fileNameParam;
    std::string extension = getFileExtension(filePathStr);

    if (extension == "txt") {
        this->readTXT(fileNameParam); // readTXT will set this->fileName
    } else if (extension == "wav") {
        this->readWAV(fileNameParam); // readWAV will set this->fileName
    } else {
        if (extension.empty()) {
            throw std::runtime_error("Filename has no extension: " + filePathStr);
        } else {
            throw std::runtime_error("Unsupported or unknown file extension: " + extension + " for file " + filePathStr);
        }
    }
}

FileAudio::FileAudio(const Audio& existingAudio) : Audio(), fileName(nullptr) {
    // Use setters to initialize base class members as requested
    this->setSampleRate(existingAudio.getSampleRate());
    this->setDuration(existingAudio.getDuration());
    this->setSampleSize(existingAudio.getSampleSize());

    // Resize this object's samples vector
    this->samples.resize(this->getSampleSize()); // Use getter post-setting

    // Copy samples from existingAudio
    for (size_t i = 0; i < this->getSampleSize(); ++i) {
        this->samples[i] = existingAudio[i]; // Uses existingAudio's operator[]
    }
}

void FileAudio::writeTXT(const char *fileName) const {
    std::ofstream file(fileName);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file for writing: " + std::string(fileName));
    }

    try {
        file << this->getDuration() << " " << this->getSampleRate() << " " << this->getSampleSize() << "\n";

        // Write samples
        for (size_t i = 0; i < this->getSampleSize(); ++i) {
            file << this->samples[i] << (i == this->getSampleSize() - 1 ? "" : " ");
        }
        file << "\n";

        file.close();
    } catch (const std::exception& ex) {
        if (file.is_open()) file.close(); // Ensure file is closed on error
        std::cerr << "Error writing TXT file: " << ex.what() << std::endl; // Should be std::cerr
        throw std::runtime_error("Error during TXT file writing: " + std::string(ex.what()));
    } catch (...) {
        if (file.is_open()) file.close(); // Ensure file is closed on unknown error
        std::cerr << "An unknown error occurred while writing TXT file." << std::endl;
        throw std::runtime_error("Unknown error during TXT file writing.");
    }
}

double FileAudio::operator[](size_t index) const {
    if (index >= this->samples.size()) { // Use this->samples for clarity
        throw std::out_of_range("Index out of range in FileAudio::operator[] const");
    }
    return this->samples[index];
}

double &FileAudio::operator[](size_t index) {
    if (index >= this->samples.size()) { // Use this->samples for clarity
        throw std::out_of_range("Index out of range in FileAudio::operator[]");
    }
    return this->samples[index];
}

FileAudio *FileAudio::clone() const {
    return new FileAudio(*this);
}

std::ostream &FileAudio::printToStream(std::ostream &out) const {
    out << this->getDuration() << '\t' << this->getSampleRate() << '\t' << this->getSampleSize() << '\t';
    for (size_t i = 0; i < this->getSampleSize(); ++i) {
        out << this->samples[i] << (i == this->getSampleSize() - 1 ? "" : " ");
    }
    out << std::endl;
    return out;
}

void FileAudio::print() const {
    Audio::print();

}

void FileAudio::readTXT(const char *fileName) {
    std::ifstream file(fileName);
    try {

        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + std::string(fileName));
        }

        // Read the header information
        // Store to temp variables then use setters as per new constructor guidelines
        double tempDuration;
        double tempSampleRate; // sampleRate in Audio class is double
        size_t tempSampleSize;

        file >> tempDuration >> tempSampleRate >> tempSampleSize;

        this->setDuration(tempDuration);
        this->setSampleRate(tempSampleRate);
        this->setSampleSize(tempSampleSize);

        this->fileName = fileName; // Update the fileName member field

        // Read all the samples
        this->samples.resize(this->getSampleSize()); // Use getter
        for (size_t i = 0; i < this->getSampleSize(); ++i) {
            if (!(file >> this->samples[i])) {
                throw std::runtime_error("Failed to read sample data or unexpected end of file");
            }
        }
        file.close();

    } catch (const std::exception &ex) {
        if (file.is_open()) file.close();
        std::cerr << ex.what() << '\n';
        throw;
    } catch (...) {
        if (file.is_open()) file.close();
        std::cerr << "An error occurred" << '\n';
        throw;
    }
}

// Helper function to read little-endian integers (static, so not part of class instance)
static int32_t readLEint(std::istream& stream, int size) {
    int32_t value = 0;
    char byte;
    for (int i = 0; i < size; ++i) {
        stream.get(byte);
        if (stream.fail()) {
            throw std::runtime_error("Failed to read integer from stream or unexpected end of file");
        }
        value |= static_cast<unsigned char>(byte) << (i * 8);
    }
    return value;
}

void FileAudio::readWAV(const char *fileName) {
    std::ifstream file(fileName, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open WAV file: " + std::string(fileName));
    }

    try {
        char chunkID[4];
        // Read RIFF chunk descriptor
        file.read(chunkID, 4);
        if (file.gcount() < 4 || std::string(chunkID, 4) != "RIFF") { // check gcount
            throw std::runtime_error("Invalid WAV file: Missing RIFF chunk");
        }

        readLEint(file, 4); // chunkSize

        file.read(chunkID, 4);
        if (file.gcount() < 4 || std::string(chunkID, 4) != "WAVE") { // check gcount
            throw std::runtime_error("Invalid WAV file: Missing WAVE format");
        }

        // Read fmt sub-chunk
        file.read(chunkID, 4);
        if (file.gcount() < 4 || std::string(chunkID, 4) != "fmt ") { // check gcount
            throw std::runtime_error("Invalid WAV file: Missing fmt sub-chunk");
        }

        int32_t subchunk1Size = readLEint(file, 4);
        /*int16_t audioFormat =*/ readLEint(file, 2); // Not stored in class
        int16_t numChannels = readLEint(file, 2);   // Used locally
        this->setSampleRate(static_cast<double>(readLEint(file, 4))); // Use setter
        /*int32_t byteRate =*/ readLEint(file, 4);         // Not stored
        /*int16_t blockAlign =*/ readLEint(file, 2);       // Not stored
        int16_t bitsPerSample = readLEint(file, 2); // Used locally

        // Skip extra fmt bytes if any
        if (subchunk1Size > 16) {
            file.seekg(subchunk1Size - 16, std::ios::cur);
        }


        // Find data sub-chunk
        bool dataChunkFound = false;
        while(file.good()) { // Loop while stream is in a good state
            file.read(chunkID, 4);
            if (file.gcount() < 4) { // If read failed or not enough bytes
                break;
            }
            if (std::string(chunkID, 4) == "data") {
                dataChunkFound = true;
                break;
            }
            // Skip unknown chunks
            int32_t unknownChunkSize = readLEint(file, 4);
            if (file.fail()) { // Check if readLEint failed
                throw std::runtime_error("Failed to read size of unknown chunk during WAV parse.");
            }
            file.seekg(unknownChunkSize, std::ios::cur);
            if (file.fail()) { // Check if seekg failed
                throw std::runtime_error("Failed to seek past unknown chunk during WAV parse.");
            }
        }

        if (!dataChunkFound) {
            throw std::runtime_error("Invalid WAV file: Missing data sub-chunk or read error before finding it.");
        }

        int32_t subchunk2Size = readLEint(file, 4); // Data size

        size_t numTotalSamples = (subchunk2Size / (bitsPerSample / 8));
        size_t samplesPerChannel = numChannels > 0 ? numTotalSamples / numChannels : 0;

        this->setSampleSize(samplesPerChannel);
        if (this->getSampleRate() > 0) { // Avoid division by zero
            this->setDuration(static_cast<double>(this->getSampleSize()) / this->getSampleRate());
        } else {
            this->setDuration(0.0); // Or handle as an error case
        }
        this->samples.resize(this->getSampleSize());
        this->fileName = fileName; // Update fileName if reading from a new WAV file

        if (bitsPerSample != 16) {
            // For now, only support 16-bit PCM
            throw std::runtime_error("Unsupported WAV format: Only 16-bit PCM is supported.");
        }

        for (size_t i = 0; i < this->sampleSize; ++i) {
            int16_t sampleValue = readLEint(file, 2); // Read 2 bytes for 16-bit sample
            // Normalize to [-1.0, 1.0]
            this->samples[i] = static_cast<double>(sampleValue) / 32768.0; // Max value for int16 is 32767

            if (numChannels > 1) { // If stereo, skip the other channel(s)
                file.seekg((numChannels - 1) * (bitsPerSample / 8), std::ios::cur);
            }
        }

        file.close();

    } catch (const std::exception& ex) {
        file.close();
        std::cerr << "Error reading WAV file: " << ex.what() << std::endl;
        throw;
    } catch (...) {
        file.close();
        std::cerr << "An unknown error occurred while reading WAV file." << std::endl;
        throw;
    }
}

void FileAudio::writeWAV(const char *fileName) const {
    std::ofstream wav(fileName, std::ios::binary);
    if (!wav.is_open()) {
        throw std::runtime_error("Error opening file for writing: " + std::string(fileName));
    }

    try {
        // WAV Header parameters
        const int numChannels = 1; // Mono
        const int bitsPerSample = 16; // 16-bit PCM
        const int audioFormat = 1; // PCM
        const int subchunk1Size = 16; // PCM header size

        int actualSampleRate = static_cast<int>(this->sampleRate);
        size_t actualSampleSize = this->sampleSize; // Number of samples

        // Calculate dynamic header fields
        int byteRate = actualSampleRate * numChannels * (bitsPerSample / 8);
        int blockAlign = numChannels * (bitsPerSample / 8);
        int subchunk2Size = actualSampleSize * numChannels * (bitsPerSample / 8);
        int chunkSize = 36 + subchunk2Size;

        // Write RIFF Header
        wav.write("RIFF", 4);
        writeAsBytes(wav, chunkSize, 4); // Placeholder, will be updated
        wav.write("WAVE", 4);

        // Write fmt sub-chunk
        wav.write("fmt ", 4);
        writeAsBytes(wav, subchunk1Size, 4);
        writeAsBytes(wav, audioFormat, 2);
        writeAsBytes(wav, numChannels, 2);
        writeAsBytes(wav, actualSampleRate, 4);
        writeAsBytes(wav, byteRate, 4);
        writeAsBytes(wav, blockAlign, 2);
        writeAsBytes(wav, bitsPerSample, 2);

        // Write data sub-chunk header
        wav.write("data", 4);
        writeAsBytes(wav, subchunk2Size, 4); // Placeholder, will be updated

        // Write audio samples
        for (size_t i = 0; i < actualSampleSize; ++i) {
            double sampleDouble = this->samples[i]; // samples is FileAudio member
            // Clamp to [-1.0, 1.0] manually
            if (sampleDouble < -1.0) {
                sampleDouble = -1.0;
            } else if (sampleDouble > 1.0) {
                sampleDouble = 1.0;
            }
            // Convert to 16-bit integer
            int16_t sampleInt = static_cast<int16_t>(sampleDouble * 32767.0);
            // Pass int16_t as int to writeAsBytes, it will handle writing 2 bytes.
            writeAsBytes(wav, static_cast<int>(sampleInt), 2);
        }

        // Update chunk sizes
        std::streampos endOfDataPos = wav.tellp();
        if(endOfDataPos == std::streampos(-1)) { // Error check for tellp
            throw std::runtime_error("Failed to get stream position after writing WAV data.");
        }

        int actualDataBytesWritten = static_cast<int>(endOfDataPos) - 44; // 44 bytes is typical for header before data
        if (actualDataBytesWritten < 0) {
            throw std::runtime_error("Calculated negative data size for WAV, header issue likely.");
        }
        wav.seekp(40, std::ios::beg); // Position for Subchunk2Size (data chunk size)
        if(wav.fail()){ throw std::runtime_error("Failed to seek to Subchunk2Size position in WAV.");}
        writeAsBytes(wav, actualDataBytesWritten, 4);

        int actualChunkSize = 36 + actualDataBytesWritten; // RIFF chunk size: 36 + data size
        wav.seekp(4, std::ios::beg); // Position for ChunkSize (overall file size - 8)
        if(wav.fail()){ throw std::runtime_error("Failed to seek to ChunkSize position in WAV.");}
        writeAsBytes(wav, actualChunkSize, 4);

        wav.close();
    } catch (const std::exception& ex) {
        if (wav.is_open()) wav.close();
        std::cerr << "Error writing WAV file: " << ex.what() << std::endl;
        throw std::runtime_error("Error during WAV file writing: " + std::string(ex.what()));
    } catch (...) {
        if (wav.is_open()) wav.close();
        std::cerr << "An unknown error occurred while writing WAV file." << std::endl;
        throw std::runtime_error("Unknown error during WAV file writing.");
    }
}


void FileAudio::writeAsBytes(std::ostream &file, int value, int byteSize) {
    file.write(reinterpret_cast<const char*>(&value), byteSize);
}


FileAudioCreator::FileAudioCreator() : AudioCreator("FILE") {

}

Audio *FileAudioCreator::createAudio(std::istream &in) const {
    try {

        std::string fileName;
        in >> fileName;

        return new FileAudio(fileName.c_str());

    } catch (const std::exception &ex) {
        std::cerr << ex.what() << std::endl;
        throw;
    } catch (...) {
        throw;
    }
}

static FileAudioCreator __;
