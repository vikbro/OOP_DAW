//
// Created by vikso on 22/05/2025.
//

#include "FileAudio.hpp"

FileAudio::FileAudio(const char *fileName) : Audio() {
    std::ifstream file(fileName);
    try{

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

    }catch (const std::exception& ex){
        file.close();
        std::cerr << ex.what() << '\n';
        throw;
    }catch (...){
        file.close();
        std::cerr << "An error occurred"<< '\n';
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
