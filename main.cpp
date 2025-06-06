#include <iostream>
#include "Silence.hpp"
#include "FileAudio.hpp"
#include "AudioFactory.hpp"

int main() {
    try {
        std::ofstream oFile("SONG.txt");
        oFile << "15.5 " << "17 " << "3 " << 1.5 << " " << 2 << " " << 4;
        oFile.close();

        Silence s(10, 5);
        s.printToStream(std::cout);

        Audio *file = AudioFactory::getInstance().createAudio(std::cin);
        file->printToStream(std::cout);

        Audio *s2 = AudioFactory::getInstance().createAudio(std::cin);

        s2->printToStream(std::cout);
        std::cout << "Hello, World!" << std::endl;
        delete s2;
        return 0;

    } catch (const std::exception &ex) {
        std::cerr << ex.what();
    }
}





