//#include <iostream>
#include "Silence.hpp"
#include "FileAudio.hpp"
#include "Effect.hpp"
#include "AudioFactory.hpp"

int main() {
    //Make some basic test
    try {
        std::ofstream oFile("PESEN.txt");
        oFile << "30 " << "4 " << "5 " << 10 << " " << 10 << " " << 10 << " " << 10 << " " << 10;
        oFile.close();

//        std::cout << std::filesystem::current_path() << std::endl;
        FileAudio::writeWAV("sin.wav");
        Silence s(10, 5);
        s.printToStream(std::cout);

//        std::string cmd = "FILE SONG.txt";
//        std::stringstream cmdStream(cmd);
//        std::string cmd = "FILE SONG.txt";
        std::stringstream cmdStream("FILE SONG.txt");

        Audio *file = AudioFactory::getInstance().createAudio(cmdStream);

//        file->printToStream(std::cout);

//        Audio *s2 = AudioFactory::getInstance().createAudio(std::cin);

//        s2->printToStream(std::cout);
        std::cout << "Hello, World!" << std::endl;
//        delete s2;


        Amplify amplify{10};
        FadeIn fadeIn{2,2};
        FadeOut fadeOut{2,2};

		Normalize normalize(*file, 1.0);

		file->printToStream(std::cout);

        Effect<FadeIn> ef1(file,fadeIn);
        Effect<FadeOut> ef2(&ef1,fadeOut);
        Effect<Normalize> efN(&ef2,normalize);
        ef1.printToStream(std::cout);
//        std::cout << std::endl;

        ef2.printToStream(std::cout);
//        std::cout << std::endl;
        efN.printToStream(std::cout);


        const auto& constEf1 = ef2;
        for (int i = 0; i < constEf1.getSampleSize(); ++i) {
            std::cout << constEf1[i] << " ";
        }

		delete file;
        return 0;

    } catch (const std::exception &ex) {
        std::cerr << ex.what();
    }
}





