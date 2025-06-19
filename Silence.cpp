//
// Created by vikso on 22/05/2025.
//

#include "Silence.hpp"

Silence::Silence(double duration, float sampleRate) : Audio() {
    this->duration = duration;
    this->sampleRate = sampleRate;
}

double Silence::operator[](size_t index) const {
    return 0;
}

double &Silence::operator[](size_t index) {
    throw std::logic_error("Can not access");
}

Silence *Silence::clone() const {
    return new Silence(*this);
}

std::ostream &Silence::printToStream(std::ostream &out) const {
    out << this->duration << ' ' << this->sampleRate << ' ' << this->sampleSize << ' ';
    for (int i = 0; i < sampleSize; ++i) {
//        TODO maybe fix this implementation
        out << 0 << ' ';
    }
    out << std::endl;
    return out;
}

void Silence::print() const {
    Audio::print();
}

Audio *SilenceCreator::createAudio(std::istream &in) const {
    try {

        double tempDuration;
        float tempSampleRate;
        size_t tempSampleSize;
        in >> tempDuration >> tempSampleRate >> tempSampleSize;
        if (!in.good()) {
            throw std::runtime_error("Unable to read data from input");
        }

        return new Silence(tempDuration, tempSampleRate);

    } catch (const std::exception &ex) {
        std::cerr << ex.what() << '\n';
        throw;
    } catch (...) {
        throw;
    }
}

SilenceCreator::SilenceCreator() : AudioCreator("SLNC") {

}

static SilenceCreator __;
