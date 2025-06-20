//
// Created by vikso on 22/05/2025.
//

#ifndef DAW_FILEAUDIO_HPP
#define DAW_FILEAUDIO_HPP
#include "Audio.hpp"

class FileAudio : public Audio{
private:
    std::vector<sample> samples;
    size_t currentSize;
    const char* fileName;

private:
    static void writeAsBytes(std::ostream& file, int value, int byteSize);

public:
    FileAudio(); // Default constructor
    FileAudio(const char* fileName) ;
    FileAudio(const Audio& existingAudio);
//    FileAudio(const std::string& fileName);

    double operator[](size_t index) const override;
    double &operator[](size_t index) override;

    FileAudio* clone() const override;

    void readTXT(const char* fileName);
    void readWAV(const char* fileName);

    void writeTXT(const char* fileName) const;
    void writeWAV(const char* fileName) const;

    std::ostream &printToStream(std::ostream &out) const override;
    void print() const override;

};


class FileAudioCreator : AudioCreator {
public:
    FileAudioCreator();

    Audio *createAudio(std::istream &in) const override;
};




#endif //DAW_FILEAUDIO_HPP
