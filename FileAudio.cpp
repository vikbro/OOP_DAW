//
// Created by vikso on 22/05/2025.
//

#include "FileAudio.hpp"
#include <cmath>

//Make a decorator for different file formats
FileAudio::FileAudio(const char *fileName) : Audio() {
    std::ifstream file(fileName);
    try {

        if (!file.is_open()) {
            throw std::runtime_error("Failed to open file: " + std::string(fileName));
        }

//        size_t sampleCount;

        // Read the header information
        file >> duration >> sampleRate >> sampleSize;

        // Read all the samples
        samples.resize(sampleSize);
        for (size_t i = 0; i < sampleSize; ++i) {
            if (!(file >> samples[i])) {
                throw std::runtime_error("Failed to read sample data or unexpected end of file");
            }
        }
        file.close();

    } catch (const std::exception &ex) {
        file.close();
        std::cerr << ex.what() << '\n';
        throw;
    } catch (...) {
        file.close();
        std::cerr << "An error occurred" << '\n';
        throw;
    }
}

double FileAudio::operator[](size_t index) const {
    return samples[index];
}

double &FileAudio::operator[](size_t index) {
    return samples[index];
}

FileAudio *FileAudio::clone() const {
    return new FileAudio(*this);
}

std::ostream &FileAudio::printToStream(std::ostream &out) const {
    out << this->duration << '\t' << this->sampleRate << '\t' << this->sampleSize << '\t';
    for (int i = 0; i < sampleSize; ++i) {
//        TODO maybe fix this implementation
        out << samples[i] << ' ';
    }
    out << std::endl;
    return out;
}

void FileAudio::print() const {
    Audio::print();

}

void FileAudio::writeWAV(const char *fileName) {
    std::ofstream wav(fileName, std::ios::binary);
    if (!wav.is_open())throw std::runtime_error("Error opening file");
    //Should be sound variables
    const int duration = 2;
    const int maxAmplitude = 32760; // because we are using a 16 bit numbers
    const double frequency = 350;

//TODO FIx this constants
    const std::string chunckID = "RIFF";
    const std::string chunckSize = "----";
    const std::string format = "WAVE";
    const std::string subchunk1ID = "fmt ";//4 bytes thats why space

    const int subchunck1Size = 16;
    const int audioFormat = 1; // 2 bytes
    const int numChannels = 2; // 2 bytes
    const int sampleRate = 44100;
    const int byte_rate = sampleRate * numChannels * (subchunck1Size / 8);
    const int blockAllign = numChannels * (subchunck1Size / 8); // 2 bytes
    const int bitsPerSample = 16; // 2 bytes

    const std::string subchunk2ID = "data";//4 bytes thats why space
    const std::string subchunk2Size = "----";//4 bytes thats why space

    wav << chunckID;
    wav << chunckSize;
    wav << format;

    wav << subchunk1ID;
    writeAsBytes(wav,subchunck1Size,4);
    writeAsBytes(wav,audioFormat, 2);
    writeAsBytes(wav,numChannels,2);
    writeAsBytes(wav,sampleRate,4);
    writeAsBytes(wav,byte_rate,4);
    writeAsBytes(wav,blockAllign,2);
    writeAsBytes(wav,bitsPerSample,2);

    wav << subchunk2ID;
    wav << subchunk2Size;

    int startAudio = wav.tellp();

    for (int i = 0; i < sampleRate*duration; ++i) {
//        Respect max amplitude and operate as a wave
        double amplitude = (double) i / sampleRate *maxAmplitude;
        double value = sin((2*3.14*i*frequency)/sampleRate);

//        Tuk triabva da e generator klasa
        double channel1 = amplitude*value/2;
        writeAsBytes(wav,channel1,2);
        writeAsBytes(wav,channel1,2);
    }
    int endAudio = wav.tellp();

    wav.seekp(startAudio-4);

    writeAsBytes(wav, endAudio-startAudio, 4);
    wav.seekp(4,std::ios::beg);
    writeAsBytes(wav,endAudio-8,4);
    wav.close();

}

void FileAudio::writeAsBytes(std::ostream &file, int value, int byteSize) {
    file.write(reinterpret_cast<const char*> (&value),byteSize);
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
